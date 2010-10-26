#include "ephoto.h"

#if 0
static void _ephoto_preferences_pager_switch(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_preferences_hide(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_preferences_item_change(void *data, Evas_Object *obj, void *event_info);
static void _ephoto_preferences_slideshow_transition(void *data, Evas_Object *obj, void *event_info);

static void
_ephoto_key_pressed(void *data, Evas *e, Evas_Object *obj, void *event_data)
{
	Evas_Event_Key_Down *eku;
	eku = (Evas_Event_Key_Down *)event_data;
	if (!strcmp(eku->key, "Escape"))
		_ephoto_preferences_hide(data, NULL, NULL);
}

void
ephoto_show_preferences(Ephoto *em)
{
        if (!em->prefs_win)
        {
                Evas_Object *o, *tb, *box, *pager, *pg1, *pg2, *pg3, *scr;
                const Eina_List *transitions, *l;
                const char *transition;

                em->prefs_win = o = elm_win_inwin_add(em->win);
                elm_object_style_set(o, "minimal");

                box = elm_box_add(em->prefs_win);
                evas_object_show(box);	
                evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                elm_win_inwin_content_set(o, box);

                tb = elm_toolbar_add(box);
                elm_toolbar_homogenous_set(tb, EINA_FALSE);
                evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0.5);

                elm_box_pack_end(box, tb);
                evas_object_show(tb);

                pager = elm_pager_add(box);

                pg1 = elm_box_add(pager);
                evas_object_size_hint_weight_set(pg1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_show(pg1);
                elm_pager_content_push(pager, pg1);
                evas_object_data_set(pg1, "pager", pager);
                elm_toolbar_item_append(tb, NULL, "General", _ephoto_preferences_pager_switch, pg1);

                elm_box_pack_end(box, pager);
                evas_object_show(pager);

                pg2 = elm_table_add(pager);
                evas_object_size_hint_weight_set(pg2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_show(pg2);
                elm_pager_content_push(pager, pg2);
                evas_object_data_set(pg2, "pager", pager);
                elm_toolbar_item_append(tb, NULL, "Slideshow",  _ephoto_preferences_pager_switch, pg2);

                o = elm_label_add(pg2);
                elm_label_label_set(o, "Delay:");
                evas_object_show(o);
                elm_table_pack(pg2, o, 0, 0, 1, 1);

                o = elm_spinner_add(pg2);
                elm_spinner_label_format_set(o, "%1.1f seconds");
                elm_spinner_step_set(o, 0.1);
                elm_spinner_min_max_set(o, 1.0, 10.0);
                elm_spinner_value_set(o, em->config->slideshow_timeout);
                evas_object_data_set(o, "config", "slideshow_timeout");
                evas_object_smart_callback_add(o, "delay,changed",
                                               _ephoto_preferences_item_change, em);
                evas_object_show(o);
                elm_table_pack(pg2, o, 1, 0, 1, 1);

                o = elm_label_add(pg2);
                elm_label_label_set(o, "Transition:");
                evas_object_show(o);
                elm_table_pack(pg2, o, 0, 1, 1, 1);

                o = elm_hoversel_add(pg2);
                elm_hoversel_hover_parent_set(o, em->win);
                elm_hoversel_label_set(o, em->config->slideshow_transition);
                transitions = elm_slideshow_transitions_get(em->slideshow);
                EINA_LIST_FOREACH(transitions, l, transition)
                {
                        elm_hoversel_item_add(o, transition, NULL, ELM_ICON_NONE, _ephoto_preferences_slideshow_transition, em);
                }
                evas_object_show(o);
                elm_table_pack(pg2, o, 1, 1, 1, 1);

                pg3 = elm_box_add(pager);
                evas_object_size_hint_weight_set(pg3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_show(pg3);
                elm_pager_content_push(pager, pg3);
                evas_object_data_set(pg3, "pager", pager);
                elm_toolbar_item_append(tb, NULL, "External Editor", _ephoto_preferences_pager_switch, pg3);

                o = elm_label_add(pg3);
                elm_label_label_set(o, "Image editor:");
                elm_box_pack_end(pg3, o);
                evas_object_show(o);

                scr = elm_scroller_add(pg3);
                elm_box_pack_end(pg3, scr);
                elm_scroller_bounce_set(scr, EINA_TRUE, EINA_FALSE);
                evas_object_size_hint_weight_set(scr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_size_hint_align_set(scr, EVAS_HINT_FILL, EVAS_HINT_FILL);
                evas_object_show(scr);

                o = elm_entry_add(pg3);
                elm_entry_single_line_set(o, EINA_TRUE);
                evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_size_hint_align_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
                elm_entry_entry_set(o, em->config->editor);
                evas_object_data_set(o, "config", "editor");
                evas_object_smart_callback_add(o, "changed",
                                               _ephoto_preferences_item_change, em);
                elm_scroller_content_set(scr, o);
                evas_object_show(o);

                o = elm_button_add(box);
                elm_button_label_set(o, "Close");
                evas_object_size_hint_align_set(o, 1.0, 0.5);
                evas_object_smart_callback_add(o, "clicked", _ephoto_preferences_hide, em);
                elm_box_pack_end(box, o);
                evas_object_show(o);


                evas_object_event_callback_add(em->prefs_win, EVAS_CALLBACK_KEY_DOWN,
                                       _ephoto_key_pressed, em);

                elm_toolbar_item_selected_set(elm_toolbar_first_item_get(tb),
                                              EINA_TRUE);
                elm_pager_content_promote(pager, pg1);
        }

        elm_win_inwin_activate(em->prefs_win);
        evas_object_focus_set(em->prefs_win, EINA_TRUE);
}

static void
_ephoto_preferences_pager_switch(void *data, Evas_Object *obj, void *event_info)
{
        Evas_Object *o = data;
        Evas_Object *pager = evas_object_data_get(o, "pager");

        elm_pager_content_promote(pager, o);
}

static void
_ephoto_preferences_item_change(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto *em = data;
        const char *key = evas_object_data_get(obj, "config");

        if (!strcmp(key, "slideshow_timeout"))
                em->config->slideshow_timeout = elm_spinner_value_get(obj);
        else if (!strcmp(key, "editor"))
                eina_stringshare_replace(
                    &em->config->editor,
                    eina_stringshare_add(elm_entry_entry_get(obj)));

        ephoto_config_save(em, EINA_FALSE);
}

static void
_ephoto_preferences_slideshow_transition(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto *em = data;
        Elm_Hoversel_Item *it = event_info;
        const char *transition = elm_hoversel_item_label_get(it);
        
        elm_hoversel_label_set(obj, transition);
        em->config->slideshow_transition = transition;
        ephoto_config_save(em, EINA_FALSE);
}

static void
_ephoto_preferences_hide(void *data, Evas_Object *obj, void *event_info)
{
        Ephoto *em = data;

        evas_object_hide(em->prefs_win);
        evas_object_focus_set(em->thumb_browser, EINA_TRUE);
}

#endif
