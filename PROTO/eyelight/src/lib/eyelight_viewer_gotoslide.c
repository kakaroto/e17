/*
 * =====================================================================================
 *
 *       Filename:  eyelight_viewer_gotoslide.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/14/2008 08:48:06 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */


#include "eyelight_viewer.h"



void eyelight_viewer_gotoslide_start(Eyelight_Viewer* pres)
{
    Evas_Object *o;
    int w_win, h_win;

    evas_output_viewport_get(pres->evas, NULL, NULL, &w_win, &h_win);

    pres->state = EYELIGHT_VIEWER_STATE_GOTOSLIDE;
    pres->gotoslide_number = 0;

    o = edje_object_add(pres->evas);
    pres->gotoslide_object = o;
    if(edje_object_file_set(o, pres->theme, "eyelight/gotoslide") ==  0)
        printf("eyelight_viewer_gotoslide_start(), edje_object_file_set() erreur! 1\n");
    evas_object_show(o);
    evas_object_resize(o, pres->current_size_w, pres->current_size_h);
    evas_object_smart_member_add(o, pres->smart_obj);
    evas_object_move(o, pres->current_pos_x, pres->current_pos_y);
    evas_object_clip_set(o, pres->current_clip);
}

void eyelight_viewer_gotoslide_stop(Eyelight_Viewer* pres)
{
    pres->state = EYELIGHT_VIEWER_STATE_DEFAULT;
    evas_object_del(pres->gotoslide_object);
}

void eyelight_viewer_gotoslide_digit_add(Eyelight_Viewer* pres, int digit)
{
    char buf[EYELIGHT_BUFLEN];
    pres->gotoslide_number = pres->gotoslide_number * 10 + digit;
    snprintf(buf,EYELIGHT_BUFLEN,"%d",pres->gotoslide_number);
    edje_object_part_text_set(pres->gotoslide_object,"object.text",buf);
}

void eyelight_viewer_gotoslide_digit_last_remove(Eyelight_Viewer* pres)
{
    char buf[EYELIGHT_BUFLEN];
    pres->gotoslide_number /= 10;
    snprintf(buf,EYELIGHT_BUFLEN,"%d",pres->gotoslide_number);
    edje_object_part_text_set(pres->gotoslide_object,"object.text",buf);
}

void eyelight_viewer_gotoslide_goto(Eyelight_Viewer* pres)
{
    if(pres->size >= pres->gotoslide_number && pres->gotoslide_number>0)
    {
        eyelight_viewer_gotoslide_stop(pres);
        eyelight_viewer_slide_goto(pres, pres->gotoslide_number-1);
    }
    else
    {
        char buf[EYELIGHT_BUFLEN];
        snprintf(buf,EYELIGHT_BUFLEN,"The presentation has %d slides.",pres->size);
        edje_object_signal_emit(pres->gotoslide_object,"gotoslide,error,show","eyelight");
        edje_object_part_text_set(pres->gotoslide_object,"object.error",buf);
    }
}

void eyelight_viewer_gotoslide_smart_obj_set(Eyelight_Viewer* pres, Evas_Object *obj)
{
    evas_object_smart_member_add(pres->gotoslide_object,obj);
}

void eyelight_viewer_gotoslide_resize(Eyelight_Viewer* pres, int w, int h)
{
    evas_object_resize(pres->gotoslide_object,w,h);
}

void eyelight_viewer_gotoslide_move(Eyelight_Viewer* pres, int x, int y)
{
    evas_object_move(pres->gotoslide_object,x,y);
}

void eyelight_viewer_gotoslide_clip_set(Eyelight_Viewer* pres, Evas_Object *clip)
{
    evas_object_clip_set(pres->gotoslide_object,clip);
}
