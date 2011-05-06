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

#include "eyelight_viewer.h"

static int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos, int w, int h);
static Eina_Bool _eyelight_viewer_thumbnails_load_idle(void *data);
static int* _eyelight_viewer_thumbnails_resize(const char *file, int index, const int* pixel,int src_w,int src_h,int new_w,int new_h);
static const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int w, int h);

void eyelight_viewer_thumbnails_background_load_start(Eyelight_Viewer* pres)
{
    if(pres->thumbnails.is_background_load)
        return ;
    pres->thumbnails.is_background_load = 1;

    pres->thumbnails.idle_current_slide =  0;
    pres->thumbnails.idle = ecore_idler_add(_eyelight_viewer_thumbnails_load_idle,pres);
}

void eyelight_viewer_thumbnails_background_load_stop(Eyelight_Viewer* pres)
{
    if(pres->thumbnails.idle)
    {
        ecore_idler_del(pres->thumbnails.idle);
        pres->thumbnails.idle = NULL;
    }
    pres->thumbnails.is_background_load = 0;
}

void eyelight_viewer_thumbnails_size_set(Eyelight_Viewer *pres, int w, int h)
{
    int i = 0;

    //recreate all thumbs already created
    for(i=0; i<eyelight_viewer_size_get(pres); i++)
    {
        Eyelight_Slide *slide = eina_list_nth(pres->slides, i);
        if(slide->thumb.thumb || (pres->dump_in && slide->thumb.pos >= 0))
        {
            EYELIGHT_FREE(slide->thumb.thumb);
            eyelight_viewer_thumbnails_get(pres, i);
        }
    }
}

const Eyelight_Thumb* eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos)
{
        return eyelight_viewer_thumbnails_custom_size_get(pres,pos,pres->thumbnails.default_size_w, pres->thumbnails.default_size_h);
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
static const Eyelight_Thumb* _eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos, int size_w, int size_h)
{
    Eyelight_Slide *slide = eina_list_nth(pres->slides, pos);

    if(!slide->thumb.thumb)
    {
        if (pres->dump_in)
	  {
	     slide->thumb.pos = pos;
	     slide->thumb.w = size_w;
	     slide->thumb.h = size_h;
	  }
	else
	  {
	     slide->thumb.thumb = _eyelight_viewer_thumbnails_create(pres,pos,size_w,size_h);

	     if(!slide->thumb.thumb)
	       return NULL;

	     slide->thumb.w = size_w;
	     slide->thumb.h = size_h;
	  }

        if(pres->thumbnails.done_cb)
            pres->thumbnails.done_cb(pres, pos, &(slide->thumb),
                    pres->thumbnails.done_cb_data);
    }

    return &(slide->thumb);
}

static int* _eyelight_viewer_thumbnails_create(Eyelight_Viewer* pres,int pos,int size_w, int size_h)
{
    Ecore_Evas    *ee;
    Evas *e;
    Evas_Object *o;
    int *pixel_resize;
    const int *pixel;
    Eyelight_Slide *slide;
    int i;

    int buffer_w= pres->default_size_w;
    int buffer_h= pres->default_size_h;
    ee = ecore_evas_buffer_new(buffer_w,buffer_h);
    e = ecore_evas_get (ee);

    Eyelight_Viewer *pres_copy = calloc(1,sizeof(Eyelight_Viewer));
    pres_copy->evas = e;
    pres_copy->size = pres->size;
    pres_copy->compiler = pres->compiler;
    pres_copy->theme = pres->theme;
    pres_copy->elt_file = pres->elt_file;
    pres_copy->dump_out = pres->dump_out;
    pres_copy->dump_in = pres->dump_in;

    for(i=0;i<pres_copy->size;i++)
        pres_copy->slides = eina_list_append(pres_copy->slides, eyelight_slide_new(pres_copy));

    slide = eina_list_nth(pres_copy->slides, pos);
    //create a thumbnail of the slide
    o = eyelight_viewer_slide_load(pres_copy,slide,pos);
    evas_object_move(o,-buffer_w,-buffer_h);
    evas_object_resize(o,buffer_w,buffer_h);
    evas_object_show(o);

    edje_object_signal_emit(o,"thumbnail","eyelight");

    //set all videos to 33%
    //TODO Do not work because the video is not loaded at this time
    {
        Eina_List *l;
        Eyelight_Video *video;
        EINA_LIST_FOREACH(slide->items_video, l, video)
        {
            emotion_object_position_set(video->o_inter, 100);
        }
    }

    ecore_main_loop_iterate();

    pixel = ecore_evas_buffer_pixels_get(ee);

    pixel_resize = _eyelight_viewer_thumbnails_resize(pres->dump_out, pos, pixel,buffer_w,buffer_h,
            size_w,
            size_h);

    EINA_LIST_FREE(pres_copy->slides, slide)
    {
        eyelight_slide_clean(slide);
        EYELIGHT_FREE(slide);
    }
    EYELIGHT_FREE(pres_copy);
    ecore_evas_free(ee);

    return pixel_resize;
}

void eyelight_viewer_thumbnails_clean(Eyelight_Viewer* pres, int min, int max)
{
    int i;

    for(i=0;i<pres->size;i++)
        if(!pres->thumbnails.is_background_load && (i<min || i>=max) )
        {
            Eyelight_Slide *slide = eina_list_nth(pres->slides, i);
            EYELIGHT_FREE(slide->thumb.thumb);
        }
}

static int* _eyelight_viewer_thumbnails_resize(const char *file, int pos, const int* pixel,int src_w,int src_h,int new_w,int new_h)
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
    evas_object_image_data_set(o_image,(int *)pixel);
    evas_object_move(o_image,0,0);
    evas_object_resize(o_image,new_w,new_h);
    evas_object_show(o_image);

    new_pixel = ecore_evas_buffer_pixels_get(ee);
    new_pixel_copy = malloc(sizeof(int)*new_w*new_h);
    memcpy(new_pixel_copy,new_pixel,sizeof(int)*new_w*new_h);

    /* FIXME: This could be done in another thread, would make startup more interactiv. */
    if (file)
      {
	 char buffer[10];

	 eina_convert_itoa(pos, buffer);

	 if (strchr(file, '.'))
	   {
	      Eet_File *ef;
	      char *tmp;
	      int bytes;

	      tmp = alloca(strlen(buffer) + 16);

	      strcpy(tmp, "eyelight/thumb/");
	      strcat(tmp, buffer);

	      ef = eet_open(file, EET_FILE_MODE_READ_WRITE);

	      if (ef)
		{
		   bytes = eet_data_image_write(ef, tmp, new_pixel, new_w, new_h, 0, 1, 0, 0);
		   DBG("Inserting thumb %i (%i, %i)", pos, new_w, new_h);
		   eet_close(ef);
		}
	   }
	 else
	   {
	      char *tmp;

	      tmp = alloca(strlen(buffer) + strlen(file) + 6);
	      sprintf(tmp, "%s.%s.jpg", file, buffer);

	      evas_object_image_save(o_image, tmp, NULL, "quality=90");
	   }
      }

    evas_object_image_data_set(o_image,NULL);

    ecore_evas_free(ee);

    return new_pixel_copy;
}

static Eina_Bool _eyelight_viewer_thumbnails_load_idle(void *data)
{
    Eyelight_Viewer* pres = (Eyelight_Viewer*)data;

    int i = pres->thumbnails.idle_current_slide;

    if(i>=pres->size)
    {
        pres->thumbnails.idle = NULL;
        return EINA_FALSE;
    }

    const Eyelight_Thumb* thumb = _eyelight_viewer_thumbnails_get(pres,i,pres->thumbnails.default_size_w, pres->thumbnails.default_size_h);

    pres->thumbnails.idle_current_slide++;
    return EINA_TRUE;
}


void eyelight_viewer_thumbnails_destroy(Eyelight_Viewer* pres)
{
   if (pres->thumbnails.idle)
     {
        ecore_idler_del(pres->thumbnails.idle);
	pres->thumbnails.idle = NULL;
     }
}


