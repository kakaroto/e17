
#ifndef __EWL_BOX_H__
#define __EWL_BOX_H__

enum _ewl_box_type {
	Ewl_Box_Type_Horisontal		= (1 << 1),
	Ewl_Box_Type_Vertical		= (1 << 2)
};

typedef enum _ewl_box_type	Ewl_Box_Type;

struct _ewl_box {
	Ewl_Widget widget;
	Ewl_Box_Type type;
	unsigned int spacing;
	unsigned int homogeneous;
};

typedef struct _ewl_box		Ewl_Box;

#define EWL_BOX(box) ((Ewl_Box *) box)

Ewl_Widget * ewl_box_new(Ewl_Box_Type type);
void ewl_box_set_type(Ewl_Widget * widget, Ewl_Box_Type type);
void ewl_box_set_spacing(Ewl_Widget * widget, unsigned int spacing);
void ewl_box_set_homogeneous(Ewl_Widget * widget, unsigned int homogeneous);

#endif
