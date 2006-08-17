/** @file etk_alignment.h */
#ifndef _ETK_ALIGNMENT_H_
#define _ETK_ALIGNMENT_H_

#include "etk_bin.h"
#include "etk_types.h"

/**
 * @defgroup Etk_Alignment Etk_Alignment
 * @brief The Etk_Alignment widget is a container that can contain
 * one child which can be aligned and scaled inside the container
 * @{
 */
 
/** Gets the type of an alignment */
#define ETK_ALIGNMENT_TYPE       (etk_alignment_type_get())
/** Casts the object to an Etk_Alignment */
#define ETK_ALIGNMENT(obj)       (ETK_OBJECT_CAST((obj), ETK_ALIGNMENT_TYPE, Etk_Alignment))
/** Checks if the object is an Etk_Alignment */
#define ETK_IS_ALIGNMENT(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_ALIGNMENT_TYPE))

/**
 * @brief @widget A container containing one child which can be aligned and scaled
 * @structinfo
 */
struct Etk_Alignment
{
   /* private: */
   /* Inherit from Etk_Bin */
   Etk_Bin bin;

   float xalign;
   float yalign;
   float xscale;
   float yscale;
};

Etk_Type   *etk_alignment_type_get();
Etk_Widget *etk_alignment_new(float xalign, float yalign, float xscale, float yscale);

void etk_alignment_set(Etk_Alignment *alignment, float xalign, float yalign, float xscale, float yscale);
void etk_alignment_get(Etk_Alignment *alignment, float *xalign, float *yalign, float *xscale, float *yscale);

/** @} */

#endif
