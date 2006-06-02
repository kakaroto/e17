/** @file etk_textblock.h */
#ifndef _ETK_TEXTBLOCK_H_
#define _ETK_TEXTBLOCK_H_

#include "etk_object.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Textblock Etk_Textblock
 * @brief TODO: Etk_Textblock: brief description
 * @{
 */

/** Gets the type of a textblock */
#define ETK_TEXTBLOCK_TYPE       (etk_textblock_type_get())
/** Casts the object to an Etk_Textblock */
#define ETK_TEXTBLOCK(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK_TYPE, Etk_Textblock))
/** Check if the object is an Etk_Textblock */
#define ETK_IS_TEXTBLOCK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK_TYPE))

/** Gets the type of a textblock iterator */
#define ETK_TEXTBLOCK_ITER_TYPE       (etk_textblock_iter_type_get())
/** Casts the object to an Etk_Textblock_Iter */
#define ETK_TEXTBLOCK_ITER(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK_ITER_TYPE, Etk_Textblock_Iter))
/** Check if the object is an Etk_Textblock_Iter */
#define ETK_IS_TEXTBLOCK_ITER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK_ITER_TYPE))

/** TODOC */
typedef enum Etk_Textblock_Wrap
{
   ETK_TEXTBLOCK_WRAP_NONE,     /**< TODOC */        
   ETK_TEXTBLOCK_WRAP_WORD,     /**< TODOC */
   ETK_TEXTBLOCK_WRAP_CHAR      /**< TODOC */
} Etk_Textblock_Wrap;

/**
 * @brief @object The structure of a textblock iterator
 * @structinfo
 */
struct Etk_Textblock_Iter
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   Etk_Textblock *tb;
};

/**
 * @brief @object The structure of a textblock
 * @structinfo
 */
struct Etk_Textblock
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
   Evas_List *iters;
   Evas_List *evas_objects;
};

Etk_Type *etk_textblock_type_get();
Etk_Type *etk_textblock_iter_type_get();

Etk_Textblock *etk_textblock_new();

Evas_Object *etk_textblock_object_add(Etk_Textblock *tb, Evas *evas);

void etk_textblock_wrap_set(Evas_Object *tbo, Etk_Textblock_Wrap wrap);
Etk_Textblock_Wrap etk_textblock_wrap_get(Evas_Object *tbo);

/** @} */
 
#endif
