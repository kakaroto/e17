#ifndef __EWL_NOTEBOOK_H__
#define __EWL_NOTEBOOK_H__

typedef struct Ewl_Notebook Ewl_Notebook;

#define EWL_NOTEBOOK(notebook) ((Ewl_Notebook *) notebook)

struct Ewl_Notebook
{
	Ewl_Container   container;

	Ewl_Widget     *tab_box;
	Ewl_Widget     *visible_page;

	Ewl_Notebook_Flags flags;
};

Ewl_Widget     *ewl_notebook_new(void);
void            ewl_notebook_init(Ewl_Notebook * n);
void            ewl_notebook_append_page(Ewl_Notebook * n, Ewl_Widget * c,
					 Ewl_Widget * l);
void            ewl_notebook_prepend_page(Ewl_Notebook * n, Ewl_Widget * c,
					  Ewl_Widget * l);
void            ewl_notebook_insert_page(Ewl_Notebook * n, Ewl_Widget * c,
					 Ewl_Widget * l, int p);

Ewl_Widget *ewl_notebook_remove_first_page(Ewl_Notebook * n);
Ewl_Widget *ewl_notebook_remove_last_page(Ewl_Notebook * n);
Ewl_Widget *ewl_notebook_remove_page(Ewl_Notebook * n, int i);
Ewl_Widget *ewl_notebook_remove_visible(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_alignment(Ewl_Notebook * n,
						unsigned int a);
unsigned int    ewl_notebook_get_tabs_alignment(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_position(Ewl_Notebook * n,
					       Ewl_Position p);
Ewl_Position    ewl_notebook_get_tabs_position(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_visible(Ewl_Notebook * n, int show);

#endif				/* __EWL_NOTEBOOK_H__ */
