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


typedef struct Eyelight_Viewer Eyelight_Viewer;
typedef enum Eyelight_Viewer_State Eyelight_Viewer_State;
typedef struct Eyelight_Text_Class Eyelight_Text_Class;
#include "Eyelight.h"
#include <Eet.h>
#include "eyelight_viewer_thumbnails.h"

enum Eyelight_Viewer_State
{
    EYELIGHT_VIEWER_STATE_DEFAULT,
    EYELIGHT_VIEWER_STATE_EXPOSE,
    EYELIGHT_VIEWER_STATE_SLIDESHOW,
    EYELIGHT_VIEWER_STATE_GOTOSLIDE,
    EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS
};

struct Eyelight_Viewer
{
    Eyelight_Viewer_State state;

    Evas* evas;
    char* presentation;
    char* elt_file;
    char* edc_file;
    char *edje_file;

    char* theme;

    int default_size_w;
    int default_size_h;

    int with_border;
    Evas_Object** slides;
    const char** transition_effect_next;
    const char** transition_effect_previous;
    int size;
    int current;
    Evas_Object* slide_with_transition[2];

    Ecore_List* text_class;

    Eyelight_Thumbnails* thumbnails;

    //expose
    Evas_Object** expose_slides;
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

struct Eyelight_Text_Class
{
    char* name;
    char* font;
    int size;
};

Eyelight_Viewer_State eyelight_viewer_state_get(Eyelight_Viewer* pres);


Eyelight_Viewer* eyelight_viewer_new(Evas* evas,char* pres, char* theme,int with_boder);
void eyelight_viewer_destroy(Eyelight_Viewer**pres);



void eyelight_viewer_slides_init(Eyelight_Viewer*pres,int w, int h);
int eyelight_viewer_size_get(Eyelight_Viewer*pres);

int eyelight_viewer_current_id_get(Eyelight_Viewer* pres);

Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,int pos);
Evas_Object* eyelight_viewer_slide_load(Eyelight_Viewer*pres,int pos);

void eyelight_viewer_default_transitions_load(Eyelight_Viewer*pres);
void eyelight_viewer_slide_transitions_load(Eyelight_Viewer*pres,int slide);
void eyelight_viewer_slide_transitions_get(Eyelight_Viewer*pres,int slide, const char** previous, const char** next);
void eyelight_viewer_transitions_stop(Eyelight_Viewer*pres);
void eyelight_viewer_resize_screen(Eyelight_Viewer*pres, Evas_Coord w, Evas_Coord h);

void eyelight_viewer_slide_next(Eyelight_Viewer*pres);
void eyelight_viewer_slide_previous(Eyelight_Viewer*pres);
void eyelight_viewer_slide_goto(Eyelight_Viewer* pres, int slide_id);

void eyelight_viewer_expose_start(Eyelight_Viewer* pres, int select,int nb_lines, int nb_cols);
void eyelight_viewer_expose_stop(Eyelight_Viewer* pres);
void eyelight_viewer_expose_next(Eyelight_Viewer* pres);
void eyelight_viewer_expose_previous(Eyelight_Viewer* pres);
void eyelight_viewer_expose_window_next(Eyelight_Viewer* pres);
void eyelight_viewer_expose_window_previous(Eyelight_Viewer* pres);
void eyelight_viewer_expose_down(Eyelight_Viewer* pres);
void eyelight_viewer_expose_up(Eyelight_Viewer* pres);
void eyelight_viewer_expose_select(Eyelight_Viewer* pres);
void eyelight_viewer_expose_resize(Eyelight_Viewer* pres,int w, int h);

void eyelight_viewer_gotoslide_start(Eyelight_Viewer* pres);
void eyelight_viewer_gotoslide_stop(Eyelight_Viewer* pres);
void eyelight_viewer_gotoslide_digit_add(Eyelight_Viewer* pres, int digit);
void eyelight_viewer_gotoslide_goto(Eyelight_Viewer* pres);
void eyelight_viewer_gotoslide_digit_last_remove(Eyelight_Viewer* pres);
void eyelight_viewer_gotoslide_resize(Eyelight_Viewer* pres, int w, int h);


void eyelight_viewer_slideshow_start(Eyelight_Viewer* pres,int select);
void eyelight_viewer_slideshow_stop(Eyelight_Viewer* pres);
void eyelight_viewer_slideshow_next(Eyelight_Viewer* pres);
void eyelight_viewer_slideshow_previous(Eyelight_Viewer* pres);
void eyelight_viewer_slideshow_select(Eyelight_Viewer* pres);
void eyelight_viewer_slideshow_resize(Eyelight_Viewer* pres,int w, int h);

void eyelight_viewer_tableofcontents_start(Eyelight_Viewer* pres,int select);
void eyelight_viewer_tableofcontents_stop(Eyelight_Viewer* pres);
void eyelight_viewer_tableofcontents_next(Eyelight_Viewer* pres);
void eyelight_viewer_tableofcontents_previous(Eyelight_Viewer* pres);
void eyelight_viewer_tableofcontents_select(Eyelight_Viewer* pres);
void eyelight_viewer_tableofcontents_resize(Eyelight_Viewer* pres, int w, int h);

void eyelight_viewer_object_font_size_update(Eyelight_Viewer* pres,Evas_Object* o, int w, int h);
void eyelight_viewer_font_size_update(Eyelight_Viewer* pres);



#endif   /* ----- #ifndef EYELIGHT_VIEWER_INC  ----- */

