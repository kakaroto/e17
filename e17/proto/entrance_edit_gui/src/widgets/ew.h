#ifndef _EW_H
#define _EW_H

typedef struct _Entrance_Widget {
	Etk_Widget *owner;
	Etk_Widget *box;
	Etk_Widget *label;
	Etk_Widget *dialog_hbox;
	Etk_Widget *entry_control;
	
	Etk_Tree_Col *list_col;
	Evas_Object *preview_smart;
	void *extra;
} *Entrance_Widget;

#define _EW_FREE(ew) if(ew) \
{ \
	etk_object_destroy(ETK_OBJECT(ew)); \
	free(ew); \
}

#define EW_FREE(ew) if(ew) \
{ \
		if(ew->owner) { _EW_FREE(ew->owner); } \
		if(ew->box) { _EW_FREE(ew->box); } \
		if(ew->label) { _EW_FREE(ew->label); } \
		if(ew->dialog_hbox) { _EW_FREE(ew->dialog_hbox); } \
		if(ew->entry_control) { _EW_FREE(ew->entry_control); } \
		if(ew->list_col) { _EW_FREE(ew->list_col); } \
		if(ew->preview_smart) { _EW_FREE(ew->preview_smart); } \
		if(ew->extra) { free(ew->extra); } \
		free(ew); \
}

#define EW_FALSE 0
#define EW_TRUE 1

int ew_init(int *, char ***);
int ew_shutdown(void);
void ew_main(void);
void ew_main_quit(void);

Entrance_Widget ew_new(void);
void ew_widget_destroy(Entrance_Widget);
void ew_widget_extra_set(Entrance_Widget, void *);
void *ew_widget_extra_get(Entrance_Widget);

#endif
