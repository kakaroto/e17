/** @file etk_frame.h */
#ifndef _ETK_FRAME_H_
#define _ETK_FRAME_H_

#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Frame Etk_Frame
 * @brief A frame is a bin container with a label. It's useful to group widgets that logically need to be together
 * @{
 */

/** Gets the type of an frame */
#define ETK_FRAME_TYPE       (etk_frame_type_get())
/** Casts the object to an Etk_Frame */
#define ETK_FRAME(obj)       (ETK_OBJECT_CAST((obj), ETK_FRAME_TYPE, Etk_Frame))
/**  Checks if the object is an Etk_Frame */
#define ETK_IS_FRAME(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_FRAME_TYPE))

/**
 * @brief @widget The structure of a frame
 * @structinfo
 */
struct Etk_Frame
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   char *label;
};

Etk_Type *etk_frame_type_get();
Etk_Widget *etk_frame_new(const char *label);

void etk_frame_label_set(Etk_Frame *frame, const char *label);
const char *etk_frame_label_get(Etk_Frame *frame);

/** @} */

#endif
