
#include <Ewl.h>


void __ewl_notebook_init(Ewl_Notebook * n);
void __ewl_notebook_realize(Ewl_Widget * w, void *ev_data,
			    void *user_data);
void __ewl_notebook_configure(Ewl_Widget * w, void *ev_data,
			      void *user_data);
void __ewl_notebook_destroy(Ewl_Widget * w, void *ev_data,
			    void *user_data);
void __ewl_notebook_tab_box_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void __ewl_notebook_tab_button_configure(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void __ewl_notebook_tab_button_theme_update(Ewl_Widget * w, void *ev_data,
					    void *user_data);

void __ewl_notebook_tab_mouse_down(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static Ewl_NotebookPage *ewl_notebook_page_new(Ewl_Widget * t,
					       Ewl_Widget * p);

void __ewl_container_configure(Ewl_Widget * w, void *event_data,
			       void *user_data);


Ewl_NotebookPage *__ewl_notebook_page_create(Ewl_Notebook * n,
					     Ewl_Widget * c,
					     Ewl_Widget * l);


Ewl_Widget *
ewl_notebook_new(void)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = NEW(Ewl_Notebook, 1);
	memset(n, 0, sizeof(Ewl_Notebook));

	n->tab_box = ewl_hbox_new();
	ewl_widget_set_appearance(n->tab_box,
				  "/appearance/notebook/tab_box");

	__ewl_notebook_init(n);

	DRETURN_PTR(EWL_WIDGET(n), DLEVEL_TESTING);
}

void
ewl_notebook_append_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	n = EWL_NOTEBOOK(w);
	np = __ewl_notebook_page_create(n, c, l);

	ewl_container_append_child(EWL_CONTAINER(w), np->page);
	ewl_container_append_child(EWL_CONTAINER(n->tab_box), np->tab);

	ewd_list_append(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_prepend_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	n = EWL_NOTEBOOK(w);
	np = __ewl_notebook_page_create(n, c, l);

	ewl_container_prepend_child(EWL_CONTAINER(w), np->page);
	ewl_container_prepend_child(EWL_CONTAINER(n->tab_box), np->tab);

	ewd_list_prepend(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_insert_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l,
			 int p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook_set_tabs_alignment(Ewl_Widget * w, Ewl_Alignment a)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	ewl_object_set_alignment(EWL_OBJECT(n->tab_box), a);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Alignment
ewl_notebook_get_tabs_alignment(Ewl_Widget * w)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	n = EWL_NOTEBOOK(w);

	DRETURN_INT(ewl_object_get_alignment(EWL_OBJECT(n->tab_box)),
		    DLEVEL_TESTING);
}

void
ewl_notebook_set_tabs_position(Ewl_Widget * w, Ewl_Position p)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;
	char file[1024];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	n->tabs_position = p;

	switch (n->tabs_position) {
	case EWL_POSITION_LEFT:
	case EWL_POSITION_RIGHT:
		ewl_box_set_orientation(n->tab_box,
					EWL_ORIENTATION_VERTICAL);
		break;
	case EWL_POSITION_TOP:
	case EWL_POSITION_BOTTOM:
		ewl_box_set_orientation(n->tab_box,
					EWL_ORIENTATION_HORIZONTAL);
		break;
	default:
		ewl_box_set_orientation(n->tab_box,
					EWL_ORIENTATION_HORIZONTAL);
		break;
	}

	switch (n->tabs_position) {
	case EWL_POSITION_LEFT:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-left.bits.db");
		break;
	case EWL_POSITION_RIGHT:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-right.bits.db");
		break;
	case EWL_POSITION_BOTTOM:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-bottom.bits.db");
		break;
	case EWL_POSITION_TOP:
	default:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-top.bits.db");
		break;
	}

	ewd_list_goto_first(n->pages);

	while ((np = ewd_list_next(n->pages)) != NULL)
		ewl_theme_data_set(np->tab,
				   "/appearance/notebook/tab_button/base",
				   file);

	ewl_widget_configure(w);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Position
ewl_notebook_get_tabs_position(Ewl_Widget * w)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, 0);

	n = EWL_NOTEBOOK(w);

	DRETURN_INT(n->tabs_position, DLEVEL_TESTING);
}

void
__ewl_notebook_init(Ewl_Notebook * n)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	ewl_container_init(EWL_CONTAINER(w),
			   "/appearance/notebook/default");
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_notebook_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_configure, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			     __ewl_notebook_destroy, NULL);

	ewl_callback_append(n->tab_box, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_tab_box_configure, NULL);

	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE,
			 __ewl_container_configure);

	n->pages = ewd_list_new();

	n->tabs_position = EWL_POSITION_TOP;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_notebook_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	ewl_container_append_child(EWL_CONTAINER(w), n->tab_box);
	ewl_widget_realize(n->tab_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_notebook_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Alignment a;
	Ewl_Notebook *n;
	int ww = 0, hh = 0;
	int rx = 0, ry = 0, rw = 0, rh = 0;
	int ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	if (EWL_CONTAINER(w)->clip_box) {
		evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
			  CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w), CURRENT_H(w));
	}

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

	ewl_object_request_geometry(EWL_OBJECT(n->tab_box), rx, ry, rw,
				    rh);
	ewl_widget_configure(n->tab_box);

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
						    rr,
						    CURRENT_H(w) - tt -
						    bb);
			break;
		case EWL_POSITION_RIGHT:
			ebits_move(w->ebits_object, CURRENT_X(w),
				   CURRENT_Y(w));
			ebits_resize(w->ebits_object,
				     CURRENT_W(w) - rw + rr, CURRENT_H(w));
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + ll,
						    CURRENT_Y(w) + rr,
						    CURRENT_W(w) - rw -
						    ll,
						    CURRENT_H(w) - tt -
						    bb);
			break;
		case EWL_POSITION_BOTTOM:
			ebits_move(w->ebits_object, CURRENT_X(w),
				   CURRENT_Y(w));
			ebits_resize(w->ebits_object, CURRENT_W(w),
				     CURRENT_H(w) - rh + bb);
			ewl_object_request_geometry(EWL_OBJECT
						    (n->visible_np->page),
						    CURRENT_X(w) + ll,
						    CURRENT_Y(w) + tt,
						    CURRENT_W(w) - ll -
						    rr,
						    CURRENT_H(w) - rh -
						    tt);
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
						    rr,
						    CURRENT_H(w) - rh -
						    bb);
			break;

		}
	}

	ewl_widget_configure(n->visible_np->page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_notebook_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;
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

void
__ewl_notebook_tab_box_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data)
{
	Ewl_Container *c;
	Ewl_Notebook *n;
	Ewl_Widget *child;
	int ww = 0, hh = 0;
	int mw = 0, mh = 0;
	int rw = 0, rh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);
	n = EWL_NOTEBOOK(w->parent);

	if (!c->children || ewd_list_is_empty(c->children) || !n)
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);

	while ((child = ewd_list_next(c->children)) != NULL) {
		ww += MAXIMUM_W(child);
		hh += MAXIMUM_H(child);

		if (MAXIMUM_W(child) > mw)
			mw = MAXIMUM_W(child);

		if (MAXIMUM_H(child) > mh)
			mh = MAXIMUM_H(child);
	}

	if (n->tabs_position == EWL_POSITION_LEFT ||
	    n->tabs_position == EWL_POSITION_RIGHT) {
		ewd_list_goto_first(c->children);

		while ((child = ewd_list_next(c->children)) != NULL) {
			ewl_object_set_custom_size(EWL_OBJECT(child), mw,
						   CURRENT_H(child));
			ewl_object_request_size(EWL_OBJECT(child), mw,
						CURRENT_H(child));
			ewl_widget_configure(child);
		}
	}

	switch (n->tabs_position) {
	case EWL_POSITION_LEFT:
	case EWL_POSITION_RIGHT:
		rw = mw;
		rh = hh;
		break;
	case EWL_POSITION_TOP:
	case EWL_POSITION_BOTTOM:
	default:
		rw = ww;
		rh = mh;
		break;
	}

	ewl_object_set_custom_size(EWL_OBJECT(w), rw, rh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_notebook_tab_button_configure(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;
	int ww = 0, hh = 0;
	int ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	if (!c->children || ewd_list_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(c->children);

	while ((child = ewd_list_next(c->children)) != NULL) {
		ww += CURRENT_W(child);

		if (CURRENT_H(child) > hh)
			hh = CURRENT_H(child);
	}

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	ewl_object_set_custom_size(EWL_OBJECT(w), ww + ll + rr,
				   hh + tt + bb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_notebook_tab_mouse_down(Ewl_Widget * w, void *ev_data,
			      void *user_data)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(user_data);

	if (n->visible_np && n->visible_np->tab == w)
		DRETURN(DLEVEL_STABLE);

	if (n->visible_np) {
		if (n->visible_np->tab == w)
			DRETURN(DLEVEL_STABLE);

		if (n->visible_np->tab->ebits_object)
			ebits_set_named_bit_state(n->visible_np->tab->
						  ebits_object, "Gate",
						  "normal");

		ewl_widget_hide(n->visible_np->page);

		ewl_object_request_position(EWL_OBJECT
					    (n->visible_np->page), 1 << 30,
					    1 << 30);
		ewl_widget_configure(n->visible_np->page);
	}

	ewd_list_goto_first(n->pages);

	while ((np = ewd_list_next(n->pages)) != NULL) {
		if (np->tab == w) {
			ewl_widget_show(np->page);

			n->visible_np = np;

			break;
		}
	}

	if (n->visible_np->tab->ebits_object)
		ebits_set_named_bit_state(n->visible_np->tab->ebits_object,
					  "Gate", "hilited");

	ewl_widget_configure(EWL_WIDGET(n));
	ewl_widget_configure(EWL_WIDGET(n));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_NotebookPage *
ewl_notebook_page_new(Ewl_Widget * t, Ewl_Widget * p)
{
	Ewl_NotebookPage *np;

	np = NEW(Ewl_NotebookPage, 1);
	memset(np, 0, sizeof(Ewl_NotebookPage));

	np->tab = t;
	np->page = p;

	DRETURN_PTR(np, DLEVEL_TESTING);
}

Ewl_NotebookPage *
__ewl_notebook_page_create(Ewl_Notebook * n, Ewl_Widget * c,
			   Ewl_Widget * l)
{
	Ewl_NotebookPage *np;
	Ewl_Widget *box, *button;
	char file[1024];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);
	DCHECK_PARAM_PTR_RET("c", c, NULL);
	DCHECK_PARAM_PTR_RET("l", l, NULL);

	switch (n->tabs_position) {
	case EWL_POSITION_LEFT:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-left.bits.db");
		break;
	case EWL_POSITION_RIGHT:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-right.bits.db");
		break;
	case EWL_POSITION_BOTTOM:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-bottom.bits.db");
		break;
	case EWL_POSITION_TOP:
	default:
		snprintf(file, 1024,
			 "/appearance/notebook/tab_button/base-top.bits.db");
		break;

	}

	box = ewl_vbox_new();
	ewl_widget_set_appearance(box, "/appearance/notebook/content_box");
	ewl_container_append_child(EWL_CONTAINER(box), c);
	ewl_object_request_position(EWL_OBJECT(box), 1 << 30, 1 << 30);
	ewl_widget_configure(box);

	button = ewl_button_new(NULL);
	ewl_widget_set_appearance(button,
				  "/appearance/notebook/tab_button");
	ewl_callback_append(button, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_tab_button_configure, NULL);
	ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_notebook_tab_mouse_down, n);
	ewl_callback_append(button, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_notebook_tab_button_theme_update, n);
	ewl_widget_show(button);

	ewl_container_append_child(EWL_CONTAINER(button), l);

	np = ewl_notebook_page_new(button, box);

	DRETURN_PTR(np, DLEVEL_TESTING);
}

void
__ewl_notebook_tab_button_theme_update(Ewl_Widget * w, void *ev_data,
				       void *user_data)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	n = user_data;

	if ((!n->visible_np || n->visible_np->tab != w) && w->ebits_object)
		ebits_set_named_bit_state(w->ebits_object, "Gate",
					  "normal");
	else if (w->ebits_object)
		ebits_set_named_bit_state(w->ebits_object, "Gate",
					  "hilited");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
