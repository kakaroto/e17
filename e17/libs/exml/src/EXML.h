#ifndef _EXML_H
# define _EXML_H

/**
 * @file EXML.h
 * @brief Contains XML reading/writing functions
 */

# ifdef __cplusplus
extern "C" {
# endif

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

EXML_Node *exml_node_new();
int exml_node_init(EXML_Node *node);

#ifdef __cplusplus
}
#endif
#endif				/* _EXML_H */

