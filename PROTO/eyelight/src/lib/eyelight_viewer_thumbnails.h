/*
 * =====================================================================================
 *
 *       Filename:  thumbnails.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/09/08 16:12:34 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  THUMBNAILS_INC
#define  THUMBNAILS_INC

#include "eyelight_viewer.h"
#include "Eyelight_Edit.h"

typedef struct Eyelight_Thumbnails Eyelight_Thumbnails;

struct Eyelight_Thumbnails
{
    //when we load the slides thumbnails in the background
    int is_background_load;
    Ecore_Idler* idle;
    int idle_current_slide;

    //default size of a thumbnail
    int default_size_w;
    int default_size_h;

    //call when a thumbnail is done
    Eyelight_Thumbnails_slide_done_cb done_cb;
    void *done_cb_data;
};


void eyelight_viewer_thumbnails_background_load_start(Eyelight_Viewer* pres);
void eyelight_viewer_thumbnails_background_load_stop(Eyelight_Viewer* pres);

const Eyelight_Thumb* eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos);
const Eyelight_Thumb* eyelight_viewer_thumbnails_custom_size_get(Eyelight_Viewer* pres, int pos, int w, int h);
EAPI void eyelight_viewer_thumbnails_clean(Eyelight_Viewer* pres,int min, int max);


#endif   /* ----- #ifndef THUMBNAILS_INC  ----- */

