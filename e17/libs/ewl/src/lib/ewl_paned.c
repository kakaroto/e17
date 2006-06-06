#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

typedef struct 
{
	int (*minimum_size)(Ewl_Object *o);
	int (*current_size)(Ewl_Object *o);
	int (*preferred_size)(Ewl_Object *o);
	int (*current_position)(Ewl_Object *o);

	void (*variable_request)(Ewl_Object *o, int size);
	void (*stable_request)(Ewl_Object *o, int size);
	void (*position_request)(Ewl_Object *o, int pos);
	void (*stable_position_request)(Ewl_Object *o, int pos);
} Ewl_Paned_Layout;

static Ewl_Paned_Layout *horizontal_layout = NULL;
static Ewl_Paned_Layout *vertical_layout = NULL;
static Ewl_Paned_Layout *layout = NULL;

static void ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev,
							void *data);
static void ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev,
							void *data);
static void ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev,
							void *data);

static void ewl_paned_grabbers_update(Ewl_Paned *p);
static void ewl_paned_layout_setup(void);
static int ewl_paned_widgets_resize(Ecore_List *list, int space);
static int ewl_paned_widgets_place(Ecore_List *from, Ecore_List *to,
			Ewl_Paned_Layout *layout, int (*size)(Ewl_Object *o), 
			int other);

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

	if (!ewl_paned_init(pane)) 
	{
		ewl_widget_destroy(EWL_WIDGET(pane));
		pane = NULL;
	}
	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with horizontal orientation */
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
		ewl_paned_orientation_set(EWL_PANED(pane), 
						EWL_ORIENTATION_VERTICAL);

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

	if (!horizontal_layout)
		ewl_paned_layout_setup();

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

	p->new_panes = ecore_list_new();

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
 * @brief Set the orientation of the paned widget
 */
void
ewl_paned_orientation_set(Ewl_Paned *p, Ewl_Orientation o)
{
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);

	if (p->orientation == o)
		DRETURN(DLEVEL_STABLE);

	p->orientation = o;

	/* loop over all the children and change the orientation of all
 	 * of the grabbers */
	ecore_dlist_goto_first(EWL_CONTAINER(p)->children);
	while ((child = ecore_dlist_next(EWL_CONTAINER(p)->children)))
	{
		if (ewl_widget_type_is(child, EWL_PANED_GRABBER_TYPE))
			ewl_paned_grabber_paned_orientation_set(
					EWL_PANED_GRABBER(child), o);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: The Ewl_Paned to get the orientation from
 * @return Returns the Ewl_Orientation currently set on the paned
 * @brief Get the orientation of the paned widget
 */
Ewl_Orientation
ewl_paned_orientation_get(Ewl_Paned *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("p", p, EWL_PANED_TYPE, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(p->orientation, DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
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

	/* if this is a grabber we can skipp it */
	if (ewl_widget_type_is(w, EWL_PANED_GRABBER_TYPE))
		DRETURN(DLEVEL_STABLE);

	o = ewl_paned_grabber_new();
	ewl_paned_grabber_paned_orientation_set(EWL_PANED_GRABBER(o),
				ewl_paned_orientation_get(EWL_PANED(c)));

	ecore_list_append(EWL_PANED(c)->new_panes, w);

	/* insert the grabber at the same position as the pane so the
	 * grabber ends up to the left of the pane in the children list */
	idx = ewl_container_child_index_get(c, w);
	ewl_container_child_insert(c, o, idx);
	ewl_paned_grabbers_update(EWL_PANED(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param idx: The index that the child was removed from
 * @return Returns no value
 * @brief The child remove callback
 */
void
ewl_paned_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx)
{
	Ewl_Widget *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* skip grabbers */
	if (ewl_widget_type_is(w, EWL_PANED_GRABBER_TYPE))
		DRETURN(DLEVEL_STABLE);

	/* our grabber is always to our left, since we were just removed
	 * from idx that means it's at idx - 1 */
	o = ewl_container_child_internal_get(c, idx - 1);
	ewl_widget_destroy(o);

	/* if this is on the new pane list, remove it */
	if (ecore_list_goto(EWL_PANED(c)->new_panes, w))
		ecore_list_remove(EWL_PANED(c)->new_panes);

	ewl_paned_grabbers_update(EWL_PANED(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_paned_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	int cw, ch, ww, wh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
	ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);

	if (EWL_ORIENTATION_HORIZONTAL == EWL_PANED(c)->orientation)
	{
		cw += ww;
		if (wh > ch) ch = wh;
	}
	else
	{
		ch += wh;
		if (ww > cw) cw = ww;
	}

	ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);
	ewl_paned_grabbers_update(EWL_PANED(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child hide callback
 */
void
ewl_paned_cb_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
	int cw, ch, ww, wh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_object_preferred_size_get(EWL_OBJECT(w), &ww, &wh);
	ewl_object_preferred_inner_size_get(EWL_OBJECT(c), &cw, &ch);

	if (EWL_ORIENTATION_HORIZONTAL == EWL_PANED(c)->orientation)
		cw -= ww;
	else
		ch -= wh;

	ewl_object_preferred_inner_size_set(EWL_OBJECT(c), cw, ch);
	ewl_paned_grabbers_update(EWL_PANED(c));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_paned_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
					void *data __UNUSED__)
{
	Ewl_Paned *p;
	Ewl_Container *c;
	Ewl_Widget *child;
	Ecore_List *unsized, *sized;
	int grabber_size = 0, needed_pane_size = 0, min_pane_size = 0;
	int tot_paned_size = 0, cur_pane_size = 0;
	int available_size, pos = 0, other_size, main_dir, other_dir;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PANED(w);
	c = EWL_CONTAINER(p);

	unsized = ecore_list_new();
	sized = ecore_list_new();

	if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
	{
		layout = horizontal_layout;
		tot_paned_size = CURRENT_W(w);
		other_size = CURRENT_H(w);
		main_dir = CURRENT_X(w);
		other_dir = CURRENT_Y(w);
	}
	else
	{
		layout = vertical_layout;
		tot_paned_size = CURRENT_H(w);
		other_size = CURRENT_W(w);
		main_dir = CURRENT_Y(w);
		other_dir = CURRENT_X(w);
	}

	/* get a list of panes and grabbers that will be shown and the sizes
	 * we have to work with */
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		if (!VISIBLE(child)) continue;

		/* if the widget is a grabber. remove space from total and 
		 * sum up needed/current space for panes */
		if (ewl_widget_type_is(child, EWL_PANED_GRABBER_TYPE))
		{
			/* get the grabber size if needed */
			if (!grabber_size)
				grabber_size = 
					layout->current_size(EWL_OBJECT(child));

			tot_paned_size -= grabber_size;
		}
		else
		{
			/* if the child is on the new panes list it still
			 * needs to be placed, so take it's size into
			 * account */
			if (ecore_list_goto(p->new_panes, child) == child)
			{
				needed_pane_size += 
					layout->preferred_size(EWL_OBJECT(child));
				min_pane_size +=
					layout->minimum_size(EWL_OBJECT(child));

				/* remove the child from the new panes list
				 * and set it to the list of widgets to be
				 * sized */
				ecore_list_remove(p->new_panes);
				ecore_list_append(unsized, child);
			}
			else
			{
				cur_pane_size += 
					layout->current_size(EWL_OBJECT(child));
				ecore_list_append(sized, child);
			}
		}
	}

	available_size = tot_paned_size - cur_pane_size;

	/* the paned has gotten smaller, we need to reclaim space from the
	 * currently sized widgets */
	if (available_size < 0)
		available_size -= ewl_paned_widgets_resize(sized, available_size);

	/* available space is less then our needed space, we need to resize
	 * the other widgets until we have at least min_pane_size available */
	if (available_size < min_pane_size)
		available_size += ewl_paned_widgets_resize(sized, 
					min_pane_size - available_size);

	/* we have the minimum space, but not our preferred space. Give each
	 * unsized widget their minimum size .
	 */
	if ((min_pane_size <= available_size) 
			&& (available_size < needed_pane_size))
		available_size -= ewl_paned_widgets_place(unsized, sized,
						layout,
						layout->minimum_size,
						other_size);
	/* give each widget their preferred size */
	else
		available_size -= ewl_paned_widgets_place(unsized, sized,
						layout, layout->preferred_size, 
						other_size);

	/* now that the widgets have all been sized we need to distrubute
	 * any extra space available */
	if (available_size > 0)
	{
		int give = 0;
		int nodes = 0;

		nodes = ecore_list_nodes(sized);
		while((available_size > 0) && (nodes > 0))
		{
			give = floor(available_size / nodes);
			ecore_list_goto_first(sized);
			while ((child = ecore_list_current(sized)))
			{
				int size, new_size;

				size = layout->current_size(EWL_OBJECT(child));
				layout->variable_request(EWL_OBJECT(child), size + give);
				new_size = layout->current_size(EWL_OBJECT(child));

				if ((new_size - size) > 0)
				{
					available_size -= (new_size - size);
					ecore_list_next(sized);
				}
				else
					ecore_list_remove(sized);
			}
			nodes = ecore_list_nodes(sized);
		}
	}

	/* now that all of the space is filled we can go and layout all of
	 * the available widgets */
	pos = main_dir;
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		int ow, oh;

		if (VISIBLE(child))
		{
			ewl_object_current_size_get(EWL_OBJECT(child), &ow, &oh);

			if (ewl_paned_orientation_get(p) ==
					EWL_ORIENTATION_HORIZONTAL)
			{
				ewl_object_place(EWL_OBJECT(child), pos, other_dir, 
									ow, other_size);
				pos += ow;
			}
			else
			{
				ewl_object_place(EWL_OBJECT(child), other_dir, pos, 
									other_size, oh);
				pos += oh;
			}
		}
	}

	ecore_list_destroy(sized);
	ecore_list_destroy(unsized);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Look at all of the grabbers and show/hide them as needed */
static void
ewl_paned_grabbers_update(Ewl_Paned *p)
{
	Ewl_Widget *child, *g = NULL;
	Ewl_Container *c;
	int left = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PANED_TYPE);

	if (p->updating_grabbers)
		DRETURN(DLEVEL_STABLE);

	p->updating_grabbers = 1;

	c = EWL_CONTAINER(p);
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		/* internal means grabber */
		if (ewl_widget_type_is(child, EWL_PANED_GRABBER_TYPE))
		{
			/* if there is a widget to the left we store the
			 * grabber */
			if (left) g = child;
			ewl_widget_hide(child);
		}
		else if (VISIBLE(child))
		{
			left = 1;

			/* if we have a grabber to the left of us we need to
			 * show it as that means there is a widget to the
			 * left of us */
			if (g)
			{
				ewl_widget_show(g);
				g = NULL;
			}
		}
	}

	p->updating_grabbers = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Paned_Grabber stuff
 */

/**
 * @internal
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
 * @internal
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
	ewl_paned_grabber_paned_orientation_set(g, EWL_ORIENTATION_VERTICAL);

	ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_MOUSE_DOWN,
					ewl_paned_grabber_cb_mouse_down, NULL);
	ewl_callback_append(EWL_WIDGET(g), EWL_CALLBACK_MOUSE_UP,
					ewl_paned_grabber_cb_mouse_up, NULL);

	ewl_widget_internal_set(EWL_WIDGET(g), TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param g: The Ewl_Paned_Grabber to set the orientation on
 * @param o: The Ewl_Orientation to set on the grabber
 * @return Returns no value.
 */
void
ewl_paned_grabber_paned_orientation_set(Ewl_Paned_Grabber *g, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("g", g);
	DCHECK_TYPE("g", g, EWL_PANED_GRABBER_TYPE);

	if (o == EWL_ORIENTATION_HORIZONTAL)
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
 * @internal
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
 * @internal
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

static void
ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev __UNUSED__,
						void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_paned_grabber_cb_mouse_move, NULL);
	ewl_widget_state_set(w, "selected", EWL_STATE_PERSISTENT);

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
	ewl_widget_state_set(w, "default", EWL_STATE_PERSISTENT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev,
						void *data __UNUSED__)
{
	Ewl_Event_Mouse_Move *e;
	Ewl_Paned_Grabber *g, *stop_grabber = NULL;
	Ewl_Widget *shrink = NULL, *grow = NULL, *child;
	Ewl_Container *c;
	Ewl_Paned *p;
	int cur_pos, amt, block_pos, grab_size, pos, cur_size = 0;
	int new_size, left, min, shrink_size, move_pos;
	int pane_pos, pane_size, pane_position;
	void *(*give_to)(Ecore_DList *list);
	void *(*take_from)(Ecore_DList *list);
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = ev;
	g = EWL_PANED_GRABBER(w);
	p = EWL_PANED(w->parent);
	c = EWL_CONTAINER(p);

	if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
	{
		layout = horizontal_layout;
		move_pos = e->x;
		pane_position = CURRENT_X(p);
		pane_size = CURRENT_W(p);
	}
	else
	{
		layout = vertical_layout;
		move_pos = e->y;
		pane_position = CURRENT_Y(p);
		pane_size = CURRENT_H(p);
	}

	cur_pos = layout->current_position(EWL_OBJECT(w));

	/* shifting left */
	if ((cur_pos - move_pos) > 0)
	{
		give_to = ecore_dlist_next;
		take_from = ecore_dlist_previous;
		amt = cur_pos - move_pos;
		left = 1;
	}
	else if ((cur_pos - move_pos) < 0)
	{
		give_to = ecore_dlist_previous;
		take_from = ecore_dlist_next;
		amt = move_pos - cur_pos;
		left = 0;
	}
	else
		DRETURN(DLEVEL_STABLE);

	/* find the pane we are shifting into and the grabber
	 * blockign us */
	ecore_dlist_goto(c->children, w);

	/* move past the selected grabber */
	take_from(c->children);
	while ((child = take_from(c->children)))
	{
		if (!VISIBLE(child)) continue;

		if (ewl_widget_type_is(child, EWL_PANED_GRABBER_TYPE))
		{
			stop_grabber = EWL_PANED_GRABBER(child);
			break;
		}
		else
			shrink = child;
	}

	/* find the pane we are giving space too */
	ecore_dlist_goto(c->children, w);

	/* move past the selected grabber */
	give_to(c->children);
	while ((child = give_to(c->children)))
	{
		if (!VISIBLE(child)) continue;

		if (ewl_widget_type_is(child, EWL_PANED_GRABBER_TYPE))
			printf("ERROR found grabber instead of child?\n");
		else
		{
			grow = child;
			break;
		}
	}

	/* make sure the grabber dosen't move past the grabber before it or
	 * the edges of the paned */
	grab_size = layout->current_size(EWL_OBJECT(w));
	if (stop_grabber)
		block_pos = layout->current_position(EWL_OBJECT(stop_grabber));
	else if (left)
		block_pos = pane_position;
	else
		block_pos = pane_position + pane_size;

	min = layout->minimum_size(EWL_OBJECT(shrink));
	cur_size = layout->current_size(EWL_OBJECT(shrink));
	if (left)
	{
		if ((cur_pos - amt) < (block_pos + grab_size + min))
			pos = block_pos + grab_size + min;
		else
			pos = cur_pos - amt;

		amt = cur_pos - pos;

		/* don't have to shrink free space */
		if ((cur_pos - amt) < (block_pos + grab_size + cur_size))
			shrink_size = amt - (cur_pos - cur_size - block_pos - grab_size);
		else
			shrink_size = 0;

		/* move the right pane */
		pane_pos = layout->current_position(EWL_OBJECT(grow));
		layout->position_request(EWL_OBJECT(grow), pane_pos - amt);
	}
	else
	{
		if ((cur_pos + amt + grab_size + min) > block_pos)
			pos = block_pos - min - grab_size;
		else
			pos = cur_pos + amt;

		amt = pos - cur_pos;

		/* don't have to shrink free space */
		if ((cur_pos + amt + grab_size + cur_size) > block_pos)
			shrink_size = amt - (block_pos - cur_pos - cur_size);
		else
			shrink_size = 0;

		/* move the right pane */
		pane_pos = layout->current_position(EWL_OBJECT(shrink));
		layout->position_request(EWL_OBJECT(shrink), pane_pos + amt);
	}

	cur_size = layout->current_size(EWL_OBJECT(shrink));
	layout->variable_request(EWL_OBJECT(shrink), cur_size - shrink_size);
	new_size = layout->current_size(EWL_OBJECT(shrink));

	/* XXX didn't shirnk the entire amount do something ... */
	if ((cur_size - new_size) != shrink_size)
	{
		printf("ERROR, didn't shrink the entire amount ...?.\n");
	}

	cur_size = layout->current_size(EWL_OBJECT(grow));
	layout->variable_request(EWL_OBJECT(grow), cur_size + amt);
	new_size = layout->current_size(EWL_OBJECT(grow));

	/* place the grabber */
	layout->position_request(EWL_OBJECT(w), pos);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_layout_setup(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	horizontal_layout = NEW(Ewl_Paned_Layout, 1);
	if (!horizontal_layout) 
		DRETURN(DLEVEL_STABLE);
	
	horizontal_layout->minimum_size = ewl_object_minimum_w_get;
	horizontal_layout->current_size = ewl_object_current_w_get;
	horizontal_layout->preferred_size = ewl_object_preferred_w_get;
	horizontal_layout->current_position = ewl_object_current_x_get;
	horizontal_layout->variable_request = ewl_object_w_request;
	horizontal_layout->stable_request = ewl_object_h_request;
	horizontal_layout->position_request = ewl_object_x_request;
	horizontal_layout->stable_position_request = ewl_object_y_request;

	vertical_layout = NEW(Ewl_Paned_Layout, 1);
	if (!vertical_layout) 
		DRETURN(DLEVEL_STABLE);
	
	vertical_layout->minimum_size = ewl_object_minimum_h_get;
	vertical_layout->current_size = ewl_object_current_h_get;
	vertical_layout->preferred_size = ewl_object_preferred_h_get;
	vertical_layout->current_position = ewl_object_current_y_get;
	vertical_layout->variable_request = ewl_object_h_request;
	vertical_layout->stable_request = ewl_object_w_request;
	vertical_layout->position_request = ewl_object_y_request;
	vertical_layout->stable_position_request = ewl_object_x_request;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_paned_widgets_resize(Ecore_List *list, int space)
{
	int take, nodes = 0, moved = 0;
	Ecore_List *tmp;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("list", list, 0);

	/* make a temporary list */
	tmp = ecore_list_new();
	ecore_list_goto_first(list);
	while ((child = ecore_list_next(list)))
		ecore_list_append(tmp, child);

	space = abs(space);
	nodes = ecore_list_nodes(tmp);
	while ((space > 0) && (nodes > 0))
	{
		take = floor(space / nodes);

		ecore_list_goto_first(tmp);
		while ((child = ecore_list_current(tmp)))
		{
			int size, new_size;

			size = layout->current_size(EWL_OBJECT(child));
			layout->variable_request(EWL_OBJECT(child), size - take);
			new_size = layout->current_size(EWL_OBJECT(child));

			if ((size - new_size) > 0)
			{
				int dist;
				dist = size - new_size;
				moved += dist;
				space -= dist;
				ecore_list_next(tmp);
			}
			else
				ecore_list_remove(tmp);
		}
		nodes = ecore_list_nodes(tmp);
	}
	ecore_list_destroy(tmp);

	DRETURN_INT(moved, DLEVEL_STABLE);
}
		
static int
ewl_paned_widgets_place(Ecore_List *from, Ecore_List *to,
			Ewl_Paned_Layout *layout,
			int (*size)(Ewl_Object *o), int other)
{
	Ewl_Widget *child;
	int current, ret = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("from", from, 0);
	DCHECK_PARAM_PTR_RET("to", to, 0);
	DCHECK_PARAM_PTR_RET("size", size, 0);

	while ((child = ecore_list_remove_first(from)))
	{
		current = size(EWL_OBJECT(child));
		layout->variable_request(EWL_OBJECT(child), current);
		layout->stable_request(EWL_OBJECT(child), other);

		ret += current;
		ecore_list_append(to, child);
	}

	DRETURN_INT(ret, DLEVEL_STABLE);
}

