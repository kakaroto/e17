#ifndef EWL_TREE2_VIEW_PLAIN_H
#define EWL_TREE2_VIEW_PLAIN_H

#define EWL_TREE2_VIEW_PLAIN_TYPE "tree2_view_plain"

typedef struct Ewl_Tree2_View_Plain Ewl_Tree2_View_Plain;

#define EWL_TREE2_VIEW_PLAIN(tv) ((Ewl_Tree2_View_Plain*)tv)

struct Ewl_Tree2_View_Plain
{
	Ewl_Tree2_View view;
};

Ewl_View	*ewl_tree2_view_plain_get(void);

Ewl_Widget	*ewl_tree2_view_plain_new(void);
int		 ewl_tree2_view_plain_init(Ewl_Tree2_View_Plain *tv);

#endif

