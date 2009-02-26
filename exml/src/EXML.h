#ifndef _EXML_H
# define _EXML_H

#include <Ecore.h>
#include <Ecore_Data.h>

/**
 * @file EXML.h
 * @brief Contains XML reading/writing functions
 */

# ifdef __cplusplus
extern "C" {
# endif

#undef NEW
/**
 * @def NEW(type, num)
 * Allocates memory of @a num elements of sizeof(@a type).
 */
#define NEW(type, num) calloc(num, sizeof(type));

#undef REALLOC
/**
 * @def REALLOC(dat, type, num)
 * Reallocates memory pointed to by @a dat to @a num elements of sizeof(@a
 * type).
 */
#define REALLOC(dat, type, num) \
{ \
	if (dat) \
	  { \
		dat = realloc(dat, sizeof(type) * num); \
	  } \
}

#undef FREE
/**
 * @def FREE(dat)
 * Free the data pointed to by @a dat and it to NULL.
 */
#define FREE(dat) \
{ \
	free(dat); dat = NULL; \
}


#undef IF_FREE
/**
 * @def IF_FREE(dat)
 * If @a dat is non-NULL, free @a dat and assign it to NULL.
 */
#define IF_FREE(dat) \
{ \
	if (dat) FREE(dat); \
}

inline void exml_print_warning(const char *function, const char *sparam);

/* convenience macros for checking pointer parameters for non-NULL */
#undef CHECK_PARAM_POINTER_RETURN
#define CHECK_PARAM_POINTER_RETURN(sparam, param, ret) \
     if (!(param)) \
	 { \
	    exml_print_warning(__FUNCTION__, sparam); \
	    return ret; \
	 }
   
#undef CHECK_PARAM_POINTER
#define CHECK_PARAM_POINTER(sparam, param) \
     if (!(param)) \
	 { \
	    exml_print_warning(__FUNCTION__, sparam); \
	    return; \
	 }

/**
 * I want this one to be opaque simply because one of the elements will be
 * a libxslt pointer.
 */
typedef struct _exml_xsl EXML_XSL;

typedef struct _exml_node EXML_Node;

typedef struct _exml EXML;

struct _exml_node {
	char *tag;
	Ecore_Hash *attributes; /* string->string hash of attributes for this tag */
	char *value;
	Ecore_List *children; /* list of child xml nodes */
	EXML_Node *parent;
};

struct _exml {
	EXML_Node *top; /* top level node... must only be one */
	EXML_Node *current;
	Ecore_Hash *buffers;
};

/* Creating and initializing new xml structures */
EXML *exml_new();
int exml_init(EXML *xml);

/* Adding child tags to the current level */
inline int exml_start(EXML *xml);
inline int exml_end(EXML *xml);
inline int exml_tag_set(EXML *xml, char *tag);
inline int exml_attribute_set(EXML *xml, char *attr, char *value);
inline int exml_value_set(EXML *xml, char *value);

/* Removing tags from the document (recursive) */
inline int exml_tag_remove(EXML *xml);
void exml_destroy(EXML *xml);

/* Retrieve the current tag information */
inline EXML_Node *exml_get(EXML *xml);
inline char *exml_tag_get(EXML *xml);
inline char *exml_attribute_get(EXML *xml, char *attr);
inline char *exml_value_get(EXML *xml);

/**
 * Traverse the document, next steps to the next node in the current list and
 * returns the next tag's value, down and up move the current list up or down
 * the hierarchy (down returns NULL if there are no children)
 */
inline char *exml_goto_top(EXML *xml);
inline char *exml_goto_node(EXML *xml, EXML_Node *node);
inline char *exml_goto(EXML *xml, char *tag, char *value);
inline char *exml_next(EXML *xml);
inline char *exml_next_nomove(EXML *xml);
inline char *exml_down(EXML *xml);
inline char *exml_up(EXML *xml);

inline int exml_has_children(EXML *xml);
void exml_clear(EXML *xml);

int exml_file_read(EXML *xml, char *filename);
int exml_fd_read(EXML *xml, int fd);
int exml_mem_read(EXML *xml, void *s_mem, size_t len);
int exml_file_write(EXML *xml, char *filename);
int exml_fd_write(EXML *xml, int fd);
void *exml_mem_write(EXML *xml, size_t *len);

/**
 * XSLT utility functions
 */
EXML_XSL *exml_xsl_new(char *filename);
int exml_xsl_init(EXML_XSL *xsl, char *filename);
void exml_xsl_destroy(EXML_XSL *xsl);
int exml_transform_file_write(EXML *xml, EXML_XSL *xsl, const char *params[],
                              char *filename, int compression);
int exml_transform_fd_write(EXML *xml, EXML_XSL *xsl, const char *params[],
                            int fd);
void *exml_transform_mem_write(EXML *xml, EXML_XSL *xsl, const char *params[],
                               ssize_t *len);
void exml_transform_mem_free(EXML_XSL *xsl, void *ptr);

EXML_Node *exml_node_new();
int exml_node_init(EXML_Node *node);

#ifdef __cplusplus
}
#endif
#endif				/* _EXML_H */

