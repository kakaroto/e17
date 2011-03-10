/*
 * =====================================================================================
 *
 *       Filename:  eyelight_object.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/12/2009 11:56:25 AM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#elif defined __GNUC__
# define alloca __builtin_alloca
#elif defined _AIX
# define alloca __alloca
#elif defined _MSC_VER
# include <malloc.h>
# define alloca _alloca
#else
# include <stddef.h>
# ifdef  __cplusplus
extern "C"
# endif
void *alloca (size_t);
#endif

#define EDJE_EDIT_IS_UNSTABLE_AND_I_KNOW_ABOUT_IT 1
#include <Edje_Edit.h>
#include <Emotion.h>

#include "Eyelight_Smart.h"
#include "eyelight_object.h"

/**
 * Returns the object which contains the area "area" <br>
 * If "area" is not a custom area, the method return slide->obj.
 * Because the area is supposed to be a default area from the theme layout <br>
 * The method also return the complete area's name: "area.name".
 * complete_area_name must be allocated with the size EYELIGHT_BUFLEN
 */
Evas_Object *eyelight_object_area_obj_get(Eyelight_Viewer *pres, Eyelight_Slide *slide, const char *area, char *complete_area_name)
{
    Eina_List *l;
    Eyelight_Area *e_area;
    EINA_LIST_FOREACH(slide->areas, l, e_area)
    {
        if(strcmp(e_area->name, area) == 0)
        {
            snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.custom");
            return e_area->obj;
        }
    }

    snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.%s", area);
    return slide->obj;
}

Evas_Object *eyelight_object_title_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *default_text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    //set the title
    Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
    const char * title = NULL;
    if(node_title)
        title = eyelight_retrieve_value_of_prop(node_title,0);
    else if(!node_title && default_text)
        title = default_text;

    if(title)
    {
        Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "title", buf);

        Evas_Object *o_title = edje_object_add(pres->evas);
        if(edje_object_file_set(o_title, pres->theme, "eyelight/title") ==  0)
            WARN("load group eyelight/title error! %d \n",
                    edje_object_load_error_get(o_title));
        edje_object_part_text_set(o_title,"object.title",title);
	edje_object_size_min_calc(o_title, &w, &h);
	evas_object_size_hint_min_set(o_title, w, h);
        evas_object_size_hint_align_set(o_title, -1, -1);
        evas_object_size_hint_weight_set(o_title, -1, -1);
        edje_object_scale_set(o_title, pres->current_scale);
        evas_object_show(o_title);
        edje_object_part_box_append(o_area,buf,o_title);

        if(pres->edit_mode)
        {
            node->obj = o_title;
            edje_object_signal_emit(o_title, "edit,mode,1", "eyelight");
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_title;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_title, "select", "image", pres->edit_cb, edit);
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_title);


        return o_title;
    }
    return NULL;
}

Evas_Object *eyelight_object_subtitle_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *default_text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Eyelight_Node *node_subtitle = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_SUBTITLE);
    const char * subtitle = NULL;
    if(node_subtitle)
        subtitle = eyelight_retrieve_value_of_prop(node_subtitle,0);
    else if(!node_subtitle && default_text)
        subtitle = default_text;
    if(subtitle)
    {
        Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "subtitle", buf);

        Evas_Object *o_subtitle = edje_object_add(pres->evas);
        if(edje_object_file_set(o_subtitle, pres->theme, "eyelight/subtitle") ==  0)
            WARN("load group eyelight/subtitle error! %d \n",
                    edje_object_load_error_get(o_subtitle));
        edje_object_part_text_set(o_subtitle,"object.subtitle",subtitle);
	edje_object_size_min_calc(o_subtitle, &w, &h);
	evas_object_size_hint_min_set(o_subtitle, w, h);
        evas_object_size_hint_align_set(o_subtitle, -1, -1);
        evas_object_size_hint_weight_set(o_subtitle, -1, -1);
        edje_object_scale_set(o_subtitle, pres->current_scale);
        evas_object_show(o_subtitle);
        edje_object_part_box_append(o_area, buf,o_subtitle);

        if(pres->edit_mode)
        {
            node->obj = o_subtitle;
            edje_object_signal_emit(o_subtitle, "edit,mode,1", "eyelight");
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_subtitle;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_subtitle, "select", "image", pres->edit_cb, edit);
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_subtitle);
        return o_subtitle;
    }
    return NULL;
}

Evas_Object *eyelight_object_header_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *default_image)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Eyelight_Node *node_header_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_HEADER_IMAGE);
    const char * header_image = NULL;
    if(node_header_image)
        header_image = eyelight_retrieve_value_of_prop(node_header_image,0);
    else if(!node_header_image && default_image)
        header_image = default_image;
    if(header_image)
    {
        Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "header_image", buf);

        Evas_Object *o_header_image = edje_object_add(pres->evas);
        if(edje_object_file_set(o_header_image, pres->theme, "eyelight/header_image") ==  0)
            WARN("load group eyelight/header_image error! %d \n",
                    edje_object_load_error_get(o_header_image));
        const Evas_Object *part_image = edje_object_part_object_get(o_header_image, "object.header_image");
        char *image_path = eyelight_compile_image_path_new(pres,header_image);
        evas_object_image_file_set((Evas_Object*)part_image, image_path, pres->dump_in || pres->dump_out ? header_image : NULL);
        EYELIGHT_FREE(image_path);
	edje_object_size_min_calc(o_header_image, &w, &h);
	evas_object_size_hint_min_set(o_header_image, w, h);
        evas_object_size_hint_align_set(o_header_image, -1, -1);
        evas_object_size_hint_weight_set(o_header_image, -1, -1);
        edje_object_scale_set(o_header_image, pres->current_scale);
        evas_object_show(o_header_image);
        edje_object_part_box_append(o_area, buf,o_header_image);

        if(pres->edit_mode)
        {
            node->obj = o_header_image;
            edje_object_signal_emit(o_header_image, "edit,mode,1", "eyelight");
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_header_image;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_header_image, "select", "image", pres->edit_cb, edit);
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_header_image);

        return o_header_image;
    }

    return NULL;
}

Evas_Object *eyelight_object_foot_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *default_text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Eyelight_Node *node_foot_text = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_TEXT);
    const char * foot_text = NULL;
    if(node_foot_text)
        foot_text = eyelight_retrieve_value_of_prop(node_foot_text,0);
    else if(!node_foot_text && default_text)
        foot_text = default_text;
    if(foot_text)
    {
        Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "foot_text", buf);

        Evas_Object *o_foot_text = edje_object_add(pres->evas);
        if(edje_object_file_set(o_foot_text, pres->theme, "eyelight/foot_text") ==  0)
            WARN("load group eyelight/foot_text error! %d \n",
                    edje_object_load_error_get(o_foot_text));
        edje_object_part_text_set(o_foot_text,"object.foot_text",foot_text);
	edje_object_size_min_calc(o_foot_text, &w, &h);
	evas_object_size_hint_min_set(o_foot_text, w, h);
        evas_object_size_hint_align_set(o_foot_text, -1, -1);
        evas_object_size_hint_weight_set(o_foot_text, -1, -1);
        edje_object_scale_set(o_foot_text, pres->current_scale);
        evas_object_show(o_foot_text);
        edje_object_part_box_append(o_area,buf,o_foot_text);

        if(pres->edit_mode)
        {
            node->obj = o_foot_text;
            edje_object_signal_emit(o_foot_text, "edit,mode,1", "eyelight");
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_foot_text;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_foot_text, "select", "image", pres->edit_cb, edit);
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_foot_text);
        return o_foot_text;
    }
    return NULL;
}


Evas_Object *eyelight_object_foot_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *default_image)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Eyelight_Node *node_foot_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_IMAGE);
    const char * foot_image = NULL;
    if(node_foot_image)
        foot_image = eyelight_retrieve_value_of_prop(node_foot_image,0);
    else if(!node_foot_image && default_image)
        foot_image = default_image;
    if(foot_image)
    {
        Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "foot_image", buf);

        Evas_Object *o_foot_image = edje_object_add(pres->evas);
        if(edje_object_file_set(o_foot_image, pres->theme, "eyelight/foot_image") ==  0)
            WARN("load group eyelight/foot_image error! %d \n",
                    edje_object_load_error_get(o_foot_image));
        const Evas_Object *part_image = edje_object_part_object_get(o_foot_image, "object.foot_image");
        char *image_path = eyelight_compile_image_path_new(pres,foot_image);
        evas_object_image_file_set((Evas_Object*)part_image, image_path, pres->dump_in || pres->dump_out ? foot_image : NULL);
        EYELIGHT_FREE(image_path);
	edje_object_size_min_calc(o_foot_image, &w, &h);
	evas_object_size_hint_min_set(o_foot_image, w, h);
        evas_object_size_hint_align_set(o_foot_image, -1, -1);
        evas_object_size_hint_weight_set(o_foot_image, -1, -1);
        edje_object_scale_set(o_foot_image, pres->current_scale);
        evas_object_show(o_foot_image);
        edje_object_part_box_append(o_area, buf,o_foot_image);

        if(pres->edit_mode)
        {
            node->obj = o_foot_image;
            edje_object_signal_emit(o_foot_image, "edit,mode,1", "eyelight");
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_foot_image;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_foot_image, "select", "image", pres->edit_cb, edit);
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_foot_image);

        return o_foot_image;
    }

    return NULL;
}


Evas_Object *eyelight_object_pages_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int slide_number, int nb_slides)
{
    char buf[1024], buf_area[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, "pages", buf_area);
    snprintf(buf,1024," %d/%d",slide_number+1,nb_slides);
    Evas_Object *o_pages = edje_object_add(pres->evas);
    if(edje_object_file_set(o_pages, pres->theme, "eyelight/pages") ==  0)
        WARN("load group eyelight/pages error! %d \n",
                edje_object_load_error_get(o_pages));
    edje_object_part_text_set(o_pages,"object.pages",buf);
    edje_object_size_min_calc(o_pages, &w, &h);
    evas_object_size_hint_min_set(o_pages, w, h);
    evas_object_size_hint_align_set(o_pages, -1, -1);
    evas_object_size_hint_weight_set(o_pages, -1, -1);
    edje_object_scale_set(o_pages, pres->current_scale);
    evas_object_show(o_pages);
    edje_object_part_box_append(o_area, buf_area,o_pages);

    slide->items_all = eina_list_append(
            slide->items_all,
            o_pages);
    return o_pages;
}



Evas_Object *eyelight_object_item_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_text") ==  0)
        WARN("load group eyelight/item_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);

    edje_object_size_min_calc(o_text, &w, &h);
    evas_object_size_hint_min_set(o_text, w, h);
    evas_object_size_hint_align_set(o_text, -1.0, 0.0);
    evas_object_size_hint_weight_set(o_text, 1.0, 1.0);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);
    edje_object_part_box_append(o_area,buf,o_text);

    if(pres->edit_mode)
    {
        node->obj = o_text;
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_text;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}

Evas_Object *eyelight_object_item_code_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_code") ==  0)
        WARN("load group eyelight/item_code error! %d \n",
                edje_object_load_error_get(o_text));

    //TODO: HERE we can parse and set colors to the text !
    edje_object_part_text_set(o_text,"object.text",text);

    edje_object_size_min_calc(o_text, &w, &h);
    evas_object_size_hint_min_set(o_text, w, h);

    evas_object_size_hint_align_set(o_text, -1.0, 0.0);
    evas_object_size_hint_weight_set(o_text, 1.0, 1.0);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);
    edje_object_part_box_append(o_area,buf,o_text);

    if(pres->edit_mode)
    {
        node->obj = o_text;
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_text;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}

Evas_Object *eyelight_object_item_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *image, int border, int shadow, double aspect_x, double aspect_y, int keep_aspect)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_image = edje_object_add(pres->evas);
    if(edje_object_file_set(o_image, pres->theme, "eyelight/item_image") ==  0)
        WARN("load group eyelight/item_image error! %d \n",
                edje_object_load_error_get(o_image));
    const Evas_Object *part_image = edje_object_part_object_get(o_image, "object.image");
    char *image_path = eyelight_compile_image_path_new(pres,image);
    evas_object_image_file_set((Evas_Object*)part_image, image_path, pres->dump_in || pres->dump_out ? image : NULL);
    edje_object_size_min_calc(o_image, &w, &h);
    evas_object_size_hint_min_set(o_image, w, h);
    evas_object_size_hint_align_set(o_image, -1, -1);
    evas_object_size_hint_weight_set(o_image, -1, -1);
    edje_object_scale_set(o_image, pres->current_scale);
    evas_object_show(o_image);
    edje_object_part_box_append(o_area,buf,o_image);

    if(border)
        edje_object_signal_emit(o_image, "border,show","eyelight");
    if(shadow)
        edje_object_signal_emit(o_image, "shadow,show","eyelight");


    if(keep_aspect)
    {
        Evas_Coord w, h;
        Evas_Object *o = evas_object_image_add(evas_object_evas_get(o_area));
        evas_object_image_file_set(o, image_path,  pres->dump_in || pres->dump_out ? image : NULL);
        evas_object_image_size_get(o, &w, &h);
        evas_object_del(o);
        aspect_x = w/(float)h;
        aspect_y = 1;
    }

    Edje_Message_Float_Set *msg = alloca(sizeof(Edje_Message_Float_Set) + (1 * sizeof(float)));
    msg->count = 2;
    msg->val[0] = aspect_x;
    msg->val[1] = aspect_y;
    edje_object_message_send(o_image,EDJE_MESSAGE_FLOAT_SET , 0, msg);

    if(pres->edit_mode)
    {
        node->obj = o_image;
        edje_object_signal_emit(o_image, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_image;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_image, "select", "image", pres->edit_cb, edit);
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_image);

    EYELIGHT_FREE(image_path);
    return o_image;
}

/**   Callbacks used by a video item */


void _video_play_cb(void *data, Evas_Object *o, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,1);
    edje_object_signal_emit(o, "play", "video_state");
}

void _video_pause_cb(void *data, Evas_Object *o, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,0);
    edje_object_signal_emit(o, "pause", "video_state");
}

void _video_stop_cb(void *data, Evas_Object *o, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,0);
    emotion_object_position_set(ov, 0);
    edje_object_signal_emit(o, "stop", "video_state");
    edje_object_part_drag_value_set(ov, "object.slider", 0.0, 0.0);
}

void _video_slider_cb(void *data, Evas_Object *o, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    double len;
    double x, y;

    Evas_Object *ov = data;

    edje_object_part_drag_value_get(o, "object.slider", &x, &y);
    len = emotion_object_play_length_get(ov);
    emotion_object_position_set(ov, x * len);
    _video_play_cb(o, ov, NULL,NULL);
}

void _video_obj_progress_cb(void *data, Evas_Object *o, void  *event_info __UNUSED__)
{
    double len, pos, scale;

    Evas_Object *ov = data;

    pos = emotion_object_position_get(o);
    len = emotion_object_play_length_get(o);
    scale = (len > 0.0) ? pos / len : 0.0;
    edje_object_part_drag_value_set(ov, "object.slider", scale, 0.0);
}

void _video_obj_replay_cb(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
    Eyelight_Video *e_video = data;
    emotion_object_position_set(e_video->o_inter,0);
    if(e_video->replay)
    {
        _video_play_cb(e_video->o_inter, e_video->o_video, NULL,NULL);
    }
}

Evas_Object *eyelight_object_item_presentation_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *presentation, const char *theme, int border, int shadow)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_pres = edje_object_add(pres->evas);
    if(edje_object_file_set(o_pres, pres->theme, "eyelight/item_presentation") ==  0)
        WARN("load group eyelight/item_presentation error! %d \n",
                edje_object_load_error_get(o_pres));

    Evas_Object *o_inter = eyelight_object_add(pres->evas);
    eyelight_object_theme_file_set(o_inter, theme);
    eyelight_object_presentation_file_set(o_inter, presentation);
    eyelight_object_event_set(o_inter,1);
    evas_object_show(o_inter);

    edje_object_part_swallow(o_pres,"object.swallow",o_inter);

    if(shadow)
        edje_object_signal_emit(o_pres, "shadow,show","eyelight");
    if(border)
        edje_object_signal_emit(o_pres, "border,show","eyelight");

    edje_object_size_min_calc(o_pres, &w, &h);
    evas_object_size_hint_min_set(o_pres, w, h);
    evas_object_size_hint_align_set(o_pres, -1, -1);
    evas_object_size_hint_weight_set(o_pres, -1, -1);
    evas_object_show(o_pres);
    edje_object_part_box_append(o_area,buf,o_pres);

    if(pres->edit_mode)
    {
        node->obj = o_pres;
        edje_object_signal_emit(o_pres, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_pres;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_pres, "select", "image", pres->edit_cb, edit);
    }

    slide->items_edje = eina_list_append(
            slide->items_edje, o_inter);

    slide->items_all = eina_list_append(
            slide->items_all,
            o_pres);
    return o_pres;
}

/** */
Evas_Object *eyelight_object_item_video_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *video, int alpha, int autoplay, int replay, int border, int shadow)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_video = edje_object_add(pres->evas);
    if(edje_object_file_set(o_video, pres->theme, "eyelight/item_video") ==  0)
        WARN("load group eyelight/item_video error! %d \n",
                edje_object_load_error_get(o_video));


    char *video_path = eyelight_compile_object_path_new(pres,video);

    if(!ecore_file_exists(video_path))
        WARN("Video %s not found !\n",video_path);

    /* basic video object setup */
    Evas_Object *o_inter = emotion_object_add(pres->evas);
    if (!emotion_object_init(o_inter, NULL))
    {
        ERR("Emotion init failed !\n");
        exit (0);
    }
    emotion_object_vis_set(o_inter, EMOTION_VIS_NONE);
    emotion_object_file_set(o_inter, video_path);
    evas_object_move(o_inter, 0, 0);
    evas_object_resize(o_inter, 320, 240);
    emotion_object_smooth_scale_set(o_inter, 1);
    evas_object_show(o_inter);
    edje_object_signal_emit(o_inter, "video_state", "play");
    evas_object_repeat_events_set(o_inter,1);

    Eyelight_Video *e_video = calloc(1,sizeof(Eyelight_Video));
    e_video->video = (char*)video;
    e_video->o_inter = o_inter;
    e_video->o_video = o_video;
    e_video->replay = replay;
    e_video->pres = pres;

    evas_object_smart_callback_add(o_inter, "position_update", _video_obj_progress_cb, o_video);
    evas_object_smart_callback_add(o_inter, "playback_finished", _video_obj_replay_cb, e_video);

    edje_object_signal_callback_add (o_video,"play","video_control",_video_play_cb,o_inter);
    edje_object_signal_callback_add (o_video,"pause","video_control",_video_pause_cb,o_inter);
    edje_object_signal_callback_add (o_video,"stop","video_control",_video_stop_cb,o_inter);
    edje_object_signal_callback_add (o_video,"move","video_control",_video_slider_cb,o_inter);

    if(autoplay)
        _video_play_cb(o_inter, o_video, NULL,NULL);
    else
        _video_stop_cb(o_inter, o_video, NULL, NULL);

    if(shadow)
        edje_object_signal_emit(o_video, "shadow,show","eyelight");
    if(border)
        edje_object_signal_emit(o_video, "border,show","eyelight");
    if(pres->edit_mode)
    {
        node->obj = o_video;
        edje_object_signal_emit(o_video, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_video;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_video, "select", "image", pres->edit_cb, edit);
    }

    EYELIGHT_FREE(video_path);
    edje_object_part_swallow(o_video,"object.swallow",o_inter);

    evas_object_color_set(o_inter, 255, 255, 255, alpha);

    edje_object_size_min_calc(o_video, &w, &h);
    evas_object_size_hint_min_set(o_video, w, h);
    evas_object_size_hint_align_set(o_video, -1, -1);
    evas_object_size_hint_weight_set(o_video, -1, -1);
    edje_object_scale_set(o_video, pres->current_scale);
    evas_object_show(o_video);
    edje_object_part_box_append(o_area,buf,o_video);

    slide->items_video = eina_list_append(
            slide->items_video,
            e_video);
    slide->items_all = eina_list_append(
            slide->items_all,
            o_video);
    return o_video;
}

Evas_Object *eyelight_object_item_simple_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_simple_text") ==  0)
        WARN("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    edje_object_size_min_calc(o_text, &w, &h);
    evas_object_size_hint_min_set(o_text, w, h);
    evas_object_size_hint_align_set(o_text, -1.0, 0.0);
    evas_object_size_hint_weight_set(o_text, 1.0, 1.0);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);

    edje_object_part_box_append(o_area,buf,o_text);

    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");

    if(pres->edit_mode)
    {
        node->obj = o_text;
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_text;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}


Evas_Object *eyelight_object_item_numbering_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, char *numbering_id, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN],buf2[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_numbering_text") ==  0)
        WARN("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    edje_object_size_min_calc(o_text, &w, &h);
    evas_object_size_hint_min_set(o_text, w, h);

    snprintf(buf2,EYELIGHT_BUFLEN, " %s) ", numbering_id);
    edje_object_part_text_set(o_text,"object.numbering",buf2);

    evas_object_size_hint_align_set(o_text, -1, 0.0);
    evas_object_size_hint_weight_set(o_text, 1, 1);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);

    edje_object_part_box_append(o_area,buf,o_text);


    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");
    if(pres->edit_mode)
    {
        node->obj = o_text;
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_text;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}


Evas_Object *eyelight_object_item_edje_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, const char *edje_file, const char *edje_group)
{
    char buf[EYELIGHT_BUFLEN];
    int w, h;

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, area, buf);

    Evas_Object *o_edje = edje_object_add(pres->evas);
    if(edje_object_file_set(o_edje, pres->theme, "eyelight/item_edje") ==  0)
        WARN("load group eyelight/item_edje error! %d \n",
                edje_object_load_error_get(o_edje));

    char *edje_path = eyelight_compile_object_path_new(pres,edje_file);
    Evas_Object *o_inter = edje_object_add(pres->evas);
    edje_object_scale_set(o_inter, pres->current_scale);

    if(edje_object_file_set(o_inter, edje_path, edje_group) ==  0)
        WARN("load group %s from %s error! %d \n",
                edje_group,edje_path,
                edje_object_load_error_get(o_edje));

    edje_object_part_swallow(o_edje,"object.swallow",o_inter);

    EYELIGHT_FREE(edje_path);

    edje_object_size_min_calc(o_edje, &w, &h);
    evas_object_size_hint_min_set(o_edje, w, h);
    evas_object_size_hint_align_set(o_edje, -1, -1);
    evas_object_size_hint_weight_set(o_edje, -1, -1);
    edje_object_scale_set(o_edje, pres->current_scale);
    evas_object_show(o_edje);
    edje_object_part_box_append(o_area,buf,o_edje);

    if(pres->edit_mode)
    {
        node->obj = o_edje;

        edje_object_signal_emit(o_edje, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_edje;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
            edje_object_signal_callback_add(o_edje, "select", "image", pres->edit_cb, edit);
    }

    slide->items_edje = eina_list_append(
            slide->items_edje,
            o_inter);
    slide->items_all = eina_list_append(
            slide->items_all,
            o_edje);
    return o_edje;
}

Evas_Object *eyelight_object_custom_area_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, Eyelight_Node *node, const char *area, double rel1_x, double rel1_y, double rel2_x, double rel2_y)
{
    int w, h;

    Evas_Object *o_area = edje_object_add(pres->evas);
    if(edje_object_file_set(o_area, pres->theme, "eyelight/custom_area") ==  0)
        WARN("load group eyelight/custom_area error! %d \n",
                edje_object_load_error_get(o_area));

    edje_object_size_min_calc(o_area, &w, &h);
    evas_object_size_hint_min_set(o_area, w, h);
    evas_object_size_hint_align_set(o_area, -1, -1);
    evas_object_size_hint_weight_set(o_area, -1, -1);
    edje_object_scale_set(o_area, pres->current_scale);
    evas_object_show(o_area);

    //set the size
    Edje_Message_Float_Set *msg = alloca(sizeof(Edje_Message_Float_Set) + (3 * sizeof(float)));
    msg->count=4;
    msg->val[0] = rel1_x;
    msg->val[1] = rel1_y;
    msg->val[2] = rel2_x;
    msg->val[3] = rel2_y;
    edje_object_message_send(o_area,EDJE_MESSAGE_FLOAT_SET , 0, msg);

    //set the name
    edje_object_part_text_set(o_area,"area.custom.name",area);

    edje_object_part_box_append(slide->obj,"area.custom",o_area);

    if(pres->with_border)
        edje_object_signal_emit(o_area,"border,show", "eyelight");
    if(pres->edit_mode)
    {
        node->obj = o_area;

        edje_object_signal_emit(o_area, "edit,mode,1", "eyelight");
        Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
        edit->obj = o_area;
        edit->node = node;
        edit->slide = slide;
        edit->data = pres->edit_data;
        slide->edits = eina_list_append(slide->edits, edit);

        if(pres->edit_cb)
        {
            edje_object_signal_callback_add(o_area, "select", "area", pres->edit_cb, edit);
            edje_object_signal_callback_add(o_area, "move,start", "area", pres->edit_cb, edit);
            edje_object_signal_callback_add(o_area, "move,end", "area", pres->edit_cb, edit);
            edje_object_signal_callback_add(o_area, "resize,start", "area", pres->edit_cb, edit);
            edje_object_signal_callback_add(o_area, "resize,end", "area", pres->edit_cb, edit);
            edje_object_signal_callback_add(o_area, "move", "area", pres->edit_cb, edit);
        }
    }

    Eyelight_Area *e_area = calloc(1,sizeof(Eyelight_Area));
    e_area->obj = o_area;
    e_area->name = strdup(area);
    e_area->node_def = node;


    slide->areas = eina_list_append(
            slide->areas,
            e_area);
    slide->items_all = eina_list_append(
            slide->items_all,
            o_area);
    return o_area;
}



