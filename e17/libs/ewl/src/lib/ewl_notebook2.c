#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

#define EWL_NOTEBOOK2_TAB "ewl_notebook2_tab"
#define EWL_NOTEBOOK2_PAGE "ewl_notebook2_page"

Ewl_Widget *
ewl_notebook2_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Notebook2, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_notebook2_init(EWL_NOTEBOOK2(w)))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

int
ewl_notebook2_init(Ewl_Notebook2 *n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, FALSE);

	if (!ewl_box_init(EWL_BOX(n)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(n), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(n), EWL_FLAG_FILL_ALL);

	ewl_widget_appearance_set(EWL_WIDGET(n), EWL_NOTEBOOK2_TYPE);
	ewl_widget_inherit(EWL_WIDGET(n), EWL_NOTEBOOK2_TYPE);

	n->tabbar_position = EWL_POSITION_TOP;

	n->body.tabbar = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(n), n->body.tabbar);
	ewl_object_fill_policy_set(EWL_OBJECT(n->body.tabbar), 
				EWL_FLAG_FILL_HSHRINK);
	ewl_object_alignment_set(EWL_OBJECT(n->body.tabbar), 
						EWL_FLAG_ALIGN_CENTER);
	ewl_widget_internal_set(n->body.tabbar, TRUE);
	ewl_widget_appearance_set(n->body.tabbar, "tabbar");
	ewl_widget_show(n->body.tabbar);

	n->body.pages = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(n), n->body.pages);
	ewl_widget_internal_set(n->body.pages, TRUE);
	ewl_widget_appearance_set(n->body.pages, "pages");
	ewl_widget_show(n->body.pages);

	ewl_callback_prepend(EWL_WIDGET(n), EWL_CALLBACK_DESTROY,
				ewl_notebook2_cb_destroy, NULL);

	ewl_container_redirect_set(EWL_CONTAINER(n), 
					EWL_CONTAINER(n->body.pages));

	ewl_container_show_notify_set(EWL_CONTAINER(n->body.pages), 
					ewl_notebook2_cb_child_show);
	ewl_container_hide_notify_set(EWL_CONTAINER(n->body.pages), 
					ewl_notebook2_cb_child_hide);
	ewl_container_add_notify_set(EWL_CONTAINER(n->body.pages), 
					ewl_notebook2_cb_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(n->body.pages), 
					ewl_notebook2_cb_child_remove);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_notebook2_tabbar_alignment_set(Ewl_Notebook2 *n, unsigned int align)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_TYPE("n", n, EWL_NOTEBOOK2_TYPE);

	ewl_object_alignment_set(EWL_OBJECT(n->body.tabbar), align);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_notebook2_tabbar_alignment_get(Ewl_Notebook2 *n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, 0);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, 0);

	DRETURN_INT(ewl_object_alignment_get(EWL_OBJECT(n->body.tabbar)),
							DLEVEL_STABLE);
}

void
ewl_notebook2_tabbar_position_set(Ewl_Notebook2 *n, Ewl_Position pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_TYPE("n", n, "notebook2");

	if (n->tabbar_position == pos)
		DRETURN(DLEVEL_STABLE);

	n->tabbar_position = pos;
	switch(pos)
	{
		case EWL_POSITION_LEFT:
		case EWL_POSITION_RIGHT:
			ewl_box_orientation_set(EWL_BOX(n),
						EWL_ORIENTATION_HORIZONTAL);
			ewl_box_orientation_set(EWL_BOX(n->body.tabbar),
						EWL_ORIENTATION_VERTICAL);
			break;

		case EWL_POSITION_TOP:
		case EWL_POSITION_BOTTOM:
		default:
			ewl_box_orientation_set(EWL_BOX(n),
						EWL_ORIENTATION_VERTICAL);
			ewl_box_orientation_set(EWL_BOX(n->body.tabbar),
						EWL_ORIENTATION_HORIZONTAL);
			break;
	}

	ewl_container_child_remove(EWL_CONTAINER(n), n->body.tabbar);

	/* remove the redirect so we can stick the tabbar back in */
	ewl_container_redirect_set(EWL_CONTAINER(n), NULL);

	if ((n->tabbar_position == EWL_POSITION_RIGHT)
			|| (n->tabbar_position == EWL_POSITION_BOTTOM))
		ewl_container_child_append(EWL_CONTAINER(n), n->body.tabbar);

	else if ((n->tabbar_position == EWL_POSITION_LEFT)
			|| (n->tabbar_position == EWL_POSITION_TOP))
		ewl_container_child_prepend(EWL_CONTAINER(n), n->body.tabbar);

	ewl_container_redirect_set(EWL_CONTAINER(n), 
					EWL_CONTAINER(n->body.pages));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Position
ewl_notebook2_tabbar_position_get(Ewl_Notebook2 *n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, EWL_POSITION_TOP);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, EWL_POSITION_TOP);

	DRETURN_INT(n->tabbar_position, DLEVEL_STABLE);
}

void
ewl_notebook2_tabbar_visible_set(Ewl_Notebook2 *n, unsigned int visible)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_TYPE("n", n, EWL_NOTEBOOK2_TYPE);

	if (visible) ewl_widget_show(n->body.tabbar);
	else ewl_widget_hide(n->body.tabbar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_notebook2_tabbar_visible_get(Ewl_Notebook2 *n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, FALSE);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, FALSE);

	DRETURN_INT(((VISIBLE(n->body.tabbar)) ? TRUE : FALSE), 
							DLEVEL_STABLE);
}

void
ewl_notebook2_visible_page_set(Ewl_Notebook2 *n, Ewl_Widget *page)
{
	Ewl_Widget *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_TYPE("n", n, EWL_NOTEBOOK2_TYPE);

	if (page == n->cur_page)
		DRETURN(DLEVEL_STABLE);

	/* XXX make these and attach */
	if (n->cur_page)
	{
		Ewl_Widget *w;

		t = ewl_widget_data_get(n->cur_page, EWL_NOTEBOOK2_TAB);
		if (t) ewl_widget_state_set(t, "default");

		/* make sure we set n->cur_page null first or the hide
		 * callback won't let us hide */
		w = n->cur_page;
		n->cur_page = NULL;
		ewl_widget_hide(w);
	}

	n->cur_page = page;
	ewl_widget_show(n->cur_page);

	t = ewl_widget_data_get(n->cur_page, EWL_NOTEBOOK2_TAB);
	if (t) ewl_widget_state_set(t, "selected");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Widget *
ewl_notebook2_visible_page_get(Ewl_Notebook2 *n)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, NULL);

	DRETURN_PTR(n->cur_page, DLEVEL_STABLE);
}

void
ewl_notebook2_page_tab_text_set(Ewl_Notebook2 *n, Ewl_Widget *page, 
							const char *text)
{
	Ewl_Widget *t = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("page", page);
	DCHECK_TYPE("n", n, EWL_NOTEBOOK2_TYPE);
	DCHECK_TYPE("page", page, EWL_WIDGET_TYPE);

	if (text)
	{
		t = ewl_label_new();
		ewl_label_text_set(EWL_LABEL(t), text);
		ewl_widget_show(t);
	}

	ewl_notebook2_page_tab_widget_set(n, page, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

const char *
ewl_notebook2_page_tab_text_get(Ewl_Notebook2 *n, Ewl_Widget *page)
{
	Ewl_Widget *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, EWL_NOTEBOOK2_TYPE);
	DCHECK_PARAM_PTR_RET("page", page, EWL_WIDGET_TYPE);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, NULL);
	DCHECK_TYPE_RET("page", page, EWL_WIDGET_TYPE, NULL);

	/* get the label widget */
	o = ewl_notebook2_page_tab_widget_get(n, page);

	DRETURN_PTR((o ? ewl_label_text_get(EWL_LABEL(o)) : NULL), DLEVEL_STABLE);
}

void
ewl_notebook2_page_tab_widget_set(Ewl_Notebook2 *n, Ewl_Widget *page,
							Ewl_Widget *tab)
{
	Ewl_Widget *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("n", n);
	DCHECK_PARAM_PTR("page", page);
	DCHECK_TYPE("n", n, EWL_NOTEBOOK2_TYPE);
	DCHECK_TYPE("page", page, EWL_WIDGET_TYPE);

	if (!tab)
	{
		tab = ewl_hbox_new();
		ewl_widget_show(tab);
	}

	/* XXX make this attach data */
	t = ewl_widget_data_get(page, EWL_NOTEBOOK2_TAB);
	if (!t)
	{
		t = ewl_hbox_new();
		ewl_widget_appearance_set(t, "tab");
		ewl_widget_data_set(page, EWL_NOTEBOOK2_TAB, t);
		ewl_widget_data_set(t, EWL_NOTEBOOK2_PAGE, page);
		ewl_widget_show(t);

		ewl_callback_append(t, EWL_CALLBACK_CLICKED,
					ewl_notebook2_cb_tab_clicked, n);

		/* XXX This has to insert into the position that the page is
		 * in, not onto the end */
		ewl_container_child_append(EWL_CONTAINER(n->body.tabbar), t);
	}

	/* if this is the current page set it's tab to selected */
	if (n->cur_page == page)
		ewl_widget_state_set(t, "selected");

	ewl_container_child_append(EWL_CONTAINER(t), tab);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Widget *
ewl_notebook2_page_tab_widget_get(Ewl_Notebook2 *n, Ewl_Widget *page)
{
	Ewl_Widget *t, *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("n", n, NULL);
	DCHECK_PARAM_PTR_RET("page", page, NULL);
	DCHECK_TYPE_RET("n", n, EWL_NOTEBOOK2_TYPE, NULL);
	DCHECK_TYPE_RET("page", page, EWL_WIDGET_TYPE, NULL);

	t = ewl_widget_data_get(page, EWL_NOTEBOOK2_TAB);
	if (!t)
	{
		DWARNING("We have a notebook page with no tab, bad, very bad.\n");
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	o = ewl_container_child_get(EWL_CONTAINER(n), 0);

	DRETURN_PTR(o, DLEVEL_STABLE);
}

void
ewl_notebook2_cb_destroy(Ewl_Widget *w, void *ev, void *data)
{
}

void
ewl_notebook2_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Notebook2 *n;
	int pw, ph;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	n = EWL_NOTEBOOK2(EWL_WIDGET(c)->parent);

	if (n->cur_page != w)
		ewl_widget_hide(w);

	ewl_object_preferred_size_get(EWL_OBJECT(n->cur_page), &pw, &ph);
	ewl_object_preferred_inner_size_set(EWL_OBJECT(n->body.pages), pw, ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook2_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Notebook2 *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	n = EWL_NOTEBOOK2(EWL_WIDGET(c)->parent);

	if (n->cur_page == w)
		ewl_widget_show(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook2_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Notebook2 *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	n = EWL_NOTEBOOK2(EWL_WIDGET(c)->parent);

	/* stick a null tab in there so that it at least shows up */
	ewl_notebook2_page_tab_widget_set(n, w, NULL);

	/* we have no current page, make it this one */
	if (!n->cur_page)
	{
		ewl_notebook2_visible_page_set(n, w);
		ewl_widget_show(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook2_cb_child_remove(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Widget *t;
	Ewl_Notebook2 *n;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* XXX nasty data_get */
	n = EWL_NOTEBOOK2(EWL_WIDGET(c)->parent);
	t = ewl_widget_data_get(w, EWL_NOTEBOOK2_TAB);
	if (t) ewl_container_child_remove(EWL_CONTAINER(n->body.tabbar), t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_notebook2_cb_tab_clicked(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Widget *page;
	Ewl_Notebook2 *n;

	DENTER_FUNCTION(DLEVEL_STABLE);

	n = data;

	/* XXX nast data_get */
	page = ewl_widget_data_get(w, EWL_NOTEBOOK2_PAGE);
	ewl_notebook2_visible_page_set(n, page);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


