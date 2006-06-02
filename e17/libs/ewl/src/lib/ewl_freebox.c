#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_freebox_layout_auto(Ewl_Freebox *fb);
static void ewl_freebox_layout_manual(Ewl_Freebox *fb);
static void ewl_freebox_layout_comparator(Ewl_Freebox *fb);

/**
 * @note
 * This will allow for differnt types of 'free' layout depending on what you
 * need. First there is AUTO layout, which will just place the widgets in
 * rows and columns (this will expand the freebox as needed while
 * trying to maintain the given space). 
 * 
 * The second type of layout is MANUAL. Initially this will layout as per
 * AUTO but after that the user will be able to drag the widgets around and
 * put them where they want.
 * 
 * The third type of layout is COMPARATOR. This will require the comparator
 * function to be set into the freebox. Then, as it is layout out widgets it
 * will call the comparator to determine which should be placed first. It
 * will then layout in rows/columns the same as AUTO. (This is essentially
 * AUTO with sorting.)
 */

/**
 * @return Returns a new horizontal Ewl_Freebox widget or NULL on failure
 * @brief creates and initializes a new freebox widget
 */
Ewl_Widget *
ewl_hfreebox_new(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_freebox_new(), DLEVEL_STABLE);
}

/**
 * @return Returns a new vertical Ewl_Freebox widget or NULL on failure
 * @brief creates and initializes a new freebox widget
 */
Ewl_Widget *
ewl_vfreebox_new(void)
{
	Ewl_Widget *fb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fb = ewl_freebox_new();
	EWL_FREEBOX(fb)->orientation = EWL_ORIENTATION_VERTICAL;

	DRETURN_PTR(fb, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Freebox on success or NULL on failure
 * @brief Creates a new, initialized Ewl_Freebox widget
 */
Ewl_Widget *
ewl_freebox_new(void)
{
	Ewl_Widget *fb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fb = NEW(Ewl_Freebox, 1);
	if (!fb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_freebox_init(EWL_FREEBOX(fb)))
	{
		ewl_widget_destroy(fb);
		fb = NULL;
	}

	DRETURN_PTR(fb, DLEVEL_STABLE);
}

/**
 * @param fb: The Ewl_Freebox to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Freebox widget to default values.
 */
int
ewl_freebox_init(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fb", fb, TRUE);

	if (!ewl_container_init(EWL_CONTAINER(fb)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(fb), EWL_FREEBOX_TYPE);
	ewl_widget_inherit(EWL_WIDGET(fb), EWL_FREEBOX_TYPE);

	ewl_callback_append(EWL_WIDGET(fb), EWL_CALLBACK_CONFIGURE,
				ewl_freebox_cb_configure, NULL);
	ewl_container_add_notify_set(EWL_CONTAINER(fb),
				ewl_freebox_cb_child_add);
	ewl_container_show_notify_set(EWL_CONTAINER(fb),
				ewl_freebox_cb_child_show);

	fb->layout = EWL_FREEBOX_LAYOUT_AUTO;
	fb->orientation = EWL_ORIENTATION_HORIZONTAL;

	ewl_widget_focusable_set(EWL_WIDGET(fb), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param fb: The freebox to use
 * @param orientation: The orientation to set
 * @return Returns no value
 * @brief Sets the orientation of the freebox
 */
void
ewl_freebox_orientation_set(Ewl_Freebox *fb, Ewl_Orientation orientation)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	if (orientation == fb->orientation)
		DRETURN(DLEVEL_STABLE);

	fb->orientation = orientation;
	ewl_widget_configure(EWL_WIDGET(fb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fb: The freebox to use
 * @return Returns the orientation of the freebox
 * @brief Retrieve the current orientation of the freebox 
 */
Ewl_Orientation
ewl_freebox_orientation_get(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fb", fb, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("fb", fb, EWL_FREEBOX_TYPE,
					EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(fb->orientation, DLEVEL_STABLE);
}

/**
 * @param fb: The Ewl_Freebox to change
 * @param type: The Ewl_Freebox_Layout_Type to set on this freebox
 * @return Returns no value
 * @brief Set the layout type of the Ewl_Freebox
 */
void
ewl_freebox_layout_type_set(Ewl_Freebox *fb, Ewl_Freebox_Layout_Type type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	if (fb->layout == type)
		DRETURN(DLEVEL_STABLE);

	fb->layout = type;
	ewl_widget_configure(EWL_WIDGET(fb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param fb: The Ewl_Freebox to get the layout from 
 * @return Returns the Ewl_Freebox_Layout_Type currently set on this freebox
 * @brief Retrieve the layout type of the Ewl_Freebox
 */
Ewl_Freebox_Layout_Type
ewl_freebox_layout_type_get(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fb", fb, EWL_FREEBOX_LAYOUT_AUTO);
	DCHECK_TYPE_RET("fb", fb, EWL_FREEBOX_TYPE, EWL_FREEBOX_LAYOUT_AUTO);

	DRETURN_INT(fb->layout, DLEVEL_STABLE);
}

/**
 * @param fb: The Ewl_Freebox to set the comparator on
 * @param cmp: The Ewl_Freebox_Comparator functon to set
 * @return Returns no value 
 * @brief Set the comparator to use in the Ewl_Freebox
 */
void
ewl_freebox_comparator_set(Ewl_Freebox *fb, Ewl_Freebox_Comparator cmp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_PARAM_PTR("cmp", cmp);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	if (fb->comparator == cmp)
		DRETURN(DLEVEL_STABLE);

	fb->comparator = cmp;
	fb->sorted = FALSE;
	ewl_widget_configure(EWL_WIDGET(fb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/** 
 * @param fb: The Ewl_Freebox to get the comparator from
 * @return Returns the Ewl_Freebox_Comparator set on the given Ewl_Freebox
 * or NULL if none set.
 * @brief Retrieve the comparator set in the Ewl_Freebox
 */
Ewl_Freebox_Comparator
ewl_freebox_comparator_get(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("fb", fb, NULL);
	DCHECK_TYPE_RET("fb", fb, EWL_FREEBOX_TYPE, NULL);

	DRETURN_INT(fb->comparator, DLEVEL_STABLE);
}

/**
 * @param fb: The Ewl_Freebox to for a resort on
 * @return Returns no value.
 * @brief This will force the freebox to resort and redraw it's contents.
 * Useful if part of the content changes but no child widgets where
 * added/removed. (An icon label changes for example).
 */
void
ewl_freebox_resort(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	fb->sorted = FALSE;
	ewl_widget_configure(EWL_WIDGET(fb));

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
ewl_freebox_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Freebox *fb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_FREEBOX_TYPE);

	fb = EWL_FREEBOX(w);
	if (fb->layout == EWL_FREEBOX_LAYOUT_AUTO)
		ewl_freebox_layout_auto(fb);

	else if (fb->layout == EWL_FREEBOX_LAYOUT_MANUAL)
		ewl_freebox_layout_manual(fb);
	
	else if (fb->layout == EWL_FREEBOX_LAYOUT_COMPARATOR)
		ewl_freebox_layout_comparator(fb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container
 * @param w: The widget to work with
 * @return Returns no value
 * @brief Callback for when a child is added
 */
void
ewl_freebox_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Freebox *fb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* This only does a sorted = FALSE because in the COMPARATOR layout
	 * it will remove all of the widgets from the container and
	 * re-insert them. So, if we're doing anything more compilcated
	 * it'll get all fucked up */

	fb = EWL_FREEBOX(c);
	fb->sorted = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container
 * @param w: The widget to work with
 * @return Returns no value
 * @brief Callback for when a child is shown
 */
void
ewl_freebox_cb_child_show(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Freebox *fb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	fb = EWL_FREEBOX(c);
	fb->sorted = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}	

/* This will run through the list of child widgets and lay them out in rows
 * based on their size. */
static void
ewl_freebox_layout_auto(Ewl_Freebox *fb)
{
	Ewl_Container *c;
	Ewl_Widget *child;
	int max_pos, largest_size = 0, cur_pos = 0, cur_align;
	int base_pos, start_pos, pad = 0, *x, *y;
	int *stable_dir, *grow_dir, child_h, child_w;

	void (*pref_inner)(Ewl_Object *obj, int size);
	int (*current_pos)(Ewl_Object *obj);
	int (*current_size)(Ewl_Object *obj);
	int (*current_anchor)(Ewl_Object *obj);
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	pad = ewl_theme_data_int_get(EWL_WIDGET(fb),
					"/freebox/auto/padding");

	if (fb->orientation == EWL_ORIENTATION_HORIZONTAL)
	{
		pref_inner = ewl_object_preferred_inner_h_set;
		current_pos = ewl_object_current_x_get;
		current_size = ewl_object_current_w_get;
		current_anchor = ewl_object_current_y_get;

		stable_dir = &child_w;
		grow_dir = &child_h;

		x = &cur_align;
		y = &cur_pos;
	}
	else
	{
		pref_inner = ewl_object_preferred_inner_w_set;
		current_pos = ewl_object_current_y_get;
		current_size = ewl_object_current_h_get;
		current_anchor = ewl_object_current_x_get;

		stable_dir = &child_h;
		grow_dir = &child_w;

		x = &cur_pos;
		y = &cur_align;
	}

	base_pos = current_pos(EWL_OBJECT(fb));
	max_pos = base_pos + current_size(EWL_OBJECT(fb));

	start_pos = cur_pos = current_anchor(EWL_OBJECT(fb));
	cur_align = base_pos;

	c = EWL_CONTAINER(fb);
	ecore_dlist_goto_first(c->children);
	while ((child = ecore_dlist_next(c->children)))
	{
		if (!VISIBLE(child)) continue;
		ewl_object_current_size_get(EWL_OBJECT(child), 
						&child_w, &child_h);

		/* past end of widget, wrap */
		if ((cur_align + *stable_dir) > max_pos)
		{
			cur_align = base_pos;
			cur_pos += largest_size + pad;
			largest_size = 0;
		}

		if (*grow_dir > largest_size) 
			largest_size = *grow_dir;

		ewl_object_place(EWL_OBJECT(child), *x, *y, child_w, child_h);
		cur_align += *stable_dir + pad;
	}
	pref_inner(EWL_OBJECT(fb), cur_pos - start_pos + largest_size + pad);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* If the widget has been placed, (assuming (x, y) (0, 0) to mean unplaced)
 * then we will just put it where it currently is, otherwise we'll put it
 * somewhere sane ... Where that maybe, I'm not sure yet, heh */
static void
ewl_freebox_layout_manual(Ewl_Freebox *fb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * This will use the supplied comparator function in order to sort the
 * widgets, it will then use the auto layout algorithm to put the widgets on
 * screen */
static void
ewl_freebox_layout_comparator(Ewl_Freebox *fb)
{
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("fb", fb);
	DCHECK_TYPE("fb", fb, EWL_FREEBOX_TYPE);

	/* we're boned if we don't have a comparator */
	if (!fb->comparator)
	{
		DWARNING("No comparator set and using "
			 "EWL_FREEBOX_LAYOUT_COMPARATOR. "
			 "Bad programmer, bad.\n");
		DRETURN(DLEVEL_STABLE);
	}

	c = EWL_CONTAINER(fb);

	/* sort the data if needed */
	if (!fb->sorted)
	{
		Ecore_Sheap *sheap;
		sheap = ecore_sheap_new(ECORE_COMPARE_CB(fb->comparator), 
				ewl_container_child_count_get(EWL_CONTAINER(fb)));

		/* stick it all in the heap */
		while ((child = ecore_dlist_remove_first(c->children)))
			ecore_sheap_insert(sheap, child);

		ecore_sheap_sort(sheap);

		/* pull it from the heap and stick back into the container */
		while ((child = ecore_sheap_extract(sheap)))
			ecore_dlist_append(c->children, child);

		ecore_sheap_destroy(sheap);

		/* we set sorted to TRUE at the end here as each of those
		 * inserts into the container will trigger our child_add
		 * callback which will set sorted to FALSE */
		fb->sorted = TRUE;
	}

	/* just use the autolayout which will layout in the order the items
	 * are in the container. So, since we've already sorted the items
	 * their in the correct order for auto layout */
	ewl_freebox_layout_auto(fb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


