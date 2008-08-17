#include "note.h"
#include "menu.h"

/* API */

Menu           *
menu_create()
{
	Menu           *p = malloc(sizeof(Menu));

	if (p == NULL)
		return (NULL);

	p->win = ewl_window_new();
	ewl_window_move((Ewl_Window *) p->win, 1, 1);

	p->menu = ewl_imenu_new();
	ewl_button_label_set(EWL_BUTTON(p->menu), "Note Menu");
	ewl_container_child_append((Ewl_Container *) p->win, p->menu);

	return (p);
}

void
menu_delete(Menu * menu)
{
	if (menu != NULL)
		free(menu);
	if (menu->items != NULL) {
		Evas_List      *p = menu->items;

		while (evas_list_data(p) != NULL) {
			free((MenuItem *) evas_list_data(p));
			p = evas_list_next(p);
		}
		evas_list_free(menu->items);
	}
	return;
}

void
menu_show(Menu * menu)
{
	ewl_widget_show(menu->win);
	ewl_callback_call(menu->menu, EWL_CALLBACK_FOCUS_IN);
	return;
}

void
menu_hide(Menu * menu)
{
	ewl_widget_hide(menu->win);
}

void
menu_item_add(Menu * menu, char *name, void (*func) (void *data), void *data)
{
	Evas_List      *p;
	MenuItem       *item = malloc(sizeof(MenuItem));

	p = evas_list_append(evas_list_last(menu->items), (void *) item);

	item->cb = func;
	item->data = data;

	item->item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item->item), name);
	ewl_container_child_append((Ewl_Container *) menu->menu, item->item);
	ewl_callback_append(item->item, EWL_CALLBACK_FOCUS_IN, cb_menu_process,
			    item);
	ewl_widget_show(item->item);

	item->menu = (void *) menu;
}

/* Internal */

void
cb_menu_process(Ewl_Widget * w, void *ev_data, void *data)
{
	MenuItem       *item = (MenuItem *) item;

//      (item->cb)((void*)item->data);
	menu_hide((Menu *) item->menu);
	menu_delete((Menu *) item->menu);
	item = NULL;
	return;
}
