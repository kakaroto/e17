
#include <Ewl.h>


void __ewl_notebook_init(Ewl_Notebook * n);
void __ewl_notebook_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_notebook_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_notebook_destroy(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_notebook_tab_button_configure(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void __ewl_notebook_tab_mouse_down(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static Ewl_NotebookPage *ewl_notebook_page_new(Ewl_Widget * t,
					       Ewl_Widget * p);

Ewl_Widget *
ewl_notebook_new(void)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION;

	n = NEW(Ewl_Notebook, 1);
	memset(n, 0, sizeof(Ewl_Notebook));

	n->tab_box = ewl_hbox_new();
	ewl_widget_set_appearance(n->tab_box, "/appearance/notebook/tab_box");

	__ewl_notebook_init(n);

	DRETURN_PTR(EWL_WIDGET(n));
}

void
ewl_notebook_append_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;
	Ewl_Widget *box, *button;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	n = EWL_NOTEBOOK(w);

	box = ewl_vbox_new();
	ewl_widget_set_appearance(box, "/appearance/notebook/content_box");
	ewl_container_append_child(EWL_CONTAINER(w), box);
	ewl_container_append_child(EWL_CONTAINER(box), c);
	ewl_object_request_position(EWL_OBJECT(box), 1 << 30, 1 << 30);
	ewl_widget_configure(box);

	button = ewl_button_new(NULL);
	ewl_widget_set_appearance(button, "/appearance/notebook/tab_button");
	ewl_container_append_child(EWL_CONTAINER(n->tab_box), button);
	ewl_callback_append(button, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_tab_button_configure, NULL);
	ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_notebook_tab_mouse_down, w);
	ewl_widget_show(button);

	ewl_container_append_child(EWL_CONTAINER(button), l);

	np = ewl_notebook_page_new(button, box);

	ewd_list_append(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_notebook_prepend_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;
	Ewl_Widget *box, *button;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	n = EWL_NOTEBOOK(w);

	box = ewl_vbox_new();
	ewl_widget_set_appearance(box, "/appearance/notebook/content_box");
	ewl_container_prepend_child(EWL_CONTAINER(w), box);
	ewl_container_append_child(EWL_CONTAINER(box), c);
	ewl_object_request_position(EWL_OBJECT(box), 1 << 30, 1 << 30);
	ewl_widget_configure(box);

	button = ewl_button_new(NULL);
	ewl_widget_set_appearance(button, "/appearance/notebook/tab_button");
	ewl_container_prepend_child(EWL_CONTAINER(n->tab_box), button);
	ewl_callback_append(button, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_tab_button_configure, NULL);
	ewl_callback_append(button, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_notebook_tab_mouse_down, w);
	ewl_widget_show(button);

	ewl_container_append_child(EWL_CONTAINER(button), l);

	np = ewl_notebook_page_new(button, box);

	ewd_list_prepend(n->pages, np);

	if (!n->visible_np)
		n->visible_np = np;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_notebook_insert_page(Ewl_Widget * w, Ewl_Widget * c, Ewl_Widget * l,
			 int p)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("l", l);

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_init(Ewl_Notebook * n)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("n", n);

	w = EWL_WIDGET(n);

	ewl_container_init(EWL_CONTAINER(w), "/appearance/notebook/default");
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_notebook_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_notebook_configure, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			     __ewl_notebook_destroy, NULL);

	n->pages = ewd_list_new();

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	ewl_container_append_child(EWL_CONTAINER(w), n->tab_box);
	ewl_widget_realize(n->tab_box);

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;
	int xx, yy, ww, hh;
	int ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	ww = CURRENT_W(w);
	hh = 17;

	ewl_object_request_geometry(EWL_OBJECT(n->tab_box), xx, yy, ww, hh);
	ewl_widget_configure(n->tab_box);

	ll = 3;
	rr = 3;
	tt = 3;
	bb = 3;

	if (n->visible_np && n->visible_np->page)
	  {
		  if (n->visible_np->tab->ebits_object)
			  ebits_set_named_bit_state(n->visible_np->tab->
						    ebits_object, "Bottom",
						    "hilited");

		  ewl_object_request_geometry(EWL_OBJECT(n->visible_np->page),
					      xx + ll, yy + tt + hh,
					      ww - ll - rr,
					      CURRENT_H(w) - hh - tt - bb);
		  ewl_widget_configure(n->visible_np->page);
	  }

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(w);

	while ((np = ewd_list_next(n->pages)) != NULL)
		FREE(np);

	ewd_list_destroy(n->pages);

	n->visible_np = NULL;

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_tab_button_configure(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	Ewl_Container *c;
	Ewl_Widget *child;
	int ww = 0, ll = 0, rr = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	c = EWL_CONTAINER(w);

	ewd_list_goto_first(c->children);

	while ((child = ewd_list_next(c->children)) != NULL)
		ww += CURRENT_W(child);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, NULL, NULL);

	ewl_object_set_maximum_width(EWL_OBJECT(w), ww + ll + rr);

	DLEAVE_FUNCTION;
}

void
__ewl_notebook_tab_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Notebook *n;
	Ewl_NotebookPage *np;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	n = EWL_NOTEBOOK(user_data);

	if (n->visible_np && n->visible_np->tab == w)
		DRETURN;

	if (n->visible_np)
	  {
		  if (n->visible_np->tab == w)
			  DRETURN;

		  if (n->visible_np->tab->ebits_object)
			  ebits_set_named_bit_state(n->visible_np->tab->
						    ebits_object, "Bottom",
						    "normal");

		  ewl_widget_hide(n->visible_np->page);

		  ewl_object_request_position(EWL_OBJECT(n->visible_np->page),
					      1 << 30, 1 << 30);
		  ewl_widget_configure(n->visible_np->page);
	  }

	ewd_list_goto_first(n->pages);

	while ((np = ewd_list_next(n->pages)) != NULL)
	  {
		  if (np->tab == w)
		    {
			    ewl_widget_show(np->page);

			    n->visible_np = np;

			    break;
		    }
	  }

	ewl_widget_configure(EWL_WIDGET(n));
	ewl_widget_configure(EWL_WIDGET(n));

	DLEAVE_FUNCTION;
}

static Ewl_NotebookPage *
ewl_notebook_page_new(Ewl_Widget * t, Ewl_Widget * p)
{
	Ewl_NotebookPage *np;

	np = NEW(Ewl_NotebookPage, 1);
	memset(np, 0, sizeof(Ewl_NotebookPage));

	np->tab = t;
	np->page = p;

	DRETURN_PTR(np);
}
