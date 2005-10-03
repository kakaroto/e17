#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static int ewl_notebook_page_index_get(Ewl_Notebook *n,
					Ewl_Notebook_Page *p);

/**
 * ewl_notebook_new - create a new notebook
 * 
 * Returns a newly allocated notebook on success, NULL on failure.
 */
Ewl_Widget *
ewl_notebook_new(void)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = NEW(Ewl_Notebook, 1);
	if (!n) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_notebook_init(n)) {
		ewl_widget_destroy(EWL_WIDGET(n));
		n = NULL;
	}
	DRETURN_PTR(n, DLEVEL_STABLE);
}

/**
 * ewl_notebook_init - initialize a notebook to default values and callbacks
 * @n: the notebook to initialize
 *
 * Returns no value. Sets the fields and callbacks of @n to their
 * defaults.
 */
int
ewl_notebook_init(Ewl_Notebook * n)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, FALSE);

	w = EWL_WIDGET(n);

	if (!ewl_box_init(EWL_BOX(n))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_box_orientation_set(EWL_BOX(n), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

	/*
	 * these boxes will be appended when we call
	 * ewl_notebook_position_set so don't bother doing it here too
	 */
	n->tab_box = ewl_hbox_new();
	if (!n->tab_box) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_object_fill_policy_set(EWL_OBJECT(n->tab_box), EWL_FLAG_FILL_NONE);
	ewl_widget_internal_set(n->tab_box, TRUE);
	ewl_widget_appearance_set(n->tab_box, "tab_box");
	ewl_widget_show(n->tab_box);

	n->page_box = ewl_vbox_new();
	if (!n->page_box) {
		ewl_widget_destroy(n->tab_box);
		n->tab_box = NULL;

		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_internal_set(n->page_box, TRUE);
	ewl_widget_show(n->page_box);

	n->pages = ecore_list_new();
	if (!n->pages) {
		ewl_widget_destroy(n->tab_box);
		n->tab_box = NULL;

		ewl_widget_destroy(n->page_box);
		n->page_box = NULL;

		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_notebook_tabs_position_set(n, EWL_POSITION_TOP);
	ewl_notebook_tabs_alignment_set(n, EWL_FLAG_ALIGN_CENTER);
	ewl_widget_appearance_set(w, "notebook");
	ewl_widget_inherit(w, "notebook");

	ewl_callback_append(EWL_WIDGET(n), EWL_CALLBACK_DESTROY, 
							ewl_notebook_destroy_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_notebook_page_append - append a page to the notebook
 * @param n: the notebook to append the page
 * @param t: the tab of the new page added
 * @param p: the contents of the page added
 *
 * @return Returns no value. Appends a page to the list of available pages that will
 * be available for display.
 */
void
ewl_notebook_page_append(Ewl_Notebook * n, Ewl_Widget * t, Ewl_Widget * p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	ewl_notebook_page_insert(n, t, p, ecore_list_nodes(n->pages));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_page_prepend - prepend a page to the notebook
 * @param n: the notebook to prepend the page
 * @param t: the tab of the new page added
 * @param p: the contents of the page added
 *
 * @return Returns no value. Prepends a page to the list of available pages that will
 * be available for display.
 */
void
ewl_notebook_page_prepend(Ewl_Notebook * n, Ewl_Widget * t, Ewl_Widget * p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_notebook_page_insert(n, t, p, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_page_insert - insert a page to the notebook
 * @param n: the notebook to insert the page
 * @param t: the tab of the new page added
 * @param p: the contents of the page added
 * @param pos: the position in the list of tabs to add the page
 *
 * @return Returns no value. Insert a page to the list of available pages that will
 * be available for display.
 */
void
ewl_notebook_page_insert(Ewl_Notebook * n, Ewl_Widget * t,
					Ewl_Widget * p, int pos)
{
	Ewl_Notebook_Page *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("p", p);

	page = NEW(Ewl_Notebook_Page, 1);
	if (!page) {
		DLEAVE_FUNCTION(DLEVEL_STABLE);
	}

	page->page = p;
	page->tab = ewl_button_new();
	if (t)
		ewl_container_child_append(EWL_CONTAINER(page->tab), t);

	ewl_callback_append(page->tab, EWL_CALLBACK_CLICKED,
				ewl_notebook_tab_click_cb, page);
	ewl_object_fill_policy_set(EWL_OBJECT(page->tab),
						EWL_FLAG_FILL_NONE);
/*	ewl_widget_appearance_set(EWL_WIDGET(page->tab), "tab"); */
	ewl_widget_show(page->tab);

	ewl_container_child_insert(EWL_CONTAINER(n->tab_box), page->tab, pos);
	ewl_container_child_insert(EWL_CONTAINER(n->page_box), page->page, pos);

	ewl_widget_data_set(p, n, page);

	ewl_callback_append(p, EWL_CALLBACK_REPARENT,
				ewl_notebook_page_reparent_cb, n);
	ewl_callback_append(p, EWL_CALLBACK_SHOW,
				ewl_notebook_page_show_cb, n);
	ewl_callback_append(p, EWL_CALLBACK_HIDE,
				ewl_notebook_page_hide_cb, n);
	ewl_callback_append(p, EWL_CALLBACK_DESTROY,
				ewl_notebook_page_destroy_cb, n);

	ecore_list_goto_index(n->pages, pos);
	ecore_list_insert(n->pages, page);

	if (!n->visible_page) 
		ewl_notebook_visible_page_set(n, pos);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_first_page_remove - remove the first page from the notebook
 * @param n: the notebook to remove the first page
 *
 * @return Returns a pointer to the removed page on success, NULL on failure.
 */
void
ewl_notebook_first_page_remove(Ewl_Notebook * n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_notebook_page_remove(n, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_first_page_remove - remove the last page from the notebook
 * @param n: the notebook to remove the last page
 *
 * @return Returns a pointer to the removed page on success, NULL on failure.
 */
void
ewl_notebook_last_page_remove(Ewl_Notebook * n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_notebook_page_remove(n, ecore_list_nodes(n->pages));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_page_remove - remove the specified page from the notebook
 * @param n: the notebook to remove the specified page
 * @param i: the position in the list of pages to remove from @n
 *
 * @return Returns a pointer to the removed page on success, NULL on failure.
 */
void
ewl_notebook_page_remove(Ewl_Notebook * n, int i)
{
	Ewl_Notebook_Page *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	if (!ecore_list_nodes(n->pages)) {
		DRETURN(DLEVEL_STABLE);
	}

	ecore_list_goto_index(n->pages, i);
	page = ecore_list_remove(n->pages);
	if (page) {
		if (page == n->visible_page)
			ewl_notebook_visible_page_set(n, 0);

		if (page->tab) {
			ewl_container_child_remove(EWL_CONTAINER(n->tab_box),
								page->tab);
			ewl_widget_destroy(page->tab);
			page->tab = NULL;
		}
		if (page->page) {
			ewl_widget_destroy(page->page);
			page->page = NULL;
		}

		ewl_container_child_remove(EWL_CONTAINER(n->page_box),
							page->page);
		FREE(page);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_visible_page_remove - remove the visible page from the notebook
 * @param n: the notebook to remove the visible page
 *
 * @return Returns no value.
 */
void
ewl_notebook_visible_page_remove(Ewl_Notebook * n)
{
	Ewl_Notebook_Page *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	if (!n->visible_page) {
		DRETURN(DLEVEL_STABLE);
	}

	page = ecore_list_goto(n->pages, n->visible_page);
	if (!page) {
		DRETURN(DLEVEL_STABLE);
	}
	ewl_notebook_page_remove(n, ecore_list_index(n->pages));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_tabs_alignment_set - set the alignment of a notebooks tabs
 * @param n: the notebook to change tab alignment
 * @param a: the new alignment for the tabs of @a n
 *
 * @return Returns no value. Changes the alignment of the tabs on @a n to @a a, and
 * updates the display.
 */
void
ewl_notebook_tabs_alignment_set(Ewl_Notebook * n, unsigned int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_object_alignment_set(EWL_OBJECT(n->tab_box), a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_tabs_alignment_get - get the alignment of a notebooks tabs
 * @param n: the notebook to get tab alignment
 *
 * @return Returns the tab alignment of the notebook @n on success, 0 on failure.
 */
unsigned int
ewl_notebook_tabs_alignment_get(Ewl_Notebook * n)
{
	unsigned int align;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, 0);

	align = ewl_object_alignment_get(EWL_OBJECT(n->tab_box));

	DRETURN_INT(align, DLEVEL_STABLE);
}

/**
 * ewl_notebook_tabs_position_set - set a notebooks tab position
 * @param n: the notebook to change tab position
 * @param p: the new position for the tabs of @n
 *
 * @return Returns no value. Changes the tab position of @n to @p and updates the
 * display.
 */
void
ewl_notebook_tabs_position_set(Ewl_Notebook *n, Ewl_Position p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	if (n->tab_position == p) {
		DRETURN(DLEVEL_STABLE);
	}

	n->tab_position = p;
	switch (n->tab_position) {
		case EWL_POSITION_LEFT:
		case EWL_POSITION_RIGHT:
			ewl_box_orientation_set(EWL_BOX(n),
				EWL_ORIENTATION_HORIZONTAL);
			ewl_box_orientation_set(EWL_BOX(n->tab_box),
				EWL_ORIENTATION_VERTICAL);
			/*
			ewl_object_fill_policy_set(EWL_OBJECT(n->tab_box),
				EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);
				*/
			break;

		case EWL_POSITION_TOP:
		case EWL_POSITION_BOTTOM:
		default:
			ewl_box_orientation_set(EWL_BOX(n),
				EWL_ORIENTATION_VERTICAL);
			ewl_box_orientation_set(EWL_BOX(n->tab_box),
				EWL_ORIENTATION_HORIZONTAL);
			/*
			ewl_object_fill_policy_set(EWL_OBJECT(n->tab_box),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
			*/
			break;
	}

	ewl_container_child_remove(EWL_CONTAINER(n), n->tab_box);
	ewl_container_child_remove(EWL_CONTAINER(n), n->page_box);

	if ((n->tab_position == EWL_POSITION_RIGHT)
			|| (n->tab_position == EWL_POSITION_BOTTOM)) {
		ewl_container_child_append(EWL_CONTAINER(n), n->page_box);
		ewl_container_child_append(EWL_CONTAINER(n), n->tab_box);
	} else {
		ewl_container_child_append(EWL_CONTAINER(n), n->tab_box);
		ewl_container_child_append(EWL_CONTAINER(n), n->page_box);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_tabs_position_get - get the position of a notebooks tabs
 * @param n: the notebook to retrieve the tab position
 *
 * @return Returns the position of the tabs in @n on success, 0 on failure.
 */
Ewl_Position
ewl_notebook_tabs_position_get(Ewl_Notebook * n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, 0);

	DRETURN_INT(n->tab_position, DLEVEL_STABLE);
}

/**
 * ewl_notebook_tabs_visible_set - set the visibility of the tabs
 * @param n: the notebook to change flags
 * @param show: the sets if the tabs should be shown (0) or not (1)
 *
 * @return Returns no value. Sets the visiblity for the tabs
 */
void
ewl_notebook_tabs_visible_set(Ewl_Notebook * n, int show)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	if (show) 
		ewl_widget_show(n->tab_box);
	else 
		ewl_widget_hide(n->tab_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: the notebook to switch tabs
 * @param t: the tab number to switch to
 * @return Returns no value.
 * @brief Switches to the tab number @a t in the notebook widget @a n.
 *
 * The notebook @a n switches to the tab number @a t where @a t is between 0
 * and the number of widgets in the notebook.
 */
void
ewl_notebook_visible_page_set(Ewl_Notebook *n, int t)
{
	Ewl_Notebook_Page *page, *old_visible;
    
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	if (!ecore_list_nodes(n->pages)) {
		DRETURN(DLEVEL_STABLE);
	}

	page = ecore_list_goto_index(n->pages, t);
	if (!page) {
		DRETURN(DLEVEL_STABLE);
	}

	old_visible = n->visible_page;
	n->visible_page = page;
/*	ewl_widget_appearance_set(page->tab, "selected_tab"); */
	ewl_callback_del(page->tab, EWL_CALLBACK_CLICKED,
				ewl_notebook_tab_click_cb);
	ewl_widget_show(page->page);


	if (old_visible) {
		ewl_callback_append(old_visible->tab, EWL_CALLBACK_CLICKED,
				ewl_notebook_tab_click_cb, old_visible);
/*		ewl_widget_appearance_set(old_visible->tab, "tab"); */
		ewl_widget_hide(old_visible->page);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: the notebook to retrieve the current visible page number
 * @return Returns the current page number on success.
 * @brief Retrieves the position of the current page in the notebook @a n or
 * -1 on error..
 */
int
ewl_notebook_visible_page_get(Ewl_Notebook *n)
{
	int index = -1;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, index);

	index = ewl_notebook_page_index_get(n, n->visible_page);

	DRETURN_INT(index, DLEVEL_STABLE);
}

void
ewl_notebook_tab_click_cb(Ewl_Widget *widget, void *ev_data __UNUSED__,
							void *user_data)
{   
	Ewl_Notebook *nb;
	Ewl_Notebook_Page *page;
	int index;

	page = EWL_NOTEBOOK_PAGE(user_data);

	/* find the notebook containing the tab_bar containing this widget */
	nb = EWL_NOTEBOOK(widget->parent->parent);

	index = ewl_notebook_page_index_get(nb, page);
	if (index == -1)
		return;

	ewl_notebook_visible_page_set(nb, index);
}

void
ewl_notebook_page_reparent_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;
	Ewl_Notebook_Page *p;
	int index;

	n = EWL_NOTEBOOK(user_data);

	/* if we are alerady the parent, don't do anything... */
	if (EWL_WIDGET(ev_data) == w->parent)
		return;

	p = ewl_widget_data_get(w, n);
	if (!p) return;

	index = ewl_notebook_page_index_get(n, p);
	if (index == -1)
		return;

	ewl_notebook_page_remove(n, index);
}

/*
 * don't let them show the widget if we are controlling it and its not the
 * visible page 
 */
void 
ewl_notebook_page_show_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data)
{
    Ewl_Notebook *n;
    Ewl_Notebook_Page *p;

    n = EWL_NOTEBOOK(user_data);
    p = ewl_widget_data_get(w, n);
    if (!p) return;

    if (p != n->visible_page)
	ewl_widget_hide(w);
}

/*
 * don't let them hide the widget if we are controlling it and its the
 * visible page
 */
void
ewl_notebook_page_hide_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data)
{
    Ewl_Notebook *n;
    Ewl_Notebook_Page *p;

    n = EWL_NOTEBOOK(user_data);
    p = ewl_widget_data_get(w, n);
    if (!p) return;

    if (p == n->visible_page)
	ewl_widget_show(w);
}

/*
 * if the user deletes one of our pages we need to clean up after ourselfs
 */
void
ewl_notebook_page_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data)
{
    Ewl_Notebook *n;
    Ewl_Notebook_Page *p;
    int index;

    n = EWL_NOTEBOOK(user_data);
    p = ewl_widget_data_get(w, n);
    if (!p) return;

    index = ewl_notebook_page_index_get(n, p);
    if (index == -1)
	return;

    ewl_notebook_page_remove(n, index);
}

void
ewl_notebook_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Notebook *n;

	n = EWL_NOTEBOOK(w);

	ecore_list_destroy(n->pages);
	n->pages = NULL;	
}

static int
ewl_notebook_page_index_get(Ewl_Notebook *n, Ewl_Notebook_Page *p)
{
	if (!ecore_list_goto(n->pages, p))
		return -1;
	
	return ecore_list_index(n->pages);
}

