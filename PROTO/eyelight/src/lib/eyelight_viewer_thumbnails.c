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


int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos);
int _eyelight_viewer_thumbnails_load_idle(void *data);
int* _eyelight_viewer_thumbnails_resize(const int* pixel,int src_w,int src_h,int new_w,int new_h);
const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int free_if_in_edj);

void eyelight_viewer_thumbnails_init(Eyelight_Viewer* pres)
{
    Eet_File* file;

    pres->thumbnails = calloc(1,sizeof(Eyelight_Thumbnails));
    pres->thumbnails->default_size_w = pres->default_size_w/4;
    pres->thumbnails->default_size_h = pres->default_size_h/4;

    file = eet_open(pres->edje_file,EET_FILE_MODE_READ_WRITE);
    if(file)
    {
        pres->thumbnails->is_write_edj = 1;
        eet_close(file);
    }
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
    return _eyelight_viewer_thumbnails_get(pres,pos,0);
}

/*
 * @brief used to get a thumbnails
 * if we load all the thumbnails (eyelight_viewer_thumbnails_background_load_start)
 * and the thumbnail is (or is write) in the edj file, we can free the memory, the edj file is the buffer
 * else we keep the thumbnail in our buffer
 * @param pres a valid presentation
 * @param pos a valid slide position
 * @param free_if_in_edj, the slide will be free if it is in the edj file
 * @return returns a slide if success,
 * NULL if an error is occurs
 * and NULL if we set free_if_in_edj and free is free
 */
const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int free_if_in_edj)
{
    Eet_File* file;
    unsigned int w,h;
    int alpha,compress,quality,lossy;
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Thumbnails* thumbnails;
    int can_free = 0;

    if(!pres->thumbnails->thumbnails)
        pres->thumbnails->thumbnails = calloc(pres->size,sizeof(Eyelight_Thumb));

    thumbnails = pres->thumbnails;
    snprintf(buf,EYELIGHT_BUFLEN,"thumb/slide/%d",pos);

    if(!thumbnails->thumbnails[pos].thumb)
    {
        file = eet_open(pres->edje_file,EET_FILE_MODE_READ);
        if(file)
        {
            thumbnails->thumbnails[pos].thumb = eet_data_image_read(file,buf,&w,&h,&alpha,
                    &compress,&quality,&lossy);
            if(thumbnails->thumbnails[pos].thumb)
            {
                thumbnails->thumbnails[pos].w = w;
                thumbnails->thumbnails[pos].h = h;
                thumbnails->thumbnails[pos].is_in_edj = 1;
                can_free = 1;
            }
            eet_close(file);
        }
    }

    if(!thumbnails->thumbnails[pos].thumb)
    {
        thumbnails->thumbnails[pos].thumb = _eyelight_viewer_thumbnails_create(pres,pos);

        if(!thumbnails->thumbnails[pos].thumb)
            return NULL;

        thumbnails->thumbnails[pos].is_in_edj = 0;
        thumbnails->thumbnails[pos].w = thumbnails->default_size_w;
        thumbnails->thumbnails[pos].h = thumbnails->default_size_h;

        if(thumbnails->is_write_edj)
        {
            file = eet_open(pres->edje_file,EET_FILE_MODE_READ_WRITE);
            if(file)
            {
                eet_data_image_write(file,buf,
                        thumbnails->thumbnails[pos].thumb,
                        thumbnails->thumbnails[pos].w,
                        thumbnails->thumbnails[pos].h,
                        0,4,100,0);
                eet_close(file);
                thumbnails->thumbnails[pos].is_in_edj = 1;
                can_free = 1;
            }
        }
    }

    if(free_if_in_edj && can_free)
    {
        EYELIGHT_FREE(thumbnails->thumbnails[pos].thumb);
        return NULL;
    }

    return &(thumbnails->thumbnails[pos]);
}

int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos)
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

    //create a thumbnail of the slide
    o = edje_object_add(e);
    snprintf(buf,buffer_w,"slide_%d",pos+1);
    if(edje_object_file_set(o, pres->edje_file, buf) ==  0)
        printf("eyelight_viewer_expose_slides_load(), edje_object_file_set() erreur! 4 %d\n",edje_object_load_error_get(o));
    evas_object_move(o,-buffer_w,-buffer_h);
    evas_object_resize(o,buffer_w,buffer_h);
    evas_object_show(o);

    pixel = ecore_evas_buffer_pixels_get(ee);

    pixel_resize = _eyelight_viewer_thumbnails_resize(pixel,buffer_w,buffer_h,
            pres->thumbnails->default_size_w,
            pres->thumbnails->default_size_h);

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

    Eyelight_Thumb* thumb = _eyelight_viewer_thumbnails_get(pres,i,1);

    thumbnails->idle_current_slide++;
    return 1;
}


void eyelight_viewer_thumbnails_destroy(Eyelight_Viewer* pres)
{
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


