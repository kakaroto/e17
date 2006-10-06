/** @file etk_embed.h */
#ifndef _ETK_EMBED_H_
#define _ETK_EMBED_H_

#include "etk_toplevel.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Embed Etk_Embed
 * @brief The Etk_Embed widget is a toplevel widget that can be embed in an existing Evas and can be
 * manipulated as a normal evas object.
 * @{
 */

/** @brief Gets the type of an embed widget */
#define ETK_EMBED_TYPE       (etk_embed_type_get())
/** @brief Casts the object to an Etk_Embed */
#define ETK_EMBED(obj)       (ETK_OBJECT_CAST((obj), ETK_EMBED_TYPE, Etk_Embed))
/** @brief Check if the object is an Etk_Embed */
#define ETK_IS_EMBED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_EMBED_TYPE))

/**
 * @brief @widget A toplevel widget that can be manipulated as a normal evas object.
 * @structinfo
 */
struct Etk_Embed
{
   /* private: */
   /* Inherit from Etk_Toplevel */
   Etk_Toplevel toplevel;
   
   void (*window_position_get)(void *window_data, int *x, int *y);
   void *window_data;
};

Etk_Type   *etk_embed_type_get();
Etk_Widget *etk_embed_new(Evas *evas, void (*window_position_get)(void *window_data, int *x, int *y), void *window_data);

Evas_Object *etk_embed_object_get(Etk_Embed *embed_widget);
  
/** @} */

#endif
