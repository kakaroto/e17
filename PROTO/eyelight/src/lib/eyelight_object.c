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

#include "eyelight_object.h"
#include <Edje_Edit.h>
#include <Emotion.h>
#include "Eyelight_Smart.h"

/**
 * Returns the object which contains the area "area" <br>
 * If "area" is not a custom area, the method return slide->obj.
 * Because the area is supposed to be a default area from the theme layout <br>
 * The method also return the complete area's name: "area.name".
 * complete_area_name must be allocate with the size EYELIGHT_BUFLEN
 */
Evas_Object *eyelight_object_area_obj_get(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, const char *area, char *complete_area_name)
{
    Eina_List *l;
    Eyelight_Custom_Area *custom_area;
    EINA_LIST_FOREACH(slide->custom_areas, l, custom_area)
    {
        if(strcmp(custom_area->name, area) == 0)
        {
            snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.custom");
            return custom_area->obj;
        }
    }

    snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.%s", area);
    return slide->obj;
}


Evas_Object *eyelight_object_title_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *default_text)
{
    //set the title
    Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
    const char * title = NULL;
    if(node_title)
        title = eyelight_retrieve_value_of_prop(node_title,0);
    else if(!node_title && default_text)
        title = default_text;
    if(title)
    {
        Evas_Object *o_title = edje_object_add(pres->evas);
        if(edje_object_file_set(o_title, pres->theme, "eyelight/title") ==  0)
            printf("load group eyelight/title error! %d \n",
                    edje_object_load_error_get(o_title));
        edje_object_part_text_set(o_title,"object.title",title);
        evas_object_size_hint_align_set(o_title, -1, -1);
        evas_object_size_hint_weight_set(o_title, -1, -1);
        edje_object_scale_set(o_title, pres->current_scale);
        evas_object_show(o_title);
        edje_object_part_box_append(slide->obj,"area.title",o_title);

        if(pres->edit_mode)
        {
            edje_object_signal_emit(o_title, "edit,mode,1", "eyelight");
            if(pres->edit_mode)
            {
                Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
                edit->obj = o_title;
                edit->node = node;
                edit->slide = slide;
                edit->data = pres->edit_data;
                slide->edits = eina_list_append(slide->edits, edit);

                if(pres->edit_cb)
                    edje_object_signal_callback_add(o_title, "select", "image", pres->edit_cb, edit);
            }
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_title);

        return o_title;
    }
    return NULL;
}

Evas_Object *eyelight_object_subtitle_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *default_text)
{
    Eyelight_Node *node_subtitle = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_SUBTITLE);
    const char * subtitle = NULL;
    if(node_subtitle)
        subtitle = eyelight_retrieve_value_of_prop(node_subtitle,0);
    else if(!node_subtitle && default_text)
        subtitle = default_text;
    if(subtitle)
    {
        Evas_Object *o_subtitle = edje_object_add(pres->evas);
        if(edje_object_file_set(o_subtitle, pres->theme, "eyelight/subtitle") ==  0)
            printf("load group eyelight/subtitle error! %d \n",
                    edje_object_load_error_get(o_subtitle));
        edje_object_part_text_set(o_subtitle,"object.subtitle",subtitle);
        evas_object_size_hint_align_set(o_subtitle, -1, -1);
        evas_object_size_hint_weight_set(o_subtitle, -1, -1);
        edje_object_scale_set(o_subtitle, pres->current_scale);
        evas_object_show(o_subtitle);
        edje_object_part_box_append(slide->obj,"area.subtitle",o_subtitle);

        if(pres->edit_mode)
        {
            edje_object_signal_emit(o_subtitle, "edit,mode,1", "eyelight");
            if(pres->edit_mode)
            {
                Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
                edit->obj = o_subtitle;
                edit->node = node;
                edit->slide = slide;
                edit->data = pres->edit_data;
                slide->edits = eina_list_append(slide->edits, edit);

                if(pres->edit_cb)
                    edje_object_signal_callback_add(o_subtitle, "select", "image", pres->edit_cb, edit);
            }
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_subtitle);
        return o_subtitle;
    }
    return NULL;
}

Evas_Object *eyelight_object_header_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *default_image)
{
    Eyelight_Node *node_header_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_HEADER_IMAGE);
    const char * header_image = NULL;
    if(node_header_image)
        header_image = eyelight_retrieve_value_of_prop(node_header_image,0);
    else if(!node_header_image && default_image)
        header_image = default_image;
    if(header_image)
    {
        Evas_Object *o_header_image = edje_object_add(pres->evas);
        if(edje_object_file_set(o_header_image, pres->theme, "eyelight/header_image") ==  0)
            printf("load group eyelight/header_image error! %d \n",
                    edje_object_load_error_get(o_header_image));
        const Evas_Object *part_image = edje_object_part_object_get(o_header_image, "object.header_image");
        char *image_path = eyelight_compile_image_path_new(pres,header_image);
        evas_object_image_file_set((Evas_Object*)part_image, image_path, NULL);
        EYELIGHT_FREE(image_path);
        evas_object_size_hint_align_set(o_header_image, -1, -1);
        evas_object_size_hint_weight_set(o_header_image, -1, -1);
        edje_object_scale_set(o_header_image, pres->current_scale);
        evas_object_show(o_header_image);
        edje_object_part_box_append(slide->obj,"area.header_image",o_header_image);

        if(pres->edit_mode)
        {
            edje_object_signal_emit(o_header_image, "edit,mode,1", "eyelight");
            if(pres->edit_mode)
            {
                Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
                edit->obj = o_header_image;
                edit->node = node;
                edit->slide = slide;
                edit->data = pres->edit_data;
                slide->edits = eina_list_append(slide->edits, edit);

                if(pres->edit_cb)
                    edje_object_signal_callback_add(o_header_image, "select", "image", pres->edit_cb, edit);
            }
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_header_image);

        return o_header_image;
    }

    return NULL;
}

Evas_Object *eyelight_object_foot_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *default_text)
{
    Eyelight_Node *node_foot_text = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_TEXT);
    const char * foot_text = NULL;
    if(node_foot_text)
        foot_text = eyelight_retrieve_value_of_prop(node_foot_text,0);
    else if(!node_foot_text && default_text)
        foot_text = default_text;
    if(foot_text)
    {
        Evas_Object *o_foot_text = edje_object_add(pres->evas);
        if(edje_object_file_set(o_foot_text, pres->theme, "eyelight/foot_text") ==  0)
            printf("load group eyelight/foot_text error! %d \n",
                    edje_object_load_error_get(o_foot_text));
        edje_object_part_text_set(o_foot_text,"object.foot_text",foot_text);
        evas_object_size_hint_align_set(o_foot_text, -1, -1);
        evas_object_size_hint_weight_set(o_foot_text, -1, -1);
        edje_object_scale_set(o_foot_text, pres->current_scale);
        evas_object_show(o_foot_text);
        edje_object_part_box_append(slide->obj,"area.foot_text",o_foot_text);

        if(pres->edit_mode)
        {
            edje_object_signal_emit(o_foot_text, "edit,mode,1", "eyelight");
            if(pres->edit_mode)
            {
                Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
                edit->obj = o_foot_text;
                edit->node = node;
                edit->slide = slide;
                edit->data = pres->edit_data;
                slide->edits = eina_list_append(slide->edits, edit);

                if(pres->edit_cb)
                    edje_object_signal_callback_add(o_foot_text, "select", "image", pres->edit_cb, edit);
            }
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_foot_text);
        return o_foot_text;
    }
    return NULL;
}


Evas_Object *eyelight_object_foot_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *default_image)
{
    Eyelight_Node *node_foot_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_IMAGE);
    const char * foot_image = NULL;
    if(node_foot_image)
        foot_image = eyelight_retrieve_value_of_prop(node_foot_image,0);
    else if(!node_foot_image && default_image)
        foot_image = default_image;
    if(foot_image)
    {
        Evas_Object *o_foot_image = edje_object_add(pres->evas);
        if(edje_object_file_set(o_foot_image, pres->theme, "eyelight/foot_image") ==  0)
            printf("load group eyelight/foot_image error! %d \n",
                    edje_object_load_error_get(o_foot_image));
        const Evas_Object *part_image = edje_object_part_object_get(o_foot_image, "object.foot_image");
        char *image_path = eyelight_compile_image_path_new(pres,foot_image);
        evas_object_image_file_set((Evas_Object*)part_image, image_path, NULL);
        EYELIGHT_FREE(image_path);
        evas_object_size_hint_align_set(o_foot_image, -1, -1);
        evas_object_size_hint_weight_set(o_foot_image, -1, -1);
        edje_object_scale_set(o_foot_image, pres->current_scale);
        evas_object_show(o_foot_image);
        edje_object_part_box_append(slide->obj,"area.foot_image",o_foot_image);

        if(pres->edit_mode)
        {
            edje_object_signal_emit(o_foot_image, "edit,mode,1", "eyelight");
            if(pres->edit_mode)
            {
                Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
                edit->obj = o_foot_image;
                edit->node = node;
                edit->slide = slide;
                edit->data = pres->edit_data;
                slide->edits = eina_list_append(slide->edits, edit);

                if(pres->edit_cb)
                    edje_object_signal_callback_add(o_foot_image, "select", "image", pres->edit_cb, edit);
            }
        }

        slide->items_all = eina_list_append(
                slide->items_all,
                o_foot_image);

        return o_foot_image;
    }

    return NULL;
}


Evas_Object *eyelight_object_pages_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, int slide_number, int nb_slides)
{
    char buf[1024];
    snprintf(buf,1024," %d/%d",slide_number+1,nb_slides);
    Evas_Object *o_pages = edje_object_add(pres->evas);
    if(edje_object_file_set(o_pages, pres->theme, "eyelight/pages") ==  0)
        printf("load group eyelight/pages error! %d \n",
                edje_object_load_error_get(o_pages));
    edje_object_part_text_set(o_pages,"object.pages",buf);
    evas_object_size_hint_align_set(o_pages, -1, -1);
    evas_object_size_hint_weight_set(o_pages, -1, -1);
    edje_object_scale_set(o_pages, pres->current_scale);
    evas_object_show(o_pages);
    edje_object_part_box_append(slide->obj,"area.pages",o_pages);

    slide->items_all = eina_list_append(
            slide->items_all,
            o_pages);
    return o_pages;
}



Evas_Object *eyelight_object_item_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_text") ==  0)
        printf("load group eyelight/item_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, 1);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);
    edje_object_part_box_append(o_area,buf,o_text);

    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_text;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
        }
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}


Evas_Object *eyelight_object_item_image_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, const char *image, int border, int shadow)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_image = edje_object_add(pres->evas);
    if(edje_object_file_set(o_image, pres->theme, "eyelight/item_image") ==  0)
        printf("load group eyelight/item_image error! %d \n",
                edje_object_load_error_get(o_image));
    const Evas_Object *part_image = edje_object_part_object_get(o_image, "object.image");
    char *image_path = eyelight_compile_image_path_new(pres,image);
    evas_object_image_file_set((Evas_Object*)part_image, image_path, NULL);
    EYELIGHT_FREE(image_path);
    evas_object_size_hint_align_set(o_image, -1, -1);
    evas_object_size_hint_weight_set(o_image, -1, -1);
    edje_object_scale_set(o_image, pres->current_scale);
    evas_object_show(o_image);
    edje_object_part_box_append(o_area,buf,o_image);


    if(border)
        edje_object_signal_emit(o_image, "border,show","eyelight");
    if(shadow)
        edje_object_signal_emit(o_image, "shadow,show","eyelight");
    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_image, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_image;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_image, "select", "image", pres->edit_cb, edit);
        }
    }


    slide->items_all = eina_list_append(
            slide->items_all,
            o_image);

    return o_image;
}

/**   Callbacks used by a video item */


void _video_play_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,1);
    edje_object_signal_emit(o, "play", "video_state");
}

void _video_pause_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,0);
    edje_object_signal_emit(o, "pause", "video_state");
}

void _video_stop_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Evas_Object *ov = data;
    emotion_object_play_set(ov,0);
    emotion_object_position_set(ov, 0);
    edje_object_signal_emit(o, "stop", "video_state");
    edje_object_part_drag_value_set(ov, "object.slider", 0.0, 0.0);
}

void _video_slider_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    double len;
    double x, y;

    Evas_Object *ov = data;

    edje_object_part_drag_value_get(o, "object.slider", &x, &y);
    len = emotion_object_play_length_get(ov);
    emotion_object_position_set(ov, x * len);
    _video_play_cb(o, ov, NULL,NULL);
}

void _video_obj_progress_cb(void *data, Evas_Object *o, void  *event_info)
{
    double len, pos, scale;

    Evas_Object *ov = data;

    pos = emotion_object_position_get(o);
    len = emotion_object_play_length_get(o);
    scale = (len > 0.0) ? pos / len : 0.0;
    edje_object_part_drag_value_set(ov, "object.slider", scale, 0.0);
}

void _video_obj_replay_cb(void *data, Evas_Object *o, void *event_info)
{
    double len, pos;

    Eyelight_Video *e_video = data;
    emotion_object_position_set(e_video->o_inter,0);
    if(e_video->replay)
    {
        _video_play_cb(e_video->o_inter, e_video->o_video, NULL,NULL);
    }
}

Evas_Object *eyelight_object_item_presentation_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, const char *presentation, const char *theme, int border, int shadow)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_pres = edje_object_add(pres->evas);
    if(edje_object_file_set(o_pres, pres->theme, "eyelight/item_presentation") ==  0)
        printf("load group eyelight/item_presentation error! %d \n",
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

    evas_object_size_hint_align_set(o_pres, -1, -1);
    evas_object_size_hint_weight_set(o_pres, -1, -1);
    evas_object_show(o_pres);
    edje_object_part_box_append(o_area,buf,o_pres);

    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_pres, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_pres;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_pres, "select", "image", pres->edit_cb, edit);
        }
    }

    slide->items_edje = eina_list_append(
            slide->items_edje, o_inter);

    slide->items_all = eina_list_append(
            slide->items_all,
            o_pres);
    return o_pres;
}

/** */
Evas_Object *eyelight_object_item_video_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, const char *video, int alpha, int autoplay, int replay, int border, int shadow)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_video = edje_object_add(pres->evas);
    if(edje_object_file_set(o_video, pres->theme, "eyelight/item_video") ==  0)
        printf("load group eyelight/item_video error! %d \n",
                edje_object_load_error_get(o_video));


    char *video_path = eyelight_compile_image_path_new(pres,video);

    if(!ecore_file_exists(video_path))
        printf("Video %s not found !\n",video_path);

    /* basic video object setup */
    Evas_Object *o_inter = emotion_object_add(pres->evas);
    if (!emotion_object_init(o_inter, pres->video_module))
    {
        printf("Emotion init failed !\n");
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
        edje_object_signal_emit(o_video, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_video;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_video, "select", "image", pres->edit_cb, edit);
        }
    }

    EYELIGHT_FREE(video_path);
    edje_object_part_swallow(o_video,"object.swallow",o_inter);

    evas_object_color_set(o_inter, 255, 255, 255, alpha);

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

Evas_Object *eyelight_object_item_simple_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_simple_text") ==  0)
        printf("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);

    edje_object_part_box_append(o_area,buf,o_text);

    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");

    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_text;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
        }
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}


Evas_Object *eyelight_object_item_summary_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, int id_summary, int id_item, const char *area, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_summary_text") ==  0)
        printf("load group eyelight/item_summary_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    evas_object_size_hint_align_set(o_text, -1, 0.5);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);
    edje_object_part_box_append(o_area,buf,o_text);

    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");

    if(id_summary == -1 || id_item<id_summary)
        edje_object_signal_emit(o_text, "before","eyelight");
    else if(id_item==id_summary)
        edje_object_signal_emit(o_text, "current","eyelight");
    else
        edje_object_signal_emit(o_text, "after","eyelight");

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}

Evas_Object *eyelight_object_item_numbering_text_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, char *numbering_id, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN],buf2[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_numbering_text") ==  0)
        printf("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);

    snprintf(buf2,EYELIGHT_BUFLEN, " %s) ", numbering_id);
    edje_object_part_text_set(o_text,"object.numbering",buf2);

    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    edje_object_scale_set(o_text, pres->current_scale);
    evas_object_show(o_text);

    edje_object_part_box_append(o_area,buf,o_text);


    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");
    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_text, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_text;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_text, "select", "image", pres->edit_cb, edit);
        }
    }

    slide->items_all = eina_list_append(
            slide->items_all,
            o_text);
    return o_text;
}


Evas_Object *eyelight_object_item_edje_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, const char *edje_file, const char *edje_group)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, slide, id_slide, area, buf);

    Evas_Object *o_edje = edje_object_add(pres->evas);
    if(edje_object_file_set(o_edje, pres->theme, "eyelight/item_edje") ==  0)
        printf("load group eyelight/item_edje error! %d \n",
                edje_object_load_error_get(o_edje));

    char *edje_path = eyelight_compile_image_path_new(pres,edje_file);
    Evas_Object *o_inter = edje_object_add(pres->evas);
    edje_object_scale_set(o_inter, pres->current_scale);

    if(edje_object_file_set(o_inter, edje_path, edje_group) ==  0)
        printf("load group %s from %s error! %d \n",
                edje_group,edje_path,
                edje_object_load_error_get(o_edje));

    edje_object_part_swallow(o_edje,"object.swallow",o_inter);

    EYELIGHT_FREE(edje_path);

    evas_object_size_hint_align_set(o_edje, -1, -1);
    evas_object_size_hint_weight_set(o_edje, -1, -1);
    edje_object_scale_set(o_edje, pres->current_scale);
    evas_object_show(o_edje);
    edje_object_part_box_append(o_area,buf,o_edje);

    if(pres->edit_mode)
    {
        edje_object_signal_emit(o_edje, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
            Eyelight_Edit *edit = calloc(1,sizeof(Eyelight_Edit));
            edit->obj = o_edje;
            edit->node = node;
            edit->slide = slide;
            edit->data = pres->edit_data;
            slide->edits = eina_list_append(slide->edits, edit);

            if(pres->edit_cb)
                edje_object_signal_callback_add(o_edje, "select", "image", pres->edit_cb, edit);
        }
    }

    slide->items_edje = eina_list_append(
            slide->items_edje,
            o_inter);
    slide->items_all = eina_list_append(
            slide->items_all,
            o_edje);
    return o_edje;
}

Evas_Object *eyelight_object_custom_area_add(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node, const char *area, double rel1_x, double rel1_y, double rel2_x, double rel2_y)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = edje_object_add(pres->evas);
    if(edje_object_file_set(o_area, pres->theme, "eyelight/custom_area") ==  0)
        printf("load group eyelight/custom_area error! %d \n",
                edje_object_load_error_get(o_area));

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
        edje_object_signal_emit(o_area, "edit,mode,1", "eyelight");
        if(pres->edit_mode)
        {
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
    }

    Eyelight_Custom_Area *custom_area = calloc(1,sizeof(Eyelight_Custom_Area));
    custom_area->obj = o_area;
    custom_area->name = strdup(area);

    node->obj = o_area;
    slide->custom_areas = eina_list_append(
            slide->custom_areas,
            custom_area);
    slide->items_all = eina_list_append(
            slide->items_all,
            o_area);
    return o_area;
}



