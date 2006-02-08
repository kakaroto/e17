#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_paned_grabber_cb_mouse_in(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_paned_grabber_cb_mouse_out(Ewl_Widget *w, void *ev, 
							void *data);

static void ewl_paned_configure_horizontal(Ewl_Paned *p);
static void ewl_paned_configure_vertical(Ewl_Paned *p);

static void ewl_paned_grabber_horizontal_shift(Ewl_Paned *p, 
					Ewl_Widget *w, int to);
static void ewl_paned_grabber_vertical_shift(Ewl_Paned *p, 
					Ewl_Widget *w, int to);

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget
 */
Ewl_Widget *
ewl_paned_new(void) 
{
	Ewl_Paned *pane;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = NEW(Ewl_Paned, 1);
	if (!pane)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_paned_init(pane)) {
		ewl_widget_destroy(EWL_WIDGET(pane));
		pane = NULL;
	}

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with horizontal orientation
 */
Ewl_Widget *
ewl_hpaned_new(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_paned_new(), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with vertical orientation
 */
Ewl_Widget *
ewl_vpaned_new(void)
{
	Ewl_Widget *pane;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = ewl_paned_new();
	if (pane)
		EWL_PANED(pane)->orientation = EWL_ORIENTATION_VERTICAL;

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @param p: the paned widget to initialize
 * @return Returns 1 on success or 0 on failure
 * @brief Initialize a new paned widget to default values
 */
int
ewl_paned_init(Ewl_Paned *p)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	w = EWL_WIDGET(p);

	if (!ewl_container_init(EWL_CONTAINER(p))) 
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_PANED_TYPE);
	ewl_widget_inherit(w, EWL_PANED_TYPE);
	
	p->orientation = EWL_ORIENTATION_HORIZONTAL;

	ewl_container_add_notify_set(EWL_CONTAINER(p),
					ewl_paned_cb_child_add);
	ewl_container_remove_notify_set(EWL_CONTAINER(p),
					ewl_paned_cb_child_remove);
	ewl_container_show_notify_set(EWL_CONTAINER(p),
					ewl_paned_cb_child_show);
	ewl_container_hide_notify_set(EWL_CONTAINER(p),
					ewl_paned_cb_child_hide);
	ewl_container_resize_notify_set(EWL_CONTAINER(p),
					ewl_paned_cb_child_resize);

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				ewl_paned_cb_configure, NULL);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

	ewl_widget_focusable_set(w, FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: The paned to set the orientation on
 * @param o: The Ewl_Orientation to set
 * @return Returns no value.
 */
void 
ewl_paned_orientation_set(Ewl_Paned *p, Ewl_Orientation o)
{
	Ewl_Widget *child;
	Ewl_Orientation sep;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);

	if (p->orientation == o)
		DRETURN(DLEVEL_STABLE);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		sep = EWL_ORIENTATION_VERTICAL;
	else
		sep = EWL_ORIENTATION_HORIZONTAL;

	ecore_list_goto_first(EWL_CONTAINER(p)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(p)->children)))
	{
		/* Update each internal child to have the correct
		 * appearance/orientation. XXX This assumes that all 
		 * internal widgets will be grabbers ... */
		if (ewl_widget_internal_is(child))
			ewl_paned_grabber_orientation_set(
					EWL_PANED_GRABBER(child), sep);
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The Ewl_Paned to get the orientation from
 * @return Returns the Ewl_Orientation currently set on the paned
 */
Ewl_Orientation
ewl_paned_orientation_get(Ewl_Paned *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("p", p, EWL_PANED_TYPE, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(p->orientation, DLEVEL_STABLE);
}

void
ewl_paned_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Widget *o;
	int idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* if this is an internal widget then we skip it as I'm assuming
	 * that everything internal is a grabber. Or, if there is only one
	 * element in the container we don't need a grabber */
	if ((ewl_widget_internal_is(w)) 
			|| (ewl_container_child_count_get(c) == 1))
		DRETURN(DLEVEL_STABLE);

	/* create the required grabber */
	if (EWL_PANED(c)->orientation == EWL_ORIENTATION_HORIZONTAL)
		idx = EWL_ORIENTATION_VERTICAL;
	else
		idx = EWL_ORIENTATION_HORIZONTAL;

	o = ewl_paned_grabber_new();
	ewl_paned_grabber_orientation_set(EWL_PANED_GRABBER(o), idx);

	ewl_callback_append(o, EWL_CALLBACK_MOUSE_DOWN,
			ewl_paned_grabber_cb_mouse_down, c);
	ewl_callback_append(o, EWL_CALLBACK_MOUSE_UP,
			ewl_paned_grabber_cb_mouse_up, c);
	ewl_callback_append(o, EWL_CALLBACK_MOUSE_IN,
			ewl_paned_grabber_cb_mouse_in, c);
	ewl_callback_append(o, EWL_CALLBACK_MOUSE_OUT,
			ewl_paned_grabber_cb_mouse_out, c);

	idx = ewl_container_child_index_get(c, w);
	ewl_container_child_insert(c, o, idx);

	/* only show the grabber if the widget is visible */
	if (VISIBLE(w))
		ewl_widget_show(o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* The widget will already have been removed from the container. So,
 * we have to take a look at the first child and see if it's
 * internal, if so, remove it and done. Otherwise we have to walk the
 * children and look for 2 internals beside each other and remove
 * one of them. If that isnt' found and the last widget is internal
 * then remove that. */
void
ewl_paned_cb_child_remove(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Widget *child, *prev = NULL, *last = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* check first widget */
	child = ecore_list_goto_first(c->children);

	/* if there are no children remaining in the container we're done */
	if (!child) 
		DRETURN(DLEVEL_STABLE);

	/* try to find the node to remove */
	while ((child = ecore_list_current(c->children)))
	{
		if (ewl_widget_internal_is(child))
		{
			/* the widget before us was internal so we can
			 * remove one of them */
			if (prev)
			{
				ecore_list_remove(c->children);
				last = NULL;
				break;
			}
			else
				prev = child;
		}
		else
			prev = NULL;

		last = child;
		ecore_list_next(c->children);
	}

	/* the last widget was internal we need to remove it */
	if (last && ewl_widget_internal_is(last))
	{
		ecore_list_goto(c->children, last);
		ecore_list_remove(c->children);
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_paned_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size __UNUSED__, 
						Ewl_Orientation o __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_paned_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	int cw, ch, ww, wh;
	Ewl_Widget *cur, *prev = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
	ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);

	/* increase the containers perferred width by the preferred width of
	 * the shown widget (including the grabbers */
	if (EWL_PANED(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		cw += ww;
		if (wh > ch)
			ch = wh;
	}
	else {
		if (ww > cw)
			cw = ww;
		ch += wh;
	}

	ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);

	/* don't do anything for the internal widgets */
	if (ewl_widget_internal_is(w)) 
		DRETURN(DLEVEL_STABLE);

	/* show the grabber */
	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (cur == w)
		{
			/* prev was visible so we need to make the next
			 * visible */
			if (!prev || VISIBLE(prev))
			{
				prev = ecore_list_next(c->children);
				if (prev) ewl_widget_show(prev);
			}
			else if (prev)
				ewl_widget_show(prev);

			break;
		}
		prev = cur;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_paned_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	int cw, ch, ww, wh;
	Ewl_Widget *cur, *prev = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
	ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);

	/* decrease the containers perferred width by the preferred width of
	 * the hidden widget (including the grabbers */
	if (EWL_PANED(c)->orientation == EWL_ORIENTATION_HORIZONTAL)
		cw -= ww;
	else
		ch -= wh;

	ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);

	/* don't do anything for internal widgets */
	if (ewl_widget_internal_is(w)) 
		DRETURN(DLEVEL_STABLE);

	/* hide the grabber */
	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (cur == w)
		{
			/* prev was visible so we need to make the next
			 * visible */
			if (!prev || (!VISIBLE(prev)))
			{
				prev = ecore_list_next(c->children);
				if (prev)
					ewl_widget_hide(prev);
			}
			else if (prev)
				ewl_widget_hide(prev);
			break;
		}
		prev = cur;
	}
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* this will size widgets as needed depending on where their grabber is. The
 * widget can not be smaller then it's minimum size */
void
ewl_paned_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Paned *p;
	Ewl_Widget *cur;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PANED(w);

	/* the easy one */
	if (ecore_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	/* only one widget, give entire size */
	if (ecore_list_nodes(EWL_CONTAINER(w)->children) == 1)
	{
		cur = ecore_list_goto_first(EWL_CONTAINER(w)->children);
		ewl_object_place(EWL_OBJECT(cur), CURRENT_X(p), CURRENT_Y(p),
						CURRENT_W(p), CURRENT_H(p));
		DRETURN(DLEVEL_STABLE);
	}

	if (p->orientation == EWL_ORIENTATION_HORIZONTAL)
		ewl_paned_configure_horizontal(p);
	else
		ewl_paned_configure_vertical(p);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_configure_horizontal(Ewl_Paned *p)
{
	Ewl_Widget *cur, *prev = NULL;
	Ewl_Container *c;
	int cur_pos, size, new_size, pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);

	cur_pos = CURRENT_X(p);
	size = CURRENT_W(p);

	c = EWL_CONTAINER(p);

	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (!VISIBLE(cur)) continue;

		/* if we aren't internal then we're a pane, store us away
		 * and goto the next widget which _should_ be an internal
		 * grabber */
		if (!ewl_widget_internal_is(cur))
		{
			prev = cur;
			continue;
		}

		/* the grabber is before the start of the pane, so we're
		 * assuming it hasn't been placed yet */
		if ((CURRENT_X(cur) < CURRENT_X(p)))
		{
			int panes, pane_size, grabbers, grab_size, i;
			int pref_size = 0, min_size = 0, use_min = 0, extra = 0;
			Ewl_Widget *skip;

			/* the previous widget and this grabber */
			panes = 1;
			grabbers = 1;
			grab_size = CURRENT_W(cur);
			pref_size = ewl_object_preferred_w_get(EWL_OBJECT(prev));
			min_size = ewl_object_minimum_w_get(EWL_OBJECT(prev));
			pane_size = 0; 
			skip = cur;

			while ((skip = ecore_list_next(c->children)))
			{
				if (!VISIBLE(skip)) continue;
				if (!ewl_widget_internal_is(skip))
				{
					panes ++;
					pref_size += ewl_object_preferred_w_get(
								EWL_OBJECT(skip));
					min_size += ewl_object_minimum_w_get(
								EWL_OBJECT(skip));
					continue;
				}
				
				if ((CURRENT_X(skip) < CURRENT_X(p)))
				{
					grab_size += CURRENT_W(skip);
					grabbers ++;
					continue;
				}
				break;
			}

			/* we have a grabber thats been placed,
			 * calculate the size per pane */
			if (skip) pane_size = CURRENT_X(skip) - cur_pos;
			else pane_size = CURRENT_W(p) - cur_pos;

			pane_size -= grab_size;
			if (pane_size < pref_size)
			{
				extra = (pane_size - min_size) / panes;
				use_min = 1;
			}

			ecore_list_goto(c->children, prev);
			i = 0;
			while ((prev = ecore_list_next(c->children)))
			{
				if (ewl_widget_internal_is(prev))
				{
					ewl_object_place(EWL_OBJECT(prev),
							cur_pos, CURRENT_Y(p),
							CURRENT_W(prev), 
							CURRENT_H(p));

					cur_pos += CURRENT_W(prev);
				}
				else
				{
					int size = 0;

					if (use_min)
					{
						size = ewl_object_minimum_w_get(
								EWL_OBJECT(prev));
						size += extra;
					}
					else
						size = ewl_object_preferred_w_get(
								EWL_OBJECT(prev));

					ewl_object_place(EWL_OBJECT(prev),
							cur_pos, CURRENT_Y(p), 
							size, CURRENT_H(p));

					cur_pos += size;
				}

				i++;
				if (i == (panes + grabbers)) 
					break;
			}
			prev = NULL;
			continue;
		}

		pos = ewl_object_current_x_get(EWL_OBJECT(cur));
		new_size = pos - cur_pos;

		ewl_object_place(EWL_OBJECT(prev), cur_pos, CURRENT_Y(p),
						new_size, CURRENT_H(p));	
		cur_pos += new_size;

		/* place the grabber and move to its left side */
		ewl_object_place(EWL_OBJECT(cur), cur_pos, CURRENT_Y(p),
						CURRENT_W(cur), CURRENT_H(p));
		cur_pos += CURRENT_W(cur);
		prev = NULL;
	}

	/* if there is one left over, place it ... */
	if (prev)
	{
		int new_size;
		new_size = size - cur_pos;
		ewl_object_place(EWL_OBJECT(prev), cur_pos, CURRENT_Y(p), 
					new_size, CURRENT_H(p));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_configure_vertical(Ewl_Paned *p)
{
	Ewl_Widget *cur, *prev = NULL;
	Ewl_Container *c;
	int cur_pos, size, new_size, pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);

	cur_pos = CURRENT_Y(p);
	size = CURRENT_H(p);

	c = EWL_CONTAINER(p);

	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (!VISIBLE(cur)) continue;

		/* if we aren't internal then we're a pane, store us away
		 * and goto the next widget which _should_ be an internal
		 * grabber */
		if (!ewl_widget_internal_is(cur))
		{
			prev = cur;
			continue;
		}

		/* the grabber is before the start of the pane, so we're
		 * assuming it hasn't been placed yet */
		if ((CURRENT_Y(cur) < CURRENT_Y(p)))
		{
			int panes, pane_size, grabbers, grab_size, i;
			int pref_size = 0, min_size = 0, use_min = 0, extra = 0;
			Ewl_Widget *skip;

			/* the previous widget and this grabber */
			panes = 1;
			grabbers = 1;
			grab_size = CURRENT_H(cur);
			pref_size = ewl_object_preferred_h_get(EWL_OBJECT(prev));
			min_size = ewl_object_minimum_h_get(EWL_OBJECT(prev));
			pane_size = 0; 
			skip = cur;

			while ((skip = ecore_list_next(c->children)))
			{
				if (!VISIBLE(skip)) continue;
				if (!ewl_widget_internal_is(skip))
				{
					panes ++;
					continue;
				}
				
				if ((CURRENT_Y(skip) < CURRENT_Y(p)))
				{
					grab_size += CURRENT_H(skip);
					pref_size += ewl_object_preferred_h_get(
								EWL_OBJECT(skip));
					min_size += ewl_object_minimum_h_get(
								EWL_OBJECT(skip));
					grabbers ++;
					continue;
				}
				break;
			}

			/* we have a grabber thats been placed,
			 * calculate the size per pane */
			if (skip) pane_size = CURRENT_Y(skip) - cur_pos;
			else pane_size = CURRENT_H(p) - cur_pos;

			pane_size -= grab_size;
			if (pane_size < pref_size)
			{
				extra = (pane_size - min_size) / panes;
				use_min = 1;
			}

			ecore_list_goto(c->children, prev);
			i = 0;
			while ((prev = ecore_list_next(c->children)))
			{
				if (ewl_widget_internal_is(prev))
				{
					ewl_object_place(EWL_OBJECT(prev),
							CURRENT_X(p), cur_pos,
							CURRENT_W(p), 
							CURRENT_H(prev));

					cur_pos += CURRENT_H(prev);
				}
				else
				{
					int size = 0;

					if (use_min)
					{
						size = ewl_object_minimum_h_get(
								EWL_OBJECT(prev));
						size += extra;
					}
					else
						size = ewl_object_preferred_h_get(
								EWL_OBJECT(prev));

					ewl_object_place(EWL_OBJECT(prev),
							CURRENT_X(p), cur_pos,
							CURRENT_W(p), size);

					cur_pos += size;
				}

				i++;
				if (i == (panes + grabbers)) 
					break;
			}
			prev = NULL;
			continue;
		}

		pos = ewl_object_current_y_get(EWL_OBJECT(cur));
		new_size = pos - cur_pos;

		ewl_object_place(EWL_OBJECT(prev), CURRENT_X(p), cur_pos,
						CURRENT_H(p), new_size);
		cur_pos += new_size;

		/* place the grabber and move to its left side */
		ewl_object_place(EWL_OBJECT(cur), CURRENT_Y(p), cur_pos,
						CURRENT_W(p), CURRENT_H(cur));
		cur_pos += CURRENT_H(cur);
		prev = NULL;
	}

	/* if there is one left over, place it ... */
	if (prev)
	{
		int new_size;
		new_size = size - cur_pos;
		ewl_object_place(EWL_OBJECT(prev), CURRENT_Y(p), cur_pos,
					CURRENT_W(p), new_size);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev __UNUSED__, 
						void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_paned_grabber_cb_mouse_move, data);
	ewl_widget_state_set(w, "selected");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_paned_grabber_cb_mouse_move);
	ewl_widget_state_set(w, "default");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Paned *p;
	Ewl_Event_Mouse_Move *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = data;
	e = ev;

	if (p->orientation == EWL_ORIENTATION_HORIZONTAL)
	{
		if ((e->x > CURRENT_X(p)) 
				&& (e->x < (CURRENT_X(p) + CURRENT_W(p))))
			ewl_paned_grabber_horizontal_shift(p, w, e->x);
	}
	else
	{
		if ((e->y > CURRENT_Y(p)) 
				&& (e->y < (CURRENT_Y(p) + CURRENT_H(p))))
			ewl_paned_grabber_vertical_shift(p, w, e->y);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_in(Ewl_Widget *w, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* XXX change cursor */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_out(Ewl_Widget *w, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* XXX change cursor */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_horizontal_shift(Ewl_Paned *p, Ewl_Widget *w, int to)
{
	Ewl_Container *c;
	Ewl_Widget *before = NULL, *after = NULL, *cur;
	int bx, ax, found = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	bx = CURRENT_X(p);
	ax = CURRENT_X(p) + CURRENT_W(p);

	c = EWL_CONTAINER(p);

	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (!ewl_widget_internal_is(cur)) 
		{
			if (!found) before = cur;
			else after = cur;
		}
		else
		{
			/* is this hte grabber we want? */
			if (cur == w)
				found = 1;
			else
			{
				if (!found)
					bx = CURRENT_X(cur) +
						CURRENT_W(cur);
				else
				{
					ax = CURRENT_X(cur);
					break;
				}
			}
		}
	}

	/* if we don't have a prev/next pane we're fucked */
	if (!before || !after)
	{
		DWARNING("Grabber but no panes beside it...\n");
		DRETURN(DLEVEL_STABLE);
	}

	/* don't move to the left of the grabber to the left of us */
	if (to < (bx + CURRENT_W(before)))
		to = bx + CURRENT_W(before);

	/* dont' move to the right of the grabber to the right of us */
	if (to > ax)
		to = ax;

	/* XXX shoud check min widget sizes here */
	ewl_object_place(EWL_OBJECT(before), CURRENT_X(before), 
					CURRENT_Y(before), 
					to - (bx + CURRENT_W(before)), 
					CURRENT_H(before));

	ewl_object_place(EWL_OBJECT(after), to + CURRENT_W(w), 
					CURRENT_Y(after), 
					ax - (to + CURRENT_W(w)),
					CURRENT_H(after));

	ewl_object_x_request(EWL_OBJECT(w), to);
	ewl_widget_configure(EWL_WIDGET(p));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_vertical_shift(Ewl_Paned *p, Ewl_Widget *w, int to) 
{
	Ewl_Container *c;
	Ewl_Widget *before = NULL, *after = NULL, *cur;
	int by, ay, found = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	by = CURRENT_Y(p);
	ay = CURRENT_Y(p) + CURRENT_H(p);

	c = EWL_CONTAINER(p);

	ecore_list_goto_first(c->children);
	while ((cur = ecore_list_next(c->children)))
	{
		if (!ewl_widget_internal_is(cur)) 
		{
			if (!found) before = cur;
			else after = cur;
		}
		else
		{
			/* is this hte grabber we want? */
			if (cur == w)
				found = 1;
			else
			{
				if (!found)
					by = CURRENT_Y(cur) +
						CURRENT_H(cur);
				else
				{
					ay = CURRENT_Y(cur);
					break;
				}
			}
		}
	}

	/* if we don't have a prev/next pane we're fucked */
	if (!before || !after)
	{
		DWARNING("Grabber but no panes beside it...\n");
		DRETURN(DLEVEL_STABLE);
	}

	/* don't move above the grabber above us */
	if (to < (by + CURRENT_H(before)))
		to = by + CURRENT_H(before);

	/* dont' move to the right of the grabber to the right of us */
	if (to > ay)
		to = ay;

	/* XXX shoud check min widget sizes here */
	ewl_object_place(EWL_OBJECT(before), CURRENT_X(before), 
					CURRENT_Y(before), CURRENT_W(before), 
					to - (by + CURRENT_H(before)));

	ewl_object_place(EWL_OBJECT(after), CURRENT_X(after),
					to + CURRENT_H(w), 
					CURRENT_W(after),
					ay - (to + CURRENT_H(w)));

	ewl_object_y_request(EWL_OBJECT(w), to);
	ewl_widget_configure(EWL_WIDGET(p));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Paned_Grabber stuff
 */

/**
 * @return Returns a new Ewl_Paned_Grabber widget or NULL on failure
 */
Ewl_Widget *
ewl_paned_grabber_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Paned_Grabber, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_paned_grabber_init(EWL_PANED_GRABBER(w)))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param g: The Ewl_Paned_Grabber to initialize
 * @return Returns TRUE on success or FALSE on failure
 */
int
ewl_paned_grabber_init(Ewl_Paned_Grabber *g)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("g", g, FALSE);

	if (!ewl_separator_init(EWL_SEPARATOR(g)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_inherit(EWL_WIDGET(g), EWL_PANED_GRABBER_TYPE);
	ewl_paned_grabber_orientation_set(g, EWL_ORIENTATION_HORIZONTAL);

	/* grabber is always internal to the paned */
	ewl_widget_internal_set(EWL_WIDGET(g), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param g: The Ewl_Paned_Grabber to set the orientation on
 * @param o: The Ewl_Orientation to set on the grabber
 * @return Returns no value.
 */
void
ewl_paned_grabber_orientation_set(Ewl_Paned_Grabber *g, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_PANED_GRABBER_TYPE);

	if (o == EWL_ORIENTATION_VERTICAL)
	{
		ewl_separator_orientation_set(EWL_SEPARATOR(g), 
						EWL_ORIENTATION_VERTICAL);
		ewl_widget_appearance_set(EWL_WIDGET(g), "grabber/vertical");
		ewl_paned_grabber_show_cursor_for(g, 
					EWL_POSITION_LEFT | EWL_POSITION_RIGHT);
	}
	else
	{
		ewl_separator_orientation_set(EWL_SEPARATOR(g),
						EWL_ORIENTATION_HORIZONTAL);
		ewl_widget_appearance_set(EWL_WIDGET(g), "grabber/horizontal");
		ewl_paned_grabber_show_cursor_for(g,
					EWL_POSITION_TOP | EWL_POSITION_BOTTOM);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param g: The Ewl_Paned_Grabber to get the orientation from
 * @return Returns the Ewl_Orientation set on the grabber
 */
Ewl_Orientation 
ewl_paned_grabber_orientation_get(Ewl_Paned_Grabber *g)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("g", g, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("g", g, EWL_PANED_GRABBER_TYPE, 
					EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(ewl_separator_orientation_get(EWL_SEPARATOR(g)), 
						DLEVEL_STABLE);
}

/**
 * @param g: The Ewl_Paned_Grabber to set the cursor for
 * @param dir: The diretions to show arrows for
 * @return Returns no value.
 *
 * @brief This will show the arrows to allow the grabber to move in the
 * directions specified by @a dir.
 */
void
ewl_paned_grabber_show_cursor_for(Ewl_Paned_Grabber *g, unsigned int dir)
{
	int pos = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_PANED_GRABBER_TYPE);

	if ((dir & EWL_POSITION_LEFT) && (dir & EWL_POSITION_RIGHT))
		pos = EWL_MOUSE_CURSOR_SB_H_DOUBLE_ARROW;
	else if ((dir & EWL_POSITION_TOP) && (dir & EWL_POSITION_BOTTOM))
		pos = EWL_MOUSE_CURSOR_SB_V_DOUBLE_ARROW;
	else if (dir & EWL_POSITION_LEFT)
		pos = EWL_MOUSE_CURSOR_SB_LEFT_ARROW;
	else if (dir & EWL_POSITION_RIGHT)
		pos = EWL_MOUSE_CURSOR_SB_RIGHT_ARROW;
	else if (dir & EWL_POSITION_TOP)
		pos = EWL_MOUSE_CURSOR_SB_UP_ARROW;
	else
		pos = EWL_MOUSE_CURSOR_SB_DOWN_ARROW;

	ewl_attach_mouse_cursor_set(EWL_WIDGET(g), pos);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


