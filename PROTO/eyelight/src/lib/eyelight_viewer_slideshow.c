/*
 * =====================================================================================
 *
 *       Filename:  eyelight_viewer_slideshow.c
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


void _eyelight_viewer_slideshow_slides_load(Eyelight_Viewer* pres);
void _eyelight_viewer_slideshow_slides_destroy(Eyelight_Viewer* pres);
void _eyelight_viewer_slideshow_next_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_slideshow_previous_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_slideshow_slide_select_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_slideshow_next_end_cb(void *data, Evas_Object *o, const char *emission, const char *source);
void _eyelight_viewer_slideshow_previous_end_cb(void *data, Evas_Object *o, const char *emission, const char *source);



void eyelight_viewer_slideshow_start(Eyelight_Viewer* pres,int select)
{
    Evas_Object *o;
    int w_win, h_win;
    const char* data;

    evas_output_viewport_get(pres->evas, NULL, NULL, &w_win, &h_win);

    pres->state = EYELIGHT_VIEWER_STATE_SLIDESHOW;
    pres->slideshow_current = select;

    o = edje_object_add(pres->evas);
    pres->slideshow_background = o;
    if(edje_object_file_set(o, pres->edje_file, "slideshow/main") ==  0)
        printf("eyelight_viewer_slideshow_start(), edje_object_file_set() erreur! 1\n");

    data = edje_object_data_get(o, "nb_slides");
    pres->slideshow_nb_slides = atoi(data);
    evas_object_show(o);
    evas_object_resize(o, w_win, h_win);

    edje_object_signal_callback_add(o,"slideshow,slide,next","eyelight",_eyelight_viewer_slideshow_next_cb,pres);
    edje_object_signal_callback_add(o,"slideshow,slide,previous","eyelight",_eyelight_viewer_slideshow_previous_cb,pres);
    edje_object_signal_callback_add(o,"slideshow,next,end","eyelight",_eyelight_viewer_slideshow_next_end_cb,pres);
    edje_object_signal_callback_add(o,"slideshow,previous,end","eyelight",_eyelight_viewer_slideshow_previous_end_cb,pres);

    _eyelight_viewer_slideshow_slides_load(pres);
    edje_object_signal_emit(pres->slideshow_background,
           "show","eyelight");
}

void _eyelight_viewer_slideshow_slides_load(Eyelight_Viewer* pres)
{
    int i;

    Evas_Object *o,*o_image, *o_swallow;
    char buf[EYELIGHT_BUFLEN];

    int nb_slides = pres->slideshow_nb_slides;

    int first_slide = pres->slideshow_current - (nb_slides/2);
    if(!pres->slideshow_image_thumbnails)
        pres->slideshow_image_thumbnails = calloc(nb_slides,sizeof(Evas_Object*));
    int w_swallow, h_swallow;

    for(i=0;i<nb_slides; i++)
    {
        if(first_slide+i<0 || i+first_slide>=pres->size)
        {
            snprintf(buf,EYELIGHT_BUFLEN,"slideshow,slide,hide,%d",i+1);
            edje_object_signal_emit(pres->slideshow_background,
                buf,"eyelight");
        }
        else
        {
            snprintf(buf,EYELIGHT_BUFLEN,"slideshow,slide,show,%d",i+1);
            edje_object_signal_emit(pres->slideshow_background,
                    buf,"eyelight");


            if(!pres->slideshow_image_thumbnails[i])
            {
                const Eyelight_Thumb* thumb = eyelight_viewer_thumbnails_get(pres,first_slide+i);

                o_image = evas_object_image_add(pres->evas);
                pres->slideshow_image_thumbnails[i] = o_image;
                evas_object_image_fill_set(o_image,0,0,thumb->w,thumb->h);
                evas_object_move(o_image,0,0);
                evas_object_image_size_set(o_image, thumb->w, thumb->h);
                evas_object_image_data_set(o_image,thumb->thumb);

                evas_object_show(o_image);
                snprintf(buf,EYELIGHT_BUFLEN,"slideshow/swallow/%d",i+1);
                edje_object_part_swallow(pres->slideshow_background,buf,o_image);
                edje_object_part_geometry_get(pres->slideshow_background,buf,NULL,NULL,&w_swallow, &h_swallow);
                evas_object_image_fill_set(o_image,0,0,w_swallow,h_swallow);
            }
        }
    }
    eyelight_viewer_thumbnails_clean(pres,first_slide,first_slide+nb_slides);
}

void _eyelight_viewer_slideshow_slides_destroy(Eyelight_Viewer* pres)
{
    int i;
    int size = pres->slideshow_nb_slides;

    for(i=0;i<size;i++)
    {
        if(pres->slideshow_image_thumbnails[i])
        {
            evas_object_image_data_set(pres->slideshow_image_thumbnails[i],NULL);
            evas_object_del(pres->slideshow_image_thumbnails[i]);
        }
    }
    evas_object_del(pres->slideshow_background);
    EYELIGHT_FREE(pres->slideshow_image_thumbnails);
}

void eyelight_viewer_slideshow_resize(Eyelight_Viewer* pres,int w, int h)
{
    int nb_slides = pres->slideshow_nb_slides;
    int w_swallow,h_swallow;
    int i;
    char buf[EYELIGHT_BUFLEN];

    evas_object_resize(pres->slideshow_background,w,h);

    for(i=0;i<nb_slides;i++)
    {
        if(pres->slideshow_image_thumbnails[i])
        {
            snprintf(buf,EYELIGHT_BUFLEN,"slideshow/swallow/%d",i+1);
            edje_object_part_geometry_get(pres->slideshow_background,buf,NULL,NULL,
                    &w_swallow, &h_swallow);
            evas_object_image_fill_set(pres->slideshow_image_thumbnails[i],0,0,
                    w_swallow,h_swallow);
        }
    }
}

void eyelight_viewer_slideshow_next(Eyelight_Viewer* pres)
{
    if(pres->slideshow_current >= pres->size-1)
        return ;

    if(pres->slideshow_is_next_program)
    {
        edje_object_signal_emit(pres->slideshow_background,
                "slideshow,stop","eyelight");
        _eyelight_viewer_slideshow_next_end_cb(pres,NULL,NULL,NULL);
    }
    else if(pres->slideshow_is_previous_program)
    {
        edje_object_signal_emit(pres->slideshow_background,
                "slideshow,stop","eyelight");
        _eyelight_viewer_slideshow_previous_end_cb(pres,NULL,NULL,NULL);
    }
    pres->slideshow_is_next_program = 1;
    pres->slideshow_current++;

    edje_object_signal_emit(pres->slideshow_background,
                "slideshow,next","eyelight");

    eyelight_viewer_slide_next(pres);
}

void eyelight_viewer_slideshow_previous(Eyelight_Viewer* pres)
{
    if(pres->slideshow_current <=0)
        return ;

    if(pres->slideshow_is_next_program)
    {
        edje_object_signal_emit(pres->slideshow_background,
                "slideshow,stop","eyelight");
        _eyelight_viewer_slideshow_next_end_cb(pres,NULL,NULL,NULL);
    }
    else if(pres->slideshow_is_previous_program)
    {
        edje_object_signal_emit(pres->slideshow_background,
                "slideshow,stop","eyelight");
        _eyelight_viewer_slideshow_previous_end_cb(pres,NULL,NULL,NULL);
    }
    pres->slideshow_is_previous_program = 1;
    pres->slideshow_current--;

    edje_object_signal_emit(pres->slideshow_background,
                "slideshow,previous","eyelight");
    eyelight_viewer_slide_previous(pres);
}

void eyelight_viewer_slideshow_stop(Eyelight_Viewer* pres)
{
    if(!pres->state == EYELIGHT_VIEWER_STATE_SLIDESHOW)
        return ;

    evas_object_del(pres->slideshow_background);
    _eyelight_viewer_slideshow_slides_destroy(pres);
    pres->state = EYELIGHT_VIEWER_STATE_DEFAULT;
    eyelight_viewer_thumbnails_clean(pres,-1,-1);
}

void eyelight_viewer_slideshow_select(Eyelight_Viewer* pres)
{
    eyelight_viewer_slideshow_stop(pres);
}



void _eyelight_viewer_slideshow_next_end_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    int h_swallow,w_swallow;
    int i;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    if(pres->slideshow_image_thumbnails[0]!=NULL)
    {
        evas_object_image_data_set(pres->slideshow_image_thumbnails[0],NULL);
        evas_object_del(pres->slideshow_image_thumbnails[0]);
        pres->slideshow_image_thumbnails[0] = NULL;
    }

    for(i=0;i<pres->slideshow_nb_slides-1;i++)
    {
        snprintf(buf,EYELIGHT_BUFLEN,"slideshow/swallow/%d",i+1);
        pres->slideshow_image_thumbnails[i] = pres->slideshow_image_thumbnails[i+1];

        if(pres->slideshow_image_thumbnails[i])
        {
            edje_object_part_swallow(pres->slideshow_background
                    ,buf,pres->slideshow_image_thumbnails[i]);
            edje_object_part_geometry_get(pres->slideshow_background
                    ,buf,NULL,NULL,&w_swallow, &h_swallow);
            evas_object_image_fill_set(
                    pres->slideshow_image_thumbnails[i]
                    ,0,0,w_swallow,h_swallow);
        }
    }
    pres->slideshow_image_thumbnails[pres->slideshow_nb_slides-1]= NULL;
    edje_object_signal_emit(pres->slideshow_background,
           "slideshow,init","eyelight");

    _eyelight_viewer_slideshow_slides_load(pres);
    pres->slideshow_is_next_program = 0;
}


void _eyelight_viewer_slideshow_previous_end_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    int h_swallow,w_swallow;
    int i;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;

    int size = pres->slideshow_nb_slides;
    if(pres->slideshow_image_thumbnails[size-1]!=NULL)
    {
        evas_object_image_data_set(pres->slideshow_image_thumbnails[size-1],NULL);
        evas_object_del(pres->slideshow_image_thumbnails[size-1]);
    }

    for(i=size-1;i>0;i--)
    {
        pres->slideshow_image_thumbnails[i] = pres->slideshow_image_thumbnails[i-1];

        snprintf(buf,EYELIGHT_BUFLEN,"slideshow/swallow/%d",i+1);
        edje_object_part_swallow(pres->slideshow_background,buf,pres->slideshow_image_thumbnails[i]);
        edje_object_part_geometry_get(pres->slideshow_background,buf,NULL,NULL,&w_swallow, &h_swallow);
        evas_object_image_fill_set(pres->slideshow_image_thumbnails[i],0,0,w_swallow,h_swallow);
    }
    pres->slideshow_image_thumbnails[0] = NULL;


    edje_object_signal_emit(pres->slideshow_background,
                "slideshow,init","eyelight");

    _eyelight_viewer_slideshow_slides_load(pres);
    pres->slideshow_is_previous_program = 0;
}

void _eyelight_viewer_slideshow_next_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_slideshow_next(pres);
}


void _eyelight_viewer_slideshow_previous_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    eyelight_viewer_slideshow_previous(pres);
}

void _eyelight_viewer_slideshow_slide_select_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    int i;

    /*for(i=0;i<pres->slideshow_nb_slides;i++)
    {
        if(o==pres->slideshow_slides[i])
        {
            pres->slideshow_current=i;
            //eyelight_viewer_slideshow_select(pres);
            return ;
        }
    }
    */
}
