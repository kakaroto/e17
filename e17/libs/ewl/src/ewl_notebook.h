#ifndef __EWL_NOTEBOOK_H__
#define __EWL_NOTEBOOK_H__

/*
 * @themekey /notebook/file
 * @themekey /notebook/group
 */

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
int             ewl_notebook_init(Ewl_Notebook * n);
void            ewl_notebook_page_append(Ewl_Notebook * n, Ewl_Widget * c,
                                                    Ewl_Widget * l);
void            ewl_notebook_page_prepend(Ewl_Notebook * n, Ewl_Widget * c,
                                                    Ewl_Widget * l);
void            ewl_notebook_page_insert(Ewl_Notebook * n, Ewl_Widget * c,
                                                    Ewl_Widget * l, int p);

void            ewl_notebook_first_page_remove(Ewl_Notebook * n);
void            ewl_notebook_last_page_remove(Ewl_Notebook * n);
void            ewl_notebook_page_remove(Ewl_Notebook * n, int i);
void            ewl_notebook_visible_page_remove(Ewl_Notebook * n);

void            ewl_notebook_tabs_alignment_set(Ewl_Notebook * n,
                                                    unsigned int a);
unsigned int    ewl_notebook_tabs_alignment_get(Ewl_Notebook * n);

void            ewl_notebook_tabs_position_set(Ewl_Notebook * n,
                                                    Ewl_Position p);
Ewl_Position    ewl_notebook_tabs_position_get(Ewl_Notebook * n);

void            ewl_notebook_tabs_visible_set(Ewl_Notebook * n, int show);

void            ewl_notebook_visible_page_set(Ewl_Notebook *n, int t);
int             ewl_notebook_visible_page_get(Ewl_Notebook *n);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_notebook_tab_click_cb(Ewl_Widget *widget, void *ev_data,
				    void *user_data); 

void            ewl_notebook_configure_top_cb(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            ewl_notebook_configure_bottom_cb(Ewl_Widget * w, void *ev_data,
					         void *user_data);
void            ewl_notebook_configure_left_cb(Ewl_Widget * w, void *ev_data,
					       void *user_data);
void            ewl_notebook_configure_right_cb(Ewl_Widget * w, void *ev_data,
					        void *user_data);
void            ewl_notebook_page_reparent_cb(Ewl_Widget *w, void *ev_data, 
					      void *user_data);
void            ewl_notebook_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void            ewl_notebook_child_resize_cb(Ewl_Container *c, Ewl_Widget *w,
				       int size, Ewl_Orientation o);


#endif				/* __EWL_NOTEBOOK_H__ */
