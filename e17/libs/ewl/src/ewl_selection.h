
#ifndef __EWL_SELECTION_H__
#define __EWL_SELECTION_H__

typedef struct _ewl_selection Ewl_Selection;

#define EWL_SELECTION(selection) ((Ewl_Selection *) selection)

struct _ewl_selection
{
	Ewl_Widget widget;

	int start_pos;
	int end_pos;
};

Ewl_Widget *ewl_selection_new(void);
void ewl_selection_set_covered(Ewl_Widget * w, int s, int e);
void ewl_selection_get_covered(Ewl_Widget * w, int *s, int *e);
void ewl_selection_expand(Ewl_Widget * w, int p);


#endif /* __EWL_SELECTION_H__ */
