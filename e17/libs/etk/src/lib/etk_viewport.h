/** @file etk_viewport.h */
#ifndef _ETK_VIEWPORT_H_
#define _ETK_VIEWPORT_H_

#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Viewport Etk_Viewport
 * @{
 */

/** @brief Gets the type of an viewport */
#define ETK_VIEWPORT_TYPE       (etk_viewport_type_get())
/** @brief Casts the object to an Etk_Viewport */
#define ETK_VIEWPORT(obj)       (ETK_OBJECT_CAST((obj), ETK_VIEWPORT_TYPE, Etk_Viewport))
/** @brief Checks if the object is an Etk_Viewport */
#define ETK_IS_VIEWPORT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_VIEWPORT_TYPE))

/**
 * @struct Etk_Viewport
 * @brief A viewport is a bin container which has the ability to scroll. Thus, all the widgets that @n
 * have no scrolling ability have to be placed in a viewport, in order to be scrollable in an Etk_Scrolled_View
 */
struct _Etk_Viewport
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   Evas_Object *clip;
   int xscroll, yscroll;
};

Etk_Type *etk_viewport_type_get();
Etk_Widget *etk_viewport_new();

/** @} */

#endif
