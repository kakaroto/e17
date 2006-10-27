#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#if 0
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#endif

#define EWL_DND_WINDOW_ROOT 0

int EWL_CALLBACK_DND_POSITION = 0; /**< A DND position event **/
int EWL_CALLBACK_DND_ENTER = 0; /**< On enter of a widget **/
int EWL_CALLBACK_DND_LEAVE = 0; /**< On exit of a widget **/
int EWL_CALLBACK_DND_DROP = 0; /**< Drop event **/
int EWL_CALLBACK_DND_DATA = 0; /**< Data event **/

static int ewl_dragging_current = 0;
static int ewl_dnd_move_count = 0;
#if 0
static Ecore_Evas *ewl_dnd_drag_canvas;
static Ecore_X_Window ewl_dnd_drag_win = 0;
#endif

static Ewl_Widget *ewl_dnd_widget = NULL;

static Ecore_Hash *ewl_dnd_position_hash;
static Ecore_Hash *ewl_dnd_provided_hash;
static Ecore_Hash *ewl_dnd_accepted_hash;
static int ewl_dnd_status = 0;

Ecore_Event_Handler *ewl_dnd_mouse_up_handler;
Ecore_Event_Handler *ewl_dnd_mouse_move_handler;

static char *ewl_dnd_types_encode(const char **types);
static char **ewl_dnd_types_decode(const char *types);
static char * ewl_dnd_type_stpcpy(char *dst, const char *src);
static int ewl_dnd_types_encoded_contains(char *types, char *type);

#if 0
static int ewl_dnd_event_mouse_up(void *data, int type, void *event);
static int ewl_dnd_event_dnd_move(void *data, int type, void *event);
#endif

static void ewl_dnd_cb_render(Ewl_Widget *w, void *event, void *data);

/**
 * @internal
 * @return Returns TRUE if the DND system was successfully initialized,
 * FALSE otherwise
 * @brief Initialize the DND sybsystem
 */
int
ewl_dnd_init(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	EWL_CALLBACK_DND_POSITION = ewl_callback_type_add();
	EWL_CALLBACK_DND_ENTER = ewl_callback_type_add();
	EWL_CALLBACK_DND_LEAVE = ewl_callback_type_add();
	EWL_CALLBACK_DND_DROP = ewl_callback_type_add();
	EWL_CALLBACK_DND_DATA = ewl_callback_type_add();

	ewl_dnd_position_hash = ecore_hash_new(ecore_direct_hash, 
						ecore_direct_compare);
	if (!ewl_dnd_position_hash)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_dnd_provided_hash = ecore_hash_new(ecore_direct_hash, 
						ecore_direct_compare);
	if (!ewl_dnd_provided_hash) {
		ecore_hash_destroy(ewl_dnd_position_hash);
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_dnd_accepted_hash = ecore_hash_new(ecore_direct_hash, 
						ecore_direct_compare);
	if (!ewl_dnd_accepted_hash) {
		ecore_hash_destroy(ewl_dnd_provided_hash);
		ecore_hash_destroy(ewl_dnd_position_hash);
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_dragging_current = 0;
	ewl_dnd_status = 1;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value.
 * @brief Shuts down the EWL DND system
 */
void
ewl_dnd_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_hash_destroy(ewl_dnd_position_hash);
	ecore_hash_destroy(ewl_dnd_provided_hash);
	ecore_hash_destroy(ewl_dnd_accepted_hash);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to add
 * @return Returns no value
 * @brief: Adds the given widget @a w to the position hash
 */
void
ewl_dnd_position_windows_set(Ewl_Widget *w) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ecore_hash_set(ewl_dnd_position_hash, w, (void *)1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to set provided types
 * @param types: A NULL terminated array of mimetypes widget provides for DND
 * @return Returns no value
 * @brief: Sets the mimetypes the designated widget can provide for DND
 */
void
ewl_dnd_provided_types_set(Ewl_Widget *w, const char **types)
{
	char *type;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	type = ecore_hash_get(ewl_dnd_provided_hash, w);
	IF_FREE(type);

	if (types && *types) {
		type = ewl_dnd_types_encode(types);
		ecore_hash_set(ewl_dnd_provided_hash, w, type);
		ewl_object_flags_add(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_DND_SOURCE,
				EWL_FLAGS_PROPERTY_MASK);
	}
	else {
		type = ecore_hash_remove(ewl_dnd_provided_hash, w);
		IF_FREE(type);
		ewl_object_flags_remove(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_DND_SOURCE,
				EWL_FLAGS_PROPERTY_MASK);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to test for an provided type
 * @param type: The mimetype to test for provideance on a specific widget
 * @return Returns TRUE if the types contains the given type, FALSE otherwise
 * @brief: Verifies the specified widget provides the given mimetype
 */
int
ewl_dnd_provided_types_contains(Ewl_Widget *w, char *type)
{
	char *types;
	int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, FALSE);

	types = ecore_hash_get(ewl_dnd_provided_hash, w);
	if (types) ret = ewl_dnd_types_encoded_contains(types, type);

	DRETURN_INT(ret, DLEVEL_STABLE);
}


/**
 * @param w: The widget to retrieve provided types
 * @return Returns a NULL terminated array of mimetypes widget provides for DND
 * @brief: Gets the mimetypes the designated widget can provide for DND
 */
char **
ewl_dnd_provided_types_get(Ewl_Widget *w)
{
	const char *types;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	types = ecore_hash_get(ewl_dnd_provided_hash, w);
	DRETURN_PTR(ewl_dnd_types_decode(types), DLEVEL_STABLE);
}

/**
 * @param w: The widget to set accepted types
 * @param types: A NULL terminated array of mimetypes widget accepts for DND
 * @return Returns no value
 * @brief: Sets the mimetypes the designated widget can accept for DND
 */
void
ewl_dnd_accepted_types_set(Ewl_Widget *w, const char **types)
{
	char *type;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	type = ecore_hash_remove(ewl_dnd_accepted_hash, w);
	IF_FREE(type);

	if (types && *types) {
		type = ewl_dnd_types_encode(types);
		ecore_hash_set(ewl_dnd_accepted_hash, w, type);
		ewl_object_flags_add(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_DND_TARGET,
				EWL_FLAGS_PROPERTY_MASK);
		if (REALIZED(w) && !OBSCURED(w)) {
			Ewl_Embed *emb;

			emb = ewl_embed_widget_find(w);
			ewl_embed_dnd_aware_set(emb);
		}
	}
	else {
		ewl_object_flags_remove(EWL_OBJECT(w),
				EWL_FLAG_PROPERTY_DND_TARGET,
				EWL_FLAGS_PROPERTY_MASK);
		if (REALIZED(w) && !OBSCURED(w)) {
			Ewl_Embed *emb;

			emb = ewl_embed_widget_find(w);
			ewl_embed_dnd_aware_remove(emb);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to test for an accepted type
 * @param type: The mimetype to test for acceptance on a specific widget
 * @return Returns TRUE if the widget accepts the given type, FALSE otherwise
 * @brief: Verifies the specified widget accepts the given mimetype
 */
int
ewl_dnd_accepted_types_contains(Ewl_Widget *w, char *type)
{
	char *types;
	int ret = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, FALSE);

	types = ecore_hash_get(ewl_dnd_accepted_hash, w);
	if (types) ret = ewl_dnd_types_encoded_contains(types, type);

	DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: The widget to retrieve accepted types
 * @return Returns a NULL terminated array of mimetypes widget accepts for DND
 * @brief: Gets the mimetypes the designated widget can accept for DND
 */
const char **
ewl_dnd_accepted_types_get(Ewl_Widget *w)
{
	const char *types;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	types = ecore_hash_get(ewl_dnd_provided_hash, w);

	DRETURN_PTR(ewl_dnd_types_decode(types), DLEVEL_STABLE);
}


/**
 * @param w: The widget to start dragging
 * @return Returns no value
 * @brief Tells the widget to start dragging
 */
void
ewl_dnd_drag_start(Ewl_Widget *w) 
{
	Ewl_Widget *win;
	Ewl_Widget *image;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!ewl_dnd_status || ewl_dragging_current) 
		DRETURN(DLEVEL_STABLE);

	ewl_dragging_current = 1;
	ewl_dnd_widget = w;
	ewl_dnd_move_count = 0;

	win = ewl_window_new();
	ewl_embed_engine_name_set(EWL_EMBED(win), "buffer");
	ewl_callback_append(win, EWL_CALLBACK_VALUE_CHANGED, ewl_dnd_cb_render,
			ewl_embed_widget_find(w));
	ewl_widget_show(win);

	image = ewl_image_new();
	ewl_image_file_path_set(EWL_IMAGE(image), PACKAGE_DATA_DIR
			"/ewl/images/World.png");
	ewl_object_custom_size_set(EWL_OBJECT(image), 64, 64);
	ewl_container_child_append(EWL_CONTAINER(win), image);
	ewl_widget_show(image);

	/* Start the drag operation */
	/*
	ecore_x_dnd_types_set(ewl_dnd_drag_win, ewl_dnd_provided_types_get(w),
			1);
	ecore_x_dnd_begin(ewl_dnd_drag_win, NULL, 0);
	*/

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value
 * @brief Disables DND
 */
void
ewl_dnd_disable(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_status = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value
 * @brief Enables DND
 */
void
ewl_dnd_enable(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_status = 1;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the current DND status
 * @brief Retrieves the current DND status
 */
int
ewl_dnd_status_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(ewl_dnd_status, DLEVEL_STABLE);
}

/**
 * @return Returns the current DND widget
 * @brief Retrieves the current DND widget
 */
Ewl_Widget *
ewl_dnd_drag_widget_get(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_dnd_widget, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Clears the current DND widget
 */
void
ewl_dnd_drag_widget_clear(void) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_dnd_widget = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_dnd_cb_render(Ewl_Widget *w, void *event __UNUSED__, void *data)
{
	int handle;
	Ewl_Embed *embed = EWL_EMBED(data);
	Ewl_Embed *cursor_win = EWL_EMBED(w);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	handle = ewl_engine_pointer_data_new(embed, cursor_win->evas_window,
			ewl_object_current_w_get(EWL_OBJECT(cursor_win)),
			ewl_object_current_h_get(EWL_OBJECT(cursor_win)));
	ewl_engine_pointer_set(embed, handle);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_dnd_types_encoded_contains(char *types, char *type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	while (*types) {
		int len;

		len = strlen(types);
		if (!(strcmp(types, type)))
			DRETURN_INT(TRUE, DLEVEL_STABLE);
		types += len + 1;
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}


static char *
ewl_dnd_types_encode(const char **types)
{
	char *type, *tmptype;
	int count, i = 0;
	int len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Determine the length of all types.
	 */
	for (tmptype = (char *)types[0]; tmptype; tmptype = (char *)types[i]) {
		len += strlen(tmptype) + 1;
		i++;
	}

	type = tmptype = NEW(char, len + 1);
	count = i;
	for (i = 0; i < count; i++) {
		tmptype = ewl_dnd_type_stpcpy(tmptype, types[i]);
		tmptype++;
	}
	*tmptype = '\0';

	DRETURN_PTR(type, DLEVEL_STABLE);
}

static char **
ewl_dnd_types_decode(const char *types)
{
	int count;
	const char *tmp;
	char **list;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!types)
		DRETURN_PTR(types, DLEVEL_STABLE);

	/*
	 * Short lists so iterate over multiple times rather than incur
	 * allocation overhead.
	 */
	for (tmp = types, count = 0; *tmp; tmp++, count++) {
		while (*tmp) tmp++;
	}

	list = NEW(char *, count + 1);
	for (tmp = types, count = 0; *tmp; tmp++, count++) {
		list[count] = strdup(tmp);
		while (*tmp) tmp++;
	}

	DRETURN_PTR(list, DLEVEL_STABLE);
}

static char *
ewl_dnd_type_stpcpy(char *dst, const char *src)
{
	while (*src) {
		*dst = *src;
		dst++;
		src++;
	}
	*dst = '\0';

	return dst;
}

#if 0
static int
ewl_dnd_event_dnd_move(void *data __UNUSED__, int type __UNUSED__, 
							void *event)
{
	Ecore_X_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("event", event, FALSE);

	ev = event;

	if (!ewl_dnd_status) DRETURN_INT(TRUE, DLEVEL_STABLE);

	ewl_dnd_move_count++;
	if (ewl_dnd_move_count == 1) 
		ecore_evas_show(ewl_dnd_drag_canvas);

	if (ewl_dnd_drag_canvas) 
		ecore_evas_move(ewl_dnd_drag_canvas, ev->x - 15, ev->y - 15);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_dnd_event_mouse_up(void *data __UNUSED__, int type __UNUSED__, 
						void *event __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_dnd_drag_canvas && ewl_dragging_current) {
		Ecore_List *pos;
		void *val;

		ecore_x_pointer_ungrab();
		ecore_x_keyboard_ungrab();

		ecore_event_handler_del(ewl_dnd_mouse_up_handler);
		ecore_event_handler_del(ewl_dnd_mouse_move_handler);

		ecore_evas_free(ewl_dnd_drag_canvas);
		ewl_dnd_drag_canvas = NULL;
		ecore_x_window_del(ewl_dnd_drag_win);
		ecore_x_dnd_drop();

		/* Kill all last position references so they don't get
		 * carried over to the next drag */
		pos = ecore_hash_keys(ewl_dnd_position_hash);
		ecore_list_goto_first(pos);
		while ((val = ecore_list_remove_first(pos))) {
			EWL_EMBED(val)->dnd_last_position = NULL;
			ecore_hash_remove(ewl_dnd_position_hash, val);
		}
		ecore_list_destroy(pos);

		ewl_dragging_current = 0;
		ewl_widget_dnd_reset();
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}
#endif

