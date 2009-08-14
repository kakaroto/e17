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


#include "Eyelight.h"
#include <Eet.h>
#include "eyelight_viewer_thumbnails.h"
#include "eyelight_compiler_parser.h"

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

    //If the presentation should be a member of a smart obj
    Evas_Object *smart_obj;

    int with_border;
    Evas_Object** slides;
    //for each slide, the list of items (text item, image ...)
    //we need this list to apply the scale when the size of the presentation change
    //theses objects are destroyed when the slide is destroyed, we do not need to destroy them
    Eina_List** edje_items;
    //for each slide, the list of edje object swallow inside
    //this is the objects added by the block edje{} in a presentation
    //when we destroy a slide we need to destroy each object swallowed inside
    Eina_List** edje_objects;
    //for each slide, the list of custom area
    //the type of nodes is Eyelight_Custom_Area
    Eina_List** custom_areas;
    //for each slide, the list of video object
    //the type of nodes is Eyelight_Video
    Eina_List** video_objects;
    char *video_module;

    const char** transition_effect_next;
    const char** transition_effect_previous;
    int size;
    int current;
    Evas_Object* slide_with_transition[2];

    Eyelight_Thumbnails* thumbnails;

    //pdf generator
    char* pdf_file;

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

Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,int pos);
Evas_Object* eyelight_viewer_slide_load(Eyelight_Viewer*pres,int pos);

void eyelight_viewer_default_transitions_load(Eyelight_Viewer*pres);
void eyelight_viewer_slide_transitions_load(Eyelight_Viewer*pres,int slide);
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

