/** @file etk_box.h */
#ifndef _ETK_BOX_H_
#define _ETK_BOX_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Box Etk_Box
 * @{
 */

/** @brief Gets the type of a box */
#define ETK_BOX_TYPE       (etk_box_type_get())
/** @brief Casts the object to an Etk_Box */
#define ETK_BOX(obj)       (ETK_OBJECT_CAST((obj), ETK_BOX_TYPE, Etk_Box))
/** @brief Checks if the object is an Etk_Box */
#define ETK_IS_BOX(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_BOX_TYPE))

/**
 * @struct Etk_Box_Child_Props
 * @brief Describes how a child of the box should expand, fill (...) the allocated space
 * @note You can change directly those values, but you'll then have to call @a etk_widget_size_recalc() on the bin
 */
struct _Etk_Box_Child_Props
{
   int padding;
   Etk_Bool expand;
   Etk_Bool fill;
   Etk_Bool pack_end;
};

/**
 * @struct Etk_Box
 * @brief An Etk_Box is a container that can contain several children packed in one direction (horizontal of vertical) 
 */
struct _Etk_Box
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   int spacing;
   Etk_Bool homogeneous;
   Evas_List *children;
};

Etk_Type *etk_box_type_get();

void etk_box_pack_start(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding);
void etk_box_pack_end(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding);
void etk_box_child_reorder(Etk_Box *box, Etk_Widget *child, int position);

void etk_box_spacing_set(Etk_Box *box, int spacing);
int etk_box_spacing_get(Etk_Box *box);

void etk_box_homogeneous_set(Etk_Box *box, Etk_Bool homogeneous);
Etk_Bool etk_box_homogeneous_get(Etk_Box *box);

/** @} */

#endif
