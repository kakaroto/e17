#include <Ewl.h>

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
	if (!n)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_notebook_init(n);

	DRETURN_PTR(EWL_WIDGET(n), DLEVEL_TESTING);
}

/**
 * ewl_notebook_init - initialize a notebook to default values and callbacks
 * @n: the notebook to initialize
 *
 * Returns no value. Sets the fields and callbacks of @n to their defaults.
 */
int ewl_notebook_init(Ewl_Notebook * n)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, FALSE);

	w = EWL_WIDGET(n);

	/*
	 * Initialize the container portion of the notebook and set the fill
	 * policy to fill the area available.
	 */
	if (!ewl_container_init(EWL_CONTAINER(w), "tnotebook",
				ewl_notebook_add_cb, ewl_notebook_resize_cb,
				ewl_notebook_add_cb))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

	/*
	 * Set the default position of the tabs.
	 */
	n->flags |= EWL_POSITION_TOP;

	/*
	 * Create the box to hold tabs and make the box fill the area of the
	 * notebook.
	 */
	n->tab_box = ewl_hbox_new();
	if (n->tab_box) {
		ewl_widget_set_internal(n->tab_box, TRUE);
		ewl_object_set_fill_policy(EWL_OBJECT(n->tab_box),
					   EWL_FLAG_FILL_NONE);
		ewl_object_set_alignment(EWL_OBJECT(n->tab_box),
					 EWL_FLAG_ALIGN_LEFT |
					 EWL_FLAG_ALIGN_TOP);
		ewl_container_append_child(EWL_CONTAINER(n), n->tab_box);
		ewl_widget_show(n->tab_box);
	}
	else
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Attach the necessary callbacks for the notebook
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_notebook_configure_top_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
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
	Ewl_Widget     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);

	b = ewl_button_new(NULL);
	if (t)
		ewl_container_append_child(EWL_CONTAINER(b), t);
	ewl_callback_append(b, EWL_CALLBACK_CLICKED, ewl_notebook_tab_click_cb,
			    p);
	ewl_widget_show(b);

	ewl_container_append_child(EWL_CONTAINER(n->tab_box), b);
	ewl_container_append_child(EWL_CONTAINER(w), p);
	ewl_widget_set_data(p, n, b);

	if (!n->visible_page)
		n->visible_page = p;

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
	Ewl_Widget     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);

	b = ewl_button_new(NULL);
	if (t)
		ewl_container_append_child(EWL_CONTAINER(b), t);
	ewl_callback_append(b, EWL_CALLBACK_CLICKED, ewl_notebook_tab_click_cb,
			    p);
	ewl_widget_show(b);

	ewl_container_prepend_child(EWL_CONTAINER(n->tab_box), b);
	ewl_container_prepend_child(EWL_CONTAINER(w), p);
	ewl_widget_set_data(p, n, b);

	if (!n->visible_page)
		n->visible_page = p;

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
	Ewl_Widget     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(n);

	b = ewl_button_new(NULL);
	if (t)
		ewl_container_append_child(EWL_CONTAINER(b), t);
	ewl_callback_append(b, EWL_CALLBACK_CLICKED, ewl_notebook_tab_click_cb,
			    p);
	ewl_widget_show(b);

	ewl_container_insert_child(EWL_CONTAINER(n->tab_box), b, pos);
	ewl_container_insert_child(EWL_CONTAINER(w), p, pos);
	ewl_widget_set_data(p, n, b);

	if (!n->visible_page)
		n->visible_page = p;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_first_page - remove the first page from the notebook
 * @n: the notebook to remove the first page
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
void ewl_notebook_remove_first_page(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_Widget     *page = NULL;
	Ewl_Widget     *tab;
	Ewl_Container  *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);
	c = EWL_CONTAINER(n);

	if (!ewd_list_nodes(c->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);
	while ((page = ewd_list_next(c->children)) && (page == n->tab_box));

	if (page) {
		ewl_container_remove_child(c, page);
		tab = ewl_widget_get_data(page, n);
		if (tab)
			ewl_widget_destroy(tab);
		if (page == n->visible_page) {
			n->visible_page = NULL;
			ewl_notebook_set_visible_page(n, 0);
		}
		ewl_widget_destroy(page);
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_first_page - remove the last page from the notebook
 * @n: the notebook to remove the last page
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
void ewl_notebook_remove_last_page(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_Widget     *page = NULL;
	Ewl_Widget     *last = NULL;
	Ewl_Widget     *tab;
	Ewl_Container  *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);
	c = EWL_CONTAINER(n);

	if (!ewd_list_nodes(c->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);
	while ((last = ewd_list_next(c->children))) {
		if (page != n->tab_box) {
			page = last;
		}
	}

	if (page) {
		ewl_container_remove_child(c, page);
		tab = ewl_widget_get_data(page, n);
		if (tab)
			ewl_widget_destroy(tab);
		if (page == n->visible_page) {
			n->visible_page = NULL;
			ewl_notebook_set_visible_page(n, 0);
		}
		ewl_widget_destroy(page);
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_page - remove the specified page from the notebook
 * @n: the notebook to remove the specified page
 * @i: the position in the list of pages to remove from @n
 *
 * Returns a pointer to the removed page on success, NULL on failure.
 */
void ewl_notebook_remove_page(Ewl_Notebook * n, int i)
{
	int j = 1;
	Ewl_Widget     *w;
	Ewl_Widget     *page = NULL;
	Ewl_Widget     *tab;
	Ewl_Container  *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);
	c = EWL_CONTAINER(n);

	if (!ewd_list_nodes(c->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);
	while ((page = ewd_list_next(c->children)) && j < i) {
		if (page != n->tab_box)
			j++;
	}

	if (page) {
		ewl_container_remove_child(c, page);
		tab = ewl_widget_get_data(page, n);
		if (tab)
			ewl_widget_destroy(tab);
		if (page == n->visible_page) {
			n->visible_page = NULL;
			ewl_notebook_set_visible_page(n, 0);
		}
		ewl_widget_destroy(page);
	}

	DRETURN(DLEVEL_STABLE);
}

/**
 * ewl_notebook_remove_visible - remove the visible page from the notebook
 * @n: the notebook to remove the visible page
 *
 * Returns no value.
 */
void ewl_notebook_remove_visible(Ewl_Notebook * n)
{
	Ewl_Widget     *w;
	Ewl_Widget     *page = NULL;
	Ewl_Widget     *tab;
	Ewl_Container  *c;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);
	c = EWL_CONTAINER(n);

	/*
	 * No visible page? What the hell are we doing here then?!?
	 */
	if (!n->visible_page)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Search out the page in the notebook, and the tab that goes with it.
	 */
	ewd_list_goto_first(c->children);
	while ((page = ewd_list_next(c->children)) && page != n->visible_page);

	/*
	 * We found a page, sweet, kick it to the curb!
	 */
	if (page) {

		/*
		 * Remove from the list of pages in this notebook, and set it
		 * to be freed.
		 */
		ewl_container_remove_child(c, page);
		tab = ewl_widget_get_data(page, n);
		if (tab)
			ewl_widget_destroy(tab);
		ewl_widget_destroy(page);

		/*
		 * Set a usable visible page.
		 */
		n->visible_page = NULL;
		ewl_notebook_set_visible_page(n, 0);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_notebook_set_tabs_alignment - set the alignment of a notebooks tabs
 * @n: the notebook to change tab alignment
 * @a: the new alignment for the tabs of @n
 *
 * Returns no value. Changes the alignment of the tabs on @n to @a, and
 * updates the display.
 */
void ewl_notebook_set_tabs_alignment(Ewl_Notebook * n, unsigned int a)
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
unsigned int ewl_notebook_get_tabs_alignment(Ewl_Notebook * n)
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
	char            file[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	if (n->flags & p)
		DRETURN(DLEVEL_STABLE);

	switch (n->flags & EWL_POSITION_MASK) {
		case EWL_POSITION_LEFT:
			ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
					 ewl_notebook_configure_left_cb);
			break;
		case EWL_POSITION_RIGHT:
			ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
					 ewl_notebook_configure_right_cb);
			break;
		case EWL_POSITION_BOTTOM:
			ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
					 ewl_notebook_configure_bottom_cb);
			break;
		case EWL_POSITION_TOP:
		default:
			ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
					 ewl_notebook_configure_bottom_cb);
			break;
	}

	n->flags = (n->flags & ~EWL_POSITION_MASK) | p;

	switch (n->flags & EWL_POSITION_MASK) {
		case EWL_POSITION_LEFT:
			snprintf(file, PATH_MAX, "lnotebook");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_VERTICAL);
			ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
					    ewl_notebook_configure_left_cb,
					    NULL);
			break;
		case EWL_POSITION_RIGHT:
			snprintf(file, PATH_MAX, "rnotebook");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_VERTICAL);
			ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
					    ewl_notebook_configure_right_cb,
					    NULL);
			break;
		case EWL_POSITION_BOTTOM:
			snprintf(file, PATH_MAX, "bnotebook");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_HORIZONTAL);
			ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
					    ewl_notebook_configure_bottom_cb,
					    NULL);
			break;
		case EWL_POSITION_TOP:
		default:
			snprintf(file, PATH_MAX, "tnotebook");
			ewl_box_set_orientation(EWL_BOX(n->tab_box),
					EWL_ORIENTATION_HORIZONTAL);
			ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
					    ewl_notebook_configure_top_cb,
					    NULL);
			break;
	}

	ewl_widget_set_appearance(w, file);
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

	DRETURN_INT(n->flags, DLEVEL_TESTING);
}

/**
 * ewl_notebook_set_flags - set the flags for a notebook
 * @n: the notebook to change flags
 * @flags: the new flags for the notebook @n
 *
 * Returns no value. Sets the flags for the notebook @n to @flags.
 */
void ewl_notebook_set_tabs_visible(Ewl_Notebook * n, int show)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	if (show) {
		n->flags &= EWL_POSITION_MASK;
		ewl_widget_show(n->tab_box);
	}
	else {
		n->flags |= EWL_NOTEBOOK_FLAG_TABS_HIDDEN;
		ewl_widget_hide(n->tab_box);
	}

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
void ewl_notebook_set_visible_page(Ewl_Notebook *n, int t)
{
	int i = 0;
	Ewl_Container *c;
	Ewl_Widget *child = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(n);

	if (!ewd_list_nodes(c->children) || t > ewd_list_nodes(c->children))
		DRETURN(DLEVEL_STABLE);

	i = 0;
	ewd_list_goto_first(c->children);
	while ((child = ewd_list_next(c->children))) {
		if (child != n->tab_box) {
			if (i == t)
				break;
			i++;
		}
	}

	if (!child || child == n->visible_page)
		DRETURN(DLEVEL_STABLE);

	if (n->visible_page)
		ewl_widget_hide(n->visible_page);
	n->visible_page = child;
	ewl_widget_show(child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param n: the notebook to retrieve the current visible page number
 * @return Returns the current page number on success.
 * @brief Retrieves the position of the current page in the notebook @a n.
 */
int ewl_notebook_get_visible_page(Ewl_Notebook *n)
{
	int i = 0;
	Ewl_Container *c;
	Ewl_Widget *child = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(n);

	ewd_list_goto_first(c->children);
	while (child != n->visible_page &&
			(child = ewd_list_next(c->children))) {
		if (child != n->tab_box)
			i++;
	}

	DRETURN_INT(i, DLEVEL_STABLE);
}

void
ewl_notebook_tab_click_cb(Ewl_Widget *widget, void *ev_data, void *user_data)
{
	Ewl_Widget *page;
	Ewl_Notebook *nb;

	page = EWL_WIDGET(user_data);

	/*
	 * We need to find the notebook containing the tabbox containing this
	 * widget.
	 */
	nb = EWL_NOTEBOOK(widget->parent->parent);

	if (nb->visible_page)
		ewl_widget_hide(nb->visible_page);
	nb->visible_page = page;
	ewl_widget_show(nb->visible_page);
}

void
ewl_notebook_configure_top_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;
	int x, y;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	width = CURRENT_W(n);
	height = CURRENT_H(n) / 2;

	ewl_object_request_size(EWL_OBJECT(n->tab_box), width, height);
	height = ewl_object_get_current_h(EWL_OBJECT(n->tab_box));

	ewl_object_place(EWL_OBJECT(n->tab_box), CURRENT_X(w), CURRENT_Y(w),
			 width, height);
	x = CURRENT_X(w);
	y = CURRENT_Y(w) + height;
	height = CURRENT_H(w) - height;

	if (n->visible_page)
		ewl_object_request_geometry(EWL_OBJECT(n->visible_page),
				x, y, width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_configure_bottom_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;
	int x, y;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	width = CURRENT_W(n);
	height = CURRENT_H(n) / 2;

	ewl_object_request_size(EWL_OBJECT(n->tab_box), width, height);
	height = ewl_object_get_current_h(EWL_OBJECT(n->tab_box));

	x = CURRENT_X(w);
	y = CURRENT_Y(w) + height;
	height = CURRENT_H(w) - height;

	ewl_object_place(EWL_OBJECT(n->tab_box), CURRENT_X(w),
			 CURRENT_Y(w) + CURRENT_H(w) - height, width, height);
	x = CURRENT_X(w);
	y = CURRENT_Y(w);
	height = CURRENT_H(w) - height;

	if (n->visible_page)
		ewl_object_request_geometry(EWL_OBJECT(n->visible_page),
				x, y, width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_configure_left_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;
	int x, y;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	width = CURRENT_W(n) / 2;
	height = CURRENT_H(n);

	ewl_object_request_size(EWL_OBJECT(n->tab_box), width, height);
	width = ewl_object_get_current_w(EWL_OBJECT(n->tab_box));

	ewl_object_place(EWL_OBJECT(n->tab_box), CURRENT_X(w), CURRENT_Y(w),
			 width, height);
	x = CURRENT_X(w) + width;
	y = CURRENT_Y(w);
	width = CURRENT_W(w) - width;

	if (n->visible_page)
		ewl_object_request_geometry(EWL_OBJECT(n->visible_page),
				x, y, width, height);
}

void
ewl_notebook_configure_right_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook   *n;
	int x, y;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	width = CURRENT_W(n) / 2;
	height = CURRENT_H(n);

	ewl_object_request_size(EWL_OBJECT(n->tab_box), width, height);
	width = ewl_object_get_current_w(EWL_OBJECT(n->tab_box));

	ewl_object_place(EWL_OBJECT(n->tab_box),
			 CURRENT_X(w) + CURRENT_W(w) - width, CURRENT_Y(w),
			 width, height);
	x = CURRENT_X(w);
	y = CURRENT_Y(w);
	width = CURRENT_W(w) - width;

	if (n->visible_page)
		ewl_object_request_geometry(EWL_OBJECT(n->visible_page),
				x, y, width, height);
}

void
ewl_notebook_add_cb(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = EWL_NOTEBOOK(c);

	/*
	 * Sizing depends on the placement of the tabs
	 */
	if (n->flags & EWL_POSITION_LEFT || n->flags & EWL_POSITION_RIGHT) {
		ewl_container_prefer_largest(EWL_CONTAINER(c),
				EWL_ORIENTATION_VERTICAL);
		if (w != n->tab_box) {
			ewl_container_prefer_largest(EWL_CONTAINER(c),
					EWL_ORIENTATION_HORIZONTAL);
		}

		if (VISIBLE(n->tab_box))
			ewl_object_set_preferred_w(EWL_OBJECT(c),
					PREFERRED_W(c) +
					ewl_object_get_preferred_w(
						EWL_OBJECT(n->tab_box)));
	}
	else {
		ewl_container_prefer_largest(EWL_CONTAINER(c),
				EWL_ORIENTATION_HORIZONTAL);
		if (w != n->tab_box) {
			ewl_container_prefer_largest(EWL_CONTAINER(c),
					EWL_ORIENTATION_VERTICAL);
		}

		if (VISIBLE(n->tab_box))
			ewl_object_set_preferred_h(EWL_OBJECT(c),
					PREFERRED_H(c) +
					ewl_object_get_preferred_w(
						EWL_OBJECT(n->tab_box)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
		Ewl_Orientation o)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = EWL_NOTEBOOK(c);

	/*
	 * Sizing depends on the placement of the tabs
	 */
	if (n->flags & EWL_POSITION_LEFT || n->flags & EWL_POSITION_RIGHT) {
		ewl_container_prefer_largest(EWL_CONTAINER(c),
				EWL_ORIENTATION_VERTICAL);
		if (w != n->tab_box) {
			ewl_container_prefer_largest(EWL_CONTAINER(c),
					EWL_ORIENTATION_HORIZONTAL);
		}

		if (VISIBLE(n->tab_box))
			ewl_object_set_preferred_w(EWL_OBJECT(c),
					PREFERRED_W(c) +
					ewl_object_get_preferred_w(
						EWL_OBJECT(n->tab_box)));
	}
	else {
		ewl_container_prefer_largest(EWL_CONTAINER(c),
				EWL_ORIENTATION_HORIZONTAL);
		if (w != n->tab_box) {
			ewl_container_prefer_largest(EWL_CONTAINER(c),
					EWL_ORIENTATION_VERTICAL);
		}

		if (VISIBLE(n->tab_box))
			ewl_object_set_preferred_h(EWL_OBJECT(c),
					PREFERRED_H(c) +
					ewl_object_get_preferred_w(
						EWL_OBJECT(n->tab_box)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
