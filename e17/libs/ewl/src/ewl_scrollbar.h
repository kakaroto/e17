
#ifndef __EWL_SCROLLBAR_H__
#define __EWL_SCROLLBAR_H__

typedef struct _ewl_scrollbar Ewl_Scrollbar;

#define EWL_SCROLLBAR(scrollbar) ((Ewl_Scrollbar *) scrollbar)

struct _ewl_scrollbar
{
	Ewl_Box box;

	Ewl_Widget *seeker;
	Ewl_Widget *decrement;
	Ewl_Widget *increment;
};

Ewl_Widget *ewl_scrollbar_new(Ewl_Orientation orientation);
void ewl_scrollbar_init(Ewl_Scrollbar * s, Ewl_Orientation orientation);

#endif /* __EWL_SCROLLBAR_H__ */
