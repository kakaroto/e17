#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "widget.h"

#define EWL_CONTAINER(a) ((EwlContainer*)a)

typedef struct _EwlContainer EwlContainer;

struct _EwlContainer	{
	EwlWidget  widget;
	EwlList   *children;
};

/* CONTAINER NEW/FREE FUNCTIONS */
EwlWidget *ewl_container_new();
void       ewl_container_init(EwlWidget *widget);
void       ewl_container_free(EwlWidget *widget);

/* CONTAINER INSERT/REMOVE FUNCTIONS */
void       ewl_container_insert(EwlWidget *widget, EwlWidget *child);
void       ewl_container_push(EwlWidget *widget, EwlWidget *child);
void       ewl_container_remove(EwlWidget *widget, EwlWidget *child);

/* CONTAINER FOREACH FUNCTIONS */
void       ewl_container_foreach(EwlWidget *widget,
                                 void     (*cb)(EwlWidget *child,
                                                void      *data),
                                 void      *data);

/* CONTAINER EVENT CALLBACK FUNCTIONS */
void       ewl_container_realize_callback(void      *object,
                                          EwlEvent  *event,
                                          void      *data);
void       ewl_container_unrealize_callback(void      *object,
                                            EwlEvent  *event,
                                            void      *data);
void       ewl_container_show_callback(void      *object,
                                       EwlEvent  *event,
                                       void      *data);
void       ewl_container_hide_callback(void      *object,
                                       EwlEvent  *event,
                                       void      *data);
void       ewl_container_move_callback(void      *object,
                                       EwlEvent  *event,
                                       void      *data);
void       ewl_container_resize_callback(void      *object,
                                         EwlEvent  *event,
                                         void      *data);

/* PRIVATE */
typedef struct {
	void (*callback)(EwlWidget *widget, void *data);
	void  *data;
} EwlContainerPrivateData;

#endif /* _CONTAINER_H_ */
