#ifndef EWL_TREE2_VIEW_SCROLLED_H
#define EWL_TREE2_VIEW_SCROLLED_H

#define EWL_TREE2_VIEW_SCROLLED_TYPE "tree2_view_scrolled"

typedef struct Ewl_Tree2_View_Scrolled Ewl_Tree2_View_Scrolled;

#define EWL_TREE2_VIEW_SCROLLED(tv) ((Ewl_Tree2_View_Scrolled*)tv)

struct Ewl_Tree2_View_Scrolled
{
	Ewl_Box box;

	Ewl_Widget *scroll;

	unsigned char scroll_header:1;
	unsigned char scroll_visible:1;
};

Ewl_View	*ewl_tree2_view_scrolled_get(void);

Ewl_Widget	*ewl_tree2_view_scrolled_new(void);
int		 ewl_tree2_view_scrolled_init(Ewl_Tree2_View_Scrolled *tv);

void		 ewl_tree2_view_scrolled_scroll_headers_set(Ewl_Tree2_View *view,
							unsigned int scroll);
unsigned int	 ewl_tree2_view_scrolled_scroll_headers_get(Ewl_Tree2_View *view);

#endif


