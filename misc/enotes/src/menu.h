#ifndef MENU_H
#define MENU_H 1

#include <Ewl.h>

typedef struct {
	Ewl_Widget     *win;
	Ewl_Widget     *menu;
	Evas_List      *items;
} Menu;

typedef struct {
	void            (*cb) (void *data);
	void           *data;
	Ewl_Widget     *item;
	void           *menu;
} MenuItem;

/* API */
Menu           *menu_create();
void            menu_delete(Menu * menu);
void            menu_show(Menu * menu);
void            menu_hide(Menu * menu);
void            menu_item_add(Menu * menu, char *name,
			      void (*func) (void *data), void *data);

/* Internal */
void            cb_menu_process(Ewl_Widget * w, void *ev_data, void *data);

#endif
