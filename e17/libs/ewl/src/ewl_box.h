
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

struct _ewl_box {
	Ewl_Container container;
	Ewl_Orientation orientation;
	unsigned int spacing;
};

#define ewl_hbox_new() ewl_box_new(EWL_ORIENTATION_HORIZONTAL)
#define ewl_vbox_new() ewl_box_new(EWL_ORIENTATION_VERTICAL)


Ewl_Widget *ewl_box_new(Ewl_Orientation orientation);
void ewl_box_init(Ewl_Box *box, Ewl_Orientation orientation);
void ewl_box_set_orientation(Ewl_Widget * w, Ewl_Orientation o);
void ewl_box_set_spacing(Ewl_Widget * w, unsigned int spacing);
void ewl_box_set_padding(Ewl_Widget * w, int xp, int yp);

#endif				/* __EWL_BOX_H__ */
