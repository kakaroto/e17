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
#include <sys/mount.h>
#else
#include <sys/statfs.h>
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
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_filetype.h>
#include <efsd_hash.h>
#include <efsd_statcache.h>

const char unknown_string[] = "document/unknown";

typedef enum efsd_magic_type
{
  EFSD_MAGIC_8              = 0,
  EFSD_MAGIC_16             = 1,
  EFSD_MAGIC_32             = 2,
  EFSD_MAGIC_STRING         = 3
}
EfsdMagicType;

typedef enum efsd_magic_test
{
  EFSD_MAGIC_TEST_EQUAL     = 0,
  EFSD_MAGIC_TEST_NOTEQUAL  = 1,
  EFSD_MAGIC_TEST_SMALLER   = 2,
  EFSD_MAGIC_TEST_LARGER    = 3,
  EFSD_MAGIC_TEST_MASK      = 4,
  EFSD_MAGIC_TEST_NOTMASK   = 5
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
  u_int16_t           offset;
  EfsdMagicType       type;
  void               *value;
  int                 value_len;
  EfsdByteorder       byteorder;

  char                use_mask;
  int                 mask;
  EfsdMagicTest       test;

  char               *filetype;

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
static int        e_db_int8_t_get(E_DB_File * db, char *key, u_int8_t *val);
static int        e_db_int16_t_get(E_DB_File * db, char *key, u_int16_t *val);
static int        e_db_int32_t_get(E_DB_File * db, char *key, u_int32_t *val);

static EfsdMagic *magic_new(char *key, char *params);
static void       magic_free(EfsdMagic *em);
static void       magic_cleanup_level(EfsdMagic *em);
static void       magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid);
static char      *magic_test_level(EfsdMagic *em, FILE *f, char *ptr, char stop_when_found);
static char      *magic_test_perform(EfsdMagic *em, FILE *f);
static void       magic_init_level(char *key, char *ptr, EfsdMagic *em_parent);
static int        patterns_init(void);
static void       fix_byteorder(EfsdMagic *em);

static char      *magic_test_fs(char *filename, struct stat *st);
static char      *magic_test_data(char *filename);
static char      *magic_test_pattern(char *filename);

static char      *get_magic_db(void);
static char      *get_sys_patterns_db(void);
static char      *get_user_patterns_db(void);

static void       filetype_cache_init(void);
static void       filetype_cache_insert(char *filename, time_t time, char *filetype);
static void       filetype_cache_update(EfsdFiletypeCacheItem *it, time_t time, char *filetype);
static char      *filetype_cache_lookup(char *filename);

static void       filetype_hash_item_free(EfsdHashItem *it);


static char   *
get_magic_db(void)
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


static char   *
get_sys_patterns_db(void)
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


static char   *
get_user_patterns_db(void)
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



static int               
e_db_int8_t_get(E_DB_File * db, char *key, u_int8_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (u_int8_t)v;
  D_RETURN_(result);
}


static int               
e_db_int16_t_get(E_DB_File * db, char *key, u_int16_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (u_int16_t)v;
  D_RETURN_(result);
}


static int               
e_db_int32_t_get(E_DB_File * db, char *key, u_int32_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (u_int32_t)v;
  D_RETURN_(result);
}


static EfsdMagic *
magic_new(char *key, char *params)
{
  int        dummy;
  EfsdMagic *em;

  D_ENTER;

  em = NEW(EfsdMagic);
  memset(em, 0, sizeof(EfsdMagic));

  sprintf(params, "%s", "/offset");
  e_db_int_get(magic_db, key, &dummy);
  em->offset = (u_int16_t)dummy;

  sprintf(params, "%s", "/type");
  e_db_int_get(magic_db, key, (int*)&em->type);

  sprintf(params, "%s", "/byteorder");
  e_db_int_get(magic_db, key, (int*)&em->byteorder);

  sprintf(params, "%s", "/value");
  switch (em->type)
    {
    case EFSD_MAGIC_8:
      em->value = NEW(u_int8_t);
      e_db_int8_t_get(magic_db, key, (u_int8_t*)em->value);
      break;
    case EFSD_MAGIC_16:
      em->value = NEW(u_int16_t);
      e_db_int16_t_get(magic_db, key, (u_int16_t*)em->value);
      fix_byteorder(em);
      break;
    case EFSD_MAGIC_32:
      em->value = NEW(u_int32_t);
      e_db_int32_t_get(magic_db, key, (u_int32_t*)em->value);
      fix_byteorder(em);
      break;
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

  D_RETURN_(em);
}


static void       
magic_free(EfsdMagic *em)
{
  D_ENTER;

  if (!em)
    { D_RETURN; }

  if (em->value)
    FREE(em->value);
  if (em->filetype)
    FREE(em->value);

  FREE(em);

  D_RETURN;
}


static void       
magic_cleanup_level(EfsdMagic *em)
{
  EfsdMagic *m, *m2;

  D_ENTER;

  if (!em)
    { D_RETURN; }

  for (m = em->kids; m; m = m->next)
    magic_cleanup_level(m);

  m = em->kids;

  while (m)
    {
      m2 = m;
      m = m->next;

      magic_free(m2);
    }

  D_RETURN;
}


static void       
magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid)
{
  D_ENTER;

  if (!em_dad || !em_kid)
    { D_RETURN; }

  if (em_dad->kids)
    {
      em_dad->last_kid->next = em_kid;
      em_dad->last_kid = em_kid;
    }
  else
    {
      em_dad->kids = em_kid;
      em_dad->last_kid = em_kid;
    }

  D_RETURN;
}


static void
fix_byteorder(EfsdMagic *em)
{
  int    size = 0;
  int    i;
  char   tmp[4];
  char  *data;

  D_ENTER;

  if ((em->type == EFSD_MAGIC_8) ||
      (em->type == EFSD_MAGIC_16) ||
      (em->type == EFSD_MAGIC_32))
    {
      if ((em->byteorder == host_byteorder)  || 
	  (em->byteorder == EFSD_BYTEORDER_HOST))
	{
	  /* D(("Not changing byteorder.\n")); */
	  D_RETURN;
	}

      switch (em->type)
	{
	case EFSD_MAGIC_8:
	  size = sizeof(u_int8_t);
	  break;
	case EFSD_MAGIC_16:
	  size = sizeof(u_int16_t);
	  break;
	case EFSD_MAGIC_32:
	  size = sizeof(u_int32_t);
	  break;
	default:
	}

      data = (char*)em->value;

      for (i = 0; i < size; i++)
	tmp[i] = data[size-1-i];
      
      memcpy(data, tmp, size);
    }
  else
    {
      /* D(("Not changing byteorder.\n")); */
    }
  
  D_RETURN;
}


static char      *
magic_test_perform(EfsdMagic *em, FILE *f)
{
  D_ENTER;

  if (!em || !f)
    { D_RETURN_(NULL); }

  fseek(f, em->offset, SEEK_SET);

  D(("Offset %i, testing '%s' file.\n", em->offset, em->filetype));

  switch (em->type)
    {
    case EFSD_MAGIC_8:
      {
	u_int8_t val, val_test;

	D(("Performing byte test.\n"));
	
	val_test = *((u_int8_t*)em->value);

	fread(&val, sizeof(val), 1, f);
	if (em->use_mask)
	  {
	    D(("Using mask: %x\n", (u_int16_t)em->mask));
	    val &= (u_int8_t)em->mask;
	  }

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    D(("Equality test\n"));
	    if (val == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    D(("Unequality test\n"));
	    if (val != val_test)
	      { 
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    D(("Smaller test\n"));
	    if (val < val_test)
	      {
 		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    D(("Larger test\n"));
	    if (val > val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    D(("Mask test: %x == %x?\n",
	       (val & val_test),
	       val_test));
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    D(("Notmask test\n"));
	    if ((val & val_test) == 0)
	      { D_RETURN_(em->filetype); }
	    break;
	  default:
	    D(("UNKNOWN test type!\n"));
	  }
      }
      break;
    case EFSD_MAGIC_16:
      {
	u_int16_t val, val_test;

	D(("Performing short test.\n"));

	val_test = *((u_int16_t*)em->value);

	fread(&val, sizeof(val), 1, f);
	if (em->use_mask)
	  {
	    D(("Using mask: %x\n", (u_int16_t)em->mask));
	    val &= (u_int16_t)em->mask;
	  }

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    D(("Equality test\n"));
	    if (val == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    D(("Unequality test\n"));
	    if (val != val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    D(("Smaller test\n"));
	    if (val < val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    D(("Larger test\n"));
	    if (val > val_test)
	      {
 		D(("...succeeded.\n"));
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    D(("Mask test: %x == %x?\n",
	       (val & val_test),
	       val_test));
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    D(("Notmask test\n"));
	    if ((val & val_test) == 0)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  default:
	    D(("UNKNOWN test type!\n"));
	  }
      }
      break;
    case EFSD_MAGIC_32:
      {
	u_int32_t val, val_test;

	val_test = *((u_int32_t*)em->value);

	fread(&val, sizeof(val), 1, f);
	if (em->use_mask)
	  val &= (u_int32_t)em->mask;

	D(("Performing long test: %x == %x\n", val, *((u_int32_t*)em->value)));

	switch (em->test)
	  {
	  case EFSD_MAGIC_TEST_EQUAL:
	    if (val == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    if (val != val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    if (val < val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    if (val > val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    if ((val & val_test) == 0)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->filetype);
	      }
	    break;
	  default:
	    D(("UNKNOWN test type!\n"));
	  }
      }
      break;
    case EFSD_MAGIC_STRING:
      {
	int   i;
	char  s[MAXPATHLEN];

	for (i = 0; i < em->value_len; i++)
	  s[i] = fgetc(f);

	D(("Performing string test for '%s', len = %i\n", (char*)em->value, em->value_len));

	/* Fixme: add remaining string tests. */

	if (memcmp(s, em->value, em->value_len) == 0)
	  {
	    D(("...succeeded.\n"));
	    D_RETURN_(em->filetype);
	  }
      }
      break;
    default:
    }

  D(("...failed.\n"));

  D_RETURN_(NULL);
}


static char *
magic_test_level(EfsdMagic *level, FILE *f, char *ptr, char stop_when_found)
{
  EfsdMagic *em;
  char      *s, *ptr2;
  char      *result = NULL;

  D_ENTER;

  for (em = level; em; em = em->next)
    {
      if ((s = magic_test_perform(em, f)) != NULL)
	{
	  sprintf(ptr, "%s", s);
	  ptr = ptr + strlen(s);
	  result = ptr;

	  if ((ptr2 = magic_test_level(em->kids, f, ptr, FALSE)))
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
magic_init_level(char *key, char *ptr, EfsdMagic *em_parent)
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

	  em = magic_new(key, item_ptr);
	  magic_add_child(em_parent, em);
	  magic_init_level(key, item_ptr, em);
	}
      else
	{
	  D_RETURN;
	}
    }
}


static char      *
magic_test_fs(char *filename, struct stat *st)
{
  static char   s[MAXPATHLEN];
  char         *ptr;
  char          broken_link = FALSE;
  struct statfs stfs;

  D_ENTER;

  if (!st)
    D_RETURN_(NULL);

#ifdef __EMX__
   sprintf(s, "%s", "hpfs");
#else
  if (statfs(filename, &stfs) < 0)
    {
      if (S_ISLNK(st->st_mode))
	{
	  char *lastslash = strrchr(filename, '/');

	  if (lastslash)
	    {
	      *lastslash = '\0';
	      if (statfs(filename, &stfs) < 0)
		{
		  *lastslash = '/';
		  D_RETURN_(NULL);
		}

	      *lastslash = '/';
	      broken_link = TRUE;
	    }
	}
      else
	{
	  D_RETURN_(NULL);
	}
    }
#ifdef __FreeBSD__
  if (stfs.f_fstypename < 0)
    sprintf(s, "%s", "unknown-fs");
  else
    sprintf(s, "%s", stfs.f_fstypename);
#else
  switch (stfs.f_type)
    {
    case AFFS_SUPER_MAGIC:
      sprintf(s, "%s", "affs");
      break;
    case EXT_SUPER_MAGIC:
      sprintf(s, "%s", "ext");
    break;
    case EXT2_OLD_SUPER_MAGIC:
    case EXT2_SUPER_MAGIC:
      sprintf(s, "%s", "ext2");
      break;
    case HPFS_SUPER_MAGIC:
      sprintf(s, "%s", "hpfs");
      break;
    case ISOFS_SUPER_MAGIC:
      sprintf(s, "%s", "isofs");
      break;
    case MINIX_SUPER_MAGIC:
    case MINIX_SUPER_MAGIC2:
      sprintf(s, "%s", "minix");
      break;
    case MINIX2_SUPER_MAGIC:
    case MINIX2_SUPER_MAGIC2:
      sprintf(s, "%s", "minix-v2");
      break;
    case MSDOS_SUPER_MAGIC:
      sprintf(s, "%s", "msdos");
      break;
    case NCP_SUPER_MAGIC:
      sprintf(s, "%s", "novell");
      break;
    case NFS_SUPER_MAGIC:
      sprintf(s, "%s", "nfs");
      break;
    case PROC_SUPER_MAGIC:
      sprintf(s, "%s", "proc");
      break;
    case SMB_SUPER_MAGIC:
      sprintf(s, "%s", "smb");
      break;
    case XENIX_SUPER_MAGIC:
      sprintf(s, "%s", "xenix");
      break;
    case SYSV4_SUPER_MAGIC:
      sprintf(s, "%s", "sysv4");
      break;
    case SYSV2_SUPER_MAGIC:
      sprintf(s, "%s", "sysv2");
      break;
    case COH_SUPER_MAGIC:
      sprintf(s, "%s", "coh");
      break;
    case UFS_MAGIC:
      sprintf(s, "%s", "ufs");
      break;
    case _XIAFS_SUPER_MAGIC:
      sprintf(s, "%s", "xia");
      break;
    case NTFS_SUPER_MAGIC:
      sprintf(s, "%s", "ntfs");
      break;
    case XFS_SUPER_MAGIC:
      sprintf(s, "%s", "xfs");
      break;
    case REISERFS_SUPER_MAGIC:
      sprintf(s, "%s", "reiserfs");
      break;
    default:
      sprintf(s, "%s", "unknown-fs");
    }
#endif
#endif

  ptr = s + strlen(s);
    
  if (S_ISLNK(st->st_mode))
    {
      if (broken_link)	
	sprintf(ptr, "%s", "/link/broken");
      else
	sprintf(ptr, "%s", "/link");
    }
  else if (S_ISDIR(st->st_mode))
    {
      sprintf(ptr, "%s", "/dir");
    }
  else if (S_ISCHR(st->st_mode))
    {
      sprintf(ptr, "%s", "/chardev");
    }
#ifndef __EMX__
  else if (S_ISBLK(st->st_mode))
    {
      sprintf(ptr, "%s", "/block");
    }
#endif
  else if (S_ISFIFO(st->st_mode))
    {
      sprintf(ptr, "%s", "/fifo");
    }
  else if (S_ISSOCK(st->st_mode))
    {
      sprintf(ptr, "%s", "/socket");
    }
  else
    {
      /* If it's not a specific file type,
	 the fs test should fail! */
      D_RETURN_(NULL);
    }

  D_RETURN_(s);
}


static char      *
magic_test_data(char *filename)
{
  FILE        *f = NULL;
  char        *result = NULL;
  static char  s[MAXPATHLEN];

  D_ENTER;

  if ((f = fopen(filename, "r")) == NULL)
    { D_RETURN_(NULL); }
  
  result = magic_test_level(magic.kids, f, s, TRUE);

  fclose(f);

  if (result)
    {
      int last;

      last = strlen(s)-1;

      if (s[last] == '-' || s[last] == '/')
	s[last] = '\0';

      D_RETURN_(s);
    }

  D_RETURN_(NULL);
}


static int
patterns_init(void)
{
  char      *s;
  E_DB_File *db;
  int        i;
 
  D_ENTER;

  s = get_user_patterns_db();

  if (s)
    {
      patterns_user = e_db_dump_key_list(s, &num_patterns_user);
      
      if (num_patterns_user > 0)
	{
	  pattern_filetypes_user = malloc(sizeof(char*) * num_patterns_user);
	  
	  db = e_db_open_read(s);
	  
	  for (i = 0; i < num_patterns_user; i++)
	    pattern_filetypes_user[i] = e_db_str_get(db, patterns_user[i]);
	  
	  e_db_close(db);
	}  
    }
  else
    {
      num_patterns_user = 0;
      D(("User pattern db not found.\n"));
    }

  s = get_sys_patterns_db();

  if (!s)
    {
      D(("System pattern db not found.\n"));
      D_RETURN_(0);
    }

  D(("System pattern db at %s\n", s));

  if (s)
    {
      patterns = e_db_dump_key_list(s, &num_patterns);

      if (num_patterns > 0)
	{
	  pattern_filetypes = malloc(sizeof(char*) * num_patterns);
	  
	  D(("opening '%s'\n", s));
	  db = e_db_open_read(s);
      
	  for (i = 0; i < num_patterns; i++)
	    pattern_filetypes[i] = e_db_str_get(db, patterns[i]);
      
	  e_db_close(db);
	}  
    }

  D(("%i keys in user pattern db, %i keys in system pattern db.\n",
     num_patterns_user, num_patterns));

  D_RETURN_(1);
}


static char      *
magic_test_pattern(char *filename)
{
  char *ptr;
  int   i;

  D_ENTER;

  if (!filename)
    D_RETURN_(NULL);

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
	  D_RETURN_(pattern_filetypes_user[i]);
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
	  D_RETURN_(pattern_filetypes[i]);
	}
    }

  D_RETURN_(NULL);
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
filetype_cache_insert(char *filename, time_t time, char *filetype)
{
  EfsdFiletypeCacheItem *it;
  char *key;

  D_ENTER;

  key  = strdup(filename);

  it = NEW(EfsdFiletypeCacheItem);
  it->filetype = strdup(filetype);
  it->time = time;

  if (!efsd_hash_insert(filetype_cache, key, it))
    {
      FREE(key);
      FREE(it->filetype);
      FREE(it);
    }
  
  D_RETURN;
}


static void       
filetype_cache_update(EfsdFiletypeCacheItem *it, time_t time,
		      char *filetype)
{
  D_ENTER;

  FREE(it->filetype);
  it->filetype = strdup(filetype);
  it->time = time;

  D_RETURN;
}

static char      *
filetype_cache_lookup(char *filename)
{
  D_ENTER;
  D_RETURN_((char*)efsd_hash_find(filetype_cache, filename));
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


int       
efsd_filetype_init(void)
{
  char        key[MAXPATHLEN];
  char       *ptr;

  D_ENTER;

  ptr = get_magic_db();
  
  if (!ptr)
    {
      D(("System magic db not found.\n"));
      D_RETURN_(0);
    }
     
  magic_db = e_db_open_read(ptr);

  if (!magic_db)
    { 
      D(("Could not open magic db!\n"));
      D_RETURN_(0);
    }

  memset(&magic, 0, sizeof(EfsdMagic));
  ptr = key;

  magic_init_level(key, ptr, &magic);
  e_db_close(magic_db);

  filetype_cache_init();

  D_RETURN_(patterns_init());
}


void       
efsd_filetype_cleanup(void)
{
  int i;

  D_ENTER;

  magic_cleanup_level(&magic);
  magic.kids = NULL;

  for (i = 0; i < num_patterns; i++)
    {
      FREE(patterns[i]);
      FREE(pattern_filetypes[i]);
    }

  FREE(patterns);
  FREE(pattern_filetypes);

  D_RETURN;
}


char *
efsd_filetype_get(char *filename)
{
  struct stat    *st;
  char *result = NULL;
  char  realfile[MAXPATHLEN];
  EfsdFiletypeCacheItem *cached_result = NULL;

  D_ENTER;

  /* Okay -- if filetype is in cache, check file
     modification time to see if regeneration of
     filetype is necessary.
  */

  st = efsd_lstat(filename);

  if (!st)
    {
      /* Ouch -- couldn't stat the file. Testing doesn't
	 make much sense now. */
      D_RETURN_(NULL);
    }

  /* If it's a link, get stat of link target instead */
  if (S_ISLNK(st->st_mode))
    {
      if (realpath(filename, realfile))
	{
	  filename = realfile;
	  st = efsd_stat(filename);

	  if (!st)
	    D_RETURN_((char*)unknown_string);

	  D(("Link substitution succeeded.\n"));
	}
      else
	{
	  D(("Link substitution failed.\n"));
	}
    }

  if ((cached_result = (EfsdFiletypeCacheItem *)
       filetype_cache_lookup(filename)) != NULL)
    {
      D(("Cached result found for %s\n", filename));
      /* If stat failed, regenerate anyway */
      if (st)
	{
	  if (cached_result->time == st->st_mtime)
	    {
	      /* File has not been changed -- use cached value. */
	      D(("Using cached filetype on %s\n", filename));
	      D_RETURN_(cached_result->filetype);	      
	    }
	}
    }

  D(("Calculating filetype on %s\n", filename));

  /* Filetype is not in cache or file has been modified, re-test: */

  result = magic_test_fs(filename, st);
  if (result)
    {
      if (cached_result)
	filetype_cache_update(cached_result, st->st_mtime, result);
      else
	filetype_cache_insert(filename, st->st_mtime, result);

      D_RETURN_(result);
    }

  D(("magic: fs check failed.\n"));

  result = magic_test_data(filename);
  if (result)
    {
      if (cached_result)
	filetype_cache_update(cached_result, st->st_mtime, result);
      else
	filetype_cache_insert(filename, st->st_mtime, result);
      D_RETURN_(result);
    }

  D(("magic: data check failed.\n"));

  result = magic_test_pattern(filename);
  if (result)
    {
      if (cached_result)
	filetype_cache_update(cached_result, st->st_mtime, result);
      else
	filetype_cache_insert(filename, st->st_mtime, result);
      D_RETURN_(result);
    }

  D(("magic: file pattern check failed.\n"));
  
  result = (char*)unknown_string;

  if (cached_result)
    filetype_cache_update(cached_result, st->st_mtime, result);
  else
    filetype_cache_insert(filename, st->st_mtime, result);
  D_RETURN_(result);

  D_RETURN_(result);
}


