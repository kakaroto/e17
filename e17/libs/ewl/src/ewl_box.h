
/*\
|*|
|*| The box widget is a layout manager and orders widget either horizontally
|*| or vertically.
|*|
\*/

#ifndef __EWL_BOX_H__
#define __EWL_BOX_H__

typedef struct _ewl_box Ewl_Box;
#define EWL_BOX(box) ((Ewl_Box *) box)

struct _ewl_box
{
	Ewl_Container container;
	Ewl_Orientation orientation;
	unsigned int homogeneous;
	unsigned int spacing;
};

#define ewl_hbox_new() ewl_box_new(EWL_ORIENTATION_HORIZONTAL)
#define ewl_vbox_new() ewl_box_new(EWL_ORIENTATION_VERTICAL)


Ewl_Widget *ewl_box_new(int orientation);
void ewl_box_set_orientation(Ewl_Widget * widget,
			     Ewl_Orientation orientation);
void ewl_box_set_spacing(Ewl_Widget * widget, unsigned int spacing);
void ewl_box_set_homogeneous(Ewl_Widget * widget, unsigned int homogeneous);

#endif /* __EWL_BOX_H__ */
