/*
 * =====================================================================================
 *
 *       Filename:  eyelight_viewer_tableofcontents.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/14/2008 08:48:22 PM CEST
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

#include "eyelight_viewer.h"

void _eyelight_viewer_tableofcontents_slides_load(Eyelight_Viewer* pres);
void _eyelight_viewer_tableofcontents_slides_destroy(Eyelight_Viewer* pres);
void _eyelight_viewer_tableofcontents_next_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_tableofcontents_previous_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_tableofcontents_slide_select_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_tableofcontents_next_end_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_tableofcontents_previous_end_cb(void *data, Evas_Object *o, const char *emission, const char *source);



void eyelight_viewer_tableofcontents_start(Eyelight_Viewer* pres,int select)
{
    Evas_Object *o;
    int w_win, h_win;
    const char* data;

    evas_output_viewport_get(pres->evas, NULL, NULL, &w_win, &h_win);

    pres->state = EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS;
    pres->tableofcontents_current = select;

    o = edje_object_add(pres->evas);
    pres->tableofcontents_background = o;
    if(edje_object_file_set(o, pres->theme, "eyelight/tableofcontents") ==  0)
    {
        WARN("table of contents is not supported by the theme");
        return ;
    }

    data = edje_object_data_get(o, "nb_slides");
    pres->tableofcontents_nb_slides = atoi(data);
    evas_object_show(o);
    evas_object_resize(o, pres->current_size_w, pres->current_size_h);
    edje_object_scale_set(o, pres->current_scale);
    evas_object_move(o, pres->current_pos_x, pres->current_pos_y);
    evas_object_smart_member_add(o,pres->smart_obj);
    evas_object_clip_set(o, pres->current_clip);

    edje_object_signal_callback_add(o,"tableofcontents,slide,next","eyelight",_eyelight_viewer_tableofcontents_next_cb,pres);
    edje_object_signal_callback_add(o,"tableofcontents,slide,previous","eyelight",_eyelight_viewer_tableofcontents_previous_cb,pres);
    edje_object_signal_callback_add(o,"tableofcontents,next,end","eyelight",_eyelight_viewer_tableofcontents_next_end_cb,pres);
    edje_object_signal_callback_add(o,"tableofcontents,previous,end","eyelight",_eyelight_viewer_tableofcontents_previous_end_cb,pres);

    _eyelight_viewer_tableofcontents_slides_load(pres);

    edje_object_signal_emit(pres->tableofcontents_background,
           "show","eyelight");
}

void _eyelight_viewer_tableofcontents_slides_load(Eyelight_Viewer* pres)
{
    int i;
    char buf[EYELIGHT_BUFLEN];
    char *default_title;
    Eina_List *l;

    int nb_slides = pres->tableofcontents_nb_slides;

    int first_slide = pres->tableofcontents_current - (nb_slides/2);

    Eyelight_Node *node;
    i=0;
    l = pres->compiler->root->l;
    while(i<first_slide && l)
    {
        node = eina_list_data_get(l);
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK && node->name == EYELIGHT_NAME_SLIDE)
            i++;
        else if( node->type == EYELIGHT_NODE_TYPE_PROP && node->name == EYELIGHT_NAME_TITLE)
            default_title = eyelight_retrieve_value_of_prop(node,0);
        l = eina_list_next(l);
    }

    i = 0;
    node = eina_list_data_get(l);
    while(i<nb_slides)
    {
        if(!node)
        {
            snprintf(buf,EYELIGHT_BUFLEN,"tableofcontents,slide,hide,%d",i+1);
            edje_object_signal_emit(pres->tableofcontents_background,
                    buf,"eyelight");
            i++;
        }
        else if( node->type == EYELIGHT_NODE_TYPE_PROP && node->name == EYELIGHT_NAME_TITLE)
        {
            default_title = eyelight_retrieve_value_of_prop(node,0);
            l = eina_list_next(l);
            node = eina_list_data_get(l);
        }
        else if(node->type == EYELIGHT_NODE_TYPE_BLOCK && node->name == EYELIGHT_NAME_SLIDE)
        {
            if(first_slide+i<0 || i+first_slide>=pres->size)
            {
                snprintf(buf,EYELIGHT_BUFLEN,"tableofcontents,slide,hide,%d",i+1);
                edje_object_signal_emit(pres->tableofcontents_background,
                        buf,"eyelight");
            }
            else
            {
                snprintf(buf,EYELIGHT_BUFLEN,"tableofcontents,slide,show,%d"
                        ,i+1);
                edje_object_signal_emit(pres->tableofcontents_background,
                        buf,"eyelight");

                char *title = default_title;
                Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
                if(node_title)
                    title = eyelight_retrieve_value_of_prop(node_title, 0);

                snprintf(buf,EYELIGHT_BUFLEN,"object.text_%d",i+1);
                if(title)
                    edje_object_part_text_set(pres->tableofcontents_background
                            ,buf,title);
                l = eina_list_next(l);
                node = eina_list_data_get(l);
            }

            i++;
        }
        else
        {
            l = eina_list_next(l);
            node = eina_list_data_get(l);
        }
    }
}

void _eyelight_viewer_tableofcontents_slides_destroy(Eyelight_Viewer* pres)
{
    int size = pres->tableofcontents_nb_slides;

    if(pres->tableofcontents_background)
        evas_object_del(pres->tableofcontents_background);
}

void eyelight_viewer_tableofcontents_smart_obj_set(Eyelight_Viewer* pres, Evas_Object *obj)
{
    evas_object_smart_member_add(pres->tableofcontents_background,obj);
}

void eyelight_viewer_tableofcontents_resize(Eyelight_Viewer* pres, int w, int h)
{
    evas_object_resize(pres->tableofcontents_background,w,h);
}

void eyelight_viewer_tableofcontents_scale_set(Eyelight_Viewer* pres, double ratio)
{
    edje_object_scale_set(pres->tableofcontents_background,ratio);
}

void eyelight_viewer_tableofcontents_move(Eyelight_Viewer* pres, int x, int y)
{
    evas_object_move(pres->tableofcontents_background,x,y);
}

void eyelight_viewer_tableofcontents_clip_set(Eyelight_Viewer* pres, Evas_Object *clip)
{
    evas_object_clip_set(pres->tableofcontents_background,clip);
}

void eyelight_viewer_tableofcontents_next(Eyelight_Viewer* pres)
{
    if(pres->tableofcontents_current >= pres->size-1)
        return ;

    if(pres->tableofcontents_is_next_program)
    {
        edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,stop","eyelight");
        _eyelight_viewer_tableofcontents_next_end_cb(pres,NULL,NULL,NULL);
    }
    else if(pres->tableofcontents_is_previous_program)
    {
        edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,stop","eyelight");
        _eyelight_viewer_tableofcontents_previous_end_cb(pres,NULL,NULL,NULL);
    }
    pres->tableofcontents_is_next_program = 1;
    pres->tableofcontents_current++;

    edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,next","eyelight");

    eyelight_viewer_slide_next(pres);
}

void eyelight_viewer_tableofcontents_previous(Eyelight_Viewer* pres)
{
    if(pres->tableofcontents_current <=0)
        return ;

    if(pres->tableofcontents_is_next_program)
    {
        edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,stop","eyelight");
        _eyelight_viewer_tableofcontents_next_end_cb(pres,NULL,NULL,NULL);
    }
    else if(pres->tableofcontents_is_previous_program)
    {
        edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,stop","eyelight");
        _eyelight_viewer_tableofcontents_previous_end_cb(pres,NULL,NULL,NULL);
    }
    pres->tableofcontents_is_previous_program = 1;
    pres->tableofcontents_current--;

    edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,previous","eyelight");
    eyelight_viewer_slide_previous(pres);
}

void eyelight_viewer_tableofcontents_stop(Eyelight_Viewer* pres)
{
    if(!pres->state == EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS)
        return ;

    evas_object_del(pres->tableofcontents_background);
    _eyelight_viewer_tableofcontents_slides_destroy(pres);
    pres->state = EYELIGHT_VIEWER_STATE_DEFAULT;
}

void eyelight_viewer_tableofcontents_select(Eyelight_Viewer* pres)
{
    eyelight_viewer_tableofcontents_stop(pres);
}



void _eyelight_viewer_tableofcontents_next_end_cb(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    edje_object_signal_emit(pres->tableofcontents_background,
           "tableofcontents,init","eyelight");

    _eyelight_viewer_tableofcontents_slides_load(pres);
    pres->tableofcontents_is_next_program = 0;
}


void _eyelight_viewer_tableofcontents_previous_end_cb(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,init","eyelight");

    _eyelight_viewer_tableofcontents_slides_load(pres);
    pres->tableofcontents_is_previous_program = 0;
}

void _eyelight_viewer_tableofcontents_next_cb(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_tableofcontents_next(pres);
}


void _eyelight_viewer_tableofcontents_previous_cb(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_tableofcontents_previous(pres);
}

void _eyelight_viewer_tableofcontents_slide_select_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
}
