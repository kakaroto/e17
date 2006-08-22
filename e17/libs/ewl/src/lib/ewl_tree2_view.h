#ifndef EWL_TREE2_VIEW_H
#define EWL_TREE2_VIEW_H

#define EWL_TREE2_VIEW_TYPE "tree2_view"

#define EWL_TREE2_VIEW(v) ((Ewl_Tree2_View *)v)

typedef struct Ewl_Tree2_View Ewl_Tree2_View;
struct Ewl_Tree2_View
{
	Ewl_Box box;

	Ewl_Tree2 *parent;
};

int ewl_tree2_view_init(Ewl_Tree2_View *v);

void ewl_tree2_view_tree2_set(Ewl_Tree2_View *v, Ewl_Tree2 *t);
Ewl_Tree2 *ewl_tree2_view_tree2_get(Ewl_Tree2_View *v);

#endif

