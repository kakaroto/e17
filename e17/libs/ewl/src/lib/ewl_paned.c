/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_paned.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @brief Contains information on a paned layout
 */
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

static void ewl_paned_cb_shutdown(void);

static void ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev,
							void *data);
static void ewl_paned_grabber_cb_mouse_up(Ewl_Widget *w, void *ev,
							void *data);
static void ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev,
							void *data);

static void ewl_paned_grabbers_update(Ewl_Paned *p);
static void ewl_paned_layout_setup(void);
static Ewl_Widget *ewl_paned_grabber_next(Ewl_Paned *p);
static int ewl_paned_visible_panes(Ewl_Paned *p);
static int ewl_paned_widgets_place(Ewl_Paned *p, Ewl_Paned_Layout *layout);

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

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				ewl_paned_cb_configure, NULL);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
	ewl_object_alignment_set(EWL_OBJECT(w), EWL_FLAG_ALIGN_LEFT |
						EWL_FLAG_ALIGN_TOP);
	ewl_widget_focusable_set(w, FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: The paned to set the orientation on
 * @param o: The Ewl_Orientation to set
 * @return Returns no value.
 * @brief Set the orientation of the paned */
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
		if (EWL_PANED_GRABBER_IS(child))
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
	if (EWL_PANED_GRABBER_IS(w))
		DRETURN(DLEVEL_STABLE);

	o = ewl_paned_grabber_new();
	ewl_paned_grabber_paned_orientation_set(EWL_PANED_GRABBER(o),
				ewl_paned_orientation_get(EWL_PANED(c)));

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
	Ewl_Paned *p;
	Ewl_Widget *o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PANED(c);
	/* skip grabbers */
	if (EWL_PANED_GRABBER_IS(w))
		DRETURN(DLEVEL_STABLE);

	/* our grabber is always to our left, since we were just removed
	 * from idx that means it's at idx - 1 */
	o = ewl_container_child_internal_get(c, idx - 1);
	if (p->last_grab == o)
		p->last_grab = NULL;

	ewl_widget_destroy(o);
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

	EWL_PANED(c)->new_panes = TRUE;

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
	int last_size, pos_diff, available, pane_num;
	int tot_paned_size, other_size, main_dir, other_dir;
	int grabber_size = 0, cur_pos, resizable;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PANED(w);
	c = EWL_CONTAINER(p);

	/*
	 * Before we start with the code, here is a little overview what
	 * the function actually do. The main thing to understand is
	 * that we first only place the grabber and ignore more or less
	 * the pane widgets. Every resizing is done through the grabber
	 * positions. When we this have done the pane widgets will be
	 * placed between the grabber
	 */	
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

	last_size = p->last_size;
	p->last_size = tot_paned_size;
	pos_diff = p->last_pos - main_dir;
	p->last_pos = main_dir;

	if (ecore_dlist_is_empty(c->children))
		DRETURN(DLEVEL_STABLE);
	/* if there are new widgets we have to place them first else
	 * we need to move the grabber to the new position. this is important
	 * because the grabber are also the size setter for the pane
	 * widget between them */
	if (p->new_panes) {
		last_size = ewl_paned_widgets_place(p, layout);
	}
	else 
	{
		/* move the grabber to the new position */		
		ecore_dlist_goto_first(c->children);
		while ((child = ewl_paned_grabber_next(p)))
		{
			int pos;

			/* get the grabber size if needed */
			if (!grabber_size)
				grabber_size = 
					layout->current_size(EWL_OBJECT(child));

			/* move the grabber to the new position */
			pos = layout->current_position(EWL_OBJECT(child));
			pos -= pos_diff;
			layout->position_request(EWL_OBJECT(child), pos);
			/* the 'other' position is for all grabber the same
			 * so no need to fetch it from some where */
			layout->stable_position_request(EWL_OBJECT(child),
								other_dir);
			layout->stable_request(EWL_OBJECT(child), other_size);
		}
	}
	/* we need to now the number of panes
	 * minus one because we cannot change the size of the 
	 * last pane directly*/	
	resizable = pane_num = ewl_paned_visible_panes(p) - 1;
	/* We now resize the pane areas so that they fit into the new size
	 * therefor we have to first calculate, how many space is available
	 * for giving or taking it from the panes */
	available = tot_paned_size - last_size;

	while (available != 0)
	{
		int old_pos, give;

		cur_pos = old_pos = main_dir;
		/* if we have no panes we don't need to calc their place */
		if (resizable < 1)
			break;	

		/* give can also be negative, so see it as a can take or give */
		give = available / (resizable + 1);
		/* reset the resizable pane_num now */
		resizable = pane_num;
		/* to prevent rounding errors */
		if (give == 0) {
			give = (available > 0) ? 1 : -1;
			if (!p->last_grab) {
				ecore_dlist_goto_first(c->children);
			}
			else
				ecore_dlist_goto(c->children, p->last_grab);
		}
		else
			ecore_list_goto_first(c->children);

		while ((child = ecore_list_next(c->children)))
		{
			Ewl_Widget *grab;
			int min, new_size, grab_pos;
			if (!VISIBLE(child) 
				|| EWL_PANED_GRABBER_IS((child)))
				continue;

			min = layout->minimum_size(EWL_OBJECT(child));
			grab = ewl_paned_grabber_next(p);

			if (!grab) {
				new_size = p->last_size - 
						(cur_pos - p->last_pos);
				if (new_size < min) {
					available -= min - new_size;
				}
				available -= give;

				if (available == 0)
					p->last_grab = NULL;
				break;
			}
			
			grab_pos = layout->current_position(EWL_OBJECT(grab));

			if (grab_pos - old_pos + give < min) 
			{
				new_size = min;
				resizable--;
			}
			else
				new_size = grab_pos - old_pos + give;
			
			available -= new_size - (grab_pos - old_pos);

			/* if there is no space to distribute left
			 * we can stop in resizing the panes, but we
			 * still have to place them on there new
			 * places */
			if (available == 0 && give != 0) {
				give = 0;
				p->last_grab = grab;
			}
			
			cur_pos += new_size;
			layout->position_request(EWL_OBJECT(grab), cur_pos);
			cur_pos += grabber_size;
			old_pos = grab_pos + grabber_size;
		}
	}

	/* now that all of the space is filled we can go and layout all of
	 * the available widgets */
	cur_pos = main_dir;
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		Ewl_Widget *grab;
		int size;

		if (!VISIBLE(child))
			continue;

		/* first calculate the size */ 
		grab = ewl_paned_grabber_next(p);
		if (grab) {
			size = layout->current_position(EWL_OBJECT(grab))
				- cur_pos;
		}
		else
			/* this is actually the current size */
			size = p->last_size - (cur_pos - p->last_pos); 

		if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
			ewl_object_place(EWL_OBJECT(child), cur_pos, other_dir, 
							size, other_size);
		else
			ewl_object_place(EWL_OBJECT(child), other_dir, cur_pos, 
							other_size, size);

		cur_pos += size + grabber_size;
	}

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
		if (EWL_PANED_GRABBER_IS(child))
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
ewl_paned_grabber_cb_mouse_down(Ewl_Widget *w, void *ev,
						void *data __UNUSED__)
{
	Ewl_Paned *p;
	Ewl_Event_Mouse *event;
	int ds;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	p = EWL_PANED(w->parent);

	if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
		ds = CURRENT_X(w) - event->x;
	else
		ds = CURRENT_Y(w) - event->y;
	
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_paned_grabber_cb_mouse_move, 
				(void *) ds);
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
ewl_paned_grabber_cb_mouse_move(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Event_Mouse *e;
	Ewl_Paned_Grabber *stop_grabber = NULL;
	Ewl_Widget *child;
	Ewl_Container *c;
	Ewl_Paned *p;
	Ewl_Widget *prev_pane = NULL, *next_pane = NULL;

	int paned_pos, paned_size;
	int prev_grabber_pos, next_grabber_pos;
	int grabber_pos, grabber_size;
	int mouse_pos, mouse_vec, mouse_offset;
	int grabber_pos_new;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = ev;
	p = EWL_PANED(w->parent);
	c = EWL_CONTAINER(p);
	mouse_offset = (int) data;

	if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL)
	{
		layout = horizontal_layout;
		mouse_pos = e->x;
		paned_pos = CURRENT_X(p);
		paned_size = CURRENT_W(p);
	}
	else
	{
		layout = vertical_layout;
		mouse_pos = e->y;
		paned_pos = CURRENT_Y(p);
		paned_size = CURRENT_H(p);
	}

	grabber_pos = layout->current_position(EWL_OBJECT(w));
	grabber_size = layout->current_size(EWL_OBJECT(w));

	/* 
	 * this is the vector pointing from the left/top edge of the grabber
	 * to the mouse position, so is it neagtive the grabber will
	 * be moved to the left side and is it positiv to the right
	 */
	mouse_vec = mouse_pos - grabber_pos + mouse_offset;
	if (mouse_vec == 0)
		DRETURN(DLEVEL_STABLE);

	/* find the previous grabber that is blocking us */
	ecore_dlist_goto(c->children, w);

	/* move past the selected grabber */
	ecore_dlist_previous(c->children);
	while ((child = ecore_dlist_previous(c->children)))
	{
		if (!VISIBLE(child)) continue;

		if (EWL_PANED_GRABBER_IS(child))
		{
			stop_grabber = EWL_PANED_GRABBER(child);
			break;
		}
		else
			prev_pane = child;
	}

	/* if we didn't find a privous grabber set the paned position instead */
	if (stop_grabber)
		prev_grabber_pos = 
			layout->current_position(EWL_OBJECT(stop_grabber))
			+ grabber_size;
	else
		prev_grabber_pos = paned_pos;

	/* and now find the right/bottom pane */
	ecore_dlist_goto(c->children, w);
	stop_grabber = NULL;

	/* move past the selected grabber */
	ecore_dlist_next(c->children);
	while ((child = ecore_list_next(c->children)))
	{
		if (!VISIBLE(child)) continue;

		if (EWL_PANED_GRABBER_IS(child)) {
			stop_grabber = EWL_PANED_GRABBER(child);
			break;
		}
		else
			next_pane = child;
	}
	
	/* if we didn't find a prevous grabber set the paned position instead */
	if (stop_grabber)
		next_grabber_pos = 
			layout->current_position(EWL_OBJECT(stop_grabber));
	else
		next_grabber_pos = paned_pos + paned_size;

	/*
	 * now we have collected enought data to place the grabber
	 * and the panes on their new places
	 */
	/* we don't want to shrink the panes more that it is allowed */
	if (mouse_vec < 0) {
		/* the left side get shrinked */
		int pane_min = layout->minimum_size(EWL_OBJECT(prev_pane));
		if (grabber_pos + mouse_vec - prev_grabber_pos < pane_min)
			grabber_pos_new = prev_grabber_pos + pane_min;
		else
			/* note that mouse_vec is here negative! */
			grabber_pos_new = grabber_pos + mouse_vec;
	}
	else {
		/* the right/bottom side get shrinked */
		int pane_min = layout->minimum_size(EWL_OBJECT(next_pane));
		if (next_grabber_pos - (grabber_pos + mouse_vec + grabber_size)
				< pane_min)
			grabber_pos_new = 
				next_grabber_pos - pane_min - grabber_size;
		else
			grabber_pos_new = grabber_pos + mouse_vec;
	}

	/*
	 * finally we can place the stuff 
	 */
	if (ewl_paned_orientation_get(p) == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_place(EWL_OBJECT(prev_pane), prev_grabber_pos,
					CURRENT_Y(p),
					grabber_pos_new - prev_grabber_pos,
					CURRENT_H(p));
		ewl_object_place(EWL_OBJECT(w), grabber_pos_new,
					CURRENT_Y(p),
					grabber_size,
					CURRENT_H(p));
		ewl_object_place(EWL_OBJECT(next_pane), 
					grabber_pos_new + grabber_size,
					CURRENT_Y(p),
					next_grabber_pos - grabber_pos_new
					- grabber_size,
					CURRENT_H(p));
	}
	else {
		ewl_object_place(EWL_OBJECT(prev_pane), 
					CURRENT_X(p),
					prev_grabber_pos,
					CURRENT_W(p),
					grabber_pos_new - prev_grabber_pos);
		ewl_object_place(EWL_OBJECT(w), 
					CURRENT_X(p),
					grabber_pos_new,
					CURRENT_W(p),
					grabber_size);
		ewl_object_place(EWL_OBJECT(next_pane), 
					CURRENT_X(p),
					grabber_pos_new + grabber_size,
					CURRENT_W(p),
					next_grabber_pos - grabber_pos_new
					- grabber_size);
	}

	/* send a value_changed callback to signal something moved */
	ewl_callback_call(EWL_WIDGET(p), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_layout_setup(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_shutdown_add(ewl_paned_cb_shutdown);

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
ewl_paned_widgets_place(Ewl_Paned *p, Ewl_Paned_Layout *layout)
{
	Ewl_Widget *child;
	Ewl_Container *c;
	int cur_pos, main_pos;
	int other_pos, other_size;
	int grabber_size = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, 0);
	DCHECK_TYPE_RET("p", p, EWL_PANED_TYPE, 0);
	DCHECK_PARAM_PTR_RET("layout", layout, 0);

	c = EWL_CONTAINER(p);

	if (p->orientation == EWL_ORIENTATION_HORIZONTAL)
	{
		main_pos = CURRENT_X(p);
		other_pos = CURRENT_Y(p);
		other_size = CURRENT_H(p);
	}
	else
	{
		main_pos = CURRENT_Y(p);
		other_pos = CURRENT_X(p);
		other_size = CURRENT_W(p);
	}

	cur_pos = main_pos;
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		Ewl_Widget *grab;
		int min;

		if (!VISIBLE(child)) continue;

		min = layout->minimum_size(EWL_OBJECT(child));
		
		grab = ewl_paned_grabber_next(p);

		if (!grab) 
		{
			cur_pos += min;
			break;
		}
		
		if (!grabber_size)
			grabber_size = 
				layout->minimum_size(EWL_OBJECT(grab));

		cur_pos += min;
		layout->variable_request(EWL_OBJECT(grab), grabber_size);
		layout->stable_request(EWL_OBJECT(grab), other_size);
		layout->position_request(EWL_OBJECT(grab), cur_pos);
		layout->stable_position_request(EWL_OBJECT(grab), other_pos);

		cur_pos += grabber_size;
	}

	p->new_panes = FALSE;

	DRETURN_INT(cur_pos - main_pos, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_paned_grabber_next(Ewl_Paned *p)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, 0);
	DCHECK_TYPE_RET("p", p, EWL_PANED_TYPE, 0);

	c = EWL_CONTAINER(p);

	while ((child = ecore_dlist_next(c->children))) 
	{
		if (VISIBLE(child) && EWL_PANED_GRABBER_IS(child ))
			break;
	}

	DRETURN_INT(child, DLEVEL_STABLE);
}

static int
ewl_paned_visible_panes(Ewl_Paned *p)
{
	Ewl_Container *c;
	Ewl_Widget *child;
	int i = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, 0);
	DCHECK_TYPE_RET("p", p, EWL_PANED_TYPE, 0);

	c = EWL_CONTAINER(p);

	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children))) 
	{
		if (VISIBLE(child) && !EWL_PANED_GRABBER_IS(child))
			i++;
	}

	DRETURN_INT(i, DLEVEL_STABLE);
}

static void
ewl_paned_cb_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	IF_FREE(horizontal_layout);
	IF_FREE(vertical_layout);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

