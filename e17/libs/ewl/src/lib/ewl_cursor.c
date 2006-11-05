#include <Ewl.h>
#include "ewl_cursor.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_cursor_cb_render(Ewl_Widget *w, void *ev, void *data);
static void ewl_cursor_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a new Ewl_Cursor on success, or NULL on failure
 * @brief Creates and initializes a new Ewl_Cursor widget
 */
Ewl_Widget *
ewl_cursor_new(void)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Cursor, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_cursor_init(EWL_CURSOR(w)))
	{
		ewl_widget_destroy(w);
		w = NULL;
	}

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param cursor: The Ewl_Cursor to initialize
 * @return Returns TRUE on success, FALSE on failure
 * @brief Initializes an Ewl_Cursor widget
 */
int
ewl_cursor_init(Ewl_Cursor *cursor)
{
	Ewl_Widget *w = EWL_WIDGET(cursor);

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("cursor", cursor, FALSE);
	DCHECK_TYPE_RET("cursor", cursor, EWL_CURSOR_TYPE, FALSE);

	if (!ewl_window_init(EWL_WINDOW(cursor)))
		DRETURN_INT(FALSE, DLEVEL_UNSTABLE);

	ewl_widget_appearance_set(w, EWL_CURSOR_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);
	ewl_object_size_request(EWL_OBJECT(w), 64, 64);
	ewl_embed_engine_name_set(EWL_EMBED(cursor), "evas_buffer");
	ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
			ewl_cursor_cb_render, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, ewl_cursor_cb_destroy,
			NULL);

	DRETURN_INT(TRUE, DLEVEL_UNSTABLE);
}

static void
ewl_cursor_cb_render(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
       	int handle;
	int width, height;
	Ewl_Widget *parent;
	int old = 0;
	Ewl_Cursor *cursor = EWL_CURSOR(w);

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_CURSOR_TYPE);

	width = ewl_object_current_w_get(EWL_OBJECT(cursor));
	height = ewl_object_current_h_get(EWL_OBJECT(cursor));

	/*
	 * Find the widget setting the current cursor.
	 */
	parent = ewl_widget_focused_get();
	if (parent)
		parent = EWL_WIDGET(ewl_embed_widget_find(parent));

	if (parent)
		old = EWL_EMBED(parent)->cursor;

	if (cursor->handle)
		ewl_engine_pointer_free(EWL_EMBED(parent), cursor->handle);

	handle = ewl_engine_pointer_data_new(EWL_EMBED(parent),
			EWL_EMBED(cursor)->evas_window, width, height);

	/* FIXME: Needs to be done for all widgets with this cursor
	ewl_attach_mouse_cursor_set(parent, handle);
	if (EWL_EMBED(parent)->cursor == old)
		ewl_embed_mouse_cursor_set(entry);
	*/

	cursor->handle = handle;

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

static void
ewl_cursor_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Cursor *cursor = EWL_CURSOR(w);

	/* FIXME: Also needs to be handled for the correct engine refs
	if (cursor->handle)
		ewl_engine_pointer_free(parent, cursor->handle);
		*/
	cursor->handle = 0;
}

