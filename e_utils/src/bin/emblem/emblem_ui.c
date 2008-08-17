#include "Emblem.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

/* TODO
 *  * make the thumbs cache to disk, check the edj mtime againts a the time
 *    of the cached version. Each bg has one or more .edj thumb files. these
 *    files will need the screen size in there name and either the timestamp
 *    or an md5 in them (look at epsilon). Then for the menu only load these
 *    cached, so there is only one fs image around
 *  * need to calcluate SCREEN_W and SCREEN_H dynamically
 *  * need to put a border around the current menubar item that the mouse is
 *    over, like a beveled edge or something
 *  * make startup faster? ... (not sure what to do about it)
 *  * when libe supports it need some way to set the bg on different
 *    containers. maybe a little image bottom right above the menu bar, one
 *    for each container, you select the active one and it wills et the bg
 *    on that container?
 *  * disable the scroll arrow when your at the edge
 *  * The calculation to turn on the scroll arrows seems to be a bit off.
 *    Make the arrows diappear and then slowly resize down, seems to be
 *    about a 10px delay before the arrows kick in
 */

#define WIDTH 600
#define HEIGHT 400

#define THUMB_W 320
#define THUMB_H 240

#define SCREEN_W 1600
#define SCREEN_H 1200

static int emblem_load_bgs(Emblem *em, char *dir_path);
static Evas_Object *emblem_evas_object_get(Emblem *em, const char *fname,
                                                Evas_Coord w, Evas_Coord h);
static int emblem_ui_e_bg_get(void *data, int type, void *ev);
static int emblem_ui_e_bg_dirs_list(void *data, int type, void *ev);
static int emblem_ui_e_theme_dirs_list(void *data, int type, void *ev);
static void emblem_current_bg_set(Emblem *em, char *file);

static void emblem_ui_init_dir(Emblem *em, char *dir);
static void emblem_ui_resize_cb(Ecore_Evas *ee);

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
    Evas_Object *o;
    Evas *evas;

    ee = ecore_evas_software_x11_new(em->display, 0, 0, 0, WIDTH, HEIGHT);
    ecore_evas_name_class_set(ee, "Emblem", "Emblem");
    ecore_evas_title_set(ee, "Emblem");
    ecore_evas_callback_resize_set(ee, emblem_ui_resize_cb);
    ecore_evas_data_set(ee, "emblem", em);
    ecore_evas_show(ee);
    em->gui.ee = ee;

    evas = ecore_evas_get(ee);
    em->gui.evas = evas;

    o = edje_object_add(evas);
    if (!edje_object_file_set(o, em->theme, "Main"))
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
    em->gui.current = NULL;

    ecore_event_handler_add(E_RESPONSE_BACKGROUND_GET, 
                                emblem_ui_e_bg_get, em);
    ecore_event_handler_add(E_RESPONSE_BACKGROUND_DIRS_LIST,
                                emblem_ui_e_bg_dirs_list, em);
    ecore_event_handler_add(E_RESPONSE_THEME_DIRS_LIST,
                                emblem_ui_e_theme_dirs_list, em);
    e_lib_background_get();
    e_lib_theme_dirs_list();
    e_lib_background_dirs_list();

    edje_object_signal_callback_add(em->gui.edje, "left,clicked", "*",
                                            emblem_left_scroll_down_cb, em);
    edje_object_signal_callback_add(em->gui.edje, "right,clicked", "*",
                                            emblem_right_scroll_down_cb, em);
    edje_object_signal_callback_add(em->gui.edje, "left,click,release", "*",
                                            emblem_left_scroll_up_cb, em);
    edje_object_signal_callback_add(em->gui.edje, "right,click,release", "*",
                                            emblem_right_scroll_up_cb, em);
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
    Evas_Object *o;

    em = ecore_evas_data_get(ee, "emblem");

    ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
    evas_object_resize(em->gui.edje, w, h);

    edje_object_part_geometry_get(em->gui.edje, "current", 
                                            NULL, NULL, &w, &h);

    evas_object_image_size_set(em->gui.current, w, h);
    evas_object_image_fill_set(em->gui.current, 0, 0, w, h);

    o = evas_object_data_get(em->gui.current, "screen_buffer");
    evas_object_resize(o, w, h);
    evas_object_image_fill_set(o, 0, 0, w, h);
    evas_object_resize(em->gui.current, w, h);

    edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, &w, NULL);
    l = esmart_container_elements_length_get(em->gui.menu);
    state = edje_object_part_state_get(em->gui.edje, "left_arrow", NULL);

    if ((l < w) && (!strcmp(state, "default")))
            edje_object_signal_emit(em->gui.edje, "arrows,hide", "*");
    else if (!strcmp(state, "hidden"))
            edje_object_signal_emit(em->gui.edje, "arrows,show", "*");
}

static void
emblem_ui_init_dir(Emblem *em, char *dir)
{
    Evas_Coord w;
    double l;

    emblem_load_bgs(em, dir);

    edje_object_part_geometry_get(em->gui.edje, "menu_bar", NULL, NULL, &w, NULL);
    l = esmart_container_elements_length_get(em->gui.menu);

    if (l < w)
            edje_object_signal_emit(em->gui.edje, "arrows,hide", "*");
    else 
            edje_object_signal_emit(em->gui.edje, "arrows,show", "*");
}

static int
emblem_load_bgs(Emblem *em, char *dir_path)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    dir = opendir(dir_path);
    if (!dir) return 0;

    while ((entry = readdir(dir)))
    {
        Evas_Object *o;

        if (!strcmp(entry->d_name, "..")) continue;
        if (!strcmp(entry->d_name, ".")) continue;
        if (!strstr(entry->d_name, ".edj")) continue;

        snprintf(path, PATH_MAX, "%s/%s", dir_path, entry->d_name);
        o = emblem_evas_object_get(em, path, THUMB_W, THUMB_H);
        evas_object_resize(o, 64, 48);
        
        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                        emblem_menu_sel_cb, em);

        esmart_container_element_append(em->gui.menu, o);
    }
    closedir(dir);
    return 1;
}

static Evas_Object *
emblem_evas_object_get(Emblem *em, const char *fname,
                            Evas_Coord w, Evas_Coord h)
{
    Epsilon *e;
    Evas_Object *o;

    if (!(e = epsilon_new(fname)))
        return NULL;

    epsilon_key_set(e, "desktop/background");
    epsilon_resolution_set(e, SCREEN_W, SCREEN_H);

    if (epsilon_exists(e) == EPSILON_FAIL)
        epsilon_generate(e);

    o = evas_object_image_add(ecore_evas_get(em->gui.ee));
    evas_object_image_file_set(o, epsilon_thumb_file_get(e), NULL);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, SCREEN_W, SCREEN_H);
    evas_object_data_set(o, "file", strdup(fname));
    evas_object_show(o);

    epsilon_free(e);

    return o;
}

static void
emblem_current_bg_set(Emblem *em, char *file) 
{
    Evas_Coord w, h;
    Evas_Object *o, *o2, *ob;
    Ecore_Evas *ee2;
    Evas *evas2;

    if (em->gui.current)
    {
        edje_object_part_unswallow(em->gui.edje, em->gui.current);
        evas_object_hide(em->gui.current);
        evas_object_del(em->gui.current);
    }

    edje_object_part_geometry_get(em->gui.edje, "current", 
                                            NULL, NULL, &w, &h);

    em->gui.current = ecore_evas_object_image_new(em->gui.ee);  
    ob = em->gui.current;

    ee2 = evas_object_data_get(ob, "Ecore_Evas");
    evas_object_image_size_set(ob, w, h);
    evas_object_image_fill_set(ob, 0, 0, w, h);
    evas_object_show(ob);

    o2 = ecore_evas_object_image_new(ee2);
    ee2 = evas_object_data_get(o2, "Ecore_Evas");
    evas2 = ecore_evas_get(ee2);
    evas_object_image_size_set(o2, SCREEN_W, SCREEN_H);
    evas_object_move(o2, 0, 0);
    evas_object_resize(o2, w, h);
    evas_object_image_fill_set(o2, 0, 0, w, h);
    evas_object_show(o2);

    o = edje_object_add(evas2);
    evas_object_move(o, 0, 0);
    evas_object_resize(o, SCREEN_W, SCREEN_H);

    if (!edje_object_file_set(o, file, "desktop/background"))
    {
        printf("Unable to load file, %s\n", file);
        evas_object_del(o);
        return;
    }
    evas_object_show(o);

    evas_object_data_set(ob, "screen_buffer", o2);
    evas_object_data_set(ob, "file", strdup(file));  
        
    evas_object_resize(em->gui.current, w, h);
    edje_object_part_swallow(em->gui.edje, "current", em->gui.current);

    evas_object_event_callback_add(em->gui.current, EVAS_CALLBACK_MOUSE_DOWN,
                                                emblem_current_sel_cb, NULL);

    evas_object_data_set(em->gui.current, "file", file);
}

static int
emblem_ui_e_bg_get(void *data, int type __UNUSED__, void *ev)
{
    Emblem *em;
    E_Response_Background_Get *e;

    e = ev;
    em = data;
    emblem_current_bg_set(em, e->file);
    return 1;
}

static int
emblem_ui_e_bg_dirs_list(void *data, int type __UNUSED__, void *ev)
{
    Emblem *em;
    E_Response_Dirs_List *e;
    int i;

    e = ev;
    em = data;

    for (i = 0; i < e->count; i++)
        emblem_ui_init_dir(em, e->dirs[i]);

    return 1;
}

static int
emblem_ui_e_theme_dirs_list(void *data, int type __UNUSED__, void *ev)
{
    Emblem *em;
    E_Response_Dirs_List *e;
    int i;

    e = ev;
    em = data;

    for (i = 0; i < e->count; i++)
        emblem_ui_init_dir(em, e->dirs[i]);

    return 1;
}

static void
emblem_current_sel_cb(void *data __UNUSED__, Evas *evas __UNUSED__, 
                        Evas_Object *obj, void *ev __UNUSED__)
{
    char *name; 
    name = evas_object_data_get(obj, "file");
    e_lib_background_set(name);
    return;
}

static void
emblem_menu_sel_cb(void *data __UNUSED__, Evas *evas __UNUSED__, 
                    Evas_Object *obj, void *ev __UNUSED__)
{
    Emblem *em;
    char *name;

    em = data;
    name = evas_object_data_get(obj, "file");
    emblem_current_bg_set(em, name);
    return;
}

static void
emblem_left_scroll_down_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, 
                const char *emission __UNUSED__, const char *src __UNUSED__)
{
    Emblem *em;
    em = data;
    esmart_container_scroll_start(em->gui.menu, 2);
    return;
}

static void
emblem_right_scroll_down_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, 
                const char *emission __UNUSED__, const char *src __UNUSED__)
{
    Emblem *em;
    em = data;
    esmart_container_scroll_start(em->gui.menu, -2);
    return;
}

static void
emblem_left_scroll_up_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
                const char *emission __UNUSED__, const char *src __UNUSED__)
{
    Emblem *em;
    em = data;
    esmart_container_scroll_stop(em->gui.menu);
    return;
}

static void
emblem_right_scroll_up_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__,
                const char *emission __UNUSED__, const char *src __UNUSED__)
{
    Emblem *em;
    em = data;
    esmart_container_scroll_stop(em->gui.menu);
    return;
}


