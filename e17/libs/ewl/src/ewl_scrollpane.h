
#ifndef __EWL_SCROLLPANE_H__
#define __EWL_SCROLLPANE_H__

typedef struct _ewl_scrollpane Ewl_ScrollPane;

#define EWL_SCROLLPANE(scrollpane) ((Ewl_ScrollPane *) scrollpane)

struct _ewl_scrollpane
{
	Ewl_Container container;

	Ewl_Widget *box;
	Ewl_Widget *hscrollbar;
	Ewl_Widget *vscrollbar;

	struct
	{
		unsigned int w, h;
	}
	box_size;
};

Ewl_Widget *ewl_scrollpane_new(void);
void ewl_scrollpane_init(Ewl_ScrollPane * s);

void ewl_scrollpane_set_hscrollbar_flag(Ewl_ScrollPane * s,
					Ewl_ScrollBar_Flags f);
void ewl_scrollpane_set_vscrollbar_flag(Ewl_ScrollPane * s,
					Ewl_ScrollBar_Flags f);

Ewl_ScrollBar_Flags ewl_scrollpane_get_hscrollbar_flag(Ewl_ScrollPane * s);
Ewl_ScrollBar_Flags ewl_scrollpane_get_vscrollbar_flag(Ewl_ScrollPane * s);

#endif /* __EWL_SCROLLPANE_H__ */
