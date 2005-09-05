#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * ewl_selectionbook_new - create a new selectionbook
 * 
 * Returns a newly allocated selectionbook on success, NULL on failure.
 */
Ewl_Widget     *ewl_selectionbook_new()
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Selectionbook, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_selectionbook_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}


/**
 * ewl_selectionbook_init - initialize a selectionbook widget
 * @param s: the selectionbook widget
 * 
 * Returns no value. Sets the fields and callbacks of the selectionbook
 * @a s to their default values.
 */
void ewl_selectionbook_init(Ewl_Selectionbook * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(s);


	/*
	 * Initialize the inherited box fields
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_widget_inherit(w, "selectionbook");

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_selectionbook_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    ewl_selectionbook_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY,
			    ewl_selectionbook_destroy_cb, NULL);


	s->tab_bar = ewl_selectionbar_new(w);

	s->pages = ecore_list_new();
	s->current_page = NULL;
	s->num_pages = 0;


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param s: The selection book to add the page too
 * @param tab: the tab name to give the page
 * @param page: The page to add
 * @return Returns no value.
 */
void
ewl_selectionbook_page_add(Ewl_Selectionbook * s, Ewl_Widget * tab,
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

	p->tab = tab;
	p->page = page;

	ewl_callback_append(p->tab, EWL_CALLBACK_CLICKED,
			    ewl_selectionbook_page_switch_cb, s);

	ewl_widget_data_set(tab, (void *) s, (void *) p);

	if (!s->current_page) {
		s->current_page = p;
		ewl_callback_del(p->tab, EWL_CALLBACK_CLICKED,
				 ewl_selectionbook_page_switch_cb);
		ewl_container_child_append(EWL_CONTAINER(s), page);
		ewl_widget_show(page);
	}

	ewl_container_child_append(EWL_CONTAINER(s->tab_bar), tab);

	ecore_list_append(s->pages, p);

	s->num_pages++;

	ewl_widget_configure(EWL_WIDGET(s));
	ewl_widget_configure(s->tab_bar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param s: The selection book to remove the page from
 * @param num: The page number to remove
 * @param destroy: Should the page be destroyed
 * @return Returns no value
 */
void ewl_selectionbook_page_rem(Ewl_Selectionbook * s, int num,
					int destroy __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	if (s->num_pages < num)
		return;

/*
	ecore_list_goto_index(s->pages, num);

	p = (Ewl_SbookPage *) ecore_list_remove(s->pages);

	if (s->current_page == p) {
		if (num == 1) {
			if (s->num_pages == 1)
				s->current_page = NULL;
			else {
				ecore_list_goto_index(s->pages, 2);
				s->current_page = ecore_list_next(s->pages);
			}
		} else {
			ecore_list_goto_first(s->pages);
			s->current_page = ecore_list_next(s->pages);
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

*/
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The selection book to remove the page from
 * @param destroy: Destroy the page after removal 
 * @return Returns no value.     
 * @brief Removes the current page from the selection book, destroying it if
 * desired                        
 */
void ewl_selectionbook_current_page_rem(Ewl_Selectionbook * s,
					int destroy __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

/*
	p = s->current_page;
	ecore_list_goto_first(s->pages);
	s->current_page = (Ewl_SbookPage *) ecore_list_current(s->pages);

	if (s->current_page == p)
		s->current_page =
		    (Ewl_SbookPage *) ecore_list_next(s->pages);


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

*/
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_selectionbook_realize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w);

//      ewl_container_remove_child(EWL_CONTAINER(s), s->tab_bar);
	ewl_widget_show(s->tab_bar);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_selectionbook_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Selectionbook *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w);

	printf("in selectionbook configure %p\n", w);

	if (s->current_page) {
		ewl_object_geometry_request(EWL_OBJECT(s->current_page->page),
					    CURRENT_X(EWL_OBJECT(s)),
					    CURRENT_Y(EWL_OBJECT(s)),
					    CURRENT_W(EWL_OBJECT(s)),
					    CURRENT_H(EWL_OBJECT(s)));
	}


	ewl_widget_configure(s->tab_bar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void
ewl_selectionbook_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Selectionbook *s;
	Ewl_SbookPage  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(w);

	if (s->pages) {
		ecore_list_goto_first(s->pages);
		while ((p = (Ewl_SbookPage *) ecore_list_next(s->pages)) != NULL)
			ewl_widget_destroy(p->page);
	}

	ewl_widget_destroy(s->tab_bar);
	ecore_list_destroy(s->pages);

	s->pages = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



void
ewl_selectionbook_page_switch_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
							void *user_data)
{
	Ewl_Selectionbook *s;
	Ewl_SbookPage  *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SELECTIONBOOK(user_data);
	p = s->current_page;

	if (s->current_page) {
		ewl_callback_append(p->tab,
				    EWL_CALLBACK_CLICKED,
				    ewl_selectionbook_page_switch_cb, s);
		ewl_widget_hide(p->page);
		ewl_container_child_remove(EWL_CONTAINER(s), p->page);
		//ewl_widget_hide(p->page);
	}

	s->current_page = (Ewl_SbookPage *) ewl_widget_data_get(w, (void *) s);

	p = s->current_page;
	ewl_callback_del(p->tab, EWL_CALLBACK_CLICKED,
			 ewl_selectionbook_page_switch_cb);
	ewl_container_child_append(EWL_CONTAINER(s), p->page);
	ewl_widget_show(p->page);

	ewl_widget_configure(EWL_WIDGET(s));


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
