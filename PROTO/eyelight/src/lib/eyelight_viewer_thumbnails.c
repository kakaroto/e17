/*
 * =====================================================================================
 *
 *       Filename:  eyelight_viewer_thumbnails.c
 *
 *    Description:  create and manage the slide thumbnails use in the expose mode and slideshow mode
 *
 *        Version:  1.0
 *        Created:  09/06/2008 11:37:51 AM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "eyelight_viewer.h"


int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos, int w, int h);
int _eyelight_viewer_thumbnails_load_idle(void *data);
int* _eyelight_viewer_thumbnails_resize(const int* pixel,int src_w,int src_h,int new_w,int new_h);
const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int w, int h);

void eyelight_viewer_thumbnails_init(Eyelight_Viewer* pres)
{
    Eet_File* file;

    pres->thumbnails = calloc(1,sizeof(Eyelight_Thumbnails));
    pres->thumbnails->default_size_w = pres->default_size_w/4;
    pres->thumbnails->default_size_h = pres->default_size_h/4;
}

void eyelight_viewer_thumbnails_background_load_start(Eyelight_Viewer* pres)
{
    if(pres->thumbnails->is_background_load)
        return ;
    pres->thumbnails->is_background_load = 1;

    pres->thumbnails->idle_current_slide =  0;
    pres->thumbnails->idle = ecore_idler_add(_eyelight_viewer_thumbnails_load_idle,pres);
}

const Eyelight_Thumb* eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos)
{
        return eyelight_viewer_thumbnails_custom_size_get(pres,pos,pres->thumbnails->default_size_w, pres->thumbnails->default_size_h);
}

/**
 * @param pres the presentation
 * @param pos the slide which we want a thumbnail
 * @param w the thumbnail size
 * @param h the thumbnail size
 */
const Eyelight_Thumb* eyelight_viewer_thumbnails_custom_size_get(Eyelight_Viewer* pres, int pos, int w, int h)
{
    return _eyelight_viewer_thumbnails_get(pres,pos,w,h);
}

/*
 * @brief used to get a thumbnails
 * if we load all the thumbnails (eyelight_viewer_thumbnails_background_load_start)
 * and the thumbnail is (or is write) in the edj file, we can free the memory, the edj file is in the buffer
 * else we keep the thumbnail in our buffer
 * @param pres a valid presentation
 * @param pos a valid slide position
 * @param size_w the size of the thumbnail, use if we create a new thumb
 * @param size_h the size of the thumbnail, use if we create a new thumb
 * @return returns a slide if success,
 * NULL if an error is occurs
 */
const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int size_w, int size_h)
{
    Eet_File* file;
    unsigned int w,h;
    int alpha,compress,quality,lossy;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Thumbnails* thumbnails;

    if(!pres->thumbnails->thumbnails)
        pres->thumbnails->thumbnails = calloc(pres->size,sizeof(Eyelight_Thumb));

    thumbnails = pres->thumbnails;
    snprintf(buf,EYELIGHT_BUFLEN,"thumb/slide/%d",pos);

    if(!thumbnails->thumbnails[pos].thumb)
    {
        thumbnails->thumbnails[pos].thumb = _eyelight_viewer_thumbnails_create(pres,pos,size_w,size_h);

        if(!thumbnails->thumbnails[pos].thumb)
            return NULL;

        thumbnails->thumbnails[pos].is_in_edj = 0;
        thumbnails->thumbnails[pos].w = size_w;
        thumbnails->thumbnails[pos].h = size_h;
    }

    return &(thumbnails->thumbnails[pos]);
}

int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos,int size_w, int size_h)
{
    Ecore_Evas    *ee;
    Evas *e;
    Evas_Object *o;
    char buf[EYELIGHT_BUFLEN];
    int *pixel_resize;
    const int *pixel;

    int buffer_w= pres->default_size_w;
    int buffer_h= pres->default_size_h;
    ee = ecore_evas_buffer_new(buffer_w,buffer_h);
    e = ecore_evas_get (ee);

    Eyelight_Viewer *pres_copy = calloc(1,sizeof(Eyelight_Viewer));
    pres_copy->evas = e;
    pres_copy->compiler = pres->compiler;
    pres_copy->elt_file = pres->elt_file;
    pres_copy->theme = pres->theme;
    pres_copy->slides = calloc(pres->size,sizeof(Evas_Object*));
    pres_copy->custom_areas = calloc(pres->size,sizeof(Eina_List*));
    pres_copy->edje_objects = calloc(pres->size,sizeof(Eina_List*));
    pres_copy->video_objects = calloc(pres->size,sizeof(Eina_List*));
    pres_copy->size = pres->size;
    pres_copy->video_module = pres->video_module;


    //create a thumbnail of the slide
    o = eyelight_viewer_slide_load(pres_copy,pos);
    evas_object_move(o,-buffer_w,-buffer_h);
    evas_object_resize(o,buffer_w,buffer_h);
    evas_object_show(o);

    edje_object_signal_emit(o,"thumbnail","eyelight");

    //set all videos to 33%
    {
       Eina_List *l;
       Eyelight_Video *video;
       EINA_LIST_FOREACH(pres_copy->video_objects[pos], l, video)
	 {
	    emotion_object_position_set(video->o_inter, 100);
	 }
    }

    ecore_main_loop_iterate();

    pixel = ecore_evas_buffer_pixels_get(ee);

    pixel_resize = _eyelight_viewer_thumbnails_resize(pixel,buffer_w,buffer_h,
            size_w,
            size_h);

    int i;
    for(i=0;i<pres_copy->size;i++)
    {
        Eina_List *l, *l_next;
        {
            Evas_Object *data;
            EINA_LIST_FOREACH_SAFE(pres_copy->edje_objects[i], l, l_next, data)
            {
                evas_object_del(data);
                pres_copy->edje_objects[i] = eina_list_remove_list(pres_copy->edje_objects[i], l);
            }
	}
 	{
	    Eyelight_Video *data;
            EINA_LIST_FOREACH_SAFE(pres_copy->video_objects[i], l, l_next, data)
            {
                evas_object_del(data->o_inter);
		EYELIGHT_FREE(data);
                pres_copy->video_objects[i] = eina_list_remove_list(pres_copy->video_objects[i], l);
            }
        }
        {
            Eyelight_Custom_Area *data;
            EINA_LIST_FOREACH_SAFE(pres_copy->custom_areas[i], l, l_next, data)
            {
                evas_object_del(data->obj);
                EYELIGHT_FREE(data);
                pres_copy->custom_areas[i] = eina_list_remove_list(pres_copy->custom_areas[i], l);
            }
        }
    }

    EYELIGHT_FREE(pres_copy->custom_areas);
    EYELIGHT_FREE(pres_copy->edje_objects);

    EYELIGHT_FREE(pres_copy->slides);

    ecore_evas_free(ee);
    return pixel_resize;
}

void eyelight_viewer_thumbnails_clean(Eyelight_Viewer* pres, int min, int max)
{
    int i;

    for(i=0;i<pres->size;i++)
        if((i<min || i>=max)
                && (
                    !pres->thumbnails->is_background_load
                    || pres->thumbnails->thumbnails[i].is_in_edj ))
            EYELIGHT_FREE(pres->thumbnails->thumbnails[i].thumb);

    //for(i=0;i<pres->size;i++)
    //    printf("%d: %p\n",i,pres->thumbnails->thumbnails[i].thumb);
}

int* _eyelight_viewer_thumbnails_resize(const int* pixel,int src_w,int src_h,int new_w,int new_h)
{
    Ecore_Evas* ee;
    Evas* e;
    Evas_Object *o_image;

    const int* new_pixel;
    int *new_pixel_copy;

    ee = ecore_evas_buffer_new(new_w,new_h);
    e = ecore_evas_get(ee);

    o_image = evas_object_image_add(e);
    evas_object_image_fill_set(o_image,0,0,new_w,new_h);
    evas_object_image_size_set(o_image, src_w,src_h);
    evas_object_image_data_set(o_image,pixel);
    evas_object_move(o_image,0,0);
    evas_object_resize(o_image,new_w,new_h);
    evas_object_show(o_image);

    new_pixel = ecore_evas_buffer_pixels_get(ee);
    new_pixel_copy = malloc(sizeof(int)*new_w*new_h);
    memcpy(new_pixel_copy,new_pixel,sizeof(int)*new_w*new_h);

    evas_object_image_data_set(o_image,NULL);

    ecore_evas_free(ee);

    return new_pixel_copy;
}

int _eyelight_viewer_thumbnails_load_idle(void *data)
{
    Eyelight_Viewer* pres = (Eyelight_Viewer*)data;
    Eyelight_Thumbnails *thumbnails = pres->thumbnails;

    int i = thumbnails->idle_current_slide;

    if(i>=pres->size)
    {
        thumbnails->idle = NULL;
        return 0;
    }

    Eyelight_Thumb* thumb = _eyelight_viewer_thumbnails_get(pres,i,pres->thumbnails->default_size_w, pres->thumbnails->default_size_h);

    thumbnails->idle_current_slide++;
    return 1;
}


void eyelight_viewer_thumbnails_destroy(Eyelight_Viewer* pres)
{
        if(!pres->thumbnails)
                return;
        if(pres->thumbnails->idle)
                ecore_idler_del(pres->thumbnails->idle);

        if(pres->thumbnails->thumbnails)
        {
                int i;
                for(i=0;i<pres->size;i++)
                {
            EYELIGHT_FREE(pres->thumbnails->thumbnails[i].thumb);
        }
    }
    EYELIGHT_FREE(pres->thumbnails->thumbnails);
    EYELIGHT_FREE(pres->thumbnails);
}


