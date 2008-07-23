/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "edje_frontend.h"
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Resize.h>
#include <Esmart/Esmart_Draggies.h>

/* internal functions */
static void _eli_app_gui_swallow(Evas_Object * gui, 
                          const char * part, Evas_Object * o);
static void _eli_edje_frontend_ee_init(Eli_App * eap, Eli_Edje_Frontend * eef);
static void _eli_edje_frontend_resize(Eli_App * eap, int w, int h);
static void _eli_edje_frontend_cards_container_fill(Eli_App * eap, 
						Evas_Object * container);
static void _eli_edje_frontend_theme_container_fill(Eli_App * eap, 
		                                Evas_Object * container);


/* callback  declaration */
static void win_del_cb(Ecore_Evas * ee);
static void win_resize_cb(Ecore_Evas * ee);

static void _context_menu_open(void * data, Evas * e, Evas_Object * obj,
	    		  void * event_info);
static void _quit_cb(void * data, Evas_Object * o, 
                          const char * emission, const char * source);
static void _undo_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _restart_cb(void * data, Evas_Object * o, 
                          const char * emission, const char * source);
static void _hints_cb(void * data, Evas_Object * o, 
                          const char * emission, const char * source);
static void _elitaire_scroll_stop_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _elitaire_scroll_left_start_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _elitaire_scroll_right_start_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _elitaire_scroll_left_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _elitaire_scroll_right_cb(void * data, Evas_Object * o,
                          const char * emission, const char * source);
static void _key_down_cb(void * data, Evas * e, Evas_Object * obj,
                          void * event_info);
static void _key_up_cb(void * data, Evas * e, Evas_Object * obj,
                          void * event_info);

/*
 * frontend initialation
 */
void eli_frontend_edje_init(Eli_App * eap, Eli_Frontend *ef)
{
    Eli_Edje_Frontend * eef;
    
    ef->init = NULL;
    ef->del = _eli_edje_frontend_del;
    ef->gui_make = _eli_edje_frontend_gui_make;
    ef->game_new = _eli_edje_frontend_game_new;
    ef->game_end = _eli_edje_frontend_game_end;
    ef->game_kill = _eli_edje_frontend_game_kill;
    ef->state_notify = _eli_edje_frontend_state_notify;
    ef->config_changed = _eli_edje_frontend_config_changed;
    
    eef = (Eli_Edje_Frontend *) calloc(1, sizeof(Eli_Edje_Frontend));
    ef->data = eef;

    eef->ee = NULL;
    eef->elitaire = NULL;
    eef->evas = NULL;
    eef->gui = NULL;
    eef->key.ctrl_down = false;
    eef->key.alt_down = false;
    eef->conmenu = NULL;
    
    _eli_edje_frontend_ee_init(eap, eef);
}

void _eli_edje_frontend_del(Eli_App * eap)
{
     if (eap && eap->frontend)
	free(eap->frontend);
}

void _eli_edje_frontend_state_notify(Eli_App * eap)
{
#if 0
    switch (eap->state) {
        case ELI_STATE_PLAYING:
	    printf("playing\n");
	    break;
	case ELI_STATE_GAME_NONE:
	    printf("none\n");
	    break;
	case ELI_STATE_GAME_KILL:
	    printf("kill\n");
	    break;
	case ELI_STATE_GAME_END:
	    printf("end\n");
	    break;
	case ELI_STATE_HIGHSCORE:
	    printf("highscore\n");
	    break;
	default:
	    printf("rest\n");
	    break;
    }
#endif
}	    

/* init Ecore_Evas stuff */
static void _eli_edje_frontend_ee_init(Eli_App * eap, Eli_Edje_Frontend * eef)
{
    Ecore_Evas * ee;

    if (ecore_config_boolean_get("/graphic/gl")
        && ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_OPENGL_X11)) {
        ee = ecore_evas_gl_x11_new(NULL, 0, 0, 0, 0, 0);
        eap->main_win = (void *) ecore_evas_gl_x11_window_get(ee);
    }
    else {
        ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
	eap->main_win = (void *) ecore_evas_software_x11_window_get(ee);
    }

    ecore_evas_title_set(ee, "Elitaire");
    ecore_evas_name_class_set(ee, "Elitaire", "Elitaire");
    ecore_evas_data_set(ee, "eap", eap);

    ecore_evas_show(ee);
    ecore_evas_callback_delete_request_set(ee, win_del_cb);
    ecore_evas_callback_resize_set(ee, win_resize_cb);

    eef->evas = ecore_evas_get(ee);
    eef->ee = ee;
}

/* 
 * two callbacks for Ecore_Evas
 */
static void win_del_cb(Ecore_Evas * ee)
{
    if (ee) {
        Eli_App * eap;
        Eli_Edje_Frontend * eef;

        eap = (Eli_App *) ecore_evas_data_get(ee, "eap");
        eef = eli_app_edje_frontend_get(eap);
	
        if (eef->elitaire) {
            eap->next.game = strdup("exit");
            elitaire_object_giveup(eef->elitaire);
        }
        else ecore_main_loop_quit();
    }
    else ecore_main_loop_quit();
}

static void win_resize_cb(Ecore_Evas * ee)
{
    if (ee) {
        Evas_Coord w, h;
        Eli_App * eap;

        ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
        eap = (Eli_App *) ecore_evas_data_get(ee, "eap");
        _eli_edje_frontend_resize(eap, w, h);
    }
}

/* externals */
void _eli_edje_frontend_gui_make(Eli_App * eap)
{
    Evas * evas;
    Eli_Edje_Frontend * eef;
    Evas_Coord w, h;
    char * gui_theme;
    const char * shaped;
    const char * borderless;
    int is_shaped;

    is_shaped = 0;
    eef = eli_app_edje_frontend_get(eap);
    evas = eef->evas;
    eef->gui = edje_object_add(evas);
    gui_theme =
        ecore_config_theme_with_path_from_name_get(eap->theme.gui.current);

    if (edje_object_file_set(eef->gui, gui_theme, "main")) {
        evas_object_move(eef->gui, 0, 0);
        evas_object_repeat_events_set(eef->gui, 1);
        edje_object_size_min_get(eef->gui, &w, &h);
        evas_object_resize(eef->gui, w, h);
        evas_object_focus_set(eef->gui, 1);
        edje_object_signal_callback_add(eef->gui, "quit", "*", _quit_cb, 
					eef->ee);
        edje_object_signal_callback_add(eef->gui, "undo", "*", _undo_cb, eef);
        edje_object_signal_callback_add(eef->gui, "restart", "*", _restart_cb,
                                        eef);
        edje_object_signal_callback_add(eef->gui, "new_game", "*",
                                        _eli_edje_frontend_new_game_cb, eap);
        edje_object_signal_callback_add(eef->gui, "hints", "*", _hints_cb, eef);
        evas_object_event_callback_add(eef->gui, EVAS_CALLBACK_KEY_DOWN,
                                       _key_down_cb, eap);
        evas_object_event_callback_add(eef->gui, EVAS_CALLBACK_KEY_UP,
                                       _key_up_cb, eap);
        evas_object_show(eef->gui);
    }
    else {
        fprintf(stderr, _("Elitaire Error: unable to find %s theme!\n"),
                gui_theme);
        return;
    }

    /* some ecore_evas stuff */
    shaped = edje_object_data_get(eef->gui, "shaped");
    borderless = edje_object_data_get(eef->gui, "borderless");

    if (!shaped) ecore_evas_shaped_set(eef->ee, 0);
    else if (!strcmp("1", shaped)) {
        ecore_evas_shaped_set(eef->ee, 1);
        is_shaped = 1;
    }
    else ecore_evas_shaped_set(eef->ee, 0);

    if (!borderless)
        ecore_evas_borderless_set(eef->ee, 0);
    else if (!strcmp("1", borderless)) 
	ecore_evas_borderless_set(eef->ee, 1);
    else 
	ecore_evas_borderless_set(eef->ee, 0);

    ecore_evas_resize(eef->ee, w, h);
    ecore_evas_size_min_set(eef->ee, w, h);

    /* the card bar */
    if (edje_object_part_exists(eef->gui, "elitaire_cards_box")) {
        Evas_Object * container;

        container = esmart_container_new(evas);
        esmart_container_direction_set(container,
                                       CONTAINER_DIRECTION_VERTICAL);
        esmart_container_alignment_set(container, CONTAINER_ALIGN_TOP);
        evas_object_show(container);

        _eli_app_gui_swallow(eef->gui, "elitaire_cards_box", container);
        _eli_edje_frontend_cards_container_fill(eap, container);

        /* for timer based scrolling, i.e. pressed button scrolling */
        edje_object_signal_callback_add(eef->gui, "scroll,stop",
                                        "elitaire_cards_box",
                                        _elitaire_scroll_stop_cb, container);
        edje_object_signal_callback_add(eef->gui, "scroll,left,start",
                                        "elitaire_cards_box",
                                        _elitaire_scroll_left_start_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right,start",
                                        "elitaire_cards_box",
                                        _elitaire_scroll_right_start_cb,
                                        container);

        /* for interval scrolling, i.e. mouse wheel */
        edje_object_signal_callback_add(eef->gui, "scroll,left",
                                        "elitaire_cards_box",
                                        _elitaire_scroll_left_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right",
                                        "elitaire_cards_box",
                                        _elitaire_scroll_right_cb,
                                        container);
    }

    /* the theme bar */
    if (edje_object_part_exists(eef->gui, "elitaire_theme_box")) {
        Evas_Object * container;

        container = esmart_container_new(evas);
        esmart_container_direction_set(container,
                                       CONTAINER_DIRECTION_VERTICAL);
        esmart_container_alignment_set(container, CONTAINER_ALIGN_TOP);
        evas_object_show(container);

        _eli_app_gui_swallow(eef->gui, "elitaire_theme_box", container);
        _eli_edje_frontend_theme_container_fill(eap, container);

        /* for timer based scrolling, i.e. pressed button scrolling */
        edje_object_signal_callback_add(eef->gui, "scroll,stop",
                                        "elitaire_theme_box",
                                        _elitaire_scroll_stop_cb, container);
        edje_object_signal_callback_add(eef->gui, "scroll,left,start",
                                        "elitaire_theme_box",
                                        _elitaire_scroll_left_start_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right,start",
                                        "elitaire_theme_box",
                                        _elitaire_scroll_right_start_cb,
                                        container);

        /* for interval scrolling, i.e. mouse wheel */
        edje_object_signal_callback_add(eef->gui, "scroll,left",
                                        "elitaire_theme_box",
                                        _elitaire_scroll_left_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right",
                                        "elitaire_theme_box",
                                        _elitaire_scroll_right_cb,
                                        container);
    }

    /*the menu bar */
    if (edje_object_part_exists(eef->gui, "elitaire_new_box")) {
        Evas_Object * container;

        container = esmart_container_new(evas);
        esmart_container_direction_set(container,
                                       CONTAINER_DIRECTION_VERTICAL);
        esmart_container_alignment_set(container, CONTAINER_ALIGN_TOP);
        evas_object_show(container);

        _eli_app_gui_swallow(eef->gui, "elitaire_new_box", container);
        _eli_edje_frontend_menu_container_fill(eap, container);

        edje_object_signal_callback_add(eef->gui, "scroll,stop",
                                        "elitaire_new_box",
                                        _elitaire_scroll_stop_cb, container);
        edje_object_signal_callback_add(eef->gui, "scroll,left,start",
                                        "elitaire_new_box",
                                        _elitaire_scroll_left_start_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right,start",
                                        "elitaire_new_box",
                                        _elitaire_scroll_right_start_cb,
                                        container);
        
        /* for interval scrolling, i.e. mouse wheel */
        edje_object_signal_callback_add(eef->gui, "scroll,left",
                                        "elitaire_new_box",
                                        _elitaire_scroll_left_cb,
                                        container);
        edje_object_signal_callback_add(eef->gui, "scroll,right",
                                        "elitaire_new_box",
                                        _elitaire_scroll_right_cb,
                                        container);

    }
    /* theme parts for moving the window */
    if (edje_object_part_exists(eef->gui, "elitaire_drag")) {
        Evas_Object * o;
        
        o = esmart_draggies_new(eef->ee);
        esmart_draggies_button_set(o, 1);
        _eli_app_gui_swallow(eef->gui, "elitaire_drag", o);
    }

    const char *resize_types[] = {
        "elitaire_resize_left",
        "elitaire_resize_right",
        "elitaire_resize_top",
        "elitaire_resize_bottom",
        "elitaire_resize_left_top",
        "elitaire_resize_right_top",
        "elitaire_resize_left_bottom",
        "elitaire_resize_right_bottom"
    };

    Esmart_Resize_Type rt[] = {
        ESMART_RESIZE_LEFT,
        ESMART_RESIZE_RIGHT,
        ESMART_RESIZE_TOP,
        ESMART_RESIZE_BOTTOM,
        ESMART_RESIZE_LEFT_TOP,
        ESMART_RESIZE_RIGHT_TOP,
        ESMART_RESIZE_LEFT_BOTTOM,
        ESMART_RESIZE_RIGHT_BOTTOM
    };

    /* theme parts for resizing the window */
    for (int i = 0; i < 8; i++) {
        if (edje_object_part_exists(eef->gui, resize_types[i])) {
            Evas_Object * o;
            
            o = esmart_resize_new(eef->ee);
            esmart_resize_button_set(o, 1);
            esmart_resize_type_set(o, rt[i]);
            _eli_app_gui_swallow(eef->gui, resize_types[i], o);
        }
    }

    /* theme parts for the context menu */
    if (edje_object_part_exists(eef->gui, "elitaire_context_menu")) {
        Evas_Object * o;

	o = evas_object_rectangle_add(evas);
	evas_object_color_set(o, 0, 0, 0, 0);
        _eli_app_gui_swallow(eef->gui, "elitaire_context_menu", o);
    
	eef->conmenu = eli_edje_frontend_conmenu_new(eap);
        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                       _context_menu_open, eef);
    }

    if (eef->elitaire)
        _eli_app_gui_swallow(eef->gui, "elitaire_card_field", eef->elitaire);

    /* set the label text */
    const char * labels[] = {N_("Games"), N_("Themes"), N_("Cards"), NULL};
    const char * label_parts[] = {"elitaire_new_box_label",
                                "elitaire_theme_box_label",
                                "elitaire_cards_box_label", NULL};
    for (int i = 0; labels[i] != NULL;  i++)
        edje_object_part_text_set(eef->gui, label_parts[i], _(labels[i]));
}

void _eli_edje_frontend_gui_del(Eli_App * eap)
{
    Eli_Edje_Frontend * eef; 

    const char * unsw[] = {
        "elitaire_context_menu",
        "elitaire_drag",
        "elitaire_resize_left",
        "elitaire_resize_right",
        "elitaire_resize_top",
        "elitaire_resize_bottom",
        "elitaire_resize_left_top",
        "elitaire_resize_right_top",
        "elitaire_resize_left_bottom",
        "elitaire_resize_right_bottom",
        "elitaire_cards_box",
        "elitaire_theme_box",
        "elitaire_new_box"
    };

    eef = eli_app_edje_frontend_get(eap);

    for (int i = 0; i < 13; i++) {
        Evas_Object * o;
        
        o = edje_object_part_swallow_get(eef->gui, unsw[i]);
        if (o) {
            edje_object_part_unswallow(eef->gui, o);
            evas_object_del(o);
        }
    }

    if (edje_object_part_swallow_get(eef->gui, "elitaire_card_field"))
        edje_object_part_unswallow(eef->gui, eef->elitaire);

    evas_object_del(eef->gui);
    eef->gui = NULL;
}

static void _eli_edje_frontend_resize(Eli_App * eap, int w, int h)
{
    evas_object_resize(eli_app_edje_frontend_get(eap)->gui, 
                          (Evas_Coord) w, (Evas_Coord) h);
}

/* internal */
static void _eli_app_gui_swallow(Evas_Object * gui, const char *part,
                          Evas_Object * o)
{
    Evas_Coord x, y, w, h;

    edje_object_part_geometry_get(gui, part, &x, &y, &w, &h);
    evas_object_move(o, x, y);
    evas_object_resize(o, w, h);
    evas_object_show(o);
    edje_object_part_swallow(gui, part, o);
}

/* callbacks */
static void _context_menu_open(void * data, Evas * e, Evas_Object * obj,
				void * event_info)
{
    Evas_Event_Mouse_Down * ev;
    Eli_Edje_Frontend * eef; 
    int x, y;

    eef = (Eli_Edje_Frontend *) data;
    ev = (Evas_Event_Mouse_Down *) event_info;

    if (ev->button != 3)
	return;

   /* fetch the position of the window */ 
    ecore_evas_geometry_get(eef->ee, &x, &y, NULL, NULL);
    x += (Evas_Coord) ev->canvas.x;
    y += (Evas_Coord) ev->canvas.y;

    /* place the context menu */
    ewl_popup_mouse_position_set(EWL_POPUP(eef->conmenu), x, y);
    ewl_widget_show(eef->conmenu);
    ewl_widget_focus_send(eef->conmenu);
}

static void _quit_cb(void * data, Evas_Object * o, const char * emission,
              const char * source)
{
    win_del_cb((Ecore_Evas *) data);
}

static void _undo_cb(void * data, Evas_Object * o, const char * emission,
              const char * source)
{
    Eli_Edje_Frontend * eef;

    eef = (Eli_Edje_Frontend *) data;

    if (!eef) return;
    if (eef->elitaire) elitaire_object_undo(eef->elitaire);
}

static void _restart_cb(void * data, Evas_Object * o, const char * emission,
                 const char * source)
{
    Eli_Edje_Frontend * eef;

    eef = (Eli_Edje_Frontend *) data;

    if (!eef) return;
    if (eef->elitaire) elitaire_object_restart(eef->elitaire);
}

static void _hints_cb(void * data, Evas_Object * o, const char * emission,
                 const char * source)
{
    Eli_Edje_Frontend * eef;

    eef = (Eli_Edje_Frontend *) data;

    if (!eef && !eef->elitaire) return;
    if (elitaire_object_hints_visible(eef->elitaire)) 
        elitaire_object_hints_hide(eef->elitaire);
    else
        elitaire_object_hints_show(eef->elitaire);
}

static void _elitaire_scroll_stop_cb(void * data, Evas_Object * o,
                              const char * emission, const char * source)
{
    Evas_Object * container;

    container = (Evas_Object *) data;
    esmart_container_scroll_stop(container);
}

static void _elitaire_scroll_left_start_cb(void * data, Evas_Object * o,
                                    const char * emission, const char * source)
{
    Evas_Object * container;

    container = (Evas_Object *) data;
    esmart_container_scroll_start(container, -2.0);
}

static void _elitaire_scroll_right_start_cb(void * data, Evas_Object * o,
                                     const char * emission, const char * source)
{
    Evas_Object * container;

    container = (Evas_Object *) data;
    esmart_container_scroll_start(container, 2.0);
}

static void _elitaire_scroll_left_cb(void * data, Evas_Object * o,
                                     const char * emission, const char * source)
{
    Evas_Object * container;

    container = (Evas_Object *) data;
    /* XXX this value shouldn't be hardcoded */
    esmart_container_scroll(container, -10);
}

static void _elitaire_scroll_right_cb(void * data, Evas_Object * o,
                                     const char * emission, const char * source)
{
    Evas_Object * container;

    container = (Evas_Object *) data;
    /* XXX this value shouldn't be hardcoded */
    esmart_container_scroll(container, 10);
}

static void _key_down_cb(void * data, Evas * e, Evas_Object * obj, void * event_info)
{
    Eli_App * eap;
    Eli_Edje_Frontend * eef;
    Evas_Event_Key_Down * ev;

    eap = (Eli_App *) data;
    eef = eli_app_edje_frontend_get(eap);
    ev = (Evas_Event_Key_Down *) event_info;
    
    if (!strcmp(ev->key, "Control_L") || !strcmp(ev->key, "Control_R")) {
        eef->key.ctrl_down = true;
        return;
    }
    else if (!strcmp(ev->key, "Alt_L")) {
        eef->key.alt_down = true;
        return;
    }
    else if (!strcmp(ev->key, "F2"))
        edje_object_signal_emit(eef->gui, "new_game", "");
    else if (!strcmp(ev->key, "F1"))
        eli_app_about_open(eap);

    if (eef->key.ctrl_down && !eef->key.alt_down) {
        if (!strcmp(ev->key, "z")) 
            edje_object_signal_emit(eef->gui, "undo", "");
        else if (!strcmp(ev->key, "r")) 
            edje_object_signal_emit(eef->gui, "restart", "");
        else if (!strcmp(ev->key, "n")) 
            edje_object_signal_emit(eef->gui, "new_game", "");
        else if (!strcmp(ev->key, "a")) {
            if (ecore_evas_alpha_get(eef->ee)) {
                ecore_evas_alpha_set(eef->ee, 0);
                edje_object_signal_emit(eef->gui, "elitaire,alpha,off", "");
            }
            else {
                ecore_evas_alpha_set(eef->ee, 1);
                edje_object_signal_emit(eef->gui, "elitaire,alpha,on", "");
            }
        }
    }
}

static void _key_up_cb(void *data, Evas * e, Evas_Object * obj, 
		                                       void *event_info)
{
    Eli_App * eap;
    Eli_Edje_Frontend * eef;
    Evas_Event_Key_Up * ev;

    eap = (Eli_App *) data;
    eef = eli_app_edje_frontend_get(eap);
    ev = (Evas_Event_Key_Up *) event_info;

    if (!strcmp(ev->key, "Control_L")) eef->key.ctrl_down = false;
    else if (!strcmp(ev->key, "Control_R")) eef->key.ctrl_down = false;
    else if (!strcmp(ev->key, "Alt_L")) eef->key.alt_down = false;
}

static void _theme_item_selected_cb(void * data, Evas_Object * o,
                             const char * emission, const char * source)
{
    const char * theme;

    if ((theme = edje_object_part_text_get(o, "elitaire_element_value"))) 
        ecore_config_theme_set("/theme/gui", theme);
}

static void _cards_item_selected_cb(void * data, Evas_Object * o,
                             const char * emission, const char * source)
{
    const char * theme;

    if ((theme = edje_object_part_text_get(o, "elitaire_element_value")))
        ecore_config_theme_set("/theme/cards", theme);
}

/* Fill the theme container for selecting the theme */
static void _eli_edje_frontend_theme_container_fill(Eli_App * eap, 
		                              Evas_Object * container)
{
    Ecore_List * l;
    Eli_Edje_Frontend * eef;
    char * file;
    char const * theme_name;

    eef = eli_app_edje_frontend_get(eap);

    file = ecore_config_theme_with_path_from_name_get(eap->theme.gui.current);
    l = eap->theme.gui.list;

    ecore_list_first_goto(l);
    while ((theme_name = (const char *) ecore_list_next(l))) {
        Evas_Object * item;

        item = edje_object_add(eef->evas);

        if (edje_object_file_set(item, file, "element")) {
            Evas_Coord w, h;

            edje_object_size_min_get(item, NULL, &h);
            evas_object_geometry_get(container, NULL, NULL, &w, NULL);
            evas_object_resize(item, (int) w, (int) h);

            if (edje_object_part_exists(item, "elitaire_element_value")) {
                edje_object_part_text_set(item, "elitaire_element_value",
                                          theme_name);
                evas_object_show(item);

                edje_object_signal_callback_add(item, "item_selected",
                                                "item_selected",
                                                _theme_item_selected_cb, NULL);

                esmart_container_element_append(container, item);
            }
            else {
                fprintf(stderr,
                        _("Elitaire Error in %s: missing element_value part!\n"),
                        file);
                evas_object_del(item);
            }
        }
        else {
            fprintf(stderr,
                    _("Elitaire Error in %s: missing element group!\n"),
                    file);
            evas_object_del(item);
        }
    }
    ecore_list_first_goto(l);

    if (file) 
        free(file);
}

/* Fill the cards container for selecting the carddeck */
static void _eli_edje_frontend_cards_container_fill(Eli_App * eap, 
		                                     Evas_Object * container)
{
    Ecore_List * l;
    char * file;
    const char * cards_name;

    file = ecore_config_theme_with_path_from_name_get(eap->theme.gui.current);
    l = eap->theme.cards.list;

    ecore_list_first_goto(l);
    while ((cards_name = (const char *) ecore_list_next(l))) {
        Evas_Object * item;

        item = edje_object_add(evas_object_evas_get(container));
        if (edje_object_file_set(item, file, "element")) {
            Evas_Coord w, h;

            edje_object_size_min_get(item, NULL, &h);
            evas_object_geometry_get(container, NULL, NULL, &w, NULL);
            evas_object_resize(item, (int) w, (int) h);

            if (edje_object_part_exists(item, "elitaire_element_value")) {
                edje_object_part_text_set(item, "elitaire_element_value",
                                          cards_name);
                evas_object_show(item);

                edje_object_signal_callback_add(item, "item_selected",
                                                "item_selected",
                                                _cards_item_selected_cb, NULL);

                esmart_container_element_append(container, item);
            }
            else {
                fprintf(stderr,
                       _("Elitaire Error in %s: missing element_value part!\n"),
                        file);
                evas_object_del(item);
            }
        }
        else {
            fprintf(stderr,
                    _("Elitaire Error in %s: missing element group!\n"),
                    file);
            evas_object_del(item);
        }
    }
    ecore_list_first_goto(l);

    if (file)
        free(file);
}

void eli_edje_frontend_theme_offset_get(Eli_App * eap, 
		              Evas_Coord * l, Evas_Coord * r,
                              Evas_Coord * t, Evas_Coord * b)
{
    Eli_Edje_Frontend * eef;
    const char * data;

    eef = eli_app_edje_frontend_get(eap);

    data = edje_object_data_get(eef->gui, "offset/left");
    if (data) *l = atoi(data);
    else *l = 0;

    data = edje_object_data_get(eef->gui, "offset/right");
    if (data) *r = atoi(data);
    else *r = 0;

    data = edje_object_data_get(eef->gui, "offset/top");
    if (data) *t = atoi(data);
    else *t = 0;

    data = edje_object_data_get(eef->gui, "offset/bottom");
    if (data) *b = atoi(data);
    else *b = 0;

}

