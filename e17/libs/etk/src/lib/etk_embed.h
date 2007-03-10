/** @file etk_embed.h */
#ifndef _ETK_EMBED_H_
#define _ETK_EMBED_H_

#include "etk_toplevel.h"
#include "etk_types.h"

/* TODO/FIXME list:
 * - Add a method to add copy/paste support
 * - Maybe create sets of methods (pointer, position, copy/paste) for the most common Evas engines
 * - There may be a bug of focus when several embeds are packed in the same Evas
 */

/**
 * @defgroup Etk_Embed Etk_Embed
 * @brief The Etk_Embed widget is a toplevel widget that can be embedded in an existing Evas and can be
 * manipulated as a normal Evas object.
 * @{
 */

/** Gets the type of an embed widget */
#define ETK_EMBED_TYPE       (etk_embed_type_get())
/** Casts the object to an Etk_Embed */
#define ETK_EMBED(obj)       (ETK_OBJECT_CAST((obj), ETK_EMBED_TYPE, Etk_Embed))
/** Check if the object is an Etk_Embed */
#define ETK_IS_EMBED(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_EMBED_TYPE))


/**
 * @brief @widget A toplevel widget that can be embedded in an existing Evas and
 * be manipulated as a normal Evas object.
 * @structinfo
 */
struct Etk_Embed
{
   /* private: */
   /* Inherit from Etk_Toplevel */
   Etk_Toplevel toplevel;
   
   void (*position_get)(void *window_data, int *x, int *y);
   void *position_data;
   
   void (*pointer_set)(void *window_data, Etk_Pointer_Type pointer_type);
   void *pointer_data;
};


Etk_Type   *etk_embed_type_get(void);
Etk_Widget *etk_embed_new(Evas *evas);

Evas_Object *etk_embed_object_get(Etk_Embed *embed_widget);

void etk_embed_position_method_set(Etk_Embed *embed, void (*position_get)(void *position_data, int *x, int *y), void *position_data);
void etk_embed_pointer_method_set(Etk_Embed *embed, void (*pointer_set)(void *pointer_data, Etk_Pointer_Type pointer_type), void *pointer_data);

/** @} */

#endif
