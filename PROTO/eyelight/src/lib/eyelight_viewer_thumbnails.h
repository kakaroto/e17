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

typedef struct Eyelight_Thumbnails Eyelight_Thumbnails;
typedef struct Eyelight_Thumb Eyelight_Thumb;

struct Eyelight_Thumbnails
{
    Eyelight_Thumb* thumbnails;
    int is_background_load;
    int is_write_edj;
    Ecore_Idler* idle;
    int idle_current_slide;
    int default_size_w;
    int default_size_h;
};

struct Eyelight_Thumb
{
    int* thumb;
    int w;
    int h;
    int is_in_edj;
};

void eyelight_viewer_thumbnails_background_load_start(Eyelight_Viewer* pres);
const Eyelight_Thumb* eyelight_viewer_thumbnails_get(Eyelight_Viewer* pres, int pos);
void eyelight_viewer_thumbnails_clean(Eyelight_Viewer* pres,int min, int max);
void eyelight_viewer_thumbnails_init(Eyelight_Viewer* pres);




#endif   /* ----- #ifndef THUMBNAILS_INC  ----- */

