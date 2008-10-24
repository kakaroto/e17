#include "Entangle.h"

#include <limits.h>
#include <string.h>

#define WIDTH  400
#define HEIGHT 550
#define DIR_ENTRY_W 150
#define DIR_ENTRY_H 24

static Ecore_Evas *ee;
static void entangle_ui_cb_resize(Ecore_Evas *ee);
static int entangle_ui_eapps_bar_populate(Evas_Object *o);
static int entangle_ui_menu_populate(const char *name, 
                                        Evas_Object *container);
static int entangle_ui_menu_populate_list(const char *name, Ecore_List *apps, 
                                        Evas_Object *container);

/* container scroll callbacks */
static void entangle_ui_cb_scroll_up(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_scroll_down(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_up_pressed(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_up_released(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_down_pressed(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_down_released(void *data, Evas_Object *obj,
                                const char *emission, const char *src);

static void entangle_ui_cb_menu_up_pressed(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_up_released(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_down_pressed(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_down_released(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_drag(void *data, Evas_Object *obj,
                                const char *emission, const char *src);

/* container element mouse over/out callbacks */
static void entangle_ui_cb_mouse_in(void *data, Evas *evas, 
                                    Evas_Object *obj, void *ev);
static void entangle_ui_cb_mouse_out(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_mouse_down(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_mouse_up(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_mouse_move(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_move_intercept(void *data, Evas_Object *obj,
                                                Evas_Coord x, Evas_Coord y);
static void entangle_ui_cb_resize_intercept(void *data, Evas_Object *obj,
                                                Evas_Coord w, Evas_Coord h);
static void entangle_ui_cb_show_intercept(void *data, Evas_Object *obj);
static void entangle_ui_cb_hide_intercept(void *data, Evas_Object *obj);

static void entangle_ui_cb_dir_clicked(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_item_move_start(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_item_move(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_menu_item_move_end(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static void entangle_ui_cb_menu_item_delete(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_change(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_menu_item_free(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev);
static Evas_Object *entangle_ui_button_get(Evas *evas, const char *label, 
                                            const char *name);
static void entangle_ui_cmd_bar_init(Evas_Object *container);
static void entangle_ui_button_bar_init(Evas_Object *container);
static int entangle_ui_calc_container_position(Evas_Object *container, 
                                                        Evas_Coord y);
static int entangle_ui_position_compare(Evas_Object *o, Evas_Coord cur_y);
static int entangle_ui_list_find(Eina_List *list, Evas_Coord at_y);
static int entangle_ui_list_find_rec(Eina_List *list, Evas_Coord at_y, 
                                                    int start, int end);

static Evas_Object *entangle_ui_cmd_bar_button_get(Evas *evas, const char *label,  
                                    void (*func) (void *data, Evas_Object *obj, 
                                                    const char *emission, 
                                                    const char *source));
static void entangle_ui_cb_quit(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_save(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_cb_directory_add(void *data, Evas_Object *obj,
                                const char *emission, const char *src);
static void entangle_ui_dir_add_name(void *data, const char *str);

int
entangle_ui_init(const char *display, const char *theme)
{
    Evas *evas;
    Evas_Object *o, *edje;

    ee = ecore_evas_software_x11_new(display, 0, 0, 0, WIDTH, HEIGHT);
    ecore_evas_title_set(ee, "entangle");
    ecore_evas_name_class_set(ee, "entangle", "entangle");
    ecore_evas_callback_resize_set(ee, entangle_ui_cb_resize);
    ecore_evas_show(ee);

    evas = ecore_evas_get(ee);

    edje = edje_object_add(evas);
    if (!edje_object_file_set(edje, theme, "Main"))
    {
        fprintf(stderr, "Unable to locate theme.\n");
        return 0;
    }
    evas_object_move(edje, 0, 0);
    evas_object_resize(edje, WIDTH, HEIGHT);
    evas_object_name_set(edje, "edje");
    evas_object_show(edje);
    
    edje_object_signal_callback_add(edje, "menu,down,clicked",
                    "*", entangle_ui_cb_menu_down_pressed, NULL);
    edje_object_signal_callback_add(edje, "menu,down,release",
                    "*", entangle_ui_cb_menu_down_released, NULL);    
    edje_object_signal_callback_add(edje, "menu,up,clicked",
                    "*", entangle_ui_cb_menu_up_pressed, NULL);
    edje_object_signal_callback_add(edje, "menu,up,release",
                    "*", entangle_ui_cb_menu_up_released, NULL);
    edje_object_signal_callback_add(edje, "drag", 
                    "scroll_title_outline", entangle_ui_cb_menu_drag, NULL);
    
    o = esmart_container_new(evas);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_VERTICAL);
    esmart_container_alignment_set(o, CONTAINER_ALIGN_CENTER);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_NONE);
    esmart_container_spacing_set(o, 10);
    esmart_container_padding_set(o, 5, 5, 5, 5);
    edje_object_part_swallow(edje, "eapps_bar", o);
    evas_object_name_set(o, "eapps_bar");

    if (!entangle_ui_eapps_bar_populate(o))
    {
        fprintf(stderr, "Unable to setup Eapp bar.\n");
        return 0;
    }
    evas_object_show(o);

    o = esmart_container_new(evas);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_VERTICAL);
    esmart_container_alignment_set(o, CONTAINER_ALIGN_CENTER);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);
    esmart_container_spacing_set(o, 0);
    esmart_container_padding_set(o, 2, 2, 2, 2);
    edje_object_part_swallow(edje, "menu_items", o);
    evas_object_name_set(o, "menu");

    if (!entangle_ui_menu_populate("favorite", o))
    {
        fprintf(stderr, "Unable to setup the menu.\n");
        return 0;
    }
    evas_object_show(o);

    o = esmart_container_new(evas);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_HORIZONTAL);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);
    esmart_container_spacing_set(o, 10);
    esmart_container_padding_set(o, 0, 0, 0, 0);
    edje_object_part_swallow(edje, "button_bar", o);
    evas_object_name_set(o, "button_bar");
    entangle_ui_button_bar_init(o);
    evas_object_show(o);

    o = esmart_container_new(evas);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_HORIZONTAL);
    esmart_container_alignment_set(o, CONTAINER_ALIGN_RIGHT);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);
    esmart_container_spacing_set(o, 10);
    esmart_container_padding_set(o, 0, 0, 0, 0);
    edje_object_part_swallow(edje, "cmd_bar", o);
    evas_object_name_set(o, "cmd_bar");
    entangle_ui_cmd_bar_init(o);
    evas_object_show(o);

    return 1;
}

void
entangle_ui_shutdown(void)
{
    if (ee) ecore_evas_free(ee);
}

static void
entangle_ui_cb_resize(Ecore_Evas *ee)
{
    Evas *evas;
    Evas_Coord w, h;
    Evas_Object *edje, *bar;
    double l;
    const char *state;

    evas = ecore_evas_get(ee);
    ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

    edje = evas_object_name_find(evas, "edje");
    evas_object_resize(edje, w, h);

    bar = evas_object_name_find(evas, "eapps_bar");
    edje_object_part_geometry_get(edje, "eapps_bar", NULL, NULL, NULL, &h);
    l = esmart_container_elements_length_get(bar);
    state = edje_object_part_state_get(edje, "up_arrow", NULL);
    if (l < h)
    {
        if (!strcmp(state, "default"))
        {
            edje_object_signal_emit(edje, "arrows,hide", "*");
            edje_object_signal_callback_del(edje, "up,clicked", "*", 
                                            entangle_ui_cb_up_pressed);
            edje_object_signal_callback_del(edje, "down,clicked", "*", 
                                            entangle_ui_cb_down_pressed);
            edje_object_signal_callback_del(edje, "up,click,release", "*", 
                                            entangle_ui_cb_up_released);
            edje_object_signal_callback_del(edje, "down,click,release", "*", 
                                            entangle_ui_cb_down_released);
        }
    }
    else  
    {
        if (!strcmp(state, "hidden"))
        {
            edje_object_signal_emit(edje, "arrows,show", "*");
            edje_object_signal_callback_add(edje, "up,clicked", "*", 
                                            entangle_ui_cb_up_pressed, NULL);
            edje_object_signal_callback_add(edje, "down,clicked", "*", 
                                            entangle_ui_cb_down_pressed, NULL);
            edje_object_signal_callback_add(edje, "up,click,release", "*", 
                                            entangle_ui_cb_up_released, NULL);
            edje_object_signal_callback_add(edje, "down,click,release", "*", 
                                            entangle_ui_cb_down_released, NULL);
        }
    }

    bar = evas_object_name_find(evas, "menu");
    edje_object_part_geometry_get(edje, "menu_items", NULL, NULL, NULL, &h);
    l = esmart_container_elements_length_get(bar);
    state = edje_object_part_state_get(edje, "scroll_title_outline", NULL);

    if (l < h)
    {
        if (!strcmp(state, "default"))
            edje_object_signal_emit(edje, "scroll,buttons,hide", "*");
    }
    else
    {
        if (!strcmp(state, "hidden"))
            edje_object_signal_emit(edje, "scroll,buttons,show", "*");
    }
}
                
static int
entangle_ui_eapps_bar_populate(Evas_Object *container)
{
    Ecore_List *eapps;
    Evas_Object *edje;
    Evas_Coord h;
    Evas *evas;
    int i;
    double l;
    const char *state;

    evas = evas_object_evas_get(container);
    edje = evas_object_name_find(evas, "edje");

    eapps = entangle_eapps_key_list_get();
    if (!eapps) return 0;

    for (i = 0; i < ecore_list_count(eapps); i++)
    {
        Entangle_Eapp *eapp;
        Evas_Object *o, *o2;
        char *key;

        key = ecore_list_index_goto(eapps, i);
        eapp = entangle_eapps_get(key);

        o = edje_object_add(evas);
        if (!edje_object_file_set(o, eapp->path, "icon"))
        {
            fprintf(stderr, "Unable to load Edje from file '%s'.\n", eapp->path);
            evas_object_del(o);
            continue;
        }

        o2 = evas_object_rectangle_add(evas);
        evas_object_data_set(o, "event_layer", o2);
        evas_object_stack_above(o2, o);
        evas_object_repeat_events_set(o2, 1);
        evas_object_color_set(o2, 0, 0, 0, 0);
        evas_object_event_callback_add(o2, EVAS_CALLBACK_MOUSE_IN, 
                                            entangle_ui_cb_mouse_in, eapp);
        evas_object_event_callback_add(o2, EVAS_CALLBACK_MOUSE_OUT, 
                                            entangle_ui_cb_mouse_out, eapp);
        evas_object_event_callback_add(o2, EVAS_CALLBACK_MOUSE_DOWN,
                                            entangle_ui_cb_mouse_down, eapp);

        evas_object_intercept_move_callback_add(o,
                                            entangle_ui_cb_move_intercept, o2);
        evas_object_intercept_resize_callback_add(o,
                                            entangle_ui_cb_resize_intercept, o2);
        evas_object_intercept_hide_callback_add(o,
                                            entangle_ui_cb_hide_intercept, o2);
        evas_object_intercept_show_callback_add(o,
                                            entangle_ui_cb_show_intercept, o2);

        esmart_container_element_append(container, o);
    }

    edje_object_part_geometry_get(edje, "eapps_bar", NULL, NULL, NULL, &h);
    l = esmart_container_elements_length_get(container);
    state = edje_object_part_state_get(edje, "up_arrow", NULL);

    if (l < h) 
        edje_object_signal_emit(edje, "arrows,hide", "*");
    else  
    {
        edje_object_signal_emit(edje, "arrows,show", "*");
        edje_object_signal_callback_add(edje, "up,clicked", "*", entangle_ui_cb_up_pressed, NULL);
        edje_object_signal_callback_add(edje, "down,clicked", "*", entangle_ui_cb_down_pressed, NULL);
        edje_object_signal_callback_add(edje, "up,click,release", "*", entangle_ui_cb_up_released, NULL);
        edje_object_signal_callback_add(edje, "down,click,release", "*", entangle_ui_cb_down_released, NULL);
    }
    
    edje_object_signal_callback_add(edje, "wheel,up", "*", entangle_ui_cb_scroll_up, NULL);
    edje_object_signal_callback_add(edje, "wheel,down", "*", entangle_ui_cb_scroll_down, NULL);
    
    return 1;
}

static int _scroll_count = 0;

static int
entangle_ui_cb_scroll_stop(void *data)
{
   Evas *evas = data;
   Evas_Object *o;

   if (_scroll_count > 0)
      _scroll_count--;
   else if (_scroll_count < 0)
      _scroll_count++;
   
   if (!_scroll_count)
   {
      o = evas_object_name_find(evas, "eapps_bar");
      esmart_container_scroll_stop(o);
   }
   
   return 0;
}
  
static void
entangle_ui_cb_scroll_up(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    if (!_scroll_count)
    {
       esmart_container_scroll_start(o, 4);
    }
    if (_scroll_count < 0)
       _scroll_count = 1;
    else
       _scroll_count++;
    ecore_timer_add(0.25, entangle_ui_cb_scroll_stop, evas);
}

static void
entangle_ui_cb_scroll_down(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    if (!_scroll_count)
    {
       esmart_container_scroll_start(o, -4);
    }
    if (_scroll_count > 0)
       _scroll_count = -1;
    else
       _scroll_count--;
    ecore_timer_add(0.25, entangle_ui_cb_scroll_stop, evas);
}

static void
entangle_ui_cb_up_pressed(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    esmart_container_scroll_start(o, 2);
}

static void
entangle_ui_cb_up_released(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    esmart_container_scroll_stop(o);
}

static void
entangle_ui_cb_down_pressed(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    esmart_container_scroll_start(o, -2);
}

static void
entangle_ui_cb_down_released(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "eapps_bar");
    esmart_container_scroll_stop(o);
}

static void
entangle_ui_cb_menu_up_pressed(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "menu");
    esmart_container_scroll_start(o, 2);        
}


static void
entangle_ui_cb_menu_up_released(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;
    double y;
    
    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "menu");
    esmart_container_scroll_stop(o);

    y =  esmart_container_scroll_percent_get(o);
    o = evas_object_name_find(evas, "edje");
    edje_object_part_drag_value_set(o, "scroll_title_outline", 0, y);

}

static void
entangle_ui_cb_menu_down_pressed(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "menu");
    esmart_container_scroll_start(o, -2);
}

static void
entangle_ui_cb_menu_down_released(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o;
    double y;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "menu");
    esmart_container_scroll_stop(o);

    y = esmart_container_scroll_percent_get(o);
    o = evas_object_name_find(evas, "edje");
    edje_object_part_drag_value_set(o, "scroll_title_outline", 0, y);
}

static void
entangle_ui_cb_menu_drag(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *o; 
    double y;
    
    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "edje");
    edje_object_part_drag_value_get(o, "scroll_title_outline", NULL, &y);

    o = evas_object_name_find(evas, "menu");
    esmart_container_scroll_percent_set(o, y);
}

static void
entangle_ui_cb_mouse_in(void *data, Evas *evas, 
                        Evas_Object *obj __UNUSED__, void *ev)
{
    Entangle_Eapp *eapp;
    Evas_Object *edje;
    Evas_Coord y,h;
    Evas_Event_Mouse_In *e;

    e = ev;
    edje = evas_object_name_find(evas, "edje");
    edje_object_part_geometry_get(edje, "eapps_bar", NULL, &y, NULL, &h);
    
    if ((e->canvas.y > y) && (e->canvas.y < (y + h)))
    {
        eapp = data;
        edje_object_part_text_set(edje, "eapp_info_name", eapp->name);
        edje_object_part_text_set(edje, "eapp_info_class", eapp->class);
        edje_object_signal_emit(edje, "eapp,info,show", "*");
    }
}

static void
entangle_ui_cb_mouse_out(void *data __UNUSED__, Evas *evas,
                        Evas_Object *obj __UNUSED__,  void *ev __UNUSED__)
{
    Evas_Object *edje;

    edje = evas_object_name_find(evas, "edje");
    edje_object_signal_emit(edje, "eapp,info,hide", "*");
}

static void
entangle_ui_cb_move_intercept(void *data, Evas_Object *obj,
                                    Evas_Coord x, Evas_Coord y)
{
    Evas_Object *over;

    over = data;
    evas_object_move(over, x, y);
    evas_object_move(obj, x, y);
}

static void
entangle_ui_cb_resize_intercept(void *data, Evas_Object *obj,
                                    Evas_Coord w, Evas_Coord h)
{
    Evas_Object *over;

    over = data;
    evas_object_resize(over, w, h);
    evas_object_resize(obj, w, h);
}

static void
entangle_ui_cb_show_intercept(void *data, Evas_Object *obj)
{
    Evas_Object *over;

    over = data;
    evas_object_show(over);
    evas_object_show(obj);
}

static void
entangle_ui_cb_hide_intercept(void *data, Evas_Object *obj)
{
    Evas_Object *over;

    over = data;
    evas_object_show(over);
    evas_object_show(obj);
}

static int
entangle_ui_menu_populate(const char *name, Evas_Object *container)
{
    Ecore_List *apps;
    apps = entangle_apps_list_get(name);
    return entangle_ui_menu_populate_list(name, apps, container);
}

static int
entangle_ui_menu_populate_list(const char *rel_path, Ecore_List *apps, 
                                                Evas_Object *container)
{
    Evas *evas;
    int i;
    const char *file;
    Evas_Coord h;
    double l;
    Evas_Object *o;

    evas = evas_object_evas_get(container);

    o = evas_object_name_find(evas, "edje");
    edje_object_file_get(o, &file, NULL);

    evas_object_data_set(container, "list", apps);
    evas_object_data_set(container, "list_rel_path", rel_path);

    for (i = 0; i < ecore_list_count(apps); i++)
    {
        Entangle_App *app;
        Evas_Object *obj;
        Evas_Object *o = NULL;

        app = ecore_list_index_goto(apps, i);
        obj = edje_object_add(evas);
        edje_object_file_set(obj, file, "menu_item");
        edje_object_part_text_set(obj, "text", app->eapp->name);

        edje_object_signal_callback_add(obj, "del,clicked",
                    "*", entangle_ui_cb_menu_item_delete, app);
        edje_object_signal_callback_add(obj, "move,start",
                    "*", entangle_ui_cb_menu_item_move_start, app);

        if (app->children)
        {
            char path[PATH_MAX];

            edje_object_signal_emit(obj, "arrow,show", "*");
            edje_object_signal_callback_add(obj, "dir,clicked",
                    "*", entangle_ui_cb_dir_clicked, app);

            snprintf(path, PATH_MAX, "%s/.directory.eap", app->eapp->path);
            if (ecore_file_exists(path))
            {
                o = edje_object_add(evas);
                edje_object_file_set(o, path, "icon");
                edje_object_part_swallow(obj, "icon", o);
            }
        }
        else 
        {
            o = edje_object_add(evas);
            edje_object_file_set(o, app->eapp->path, "icon");
            edje_object_part_swallow(obj, "icon", o);
        }

        if (o)
            evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE,
                                        entangle_ui_cb_menu_item_free, o);

        esmart_container_element_append(container, obj);
    }
    
    o = evas_object_name_find(evas, "edje");
    edje_object_part_geometry_get(o, "menu_items", NULL, NULL, NULL, &h);

    l = esmart_container_elements_length_get(container);

    if (l < h) 
        edje_object_signal_emit(o, "scroll,buttons,hide", "*");
    else
    {
        l =  esmart_container_scroll_percent_get(o);
        edje_object_part_drag_value_set(o, "scroll_title_outline", 0, l);
        edje_object_signal_emit(o, "scroll,buttons,show", "*");
    }
    return 1;
}

static void
entangle_ui_cb_dir_clicked(void *data, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Entangle_App *app;
    Evas *evas;
    Evas_Object *container;

    app = data;
    evas = evas_object_evas_get(obj);
    container = evas_object_name_find(evas, "menu");
    esmart_container_empty(container);
    entangle_ui_menu_populate_list(app->eapp->exe, app->children, container);
}

static void
entangle_ui_cb_menu_change(void *data, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Evas *evas;
    Evas_Object *container;
    char *name;

    name = data;
    evas = evas_object_evas_get(obj);
    container = evas_object_name_find(evas, "menu");
    esmart_container_empty(container);
    entangle_ui_menu_populate(name, container);
}

static void
entangle_ui_cb_menu_item_free(void *data, Evas *evas __UNUSED__,
                              Evas_Object *obj __UNUSED__,  
                              void *ev __UNUSED__)
{
    Evas_Object *o;
    o = data;
    evas_object_del(o);
}

static void
entangle_ui_cb_mouse_down(void *data, Evas *evas,
                            Evas_Object *obj,  void *ev)
{
    Evas_Object *o;
    Evas_Event_Mouse_Down *e;
    Entangle_Eapp *eapp;
    Evas_Coord y, h;

    e = ev;
    eapp = data;

    o = evas_object_name_find(evas, "edje");
    edje_object_part_geometry_get(o, "eapps_bar", NULL, &y, NULL, &h);

    if ((e->canvas.y > y) && (e->canvas.y < (y + h)))
    {
        if (e->button == 1)
        {
            o = edje_object_add(evas);
            edje_object_file_set(o, eapp->path, "icon");
            evas_object_move(o, e->canvas.x - 18, e->canvas.y - 18);
            evas_object_resize(o, 36, 36);
            evas_object_layer_set(o, 2);
            evas_object_color_set(o, 255, 255, 255, 198);
            evas_object_show(o);

            evas_object_data_set(o, "eapp", eapp);
            evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE,
                                                entangle_ui_cb_mouse_move, o);
            evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
                                                entangle_ui_cb_mouse_up, o);
        }
    }
}

static void
entangle_ui_cb_mouse_up(void *data, Evas *evas,
                            Evas_Object *obj,  void *ev)
{
    Evas_Event_Mouse_Up *e;
    Evas_Object *o, *o2;
    Evas_Coord x, y, w, h;
    Entangle_Eapp *eapp;

    e = ev;
    o2 = data;
    eapp = evas_object_data_get(o2, "eapp");

    o = evas_object_name_find(evas, "edje");
    edje_object_part_geometry_get(o, "menu_base", &x, &y, &w, &h);

        
    if ((e->canvas.x > x) && (e->canvas.y > y)
            && (e->canvas.x < (x + w)) && (e->canvas.y < (y + h)))
    {
        Entangle_App *app;
        Evas_Object *container, *obj, *o3;
        Ecore_List *apps;
        const char *file, *rel_path;
        int position = 0;

        container = evas_object_name_find(evas, "menu");
        apps = evas_object_data_get(container, "list");
        rel_path = evas_object_data_get(container, "list_rel_path");

        o3 = evas_object_name_find(evas, "edje");
        edje_object_file_get(o3, &file, NULL);

        app = calloc(1, sizeof(Entangle_App));
        app->eapp = eapp;

        obj = edje_object_add(evas);
        edje_object_file_set(obj, file, "menu_item");
        edje_object_part_text_set(obj, "text", app->eapp->name);

        edje_object_part_swallow(obj, "icon", o2);

        if (obj)
            evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE,
                                        entangle_ui_cb_menu_item_free, o2);

        position = entangle_ui_calc_container_position(container, e->canvas.y);
        ecore_list_index_goto(apps, position);
        ecore_list_insert(apps, app);

        esmart_container_empty(container);
        entangle_ui_menu_populate_list(rel_path, apps, container);
    }
    else evas_object_del(o2);

    evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_MOVE,
                                    entangle_ui_cb_mouse_move);
    evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_UP,
                                    entangle_ui_cb_mouse_up);
}

static void
entangle_ui_cb_mouse_move(void *data, Evas *evas __UNUSED__,
                            Evas_Object *obj __UNUSED__,  void *ev)
{
    Evas_Object *o;
    Evas_Event_Mouse_Move *e;

    o = data;
    e = ev;
    evas_object_move(o, e->cur.canvas.x - 18, e->cur.canvas.y - 18);
}

static void entangle_ui_cmd_bar_init(Evas_Object *container)
{
    Evas_Object *o;
    Evas *evas;

    evas = evas_object_evas_get(container);
    o = entangle_ui_cmd_bar_button_get(evas, "Add Dir", 
                                    entangle_ui_cb_directory_add);
    esmart_container_element_append(container, o);

    o = entangle_ui_cmd_bar_button_get(evas, "Save", entangle_ui_cb_save);
    esmart_container_element_append(container, o);

    o = entangle_ui_cmd_bar_button_get(evas, "Quit", entangle_ui_cb_quit);
    esmart_container_element_append(container, o);
}

static Evas_Object *
entangle_ui_cmd_bar_button_get(Evas *evas, const char *label,  
                                    void (*func) (void *data, Evas_Object *obj, 
                                                    const char *emission, 
                                                    const char *source))
{
    Evas_Object *o;
    const char *file;

    o = evas_object_name_find(evas, "edje");
    edje_object_file_get(o, &file, NULL);

    o = edje_object_add(evas);
    edje_object_file_set(o, file, "button");
    edje_object_part_text_set(o, "text", label);
    edje_object_signal_callback_add(o, "button,clicked", "*", func, NULL);
    evas_object_show(o);

    return o;
}

static void
entangle_ui_button_bar_init(Evas_Object *container)
{
    Evas_Object *o;
    Evas *evas;
    char path[PATH_MAX];

    evas = evas_object_evas_get(container);

    o = entangle_ui_button_get(evas, "Favorites", "favorite");
    esmart_container_element_append(container, o);

    o = entangle_ui_button_get(evas, "iBar", "bar");
    esmart_container_element_append(container, o);

    snprintf(path, PATH_MAX, "%s/.e/e/applications/engage", getenv("HOME"));
    if (ecore_file_exists(path))
    {
        o = entangle_ui_button_get(evas, "Engage", "engage");
        esmart_container_element_append(container, o);
    }

    o = entangle_ui_button_get(evas, "Startup", "startup");
    esmart_container_element_append(container, o);

    o = entangle_ui_button_get(evas, "Restart", "restart");
    esmart_container_element_append(container, o);
}

static Evas_Object *
entangle_ui_button_get(Evas *evas, const char *label, const char *name)
{
    Evas_Object *o;
    const char *file;

    o = evas_object_name_find(evas, "edje");
    edje_object_file_get(o, &file, NULL);

    o = edje_object_add(evas);
    edje_object_file_set(o, file, "button");
    edje_object_part_text_set(o, "text", label);
    edje_object_signal_callback_add(o, "button,clicked", "*", 
                                    entangle_ui_cb_menu_change, (void *)name);
    evas_object_show(o);
    return o;
}

static int
entangle_ui_calc_container_position(Evas_Object *container, Evas_Coord at_y)
{
    Eina_List *list, *l;
    Evas_Object *o;

    list = esmart_container_elements_get(container);
    if (eina_list_count(list) == 0) return 0;

    /* check the first element */
    l = eina_list_nth_list(list, 0);
    o = l->data;
    if (entangle_ui_position_compare(o, at_y) < 0) 
        return 0;

    /* check the last element */
    l = eina_list_last(list);
    o = l->data;
    if (entangle_ui_position_compare(o, at_y) > 0) 
        return eina_list_count(list);

    return entangle_ui_list_find(list, at_y);
}

static int
entangle_ui_position_compare(Evas_Object *o, Evas_Coord at_y)
{
    Evas_Coord y, h;

    evas_object_geometry_get(o, NULL, &y, NULL, &h);
    if (at_y < y) return -1;
    else if (at_y > (y + h)) return 1;
    else if (at_y < (y + (h / 2))) return 0;
    return 1;
}

static int
entangle_ui_list_find(Eina_List *list, Evas_Coord at_y)
{
    return entangle_ui_list_find_rec(list, at_y, 0, eina_list_count(list));
}

static int
entangle_ui_list_find_rec(Eina_List *list, Evas_Coord at_y, int start, int end)
{
    Eina_List *l;
    Evas_Object *o;
    int middle, position;

    if (start == end) return start;

    middle = start + ((end - start) >> 1);
    l = eina_list_nth_list(list, middle);
    o = l->data;

    position = entangle_ui_position_compare(o, at_y);
    if (position == 0) return middle;
    else if (position < 0) return entangle_ui_list_find_rec(list, at_y, start, middle);
    else return entangle_ui_list_find_rec(list, at_y, middle + 1, end);
}

static void
entangle_ui_cb_quit(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
                    const char *emission __UNUSED__,
                    const char *src __UNUSED__)
{
    ecore_main_loop_quit();
}

static void
entangle_ui_cb_save(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
                    const char *emission __UNUSED__,
                    const char *src __UNUSED__)
{
    entangle_apps_save();
}

static void
entangle_ui_cb_directory_add(void *data __UNUSED__, Evas_Object *obj,
                                const char *emission __UNUSED__,
                                const char *src __UNUSED__)
{
    Evas_Coord x, y;
    Evas_Object *o, *o2;
    Evas *evas;
    const char *file;

    evas = evas_object_evas_get(obj);
    o = evas_object_name_find(evas, "edje");
    edje_object_file_get(o, &file, NULL);

    evas_object_geometry_get(obj, &x, &y, NULL, NULL);

    o = edje_object_add(evas);
    edje_object_file_set(o, file, "add_dir");

    o2 = esmart_text_entry_new(evas);
    esmart_text_entry_edje_part_set(o2, o, "entry");
    esmart_text_entry_max_chars_set(o2, PATH_MAX);
    esmart_text_entry_is_password_set(o2, 0);
    evas_object_focus_set(o2, 1);
    esmart_text_entry_text_set(o2, "");
    esmart_text_entry_return_key_callback_set(o2, entangle_ui_dir_add_name, o);
    evas_object_resize(o2, DIR_ENTRY_W, DIR_ENTRY_H);
    evas_object_move(o2, x, y);
    evas_object_show(o2);

    evas_object_data_set(o, "entry", o2);
    evas_object_move(o, x, y);
    evas_object_resize(o, DIR_ENTRY_W, DIR_ENTRY_H);
    evas_object_show(o);
}

static void
entangle_ui_dir_add_name(void *data, const char *str)
{
    Evas_Object *o, *o2, *container;
    Evas *evas;
    Entangle_App *app;
    Ecore_List *list;
    char path[PATH_MAX], exe[PATH_MAX];
    const char *rel_path;

    o = data;
    o2 = evas_object_data_get(o, "entry");

    if (strlen(str) == 0) return;

    evas = evas_object_evas_get(o);
    container = evas_object_name_find(evas, "menu");
    list = evas_object_data_get(container, "list");
    rel_path = evas_object_data_get(container, "list_rel_path");

    snprintf(exe, PATH_MAX, "%s/%s", rel_path, str);
    snprintf(path, PATH_MAX, "%s/.e/e/applications/%s", 
                                getenv("HOME"), exe);

    app = calloc(1, sizeof(Entangle_App));
    app->eapp = calloc(1, sizeof(Entangle_Eapp));
    app->eapp->name = strdup(str);
    app->eapp->eapp_name = strdup(app->eapp->name);
    app->eapp->path = strdup(path);
    app->eapp->exe = strdup(exe);
    app->children = ecore_list_new();

    ecore_list_append(list, app);
    esmart_container_empty(container);
    entangle_ui_menu_populate_list(rel_path, list, container);

    evas_object_del(o2);
    evas_object_del(o);
}

static void
entangle_ui_cb_menu_item_delete(void *data, Evas_Object *obj,
                                const char *emission __UNUSED__, 
                                const char *src __UNUSED__)
{
    Entangle_App *app, *cur_app;
    Evas *evas;
    Evas_Object *container;
    Ecore_List *list;
    const char *rel_path;

    app = data;
    evas = evas_object_evas_get(obj);
    container = evas_object_name_find(evas, "menu");
    list = evas_object_data_get(container, "list");
    rel_path = evas_object_data_get(container, "list_rel_path");

    esmart_container_empty(container);

    cur_app = ecore_list_goto(list, app);
    if (!cur_app) return;
    ecore_list_remove(list);

    if (cur_app->children)
    {
        Ecore_List *del_list;

        del_list = entangle_apps_list_get("deleted_dirs");
        ecore_list_append(del_list, cur_app);
    }
    entangle_ui_menu_populate_list(rel_path, list, container);
}

static void
entangle_ui_cb_menu_item_move_start(void *data, Evas_Object *obj,
                                    const char *emission __UNUSED__, 
                                    const char *src __UNUSED__)
{
    Entangle_App *app, *cur_app;
    Evas *evas;
    Evas_Object *container;
    Ecore_List *list;

    app = data;
    evas = evas_object_evas_get(obj);
    container = evas_object_name_find(evas, "menu");
    list = evas_object_data_get(container, "list");
    esmart_container_element_remove(container, obj);

    cur_app = ecore_list_goto(list, app);
    if (!cur_app) return;
    ecore_list_remove(list);

    evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE,
                                        entangle_ui_cb_menu_item_move, data);
    evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
                                        entangle_ui_cb_menu_item_move_end, data);

    evas_object_color_set(obj, 255, 255, 255, 198);
}

static void
entangle_ui_cb_menu_item_move_end(void *data, Evas *evas,
                                    Evas_Object *obj,  void *ev)
{
    Evas_Event_Mouse_Up *e;
    Entangle_App *app;
    Evas_Coord x, y, w, h;
    Evas_Object *o, *container;
    int position;

    e = ev;
    app = data;

    o = evas_object_name_find(evas, "edje");
    edje_object_part_geometry_get(o, "menu_base", &x, &y, &w, &h);

    if ((e->canvas.x > x) && (e->canvas.y > y) 
            && (e->canvas.x < (x + w)) && (e->canvas.y < (y + h)))
    {
        Ecore_List *apps;
        const char *rel_path;

        container = evas_object_name_find(evas, "menu");
        position = entangle_ui_calc_container_position(container, e->canvas.y);

        evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_MOVE,
                                        entangle_ui_cb_menu_item_move);
        evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_UP,
                                        entangle_ui_cb_menu_item_move);

        apps = evas_object_data_get(container, "list");
        rel_path = evas_object_data_get(container, "list_rel_path");
        ecore_list_index_goto(apps, position);
        ecore_list_insert(apps, app);

        esmart_container_empty(container);
        entangle_ui_menu_populate_list(rel_path, apps, container);
    }
    evas_object_del(obj);
}

static void
entangle_ui_cb_menu_item_move(void *data __UNUSED__, Evas *evas __UNUSED__,
                                Evas_Object *obj,  void *ev)
{
    Evas_Event_Mouse_Move *e;
    Evas_Coord x, y;
    Evas_Coord cur_x, cur_y;
    e = ev;

    evas_object_geometry_get(obj, &cur_x, &cur_y, NULL, NULL);
    x = e->cur.canvas.x - e->prev.canvas.x;
    y = e->cur.canvas.y - e->prev.canvas.y;
    evas_object_move(obj, cur_x + x, cur_y + y);
}


