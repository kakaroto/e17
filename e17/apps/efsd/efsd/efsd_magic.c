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
#include <sys/statfs.h>
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
#include <efsd_macros.h>
#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_common.h>
#include <efsd_magic.h>


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
  EfsdByteorder       byteorder;

  char                use_mask;
  int                 mask;
  EfsdMagicTest       test;

  char               *mimetype;

  struct efsd_magic  *next;
  struct efsd_magic  *kids;
  struct efsd_magic  *last_kid;
}
EfsdMagic;


#ifdef WORDS_BIGENDIAN
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_BIG;
#else
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_SMALL;
#endif

/* These are taken from the stat(1) source code: */

#define	AFFS_SUPER_MAGIC      0xADFF
#define EXT_SUPER_MAGIC       0x137D
#define EXT2_OLD_SUPER_MAGIC  0xEF51
#define EXT2_SUPER_MAGIC      0xEF53
#define HPFS_SUPER_MAGIC      0xF995E849
#define ISOFS_SUPER_MAGIC     0x9660
#define MINIX_SUPER_MAGIC     0x137F /* orig. minix */
#define MINIX_SUPER_MAGIC2    0x138F /* 30 char minix */
#define MINIX2_SUPER_MAGIC    0x2468 /* minix V2 */
#define MINIX2_SUPER_MAGIC2   0x2478 /* minix V2, 30 char names */
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


/* The root node of the magic checks tree. Its test-related
   entries aren't used, it's only a container for the first
   level's list of EfsdMagics.
*/
static EfsdMagic  magic;

/* The db where everything is stored. */
static E_DB_File *magic_db = NULL;

/* Filename patterns */
static char     **patterns = NULL;
static char     **pattern_mimetypes = NULL;
static int        num_patterns;

static char     **patterns_user = NULL;
static char     **pattern_mimetypes_user = NULL;
static int        num_patterns_user;

/* db helper functions */
int               e_db_int8_t_get(E_DB_File * db, char *key, u_int8_t *val);
int               e_db_int16_t_get(E_DB_File * db, char *key, u_int16_t *val);
int               e_db_int32_t_get(E_DB_File * db, char *key, u_int32_t *val);

static EfsdMagic *magic_new(char *key, char *params);
static void       magic_free(EfsdMagic *em);
static void       magic_cleanup_level(EfsdMagic *em);
static void       magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid);
static char      *magic_test_level(EfsdMagic *em, FILE *f, char *ptr, char stop_when_found);
static char      *magic_test_perform(EfsdMagic *em, FILE *f);
static void       magic_init_level(char *key, char *ptr, EfsdMagic *em_parent);
static int        patterns_init(void);
static void       fix_byteorder(EfsdMagic *em);

static char      *magic_test_fs(char *filename);
static char      *magic_test_data(char *filename);
static char      *magic_test_pattern(char *filename);

char             *get_magic_db(void);
char             *get_sys_patterns_db(void);
char             *get_user_patterns_db(void);


char   *
get_magic_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/magic.db", efsd_common_get_sys_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}


char   *
get_sys_patterns_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/pattern.db", efsd_common_get_sys_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}


char   *
get_user_patterns_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/pattern.db", efsd_common_get_user_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}



int               
e_db_int8_t_get(E_DB_File * db, char *key, u_int8_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (u_int8_t)v;
  D_RETURN_(result);
}


int               
e_db_int16_t_get(E_DB_File * db, char *key, u_int16_t *val)
{
  int result;
  int v;
  
  D_ENTER;
  
  result = e_db_int_get(db, key, &v);
  *val = (u_int16_t)v;
  D_RETURN_(result);
}


int               
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
  bzero(em, sizeof(EfsdMagic));

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
      em->value = (char*)e_db_str_get(magic_db, key);
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

  sprintf(params, "%s", "/mimetype");
  em->mimetype = e_db_str_get(magic_db, key);

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
  if (em->mimetype)
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

  D(("Offset %i, testing '%s' mime.\n", em->offset, em->mimetype));

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
		D_RETURN_(em->mimetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    D(("Unequality test\n"));
	    if (val != val_test)
	      { 
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    D(("Smaller test\n"));
	    if (val < val_test)
	      {
 		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    D(("Larger test\n"));
	    if (val > val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    D(("Mask test: %x == %x?\n",
	       (val & val_test),
	       val_test));
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    D(("Notmask test\n"));
	    if ((val & val_test) == 0)
	      { D_RETURN_(em->mimetype); }
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
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    D(("Unequality test\n"));
	    if (val != val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    D(("Smaller test\n"));
	    if (val < val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    D(("Larger test\n"));
	    if (val > val_test)
	      {
 		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    D(("Mask test: %x == %x?\n",
	       (val & val_test),
	       val_test));
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    D(("Notmask test\n"));
	    if ((val & val_test) == 0)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
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
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTEQUAL:
	    if (val != val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_SMALLER:
	    if (val < val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_LARGER:
	    if (val > val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_MASK:
	    if ((val & val_test) == val_test)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTMASK:
	    if ((val & val_test) == 0)
	      {
		D(("...succeeded.\n"));
		D_RETURN_(em->mimetype);
	      }
	    break;
	  default:
	    D(("UNKNOWN test type!\n"));
	  }
      }
      break;
    case EFSD_MAGIC_STRING:
      {
	int   len;
	char  s[MAXPATHLEN];

	len = strlen(em->value);
	fgets(s, len+1, f);

	D(("Performing string test: '%s' == '%s'\n", s, (char*)em->value));

	if (memcmp(s, em->value, len) == 0)
	  {
	    D(("...succeeded.\n"));
	    D_RETURN_(em->mimetype);
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
magic_test_fs(char *filename)
{
  static char   s[MAXPATHLEN];
  char         *ptr;
  struct stat   st;
  struct statfs stfs;

  D_ENTER;

  if (stat(filename, &st) < 0)
    D_RETURN_(NULL);

#ifdef __EMX__
   sprintf(s, "%s", "hpfs");
#else
  if (statfs(filename, &stfs) < 0)
    D_RETURN_(NULL);

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
    default:
      sprintf(s, "%s", "unknown-fs");
    }
#endif

  ptr = s + strlen(s);
    
  if (S_ISLNK(st.st_mode))
    {
      sprintf(ptr, "%s", "/link");
    }
  else if (S_ISDIR(st.st_mode))
    {
      sprintf(ptr, "%s", "/dir");
    }
  else if (S_ISCHR(st.st_mode))
    {
      sprintf(ptr, "%s", "/chardev");
    }
#ifndef __EMX__
  else if (S_ISBLK(st.st_mode))
    {
      sprintf(ptr, "%s", "/block");
    }
#endif
  else if (S_ISFIFO(st.st_mode))
    {
      sprintf(ptr, "%s", "/fifo");
    }
  else if (S_ISSOCK(st.st_mode))
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

      if (s[last] == '-')
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
	  pattern_mimetypes_user = malloc(sizeof(char*) * num_patterns_user);
	  
	  db = e_db_open_read(s);
	  
	  for (i = 0; i < num_patterns_user; i++)
	    pattern_mimetypes_user[i] = e_db_str_get(db, patterns_user[i]);
	  
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
	  pattern_mimetypes = malloc(sizeof(char*) * num_patterns);
	  
	  D(("opening '%s'\n", s));
	  db = e_db_open_read(s);
      
	  for (i = 0; i < num_patterns; i++)
	    pattern_mimetypes[i] = e_db_str_get(db, patterns[i]);
      
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
	  D_RETURN_(pattern_mimetypes_user[i]);
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
	  D_RETURN_(pattern_mimetypes[i]);
	}
    }

  D_RETURN_(NULL);
}


int       
efsd_magic_init(void)
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

  bzero(&magic, sizeof(EfsdMagic));
  ptr = key;

  magic_init_level(key, ptr, &magic);
  e_db_close(magic_db);

  D_RETURN_(patterns_init());
}


void       
efsd_magic_cleanup(void)
{
  int i;

  D_ENTER;

  magic_cleanup_level(&magic);
  magic.kids = NULL;

  for (i = 0; i < num_patterns; i++)
    {
      FREE(patterns[i]);
      FREE(pattern_mimetypes[i]);
    }

  FREE(patterns);
  FREE(pattern_mimetypes);

  D_RETURN;
}


char      *
efsd_magic_get(char *filename)
{
  char *result = NULL;

  D_ENTER;

  result = magic_test_fs(filename);
  if (result)
    D_RETURN_(result);

  D(("magic: fs check failed.\n"));

  result = magic_test_data(filename);
  if (result)
    D_RETURN_(result);

  D(("magic: data check failed.\n"));

  /*
  result = magic_test_pattern(filename);
  if (result)
    D_RETURN_(result);
  */

  D(("magic: file pattern check failed.\n"));
  
  result = "document/unknown";

  D_RETURN_(result);
}


