/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#ifdef __FreeBSD__
#  include <sys/mount.h>
#else
#  if HAVE_STATFS
#    define  statfs statfs
#    include <sys/statfs.h>
#  elif HAVE_STATVFS
#    define  statfs statvfs
#    include <sys/statvfs.h>
#  endif
#endif
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>
#include <Edb.h>

#ifdef __EMX__
#include <strings.h>
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_lock.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_filetype.h>
#include <efsd_hash.h>
#include <efsd_statcache.h>

static const char unknown_string[] = "document/unknown";

typedef enum efsd_magic_type
{
  EFSD_MAGIC_8              = 0,
  EFSD_MAGIC_16             = 1,
  EFSD_MAGIC_32             = 2,
  EFSD_MAGIC_DATE           = 3,
  EFSD_MAGIC_STRING         = 4
}
EfsdMagicType;

typedef enum efsd_magic_test
{
  EFSD_MAGIC_TEST_EQUAL     =  0,
  EFSD_MAGIC_TEST_NOTEQUAL  =  1,
  EFSD_MAGIC_TEST_SMALLER   =  2,
  EFSD_MAGIC_TEST_LARGER    =  3,
  EFSD_MAGIC_TEST_MASK      =  4,
  EFSD_MAGIC_TEST_NOTMASK   =  5,
  EFSD_MAGIC_TEST_ACCEPT    =  6
}
EfsdMagicTest;

typedef enum efsd_byteorder
{
  EFSD_BYTEORDER_HOST       = 0,
  EFSD_BYTEORDER_BIG        = 1,
  EFSD_BYTEORDER_SMALL      = 2
}
EfsdByteorder;


/* This is the data structure used to define a file magic
   test. All tests are stored in a tree in order to
   be able to represent the hierarchical nature of the
   tests -- all tests below a node are more specialized,
   like ones with more ">"'s in a magic file, check the
   manpage for details.

   The tests are stored by node indices in the db --
   the first test is located by /1/FIELDS, where FIELDS
   is 'offset', 'type', 'value' etc. The other tests on the
   same layer are /2/FIELDS ... /n/FIELDS. The specialized
   tests are located by simply adding another index level,
   1/1/FIELDS, 1/2/FIELDS ... 1/m/FIELDS, etc.
*/

typedef struct efsd_magic
{
  uint16_t            offset;
  EfsdMagicType       type;
  void               *value;
  int                 value_len;
  EfsdByteorder       byteorder;

  char                use_mask;
  int                 mask;
  EfsdMagicTest       test;

  char               *filetype;
  char               *formatter;

  struct efsd_magic  *next;
  struct efsd_magic  *kids;
  struct efsd_magic  *last_kid;
}
EfsdMagic;


typedef struct efsd_filetype_cache_item
{
  char    *filetype; /* Cached filetype */
  time_t   time;     /* Timestamp of last calculation */ 
}
EfsdFiletypeCacheItem;

#ifdef WORDS_BIGENDIAN
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_BIG;
#else
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_SMALL;
#endif

/* These are mostly taken from the stat(1) source code: */

#define	AFFS_SUPER_MAGIC      0xADFF
#define EXT_SUPER_MAGIC       0x137D
#define EXT2_OLD_SUPER_MAGIC  0xEF51
#define EXT2_SUPER_MAGIC      0xEF53
#define HPFS_SUPER_MAGIC      0xF995E849
#define ISOFS_SUPER_MAGIC     0x9660
#define MINIX_SUPER_MAGIC     0x137F
#define MINIX_SUPER_MAGIC2    0x138F
#define MINIX2_SUPER_MAGIC    0x2468
#define MINIX2_SUPER_MAGIC2   0x2478
#define MSDOS_SUPER_MAGIC     0x4d44
#define NCP_SUPER_MAGIC       0x564c
#define NFS_SUPER_MAGIC       0x6969
#define PROC_SUPER_MAGIC      0x9fa0
#define SMB_SUPER_MAGIC       0x517B
#define XENIX_SUPER_MAGIC     0x012FF7B4
#define SYSV4_SUPER_MAGIC     0x012FF7B5
#define SYSV2_SUPER_MAGIC     0x012FF7B6
#define COH_SUPER_MAGIC       0x012FF7B7
#define UFS_MAGIC             0x00011954
#define _XIAFS_SUPER_MAGIC    0x012FD16D
#define	NTFS_SUPER_MAGIC      0x5346544e
#define XFS_SUPER_MAGIC       0x58465342
#define REISERFS_SUPER_MAGIC  0x52654973

/* The root node of the magic checks tree. Its test-related
   entries aren't used, it's only a container for the first
   level's list of EfsdMagics.
*/
static EfsdMagic  magic;

static EfsdLock  *filetype_cache_lock;

/* The db where everything is stored. */
static E_DB_File *magic_db = NULL;

/* Filename patterns */
static char     **patterns = NULL;
static char     **pattern_filetypes = NULL;
static int        num_patterns;

static char     **patterns_user = NULL;
static char     **pattern_filetypes_user = NULL;
static int        num_patterns_user;

static EfsdHash  *filetype_cache;

/* db helper functions */
static int        filetype_edb_int8_t_get(E_DB_File * db, char *key, uint8_t *val);
static int        filetype_edb_int16_t_get(E_DB_File * db, char *key, uint16_t *val);
static int        filetype_edb_int32_t_get(E_DB_File * db, char *key, uint32_t *val);

static EfsdMagic *filetype_magic_new(char *key, char *params);
static void       filetype_magic_free(EfsdMagic *em);
static void       filetype_magic_cleanup_level(EfsdMagic *em);
static void       filetype_magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid);
static char      *filetype_magic_test_level(EfsdMagic *em, FILE *f, char *ptr, char stop_when_found);
static char      *filetype_magic_test_perform(EfsdMagic *em, FILE *f);
static void       filetype_magic_init_level(char *key, char *ptr, EfsdMagic *em_parent);

/* Scans a format string and returns pointer to first conversion
   specifier. Escaped percentage signs are skipped, additional
   conversion specifiers are overwritten with whitespace. */
static char      *filetype_analyze_format_string(char *format);

/* Substitutes the conversion specifier in the filetype entry in EM
   with the data the current file F provides. The resulting string
   is placed in SUBST.
*/
static int        filetype_substitute_value(EfsdMagic *em, FILE *f, char *subst, int substlen);

static int        filetype_init_magic(void);
static int        filetype_init_patterns(void);
static int        filetype_init_patterns_user(void);
static void       filetype_cleanup_magic(void);
static void       filetype_cleanup_patterns(void);
static void       filetype_cleanup_patterns_user(void);

static void       filetype_fix_byteorder(EfsdMagic *em);
static int        filetype_fix_byteorder_short(EfsdMagic *em, uint16_t *val);
static int        filetype_fix_byteorder_long(EfsdMagic *em, uint32_t *val);

static int        filetype_test_fs(char *filename, struct stat *st, char *type, int len);
static int        filetype_test_magic(char *filename, char *type, int len);
static int        filetype_test_pattern(char *filename, char *type, int len);

static void       filetype_cache_init(void);
static void       filetype_cache_update(char *filename, time_t time, const char *filetype);
static EfsdFiletypeCacheItem *filetype_cache_lookup(char *filename);
static void       filetype_hash_item_free(EfsdHashItem *it);



static int
filetype_edb_int8_t_get(E_DB_File * db, char *key, uint8_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (uint8_t)v;
  D_RETURN_(result);
}


static int               
filetype_edb_int16_t_get(E_DB_File * db, char *key, uint16_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (uint16_t)v;
  D_RETURN_(result);
}


static int               
filetype_edb_int32_t_get(E_DB_File * db, char *key, uint32_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (uint32_t)v;
  D_RETURN_(result);
}


static EfsdMagic *
filetype_magic_new(char *key, char *params)
{
  int        dummy;
  EfsdMagic *em;

  D_ENTER;

  em = NEW(EfsdMagic);
  memset(em, 0, sizeof(EfsdMagic));

  sprintf(params, "%s", "/offset");
  e_db_int_get(magic_db, key, &dummy);
  em->offset = (uint16_t)dummy;

  sprintf(params, "%s", "/type");
  e_db_int_get(magic_db, key, (int*)&em->type);

  sprintf(params, "%s", "/byteorder");
  e_db_int_get(magic_db, key, (int*)&em->byteorder);

  sprintf(params, "%s", "/value");
  switch (em->type)
    {
    case EFSD_MAGIC_8:
      em->value = NEW(uint8_t);
      filetype_edb_int8_t_get(magic_db, key, (uint8_t*)em->value);
      break;
    case EFSD_MAGIC_16:
      em->value = NEW(uint16_t);
      filetype_edb_int16_t_get(magic_db, key, (uint16_t*)em->value);
      filetype_fix_byteorder(em);
      break;
    case EFSD_MAGIC_32:
      em->value = NEW(uint32_t);
      filetype_edb_int32_t_get(magic_db, key, (uint32_t*)em->value);
      filetype_fix_byteorder(em);
      break;
    case EFSD_MAGIC_DATE:
      em->value = NEW(uint32_t);
      filetype_edb_int32_t_get(magic_db, key, (uint32_t*)em->value);
      filetype_fix_byteorder(em);
    case EFSD_MAGIC_STRING:
      em->value = (char*)e_db_data_get(magic_db, key, &em->value_len);
      break;
    default:
    }

  sprintf(params, "%s", "/mask");
  if (e_db_int_get(magic_db, key, &em->mask))
    em->use_mask = TRUE;
  else
    em->use_mask = FALSE;

  sprintf(params, "%s", "/test");
  e_db_int_get(magic_db, key, (int*)&em->test);

  sprintf(params, "%s", "/filetype");
  em->filetype = e_db_str_get(magic_db, key);

  em->formatter = filetype_analyze_format_string(em->filetype);

  D_RETURN_(em);
}


static void       
filetype_magic_free(EfsdMagic *em)
{
  D_ENTER;

  if (!em)
    D_RETURN;

  FREE(em->value);
  FREE(em->value);
  FREE(em);

  D_RETURN;
}


static void       
filetype_magic_cleanup_level(EfsdMagic *em)
{
  EfsdMagic *m, *m2;

  D_ENTER;

  if (!em)
    D_RETURN;

  for (m = em->kids; m; m = m->next)
    filetype_magic_cleanup_level(m);

  m = em->kids;

  while (m)
    {
      m2 = m;
      m = m->next;

      filetype_magic_free(m2);
    }

  D_RETURN;
}


static void       
filetype_magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid)
{
  D_ENTER;

  if (!em_dad || !em_kid)
    D_RETURN;

  if (em_dad->kids)
    em_dad->last_kid->next = em_kid;
  else
    em_dad->kids = em_kid;

  em_dad->last_kid = em_kid;

  D_RETURN;
}


static void
filetype_fix_byteorder(EfsdMagic *em)
{
  D_ENTER;

  if ((em->type == EFSD_MAGIC_16) ||
      (em->type == EFSD_MAGIC_32) ||
      (em->type == EFSD_MAGIC_DATE))
    {
      if ((em->byteorder == host_byteorder)  || 
	  (em->byteorder == EFSD_BYTEORDER_HOST))
	{
	  D_RETURN;
	}
      
      switch (em->type)
	{
	case EFSD_MAGIC_16:
	  *((uint16_t*)em->value) = SWAP_SHORT(*((uint16_t*)em->value));
	  break;
	case EFSD_MAGIC_32:
	case EFSD_MAGIC_DATE:
	  *((uint32_t*)em->value) = SWAP_LONG(*((uint32_t*)em->value));
	  break;
	default:
	}
    }
  
  D_RETURN;
}


static int
filetype_fix_byteorder_short(EfsdMagic *em, uint16_t *val)
{
  D_ENTER;

  if ((em->byteorder == host_byteorder)  || 
      (em->byteorder == EFSD_BYTEORDER_HOST))
    D_RETURN_(FALSE);

  *val = SWAP_SHORT(*val);

  D_RETURN_(TRUE);
}


static int
filetype_fix_byteorder_long(EfsdMagic *em, uint32_t *val)
{
  D_ENTER;

  if ((em->byteorder == host_byteorder)  || 
      (em->byteorder == EFSD_BYTEORDER_HOST))
    D_RETURN_(FALSE);

  *val = SWAP_LONG(*val);

  D_RETURN_(TRUE);
}


static char      *
filetype_magic_test_perform(EfsdMagic *em, FILE *f)
{
  D_ENTER;

  if (!em || !f)
    D_RETURN_(NULL);

  if (fseek(f, em->offset, SEEK_SET) < 0)
    D_RETURN_(NULL);

  if (em->test == EFSD_MAGIC_TEST_ACCEPT)
    D_RETURN_(em->filetype);

  switch (em->type)
    {
    case EFSD_MAGIC_8:
      {
	uint8_t val, val_test;

	val_test = *((uint8_t*)em->value);

	if (fread(&val, sizeof(val), 1, f) != 1)
	  D_RETURN_(NULL);
	  
	if (em->use_mask)
	  val &= (uint8_t)em->mask;

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    if (val == val_test)
	      {
		D("Equality test ...succeeded.\n");
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    if (val != val_test)
	      { 
		D("Unequality test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    if (val < val_test)
	      {
		D("Smaller test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    if (val > val_test)
	      {
		D("Larger test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    if ((val & val_test) == val_test)
	      {
		D("Mask test: %x == %x? succeeded.\n",
		   (val & val_test),
		   val_test);
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    if ((val & val_test) == 0)
	      {
		D("Notmask test succeeded.\n");
		D_RETURN_(em->filetype);
	      }
	    break;
	  default:
	    D("UNKNOWN test type!\n");
	  }
      }
      break;
    case EFSD_MAGIC_16:
      {
	uint16_t val, val_test;

	val_test = *((uint16_t*)em->value);

	if (fread(&val, sizeof(val), 1, f) != 1)
	  D_RETURN_(NULL);

	if (em->use_mask)
	  val &= (uint16_t)em->mask;

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    if (val == val_test)
	      {
		D("Equality test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    if (val != val_test)
	      {
		D("Unequality test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    if (val < val_test)
	      {
		D("Smaller test ...succeeded.\n");
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    if (val > val_test)
	      {
		D("Larger test ...succeeded.\n");
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    if ((val & val_test) == val_test)
	      {
		D("Mask test: %x == %x? succeeded.\n",
		   (val & val_test),
		   val_test);
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    if ((val & val_test) == 0)
	      {
		D("Notmask test ...succeeded.\n");
		D_RETURN_(em->filetype); 
	      }
	    break;
	  default:
	    D("UNKNOWN test type!\n");
	  }
      }
      break;
    case EFSD_MAGIC_DATE:
    case EFSD_MAGIC_32:
      {
	uint32_t val, val_test;

	val_test = *((uint32_t*)em->value);

	if (fread(&val, sizeof(val), 1, f) != 1)
	  D_RETURN_(NULL);

	if (em->use_mask)
	  val &= (uint32_t)em->mask;

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    if (val == val_test)
	      {
		D("Long test: %x == %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    if (val != val_test)
	      {
		D("Long test: %x != %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    if (val < val_test)
	      {
		D("Long test: %x < %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    if (val > val_test)
	      {
		D("Long test: %x > %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    if ((val & val_test) == val_test)
	      {
		D("Long test: %x & %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    if ((val & val_test) == 0)
	      {
		D("Long test: %x & %x == 0 succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype);
	      }
	    break;
	  default:
	    D("UNKNOWN test type!\n");
	  }
      }
      break;
    case EFSD_MAGIC_STRING:
      {
	int   i;
	char  s[MAXPATHLEN];

	for (i = 0; i < (em->value_len) && (i < MAXPATHLEN); i++)
	  {
	    if ((s[i] = (char)fgetc(f)) == EOF)
	      D_RETURN_(NULL);
	  }

	/* Fixme: add remaining string tests. */

	if (memcmp(s, em->value, em->value_len) == 0)
	  {
	    D("String test for '%s', len = %i succeeded.\n", (char*)em->value, em->value_len);
	    D_RETURN_(em->filetype);
	  }
      }
      break;
    default:
    }

  D_RETURN_(NULL);
}


static char *
filetype_magic_test_level(EfsdMagic *level, FILE *f, char *ptr, char stop_when_found)
{
  EfsdMagic *em;
  char      *s, *ptr2;
  char      *result = NULL;
  char       subst[MAXPATHLEN];

  D_ENTER;

  for (em = level; em; em = em->next)
    {
      if ((s = filetype_magic_test_perform(em, f)) != NULL)
	{	  
	  if (em->formatter)
	    {
	      filetype_substitute_value(em, f, subst, MAXPATHLEN);
	      sprintf(ptr, "%s", subst);
	      ptr = ptr + strlen(subst);
	    }
	  else
	    {
	      sprintf(ptr, "%s", s);
	      ptr = ptr + strlen(s);
	    }
	  
	  result = ptr;

	  if ((ptr2 = filetype_magic_test_level(em->kids, f, ptr, FALSE)))
	    {
	      result = ptr = ptr2;
	    }

	  if (stop_when_found)
	    {
	      D_RETURN_(result);
	    }
	}
    }

  D_RETURN_(result);
}


static void
filetype_magic_init_level(char *key, char *ptr, EfsdMagic *em_parent)
{
  char        *item_ptr;
  int          i = 0, dummy;

  D_ENTER;

  for (i = 0; 1; i++)
    {
      sprintf(ptr, "/%i", i);
      item_ptr = ptr + strlen(ptr);

      sprintf(item_ptr, "%s", "/offset");

      if (e_db_int_get(magic_db, key, &dummy))
	{
	  EfsdMagic *em;

	  em = filetype_magic_new(key, item_ptr);
	  filetype_magic_add_child(em_parent, em);
	  filetype_magic_init_level(key, item_ptr, em);
	}
      else
	{
	  D_RETURN;
	}
    }

  /* Not reached. */
}


static char *
filetype_analyze_format_string(char *format)
{
  char  conversion_specs[] = "diouxXeEfFgGaAcsCSPn";
  char *strp = format;
  char *subst_loc = NULL;

  D_ENTER;

  for ( ; ; )
    {
      if ((strp = strchr(strp, '%')) == NULL)
	D_RETURN_(subst_loc);
      
      if (*(strp + 1) == '%')
	{
	  /* It's an escaped verbatim percent sign,
	     that's okay. Skip it.
	  */
	  strp += 2;
	}
      else
	{
	  /* We've found a conversion specifier.
	     If we haven't already come across one,
	     save it, otherwise delete it in the
	     string -- the format string MUST only
	     contain one formatter!
	  */
	  if (!subst_loc)
	    {
	      subst_loc = strp;
	      strp++;
	    }
	  else
	    {
	      /* The formatting string contains more than one
		 conversion specifier. It will be only called
		 with one parameter though. Therefore, erase this
		 specifier by pasting whitespace until its
		 last character (as given in CONVERSION_SPECS)
		 is found. */
	      while (strchr(conversion_specs, *strp) == NULL)
		{
		  strp++;
		  *(strp - 1) = ' ';

		  if (*strp == '\0')
		    break;
		}
		
	      /* If the pointer now does not point at \0, it's
		 the end of the conversion specifier. Nuke it.
	      */

	      if (*strp != '\0')
		*strp = ' ';
	    }
	}
    }
  
  D_RETURN_(subst_loc);
}


static int        
filetype_substitute_value(EfsdMagic *em, FILE *f, char *subst, int subst_len)
{
  char  *subst_loc = NULL;
  int    precision = MAXPATHLEN, val;
  void  *data = NULL;
  char   s[MAXPATHLEN]; /* I guess it's sane to never read more than that */
  char  *sptr = s;

  D_ENTER;

  if (!em || !subst || !em->formatter)
    D_RETURN_(FALSE);

  subst_loc = (em->formatter + 1);

  if (*(subst_loc) == '.')
    subst_loc++;

  if (*(subst_loc) >= 48 && *(subst_loc) <= 57)
    {
      precision = 0;

      while (*subst_loc != '\0' && *subst_loc >= 48 && *subst_loc <= 57)
	{
	  precision *= 10;
	  precision += (*subst_loc - 48);
	  subst_loc++;
	}
    }

  if (fseek(f, em->offset, SEEK_SET) < 0)
    D_RETURN_(FALSE);


  switch (em->type)
    {
    case EFSD_MAGIC_8:
      {
	uint8_t val8;
	
	if (fread(&val8, sizeof(val8), 1, f) != 1)
	  D_RETURN_(FALSE);

	val = (int)val8;
	data = (void*)val;
      }
      break;
    case EFSD_MAGIC_16:
      {
	uint16_t val16;
	
	if (fread(&val16, sizeof(val16), 1, f) != 1)
	  D_RETURN_(FALSE);
	filetype_fix_byteorder_short(em, &val16);

	val = (int)val16;
	data = (void*)val;
      }
      break;
    case EFSD_MAGIC_32:
      {
	uint32_t val32;
	
	if (fread(&val32, sizeof(val32), 1, f) != 1)
		D_RETURN_(FALSE);
	filetype_fix_byteorder_long(em, &val32);

	val = (int)val32;
	data = (void*)val;
      }
      break;
    case EFSD_MAGIC_DATE:
      {
	uint32_t  val32;
	
	if (fread(&val32, sizeof(val32), 1, f) != 1)
	  D_RETURN_(FALSE);
	filetype_fix_byteorder_long(em, &val32);

	CTIME((const time_t *)&val32, sptr);
	
	data = sptr;
      }
      break;
    case EFSD_MAGIC_STRING:
      {	
	while (precision > 0)
	  {
	    if ((*sptr = fgetc(f)) == EOF)
	      D_RETURN_(FALSE);
	    
	    if (*sptr == '\0')
	      break;
	    
	    precision--;
	    sptr++;
	  }

	data = s;
      }       	
      break;
    default:
      D("WARNING -- unhandled magic string substitution in %s\n", em->filetype);
    }

  D("Original string: '%s'\n", em->filetype);
  snprintf(subst, subst_len, em->filetype, data);
  D("New string: '%s'\n", subst);
  
  D_RETURN_(TRUE);
}


static int
filetype_test_fs(char *filename, struct stat *st, char *type, int len)
{
  char         *ptr;
  char          broken_link = FALSE;
  struct statfs stfs;
  int           fslen;

  D_ENTER;

  if (!st)
    D_RETURN_(FALSE);


  if (S_ISLNK(st->st_mode))
    {
      if (broken_link)	
	snprintf(type, len, "%s", "broken-link");
      else
	snprintf(type, len, "%s", "link");
    }
  else if (S_ISDIR(st->st_mode))
    {
      snprintf(type, len, "%s", "dir");
    }
  else if (S_ISCHR(st->st_mode))
    {
      snprintf(type, len, "%s", "chardev");
    }
#ifndef __EMX__
  else if (S_ISBLK(st->st_mode))
    {
      snprintf(type, len, "%s", "block");
    }
#endif
  else if (S_ISFIFO(st->st_mode))
    {
      snprintf(type, len, "%s", "fifo");
    }
  else if (S_ISSOCK(st->st_mode))
    {
      snprintf(type, len, "%s", "socket");
    }
  else
    {
      /* If it's not a specific file type,
	 the fs test should fail! */
      D_RETURN_(FALSE);
    }

  fslen = strlen(type);
  ptr = type + fslen;

#ifdef __EMX__
  snprintf(ptr, len - fslen, "/%s", "hpfs");
#elif !defined(HAVE_STATFS) && !defined(HAVE_STATFS)
  snprintf(ptr, len - fslen, "/%s", "unknown-fs");
  D_RETURN_(TRUE);
#else
  if (statfs(filename, &stfs) < 0)
    {
      if (S_ISLNK(st->st_mode))
	{
	  char *lastslash;

	  lastslash = strrchr(filename, '/');

	  if (lastslash)
	    {
	      char old = *(lastslash+1);

	      *(lastslash+1) = '\0';
	      if (statfs(filename, &stfs) < 0)
		{
		  *(lastslash+1) = old;
		  D_RETURN_(FALSE);
		}

	      *(lastslash+1) = old;
	      broken_link = TRUE;
	    }
	}
      else
	{
	  D_RETURN_(FALSE);
	}
    }
    
#  ifdef __FreeBSD__
  if (stfs.f_fstypename < 0)
    snprintf(ptr, len - fslen, "/%s", "unknown-fs");
  else
    snprintf(ptr, len - fslen, "/%s", stfs.f_fstypename);
#  elif HAVE_STATVFS
    snprintf(ptr, len - fslen, "/%s", stfs.f_basetype);
#  else
  switch (stfs.f_type)
    {
    case AFFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "affs");
      break;
    case EXT_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "ext");
    break;
    case EXT2_OLD_SUPER_MAGIC:
    case EXT2_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "ext2");
      break;
    case HPFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "hpfs");
      break;
    case ISOFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "isofs");
      break;
    case MINIX_SUPER_MAGIC:
    case MINIX_SUPER_MAGIC2:
      snprintf(ptr, len - fslen, "/%s", "minix");
      break;
    case MINIX2_SUPER_MAGIC:
    case MINIX2_SUPER_MAGIC2:
      snprintf(ptr, len - fslen, "/%s", "minix-v2");
      break;
    case MSDOS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "msdos");
      break;
    case NCP_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "novell");
      break;
    case NFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "nfs");
      break;
    case PROC_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "proc");
      break;
    case SMB_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "smb");
      break;
    case XENIX_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "xenix");
      break;
    case SYSV4_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "sysv4");
      break;
    case SYSV2_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "sysv2");
      break;
    case COH_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "coh");
      break;
    case UFS_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "ufs");
      break;
    case _XIAFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "xia");
      break;
    case NTFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "ntfs");
      break;
    case XFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "xfs");
      break;
    case REISERFS_SUPER_MAGIC:
      snprintf(ptr, len - fslen, "/%s", "reiserfs");
      break;
    default:
      snprintf(ptr, len - fslen, "/%s", "unknown-fs");
    }
#  endif
#endif

  D_RETURN_(TRUE);
}


static int
filetype_test_magic(char *filename, char *type, int len)
{
  FILE        *f = NULL;
  char        *result = NULL;
  char         s[MAXPATHLEN];

  D_ENTER;

  if ((f = fopen(filename, "r")) == NULL)
    D_RETURN_(FALSE);
  
  result = filetype_magic_test_level(magic.kids, f, s, TRUE);

  fclose(f);

  if (result)
    {
      int last;

      last = strlen(s)-1;

      if (s[last] == '-' || s[last] == '/')
	s[last] = '\0';

      strncpy(type, s, len);

      D_RETURN_(TRUE);
    }

  D_RETURN_(FALSE);
}


static int
filetype_init_magic(void)
{
  char        key[MAXPATHLEN];
  char       *ptr;
  int         i;

  D_ENTER;

  ptr = efsd_filetype_get_magic_db();
  
  if (!ptr)
    {
      D("System magic db not found.\n");
      D_RETURN_(FALSE);
    }
     
  for (i = 0; i < 3; i++)
    {
      if ((magic_db = e_db_open_read(ptr)))
	break;

      sleep(1);
    }

  if (!magic_db)
    { 
      D("Could not open magic db %s!\n", ptr);
      perror("Error");
      D_RETURN_(FALSE);
    }

  memset(&magic, 0, sizeof(EfsdMagic));
  ptr = key;

  filetype_magic_init_level(key, ptr, &magic);
  e_db_close(magic_db);
  e_db_flush();

  D_RETURN_(TRUE);
}


static int
filetype_init_patterns(void)
{
  char      *s;
  E_DB_File *db;
  int        i;
 
  D_ENTER;

  s = efsd_filetype_get_sys_patterns_db();

  if (!s)
    {
      D("System pattern db not found.\n");
      D_RETURN_(0);
    }

  D("System pattern db at %s\n", s);

  for (i = 0; i < 3; i++)
    {
      if ((patterns = e_db_dump_key_list(s, &num_patterns)))
	break;
      
      sleep(1);
    }
  
  if (num_patterns > 0)
    {
      pattern_filetypes = malloc(sizeof(char*) * num_patterns);
      
      D("opening '%s'\n", s);
      db = e_db_open_read(s);
      
      for (i = 0; i < num_patterns; i++)
	pattern_filetypes[i] = e_db_str_get(db, patterns[i]);
      
      e_db_close(db);
      e_db_flush();
    }  

  D("%i keys in system pattern db.\n",
     num_patterns);

  D_RETURN_(1);
}


static int
filetype_init_patterns_user(void)
{
  char      *s;
  E_DB_File *db;
  int        i;
 
  D_ENTER;

  s = efsd_filetype_get_user_patterns_db();

  if (s)
    {
      for (i = 0; i < 3; i++)
	{
	  if ((patterns_user = e_db_dump_key_list(s, &num_patterns_user)))
	    break;

	  sleep(1);
	}
      
      if (num_patterns_user > 0)
	{
	  pattern_filetypes_user = malloc(sizeof(char*) * num_patterns_user);
	  
	  db = e_db_open_read(s);
	  
	  for (i = 0; i < num_patterns_user; i++)
	    pattern_filetypes_user[i] = e_db_str_get(db, patterns_user[i]);
	  
	  e_db_close(db);
	  e_db_flush();
	}  
    }
  else
    {
      num_patterns_user = 0;
      D("User pattern db not found.\n");
    }

  D("%i keys in user pattern db.\n",
     num_patterns_user);

  D_RETURN_(1);
}


static int
filetype_test_pattern(char *filename, char *type, int len)
{
  char *ptr;
  int   i;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  /* Test user-defined patterns first: */

  for (i = 0; i < num_patterns_user; i++)
    {
      ptr = strrchr(filename, '/');
      if (!ptr)
	ptr = filename;
      else
	ptr++;

      if (!fnmatch(patterns_user[i], ptr, FNM_PATHNAME | FNM_PERIOD))
	{
	  strncpy(type, pattern_filetypes_user[i], len);
	  D_RETURN_(TRUE);
	}
    }

  /* If not found, use system-wide definitions. */

  for (i = 0; i < num_patterns; i++)
    {
      ptr = strrchr(filename, '/');
      if (!ptr)
	ptr = filename;
      else
	ptr++;

      if (!fnmatch(patterns[i], ptr, FNM_PATHNAME | FNM_PERIOD))
	{
	  strncpy(type, pattern_filetypes[i], len);
	  D_RETURN_(TRUE);
	}
    }

  D_RETURN_(FALSE);
}


static void       
filetype_cache_init(void)
{
  D_ENTER;

  filetype_cache = efsd_hash_new(1023, 10, (EfsdHashFunc)efsd_hash_string,
				 (EfsdCmpFunc)strcmp,
				 filetype_hash_item_free);

  D_RETURN;
}


static void       
filetype_cache_update(char *filename, time_t time,
		      const char *filetype)
{
  EfsdFiletypeCacheItem *it;

  D_ENTER;

  it = filetype_cache_lookup(filename);

  efsd_lock_get_write_access(filetype_cache_lock);

  if (it)
    {
      if (time > it->time)
	{
	  FREE(it->filetype);
	  it->filetype = strdup(filetype);
	  it->time = time;
	}
    }
  else
    {
      char *key = strdup(filename);

      it = NEW(EfsdFiletypeCacheItem);
      it->filetype = strdup(filetype);
      it->time = time;

      if (!efsd_hash_insert(filetype_cache, key, it))
	{
	  FREE(key);
	  FREE(it->filetype);
	  FREE(it);
	}
    }

  efsd_lock_release_write_access(filetype_cache_lock);

  D_RETURN;
}


static EfsdFiletypeCacheItem *
filetype_cache_lookup(char *filename)
{
  EfsdFiletypeCacheItem *filetype_it = NULL;

  D_ENTER;

  filetype_it = efsd_hash_find(filetype_cache, filename);

  D_RETURN_(filetype_it);
}


static void
filetype_hash_item_free(EfsdHashItem *it)
{
  EfsdFiletypeCacheItem *filetype_it;

  D_ENTER;

  filetype_it = (EfsdFiletypeCacheItem*)it->data;

  FREE(filetype_it->filetype);
  FREE(it->data);
  FREE(it->key);
  FREE(it);

  D_RETURN;
}


static void       
filetype_cleanup_patterns(void)
{
  int i;

  D_ENTER;

  for (i = 0; i < num_patterns; i++)
    {
      FREE(patterns[i]);
      FREE(pattern_filetypes[i]);
    }

  FREE(patterns);
  FREE(pattern_filetypes);

  D_RETURN;
}


static void       
filetype_cleanup_patterns_user(void)
{
  int i;

  D_ENTER;

  for (i = 0; i < num_patterns_user; i++)
    {
      FREE(patterns_user[i]);
      FREE(pattern_filetypes_user[i]);
    }

  FREE(patterns_user);
  FREE(pattern_filetypes_user);

  D_RETURN;
}


static void       
filetype_cleanup_magic(void)
{
  D_ENTER;

  filetype_magic_cleanup_level(&magic);
  magic.kids = NULL;

  D_RETURN;
}


int       
efsd_filetype_init(void)
{
  D_ENTER;

  filetype_cache_init();
  filetype_cache_lock = efsd_lock_new();

  if (!filetype_init_magic())
    D_RETURN_(FALSE);

  if (!filetype_init_patterns())
    D_RETURN_(FALSE);

  if (!filetype_init_patterns_user())
    D_RETURN_(FALSE);

  D_RETURN_(TRUE);
}


void       
efsd_filetype_update_patterns(void)
{
  D_ENTER;

  D("Reloading system patterns db...\n");
  efsd_lock_get_write_access(filetype_cache_lock);
  filetype_cleanup_patterns();
  filetype_init_patterns();
  efsd_lock_release_write_access(filetype_cache_lock);
  D("Done.\n");

  D_RETURN;
}


void       
efsd_filetype_update_patterns_user(void)
{
  D_ENTER;

  D("Reloading user patterns db...\n");
  efsd_lock_get_write_access(filetype_cache_lock);
  filetype_cleanup_patterns_user();
  filetype_init_patterns_user();
  efsd_lock_release_write_access(filetype_cache_lock);
  D("Done.\n");

  D_RETURN;
}


void       
efsd_filetype_update_magic(void)
{
  D_ENTER;

  D("Reloading file magic db.\n");
  efsd_lock_get_write_access(filetype_cache_lock);
  filetype_cleanup_magic();
  filetype_init_magic();
  efsd_hash_free(filetype_cache);
  filetype_cache_init();
  efsd_lock_release_write_access(filetype_cache_lock);
  D("Done.\n");

  D_RETURN;
}


void       
efsd_filetype_cleanup(void)
{
  D_ENTER;

  efsd_lock_get_write_access(filetype_cache_lock);

  filetype_cleanup_magic();
  filetype_cleanup_patterns();
  filetype_cleanup_patterns_user();

  efsd_lock_release_write_access(filetype_cache_lock);

  efsd_lock_free(filetype_cache_lock);
  filetype_cache_lock = NULL;

  D_RETURN;
}


int
efsd_filetype_get(char *filename, char *type, int len)
{
  struct stat     st;
  char            realfile[MAXPATHLEN];
  EfsdFiletypeCacheItem *cached_result = NULL;

  D_ENTER;

  efsd_misc_remove_trailing_slashes(filename);

  /* Okay -- if filetype is in cache, check file
     modification time to see if regeneration of
     filetype is necessary.
  */

  if (!efsd_lstat(filename, &st))
    {
      /* Ouch -- couldn't stat the file. Testing doesn't
	 make much sense now. */
      D_RETURN_(FALSE);
    }

  /* If it's a link, get stat of link target instead */
  if (S_ISLNK(st.st_mode))
    {
      if (realpath(filename, realfile))
	{
	  filename = realfile;

	  if (!efsd_stat(filename, &st))
	    {
	      strncpy(type, unknown_string, len);
	      D_RETURN_(TRUE);
	    }

	  D("Link substitution succeeded.\n");
	}
      else
	{
	  D("Link substitution failed.\n");
	}
    }

  efsd_lock_get_read_access(filetype_cache_lock);
  cached_result = filetype_cache_lookup(filename);

  if (cached_result)
    {
      D("Cached result found for %s\n", filename);
      if (cached_result->time == st.st_mtime)
	{
	  /* File has not been changed -- use cached value. */
	  D("Using cached filetype on %s\n", filename);
	  strncpy(type, cached_result->filetype, len);
	  efsd_lock_release_read_access(filetype_cache_lock);
	  D_RETURN_(TRUE);
	}
    }

  efsd_lock_release_read_access(filetype_cache_lock);
  D("Calculating filetype on %s\n", filename);

  /* Filetype is not in cache or file has been modified, re-test: */

  if(filetype_test_fs(filename, &st, type, len))
    {
      filetype_cache_update(filename, st.st_mtime, type);
      D_RETURN_(TRUE);
    }

  D("magic: fs check failed.\n");

  if (filetype_test_magic(filename, type, len))
    {
      filetype_cache_update(filename, st.st_mtime, type);
      D_RETURN_(TRUE);
    }

  D("magic: data check failed.\n");

  if (filetype_test_pattern(filename, type, len))
    {
      filetype_cache_update(filename, st.st_mtime, type);      
      D_RETURN_(TRUE);
    }

  D("magic: file pattern check failed.\n");

  strncpy(type, unknown_string, len);
  filetype_cache_update(filename, st.st_mtime, unknown_string);

  D_RETURN_(TRUE);
}


char   *
efsd_filetype_get_magic_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/magic.db", efsd_misc_get_sys_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}


char   *
efsd_filetype_get_sys_patterns_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/pattern.db", efsd_misc_get_sys_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}


char   *
efsd_filetype_get_user_patterns_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/pattern.db", efsd_misc_get_user_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}



