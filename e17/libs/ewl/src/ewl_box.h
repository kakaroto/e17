
#ifndef __EWL_BOX_H__
#define __EWL_BOX_H__

/**
 * @file ewl_box.h
 * Defines the Ewl_Box class used for laying out Ewl_Widget's in a horizontal
 * or vertical line.
 */

/**
 * The box widget is an Ewl_Container and lays out Ewl_Widget's either
 * horizontally or vertically.
 */
typedef struct Ewl_Box Ewl_Box;

/**
 * @def EWL_BOX(box)
 * Typecast a pointer to an Ewl_Box pointer.
 */
#define EWL_BOX(box) ((Ewl_Box *) box)

/**
 * @struct Ewl_Box
 * Inherits from an Ewl_Container to provide layout facilities for child
 * widgets placed inside. Layout is done horizontally (left-to-right), or
 * vertically (top-to-bottom). There is also a flag for homogeneous layout,
 * which gives each child Ewl_Widget equal space inside the Ewl_Box.
 */
struct Ewl_Box
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */

	Ewl_Orientation orientation; /**< Indicate the orientation of layout */
	unsigned int    spacing; /**< Space between each widget in the box */

	unsigned int    homogeneous; /**< Flag indicating space assignemnt */
};

/**
 * @def ewl_hbox_new()
 * Shortcut to allocate a new horizontal Ewl_Box.
 */
#define ewl_hbox_new() ewl_box_new(EWL_ORIENTATION_HORIZONTAL)

/**
 * @def ewl_vbox_new()
 * Shortcut to allocate a new vertical Ewl_Box.
 */
#define ewl_vbox_new() ewl_box_new(EWL_ORIENTATION_VERTICAL)


Ewl_Widget     *ewl_box_new(Ewl_Orientation orientation);
int             ewl_box_init(Ewl_Box * box, Ewl_Orientation orientation);
void            ewl_box_set_orientation(Ewl_Box * b, Ewl_Orientation o);
void            ewl_box_set_spacing(Ewl_Box * b, int spacing);
void            ewl_box_set_homogeneous(Ewl_Box *b, int h);

#endif				/* __EWL_BOX_H__ */
