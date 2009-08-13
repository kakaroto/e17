/*
 * =====================================================================================
 *
 *       Filename:  eyelight_object.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/12/2009 11:56:41 AM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_OBJECT_INC
#define  EYELIGHT_OBJECT_INC

#include "Eyelight.h"
Evas_Object *eyelight_object_area_obj_get(Eyelight_Viewer *pres, int id_slide,
        Evas_Object *o_slide, const char *area, char *complete_area_name);


Evas_Object *eyelight_object_title_add(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text);

Evas_Object *eyelight_object_subtitle_add(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text);
Evas_Object *eyelight_object_header_image_add(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node, Evas_Object *o_slide, const char *default_image);
Evas_Object *eyelight_object_foot_text_add(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node, Evas_Object *o_slide, const char *default_text);
Evas_Object *eyelight_object_foot_image_add(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node, Evas_Object *o_slide, const char *default_image);
Evas_Object *eyelight_object_pages_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, int slide_number, int nb_slides);

Evas_Object *eyelight_object_item_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area,const char *text);
Evas_Object *eyelight_object_item_image_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *image, int border, int shadow);
Evas_Object *eyelight_object_item_video_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *video, int alpha, int autoplay, int replay, int border, int shadow);
Evas_Object *eyelight_object_item_presentation_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *presentation, const char *theme, int border, int shadow);
Evas_Object *eyelight_object_item_simple_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, int depth, const char *text);
Evas_Object *eyelight_object_item_summary_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, int id_summary, int id_item,  const char *area, int depth, const char *text);
Evas_Object *eyelight_object_item_numbering_text_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, char *numbering_id, int depth, const char *text);
Evas_Object *eyelight_object_item_edje_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, const char *edj_file, const char *edje_group);

Evas_Object *eyelight_object_custom_area_add(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide, const char *area, double rel1_x, double rel1_y, double rel2_x, double rel2_y);

#endif   /* ----- #ifndef EYELIGHT_OBJECT_INC  ----- */

