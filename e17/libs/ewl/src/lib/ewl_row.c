#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a newly allocated row on success, NULL on failure.
 * @brief Allocate and initialize a new row
 */
Ewl_Widget *
ewl_row_new(void)
{
	Ewl_Widget *row;

	DENTER_FUNCTION(DLEVEL_STABLE);

	row = NEW(Ewl_Row, 1);
	if (!row)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_row_init(EWL_ROW(row))) {
		ewl_widget_destroy(row);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	DRETURN_PTR(row, DLEVEL_STABLE);
}

/**
 * @param row: the row object to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the row fields of an inheriting object
 *
 * The fields of the @a row object are initialized to their defaults.
 */
int
ewl_row_init(Ewl_Row *row)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("row", row, FALSE);

	if (!ewl_container_init(EWL_CONTAINER(row)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(row), EWL_ROW_TYPE);
	ewl_widget_inherit(EWL_WIDGET(row), EWL_ROW_TYPE);

	ewl_container_show_notify_set(EWL_CONTAINER(row), ewl_row_child_show_cb);
	ewl_container_hide_notify_set(EWL_CONTAINER(row), ewl_row_child_hide_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(row), ewl_row_resize_cb);

	ewl_object_fill_policy_set(EWL_OBJECT(row), EWL_FLAG_FILL_HFILL);

	ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_CONFIGURE,
				ewl_row_configure_cb, NULL);

	ewl_widget_focusable_set(EWL_WIDGET(row), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param row: the row to change the header row
 * @param header: header row for adjusting cell placement
 * @return Returns no value.
 * @brief Set the row header of constraints on cell widths
 *
 * Changes the row that cell widths and placements will be based on to @a
 * header.
 */
void
ewl_row_header_set(Ewl_Row *row, Ewl_Row *header)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("row", row);
	DCHECK_TYPE("row", row, EWL_ROW_TYPE);

	if (row->header == header)
		DRETURN(DLEVEL_STABLE);

	row->header = header;
	if (header) {
		ewl_object_fill_policy_set(EWL_OBJECT(row),
					   EWL_FLAG_FILL_HFILL);

		ewl_widget_configure(EWL_WIDGET(header));
	}
	else
		ewl_widget_configure(EWL_WIDGET(row));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param row: the row to retrieve a columns widget from
 * @param n: the column containing the desired widget
 * @return Returns widget located in column @a n in @a row on success.
 * @brief Retrieve the widget at a specified column
 */
Ewl_Widget *
ewl_row_column_get(Ewl_Row *row, short n)
{
	Ewl_Widget *found;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("row", row, NULL);
	DCHECK_TYPE_RET("row", row, EWL_ROW_TYPE, NULL);

	found = ecore_dlist_goto_index(EWL_CONTAINER(row)->children, n + 1);

	DRETURN_PTR(found, DLEVEL_STABLE);
}

void
ewl_row_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Row *row;
	Ewl_Container *c;
	Ewl_Object *child;
	Ewl_Object *align;
	int x;
	int remains, nodes;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = EWL_ROW(w);
	c = EWL_CONTAINER(w);

	x = CURRENT_X(w);
	remains = CURRENT_W(w);

	nodes = 0;
	ewl_container_child_iterate_begin(c);
	while ((child = ewl_container_child_next(c))) {
		if (VISIBLE(child))
			nodes++;
	}

	ewl_container_child_iterate_begin(c);

	/*
	 * This should be the common case, a row bounded by a set of fields,
	 * for forming a table.
	 */
	if (row->header) {
		int width;
		Ewl_Container *hdr;

		hdr = EWL_CONTAINER(row->header);
		ewl_container_child_iterate_begin(EWL_CONTAINER(hdr));

		/*
		 * Get the first child of the header.
		 */
		align = EWL_OBJECT(ewl_container_child_next(EWL_CONTAINER(hdr)));
		if (align) {
			x = MAX(ewl_object_current_x_get(align), CURRENT_X(w));
		}
		else
			x = CURRENT_X(w);

		/*
		 * Iterate over the children and position the header children.
		 */
		ewl_container_child_iterate_begin(EWL_CONTAINER(hdr));
		while ((child = ewl_container_child_next(c))) {
			align = EWL_OBJECT(ewl_container_child_next(EWL_CONTAINER(hdr)));
			if (align && VISIBLE(align))
				width = ewl_object_current_x_get(align) + ewl_object_current_w_get(align) - x;
			else if (nodes)
				width = remains / nodes;
			else
				width = remains;

			/*
			 * Request the necessary geometry then check what was
			 * accepted to calculate remaining steps.
			 */
			ewl_object_place(child, x, CURRENT_Y(w), width,
					 CURRENT_H(w));
			width = ewl_object_current_w_get(child);
			x += width;
			remains -= width;
			nodes--;
		}
	}
	/*
	 * In the uncommon case, we simply try to give out a fair amount of
	 * space.
	 */
	else {
		int tx = x;
		while ((child = ewl_container_child_next(c))) {
			int portion;

			/*
			 * Ask for the child to stay the current size for now.
			 */
			portion = ewl_object_current_w_get(child);
			ewl_object_position_request(child, tx, CURRENT_Y(w));
			ewl_object_w_request(child, portion);
			ewl_object_h_request(child, CURRENT_H(w));

			remains -= portion;
			portion = ewl_object_current_w_get(child);
			tx = ewl_object_current_x_get(child) + portion;
		}

		/* Divvy up remaining space */
		if (remains) {
			tx = x;
			nodes = ecore_dlist_nodes(c->children);
		       	ecore_dlist_goto_first(c->children);
			while ((child = ecore_dlist_next(c->children))) {
				int portion;
				int width = ewl_object_current_w_get(child);

				if (nodes)
					portion = remains / nodes;
				else
					portion = remains;
				ewl_object_x_request(child, tx);
				ewl_object_w_request(child, portion + width);
				remains -= portion;
				portion = ewl_object_current_w_get(child);
				tx += portion;
				nodes--;
			}
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_row_header_configure_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
							void *user_data)
{
	Ewl_Row *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	row = EWL_ROW(user_data);
	/* ewl_object_preferred_inner_w_set(EWL_OBJECT(w), CURRENT_W(row->header)); */
	ewl_widget_configure(EWL_WIDGET(row));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_row_header_destroy_cb(Ewl_Widget *w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Row *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	row = EWL_ROW(user_data);
	row->header = NULL;
	ewl_row_header_set(row, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_row_child_show_cb(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Row *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = EWL_ROW(c);
	ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
	ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) +
			ewl_object_preferred_w_get(EWL_OBJECT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_row_child_hide_cb(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Row *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	row = EWL_ROW(c);
	ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
	ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) -
			ewl_object_preferred_w_get(EWL_OBJECT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_row_resize_cb(Ewl_Container *c, Ewl_Widget *w __UNUSED__, 
				int size, Ewl_Orientation o)
{
	Ewl_Row *row;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	row = EWL_ROW(c);
	if (o == EWL_ORIENTATION_VERTICAL)
		ewl_container_largest_prefer(c, EWL_ORIENTATION_VERTICAL);
	else
		ewl_object_preferred_inner_w_set(EWL_OBJECT(c),
				PREFERRED_W(c) + size);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

