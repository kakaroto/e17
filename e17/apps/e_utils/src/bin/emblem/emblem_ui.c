#include "Emblem.h"

/* TODO
 *  * make current not change, just change the edje file and do a double
 *    buffer thing like the menu is doing
 *  * make the thumbs cache to disk, check the eet mtime againts a the time
 *    of the cached version. Each bg has one or more .eet thumb files. these
 *    files will need the screen size in there name and either the timestamp
 *    or an md5 in them (look at epsilon). Then for the menu only load these
 *    cached, so there is only one fs image around
 *  * need to calcluate SCREEN_W and SCREEN_H dynamically
 *  * seem to get a lot of space around the thumbs in the menu bar would be
 *    nice to get these more compact
 *  * need to put a border around the current menubar item that the mouse is
 *    over, like a beveled edge or something
 *  * make startup faster? ... (not sure what to do about it)
 *  * when libe supports it need some way to set the bg on different
 *    containers. maybe a little image bottom right above the menu bar, one
 *    for each container, you select the active one and it wills et the bg
 *    on that container?
 *  * disable the scroll arrow when your at the edge
 */

#define WIDTH 600
#define HEIGHT 400

#define THUMB_W 320
#define THUMB_H 240

#define SCREEN_W 1600
#define SCREEN_H 1200

static int emblem_load_bgs(Emblem *em);
static Evas_Object *emblem_evas_object_get(Emblem *em, const char *fname);

static void emblem_ui_init_job_cb(void *data);
static void emblem_ui_resize_cb(Ecore_Evas *ee);

static void emblem_intercept_show_cb(void *data, Evas_Object *obj);
static void emblem_intercept_hide_cb(void *data, Evas_Object *obj);
static void emblem_intercept_move_cb(void *data, Evas_Object *obj,
                                        Evas_Coord x, Evas_Coord y);
static void emblem_intercept_resize_cb(void *data, Evas_Object *obj,
                                        Evas_Coord w, Evas_Coord h);
static void emblem_intercept_raise_cb(void *data, Evas_Object *obj);
static void emblem_intercept_lower_cb(void *data, Evas_Object *obj);
static void emblem_intercept_stack_above_cb(void *data, Evas_Object *obj,
                                        Evas_Object *above);
static void emblem_intercept_stack_below_cb(void *data, Evas_Object *obj,
                                        Evas_Object *below);
static void emblem_intercept_layer_set_cb(void *data, Evas_Object *obj, 
                                        int l);

static void emblem_current_sel_cb(void *data, Evas *evas, Evas_Object *obj,
                                                                void *ev);
static void emblem_menu_sel_cb(void *data, Evas *evas, Evas_Object *obj, 
                                                                void *ev);

static void emblem_right_scroll_up_cb(void *data, Evas_Object *obj, 
                                const char *emission, const char *src);
static void emblem_left_scroll_up_cb(void *data, Evas_Object *obj, 
                                const char *emission, const char *src);
static void emblem_right_scroll_down_cb(void *data, Evas_Object *obj, 
                                const char *emission, const char *src);
static void emblem_left_scroll_down_cb(void *data, Evas_Object *obj, 
                                const char *emission, const char *src);

int
emblem_ui_init(Emblem *em)
{
    Ecore_Evas *ee;
    Evas *evas;
    Evas_Object *o;
    double l;
    Evas_Coord w;

    ee = ecore_evas_software_x11_new(em->display, 0, 0, 0, WIDTH, HEIGHT);
    ecore_evas_title_set(ee, PACKAGE);
    ecore_evas_callback_resize_set(ee, emblem_ui_resize_cb);
    ecore_evas_data_set(ee, "emblem", em);
    ecore_evas_show(ee);
    em->gui.ee = ee;

    evas = ecore_evas_get(ee);
    em->gui.evas = evas;

    o = edje_object_add(evas);
    if (!edje_object_file_set(o,
                PACKAGE_DATA_DIR"/emblem/default.eet", "Main"))
    {
        printf("Unable to locate theme\n");
        return 0;
    }
    evas_object_move(o, 0, 0);
    evas_object_resize(o, WIDTH, HEIGHT);
    evas_object_show(o);
    em->gui.edje = o;

    o = esmart_container_new(evas);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_HORIZONTAL);
    esmart_container_alignment_set(o, CONTAINER_ALIGN_LEFT);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_NONE);
    esmart_container_spacing_set(o, 10);
    esmart_container_padding_set(o, 5, 5, 5, 5);
    edje_object_part_swallow(em->gui.edje, "menu_bar", o);
    evas_object_show(o);
    em->gui.menu = o;

    ecore_job_add(emblem_ui_init_job_cb, em);

    return 1;
}

void
emblem_ui_shutdown(void)
{
}

static void
emblem_ui_resize_cb(Ecore_Evas *ee)
{
    Emblem *em;
    Evas_Coord w, h;
    double l;
    const char *state;

    em = ecore_evas_data_get(ee, "emblem");

    ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
    evas_object_resize(em->gui.edje, w, h);

    edje_object_part_geometry_get(em->gui.edje, "current", NULL, NULL, &w, &h);
    evas_object_resize(em->gui.current, w, h);
    evas_object_image_fill_set(em->gui.current, 0, 0, w, h);

    edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, &w, NULL);
    l = esmart_container_elements_length_get(em->gui.menu);
    state = edje_object_part_state_get(em->gui.edje, "left_arrow", NULL);

    if (l < w)
    {
        if (!strcmp(state, "default"))
        {
            edje_object_signal_emit(em->gui.edje, "arrows,hide", "*");
            edje_object_signal_callback_del(em->gui.edje, "left,clicked", "*",
                                                    emblem_left_scroll_down_cb);
            edje_object_signal_callback_del(em->gui.edje, "right,clicked", "*",
                                                    emblem_right_scroll_down_cb);
            edje_object_signal_callback_del(em->gui.edje, "left,click,release", "*",
                                                    emblem_left_scroll_up_cb);
            edje_object_signal_callback_del(em->gui.edje, "right,click,release", "*",
                                                    emblem_right_scroll_up_cb);
        }
    }
    else 
    {
        if (!strcmp(state, "hidden"))
        {
            edje_object_signal_emit(em->gui.edje, "arrows,show", "*");
            edje_object_signal_callback_add(em->gui.edje, "left,clicked", "*",
                                                emblem_left_scroll_down_cb, NULL);
            edje_object_signal_callback_add(em->gui.edje, "right,clicked", "*",
                                                emblem_right_scroll_down_cb, NULL);
            edje_object_signal_callback_add(em->gui.edje, "left,click,release", "*",
                                                emblem_left_scroll_up_cb, NULL);
            edje_object_signal_callback_add(em->gui.edje, "right,click,release", "*",
                                                emblem_right_scroll_up_cb, NULL);
        }
    }
}

static void
emblem_ui_init_job_cb(void *data)
{
    Emblem *em;
    Evas_Coord w;
    double l;

    em = data;
    emblem_load_bgs(em);

    edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, &w, NULL);
    l = esmart_container_elements_length_get(em->gui.menu);

    if (l < w)
        edje_object_signal_emit(em->gui.edje, "arrows,hide", "*");
    else 
    {
        edje_object_signal_callback_add(em->gui.edje, "left,clicked", "*",
                                                emblem_left_scroll_down_cb, em);
        edje_object_signal_callback_add(em->gui.edje, "right,clicked", "*",
                                                emblem_right_scroll_down_cb, em);
        edje_object_signal_callback_add(em->gui.edje, "left,click,release", "*",
                                                emblem_left_scroll_up_cb, em);
        edje_object_signal_callback_add(em->gui.edje, "right,click,release", "*",
                                                emblem_right_scroll_up_cb, em);
    }
}

static int
emblem_load_bgs(Emblem *em)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];
    Evas_Coord w, h;
    Evas_Object *ev;

    snprintf(path, PATH_MAX, "%s/.e/e/backgrounds", getenv("HOME"));
    dir = opendir(path);
    if (!dir) return 0;

    /* get these from the system */
    w = SCREEN_W;
    h = SCREEN_H;

    while ((entry = readdir(dir)))
    {
        Evas_Object *o;

        if (!strcmp(entry->d_name, "..")) continue;
        if (!strcmp(entry->d_name, ".")) continue;
        if (!strstr(entry->d_name, ".eet")) continue;

        snprintf(path, PATH_MAX, "%s/.e/e/backgrounds/%s", 
                                getenv("HOME"), entry->d_name);

        o = emblem_evas_object_get(em, path);
        if (em->gui.current == NULL)
        {
            em->gui.current = o;

            edje_object_part_geometry_get(em->gui.edje, "current", NULL, NULL, &w, &h);

            edje_object_part_swallow(em->gui.edje, "current", o);

            ev = evas_object_data_get(o, "ev");
            evas_object_event_callback_add(ev, EVAS_CALLBACK_MOUSE_DOWN,
                                                emblem_current_sel_cb, em);
            continue;
        }
        edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, NULL, &h);
        evas_object_resize(o, 64, 48);

        ev = evas_object_data_get(o, "ev");
        evas_object_event_callback_add(ev, EVAS_CALLBACK_MOUSE_DOWN,
                                            emblem_menu_sel_cb, em);
        esmart_container_element_append(em->gui.menu, o);
    }
    return 1;
}

static Evas_Object *
emblem_evas_object_get(Emblem *em, const char *fname)

{
    Evas_Object *o, *o2, *ob, *ev;
    Ecore_Evas *ee2;
    Evas *evas2;

    ob = o2 = ecore_evas_object_image_new(em->gui.ee);
    ee2 = evas_object_data_get(o2, "Ecore_Evas");
    evas2 = ecore_evas_get(ee2);
    evas_object_image_size_set(o2, THUMB_W, THUMB_H); /* thumb res */
    evas_object_image_fill_set(o2, 0, 0, THUMB_W, THUMB_H);
    evas_object_show(o2);

    o2 = ecore_evas_object_image_new(ee2);
    ee2 = evas_object_data_get(o2, "Ecore_Evas");
    evas2 = ecore_evas_get(ee2);
    evas_object_image_size_set(o2, SCREEN_W, SCREEN_H); /* screen res */
    evas_object_move(o2, 0, 0);
    evas_object_resize(o2, THUMB_W, THUMB_H);
    evas_object_image_fill_set(o2, 0, 0, THUMB_W, THUMB_H);
    evas_object_show(o2);

    o = edje_object_add(evas2);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, SCREEN_W, SCREEN_H);

    if (!edje_object_file_set(o, fname, "desktop/background"))
    {
        printf("Unable to load file, %s\n", fname);
        evas_object_del(o);
        return NULL;
    }
    evas_object_data_set(o, "file", strdup(fname));
    evas_object_show(o);

    ev = evas_object_rectangle_add(em->gui.evas);
    evas_object_color_set(ev, 0, 0, 0, 0);
    evas_object_show(ev);

    evas_object_data_set(ob, "ev", ev);
    evas_object_data_set(ev, "edje", ob);

    evas_object_intercept_move_callback_add(ob, 
                                emblem_intercept_move_cb, NULL);
    evas_object_intercept_resize_callback_add(ob, 
                                emblem_intercept_resize_cb, NULL);
    evas_object_intercept_raise_callback_add(ob, 
                                emblem_intercept_raise_cb, NULL);
    evas_object_intercept_lower_callback_add(ob, 
                                emblem_intercept_lower_cb, NULL);
    evas_object_intercept_layer_set_callback_add(ob, 
                                emblem_intercept_layer_set_cb, NULL);
    evas_object_intercept_stack_above_callback_add(ob, 
                                emblem_intercept_stack_above_cb, NULL);
    evas_object_intercept_stack_below_callback_add(ob, 
                                emblem_intercept_stack_below_cb, NULL);
    evas_object_intercept_show_callback_add(ob, emblem_intercept_show_cb, NULL);
    evas_object_intercept_hide_callback_add(ob, emblem_intercept_hide_cb, NULL);

    return ob;
}

static void
emblem_current_sel_cb(void *data, Evas *evas, Evas_Object *obj, void *ev)
{
    Emblem *em;
    char *name; 
    Evas_Event_Mouse_Down *e;
    Evas_Coord x, y, w, h;
    Evas_Object *o;

    e = ev;
    em = data;

    /* make sure the click was actually inside the current. When we change
     * the callbacks in the menu_sel_cb this cb will be called. so we need
     * to verify they actually selected the item to be set as the bg)
     */
    edje_object_part_geometry_get(em->gui.edje, "current", &x, &y, &w, &h);
    if ((e->canvas.x < x) || (e->canvas.y < y) 
            || (e->canvas.x > (x + w)) || (e->canvas.y > (y + h)))
        return;

    name = evas_object_data_get(em->gui.current, "file");
    e_background_set(name);

    return;
    obj = NULL;
    ev = NULL;
}

static void
emblem_menu_sel_cb(void *data, Evas *evas, Evas_Object *obj, void *ev)
{
    Emblem *em;
    char *name, *cur_name;
    Evas_Coord w, h;
    Evas_Object *o, *cur_ev, *sel_obj;

    em = data;

    cur_ev = evas_object_data_get(em->gui.current, "ev");
    sel_obj = evas_object_data_get(obj, "edje");

    evas_object_event_callback_del(cur_ev, EVAS_CALLBACK_MOUSE_DOWN,
                                            emblem_current_sel_cb);
    evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_DOWN,
                                            emblem_menu_sel_cb);

    edje_object_part_unswallow(em->gui.edje, em->gui.current);
    esmart_container_element_remove(em->gui.menu, sel_obj);

    o = em->gui.current;
    em->gui.current = sel_obj;

    edje_object_part_geometry_get(em->gui.edje, "current", NULL, NULL, &w, &h);
    evas_object_resize(em->gui.current, w, h);
    evas_object_image_fill_set(em->gui.current, 0, 0, w, h);

    evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN,
                                            emblem_current_sel_cb, em);
    edje_object_part_swallow(em->gui.edje, "current", em->gui.current);

    edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, NULL, &h);
    evas_object_resize(o, 64, 48);
    evas_object_event_callback_add(cur_ev, EVAS_CALLBACK_MOUSE_DOWN,
                                            emblem_menu_sel_cb, em);

    esmart_container_element_append(em->gui.menu, o);

    return;
    data = NULL;
    ev = NULL;
}

static void
emblem_left_scroll_down_cb(void *data, Evas_Object *obj, 
        const char *emission, const char *src)
{
    Emblem *em;

    em = data;
    esmart_container_scroll_start(em->gui.menu, -2);

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
emblem_right_scroll_down_cb(void *data, Evas_Object *obj, 
        const char *emission, const char *src)
{
    Emblem *em;

    em = data;
    esmart_container_scroll_start(em->gui.menu, 2);

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
emblem_left_scroll_up_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src)
{
    Emblem *em;

    em = data;
    esmart_container_scroll_stop(em->gui.menu);

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
emblem_right_scroll_up_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src)
{
    Emblem *em;

    em = data;
    esmart_container_scroll_stop(em->gui.menu);

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
emblem_intercept_show_cb(void *data, Evas_Object *obj)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_show(ev);
    evas_object_show(obj);
}

static void
emblem_intercept_hide_cb(void *data, Evas_Object *obj)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_hide(ev);
    evas_object_hide(obj);
}

static void
emblem_intercept_move_cb(void *data, Evas_Object *obj,
        Evas_Coord x, Evas_Coord y)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_move(ev, x, y);
    evas_object_move(obj, x, y);
}

static void
emblem_intercept_resize_cb(void *data, Evas_Object *obj,
        Evas_Coord w, Evas_Coord h)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_resize(ev, w, h);
    evas_object_resize(obj, w, h);
    evas_object_image_fill_set(obj, 0, 0, w, h);
}

static void
emblem_intercept_raise_cb(void *data, Evas_Object *obj)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_raise(obj);
    evas_object_raise(ev);
}

static void
emblem_intercept_lower_cb(void *data, Evas_Object *obj)
{
    Evas_Object *ev;


    ev = evas_object_data_get(obj, "ev");
    evas_object_lower(obj);
    evas_object_lower(ev);
}

static void
emblem_intercept_stack_above_cb(void *data, Evas_Object *obj,
        Evas_Object *above)
{
    Evas_Object *ev;


    ev = evas_object_data_get(obj, "ev");
    evas_object_stack_above(obj, above);
    evas_object_stack_above(ev, obj);
}

static void
emblem_intercept_stack_below_cb(void *data, Evas_Object *obj,
        Evas_Object *below)
{
    Evas_Object *ev;


    ev = evas_object_data_get(obj, "ev");
    evas_object_stack_below(obj, below);
    evas_object_stack_below(ev, below);
}

static void
emblem_intercept_layer_set_cb(void *data, Evas_Object *obj, int l)
{
    Evas_Object *ev;

    ev = evas_object_data_get(obj, "ev");
    evas_object_layer_set(obj, l);
    evas_object_layer_set(ev, l);
}


