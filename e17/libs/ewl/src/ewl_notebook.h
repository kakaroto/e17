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
void ewl_notebook_remove_visible(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_alignment(Ewl_Notebook * n,
						unsigned int a);
unsigned int    ewl_notebook_get_tabs_alignment(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_position(Ewl_Notebook * n,
					       Ewl_Position p);
Ewl_Position    ewl_notebook_get_tabs_position(Ewl_Notebook * n);

void            ewl_notebook_set_tabs_visible(Ewl_Notebook * n, int show);

void            ewl_notebook_set_visible_page(Ewl_Notebook *n, int t);
int             ewl_notebook_get_visible_page(Ewl_Notebook *n);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_notebook_configure_top_cb(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            ewl_notebook_configure_bottom_cb(Ewl_Widget * w, void *ev_data,
					         void *user_data);
void            ewl_notebook_configure_left_cb(Ewl_Widget * w, void *ev_data,
					       void *user_data);
void            ewl_notebook_configure_right_cb(Ewl_Widget * w, void *ev_data,
					        void *user_data);
void            ewl_notebook_add_cb(Ewl_Container *c, Ewl_Widget *w);
void            ewl_notebook_resize_cb(Ewl_Container *c, Ewl_Widget *w,
				       int size, Ewl_Orientation o);


void            ewl_notebook_tab_cb(Ewl_Widget *widget, void *ev_data, void *user_data); 

#endif				/* __EWL_NOTEBOOK_H__ */
