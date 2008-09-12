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
    if(edje_object_file_set(o, pres->edje_file, "tableofcontents/main") ==  0)
    {
        fprintf(stderr,"table of contents is not supported by the theme\n");
        return ;
    }

    data = edje_object_data_get(o, "nb_slides");
    pres->tableofcontents_nb_slides = atoi(data);
    evas_object_show(o);
    evas_object_resize(o, w_win, h_win);

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

    Ecore_Evas    *ee;
    Evas *e;
    Evas_Object *o,*o_image, *o_swallow;
    char buf[EYELIGHT_BUFLEN];
    const int * pixel;

    int nb_slides = pres->tableofcontents_nb_slides;

    int first_slide = pres->tableofcontents_current - (nb_slides/2);

    for(i=0;i<nb_slides; i++)
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

            Evas_Object* slide = eyelight_viewer_slide_get(pres
                    ,i+first_slide);
            const char* title = edje_object_data_get(slide,"title");
            snprintf(buf,EYELIGHT_BUFLEN,"tableofcontents/text/%d",i+1);
            if(title)
                edje_object_part_text_set(pres->tableofcontents_background
                        ,buf,title);
        }
    }
}

void _eyelight_viewer_tableofcontents_slides_destroy(Eyelight_Viewer* pres)
{
    int i;
    int size = pres->tableofcontents_nb_slides;

    if(pres->tableofcontents_background)
        evas_object_del(pres->tableofcontents_background);
}


void eyelight_viewer_tableofcontents_resize(Eyelight_Viewer* pres, int w, int h)
{
    evas_object_resize(pres->tableofcontents_background,w,h);
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



void _eyelight_viewer_tableofcontents_next_end_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    int h_swallow,w_swallow;
    int i;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    edje_object_signal_emit(pres->tableofcontents_background,
           "tableofcontents,init","eyelight");

    _eyelight_viewer_tableofcontents_slides_load(pres);
    pres->tableofcontents_is_next_program = 0;
}


void _eyelight_viewer_tableofcontents_previous_end_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    int h_swallow,w_swallow;
    int i;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    edje_object_signal_emit(pres->tableofcontents_background,
                "tableofcontents,init","eyelight");

    _eyelight_viewer_tableofcontents_slides_load(pres);
    pres->tableofcontents_is_previous_program = 0;
}

void _eyelight_viewer_tableofcontents_next_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_tableofcontents_next(pres);
}


void _eyelight_viewer_tableofcontents_previous_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_tableofcontents_previous(pres);
}

void _eyelight_viewer_tableofcontents_slide_select_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    int i;
}
