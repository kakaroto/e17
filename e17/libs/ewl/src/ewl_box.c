
#include <Ewl.h>

typedef struct
{

	/*
	 * Alignment is only done in the align direction.
	 */
	int             f1_align, f3_align;
	int             a1_align, a3_align;

	/*
	 * This is an indicator for filling in the proper directions.
	 */
	int             f_policy;

	/*
	 * Function pointers for getting the dimension of the widget that we
	 * care about.
	 */
	void            (*pref_fill_set) (Ewl_Object * ob, int size);
	int             (*fill_ask) (Ewl_Object * ob);
	void            (*fill_set) (Ewl_Object * ob, int size);

	int    (*align_ask) (Ewl_Object * ob);
	void            (*align_set) (Ewl_Object * ob, int size);

} Box_Orientation;

/*
 * The information for the two different orientations
 */
Box_Orientation *vertical = NULL, *horizontal = NULL;

/*
 * And a pointer to the currently used orientation
 */
Box_Orientation *info = NULL;

/*
 * These lists are used to sort children when configured.
 */
static Ecore_List *spread = NULL;

static void     ewl_box_setup();

static void     ewl_box_configure_calc(Ewl_Box * b, int *fill_size,
					 int *align_size);
static void     ewl_box_configure_fill(Ewl_Box * b, int *fill_size,
					 int *align_size);
static void     ewl_box_configure_layout(Ewl_Box * b, int *x, int *y,
					   int *fill, int *align,
					   int *align_size);
static void     ewl_box_configure_child(Ewl_Box * b, Ewl_Object * c, int *x,
					  int *y, int *align, int *align_size);

/**
 * @param o: the orientation for the box's layout
 * @return Returns NULL on failure, or a newly allocated box on success.
 * @brief Allocate and initialize a new box with given orientation
 */
Ewl_Widget     *ewl_box_new(Ewl_Orientation o)
{
	Ewl_Box        *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Box, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_box_init(b, o)) {
		ewl_widget_destroy(EWL_WIDGET(b));
		b = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the box to initialize
 * @param o: the orientation for the box to layout child widgets
 * @return Returns no value.
 * @brief Initialize the box to starting values
 *
 * Responsible for setting up default values and callbacks
 * within a box structure.
 */
int ewl_box_init(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, FALSE);

	w = EWL_WIDGET(b);

	/*
	 * Create the temporary layout lists now that they are needed.
	 */
	if (!spread)
		spread = ecore_list_new();

	/*
	 * Initialize the container portion of the box
	 */
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		if (!ewl_container_init(EWL_CONTAINER(b), "hbox"))
			DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	else {
		if (!ewl_container_init(EWL_CONTAINER(b), "vbox"))
			DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_container_add_notify(EWL_CONTAINER(b), ewl_box_child_add_cb);
	ewl_container_remove_notify(EWL_CONTAINER(b), ewl_box_child_remove_cb);
	ewl_container_resize_notify(EWL_CONTAINER(b), ewl_box_child_resize_cb);
	ewl_container_show_notify(EWL_CONTAINER(b), ewl_box_child_show_cb);
	ewl_container_hide_notify(EWL_CONTAINER(b), ewl_box_child_hide_cb);

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_box_configure_cb,
			NULL);

	/*
	 * Check if the info structs have been created yet, if not create
	 * them.
	 */
	if (!vertical)
		ewl_box_setup();

	/*
	 * Set the box's appropriate orientation, set to a garbage value so
	 * that the orientation does it's job.
	 */
	b->orientation = 0xdeadbeef;
	ewl_box_set_orientation(b, o);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param b: the box to change the orientation
 * @param o: the orientation to set for the box
 * @return Returns no value.
 * @brief Change the specified box's orientation
 *
 * Changes the orientation of the specified box, and
 * reconfigures it in order for the appearance to be updated.
 */
void ewl_box_set_orientation(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	/*
	 * Set the orientation and reconfigure the widget so that child
	 * widgets now have the new orientation layout
	 */
	if (b->orientation == o)
		DRETURN(DLEVEL_STABLE);

	b->orientation = o;
	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_set_appearance(w, "hbox");
	else
		ewl_widget_set_appearance(w, "vbox");

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the box to retrieve orientation
 * @return Returns the orientation value of the box @a b.
 * @brief Retrieves the orientation of the box
 */
Ewl_Orientation ewl_box_get_orientation(Ewl_Box *b)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, 0);

	DRETURN_INT(b->orientation, DLEVEL_STABLE);
}

/**
 * @param b: the box to change homogeneous layout
 * @param h: the boolean value to change the layout mode to
 * @return Returns no value.
 * @brief Change the homogeneous layout of the box
 *
 * Boxes use homogeneous layout by default, this can be used
 * to change that.
 */
void ewl_box_set_homogeneous(Ewl_Box *b, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	if (b->homogeneous == h)
		DRETURN(DLEVEL_STABLE);

	b->homogeneous = h;

	if (h) {
		ewl_callback_del(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
				ewl_box_configure_cb);
		ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
				ewl_box_configure_homogeneous_cb, NULL);
		ewl_container_show_notify(EWL_CONTAINER(b),
					  ewl_box_child_homogeneous_show_cb);
		ewl_container_hide_notify(EWL_CONTAINER(b),
					  ewl_box_child_homogeneous_show_cb);
	}
	else {
		ewl_callback_del(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
				ewl_box_configure_homogeneous_cb);
		ewl_callback_append(EWL_WIDGET(b), EWL_CALLBACK_CONFIGURE,
				ewl_box_configure_cb, NULL);
		ewl_container_show_notify(EWL_CONTAINER(b),
					  ewl_box_child_show_cb);
		ewl_container_hide_notify(EWL_CONTAINER(b),
					  ewl_box_child_hide_cb);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param b: the box to change the spacing
 * @param s: the spacing to put between the child widgets
 * @return Returns no value.
 * @brief Changes the spacing between the objects in the box
 *
 * Adjust the spacing of the specified box and reconfigure
 * it to change the appearance.
 */
void ewl_box_set_spacing(Ewl_Box * b, int s)
{
	int nodes;
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	nodes = ecore_list_nodes(EWL_CONTAINER(b)->children);

	if (nodes) {
		nodes--;
		if (b->orientation == EWL_ORIENTATION_HORIZONTAL)
			ewl_object_set_preferred_w(EWL_OBJECT(w),
					PREFERRED_W(w) - (nodes * b->spacing) +
					(nodes * s));
		else
			ewl_object_set_preferred_h(EWL_OBJECT(w),
					PREFERRED_H(w) - (nodes * b->spacing) +
					(nodes * s));
	}

	b->spacing = s;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_box_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Box        *b;

	int             total_fill = 0;
	int             x, y;
	int    width, height;
	int            *fill, *align;
	int   *fill_size, *align_size;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	/*
	 * Catch the easy case, and return.
	 */
	if (ecore_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	/*
	 * Get the starting values for the dimensions of the box.
	 */
	x = CURRENT_X(w);
	y = CURRENT_Y(w);
	width = CURRENT_W(w);
	height = CURRENT_H(w);

	/*
	 * These variables avoid large nested if's to handle different
	 * orientations.
	 */
	if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
		fill = &x;
		fill_size = &width;
		align = &y;
		align_size = &height;
		info = horizontal;
	} else {
		fill = &y;
		fill_size = &height;
		align = &x;
		align_size = &width;
		info = vertical;
	}

	/*
	 * Save the starting fill size.
	 */
	total_fill = *fill_size;

	/*
	 * Split the children up for configuration based on alignment.
	 */
	ewl_box_configure_calc(b, fill_size, align_size);

	/*
	 * Spread the fill space if necessary.
	 */
	ewl_box_configure_fill(b, fill_size, align_size);

	/*
	 * Layout the children in their appropriate positions.
	 */
	ewl_box_configure_layout(b, &x, &y, fill, align, align_size);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_box_configure_homogeneous_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	int             i, num;
	int             x, y;
	int             width, height;
	int            *fill;
	int            *fill_size;
	int             remainder;
	Ewl_Object     *child;
	Ewl_Box        *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ecore_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	num = 0;
	ecore_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(w)->children))) {
		if (VISIBLE(child))
			num++;
	}

	if (!num)
		DRETURN(DLEVEL_STABLE);

	b = EWL_BOX(w);

	/*
	 * Get the starting values for the dimensions of the box.
	 */
	x = CURRENT_X(w);
	y = CURRENT_Y(w);
	width = CURRENT_W(w);
	height = CURRENT_H(w);

	/*
	 * These variables avoid large nested if's to handle different
	 * orientations.
	 */
	if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
		fill = &x;
		fill_size = &width;
	} else {
		fill = &y;
		fill_size = &height;
	}

	*fill_size -= b->spacing * (num - 1);
	remainder = *fill_size % num;
	*fill_size = *fill_size / num;

	i = 0;
	ecore_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(w)->children))) {
		if (VISIBLE(child)) {
			i++;
			if (i == num)
				*fill_size += remainder;
			ewl_object_place(child, x, y, width, height);
			*fill += *fill_size + b->spacing;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Split the children into three lists for layout.
 */
static void
ewl_box_configure_calc(Ewl_Box * b, int *fill_size, int *align_size)
{
	Ewl_Object     *child;
	int    initial;

	DENTER_FUNCTION(DLEVEL_STABLE);

	initial = *fill_size / ecore_list_nodes(EWL_CONTAINER(b)->children);

	/*
	 * Sort the children into lists dependant on their alignment within the
	 * box.
	 */
	ecore_list_goto_first(EWL_CONTAINER(b)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(b)->children))) {
		int             change;

		/*
		 * Place the child on a list depending on it's matching
		 * alignment. First check for top/left alignment.
		 */
		if (VISIBLE(child)) {

			/*
			 * Set the initial fill size to the preferred size.
			 */
			info->fill_set(child, initial);

			/*
			 * Figure out how much extra space is available for
			 * filling widgets.
			 */
			change = *fill_size;
			*fill_size -= info->fill_ask(child) + b->spacing;

			/*
			 * Attempt to give the widget the full size, this will
			 * fail if the fill policy or bounds don't allow it.
			 */
			info->align_set(child, *align_size);

			/*
			 * If it has a fill policy for a direction we're
			 * concerned with, add it to the fill list.
			 */
			if (ewl_object_get_fill_policy(child) & info->f_policy)
				ecore_list_append(spread, child);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Spread space to any widgets that have fill policy set to fill. This should
 * not be called if @num_fill or *@fill_size are equal to zero.
 */
static void
ewl_box_configure_fill(Ewl_Box * b, int *fill_size, int *align_size)
{
	int             space;
	int             temp, remainder;
	Ewl_Object     *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Calculate the space to give per child. Safeguard against divide by
	 * zero.
	 */
	space = (ecore_list_is_empty(spread) ? 0 :
		 *fill_size / ecore_list_nodes(spread));

	/*
	 * As long as children keep accepting more space, we should loop
	 * through and give them what's available.
	 */
	while (space && !ecore_list_is_empty(spread)) {

		ecore_list_goto_first(spread);
		while ((c = ecore_list_current(spread))) {

			/*
			 * Save the current size of the child, then
			 * attempt to give it a portion of the space
			 * available.
			 */
			temp = info->fill_ask(c);
			info->fill_set(c, temp + space);

			/*
			 * Determine if the child accepted any of the space
			 */
			temp = info->fill_ask(c) - temp;

			/*
			 * If the child did not accept any of the size, then
			 * it's at it's max/min and is no longer useful.
			 */
			if (!temp || (*fill_size - temp < 0))
				ecore_list_remove(spread);
			else {
				*fill_size -= temp;
				ecore_list_next(spread);
			}
		}

		/*
		 * Calculate the space to give per child.
		 */
		space = (ecore_list_is_empty(spread) ? 0 :
			 *fill_size / ecore_list_nodes(spread));
	}

	/*
	 * Distribute any remaining fill space.
	 */
	while (*fill_size && !ecore_list_is_empty(spread)) {

		/*
		 * Determine the sign of the amount to be incremented.
		 */
		remainder = *fill_size / abs(*fill_size);

		/*
		 * Add the remainder sign to each child.
		 */
		ecore_list_goto_first(spread);
		while (*fill_size && (c = ecore_list_current(spread))) {

			/*
			 * Store the current size of the child.
			 */
			temp = info->fill_ask(c);

			/*
			 * Attempt to give it a portion of the remaining space
			 */
			info->fill_set(c, temp + remainder);

			/*
			 * Determine if the child accepted the space
			 */
			temp = info->fill_ask(c) - temp;

			/*
			 * Remove the child if it didn't accept any space,
			 * otherwise subtract the accepted space from the
			 * total.
			 */
			if (!temp || (*fill_size - temp < 0))
				ecore_list_remove(spread);
			else {
				*fill_size -= remainder;
				ecore_list_next(spread);
			}
		}
	}

	/*
	 * This contents of the list are no longer needed.
	 */
	ecore_list_clear(spread);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_box_configure_layout(Ewl_Box * b, int *x, int *y, int *fill,
			   int *align, int *align_size)
{
	Ewl_Object     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Configure the widgets on the first list.
	 */
	ecore_list_goto_first(EWL_CONTAINER(b)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(b)->children))) {

		if (VISIBLE(child)) {

			/*
			 * Position this child based on the determined values.
			 */
			ewl_box_configure_child(b, child, x, y, align,
						  align_size);

			/*
			 * Move to the next position for the child.
			 */
			*fill += info->fill_ask(child) + b->spacing;
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_box_configure_child(Ewl_Box * b, Ewl_Object * c, int *x, int *y,
			  int *align, int *align_size)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Adjust the alignment in the direction opposite of the
	 * orientation. The first one is the simplest as it it a
	 * direct use of the current coordinates.
	 */
	if (ewl_object_get_alignment(c) & info->a1_align) {
		ewl_object_request_position(c, *x, *y);
	}

	/*
	 * The second one is aligned against the furthest edge, so
	 * there is some calculation to be made.
	 */
	else if (ewl_object_get_alignment(c) & info->a3_align) {
		*align += *align_size - info->align_ask(c);
		ewl_object_request_position(c, *x, *y);
		*align -= *align_size - info->align_ask(c);
	}

	/*
	 * The final one is for centering the child.
	 */
	else {
		*align += (*align_size - info->align_ask(c)) / 2;
		ewl_object_request_position(c, *x, *y);
		*align -= (*align_size - info->align_ask(c)) / 2;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_box_child_add_cb(Ewl_Container * c, Ewl_Widget * w)
{
}

void
ewl_box_child_remove_cb(Ewl_Container * c, Ewl_Widget * w)
{
}

/*
 * When a child gets added to the box update it's size.
 */
void
ewl_box_child_show_cb(Ewl_Container * c, Ewl_Widget * w)
{
	int size, space = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ecore_list_nodes(c->children) > 1)
		space = EWL_BOX(c)->spacing;

	/*
	 * Base the info used on the orientation of the box.
	 */
	if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		size = PREFERRED_W(c) +
			ewl_object_get_preferred_w(EWL_OBJECT(w)) + space;
		ewl_object_set_preferred_w(EWL_OBJECT(c), size);
		ewl_container_prefer_largest(c, EWL_ORIENTATION_VERTICAL);
	}
	else {
		size = PREFERRED_H(c) +
			ewl_object_get_preferred_h(EWL_OBJECT(w)) + space;
		ewl_object_set_preferred_h(EWL_OBJECT(c), size);
		ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_box_child_homogeneous_show_cb(Ewl_Container * c, Ewl_Widget * w)
{
	int numc;
	int size, space = 0;
	DENTER_FUNCTION(DLEVEL_STABLE);

	numc = ecore_list_nodes(c->children);
	numc--;
	if (numc)
		space = EWL_BOX(c)->spacing;

	ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);
	ewl_container_prefer_largest(c, EWL_ORIENTATION_VERTICAL);
	if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		size = (PREFERRED_W(c) + space) * numc - space;
		ewl_object_set_preferred_w(EWL_OBJECT(c), size);
	}
	else {
		size = (PREFERRED_H(c) + space) * numc - space;
		ewl_object_set_preferred_h(EWL_OBJECT(c), size);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_box_child_hide_cb(Ewl_Container * c, Ewl_Widget * w)
{
	int space = 0;
	Ewl_Box *b = EWL_BOX(c);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ecore_list_nodes(c->children) > 1)
		space = b->spacing;

	if (b->orientation == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_set_preferred_w(EWL_OBJECT(c),
				PREFERRED_W(c) -
				ewl_object_get_preferred_w(EWL_OBJECT(w)) -
				space);

		ewl_container_prefer_largest(c, EWL_ORIENTATION_VERTICAL);
	}
	else {
		ewl_object_set_preferred_h(EWL_OBJECT(c),
				PREFERRED_H(c) - 
				ewl_object_get_preferred_h(EWL_OBJECT(w)) -
				space);
		ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Determine the preferred size of the box when a child changes it's preferred
 * size.
 */
void
ewl_box_child_resize_cb(Ewl_Container * c, Ewl_Widget * w, int size,
		       Ewl_Orientation o)
{
	int align_size, fill_size;
	Box_Orientation *info;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Get the appropriate dimension setting functions based on the
	 * orientation.
	 */
	if (EWL_BOX(c)->orientation == EWL_ORIENTATION_HORIZONTAL) {
		fill_size = PREFERRED_W(c);
		align_size = PREFERRED_H(c);
		info = horizontal;
	}
	else {
		fill_size = PREFERRED_H(c);
		align_size = PREFERRED_W(c);
		info = vertical;
	}

	/*
	 * If the change is in the orientation direction, just add it to the
	 * current orientation size.
	 */
	if (EWL_BOX(c)->orientation == o)
		info->pref_fill_set(EWL_OBJECT(c), fill_size + size);

	/*
	 * Find the new largest widget in the alignment direction
	 */
	else
		ewl_container_prefer_largest(c, o);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Setup some internal variables for effectively laying out the children based
 * on orientation.
 */
static void
ewl_box_setup()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!vertical) {
		vertical = NEW(Box_Orientation, 1);
		if (!vertical)
			DRETURN(DLEVEL_STABLE);

		/*
		 * This sets the aligments for filling direction.
		 */
		vertical->f1_align = EWL_FLAG_ALIGN_TOP;
		vertical->f3_align = EWL_FLAG_ALIGN_BOTTOM;

		/*
		 * These are the valid fill policies for this widget.
		 */
		vertical->f_policy =
		    EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_VFILL;

		/*
		 * This sets the aligments for the non-filling direction.
		 */
		vertical->a1_align = EWL_FLAG_ALIGN_LEFT;
		vertical->a3_align = EWL_FLAG_ALIGN_RIGHT;

		/*
		 * These functions allow for asking the dimensions of the
		 * children.
		 */
		vertical->fill_ask = ewl_object_get_current_h;

		vertical->align_ask = ewl_object_get_current_w;

		/*
		 * These functions allow for setting the dimensions of the
		 * children.
		 */
		vertical->fill_set = ewl_object_request_h;
		vertical->pref_fill_set = ewl_object_set_preferred_h;

		vertical->align_set = ewl_object_request_w;
	}

	if (!horizontal) {
		horizontal = NEW(Box_Orientation, 1);
		if (!horizontal)
			DRETURN(DLEVEL_STABLE);

		/*
		 * This sets the aligments for the filling direction.
		 */
		horizontal->f1_align = EWL_FLAG_ALIGN_LEFT;
		horizontal->f3_align = EWL_FLAG_ALIGN_RIGHT;

		/*
		 * These are the valid fill policies for this widget.
		 */
		horizontal->f_policy =
		    EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_HFILL;

		/*
		 * This sets the aligments for the non-filling direction.
		 */
		horizontal->a1_align = EWL_FLAG_ALIGN_TOP;
		horizontal->a3_align = EWL_FLAG_ALIGN_BOTTOM;

		/*
		 * These functions allow for asking the dimensions of the
		 * children.
		 */
		horizontal->fill_ask = ewl_object_get_current_w;

		horizontal->align_ask = ewl_object_get_current_h;

		/*
		 * These functions allow for setting the dimensions of the
		 * children.
		 */
		horizontal->fill_set = ewl_object_request_w;
		horizontal->pref_fill_set = ewl_object_set_preferred_w;

		horizontal->align_set = ewl_object_request_h;
	}
}
