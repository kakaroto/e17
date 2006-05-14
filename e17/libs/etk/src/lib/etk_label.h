/** @file etk_label.h */
#ifndef _ETK_LABEL_H_
#define _ETK_LABEL_H_

#include <Evas.h>
#include "etk_widget.h"

/*
 * TODO/FIXME list:
 * - The style has to be defined by the theme
 * - Fix bad size_request and bad rendering (evas_textblock bug?)
 * - Document the different style tags
 */

/**
 * @defgroup Etk_Label Etk_Label
 * @brief The Etk_Label widget is a simple widget that can display text
 * @{
 */

/** Gets the type of a label */
#define ETK_LABEL_TYPE        (etk_label_type_get())
/** Casts the object to an Etk_Label */
#define ETK_LABEL(obj)        (ETK_OBJECT_CAST((obj), ETK_LABEL_TYPE, Etk_Label))
/** Check if the object is an Etk_Label */
#define ETK_IS_LABEL(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_LABEL_TYPE))

/**
 * @brief @widget The structure of a label
 * @structinfo
 */
struct Etk_Label
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *text_object;
   Evas_Object *clip;
   char *text;

   float xalign;
   float yalign;
};

Etk_Type *etk_label_type_get();
Etk_Widget *etk_label_new(const char *text);

void etk_label_set(Etk_Label *label, const char *text);
const char *etk_label_get(Etk_Label *label);

void etk_label_alignment_set(Etk_Label *label, float xalign, float yalign);
void etk_label_alignment_get(Etk_Label *label, float *xalign, float *yalign);

/** @} */

#endif
