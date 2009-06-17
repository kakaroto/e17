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

/**
 * Returns the object which contains the area "area" <br>
 * If "area" is not a custom area, the method return o_slide.
 * Because the area is supposed to be a default area from the theme layout <br>
 * The method also return the complete area's name: "area.name".
 * complete_area_name must be allocate with the size EYELIGHT_BUFLEN
 */
Evas_Object *eyelight_object_area_obj_get(Eyelight_Viewer *pres, int id_slide,
        Evas_Object *o_slide, char *area, char **complete_area_name)
{
    Eina_List *l;
    Eyelight_Custom_Area *custom_area;
    EINA_LIST_FOREACH(pres->custom_areas[id_slide], l, custom_area)
    {
        if(strcmp(custom_area->name, area) == 0)
        {
            snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.custom");
            return custom_area->obj;
        }
    }

    snprintf(complete_area_name,EYELIGHT_BUFLEN, "area.%s", area);
    return o_slide;
}


Evas_Object *eyelight_object_title_add(Eyelight_Viewer *pres, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text)
{
    //set the title
    Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
    char * title = NULL;
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
        evas_object_show(o_title);
        evas_object_move(o_title, 0, 0);
        edje_object_part_box_append(o_slide,"area.title",o_title);

        return o_title;
    }

    return NULL;
}

Evas_Object *eyelight_object_subtitle_add(Eyelight_Viewer *pres, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text)
{
    Eyelight_Node *node_subtitle = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_SUBTITLE);
    char * subtitle = NULL;
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
        evas_object_show(o_subtitle);
        evas_object_move(o_subtitle, 0, 0);
        edje_object_part_box_append(o_slide,"area.subtitle",o_subtitle);

        return o_subtitle;
    }
    return NULL;
}

Evas_Object *eyelight_object_header_image_add(Eyelight_Viewer *pres, Eyelight_Node *node, Evas_Object *o_slide, const char *default_image)
{
    Eyelight_Node *node_header_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_HEADER_IMAGE);
    char * header_image = NULL;
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
        Evas_Object *part_image = edje_object_part_object_get(o_header_image, "object.header_image");
        char *image_path = eyelight_compile_image_path_new(pres,header_image);
        evas_object_image_file_set(part_image, image_path, NULL);
        EYELIGHT_FREE(image_path);
        evas_object_size_hint_align_set(o_header_image, -1, -1);
        evas_object_size_hint_weight_set(o_header_image, -1, -1);
        evas_object_show(o_header_image);
        evas_object_move(o_header_image, 0, 0);
        edje_object_part_box_append(o_slide,"area.header_image",o_header_image);
    }

    return NULL;
}

Evas_Object *eyelight_object_foot_text_add(Eyelight_Viewer *pres, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text)
{
    Eyelight_Node *node_foot_text = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_TEXT);
    char * foot_text = NULL;
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
        evas_object_show(o_foot_text);
        evas_object_move(o_foot_text, 0, 0);
        edje_object_part_box_append(o_slide,"area.foot_text",o_foot_text);

        return o_foot_text;
    }
    return NULL;
}


Evas_Object *eyelight_object_foot_image_add(Eyelight_Viewer *pres, Eyelight_Node *node, Evas_Object *o_slide, const char *default_image)
{
    Eyelight_Node *node_foot_image = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_FOOT_IMAGE);
    char * foot_image = NULL;
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
        Evas_Object *part_image = edje_object_part_object_get(o_foot_image, "object.foot_image");
        char *image_path = eyelight_compile_image_path_new(pres,foot_image);
        evas_object_image_file_set(part_image, image_path, NULL);
        EYELIGHT_FREE(image_path);
        evas_object_size_hint_align_set(o_foot_image, -1, -1);
        evas_object_size_hint_weight_set(o_foot_image, -1, -1);
        evas_object_show(o_foot_image);
        evas_object_move(o_foot_image, 0, 0);
        edje_object_part_box_append(o_slide,"area.foot_image",o_foot_image);
    }

    return NULL;
}


Evas_Object *eyelight_object_pages_add(Eyelight_Viewer *pres, Evas_Object *o_slide, int slide_number, int nb_slides)
{
    char buf[1024];
    snprintf(buf,1024,"%d/%d",slide_number+1,nb_slides);
    Evas_Object *o_pages = edje_object_add(pres->evas);
    if(edje_object_file_set(o_pages, pres->theme, "eyelight/pages") ==  0)
        printf("load group eyelight/pages error! %d \n",
                edje_object_load_error_get(o_pages));
    edje_object_part_text_set(o_pages,"object.pages",buf);
    evas_object_size_hint_align_set(o_pages, -1, -1);
    evas_object_size_hint_weight_set(o_pages, -1, -1);
    evas_object_show(o_pages);
    evas_object_move(o_pages, 0, 0);
    edje_object_part_box_append(o_slide,"area.pages",o_pages);

    return o_pages;
}



Evas_Object *eyelight_object_item_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, id_slide, o_slide, area, &buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_text") ==  0)
        printf("load group eyelight/item_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    evas_object_show(o_text);
    evas_object_move(o_text, 0, 0);
    edje_object_part_box_append(o_area,buf,o_text);

    return o_text;
}


Evas_Object *eyelight_object_item_image_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *image, int border)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, id_slide, o_slide, area, &buf);

    Evas_Object *o_image = edje_object_add(pres->evas);
    if(edje_object_file_set(o_image, pres->theme, "eyelight/item_image") ==  0)
        printf("load group eyelight/item_image error! %d \n",
                edje_object_load_error_get(o_image));
    Evas_Object *part_image = edje_object_part_object_get(o_image, "object.image");
    char *image_path = eyelight_compile_image_path_new(pres,image);
    evas_object_image_file_set(part_image, image_path, NULL);
    EYELIGHT_FREE(image_path);
    evas_object_size_hint_align_set(o_image, -1, -1);
    evas_object_size_hint_weight_set(o_image, -1, -1);
    evas_object_show(o_image);
    evas_object_move(o_image, 0, 0);
    edje_object_part_box_append(o_area,buf,o_image);


    if(border)
        edje_object_signal_emit(o_image, "border,show","eyelight");

    return o_image;
}


Evas_Object *eyelight_object_item_simple_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, id_slide, o_slide, area, &buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_simple_text") ==  0)
        printf("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);
    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    evas_object_show(o_text);
    evas_object_move(o_text, 0, 0);
    edje_object_part_box_append(o_area,buf,o_text);

    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");

    return o_text;
}

Evas_Object *eyelight_object_item_numbering_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, char *numbering_id, int depth, const char *text)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, id_slide, o_slide, area, &buf);

    Evas_Object *o_text = edje_object_add(pres->evas);
    if(edje_object_file_set(o_text, pres->theme, "eyelight/item_numbering_text") ==  0)
        printf("load group eyelight/item_simple_text error! %d \n",
                edje_object_load_error_get(o_text));
    edje_object_part_text_set(o_text,"object.text",text);

    evas_object_size_hint_align_set(o_text, -1, -1);
    evas_object_size_hint_weight_set(o_text, -1, -1);
    evas_object_show(o_text);
    evas_object_move(o_text, 0, 0);
    edje_object_part_box_append(o_area,buf,o_text);

    snprintf(buf,EYELIGHT_BUFLEN, "%s) ", numbering_id);
    edje_object_part_text_set(o_text,"object.numbering",buf);

    int i;
    for(i=0;i<depth;i++)
        edje_object_signal_emit(o_text,"increase,depth","eyelight");


    return o_text;
}


Evas_Object *eyelight_object_item_edje_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *edje_file, const char *edje_group)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = eyelight_object_area_obj_get(pres, id_slide, o_slide, area, &buf);

    Evas_Object *o_edje = edje_object_add(pres->evas);
    if(edje_object_file_set(o_edje, pres->theme, "eyelight/item_edje") ==  0)
        printf("load group eyelight/item_edje error! %d \n",
                edje_object_load_error_get(o_edje));

    char *edje_path = eyelight_compile_image_path_new(pres,edje_file);
    Evas_Object *o_inter = edje_object_add(pres->evas);

    if(edje_object_file_set(o_inter, edje_path, edje_group) ==  0)
        printf("load group %s from %s error! %d \n",
                edje_group,edje_path,
                edje_object_load_error_get(o_edje));

    edje_object_part_swallow(o_edje,"object.swallow",o_inter);

    EYELIGHT_FREE(edje_path);

    evas_object_size_hint_align_set(o_edje, -1, -1);
    evas_object_size_hint_weight_set(o_edje, -1, -1);
    evas_object_show(o_edje);
    evas_object_move(o_edje, 0, 0);
    edje_object_part_box_append(o_area,buf,o_edje);

    pres->edje_objects[id_slide] = eina_list_append(
            pres->edje_objects[id_slide],
            o_inter);
    return o_edje;
}

Evas_Object *eyelight_object_custom_area_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, double rel1_x, double rel1_y, double rel2_x, double rel2_y)
{
    char buf[EYELIGHT_BUFLEN];

    Evas_Object *o_area = edje_object_add(pres->evas);
    if(edje_object_file_set(o_area, pres->theme, "eyelight/custom_area") ==  0)
        printf("load group eyelight/custom_area error! %d \n",
                edje_object_load_error_get(o_area));

    evas_object_size_hint_align_set(o_area, -1, -1);
    evas_object_size_hint_weight_set(o_area, -1, -1);
    evas_object_show(o_area);
    evas_object_move(o_area, 0, 0);

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

    edje_object_part_box_append(o_slide,"area.custom",o_area);

    if(pres->with_border)
        edje_object_signal_emit(o_area,"border,show", "eyelight");


    Eyelight_Custom_Area *custom_area = calloc(1,sizeof(Eyelight_Custom_Area));
    custom_area->obj = o_area;
    custom_area->name = area;

    pres->custom_areas[id_slide] = eina_list_append(
            pres->custom_areas[id_slide],
            custom_area);

    return o_area;
}
