#include "tabpanel.h"

struct tabpanel
{
	Evas_Object *tabs;
	Evas_Object *panels;

	Eina_List *items;
};

struct tabpanel_item
{
	Tabpanel *tab;

	Elm_Toolbar_Item *item_tb;
	Evas_Object *content;
	const char *signal;

	Eina_Bool del;

	TabLanelSelectCB select_cb;
	void *data;
};


static void _tb_select_cb(void *data, Evas_Object *obj, void *event_info)
{
	Tabpanel_Item *item = data;

	if(item->signal)
		edje_object_signal_emit(item->content, item->signal, "");
	else
		elm_pager_content_promote(item->tab->panels, item->content);
	if(item->select_cb)
		item->select_cb(item->data, item->tab, item);
}

static void _hide_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
	Tabpanel *tab = data;
	Eina_List *l;
	Evas_Object *content = event_info;
	Tabpanel_Item *item;

	EINA_LIST_FOREACH(tab->items, l, item)
	{
		if(item->content == content)
			break;
	}

	if(item && item->del)
	{
		item->tab->items = eina_list_remove(item->tab->items, item);
		if(!item->signal)
			evas_object_del(item->content);
		free(item);
	}
}


Tabpanel *tabpanel_add(Evas_Object *parent)
{
	Tabpanel *tab = calloc(1, sizeof(Tabpanel));

	tab->tabs = elm_toolbar_add(parent);
	elm_toolbar_homogenous_set(tab->tabs, 0);
	tab->panels = elm_pager_add(parent);
	evas_object_size_hint_weight_set(tab->panels ,1.0, 1.0);
	evas_object_size_hint_align_set(tab->panels, -1.0, -1.0);

	elm_object_style_set(tab->panels, "fade_translucide");

	evas_object_smart_callback_add(tab->panels, "hide,finished", _hide_finished_cb, tab);

	return tab;
}

Tabpanel *tabpanel_add_with_edje(Evas_Object *parent, Evas_Object *tabs)
{
	Tabpanel *tab = calloc(1, sizeof(Tabpanel));

	tab->tabs = tabs;
	elm_toolbar_homogenous_set(tab->tabs, 0);
	tab->panels = elm_pager_add(parent);
	evas_object_size_hint_weight_set(tab->panels ,1.0, 1.0);
	evas_object_size_hint_align_set(tab->panels, -1.0, -1.0);

	elm_object_style_set(tab->panels, "fade_translucide");

	evas_object_smart_callback_add(tab->panels, "hide,finished", _hide_finished_cb, tab);

	return tab;
}

Evas_Object *tabpanel_tabs_obj_get(Tabpanel *tab)
{
	return tab->tabs;
}

Evas_Object *tabpanel_panels_obj_get(Tabpanel *tab)
{
	return tab->panels;
}


Tabpanel_Item *tabpanel_item_add(Tabpanel *tab, const char *label, Evas_Object *content,
		TabLanelSelectCB select_cb, void *data)
{
	Tabpanel_Item *item = calloc(1, sizeof(Tabpanel_Item));

	tab->items = eina_list_append(tab->items, item);

	item->tab = tab;
	item->item_tb = elm_toolbar_item_append(tab->tabs, NULL, label,
                                                _tb_select_cb, item);
	item->content = content;
	item->del = EINA_FALSE;
	if(content)
		elm_pager_content_push(tab->panels, content);
	item->data = data;
	item->select_cb = select_cb;

	elm_toolbar_item_selected_set(item->item_tb, EINA_TRUE);
	return item;
}


void tabpanel_item_select(Tabpanel_Item *item)
{	
	elm_toolbar_item_selected_set(item->item_tb, EINA_TRUE);
}

Tabpanel_Item *tabpanel_item_add_with_signal(Tabpanel *tab, const char *label, Evas_Object *edje, const char *signal,
		TabLanelSelectCB select_cb, void *data)
{
	Tabpanel_Item *item = calloc(1, sizeof(Tabpanel_Item));

	tab->items = eina_list_append(tab->items, item);

	item->tab = tab;
	item->item_tb = elm_toolbar_item_append(tab->tabs, NULL, label,
                                                _tb_select_cb, item);
	item->content = edje;
	item->signal = signal;
	item->del = EINA_FALSE;
	item->data = data;
	item->select_cb = select_cb;

	elm_toolbar_item_selected_set(item->item_tb, EINA_TRUE);
	return item;
}


void tabpanel_item_label_set(Tabpanel_Item *item, const char *label)
{	
	elm_toolbar_item_label_set(item->item_tb, label);
}

void tabpanel_del(Tabpanel *tab)
{
	free(tab);
}

void tabpanel_item_del(Tabpanel_Item *item)
{
	elm_toolbar_item_del(item->item_tb);

	if(item->tab->items)
		tabpanel_item_select(eina_list_data_get(item->tab->items));

	item->del = EINA_TRUE;
}

