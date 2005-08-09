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

/* libxml stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>

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
#include <efsd_stack.h>
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
  EFSD_MAGIC_TEST_AND       =  4,
  EFSD_MAGIC_TEST_NOTAND    =  5
}
EfsdMagicTest;

typedef enum efsd_byteorder
{
  EFSD_BYTEORDER_HOST       = 0,
  EFSD_BYTEORDER_BIG        = 1,
  EFSD_BYTEORDER_LITTLE     = 2
}
EfsdByteorder;


/* This is the data structure used to define a file magic
   test. All tests are stored in a tree in order to
   be able to represent the hierarchical nature of the
   tests -- all tests below a node are more specialized,
   like ones with more ">"'s in a magic file, check the
   manpage for details.
*/

typedef struct efsd_magic
{
  uint32_t              offset;
  EfsdMagicType         type;
  void                 *value;

  /* We need a a length field for the value when it is
     a character string, which may contain zeroes at
     arbitrary points, so strlen() would break.
  */
  int                   value_len; 

  EfsdByteorder         byteorder;

  char                  use_mask;
  int                   mask;
  EfsdMagicTest         test;

  char                 *filetype;
  char                 *formatter;
  char                 *regexp;

  struct efsd_magic    *next;
  struct efsd_magic    *prev;
  struct efsd_magic    *kids;
  struct efsd_magic    *last_kid;
}
EfsdMagic;


typedef struct efsd_pattern
{
  char                 *pattern;
  char                 *filetype;
}
EfsdPattern;


typedef struct efsd_filetype_cache_item
{
  char    *filetype; /* Cached filetype */
  time_t   time;     /* Timestamp of last calculation */ 
}
EfsdFiletypeCacheItem;

#ifdef WORDS_BIGENDIAN
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_BIG;
#else
static EfsdByteorder host_byteorder = EFSD_BYTEORDER_LITTLE;
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
static EfsdMagic  sys_magic;
static EfsdMagic  user_magic;

/* Mhmm well ideally there should be separate locking for
   user magic and system-wide magic ... */
static EfsdLock  *magic_lock;

/* Filename patterns */
static EfsdList  *user_patterns;
static EfsdList  *sys_patterns;

static EfsdLock  *filetype_cache_lock;
static EfsdHash  *filetype_cache;

/* XML interface */

typedef enum efsd_xml_parser_state
{
  EFSD_XML_STATE_UNKNOWN,
  EFSD_XML_STATE_MAGICTESTS,
  EFSD_XML_STATE_MTEST,
  EFSD_XML_STATE_PTEST,
  EFSD_XML_STATE_PATTERN,
  EFSD_XML_STATE_OFFSET,
  EFSD_XML_STATE_MAGIC,
  EFSD_XML_STATE_MASK,
  EFSD_XML_STATE_REGEXP,
  EFSD_XML_STATE_DESCR
}
EfsdXmlParserState;

typedef struct efsd_xml_parser_context
{
  EfsdMagic    *magic_root_node;

  EfsdList    **pattern_list;
  EfsdPattern  *pattern;

  char          cdata[MAXPATHLEN];

  EfsdStack    *state_stack;
  EfsdStack    *node_stack;
}
EfsdXmlParserContext;


static int          filetype_magic_load_xml(const char *filename, EfsdMagic *magic_root_node,
					    EfsdList **pattern_list);
static int          filetype_magic_save_xml(EfsdMagic *em, const char *filename);
static void         filetype_magic_test_to_xml(xmlNodePtr parent_node, EfsdMagic *em, int *id);
static void         filetype_pattern_test_to_xml(xmlNodePtr parent_node, EfsdPattern *ep);

static EfsdMagic   *filetype_magic_new(void);
static void         filetype_magic_free(EfsdMagic *em);
static void         filetype_magic_cleanup_level(EfsdMagic *em);
static void         filetype_magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid);

static void         filetype_magic_move_match_to_front(EfsdMagic *dad, EfsdMagic *kid);
static char        *filetype_magic_test_toplevel(EfsdMagic *em, FILE *f,
						 char *ptr, EfsdMagic **matching_kid);
static char        *filetype_magic_test_level(EfsdMagic *em, FILE *f,
					      char *ptr, char stop_when_found,
					      EfsdMagic **matching_kid);
static char        *filetype_magic_test_perform(EfsdMagic *em, FILE *f);

static EfsdPattern *filetype_pattern_new(void);
static void         filetype_pattern_free(EfsdPattern *ep);
static int          filetype_pattern_test(EfsdPattern *ep, const char *filename);

/* Scans a format string and returns pointer to first conversion
   specifier. Escaped percentage signs are skipped, additional
   conversion specifiers are overwritten with whitespace. */
static char        *filetype_analyze_format_string(char *format);

/* Substitutes the conversion specifier in the filetype entry in EM
   with the data the current file F provides. The resulting string
   is placed in SUBST.
*/
static int          filetype_substitute_value(EfsdMagic *em, FILE *f, char *subst, int substlen);

static int          filetype_init_system_settings(void);
static int          filetype_init_user_settings(void);
static void         filetype_cleanup_system_settings(void);
static void         filetype_cleanup_user_settings(void);

static uint16_t     filetype_fix_byteorder_short(EfsdMagic *em, uint16_t val);
static uint32_t     filetype_fix_byteorder_long(EfsdMagic *em, uint32_t val);

static int          filetype_test_fs(char *filename, struct stat *st, char *type, int len);
static int          filetype_test_magic(char *filename, char *type, int len);
static int          filetype_test_patterns(char *filename, char *type, int len);

static void         filetype_cache_init(void);
static void         filetype_cache_update(char *filename, time_t time, const char *filetype);
static EfsdFiletypeCacheItem *filetype_cache_lookup(char *filename);
static void         filetype_hash_item_free(EfsdHashItem *it);


static int
filetype_magic_save_xml(EfsdMagic *em, const char *filename)
{
  EfsdMagic *test;
  EfsdList  *pattern;
  xmlDocPtr  doc;
  int        result = TRUE;
  int        id = 0;

  D_ENTER;

  if (!em || !filename || filename[0] == '\0')
    D_RETURN_(FALSE);

  doc = xmlNewDoc("1.0");
  doc->children = xmlNewDocNode(doc, NULL, "filetypes", NULL);

  /* Build the subtrees for each toplevel test: */
  
  for (test = em->kids; test; test = test->next)
    filetype_magic_test_to_xml((xmlNodePtr)(doc->children), test, &id);

  /* Also add the system-wide pattern subtree: */
  for (pattern = sys_patterns; pattern; pattern = efsd_list_next(pattern))
    filetype_pattern_test_to_xml((xmlNodePtr)(doc->children), efsd_list_data(pattern));
  
  /* We now have a the full test tree as an XML document.
     Dump it, and indent! */
  xmlIndentTreeOutput = TRUE;
  if (xmlSaveFormatFile(filename, doc, TRUE) < 0)
    result = FALSE;
     
  /* Finally get rid of the tree. */
  xmlFreeDoc(doc);

  D_RETURN_(result);
}


static void
filetype_magic_test_to_xml(xmlNodePtr parent_node, EfsdMagic *em, int *id)
{
  EfsdMagic  *test;
  xmlNodePtr  node;
  char        s[MAXPATHLEN];
  char       *sptr = s;
  int         val;

  D_ENTER;

  node = xmlNewChild(parent_node, NULL, "mtest", NULL);

  /* Set element properties -- data, byteorder and test type: */

  switch (em->type)
    {
    case EFSD_MAGIC_8:
      xmlSetProp(node, "data", "byte");
      break;
    case EFSD_MAGIC_16:
      xmlSetProp(node, "data", "short");
      break;
    case EFSD_MAGIC_32:
      xmlSetProp(node, "data", "long");
      break;
    case EFSD_MAGIC_DATE:
      xmlSetProp(node, "data", "date");
      break;
    case EFSD_MAGIC_STRING:
      xmlSetProp(node, "data", "string");
      break;
    default:
      D("Not setting a data type.\n");
    }

  switch (em->byteorder)
    {
    case EFSD_BYTEORDER_BIG:
      xmlSetProp(node, "byteorder", "be");
      break;
    case EFSD_BYTEORDER_LITTLE:
      xmlSetProp(node, "byteorder", "le");
      break;
    case EFSD_BYTEORDER_HOST:
      xmlSetProp(node, "byteorder", "host");
      break;
    default:
      D("UNKNOWN byteorder %i\n", em->byteorder);
    }

  switch (em->test)
    {
    case EFSD_MAGIC_TEST_EQUAL:
      xmlSetProp(node, "type", "eq");
      break;
    case EFSD_MAGIC_TEST_NOTEQUAL:
      xmlSetProp(node, "type", "ne");
      break;
    case EFSD_MAGIC_TEST_SMALLER:
      xmlSetProp(node, "type", "st");
      break;
    case EFSD_MAGIC_TEST_LARGER:
      xmlSetProp(node, "type", "lt");
      break;
    case EFSD_MAGIC_TEST_AND:
      xmlSetProp(node, "type", "and");
      break;
    case EFSD_MAGIC_TEST_NOTAND:
      xmlSetProp(node, "type", "nand");
      break;
    default:
      D("Not setting test type %i.\n", em->test);
    }
  
  /* Set ID: */
  snprintf(s, MAXPATHLEN, "%u", (*id)++);
  xmlSetProp(node, "id", s);
  
  snprintf(s, MAXPATHLEN, "%u", em->offset);
  xmlNewChild(node, NULL, "offset", s);

  /* Write the magic value itself: */

  if (!em->value)
    {
      xmlNewChild(node, NULL, "magic", NULL);
    }
  else
    {
      switch (em->type)
	{
	case EFSD_MAGIC_8:
	  {
	    val = *(uint8_t*)em->value;
	    snprintf(s, MAXPATHLEN, "0x%.2hhx", val);
	  }
	  break;
	case EFSD_MAGIC_16:
	  {
	    val = filetype_fix_byteorder_short(em, *(uint16_t*)em->value);
	    snprintf(s, MAXPATHLEN, "0x%.4hx", val);
	  }
	  break;
	case EFSD_MAGIC_32:
	case EFSD_MAGIC_DATE:
	  {
	    val = filetype_fix_byteorder_long(em, *(uint32_t*)em->value);
	    snprintf(s, MAXPATHLEN, "0x%.8lx", (long int)val);
	  }
	  break;
	case EFSD_MAGIC_STRING:
	  {
	    /* We need to escape any characters that are not standard!
	       The format is always \0xXX to keep things simple.
	     */

	    int i;
	    char *str = (char*)em->value;

	    sptr = s;

	    /* Walk through string, escaping anything that's not
	       directly printable using the format given above.
	    */
	    for (i = 0; i < em->value_len; i++)
	      {
		if (str[i] > 31 && str[i] < 127)
		  {
		    /* Normal character, just copy. */
		    *sptr = str[i];
		    sptr++;
		  }
		else
		  {
		    /* Argh, need to escape: */
		    snprintf(sptr, MAXPATHLEN - (sptr - s), "\\0x%.2hhx", str[i]);
		    sptr += 5; /* Fixed length, escaped value is always \0xXY */
		  }
	      }
	    *sptr = '\0';
	    sptr = s;
	  }
	  break;
	default:
	  sptr = NULL;
	}

      xmlNewChild(node, NULL, "magic", sptr);
    }

  /* Write value mask, if any. */
  if (em->use_mask)
    {
      snprintf(s, MAXPATHLEN, "0x%.8lx", (long int) em->mask);
      xmlNewChild(node, NULL, "mask", s);
    }

  /* Write regexp mask, if any. */
  if (em->regexp)
    {
      xmlNewChild(node, NULL, "regexp", em->regexp);
    }

  xmlNewChild(node, NULL, "descr", em->filetype);
  
  for (test = em->kids; test; test = test->next)
    filetype_magic_test_to_xml(node, test, id);
    
  D_RETURN;
}


static void
filetype_pattern_test_to_xml(xmlNodePtr parent_node, EfsdPattern *ep)
{
  xmlNodePtr  node;

  D_ENTER;

  node = xmlNewChild(parent_node, NULL, "ptest", NULL);
  
  xmlNewChild(node, NULL, "pattern", ep->pattern);
  xmlNewChild(node, NULL, "descr", ep->filetype);
      
  D_RETURN;
}


static xmlEntityPtr
filetype_sax_callback_get_entity(void *ctxt, const xmlChar *name)
{
  return (NULL);
  ctxt = NULL;
  name = NULL;
}


static void
filetype_sax_callback_characters(void *user_data, const xmlChar *ch, int len)
{
  EfsdXmlParserContext *ctxt = (EfsdXmlParserContext*)user_data;
  EfsdXmlParserState    state = (EfsdXmlParserState)efsd_stack_top(ctxt->state_stack);

  D_ENTER;

  if ((state == EFSD_XML_STATE_OFFSET)  ||
      (state == EFSD_XML_STATE_MAGIC)   ||
      (state == EFSD_XML_STATE_MASK)    ||
      (state == EFSD_XML_STATE_REGEXP)  ||
      (state == EFSD_XML_STATE_PATTERN) ||
      (state == EFSD_XML_STATE_DESCR))
    {
      strncat(ctxt->cdata, ch, len);
      /* D("CDATA now '%s'\n", ctxt->cdata); */
    }

  D_RETURN;
}


static void
filetype_sax_callback_start_element(void *user_data, const xmlChar *name,
				    const xmlChar **attrs)
{
  int                   i;
  EfsdMagic            *node;
  EfsdXmlParserContext *ctxt = (EfsdXmlParserContext*)user_data;
  const xmlChar        *attr, *attr_val;

  D_ENTER;

  if (!strcmp(name, "filetypes"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_MAGICTESTS);
      efsd_stack_push(ctxt->node_stack, ctxt->magic_root_node);
    }
  else if (!strcmp(name, "mtest"))
    {
      EfsdMagic *parent_node;

      D("New node.\n");

      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_MTEST);

      /* Create new node for this test. */
      node = filetype_magic_new();

      /* Now fill in the values as passed from the parser. */
      for (i = 0; attrs[i]; i += 2)
	{
	  attr     = attrs[i];
	  attr_val = attrs[i+1];

	  if (!strcmp(attr, "data"))
	    {
	      if (!strcmp(attr_val, "byte"))
		{
		  D("Read data type BYTE\n");
		  node->type = EFSD_MAGIC_8;
		}
	      else if (!strcmp(attr_val, "short"))
		{
		  D("Read data type SHORT\n");
		  node->type = EFSD_MAGIC_16;
		}
	      else if (!strcmp(attr_val, "long"))
		{
		  D("Read data type LONG\n");
		  node->type = EFSD_MAGIC_32;
		}
	      else if (!strcmp(attr_val, "date"))
		{
		  D("Read data type DATE\n");
		  node->type = EFSD_MAGIC_DATE;
		}
	      else if (!strcmp(attr_val, "string"))
		{
		  D("Read data type STR\n");
		  node->type = EFSD_MAGIC_STRING;
		}
	      else
		{
		  D("parser warning -- unknown magic type '%s'\n", attr_val);
		}
	    }
	  else if (!strcmp(attr, "byteorder"))
	    {
	      if (!strcmp(attr_val, "le"))
		{
		  D("Read byteorder little\n");
		  node->byteorder = EFSD_BYTEORDER_LITTLE;
		}
	      else if (!strcmp(attr_val, "be"))
		{
		  D("Read byteorder big\n");
		  node->byteorder = EFSD_BYTEORDER_BIG;
		}
	      else if (!strcmp(attr_val, "host"))
		{
		  D("Read byteorder host\n");
		  node->byteorder = EFSD_BYTEORDER_HOST;
		}
	      else
		{
		  D("parser warning -- unknown byte order '%s'\n", attr_val);
		}
	    }
	  else if (!strcmp(attr, "type"))
	    {
	      if (!strcmp(attr_val, "eq"))
		{
		  node->test = EFSD_MAGIC_TEST_EQUAL;
		}
	      else if (!strcmp(attr_val, "ne"))
		{
		  node->test = EFSD_MAGIC_TEST_NOTEQUAL;
		}
	      else if (!strcmp(attr_val, "lt"))
		{
		  node->test = EFSD_MAGIC_TEST_LARGER;
		}
	      else if (!strcmp(attr_val, "st"))
		{
		  node->test = EFSD_MAGIC_TEST_SMALLER;
		}
	      else if (!strcmp(attr_val, "and"))
		{
		  node->test = EFSD_MAGIC_TEST_AND;
		}
	      else if (!strcmp(attr_val, "nand"))
		{
		  node->test = EFSD_MAGIC_TEST_NOTAND;
		}
	      else
		{
		  D("parser warning -- unknown test type '%s'\n", attr_val);
		}
	    }
	  else
	    {
	      D("parser warning -- unknown test attribute '%s'\n", attr_val);
	    }
	}
      
      /* Add the node as a kid to its parent node. */      
      if ((parent_node = efsd_stack_top(ctxt->node_stack)) != NULL)
	{
	  D("Adding node.\n");
	  filetype_magic_add_child(parent_node, node);      	    
	}

      /* Push the kid onto the stack in case we encounter
	 more specific tests. */
      efsd_stack_push(ctxt->node_stack, node);
    }
  else if (!strcmp(name, "offset"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_OFFSET);
    }
  else if (!strcmp(name, "magic"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_MAGIC);
    }
  else if (!strcmp(name, "mask"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_MASK);
    }
  else if (!strcmp(name, "regexp"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_REGEXP);
    }
  else if (!strcmp(name, "descr"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_DESCR);
    }
  else if (!strcmp(name, "ptest"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_PTEST);

      filetype_pattern_free(ctxt->pattern);
      ctxt->pattern = filetype_pattern_new();
    }
  else if (!strcmp(name, "pattern"))
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_PATTERN);
    }
  else
    {
      efsd_stack_push(ctxt->state_stack,
		      (void*)EFSD_XML_STATE_UNKNOWN);
    }

  D_RETURN;
}


static void 
filetype_sax_callback_end_element(void *user_data, const xmlChar *name)
{
  EfsdXmlParserContext *ctxt = (EfsdXmlParserContext*)user_data;
  EfsdMagic            *em = NULL;

  D_ENTER;

  efsd_stack_pop(ctxt->state_stack);
  
  if (!strcmp(name, "mtest"))
    {
      em = (EfsdMagic*)efsd_stack_pop(ctxt->node_stack);
      D("Popped node.\n");
    }
  else if (!strcmp(name, "offset"))
    {
      em = (EfsdMagic*)efsd_stack_top(ctxt->node_stack);
      em->offset = atoi(ctxt->cdata);
    }
  else if (!strcmp(name, "magic") && (ctxt->cdata[0] != '\0'))
    {      
      em = (EfsdMagic*)efsd_stack_top(ctxt->node_stack);
      
      switch (em->type)
	{
	case EFSD_MAGIC_8:
	  {
	    uint8_t *val = NEW(uint8_t);
	    *val = (uint8_t)strtoul(ctxt->cdata, NULL, 0);
	    em->value = (void*)val;
	    D("Read byte val %.2hhx\n", *val);
	  }
	  break;
	case EFSD_MAGIC_16:
	  {
	    uint16_t *val = NEW(uint16_t);
	    *val = (uint16_t)strtoul(ctxt->cdata, NULL, 0);
	    *val = filetype_fix_byteorder_short(em, *val);
	    em->value = (void*)val;
	    D("Read short val %.4hx\n", *val);
	  }
	  break;
	case EFSD_MAGIC_32:
	case EFSD_MAGIC_DATE:
	  {
	    uint32_t *val = NEW(uint32_t);
	    *val = (uint32_t)strtoul(ctxt->cdata, NULL, 0);
	    *val = filetype_fix_byteorder_long(em, *val);
	    em->value = (void*)val;
	    D("Read from %s long val %.8lx\n", ctxt->cdata, (long int)*val);
	  }
	  break;
	case EFSD_MAGIC_STRING:
	  {
	    char      *s;
	    char      *s_next, *s_last;
	    char      old;
	    long int  val;
	    
	    em->value = strdup(ctxt->cdata);
	    em->value_len = strlen(ctxt->cdata);
	    D("Read string '%s'\n", (char*)em->value);
	    
	    s = (char*) em->value;
	    s_last = s + strlen(s) + 1;
	    s_next = s;
	    
	    /* We always encode values as \0xXX, so that string
	       always has 5 characters. Replace the "\" with
	       the numeric value, move everything after the string,
	       including the terminating zero, to the location
	       after the "\". */
		
	    while ((s_next = strstr(s_next, "\\0x")) != NULL)
	      {
		old = s_next[5];
		s_next[5] = '\0';
		val = strtoul(s_next+1, NULL, 0);
		s_next[5] = old;
		
		if (val != LONG_MIN && val != LONG_MAX)
		  {
		    /* Conversion worked, now replace: */
		    *s_next = (char)val;
		    memmove(s_next + 1, s_next + 5, s_last - (s_next + 5));
		    s_next++;
		    s_last -= 4;
		    em->value_len -= 4;
		  }
		else
		  {
		    s_next += 5;
		  }
	      }
	  }
	  break;
	default:
	  D("Warning -- unknown magic type in parsed magic.\n");
	}      
    }
  else if (!strcmp(name, "mask") && (ctxt->cdata[0] != '\0'))
    {
      em = (EfsdMagic*)efsd_stack_top(ctxt->node_stack);

      em->use_mask = TRUE;
      em->mask = strtoul(ctxt->cdata, NULL, 0);
      D("Read mask val %i\n", em->mask);
    }
  else if (!strcmp(name, "regexp") && (ctxt->cdata[0] != '\0'))
    {
      em = (EfsdMagic*)efsd_stack_top(ctxt->node_stack);

      em->regexp = strdup(ctxt->cdata);
      D("Read regexp val %s\n", em->regexp);
    }
  else if (!strcmp(name, "descr"))
    {
      /* The <descr> element can occur both in pattern tests
	 and magic tests, but they are handled differently: */

      if (ctxt->pattern && (ctxt->cdata[0] != '\0'))
	{
	  FREE(ctxt->pattern->filetype);
	  ctxt->pattern->filetype = strdup(ctxt->cdata);
	}
      else
	{
	  em = (EfsdMagic*)efsd_stack_top(ctxt->node_stack);
	  
	  if (ctxt->cdata[0] != '\0')
	    {
	      em->filetype = strdup(ctxt->cdata);
	      em->formatter = filetype_analyze_format_string(em->filetype);
	    }
	  else
	    {
	      em->filetype = "";
	      em->formatter = NULL;
	    }

	  D("Read type %s\n", em->filetype);
	}
    }
  else if (!strcmp(name, "pattern") && (ctxt->cdata[0] != '\0'))
    {
      FREE(ctxt->pattern->pattern);
      ctxt->pattern->pattern = strdup(ctxt->cdata);
    }
  else if (!strcmp(name, "ptest"))
    {
      D("Adding pattern test %s --> %s\n", ctxt->pattern->pattern, ctxt->pattern->filetype);
      *(ctxt->pattern_list) = efsd_list_prepend(*(ctxt->pattern_list), ctxt->pattern);
      ctxt->pattern = NULL;
    }

  ctxt->cdata[0] = '\0';
      
  D_RETURN;
}


static int
filetype_magic_load_xml(const char *filename, EfsdMagic *magic_root_node,
			EfsdList **pattern_list)
{
  xmlSAXHandler         sax;
  EfsdXmlParserContext  ctxt;
  int                   result;

  D_ENTER;

  D("Loading file %s\n", filename);

  if (!filename || filename[0] == '\0')
    D_RETURN_(FALSE);
  
  /* Set up SAX event handlers: */
  memset(&sax, 0, sizeof(xmlSAXHandler));
  sax.characters    = filetype_sax_callback_characters;
  sax.startElement  = filetype_sax_callback_start_element;
  sax.endElement    = filetype_sax_callback_end_element;
  sax.getEntity     = filetype_sax_callback_get_entity;

  memset(&ctxt, 0, sizeof(EfsdXmlParserContext));
  ctxt.magic_root_node = magic_root_node;
  ctxt.pattern_list = pattern_list;
  ctxt.state_stack = efsd_stack_new();
  ctxt.node_stack = efsd_stack_new();

  result = xmlSAXUserParseFile(&sax, &ctxt, filename);

  efsd_stack_free(ctxt.state_stack, NULL);
  efsd_stack_free(ctxt.node_stack, NULL);

  D_RETURN_(result);
}


static EfsdMagic *
filetype_magic_new(void)
{
  EfsdMagic *em;

  D_ENTER;

  em = NEW(EfsdMagic);

  if (!em)
    D_RETURN_(NULL);

  memset(em, 0, sizeof(EfsdMagic));

  D_RETURN_(em);
}



static void       
filetype_magic_free(EfsdMagic *em)
{
  D_ENTER;

  if (!em)
    D_RETURN;

  FREE(em->value);
  if (em->filetype != "")
    FREE(em->filetype);

  /* formatter points into value, so no freeing here! */
  FREE(em->regexp);
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

  em->kids = NULL;

  D_RETURN;
}


static void       
filetype_magic_add_child(EfsdMagic *em_dad, EfsdMagic *em_kid)
{
  D_ENTER;

  if (!em_dad || !em_kid)
    D_RETURN;

  if (em_dad->kids)
    {
      em_dad->last_kid->next = em_kid;
      em_kid->prev = em_dad->last_kid;
      em_kid->next = NULL;
    }
  else
    {
      em_dad->kids = em_kid;
      em_kid->prev = em_kid->next = NULL;
    }

  em_dad->last_kid = em_kid;

  D_RETURN;
}


static EfsdPattern *
filetype_pattern_new(void)
{
  EfsdPattern *ep;

  D_ENTER;

  ep = NEW(EfsdPattern);
  
  if (!ep)
    D_RETURN_(NULL);

  memset(ep, 0, sizeof(EfsdPattern));

  D_RETURN_(ep);
}


static void         
filetype_pattern_free(EfsdPattern *ep)
{
  D_ENTER;

  if (!ep)
    D_RETURN;

  FREE(ep->pattern);
  FREE(ep->filetype);

  D_RETURN;
}


static int
filetype_pattern_test(EfsdPattern *ep, const char* filename)
{
  char *ptr;

  D_ENTER;

  ptr = strrchr(filename, '/');
  if (!ptr)
    ptr = (char*) filename;
  else
    ptr++;

  if (!fnmatch(ep->pattern, ptr, FNM_PATHNAME | FNM_PERIOD))
    D_RETURN_(TRUE);

  D_RETURN_(FALSE);
}


static uint16_t
filetype_fix_byteorder_short(EfsdMagic *em, uint16_t val)
{
  D_ENTER;

  if ((em->byteorder == host_byteorder)  || 
      (em->byteorder == EFSD_BYTEORDER_HOST))
    D_RETURN_(val);

  D_RETURN_(SWAP_SHORT(val));
}


static uint32_t
filetype_fix_byteorder_long(EfsdMagic *em, uint32_t val)
{
  D_ENTER;

  if ((em->byteorder == host_byteorder)  || 
      (em->byteorder == EFSD_BYTEORDER_HOST))
    D_RETURN_(val);

  D_RETURN_(SWAP_LONG(val));
}


static char      *
filetype_magic_test_perform(EfsdMagic *em, FILE *f)
{
  D_ENTER;
  
  if (!em || !f)
    D_RETURN_(NULL);

  /* Seek to the magic value in the file */

  if (fseek(f, em->offset, SEEK_SET) < 0)
    D_RETURN_(NULL);

  /* When no magic value is given, the test automatically passes. */

  if (!em->value)
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
	  case EFSD_MAGIC_TEST_AND:
	    if ((val & val_test) == val_test)
	      {
		D("Mask test: %x == %x? succeeded.\n",
		   (val & val_test),
		   val_test);
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTAND:
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
	  case EFSD_MAGIC_TEST_AND:
	    if ((val & val_test) == val_test)
	      {
		D("Mask test: %x == %x? succeeded.\n",
		   (val & val_test),
		   val_test);
		D_RETURN_(em->filetype);
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTAND:
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
	  case EFSD_MAGIC_TEST_AND:
	    if ((val & val_test) == val_test)
	      {
		D("Long test: %x & %x succeeded.\n", val, *((uint32_t*)em->value));
		D_RETURN_(em->filetype); 
	      }
	    break;
	  case EFSD_MAGIC_TEST_NOTAND:
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

	for (i = 0; (i < em->value_len) && (i < MAXPATHLEN); i++)
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
    default: ;
    }

  D_RETURN_(NULL);
}


/* This is a wrapper call to filetype_magic_test_level() that
   is intended for calls to the root level of the magic tree
   structure. It protects access to the tree through readlocks.
*/
static char *
filetype_magic_test_toplevel(EfsdMagic *level, FILE *f,
			     char *ptr, EfsdMagic **matching_kid)
{
  char *result = NULL;

  D_ENTER;

  efsd_lock_get_read_access(magic_lock);
  result = filetype_magic_test_level(level, f, ptr, TRUE, matching_kid);
  efsd_lock_release_read_access(magic_lock);

  D_RETURN_(result);
}

static char *
filetype_magic_test_level(EfsdMagic *level, FILE *f,
			  char *ptr, char stop_when_found,
			  EfsdMagic **matching_kid)
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

	  if ((ptr2 = filetype_magic_test_level(em->kids, f, ptr, FALSE, NULL)))
	    {
	      result = ptr = ptr2;
	    }

	  /* STOP_WHEN_FOUND is true only on the top level, on all
	     levels below, the results are appended when multiple
	     matches ocurr.
	  */

	  if (stop_when_found)
	    {
	      *matching_kid = em;
	      D_RETURN_(result);
	    }
	}
    }

  D_RETURN_(result);
}


static char *
filetype_analyze_format_string(char *format)
{
  char  conversion_specs[] = "diouxXeEfFgGaAcsCSPn";
  char *strp = format;
  char *subst_loc = NULL;

  D_ENTER;

  if (!format || format[0] == '\0')
    D_RETURN_(NULL);

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
	val16 = filetype_fix_byteorder_short(em, val16);

	val = (int)val16;
	data = (void*)val;
      }
      break;
    case EFSD_MAGIC_32:
      {
	uint32_t val32;
	
	if (fread(&val32, sizeof(val32), 1, f) != 1)
		D_RETURN_(FALSE);
	val32 = filetype_fix_byteorder_long(em, val32);

	val = (int)val32;
	data = (void*)val;
      }
      break;
    case EFSD_MAGIC_DATE:
      {
	uint32_t  val32;
	
	if (fread(&val32, sizeof(val32), 1, f) != 1)
	  D_RETURN_(FALSE);
	val32 = filetype_fix_byteorder_long(em, val32);

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

  /* First, determine the basic file type, and if it's a link, check
     whether it's dead or not. Then, look up the type of the fs that
     the file lives on. The result is something like "dir/ext2".
  */

  if (S_ISLNK(st->st_mode))
    {
      char real[MAXPATHLEN];
      int  real_len = 0;
      
      if ( (real_len = readlink(filename, real, real_len) >= 0))
	{
	  real[real_len] = '\0';
	  
	  if (!efsd_misc_file_exists(real))
	    broken_link = TRUE;
	}
      else
	{
	  broken_link = TRUE;
	}

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

  /* Okay, we have the basic file type. Now stat the filesystem,
     look up the type and complete the file type string. */

  if (statfs(filename, &stfs) < 0)
    {
      snprintf(ptr, len - fslen, "/%s", "unknown-fs");
      D_RETURN_(TRUE);
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

/* When we find a match on the top level (below the root node)
   of the magic tree, we can optimize things by moving the matched
   filetype to the head of the lists of tests. Usually files in
   a directory are of only a few different types, so this speeds
   things up when we have a directory full of, say, Jpegs.
   Thanks for the suggestion go to Raster, cheers mate.
*/
static void
filetype_magic_move_match_to_front(EfsdMagic *dad, EfsdMagic *kid)
{
  D_ENTER;

  if (!dad || !kid)
    D_RETURN;

  efsd_lock_get_write_access(magic_lock);

  /* Do NOT move this test above the above line! */
  if (!kid->prev)
    {
      /* Nothing to optimize */
      efsd_lock_release_write_access(magic_lock);
      D_RETURN;
    }

  if (kid == dad->last_kid)
    dad->last_kid = kid->prev;

  if (kid->prev)
    kid->prev->next = kid->next;

  if (kid->next)
    kid->next->prev = kid->prev;

  kid->prev = NULL;
  kid->next = dad->kids;
  kid->next->prev = kid;
  dad->kids = kid;

  efsd_lock_release_write_access(magic_lock);

  D_RETURN;
}

static int
filetype_test_magic(char *filename, char *type, int len)
{
  EfsdMagic   *match = NULL;
  FILE        *f = NULL;
  char         s[MAXPATHLEN];

  D_ENTER;

  if ((f = fopen(filename, "r")) == NULL)
    D_RETURN_(FALSE);
  
  if (filetype_magic_test_toplevel(sys_magic.kids, f, s, &match))
    {
      int last;

      filetype_magic_move_match_to_front(&sys_magic, match);

      last = strlen(s)-1;

      if (s[last] == '-' || s[last] == '/')
	s[last] = '\0';

      strncpy(type, s, len);
      fclose(f);

      D_RETURN_(TRUE);
    }

  if (filetype_magic_test_toplevel(user_magic.kids, f, s, &match))
    {
      int last;
      
      filetype_magic_move_match_to_front(&user_magic, match);

      last = strlen(s)-1;

      if (s[last] == '-' || s[last] == '/')
	s[last] = '\0';

      strncpy(type, s, len);
      fclose(f);

      D_RETURN_(TRUE);
    }

  fclose(f);
  D_RETURN_(FALSE);
}


static int
filetype_init_system_settings(void)
{
  D_ENTER;

  memset(&sys_magic, 0, sizeof(EfsdMagic));
  filetype_magic_load_xml(efsd_filetype_get_system_file(),
			  &sys_magic, &sys_patterns);
  
  /* Uncomment here to write out the file just read: */
  /* filetype_magic_save_xml(&sys_magic, "/tmp/magic.xml"); */
  
  D_RETURN_(TRUE);
}


static int
filetype_init_user_settings(void)
{
  D_ENTER;

  memset(&user_magic, 0, sizeof(EfsdMagic));
  filetype_magic_load_xml(efsd_filetype_get_user_file(),
			  &user_magic, &user_patterns);

  D_RETURN_(TRUE);
}

static int
filetype_test_patterns(char *filename, char *type, int len)
{
  EfsdList *l;
  EfsdPattern *ep;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  /* Test user-defined patterns first: */

  for (l = user_patterns; l; l = efsd_list_next(l))
    {
      ep = (EfsdPattern*)efsd_list_data(l);

      if (filetype_pattern_test(ep, filename))
	{
	  /* Found it! Now write out the filetype. */
	  strncpy(type, ep->filetype, len);	  
	  D_RETURN_(TRUE);
	}
    }

  /* If not found, use system-wide definitions. */

  for (l = sys_patterns; l; l = efsd_list_next(l))
    {
      ep = (EfsdPattern*)efsd_list_data(l);
      
      if (filetype_pattern_test(ep, filename))
	{
	  /* Found it! Now write out the filetype. */
	  strncpy(type, ep->filetype, len);	  
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
				 (EfsdFunc)filetype_hash_item_free);

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
filetype_cleanup_user_settings(void)
{
  D_ENTER;

  filetype_magic_cleanup_level(&user_magic);
  efsd_list_free(user_patterns, (EfsdFunc)filetype_pattern_free);
  user_patterns = NULL;

  D_RETURN;
}


static void       
filetype_cleanup_system_settings(void)
{
  D_ENTER;

  filetype_magic_cleanup_level(&sys_magic);
  efsd_list_free(sys_patterns, (EfsdFunc)filetype_pattern_free);
  sys_patterns = NULL;

  D_RETURN;
}


int       
efsd_filetype_init(void)
{
  D_ENTER;

  filetype_cache_init();
  filetype_cache_lock = efsd_lock_new();
  magic_lock = efsd_lock_new();

  efsd_lock_get_write_access(magic_lock);
  filetype_init_system_settings();
  filetype_init_user_settings();
  efsd_lock_release_write_access(magic_lock);

  D_RETURN_(TRUE);
}


void       
efsd_filetype_update_user_settings(void)
{
  D_ENTER;

  D("Reloading user settings.\n");
  efsd_lock_get_write_access(filetype_cache_lock);

  efsd_lock_get_write_access(magic_lock);
  filetype_cleanup_user_settings();
  filetype_init_user_settings();
  efsd_lock_release_write_access(magic_lock);

  efsd_lock_release_write_access(filetype_cache_lock);
  D("Done.\n");

  D_RETURN;
}


void       
efsd_filetype_update_system_settings(void)
{
  D_ENTER;

  D("Reloading system settings.\n");
  efsd_lock_get_write_access(filetype_cache_lock);

  efsd_lock_get_write_access(magic_lock);
  filetype_cleanup_system_settings();
  filetype_init_system_settings();
  efsd_lock_release_write_access(magic_lock);

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

  filetype_cleanup_system_settings();
  filetype_cleanup_user_settings();

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

  D("magic: magic check failed.\n");

  if (filetype_test_patterns(filename, type, len))
    {
      filetype_cache_update(filename, st.st_mtime, type);      
      D_RETURN_(TRUE);
    }

  D("magic: file pattern check failed.\n");

  strncpy(type, unknown_string, len);
  filetype_cache_update(filename, st.st_mtime, unknown_string);

  D_RETURN_(TRUE);
}


int
efsd_filetype_save_system_settings_to_file(const char *filename)
{
  int result;

  D_ENTER;

  result = filetype_magic_save_xml(&sys_magic, filename);

  D_RETURN_(result);
}


int
efsd_filetype_save_user_settings_to_file(const char *filename)
{
  int result;

  D_ENTER;

  result = filetype_magic_save_xml(&user_magic, filename);

  D_RETURN_(result);
}


char   *
efsd_filetype_get_system_file(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/filetypes.xml", efsd_misc_get_sys_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}


char   *
efsd_filetype_get_user_file(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/filetypes.xml", efsd_misc_get_user_dir());
  s[sizeof(s)-1] = '\0';

  if (efsd_misc_file_exists(s))
    D_RETURN_(s);

  D_RETURN_(NULL);
}



