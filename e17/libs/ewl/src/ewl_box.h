
#ifndef __EWL_BOX_H
#define __EWL_BOX_H

struct _ewl_box {
	Ewl_Widget						widget;
	Ewl_Orientation					orientation;
	Ebits_Object				  * ebits_object;
	unsigned int					homogeneous;
	unsigned int					spacing;
};

typedef struct _ewl_box				Ewl_Box;

#define EWL_BOX(box) ((Ewl_Box *) box)

Ewl_Widget * ewl_hbox_new(void);
Ewl_Widget * ewl_vbox_new(void);
Ewl_Widget * ewl_box_new_all(Ewl_Orientation orientation,
							 unsigned int spacing,
							 unsigned int homogeneous);
void ewl_box_set_orientation(Ewl_Widget * widget,
					  Ewl_Orientation orientation);
void ewl_box_set_spacing(Ewl_Widget * widget,
						 unsigned int spacing);
void ewl_box_set_homogeneous(Ewl_Widget * widget,
							 unsigned int homogeneous);

#endif
