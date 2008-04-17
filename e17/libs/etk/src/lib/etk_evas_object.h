/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/** @file etk_evas_object.h */
#ifndef _ETK_EVAS_OBJECT_H_
#define _ETK_EVAS_OBJECT_H_

#include <Evas.h>

#include "etk_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Evas_Object Etk_Evas_Object
 * @brief An Etk_Evas_Object is a widget that can accept any Evas object and
 * turn it into an Etk_Widget for use inside Etk.
 * @{
 */

/** Gets the type of an etk_evas widget */
#define ETK_EVAS_OBJECT_TYPE        (etk_evas_object_type_get())
/** Casts the object to an Etk_Evas_Object */
#define ETK_EVAS_OBJECT(obj)        (ETK_OBJECT_CAST((obj), ETK_EVAS_OBJECT_TYPE, Etk_Evas_Object))
/** Check if the object is an Etk_Evas_Object */
#define ETK_IS_EVAS_OBJECT(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_EVAS_OBJECT_TYPE))

/**
 * @brief @widget A widget that can load Evas objects for use inside Etk
 * @structinfo
 */
struct Etk_Evas_Object
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Evas_Object *object;
};


Etk_Type    *etk_evas_object_type_get(void);
Etk_Widget  *etk_evas_object_new();
Etk_Widget  *etk_evas_object_new_from_object(Evas_Object *evas_object);
void etk_evas_object_set_object(Etk_Evas_Object *etk_evas_object, Evas_Object *evas_object);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
