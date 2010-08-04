#include "main.h"
#include <Ecore_X.h>

/**
 * A list of functions used to manage a presentation
 * for example add a slide, delete ...
 */

typedef enum _action Action;

enum _action {
    ACTION_NONE,
    ACTION_AREA_MOVE,
    ACTION_AREA_RESIZE
};

static Evas_Object *_inwin_delete_slide;

static Eyelight_Edit *_current_obj = NULL;
static Action _current_action = ACTION_NONE;
static int _x_save, _y_save;

static void _utils_slide_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_slide_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_area_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_area_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_edit_area_image_add_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_edit_image_file_change_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_object_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_object_delete_cb(void *data, Evas_Object *obj, void *event_info);

void utils_save(const char *file)
{
    if(!eyelight_edit_save(eyelight_object_pres_get(pres), file))
    {
        printf("SAVE ERROR %s\n",file);
    }
}

void utils_slide_insert(List_Item *item_prev)
{
    List_Item *item = calloc(1, sizeof(List_Item));

    Eina_List *l;
    List_Item *_item;
    int i_prev = -1;
    if(item_prev)
    {
        i_prev = 0;
        EINA_LIST_FOREACH(l_slides, l, _item)
        {
            if(item_prev == _item)
                break;
            else
                i_prev++;
        }
    }

    if(!item_prev)
        l_slides = eina_list_prepend(l_slides, item);
    else
        l_slides = eina_list_append_relative(l_slides, item, item_prev);

    eyelight_edit_slide_insert(eyelight_object_pres_get(pres), i_prev);

    slides_grid_append_relative(item, item_prev);
    slides_list_slide_append_relative(item, item_prev);
}

void utils_slide_delete(List_Item *item)
{
    int i = 0;
    Eina_List *l;
    List_Item *_item;

    EINA_LIST_FOREACH(l_slides, l, _item)
    {
        if(item == _item)
            break;
        else
            i++;
    }


    Evas_Object *lbl, *tb, *bt, *icon;
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    _inwin_delete_slide = elm_win_inwin_add(win);
    evas_object_show(_inwin_delete_slide);
    elm_win_inwin_style_set(_inwin_delete_slide, "minimal");

    tb = elm_table_add(win);
    elm_win_inwin_content_set(_inwin_delete_slide, tb);

    //icon
    icon = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(icon, buf, "list/slides/icon");

    evas_object_size_hint_min_set(icon,1024*0.20,768*0.20);
    evas_object_size_hint_max_set(icon,1024*0.20,768*0.20);

    if(item->thumb)
    {
        int w = item->thumb->w;
        int h = item->thumb->h;
        int *image = calloc(w*h, sizeof(int));
        memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

        const Evas_Object *o_image = edje_object_part_object_get(icon, "object.icon");
        evas_object_image_filled_set((Evas_Object*)o_image,1);
        evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
        evas_object_image_size_set((Evas_Object*)o_image, w, h);
        evas_object_image_data_set((Evas_Object*)o_image, image);

        edje_object_signal_emit(icon, "icon,show", "eyelight");
    }
    evas_object_show(icon);
    elm_table_pack(tb, icon, 0, 0, 2, 1);
    //

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 1, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Are you sure you want to delete this slide ?"));
    elm_table_pack(tb, lbl, 0, 2, 2, 1);
    evas_object_show(lbl);

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 3, 1, 1);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("Yes, Delete the slide"));
    evas_object_smart_callback_add(bt, "clicked", _utils_slide_delete_cb, item);
    evas_object_color_set(bt, 255, 0, 0, 255);
    elm_table_pack(tb, bt, 0, 4, 1, 1);
    evas_object_show(bt);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("No, do not delete the slide"));
    evas_object_smart_callback_add(bt, "clicked", _utils_slide_delete_cancel_cb, item);
    elm_table_pack(tb, bt, 1, 4, 1, 1);
    evas_object_show(bt);
}

static void _utils_slide_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_object_del(_inwin_delete_slide);
}

static void _utils_slide_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
    List_Item *item = (List_Item*) data;
    int i = 0;
    Eina_List *l;
    List_Item *_item;
    List_Item *item_prev = NULL;

    EINA_LIST_FOREACH(l_slides, l, _item)
    {
        if(item == _item)
            break;
        else
        {
            item_prev = _item;
            i++;
        }
    }

    //delete the slide
    eyelight_edit_slide_delete(eyelight_object_pres_get(pres), i);

    //remove the slide from the list
    slides_list_slide_delete(item);

    //remove the slide from the grid
    slides_grid_remove(item);

    //free the item
    l_slides = eina_list_remove(l_slides, item);
    free(item);

    //select an other slide
    if(item_prev)
        elm_genlist_item_selected_set(item_prev->item, 1);
    else if(l_slides)
    {
        List_Item *_item = eina_list_data_get(l_slides);
        elm_genlist_item_selected_set(_item->item,1);
    }

    evas_object_del(_inwin_delete_slide);
}

void utils_slide_move(int id_slide, int id_after)
{
    eyelight_edit_slide_move(eyelight_object_pres_get(pres), id_slide, id_after);

    slides_list_item_move(id_slide, id_after);
}


void utils_edit_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Edit *edit = (Eyelight_Edit*) data;

    if(edit != _current_obj && strcmp(emission,"select")!=0)
        return ;

    if(_current_obj && edit != _current_obj && strcmp(emission,"select")==0)
    {
        utils_obj_unselect();
    }

    _current_obj = edit;
    switch(eyelight_edit_name_get(_current_obj))
    {
        case EYELIGHT_NAME_CUSTOM_AREA:
        case EYELIGHT_NAME_THEME_AREA:
            if(strcmp(emission, "select") == 0)
            {
                rightpanel_area_show();
                rightpanel_area_layout_set(eyelight_edit_area_layout_get(_current_obj));
            }
            if(strcmp(emission, "move,start") == 0
                    || strcmp(emission, "resize,start") == 0)
            {
                presentation_thumbscroll_set(0);

                if(strcmp(emission, "move,start") == 0)
                    _current_action = ACTION_AREA_MOVE;
                else
                    _current_action = ACTION_AREA_RESIZE;

                int x,y, x_pres, y_pres, w_pres, h_pres, x_evas, y_evas;
                ecore_x_pointer_last_xy_get(&x,&y);
                ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win)),
                        &x_evas,&y_evas, NULL, NULL);
                evas_object_geometry_get(pres, &x_pres, &y_pres, &w_pres, &h_pres);
                x = x - x_pres - x_evas;
                y = y - y_pres - y_evas;
                _x_save = x;
                _y_save = y;
            }
            else if(strcmp(emission, "move,end") == 0
                    || strcmp(emission, "resize,end") == 0)
            {
                presentation_thumbscroll_set(1);
                _current_action = ACTION_NONE;
            }
            else if(strcmp(emission, "move") == 0
                    && (_current_action == ACTION_AREA_MOVE || _current_action == ACTION_AREA_RESIZE) )
            {
                int x,y, x_pres, y_pres, w_pres, h_pres, x_evas, y_evas;
                double rel_x, rel_y;
                double rel1_x, rel1_y, rel2_x, rel2_y;
                ecore_x_pointer_last_xy_get(&x,&y);
                evas_object_geometry_get(pres, &x_pres, &y_pres, &w_pres, &h_pres);
                ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas_object_evas_get(win)),
                        &x_evas,&y_evas, NULL, NULL);

                x = x - x_pres - x_evas;
                y = y - y_pres - y_evas;
                if(x<0 || x>= w_pres || y<0 || y>=h_pres)
                    break;

                x -= _x_save;
                y -= _y_save;
                rel_x = x / (double)w_pres;
                rel_y = y / (double)h_pres;

                eyelight_edit_area_geometry_get(edit, &rel1_x, &rel1_y, &rel2_x, &rel2_y);
                if(_current_action == ACTION_AREA_MOVE)
                {
                    rel1_x += rel_x;
                    rel1_y += rel_y;
                }
                rel2_x += rel_x;
                rel2_y += rel_y;
                eyelight_edit_area_move(edit, rel1_x, rel1_y, rel2_x, rel2_y);

                _x_save = x + _x_save;
                _y_save = y + _y_save;
            }
            break;
        case EYELIGHT_NAME_IMAGE:
            if(strcmp(emission, "select") == 0)
            {
                char *file;
                int shadow, border, keep_aspect;
                rightpanel_image_show();
                eyelight_edit_image_properties_get(edit, &file, &border, &shadow, &keep_aspect);
                rightpanel_image_data_set(file, border, shadow, keep_aspect);
            }
            break;
        default: printf("DEFAULT %p %d\n", _current_obj, eyelight_edit_name_get(_current_obj));
    }
}

void utils_slide_change_cb(Eyelight_Viewer *pres, int old_slide, int new_slide, void *data)
{
    _current_obj = NULL;
    utils_obj_unselect();
}

void utils_obj_unselect()
{
    if(!_current_obj) return;

    eyelight_edit_obj_unselect(_current_obj);
    rightpanel_empty_show();
}

void utils_edit_area_up(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_CUSTOM_AREA
            && eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_THEME_AREA)
        return ;

    eyelight_edit_area_up(_current_obj);
}

void utils_edit_area_down(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_CUSTOM_AREA
            && eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_THEME_AREA)
        return ;

    eyelight_edit_area_down(_current_obj);
}

void utils_edit_area_add(void *data, Evas_Object *obj, void *event_info)
{
    eyelight_edit_area_add(eyelight_object_pres_get(pres), eyelight_object_current_id_get(pres));
}

void utils_edit_slide_default_areas_reinit(void *data, Evas_Object *obj, void *event_info)
{
    utils_obj_unselect();
    eyelight_edit_slide_default_areas_reinit(eyelight_object_pres_get(pres), eyelight_object_current_id_get(pres));
}

void utils_edit_area_delete(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *lbl, *tb, *bt;

    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_CUSTOM_AREA
            && eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_THEME_AREA)
        return ;

    _inwin_delete_slide = elm_win_inwin_add(win);
    evas_object_show(_inwin_delete_slide);
    elm_win_inwin_style_set(_inwin_delete_slide, "minimal");

    tb = elm_table_add(win);
    elm_win_inwin_content_set(_inwin_delete_slide, tb);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Are you sure you want to delete this area ?"));
    elm_table_pack(tb, lbl, 0, 2, 2, 1);
    evas_object_show(lbl);

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 3, 1, 1);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("Yes, Delete the area"));
    evas_object_smart_callback_add(bt, "clicked", _utils_area_delete_cb, NULL);
    evas_object_color_set(bt, 255, 0, 0, 255);
    elm_table_pack(tb, bt, 0, 4, 1, 1);
    evas_object_show(bt);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("No, do not delete the area"));
    evas_object_smart_callback_add(bt, "clicked", _utils_area_delete_cancel_cb, NULL);
    elm_table_pack(tb, bt, 1, 4, 1, 1);
    evas_object_show(bt);
}

static void _utils_area_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_object_del(_inwin_delete_slide);
}

static void _utils_area_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
    Eyelight_Edit *current_obj = _current_obj;
    utils_obj_unselect();
    eyelight_edit_area_delete(current_obj);
    evas_object_del(_inwin_delete_slide);
}


void utils_edit_area_image_add(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_CUSTOM_AREA
            && eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_THEME_AREA)
        return ;

    Evas_Object *inwin, *fs, *fr;

    setlocale(LC_ALL, "");

    inwin = elm_win_inwin_add(win);
    evas_object_show(inwin);

    fr = elm_frame_add(inwin);
    elm_win_inwin_content_set(inwin, fr);
    elm_frame_label_set(fr, D_("Select an image"));

    fs = elm_fileselector_add(win);
    elm_fileselector_expandable_set(fs, EINA_FALSE);
    elm_fileselector_path_set(fs, getenv("HOME"));
    evas_object_size_hint_weight_set(fs, 1.0, 1.0);
    elm_frame_content_set(fr, fs);
    evas_object_show(fs);
    evas_object_smart_callback_add(fs, "done", _utils_edit_area_image_add_cb, inwin);
}

static void _utils_edit_area_image_add_cb(void *data, Evas_Object *obj, void *event_info)
{
    const char *selected = event_info;

    if (selected)
    {
        char *file = utils_file_move_in_pres(selected);

        eyelight_edit_area_image_add(eyelight_object_pres_get(pres),_current_obj, file);
        free(file);
    }

    evas_object_del(data);
}

void utils_edit_area_layout_vertical_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Vertical"));
    eyelight_edit_area_layout_set(_current_obj, "vertical");
}

void utils_edit_area_layout_vertical_homogeneous_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Vertical homogeneous"));
    eyelight_edit_area_layout_set(_current_obj, "vertical_homogeneous");
}

void utils_edit_area_layout_vertical_flow_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Vertical flow"));
    eyelight_edit_area_layout_set(_current_obj, "vertical_flow");
}

void utils_edit_area_layout_horizontal_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Horizontal"));
    eyelight_edit_area_layout_set(_current_obj, "horizontal");
}

void utils_edit_area_layout_horizontal_homogeneous_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Horizontal homogenous"));
    eyelight_edit_area_layout_set(_current_obj, "horizontal_homogeneous");
}

void utils_edit_area_layout_horizontal_flow_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Horizontal flow"));
    eyelight_edit_area_layout_set(_current_obj, "horizontal_flow");
}

void utils_edit_area_layout_stack_set (void *data, Evas_Object *obj, void *event_info)
{
    rightpanel_area_layout_set(D_("Stack"));
    eyelight_edit_area_layout_set(_current_obj, "stack");
}




void utils_edit_image_file_change(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_IMAGE)
        return ;

    Evas_Object *inwin, *fs;

    setlocale(LC_ALL, "");

    inwin = elm_win_inwin_add(win);
    evas_object_show(inwin);

    fs = elm_fileselector_add(win);
    elm_fileselector_expandable_set(fs, EINA_FALSE);
    /* start the fileselector in the home dir */
    elm_fileselector_path_set(fs, getenv("HOME"));
    /* allow fs to expand in x & y */
    evas_object_size_hint_weight_set(fs, 1.0, 1.0);
    elm_win_resize_object_add(win, fs);
    evas_object_show(fs);
    /* the 'done' cb is called when the user press ok/cancel */
    evas_object_smart_callback_add(fs, "done", _utils_edit_image_file_change_cb, inwin);

    elm_win_inwin_content_set(inwin, fs);
}

static void _utils_edit_image_file_change_cb(void *data, Evas_Object *obj, void *event_info)
{
    const char *selected = event_info;

    if (selected)
    {
        char *file = utils_file_move_in_pres(selected);

        eyelight_edit_image_properties_set(eyelight_object_pres_get(pres),_current_obj, file, -1, -1, -1);
        free(file);
    }

    evas_object_del(data);
}

void utils_edit_image_border_change(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_IMAGE)
        return ;

    int v = elm_check_state_get(obj);
    eyelight_edit_image_properties_set(eyelight_object_pres_get(pres), _current_obj, NULL, v, -1, -1);
}

void utils_edit_image_shadow_change(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_IMAGE)
        return ;

    int v = elm_check_state_get(obj);
    eyelight_edit_image_properties_set(eyelight_object_pres_get(pres), _current_obj, NULL, -1, v, -1);
}

void utils_edit_image_keep_aspect_change(void *data, Evas_Object *obj, void *event_info)
{
    if(eyelight_edit_name_get(_current_obj) != EYELIGHT_NAME_IMAGE)
        return ;

    int v = elm_check_state_get(obj);
    eyelight_edit_image_properties_set(eyelight_object_pres_get(pres), _current_obj, NULL, -1, -1, v);
}

char *utils_file_move_in_pres(const char *file)
{
    const char *pres_file = eyelight_object_presentation_file_get(pres);
    char *dir = ecore_file_dir_get(pres_file);
    char *res;
    int ok = 1;
    int i;

    //test if file is in a subdirectory of dir
    for(i=0;i<strlen(dir);i++)
    {
        if(i >= strlen(file)
                || dir[i] != file[i])
        {
            ok = 0;
            break;
        }
        else
            i++;
    }

    if(!ok)
    {
        const char* file_name = ecore_file_file_get(file);
        char buf[PATH_MAX];
        res = calloc(PATH_MAX, sizeof(char));
        snprintf(buf, PATH_MAX, "%s/images/", dir);
        if(!ecore_file_exists(buf))
            ecore_file_mkdir(buf);

        snprintf(buf, PATH_MAX, "%s/images/%s", dir, file_name);
        ecore_file_cp(file, buf);

        snprintf(res,PATH_MAX,"images/%s", file_name);
    }
    else
    {
        const char *_file = file + strlen(dir) + 1;
        res = strdup(_file);
    }

    EYELIGHT_FREE(dir);

    return res;
}

void utils_edit_object_delete(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *lbl, *tb, *bt;

    _inwin_delete_slide = elm_win_inwin_add(win);
    evas_object_show(_inwin_delete_slide);
    elm_win_inwin_style_set(_inwin_delete_slide, "minimal");

    tb = elm_table_add(win);
    elm_win_inwin_content_set(_inwin_delete_slide, tb);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Are you sure you want to delete this object ?"));
    elm_table_pack(tb, lbl, 0, 2, 2, 1);
    evas_object_show(lbl);

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 3, 1, 1);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("Yes, Delete the object"));
    evas_object_smart_callback_add(bt, "clicked", _utils_object_delete_cb, NULL);
    evas_object_color_set(bt, 255, 0, 0, 255);
    elm_table_pack(tb, bt, 0, 4, 1, 1);
    evas_object_show(bt);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("No, do not delete the object"));
    evas_object_smart_callback_add(bt, "clicked", _utils_object_delete_cancel_cb, NULL);
    elm_table_pack(tb, bt, 1, 4, 1, 1);
    evas_object_show(bt);
}

static void _utils_object_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_object_del(_inwin_delete_slide);
}

static void _utils_object_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
    Eyelight_Edit *current_obj = _current_obj;
    utils_obj_unselect();
    eyelight_edit_object_delete(current_obj);
    evas_object_del(_inwin_delete_slide);
}


void utils_edit_object_down(void *data, Evas_Object *obj, void *event_info)
{
    eyelight_edit_object_down(eyelight_object_pres_get(pres), _current_obj);
}

void utils_edit_object_up(void *data, Evas_Object *obj, void *event_info)
{
    eyelight_edit_object_up(eyelight_object_pres_get(pres), _current_obj);
}
