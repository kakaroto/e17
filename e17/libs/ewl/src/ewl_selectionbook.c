
#include <Ewl.h>
#include "ewl_selectionbook.h"


void            __ewl_selectionbook_configure(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __ewl_selectionbook_realize(Ewl_Widget * w, void *ev_data,
					    void *user_data);
void            __ewl_selectionbook_destroy(Ewl_Widget * w, void *ev_data,
					    void *user_data);


void            __ewl_selectionbook_page_switch(Ewl_Widget * w, void *ev_data,
						void *user_data);

/**
 * ewl_selectionbook_new - create a new selectionbook
 * @window: the window the book will be added to
 * 
 * Returns a newly allocated selectionbook on success, NULL on failure.
 */
Ewl_Widget     *
ewl_selectionbook_new()
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Selectionbook, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(s, 0, sizeof(Ewl_Selectionbook));

	ewl_selectionbook_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}


/**
 * ewl_selectionbook_init - initialize a selectionbook widget
 * @s: the selectionbook widget
 * @window: the window the book will be added to
 * 
 * Returns no value. Sets the fields and callbacks of the selectionbook
 * @s to their default values.
 */
void
ewl_selectionbook_init(Ewl_Selectionbook * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(s);


	/*
	 * Initialize the inherited box fields
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);
	ewl_theme_data_set_str(w,
			       "/appearance/box/vertical/base/visible", "yes");

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_selectionbook_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_selectionbook_realize, NULL);


	s->tab_bar = ewl_selectionbar_new(w);
	ewl_container_append_child(EWL_CONTAINER(w), s->tab_bar);


	/*
	 * x,y,w,h are temporary variables here that will change
	 */
	s->panel = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(s->panel), EWL_FILL_POLICY_FILL);
	ewl_container_append_child(EWL_CONTAINER(w), s->panel);
	ewl_theme_data_set_str(EWL_WIDGET(s->panel),
			       "/appearance/box/vertical/base/visible", "yes");


	s->pages = ewd_list_new();
	s->num_pages = 0;



	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
ewl_selectionbook_add_page(Ewl_Selectionbook * s, Ewl_Widget * tab,
			   Ewl_Widget * page)
{
	Ewl_SbookPage  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_PARAM_PTR("tab", tab);
	DCHECK_PARAM_PTR("page", page);

	p = NEW(Ewl_SbookPage, 1);
	if (!p)
		return;

	memset(p, 0, sizeof(Ewl_SbookPage));

	p->tab = tab;
	p->page = page;

	ewl_callback_append(p->tab, EWL_CALLBACK_CLICKED,
			    __ewl_selectionbook_page_switch, p);

	if (!s->current_page) {
		s->current_page = p;
		ewl_callback_del(p->tab, EWL_CALLBACK_CLICKED,
				 __ewl_selectionbook_page_switch);
		ewl_container_append_child(EWL_CONTAINER(s->panel), page);
		ewl_widget_show(page);
	}

	ewl_container_append_child(EWL_CONTAINER(s->tab_bar), tab);
	ewl_widget_show(tab);

	ewd_list_append(s->pages, p);

	s->num_pages++;

	ewl_widget_configure(s->panel);
	ewl_widget_configure(s->tab_bar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_selectionbook_rem_page(Ewl_Selectionbook * s, int num, int destroy)
{
	Ewl_SbookPage  *p;;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	if (s->num_pages < num)
		return;

	ewd_list_goto_index(s->pages, num);

	p = (Ewl_SbookPage *) ewd_list_remove(s->pages);

	if (s->current_page == p) {
		if (num == 1) {
			if (s->num_pages == 1)
				s->current_page = NULL;
			else {
				ewd_list_goto_index(s->pages, 2);
				s->current_page = ewd_list_next(s->pages);
			}
		} else {
			ewd_list_goto_first(s->pages);
			s->current_page = ewd_list_next(s->pages);
		}
	}


	ewl_container_remove_child(EWL_CONTAINER(s->panel), p->page);
	ewl_container_remove_child(EWL_CONTAINER(s->tab_bar), p->tab);


	if (destroy) {
		ewl_widget_destroy(p->page);
		ewl_widget_destroy(p->tab);
	}

	p->page = NULL;
	p->tab = NULL;

	IF_FREE(p);

	s->num_pages--;

	ewl_widget_configure(EWL_WIDGET(s));


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_selectionbook_rem_current_page(Ewl_Selectionbook * s, int destroy)
{
	Ewl_SbookPage  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	p = s->current_page;
	ewd_list_goto_first(s->pages);
	s->current_page = (Ewl_SbookPage *) ewd_list_current(s->pages);

	if (s->current_page == p)
		s->current_page = (Ewl_SbookPage *) ewd_list_next(s->pages);


	ewl_container_remove_child(EWL_CONTAINER(s->panel), p->page);
	ewl_container_remove_child(EWL_CONTAINER(s->tab_bar), p->tab);

	if (destroy) {
		ewl_widget_destroy(p->page);
		ewl_widget_destroy(p->tab);
	}

	p->page = NULL;
	p->tab = NULL;

	IF_FREE(p);

	s->num_pages--;

	ewl_widget_configure(EWL_WIDGET(s));


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
__ewl_selectionbook_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w);

	ewl_widget_show(w);


	ewl_widget_show(s->tab_bar);
	ewl_widget_show(s->panel);



	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_selectionbook_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w);

	ewl_object_request_geometry(EWL_OBJECT(s->panel),
				    CURRENT_X(EWL_OBJECT(w)),
				    CURRENT_Y(EWL_OBJECT(w)),
				    CURRENT_W(EWL_OBJECT(w)),
				    CURRENT_H(EWL_OBJECT(w)));

	ewl_widget_configure(s->panel);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_selectionbook_page_switch(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Selectionbook *s;
	Ewl_SbookPage  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w->parent->parent);
	p = EWL_SBOOKPAGE(user_data);


	if (s->current_page) {
		ewl_callback_append(s->current_page->tab,
				    EWL_CALLBACK_CLICKED,
				    __ewl_selectionbook_page_switch,
				    s->current_page);
		ewl_container_remove_child(EWL_CONTAINER(s->panel),
					   s->current_page->page);
		ewl_widget_hide(s->current_page->page);
	}

	s->current_page = p;

	ewl_callback_del(p->tab, EWL_CALLBACK_CLICKED,
			 __ewl_selectionbook_page_switch);
	ewl_container_append_child(EWL_CONTAINER(s->panel), p->page);
	ewl_widget_realize(p->page);

	ewl_widget_configure(EWL_WIDGET(s->panel));
	ewl_widget_configure(EWL_WIDGET(s->tab_bar));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
