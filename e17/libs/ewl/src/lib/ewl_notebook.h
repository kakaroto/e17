#ifndef __EWL_NOTEBOOK_H__
#define __EWL_NOTEBOOK_H__

/**
 * @file ewl_notebook.h
 *
 * @defgroup Ewl_Notebook Notebook The notebook widget
 * @{
 */

/**
 * @themekey /notebook/file
 * @themekey /notebook/group
 */

#define EWL_NOTEBOOK_TYPE "notebook"

typedef struct Ewl_Notebook_Page Ewl_Notebook_Page;

#define EWL_NOTEBOOK_PAGE(page) ((Ewl_Notebook_Page *)page)

struct Ewl_Notebook_Page
{
	Ewl_Widget     *tab;
	Ewl_Widget     *page;
};

typedef struct Ewl_Notebook Ewl_Notebook;

#define EWL_NOTEBOOK(notebook) ((Ewl_Notebook *) notebook)

struct Ewl_Notebook
{
	Ewl_Box			 box;

	Ewl_Widget		*tab_box;
	Ewl_Widget		*page_box;

	Ewl_Notebook_Page	*visible_page;
	Ecore_List		*pages;

	Ewl_Position		 tab_position;
};

Ewl_Widget 	*ewl_notebook_new(void);
int	 	 ewl_notebook_init(Ewl_Notebook *n);
void		 ewl_notebook_page_append(Ewl_Notebook *n, Ewl_Widget *c,
							Ewl_Widget *l);
void		 ewl_notebook_page_prepend(Ewl_Notebook *n, Ewl_Widget *c,
							Ewl_Widget *l);
void		 ewl_notebook_page_insert(Ewl_Notebook *n, Ewl_Widget *c,
							Ewl_Widget *l, int p);

void		 ewl_notebook_first_page_remove(Ewl_Notebook *n);
void		 ewl_notebook_last_page_remove(Ewl_Notebook *n);
void		 ewl_notebook_page_remove(Ewl_Notebook *n, int i);
void		 ewl_notebook_visible_page_remove(Ewl_Notebook *n);

void		 ewl_notebook_tabs_alignment_set(Ewl_Notebook *n,
							unsigned int a);
unsigned int	 ewl_notebook_tabs_alignment_get(Ewl_Notebook *n);

void		 ewl_notebook_tabs_position_set(Ewl_Notebook *n,
							Ewl_Position p);
Ewl_Position	 ewl_notebook_tabs_position_get(Ewl_Notebook *n);

void		 ewl_notebook_tabs_visible_set(Ewl_Notebook *n, int show);

void		 ewl_notebook_visible_page_set(Ewl_Notebook *n, int t);
int		 ewl_notebook_visible_page_get(Ewl_Notebook *n);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_notebook_tab_click_cb(Ewl_Widget *widget, void *ev_data,
						void *user_data);
void ewl_notebook_destroy_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_notebook_configure_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_notebook_page_reparent_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_notebook_page_show_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_notebook_page_hide_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_notebook_page_destroy_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);

#endif


