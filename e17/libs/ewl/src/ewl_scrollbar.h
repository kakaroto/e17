
#ifndef __EWL_SCROLLBAR_H__
#define __EWL_SCROLLBAR_H__

typedef struct _ewl_scrollbar Ewl_Scrollbar;

#define EWL_SCROLLBAR(scrollbar) ((Ewl_Scrollbar *) scrollbar)

struct _ewl_scrollbar
{
	Ewl_Box box;

	Ewl_Widget *seeker;
	Ewl_Widget *button_decrement;
	Ewl_Widget *button_increment;
	Ewl_Alignment buttons_alignment;

	double fill_percentage;

	Ewl_ScrollBar_Flags flag;
};

#define ewl_hscrollbar_new() ewl_scrollbar_new(EWL_ORIENTATION_HORIZONTAL)
#define ewl_vscrollbar_new() ewl_scrollbar_new(EWL_ORIENTATION_VERTICAL)

Ewl_Widget *ewl_scrollbar_new(Ewl_Orientation orientation);
void ewl_scrollbar_init(Ewl_Scrollbar * s, Ewl_Orientation orientation);

void ewl_scrollbar_set_fill_percentage(Ewl_Scrollbar * s, double fp);
double ewl_scrollbar_get_fill_percentage(Ewl_Scrollbar * s);

double ewl_scrollbar_get_value(Ewl_Scrollbar * s);

void ewl_scrollbar_set_flag(Ewl_Scrollbar * s, Ewl_ScrollBar_Flags f);
Ewl_ScrollBar_Flags ewl_scrollbar_get_flag(Ewl_Scrollbar * s);

#endif /* __EWL_SCROLLBAR_H__ */
