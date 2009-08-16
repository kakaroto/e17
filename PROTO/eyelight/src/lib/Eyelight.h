/*
 * =====================================================================================
 *
 *       Filename:  eyelight.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  25/06/08 12:06:42 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_INC
#define  EYELIGHT_INC

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <stdio.h>
#include <Edje.h>
#include <string.h>
#include <sys/wait.h>

#include "eyelight_common.h"

/**
 * This file act like the public header of eyelight_viewer.h
 * An apps can only access to the functions defined here.
 */

typedef struct Eyelight_Viewer Eyelight_Viewer;

EAPI Eyelight_Viewer* eyelight_viewer_new(Evas* evas, const char* pres, const char* theme,int with_boder);
EAPI void eyelight_viewer_destroy(Eyelight_Viewer**pres);
EAPI void eyelight_viewer_clean(Eyelight_Viewer *pres);


EAPI Eyelight_Viewer_State eyelight_viewer_state_get(Eyelight_Viewer* pres);


EAPI int eyelight_viewer_presentation_file_set(Eyelight_Viewer *pres, const char* presentation);
EAPI int eyelight_viewer_theme_file_set(Eyelight_Viewer *pres, const char* theme);
EAPI const char* eyelight_viewer_presentation_file_get(Eyelight_Viewer *pres);
EAPI const char* eyelight_viewer_theme_file_get(Eyelight_Viewer *pres);

EAPI void eyelight_viewer_border_set(Eyelight_Viewer *pres, int border);
EAPI void eyelight_viewer_smart_obj_set(Eyelight_Viewer *pres, Evas_Object *below);
EAPI void eyelight_viewer_scale_set(Eyelight_Viewer*pres, double ratio);
EAPI void eyelight_viewer_clip_set(Eyelight_Viewer*pres, Evas_Object *clip);

EAPI int eyelight_viewer_size_get(Eyelight_Viewer*pres);
EAPI int eyelight_viewer_current_id_get(Eyelight_Viewer* pres);

EAPI void eyelight_viewer_move(Eyelight_Viewer*pres, Evas_Coord x, Evas_Coord y);
EAPI void eyelight_viewer_resize(Eyelight_Viewer*pres, Evas_Coord w, Evas_Coord h);

EAPI void eyelight_viewer_slide_next(Eyelight_Viewer*pres);
EAPI void eyelight_viewer_slide_previous(Eyelight_Viewer*pres);
EAPI void eyelight_viewer_slide_goto(Eyelight_Viewer* pres, int slide_id);
EAPI void eyelight_viewer_clear_cache_set(Eyelight_Viewer *pres, int clear);

EAPI void eyelight_viewer_expose_start(Eyelight_Viewer* pres, int select);
EAPI void eyelight_viewer_expose_stop(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_next(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_previous(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_window_next(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_window_previous(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_down(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_up(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_expose_select(Eyelight_Viewer* pres);

EAPI void eyelight_viewer_gotoslide_start(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_gotoslide_stop(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_gotoslide_digit_add(Eyelight_Viewer* pres, int digit);
EAPI void eyelight_viewer_gotoslide_goto(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_gotoslide_digit_last_remove(Eyelight_Viewer* pres);

EAPI void eyelight_viewer_slideshow_start(Eyelight_Viewer* pres,int select);
EAPI void eyelight_viewer_slideshow_stop(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_slideshow_next(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_slideshow_previous(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_slideshow_select(Eyelight_Viewer* pres);

EAPI void eyelight_viewer_tableofcontents_start(Eyelight_Viewer* pres,int select);
EAPI void eyelight_viewer_tableofcontents_stop(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_tableofcontents_next(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_tableofcontents_previous(Eyelight_Viewer* pres);
EAPI void eyelight_viewer_tableofcontents_select(Eyelight_Viewer* pres);


#endif   /* ----- #ifndef EYELIGHT_INC  ----- */

