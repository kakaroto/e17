#ifndef _EWL_CONTAINER_H_
#define _EWL_CONTAINER_H_

#include "ewlcore.h"

enum _EwlContainerPaddingEnum	{
	EWL_PADDING_LEFT,
	EWL_PADDING_TOP,
	EWL_PADDING_RIGHT,
	EWL_PADDING_BOTTOM
};

typedef struct _EwlContainer EwlContainer;
struct _EwlContainer	{
	EwlWidget         widget;
	
	int               child_padding[4];

	EwlLL            *children;
	unsigned int      max_children; /* this adds an O(n) to insert */
	EwlBool           is_full;

	void             (*insert)(EwlContainer *container,
	                           EwlWidget *child,
	                           EwlData *data);
	void             (*remove)(EwlContainer *container,
	                           EwlWidget *child,
	                           EwlData *data);
	EwlBool           propagate_events;
	void             (*propagate)(EwlContainer *c, EwlEvent *ev);
	void             (*resize_children)(EwlContainer *c);
};

EwlWidget   *ewl_container_new();
void         ewl_container_init(EwlContainer *c);
void         ewl_container_free(EwlWidget *c);

void         ewl_container_insert(EwlWidget *container, EwlWidget *child);
void         ewl_container_insert_before(EwlWidget *container,EwlWidget *child);
void         ewl_container_remove(EwlWidget *container, EwlWidget *child);
void         ewl_container_event_propagate(EwlWidget *container, EwlEvent *ev);

void         ewl_container_foreach(EwlWidget    *container,
                                   EwlBool     (*cb)(EwlLL   *w,
                                                     EwlData *d),
                                   EwlData      *d);

/* setting this has an O(2n) insert time -- you have been warned */
unsigned int ewl_container_get_max_children(EwlWidget *c);
void         ewl_container_set_max_children(EwlWidget *c, unsigned int max);

EwlBool      ewl_contaner_is_full(EwlWidget *c);

EwlBool      ewl_container_get_propagate_events(EwlWidget *c);
void         ewl_container_set_propagate_events(EwlWidget *c, EwlBool v);

void         ewl_container_render_children(EwlWidget *c);
void         ewl_container_resize_children(EwlWidget *c);

/* depcirted -- will be replaced with ewlrect calls soon */
void         ewl_container_set_child_padding(EwlWidget *c, int *l, int *t,
                                                           int *r, int *b);
void         ewl_container_get_child_padding(EwlWidget *c, int *l, int *t,
                                                           int *r, int *b);

/* private */
EwlBool      _cb_ewl_container_render_children(EwlLL *node, EwlData *data);

#endif /* _EWL_CONTAINER_H_ */
