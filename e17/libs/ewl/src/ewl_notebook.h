
#ifndef __EWL_NOTEBOOK_H__
#define __EWL_NOTEBOOK_H__

typedef struct _ewl_notebook_page Ewl_NotebookPage;

#define EWL_NOTEBOOK_PAGE(notebook_page) ((Ewl_NotebookPage *) notebook_page)

struct _ewl_notebook_page
{
	Ewl_Widget *tab;
	Ewl_Widget *page;
};

typedef struct _ewl_notebook Ewl_Notebook;

#define EWL_NOTEBOOK(notebook) ((Ewl_Notebook *) notebook)

struct _ewl_notebook
{
	Ewl_Container container;

	Ewl_Widget *tab_box;
	Ewl_Position tabs_position;

	Ewl_NotebookPage *visible_np;
	Ewd_List *pages;
};

Ewl_Widget *ewl_notebook_new(void);
void ewl_notebook_append_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l);
void ewl_notebook_prepend_page(Ewl_Widget * w, Ewl_Widget * c,
			       Ewl_Widget * l);
void ewl_notebook_insert_page(Ewl_Widget * w, Ewl_Widget * c,
			      Ewl_Widget * l, int p);

Ewl_NotebookPage *ewl_notebook_remove_first_page(Ewl_Widget * w);
Ewl_NotebookPage *ewl_notebook_remove_last_page(Ewl_Widget * w);
Ewl_NotebookPage *ewl_notebook_remove_page(Ewl_Widget * w, int i);
Ewl_NotebookPage *ewl_notebook_remove_visible(Ewl_Widget * w);

void ewl_notebook_set_tabs_alignment(Ewl_Widget * w, Ewl_Alignment a);
Ewl_Alignment ewl_notebook_get_tabs_alignment(Ewl_Widget * w);

void ewl_notebook_set_tabs_position(Ewl_Widget * w, Ewl_Position p);
Ewl_Position ewl_notebook_get_tabs_position(Ewl_Widget * w);


#endif /* __EWL_NOTEBOOK_H__ */
