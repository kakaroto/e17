
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

typedef struct _ewl_box_child Ewl_Box_Child;
#define EWL_BOX_CHILD(child) ((Ewl_Box_Child *) child)

struct _ewl_box_child {
	Ewl_Widget *widget;
	Ewl_Alignment alignment;
	Ewl_Fill_Policy fill_policy;
	int x_padding;
	int y_padding;
};

#define ewl_hbox_new() ewl_box_new(EWL_ORIENTATION_HORIZONTAL)
#define ewl_vbox_new() ewl_box_new(EWL_ORIENTATION_VERTICAL)


Ewl_Widget *ewl_box_new(Ewl_Orientation orientation);
void ewl_box_set_orientation(Ewl_Box * w, Ewl_Orientation orientation);
void ewl_box_set_spacing(Ewl_Box * w, unsigned int spacing);
void ewl_box_append_child(Ewl_Box * w, Ewl_Widget * c, Ewl_Fill_Policy fp,
			  Ewl_Alignment a, int x_padding, int y_padding);
void ewl_box_prepend_child(Ewl_Box * w, Ewl_Widget * c, Ewl_Fill_Policy fp,
			   Ewl_Alignment a, int x_padding, int y_padding);

#endif				/* __EWL_BOX_H__ */
