/*
 * =====================================================================================
 *
 *       Filename:  eyelight_viewer.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/07/08 15:21:27 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_VIEWER_INC
#define  EYELIGHT_VIEWER_INC


typedef struct Eyelight_Slide Eyelight_Slide;

#include "Eyelight.h"
#include <Eet.h>
#include "eyelight_viewer_thumbnails.h"
#include "eyelight_compiler_parser.h"
#include "Eyelight_Edit.h"


struct Eyelight_Slide
{
    Evas_Object *obj;
    // Lists of type Evas_Object*
    // All items of a slide
    Eina_List *items_all;
    // Items of type Edje
    Eina_List *items_edje;
    // List of type  Eyelight_Video*;
    // Items of type video
    Eina_List *items_video;
    // List of type Eyelight_Custom_Area
    Eina_List *custom_areas;

    const char* transition_effect_next;
    const char* transition_effect_previous;

    Eyelight_Thumb thumb;
};

struct Eyelight_Viewer
{
    Eyelight_Viewer_State state;

    Evas* evas;
    char* elt_file;

    Eyelight_Compiler *compiler;
    char* theme;

    int default_size_w;
    int default_size_h;

    int current_size_w;
    int current_size_h;

    int current_pos_x;
    int current_pos_y;

    double current_scale;
    Evas_Object *current_clip;

    //if 1, the cache of slides is never cleared
    int do_not_clear_cache;

    //If the presentation should be a member of a smart obj
    Evas_Object *smart_obj;

    int with_border;

    //list of type Eyelight_Slide
    Eina_List* slides;

    char *video_module;

    int size;
    int current;
    Eyelight_Slide* slide_with_transition[2];

    Eyelight_Thumbnails* thumbnails;

    //expose
    Evas_Object** expose_image_thumbnails;
    Evas_Object* expose_background;
    int expose_current;
    int expose_first_slide;
    int expose_nb_lines;
    int expose_nb_cols;

    //gotoslide
    Evas_Object* gotoslide_object;
    int gotoslide_number;


    //slideshow
    int slideshow_nb_slides;
    int slideshow_current;
    Evas_Object** slideshow_image_thumbnails;
    Evas_Object* slideshow_background;
    int slideshow_is_next_program;
    int slideshow_is_previous_program;

    //tableofcontents
    int tableofcontents_nb_slides;
    int tableofcontents_current;
    Evas_Object* tableofcontents_background;
    int tableofcontents_is_next_program;
    int tableofcontents_is_previous_program;
};
void eyelight_slide_clean(Eyelight_Slide *slide);

Evas_Object* eyelight_viewer_slide_load(Eyelight_Viewer*pres,Eyelight_Slide *slide, int pos);
Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,Eyelight_Slide *slide, int pos);

void eyelight_viewer_default_transitions_load(Eyelight_Viewer*pres);
void eyelight_viewer_slide_transitions_load(Eyelight_Viewer*pres, Eyelight_Slide *slide, int id);
void eyelight_viewer_slide_transitions_get(Eyelight_Viewer*pres,int slide, const char** previous, const char** next);
void eyelight_viewer_transitions_stop(Eyelight_Viewer*pres);

void eyelight_viewer_expose_resize(Eyelight_Viewer* pres,int w, int h);
void eyelight_viewer_expose_move(Eyelight_Viewer* pres,int x, int y);
void eyelight_viewer_expose_clip_set(Eyelight_Viewer* pres,Evas_Object *clip);
void eyelight_viewer_expose_smart_obj_set(Eyelight_Viewer* pres,Evas_Object *obj);
void eyelight_viewer_expose_scale_set(Eyelight_Viewer* pres,double ratio);

void eyelight_viewer_gotoslide_resize(Eyelight_Viewer* pres, int w, int h);
void eyelight_viewer_gotoslide_move(Eyelight_Viewer* pres, int x, int y);
void eyelight_viewer_gotoslide_clip_set(Eyelight_Viewer* pres, Evas_Object *clip);
void eyelight_viewer_gotoslide_smart_obj_set(Eyelight_Viewer* pres, Evas_Object *obj);
void eyelight_viewer_gotoslide_scale_set(Eyelight_Viewer* pres, double ratio);


void eyelight_viewer_slideshow_resize(Eyelight_Viewer* pres,int w, int h);
void eyelight_viewer_slideshow_move(Eyelight_Viewer* pres,int x, int y);
void eyelight_viewer_slideshow_clip_set(Eyelight_Viewer* pres,Evas_Object *clip);
void eyelight_viewer_slideshow_smart_obj_set(Eyelight_Viewer* pres,Evas_Object *obj);
void eyelight_viewer_slideshow_scale_set(Eyelight_Viewer* pres,double ratio);

void eyelight_viewer_tableofcontents_resize(Eyelight_Viewer* pres, int w, int h);
void eyelight_viewer_tableofcontents_move(Eyelight_Viewer* pres, int x, int y);
void eyelight_viewer_tableofcontents_clip_set(Eyelight_Viewer* pres, Evas_Object *clip);
void eyelight_viewer_tableofcontents_smart_obj_set(Eyelight_Viewer* pres, Evas_Object *obj);
void eyelight_viewer_tableofcontents_scale_set(Eyelight_Viewer* pres, double ratio);

#endif   /* ----- #ifndef EYELIGHT_VIEWER_INC  ----- */

