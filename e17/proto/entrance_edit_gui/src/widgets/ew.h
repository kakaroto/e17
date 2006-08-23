#ifndef _EW_H
#define _EW_H

typedef struct _Entrance_Widget {
	Etk_Widget *owner;
	Etk_Widget *box;
	Etk_Widget *hbox;
	Etk_Widget *label;
	Etk_Widget *control;
	
	Etk_Tree_Col *col;
	Evas_Object *preview_smart;
	void *extra;
} *Entrance_Widget;


#define EW_FREE(ew) if(1) \
{ \
	if(ew) \
	{ \
		if(ew->owner) {	free(ew->owner); } \
		if(ew->box) { free(ew->box); } \
		if(ew->hbox) { free(ew->hbox); } \
		if(ew->label) { free(ew->label); } \
		if(ew->control) { free(ew->control); } \
		if(ew->col) { free(ew->col); } \
		if(ew->preview_smart) { free(ew->preview_smart); } \
		if(ew->extra) { free(ew->extra) } \
		free(ew); \
	} \
} \
else \

#define EW_FALSE 0
#define EW_TRUE 1

int ew_init(int *, char ***);
int ew_shutdown(void);
void ew_main(void);
void ew_main_quit(void);

Entrance_Widget ew_new(void);
void ew_widget_destroy(Entrance_Widget);

#endif
