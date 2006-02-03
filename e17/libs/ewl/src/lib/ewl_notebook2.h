#ifndef EWL_NOTEBOOK2_H
#define EWL_NOTEBOOK2_H

#define EWL_NOTEBOOK2_TYPE "notebook2"

#define EWL_NOTEBOOK2(n) ((Ewl_Notebook2 *)n)

typedef struct Ewl_Notebook2 Ewl_Notebook2;

struct Ewl_Notebook2
{
	Ewl_Box box;

	struct {
		Ewl_Widget *tabbar;
		Ewl_Widget *pages;
	} body;

	Ewl_Widget *cur_page;
	Ewl_Position tabbar_position;
};

Ewl_Widget	*ewl_notebook2_new(void);
int		 ewl_notebook2_init(Ewl_Notebook2 *n);

void		 ewl_notebook2_tabbar_alignment_set(Ewl_Notebook2 *n, 
						unsigned int align);
unsigned int	 ewl_notebook2_tabbar_alignment_get(Ewl_Notebook2 *n);

void		 ewl_notebook2_tabbar_position_set(Ewl_Notebook2 *n, 
						Ewl_Position pos);
Ewl_Position	 ewl_notebook2_tabbar_position_get(Ewl_Notebook2 *n);

void		 ewl_notebook2_tabbar_visible_set(Ewl_Notebook2 *n,
						unsigned int visible);
unsigned int 	 ewl_notebook2_tabbar_visible_get(Ewl_Notebook2 *n);

void		 ewl_notebook2_visible_page_set(Ewl_Notebook2 *n, 
						Ewl_Widget *page);
Ewl_Widget	*ewl_notebook2_visible_page_get(Ewl_Notebook2 *n);

void		 ewl_notebook2_page_tab_text_set(Ewl_Notebook2 *n, 
						Ewl_Widget *page, 
						const char *text);
const char 	*ewl_notebook2_page_tab_text_get(Ewl_Notebook2 *n, 
						Ewl_Widget *page);

void		 ewl_notebook2_page_tab_widget_set(Ewl_Notebook2 *n,
						Ewl_Widget *page,
						Ewl_Widget *tab);
Ewl_Widget	*ewl_notebook2_page_tab_widget_get(Ewl_Notebook2 *n,
						Ewl_Widget *page);

/**
 * Internal stuff.
 */
void ewl_notebook2_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_notebook2_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook2_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook2_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook2_cb_child_remove(Ewl_Container *c, Ewl_Widget *w);

void ewl_notebook2_cb_tab_clicked(Ewl_Widget *w, void *ev, void *data);

#endif

