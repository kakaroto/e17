/** @file etk_textblock.h */
#ifndef _ETK_TEXTBLOCK_H_
#define _ETK_TEXTBLOCK_H_

#include <Evas.h>
#include <Ecore.h>
#include "etk_object.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Textblock Etk_Textblock
 * @{
 */

/** @brief Gets the type of a textblock */
#define ETK_TEXTBLOCK_TYPE       (etk_textblock_type_get())
/** @brief Casts the object to an Etk_Textblock */
#define ETK_TEXTBLOCK(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK_TYPE, Etk_Textblock))
/** @brief Checks if the object is an Etk_Textblock */
#define ETK_IS_TEXTBLOCK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK_TYPE))

/**
 * @struct Etk_Textblock_Iter
 * @brief TODO
 */
struct _Etk_Textblock_Iter
{
   Evas_Textblock_Cursor *cursor;
   Etk_Textblock *textblock;
};

/**
 * @struct Etk_Textblock
 * @brief TODO
 */
struct _Etk_Textblock
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   Evas_Object *smart_object;
   Evas_Object *textblock_object;
   Evas_Object *cursor_object;
   Evas_Object *clip;
   Evas_List *selection_rects;
   
   Etk_Textblock_Iter *cursor;
   Etk_Textblock_Iter *selection_start;
   Evas_List *iterators;
   
   Ecore_Timer *cursor_timer;
};

Etk_Type *etk_textblock_type_get();
Etk_Textblock *etk_textblock_new();

void etk_textblock_realize(Etk_Textblock *textblock, Evas *evas);
void etk_textblock_unrealize(Etk_Textblock *textblock);

/** @} */

#endif
