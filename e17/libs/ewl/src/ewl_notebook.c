
#include <Ewl.h>


void            __ewl_notebook_init(Ewl_Notebook * n);
void            __ewl_notebook_realize(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_notebook_configure(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_notebook_destroy(Ewl_Widget * w, void *ev_data,
				       void *user_data);

Ewl_NotebookPage *ewl_notebook_page_new(Ewl_Widget * t, Ewl_Widget * p);

void            __ewl_container_configure_clip_box(Ewl_Widget * w,
						   void *event_data,
						   void *user_data);


Ewl_NotebookPage *__ewl_notebook_page_create(Ewl_Notebook * n,
					     Ewl_Widget * c, Ewl_Widget * l);

/**
 * ewl_notebook_new - create a new notebook
 * 
 * Returns a newly allocated notebook on success, NULL on failure.
 */
Ewl_Widget     *ewl_notebook_new(void)
{
	Ewl_Notebook   *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = NEW(Ewl_Notebook, 1);
	ZERO(n, Ewl_Notebook, 1);

	ewl_notebook_init(n);

	DRETURN_PTR(EWL_WIDGET(n), DLEVEL_TESTING);
}

/**
 * ewl_notebook_append_page - append a page to the notebook
 * @n: the notebook to append the page
 * @t: the tab of the new page added
 * @p: the contents of the page added
 *
 * Returns no value. Appends a page to the list of available pages that will
 * be available for display.
 */
void ewl_notebook_append_page(Ewl_Notebook * n, Ewl_Widget * t, Ewl_Widget * p)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);
	np = __ewl_notebook_page_create(n, t, p);

	ewl_container_append_child(EWL_CONTAINER(w), np->page);
	ewl_container_append_child(EWL_CONTAINER(n->tab_box), np->tab);

	ewd_list_append(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_prepend_page - prepend a page to the notebook
 * @n: the notebook to prepend the page
 * @t: the tab of the new page added
 * @p: the contents of the page added
 *
 * Returns no value. Prepends a page to the list of available pages that will
 * be available for display.
 */
void ewl_notebook_prepend_page(Ewl_Notebook * n, Ewl_Widget * t, Ewl_Widget * p)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);
	np = __ewl_notebook_page_create(n, t, p);

	ewl_container_prepend_child(EWL_CONTAINER(w), np->page);
	ewl_container_prepend_child(EWL_CONTAINER(n->tab_box), np->tab);

	ewd_list_prepend(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_insert_page - insert a page to the notebook
 * @n: the notebook to insert the page
 * @t: the tab of the new page added
 * @p: the contents of the page added
 * @pos: the position in the list of tabs to add the page
 *
 * Returns no value. Insert a page to the list of available pages that will
 * be available for display.
 */
void
ewl_notebook_insert_page(Ewl_Notebook * n, Ewl_Widget * t, Ewl_Widget * p,
			 int pos)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);
	np = __ewl_notebook_page_create(n, t, p);

	ewl_container_insert_child(EWL_CONTAINER(w), np->page, pos);
	ewl_container_insert_child(EWL_CONTAINER(n->tab_box), np->tab, pos);

	ewd_list_goto_index(n->pages, pos);
	ewd_list_insert(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_first_page - remove the first page from the notebook
 * @n: the notebook to remove the first page
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
Ewl_NotebookPage *ewl_notebook_remove_first_page(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;
	Ewl_NotebookPage *nnp;
	Ewl_Widget     *tab;
	Ewl_Widget     *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);

	w = EWL_WIDGET(n);
	np = NULL;

	if (!n->pages || ewd_list_is_empty(n->pages))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	np = ewd_list_goto_first(n->pages);

	if (np == n->visible_np)
		DRETURN_PTR(ewl_notebook_remove_visible(n), DLEVEL_STABLE);

	if (!np)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_remove(n->pages);

	ewl_container_remove_child(EWL_CONTAINER(n->tab_box), np->tab);
	ewl_container_remove_child(EWL_CONTAINER(w), np->page);

	tab = ewd_list_goto_first(EWL_CONTAINER(np->tab)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->tab), tab);
	ewl_widget_hide(tab);

	page = ewd_list_goto_first(EWL_CONTAINER(np->page)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->page), page);
	ewl_widget_hide(page);

	nnp = ewl_notebook_page_new(tab, page);

	ewl_widget_destroy(np->tab);
	ewl_widget_destroy(np->page);

	ewl_widget_configure(w);

	DRETURN_PTR(nnp, DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_first_page - remove the last page from the notebook
 * @n: the notebook to remove the last page
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
Ewl_NotebookPage *ewl_notebook_remove_last_page(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;
	Ewl_NotebookPage *nnp;
	Ewl_Widget     *tab;
	Ewl_Widget     *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);

	w = EWL_WIDGET(n);
	np = NULL;

	if (!n->pages || ewd_list_is_empty(n->pages))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	np = ewd_list_goto_last(n->pages);

	if (np == n->visible_np)
		DRETURN_PTR(ewl_notebook_remove_visible(n), DLEVEL_STABLE);

	if (!np)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_remove(n->pages);

	ewl_container_remove_child(EWL_CONTAINER(n->tab_box), np->tab);
	ewl_container_remove_child(EWL_CONTAINER(w), np->page);

	tab = ewd_list_goto_first(EWL_CONTAINER(np->tab)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->tab), tab);
	ewl_widget_hide(tab);

	page = ewd_list_goto_first(EWL_CONTAINER(np->page)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->page), page);
	ewl_widget_hide(page);

	nnp = ewl_notebook_page_new(tab, page);

	ewl_widget_destroy(np->tab);
	ewl_widget_destroy(np->page);

	ewl_widget_configure(w);

	DRETURN_PTR(nnp, DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_page - remove the specified page from the notebook
 * @n: the notebook to remove the specified page
 * @i: the position in the list of pages to remove from @n
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
Ewl_NotebookPage *ewl_notebook_remove_page(Ewl_Notebook * n, int i)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;
	Ewl_NotebookPage *nnp;
	Ewl_Widget     *tab;
	Ewl_Widget     *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);

	w = EWL_WIDGET(n);
	np = NULL;

	if (!n->pages || ewd_list_is_empty(n->pages))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	np = ewd_list_goto_index(n->pages, i);

	if (np == n->visible_np)
		DRETURN_PTR(ewl_notebook_remove_visible(n), DLEVEL_STABLE);

	if (!np)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_remove(n->pages);

	ewl_container_remove_child(EWL_CONTAINER(n->tab_box), np->tab);
	ewl_container_remove_child(EWL_CONTAINER(w), np->page);

	tab = ewd_list_goto_first(EWL_CONTAINER(np->tab)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->tab), tab);
	ewl_widget_hide(tab);

	page = ewd_list_goto_first(EWL_CONTAINER(np->page)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->page), page);
	ewl_widget_hide(page);

	nnp = ewl_notebook_page_new(tab, page);

	ewl_widget_destroy(np->tab);
	ewl_widget_destroy(np->page);

	ewl_widget_configure(w);

	DRETURN_PTR(nnp, DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_visible - remove the visible page from the notebook
 * @n: the notebook to remove the visible page
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
Ewl_NotebookPage *ewl_notebook_remove_visible(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;
	Ewl_NotebookPage *nnp;
	Ewl_Widget     *tab;
	Ewl_Widget     *page;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);

	w = EWL_WIDGET(n);
	np = NULL;

	if (!n->pages || ewd_list_is_empty(n->pages))
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewd_list_goto(n->pages, n->visible_np);
	ewd_list_remove(n->pages);
	np = n->visible_np;

	if (!np)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_container_remove_child(EWL_CONTAINER(n->tab_box), np->tab);
	ewl_container_remove_child(EWL_CONTAINER(w), np->page);

	tab = ewd_list_goto_first(EWL_CONTAINER(np->tab)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->tab), tab);
	ewl_widget_hide(tab);

	page = ewd_list_goto_first(EWL_CONTAINER(np->page)->children);
	ewl_container_remove_child(EWL_CONTAINER(np->page), page);
	ewl_widget_hide(page);

	nnp = ewl_notebook_page_new(tab, page);

	ewl_widget_destroy(n->visible_np->tab);
	ewl_widget_destroy(n->visible_np->page);

	if (ewd_list_index(n->pages) >= ewd_list_nodes(n->pages))
		n->visible_np = ewd_list_goto_last(n->pages);
	else
		n->visible_np = ewd_list_current(n->pages);

	ewl_widget_show(n->visible_np->page);

	ewl_widget_theme_update(n->visible_np->tab);

	ewl_widget_configure(w);

	DRETURN_PTR(nnp, DLEVEL_STABLE);
}

/**
 * ewl_notebook_set_tabs_alignment - set the alignment of a notebooks tabs
 * @n: the notebook to change tab alignment
 * @a: the new alignment for the tabs of @n
 *
 * Returns no value. Changes the alignment of the tabs on @n to @a, and
 * updates the display.
 */
void ewl_notebook_set_tabs_alignment(Ewl_Notebook * n, Ewl_Alignment a)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	ewl_object_set_alignment(EWL_OBJECT(n->tab_box), a);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_get_tabs_alignment - get the alignment of a notebooks tabs
 * @n: the notebook to get tab alignment
 *
 * Returns the tab alignment of the notebook @n on success, 0 on failure.
 */
Ewl_Alignment ewl_notebook_get_tabs_alignment(Ewl_Notebook * n)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, 0);

	w = EWL_WIDGET(n);

	DRETURN_INT(ewl_object_get_alignment(EWL_OBJECT(n->tab_box)),
		    DLEVEL_TESTING);
}

/**
 * ewl_notebook_set_tabs_position - set a notebooks tab position
 * @n: the notebook to change tab position
 * @p: the new position for the tabs of @n
 *
 * Returns no value. Changes the tab position of @n to @p and updates the
 * display.
 */
void ewl_notebook_set_tabs_position(Ewl_Notebook * n, Ewl_Position p)
{
	Ewl_Widget     *w;
	Ewl_NotebookPage *np;
	char            file[PATH_LEN];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	n->tabs_position = p;

	switch (n->tabs_position) {
		case EWL_POSITION_LEFT:
			snprintf(file, PATH_LEN, "/notebook/tab_button/left");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_VERTICAL);
		case EWL_POSITION_RIGHT:
			snprintf(file, PATH_LEN, "/notebook/tab_button/right");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_VERTICAL);
			break;
		case EWL_POSITION_BOTTOM:
			snprintf(file, PATH_LEN, "/notebook/tab_button/bottom");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_HORIZONTAL);
			break;
		case EWL_POSITION_TOP:
		default:
			snprintf(file, PATH_LEN, "/notebook/tab_button/top");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_HORIZONTAL);
			break;
	}

	ewd_list_goto_first(n->pages);

	while ((np = ewd_list_next(n->pages)) != NULL)
		ewl_theme_data_set_str(np->tab, "/notebook/tab_button/base",
				       file);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_get_tabs_position - get the position of a notebooks tabs
 * @n: the notebook to retrieve the tab position
 *
 * Returns the position of the tabs in @n on success, 0 on failure.
 */
Ewl_Position ewl_notebook_get_tabs_position(Ewl_Notebook * n)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, 0);

	w = EWL_WIDGET(n);

	DRETURN_INT(n->tabs_position, DLEVEL_TESTING);
}

/**
 * ewl_notebook_set_flags - set the flags for a notebook
 * @n: the notebook to change flags
 * @flags: the new flags for the notebook @n
 *
 * Returns no value. Sets the flags for the notebook @n to @flags.
 */
void ewl_notebook_set_flags(Ewl_Notebook * n, Ewl_Notebook_Flags flags)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	n->flags = flags;

	if (n->flags & EWL_NOTEBOOK_FLAG_TABS_HIDDEN)
		ewl_widget_hide(n->tab_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_init - initialize a notebook to default values and callbacks
 * @n: the notebook to initialize
 *
 * Returns no value. Sets the fields and callbacks of @n to their defaults.
 */
void ewl_notebook_init(Ewl_Notebook * n)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	/*
	 * Initialize the container portion of the notebook and set the fill
	 * policy to fill the area available.
	 */
	ewl_container_init(EWL_CONTAINER(w), "/notebook", NULL, NULL);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);

	/*
	 * Create the box to hold tabs and make the box fill the area of the
	 * notebook.
	 */
	n->tab_box = ewl_hbox_new();
	ewl_widget_set_appearance(n->tab_box, "/notebook/tab_box");
	ewl_object_set_fill_policy(EWL_OBJECT(n->tab_box),
				   EWL_FILL_POLICY_FILL);

	/*
	 * Attach the necessary callbacks for the notebook
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_notebook_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_notebook_configure,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, __ewl_notebook_destroy,
			     NULL);
	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
			 __ewl_container_configure_clip_box);

	/*
	 * Create a list to keep track of the pages in the notebook and set
	 * the default position of the tabs.
	 */
	n->pages = ewd_list_new();
	n->tabs_position = EWL_POSITION_TOP;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_notebook_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	if (n->flags != EWL_NOTEBOOK_FLAG_TABS_HIDDEN) {
		ewl_container_append_child(EWL_CONTAINER(w), n->tab_box);
		ewl_widget_realize(n->tab_box);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_notebook_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Alignment   a;
	Ewl_Notebook   *n;
	Ewl_Window     *win;
	int             ww = 0, hh = 0;
	int             rx = 0, ry = 0, rw = 0, rh = 0;
	int             ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);
	win = ewl_window_find_window_by_widget(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	if (EWL_CONTAINER(w)->clip_box) {
		evas_object_move(EWL_CONTAINER(w)->clip_box,
			  CURRENT_X(w), CURRENT_Y(w));
		evas_object_resize(EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w), CURRENT_H(w));
	}

	if (n->flags != EWL_NOTEBOOK_FLAG_TABS_HIDDEN) {
		if (!n->tab_box || !EWL_CONTAINER(n->tab_box)->children ||
		    ewd_list_is_empty(EWL_CONTAINER(n->tab_box)->children))
			DRETURN(DLEVEL_STABLE);

		ewl_object_get_maximum_size(EWL_OBJECT(n->tab_box), &ww, &hh);

		a = ewl_object_get_alignment(EWL_OBJECT(n->tab_box));

		switch (n->tabs_position) {
		case EWL_POSITION_LEFT:
			switch (a) {
			case EWL_ALIGNMENT_CENTER:
				rx = CURRENT_X(w);
				ry = CURRENT_Y(w);
				ry += CURRENT_H(w) / 2;
				ry -= hh / 2;
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_BOTTOM:
				rx = CURRENT_X(w);
				ry = CURRENT_Y(w) + CURRENT_H(w) - hh;
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_TOP:
			default:
				rx = CURRENT_X(w);
				ry = CURRENT_Y(w);
				rw = ww;
				rh = hh;
				break;
			}
			break;
		case EWL_POSITION_RIGHT:
			switch (a) {
			case EWL_ALIGNMENT_CENTER:
				rx = CURRENT_X(w) + CURRENT_W(w) - ww;
				ry = CURRENT_Y(w);
				ry += CURRENT_H(w) / 2;
				ry -= hh / 2;
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_BOTTOM:
				rx = CURRENT_X(w) + CURRENT_W(w) - ww;
				ry = CURRENT_Y(w) + CURRENT_H(w) - hh;
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_TOP:
			default:
				rx = CURRENT_X(w) + CURRENT_W(w) - ww;
				ry = CURRENT_Y(w);
				rw = ww;
				rh = hh;
				break;
			}
			break;
		case EWL_POSITION_TOP:
			switch (a) {
			case EWL_ALIGNMENT_CENTER:
				rx = CURRENT_X(w);
				rx += CURRENT_W(w) / 2;
				rx -= ww / 2;
				ry = CURRENT_Y(w);
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_RIGHT:
				rx = CURRENT_X(w) + CURRENT_W(w) - ww;
				ry = CURRENT_Y(w);
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_LEFT:
			default:
				rx = CURRENT_X(w);
				ry = CURRENT_Y(w);
				rw = ww;
				rh = hh;
				break;

			}
			break;
		case EWL_POSITION_BOTTOM:
			switch (a) {
			case EWL_ALIGNMENT_CENTER:
				rx = CURRENT_X(w);
				rx += CURRENT_W(w) / 2;
				rx -= ww / 2;
				ry = CURRENT_Y(w) + CURRENT_H(w);
				ry -= hh;
				rw = ww;
				rh = CURRENT_H(n->tab_box);
				break;
			case EWL_ALIGNMENT_RIGHT:
				rx = CURRENT_X(w) + CURRENT_W(w) - ww;
				ry = CURRENT_Y(w) + CURRENT_H(w);
				ry -= hh;
				rw = ww;
				rh = hh;
				break;
			case EWL_ALIGNMENT_LEFT:
			default:
				rx = CURRENT_X(w);
				ry = CURRENT_Y(w) + CURRENT_H(w);
				ry -= hh;
				rw = ww;
				rh = hh;
				break;

			}
			break;
		default:
			break;

		}

		ewl_object_request_geometry(EWL_OBJECT(n->tab_box), rx, ry, rw, rh);
		ewl_widget_configure(n->tab_box);
	}

	if (w->ebits_object) {
		switch (n->tabs_position) {
		case EWL_POSITION_LEFT:
			ebits_move(w->ebits_object,
				   CURRENT_X(w) + rw - ll, CURRENT_Y(w));
			ebits_resize(w->ebits_object,
				     CURRENT_W(w) - rw + ll, CURRENT_H(w));
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + rw,
						    CURRENT_Y(w) + tt,
						    CURRENT_W(w) - rw -
						    rr, CURRENT_H(w) - tt - bb);
			break;
		case EWL_POSITION_RIGHT:
			ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
			ebits_resize(w->ebits_object,
				     CURRENT_W(w) - rw + rr, CURRENT_H(w));
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + ll,
						    CURRENT_Y(w) + rr,
						    CURRENT_W(w) - rw -
						    ll, CURRENT_H(w) - tt - bb);
			break;
		case EWL_POSITION_BOTTOM:
			ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
			ebits_resize(w->ebits_object, CURRENT_W(w),
				     CURRENT_H(w) - rh + bb);
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + ll,
						    CURRENT_Y(w) + tt,
						    CURRENT_W(w) - ll -
						    rr, CURRENT_H(w) - rh - tt);
			break;
		case EWL_POSITION_TOP:
		default:
			ebits_move(w->ebits_object, CURRENT_X(w),
				   CURRENT_Y(w) + rh - tt);
			ebits_resize(w->ebits_object, CURRENT_W(w),
				     CURRENT_H(w) - rh + tt);
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + ll,
						    CURRENT_Y(w) + rh,
						    CURRENT_W(w) - ll -
						    rr, CURRENT_H(w) - rh - bb);
			break;

		}
	}

	ewl_widget_configure(n->visible_np->page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_notebook_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	while ((np = ewd_list_remove_last(n->pages)) != NULL)
		FREE(np);

	ewd_list_destroy(n->pages);

	n->visible_np = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_NotebookPage *ewl_notebook_page_new(Ewl_Widget * t, Ewl_Widget * p)
{
	Ewl_NotebookPage *np;

	np = NEW(Ewl_NotebookPage, 1);
	ZERO(np, Ewl_NotebookPage, 1);

	np->tab = t;
	np->page = p;

	DRETURN_PTR(np, DLEVEL_TESTING);
}

Ewl_NotebookPage *__ewl_notebook_page_create(Ewl_Notebook * n, Ewl_Widget * c,
					     Ewl_Widget * l)
{
	Ewl_NotebookPage *np;
	Ewl_Widget     *box, *button;
	char            file[PATH_LEN];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);
	DCHECK_PARAM_PTR_RET("c", c, NULL);
	DCHECK_PARAM_PTR_RET("l", l, NULL);

	switch (n->tabs_position) {
		case EWL_POSITION_LEFT:
			snprintf(file, PATH_LEN, "/notebook/tab_button/left");
			break;
		case EWL_POSITION_RIGHT:
			snprintf(file, PATH_LEN, "/notebook/tab_button/right");
			break;
		case EWL_POSITION_BOTTOM:
			snprintf(file, PATH_LEN, "/notebook/tab_button/bottom");
			break;
		case EWL_POSITION_TOP:
		default:
			snprintf(file, PATH_LEN, "/notebook/tab_button/top");
			break;
	}

	box = ewl_vbox_new();
	ewl_widget_set_appearance(box, "/notebook/content_box");
	ewl_container_append_child(EWL_CONTAINER(box), c);
	ewl_object_request_position(EWL_OBJECT(box), 1 << 15, 1 << 15);
	ewl_widget_configure(box);

	button = ewl_button_new(NULL);
	ewl_widget_set_appearance(button, file);
	ewl_widget_show(button);

	ewl_container_append_child(EWL_CONTAINER(button), l);

	np = ewl_notebook_page_new(button, box);

	DRETURN_PTR(np, DLEVEL_TESTING);
}
