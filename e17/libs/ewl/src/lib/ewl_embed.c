#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

extern Ewl_Widget     *last_selected;
extern Ewl_Widget     *last_key;
extern Ewl_Widget     *last_focused;
extern Ewl_Widget     *dnd_widget;

Ecore_List       *ewl_embed_list = NULL;
Evas_Smart       *embedded_smart = NULL;

static void ewl_embed_smart_add_cb(Evas_Object *obj);
static void ewl_embed_smart_del_cb(Evas_Object *obj);
static void ewl_embed_smart_layer_set_cb(Evas_Object *obj, int l);
static void ewl_embed_smart_layer_adjust_cb(Evas_Object *obj);
static void ewl_embed_smart_layer_adjust_rel_cb(Evas_Object *obj,
						Evas_Object *above);
static void ewl_embed_smart_move_cb(Evas_Object *obj, Evas_Coord x,
				    Evas_Coord y);
static void ewl_embed_smart_resize_cb(Evas_Object *obj, Evas_Coord w,
				      Evas_Coord h);
static void ewl_embed_smart_show_cb(Evas_Object *obj);
static void ewl_embed_smart_hide_cb(Evas_Object *obj);
static void ewl_embed_smart_color_set_cb(Evas_Object *obj, int r, int g, int b,
					 int a);
static void ewl_embed_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip);
static void ewl_embed_smart_clip_unset_cb(Evas_Object *obj);

/*
 * Catch mouse events processed through the evas
 */
static void ewl_embed_evas_mouse_in_cb(void *data, Evas *e, Evas_Object *obj,
				       void *event_info);
static void ewl_embed_evas_mouse_out_cb(void *data, Evas *e, Evas_Object *obj,
					void *event_info);
static void ewl_embed_evas_mouse_down_cb(void *data, Evas *e, Evas_Object *obj,
					 void *event_info);
static void ewl_embed_evas_mouse_up_cb(void *data, Evas *e, Evas_Object *obj,
				       void *event_info);
static void ewl_embed_evas_mouse_move_cb(void *data, Evas *e, Evas_Object *obj,
					 void *event_info);
static void ewl_embed_evas_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj,
					  void *event_info);

/*
 * Catch key events processed through the evas
 */
static void ewl_embed_evas_key_down_cb(void *data, Evas *e, Evas_Object *obj,
				       void *event_info);
static void ewl_embed_evas_key_up_cb(void *data, Evas *e, Evas_Object *obj,
				     void *event_info);

static void strupper(char *str);

/**
 * @return Returns a new embed on success, or NULL on failure.
 * @brief Allocate and initialize a new embed
 */
Ewl_Widget *ewl_embed_new()
{
	Ewl_Embed *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Embed, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_embed_init(w)) {
		ewl_widget_destroy(EWL_WIDGET(w));
		w = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(w), DLEVEL_STABLE);
}

/**
 * @param w: the embed to be initialized to default values and callbacks
 * @return Returns TRUE or FALSE depending on if initialization succeeds.
 * @brief initialize a embed to default values and callbacks
 *
 * Sets the values and callbacks of a embed @a w to their defaults.
 */
int ewl_embed_init(Ewl_Embed * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Initialize the fields of the inherited container class
	 */
	if (!ewl_overlay_init(EWL_OVERLAY(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	ewl_widget_appearance_set(EWL_WIDGET(w), "embed");
	ewl_widget_inherit(EWL_WIDGET(w), "embed");

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_object_toplevel_set(EWL_OBJECT(w), EWL_FLAG_PROPERTY_TOPLEVEL);

	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			     ewl_embed_realize_cb, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_UNREALIZE,
			     ewl_embed_unrealize_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			     ewl_embed_destroy_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			     ewl_embed_configure_cb, NULL);

	w->max_layer = LAYER(w) = -1000;

	ecore_list_append(ewl_embed_list, w);

	w->tab_order = ecore_list_new();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param emb: the embedded container to change the target evas
 * @param evas: the new evas to draw the container and it's contents
 * @param evas_window: the window containing the evas, for event dispatching
 * @return Returns an evas smart object on success, NULL on failure.
 * @brief Change the evas used by the embedded container
 *
 * The returned smart object can be used to manipulate the area used by EWL
 * through standard evas functions.
 */
Evas_Object *
ewl_embed_evas_set(Ewl_Embed *emb, Evas *evas, Ewl_Embed_Evas_Window *evas_window)
{
	Ewl_Widget *w;
	Ecore_List   *paths;
	char       *font_path;
	char *name = "EWL Embedded Smart Object";

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("emb", emb, NULL);
	DCHECK_PARAM_PTR_RET("evas", evas, NULL);

	emb->evas = evas;
	emb->evas_window = evas_window;

	if (!embedded_smart) {
		embedded_smart = evas_smart_new(name, ewl_embed_smart_add_cb,
			ewl_embed_smart_del_cb, ewl_embed_smart_layer_set_cb,
			ewl_embed_smart_layer_adjust_cb,
			ewl_embed_smart_layer_adjust_cb,
			ewl_embed_smart_layer_adjust_rel_cb,
			ewl_embed_smart_layer_adjust_rel_cb,
			ewl_embed_smart_move_cb,
			ewl_embed_smart_resize_cb, ewl_embed_smart_show_cb,
			ewl_embed_smart_hide_cb, ewl_embed_smart_color_set_cb,
			ewl_embed_smart_clip_set_cb,
			ewl_embed_smart_clip_unset_cb, NULL);
	}

	if (emb->smart) {
		ewl_evas_object_destroy(emb->smart);
		emb->smart = NULL;
	}

	emb->smart = evas_object_smart_add(emb->evas, embedded_smart);
	evas_object_smart_data_set(emb->smart, emb);

	w = EWL_WIDGET(emb);

	if (VISIBLE(w))
		ewl_realize_request(w);

	paths = ewl_theme_font_path_get();
	ecore_list_goto_first(paths);
	while ((font_path = ecore_list_next(paths))) {
		evas_font_path_append(evas, font_path);
	}

	DRETURN_PTR(emb->smart, DLEVEL_STABLE);
}

/**
 * @param embed: the embed to inidicate if it takes focus
 * @param f: boolean to determine if the embed takes focus
 * @return Returns no value.
 * @brief Sets the boolean flag in the embed to determine if it takes focus.
 */
void ewl_embed_focus_set(Ewl_Embed *embed, int f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	embed->focus = f;
	if (embed->ev_clip)
		evas_object_focus_set(embed->ev_clip, f);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to indicate if it takes focus
 * @return Returns a boolean indicating if the embed takes focus.
 * @brief Retrieve the boolean value that indicates if the emebd takes focus.
 */
int ewl_embed_focus_get(Ewl_Embed *embed)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT(embed->focus, DLEVEL_STABLE);
}

/**
 * @param embed: the embed to retrieve maximum layer
 * @return Returns the layer used for obtaining evas events.
 * @brief Retrieve the layer being used for receiving evas events.
 */
int ewl_embed_max_layer_get(Ewl_Embed *embed)
{
	int layer;
	DENTER_FUNCTION(DLEVEL_STABLE);

	layer = embed->max_layer;

	DRETURN_INT(layer, DLEVEL_STABLE);
}

/**
 *
 * @param embed: embed to set the maximum layer
 * @param layer: the maximum layer used for handling evas events
 * @return Returns no value.
 * @brief Sets the layer for the embed to receive events.
 */
void ewl_embed_max_layer_set(Ewl_Embed *embed, int layer)
{
	if (REALIZED(embed))
		evas_object_layer_set(embed->ev_clip, layer);
	embed->max_layer = layer;
}

/**
 * @param embed: the embed where the key event is to occur
 * @param keyname: the key press to trigger
 * @param mods: the mask of key modifiers currently pressed
 * @return Returns no value.
 * @brief Sends the event for a key press into an embed.
 */
void
ewl_embed_key_down_feed(Ewl_Embed *embed, char *keyname, unsigned int mods)
{
	Ewl_Widget *temp;
	Ewl_Event_Key_Down ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_PARAM_PTR("keyname", keyname);

	ev.modifiers = mods;
	ev.keyname = strdup(keyname);

	/*
	 * If a widget has been selected then we send the keystroke to the
	 * appropriate widget.
	 */
	if (!last_key || !ewl_container_parent_of(EWL_WIDGET(embed),
				last_key)) {
		if (last_selected)
			last_key = last_selected;
		else
			last_key = EWL_WIDGET(embed);
	}

	/*
	 * Dispatcher of key down events, these get sent to the last widget
	 * selected, and every parent above it.
	 */
	temp = last_key;
	while (temp) {
		if (!(ewl_object_state_has(EWL_OBJECT(temp),
					EWL_FLAG_STATE_DISABLED)))
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_KEY_DOWN, &ev);
		temp = temp->parent;
	}

	FREE(ev.keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param embed: the embed where the key event is to occur
 * @param keyname: the key release to trigger
 * @param mods: the mask of key modifiers currently pressed
 * @return Returns no value.
 * @brief Sends the event for a key release into an embed.
 */
void ewl_embed_key_up_feed(Ewl_Embed *embed, char *keyname, unsigned int mods)
{
	Ewl_Widget *temp;
	Ewl_Event_Key_Up ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_PARAM_PTR("keyname", keyname);

	ev.modifiers = mods;
	ev.keyname = strdup(keyname);

	/*
	 * Dispatcher of key up events, these get sent to the last widget
	 * selected, and every parent above it.
	 */
	temp = last_key;
	while (temp) {
		if (!(ewl_object_state_has(EWL_OBJECT(temp),
					EWL_FLAG_STATE_DISABLED)))
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_KEY_UP, &ev);
		temp = temp->parent;
	}

	FREE(ev.keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param embed: the embed where the mouse event is to occur
 * @param b: the number of the button pressed
 * @param clicks: the number of times the button was pressed
 * @param x: the x coordinate of the mouse press
 * @param y: the y coordinate of the mouse press
 * @param mods: the mask of key modifiers currently pressed
 * @return Returns no value.
 * @brief Sends the event for a mouse button press into an embed.
 */
void
ewl_embed_mouse_down_feed(Ewl_Embed *embed, int b, int clicks, int x, int y,
			  unsigned int mods)
{
	Ewl_Event_Mouse_Down ev;
	Ewl_Widget *temp = NULL;
	Ewl_Widget *widget = NULL;
	Ewl_Widget *deselect = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);

	widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed),
			x, y);
	if (!widget)
		widget = EWL_WIDGET(embed);

	/*
	 * Save the last selected widget for further reference, do this prior
	 * to triggering the callback to avoid funkiness if the callback
	 * causes the widget to be destroyed.
	 */
	deselect = last_selected;
	last_key = last_selected = widget;

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;
	ev.button = b;
	ev.clicks = clicks;

	/*
	 * While the mouse is down the widget has a pressed state, the widget
	 * and its parents are notified in this change of state. Send the
	 * click events prior to the selection events to allow containers to
	 * take different actions depending on child state.
	 */
	temp = widget;
	while (temp) {
		if (!(ewl_object_state_has(EWL_OBJECT(temp),
					EWL_FLAG_STATE_DISABLED))) {
			ewl_object_state_add(EWL_OBJECT(temp),
					EWL_FLAG_STATE_PRESSED);
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_MOUSE_DOWN, &ev);

			if (ev.clicks > 1) {
				ewl_callback_call_with_event_data(temp,
						EWL_CALLBACK_DOUBLE_CLICKED,
						&ev);
			}
		}
		temp = temp->parent;
	}

	/*
	 * Determine whether this widget has already been selected, if not,
	 * deselect the previously selected widget and notify it of the
	 * change. Then select the new widget and notify it of the selection.
	 */
	if (widget != deselect) {
		if (deselect) {
			ewl_object_state_remove(EWL_OBJECT(deselect),
					EWL_FLAG_STATE_SELECTED);
			ewl_callback_call(deselect, EWL_CALLBACK_DESELECT);
		}

		if (widget && !(ewl_object_state_has(EWL_OBJECT(widget),
					EWL_FLAG_STATE_DISABLED))) {
			ewl_object_state_add(EWL_OBJECT(widget),
					EWL_FLAG_STATE_SELECTED);
			ewl_callback_call(widget, EWL_CALLBACK_SELECT);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param embed: the embed where the mouse event is to occur
 * @param b: the number of the button released
 * @param x: the x coordinate of the mouse release
 * @param y: the y coordinate of the mouse release
 * @param mods: the mask of key modifiers currently release
 * @return Returns no value.
 * @brief Sends the event for a mouse button release into an embed.
 */
void
ewl_embed_mouse_up_feed(Ewl_Embed *embed, int b, int x, int y,
			unsigned int mods)
{
	Ewl_Widget *temp;
	Ewl_Event_Mouse_Up ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;
	ev.button = b;

	/*
	 * When the mouse is released the widget no longer has a pressed state,
	 * the widget and its parents are notified in this change of state.
	 */
	temp = last_selected;
	while (temp) {
		if (!(ewl_object_state_has(EWL_OBJECT(temp),
				EWL_FLAG_STATE_DISABLED))) {
			ewl_object_state_remove(EWL_OBJECT(temp),
					EWL_FLAG_STATE_PRESSED);
			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_MOUSE_UP, &ev);

		}

		temp = temp->parent;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param embed: the embed where the mouse event is to occur
 * @param x: the x coordinate of the mouse move
 * @param y: the y coordinate of the mouse move
 * @param mods: the mask of key modifiers currently release
 * @return Returns no value.
 * @brief Sends the event for a mouse button release into an embed.
 */
void
ewl_embed_mouse_move_feed(Ewl_Embed *embed, int x, int y, unsigned int mods)
{
	Ewl_Widget *widget;
	Ewl_Event_Mouse_Move ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);

	widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed),
			x, y);
	if (!widget)
		widget = EWL_WIDGET(embed);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;

	/*
	 * Defocus all widgets up to the level of a shared parent of old and
	 * newly focused widgets.
	 */
	while (last_focused && (widget != last_focused) &&
			!ewl_container_parent_of(last_focused, widget)) {
		ewl_object_state_remove(EWL_OBJECT(last_focused),
				EWL_FLAG_STATE_HILITED);
		ewl_callback_call(last_focused, EWL_CALLBACK_FOCUS_OUT);
		last_focused = last_focused->parent;
	}

	/*
	 * Pass out the movement event up the chain, allows parents to
	 * react to mouse movement in their children.
	 */
	last_focused = widget;
	while (last_focused) {

		if (!(ewl_object_state_has(EWL_OBJECT(last_focused),
					EWL_FLAG_STATE_DISABLED))) {

			/*
			 * First mouse move event in a widget marks it focused.
			 */
			if (!(ewl_object_state_has(EWL_OBJECT(last_focused),
						EWL_FLAG_STATE_HILITED))) {
				ewl_object_state_add(EWL_OBJECT(last_focused),
						EWL_FLAG_STATE_HILITED);
				ewl_callback_call_with_event_data(last_focused,
						EWL_CALLBACK_FOCUS_IN, &ev);
			}

			ewl_callback_call_with_event_data(last_focused,
					EWL_CALLBACK_MOUSE_MOVE, &ev);
		}
		last_focused = last_focused->parent;
	}

	last_focused = widget;

	if (dnd_widget && ewl_object_state_has(EWL_OBJECT(dnd_widget),
				EWL_FLAG_STATE_DND))
		ewl_callback_call_with_event_data(dnd_widget,
						  EWL_CALLBACK_MOUSE_MOVE, &ev);

	if (last_selected && ewl_object_state_has(EWL_OBJECT(last_selected),
				EWL_FLAG_STATE_PRESSED))
		ewl_callback_call_with_event_data(last_selected,
						  EWL_CALLBACK_MOUSE_MOVE, &ev);
	else
		dnd_widget = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the mouse event is to occur
 * @param x: the x coordinate of the mouse out
 * @param y: the y coordinate of the mouse out
 * @param mods: the mask of key modifiers currently release
 * @return Returns no value.
 * @brief Sends a mouse out event to the last focused widget
 */
void
ewl_embed_mouse_out_feed(Ewl_Embed *embed __UNUSED__, int x, int y,
						unsigned int mods)
{
	Ewl_Event_Mouse_Out ev;
	DENTER_FUNCTION(DLEVEL_STABLE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;

	while (last_focused) {
		ewl_callback_call_with_event_data(last_focused,
						  EWL_CALLBACK_FOCUS_OUT, &ev);
		last_focused = last_focused->parent;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the mouse event is to occur
 * @param x: the x coordinate of the mouse when the wheel event occurred
 * @param y: the y coordinate of the mouse when wheel event occurred
 * @param z: the direction the mouse wheel rotated
 * @param dir: FIXME, not sure what this does, it's always 0
 * @param mods: the mask of key modifiers currently release
 * @return Returns no value.
 * @brief Sends a mouse out event to the last focused widget
 */
void
ewl_embed_mouse_wheel_feed(Ewl_Embed *embed, int x, int y, int z, int dir, unsigned int mods)
{
	Ewl_Widget *w;
	Ewl_Event_Mouse_Wheel ev;
	DENTER_FUNCTION(DLEVEL_STABLE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;
	ev.z = z;
	ev.dir = dir;

	w = last_focused;
	if (!w) {
		ewl_callback_call_with_event_data(EWL_WIDGET(embed),
						  EWL_CALLBACK_MOUSE_WHEEL,
						  &ev);
	}

	while (w) {
		ewl_callback_call_with_event_data(w, EWL_CALLBACK_MOUSE_WHEEL,
						  &ev);
		w = w->parent;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param path: the font path to add to the embeds
 * @return Returns no value.
 * @brief Add a font path to all embeds after realized
 *
 * Adds the search path to the evases created in the embeds. Using
 * ewl_theme_font_path_add is preferred.
 */
void ewl_embed_font_path_add(char *path)
{
	Ewl_Embed *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("path", path);

	ecore_list_goto_first(ewl_embed_list);
	while ((e = ecore_list_next(ewl_embed_list)))
		if (REALIZED(e))
			evas_font_path_append(e->evas, path);

	ecore_list_append(ewl_theme_font_path_get(), strdup(path));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param window: the evas window to search for on the list of embeds
 * @return Returns the found embed on success, NULL on failure.
 * @brief Find an ewl embed by its evas window
 */
Ewl_Embed      *ewl_embed_evas_window_find(Ewl_Embed_Evas_Window *window)
{
	Ewl_Embed      *retemb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ecore_list_goto_first(ewl_embed_list);

	while ((retemb = ecore_list_next(ewl_embed_list)) != NULL) {
		if (retemb->evas_window == window)
			DRETURN_PTR(retemb, DLEVEL_STABLE);
	}

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param w: the widget to search for its embed
 * @return Returns the found embed on success, NULL on failure.
 * @brief Find an ewl embed by a widget inside
 */
Ewl_Embed     *ewl_embed_widget_find(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	while (w->parent)
		w = w->parent;

	if (!ewl_object_toplevel_get(EWL_OBJECT(w)))
		w = NULL;

	DRETURN_PTR(EWL_EMBED(w), DLEVEL_STABLE);
}

/**
 * @param e: the embed that holds widgets to change tab order
 * @param w: the widget that will be moved to the front of the tab order list
 * @return Returns no value.
 * @brief Moves the widget @a w to the front of the tab order list.
 */
void ewl_embed_tab_order_push(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);

	if (!ewl_container_parent_of(EWL_WIDGET(e), w))
		DRETURN(DLEVEL_STABLE);

	if (ecore_list_goto(e->tab_order, w))
		ecore_list_remove(e->tab_order);

	ecore_list_prepend(e->tab_order, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed containing a widget to remove from the tab order
 * @param w: the widget to remove from the tab order list
 * @return Returns no value.
 * @brief Removes the widget @a w from the tab order list for @a e.
 */
void ewl_embed_tab_order_remove(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);

	if (ecore_list_goto(e->tab_order, w))
		ecore_list_remove(e->tab_order);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to change focus of it's contained widgets
 * @return Returns no value.
 * @brief Changes focus to the next widget in the circular tab order list.
 */
void ewl_embed_tab_order_next(Ewl_Embed *e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (!(w = ecore_list_next(e->tab_order))) {
		ecore_list_goto_first(e->tab_order);
		w = ecore_list_next(e->tab_order);
	}

	if (w)
		ewl_widget_focus_send(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to retrieve coord screen position
 * @param xx: the x coord to map to a screen position
 * @param yy: the y coord to map to a screen position
 * @param x: storage for the mapped screen x position
 * @param y: storage for the mapped screen y position
 * @return Returns no value.
 * @brief Maps coordinates from the Evas to screen coordinates
 */
void ewl_embed_coord_to_screen(Ewl_Embed *e, int xx, int yy, int *x, int *y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->evas) {
		if (x)
			*x = (int)(evas_coord_world_x_to_screen(e->evas,
							(Evas_Coord)(xx)));
		if (y)
			*y = (int)(evas_coord_world_y_to_screen(e->evas,
							(Evas_Coord)(yy)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb = EWL_EMBED(w);
	DENTER_FUNCTION(DLEVEL_STABLE);

	emb->ev_clip = evas_object_rectangle_add(emb->evas);

	if (w->fx_clip_box)
		evas_object_clip_set(emb->smart, w->fx_clip_box);

	if (emb->ev_clip) {
		/*
		if (w->fx_clip_box)
			evas_object_clip_set(w->fx_clip_box, emb->ev_clip);
			*/
		evas_object_color_set(emb->ev_clip, 0, 0, 0, 0);
		evas_object_show(emb->ev_clip);
		evas_object_repeat_events_set(emb->ev_clip, FALSE);
		evas_object_focus_set(emb->ev_clip, emb->focus);

		/*
		 * Catch mouse events processed through the evas
		 */
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_IN,
				ewl_embed_evas_mouse_in_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_OUT,
				ewl_embed_evas_mouse_out_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_DOWN,
				ewl_embed_evas_mouse_down_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_UP,
				ewl_embed_evas_mouse_up_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_MOVE,
				ewl_embed_evas_mouse_move_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_MOUSE_WHEEL,
				ewl_embed_evas_mouse_wheel_cb, emb);

		/*
		 * Catch key events processed through the evas
		 */
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_KEY_DOWN,
				ewl_embed_evas_key_down_cb, emb);
		evas_object_event_callback_add(emb->ev_clip,
				EVAS_CALLBACK_KEY_UP, ewl_embed_evas_key_up_cb,
				emb);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_unrealize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (EWL_EMBED(w)->smart) {
		evas_object_smart_data_set(EWL_EMBED(w)->smart, NULL);
		ewl_evas_object_destroy(EWL_EMBED(w)->smart);
		EWL_EMBED(w)->smart = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Embed      *emb = EWL_EMBED(w);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (emb->ev_clip) {
		evas_object_move(emb->ev_clip, (Evas_Coord)(CURRENT_X(w)),
				 (Evas_Coord)(CURRENT_Y(w)));
		evas_object_resize(emb->ev_clip, (Evas_Coord)(CURRENT_W(w)),
				   (Evas_Coord)(CURRENT_H(w)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = EWL_EMBED(w);

	if (ecore_list_goto(ewl_embed_list, w))
		ecore_list_remove(ewl_embed_list);

	ecore_list_destroy(emb->tab_order);
	emb->tab_order = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_add_cb(Evas_Object *obj __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Nothing to see here! Move along...
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_del_cb(Evas_Object *obj)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb) {
		emb->smart = NULL;
		ewl_widget_unrealize(EWL_WIDGET(emb));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_layer_set_cb(Evas_Object *obj, int l)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_widget_layer_set(EWL_WIDGET(emb), l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_layer_adjust_cb(Evas_Object *obj)
{
	int l;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	l = evas_object_layer_get(obj);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_widget_layer_set(EWL_WIDGET(emb), l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_layer_adjust_rel_cb(Evas_Object *obj, 
					Evas_Object *rel __UNUSED__)
{
	int l;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb) {
		l = evas_object_layer_get(obj);
		ewl_widget_layer_set(EWL_WIDGET(emb), l);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_object_position_request(EWL_OBJECT(emb), (int)(x),
					    (int)(y));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_object_size_request(EWL_OBJECT(emb), (int)(w), (int)(h));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_show_cb(Evas_Object *obj)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_widget_show(EWL_WIDGET(emb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_hide_cb(Evas_Object *obj)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_widget_hide(EWL_WIDGET(emb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_color_set_cb(Evas_Object *obj __UNUSED__, int r __UNUSED__, 
				int g __UNUSED__, int b __UNUSED__, 
				int a __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Nothing to see here! Move along...
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
{
	Ewl_Embed *emb;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	w = EWL_WIDGET(emb);
	if (emb && w->fx_clip_box && clip != w->fx_clip_box)
		evas_object_clip_set(EWL_WIDGET(emb)->fx_clip_box, clip);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_clip_unset_cb(Evas_Object *obj)
{
	Ewl_Embed *emb;
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	w = EWL_WIDGET(emb);
	if (emb && w->fx_clip_box)
		evas_object_clip_unset(EWL_WIDGET(emb)->fx_clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_in_cb(void *data __UNUSED__, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__,
				void *event_info __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * As we ignore this for the windows, may as well ignore it here.
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_out_cb(void *data, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Mouse_Out *ev = event_info;

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	ewl_embed_mouse_out_feed(embed, ev->canvas.x, ev->canvas.y,
				 ewl_ev_modifiers_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_down_cb(void *data, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Mouse_Down *ev = event_info;

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	ewl_embed_mouse_down_feed(embed, ev->button, 1, ev->canvas.x,
				  ev->canvas.y, ewl_ev_modifiers_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_up_cb(void *data, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Mouse_Up *ev = event_info;

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	ewl_embed_mouse_up_feed(embed, ev->button, ev->canvas.x,
				  ev->canvas.y, ewl_ev_modifiers_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_move_cb(void *data, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Mouse_Move *ev = event_info;

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	ewl_embed_mouse_move_feed(embed, ev->cur.canvas.x, ev->cur.canvas.y,
				  ewl_ev_modifiers_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_wheel_cb(void *data __UNUSED__, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__,
				void *event_info __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_key_down_cb(void *data, Evas *e __UNUSED__, 
			Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Key_Down *ev = event_info;
	unsigned int key_modifiers = 0;
	char *keyname = strdup(ev->keyname);

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	if (evas_key_modifier_is_set(ev->modifiers, "Shift"))
		key_modifiers |= EWL_KEY_MODIFIER_SHIFT;
	else if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
		key_modifiers |= EWL_KEY_MODIFIER_ALT;
	else if (evas_key_modifier_is_set(ev->modifiers, "Control"))
		key_modifiers |= EWL_KEY_MODIFIER_CTRL;
	else if (evas_key_modifier_is_set(ev->modifiers, "Meta"))
		key_modifiers |= EWL_KEY_MODIFIER_MOD;
	else if (evas_key_modifier_is_set(ev->modifiers, "Super"))
		key_modifiers |= EWL_KEY_MODIFIER_WIN;
	else if (evas_key_modifier_is_set(ev->modifiers, "Hyper"))
		key_modifiers |= EWL_KEY_MODIFIER_WIN;

	ewl_ev_modifiers_set(key_modifiers);

	/* fixup the space char */
	if (!strncmp(keyname, "space", 5)) {
		free(keyname);
		keyname = strdup(" ");

	/* fixup the return char */
	} else if (!strncmp(keyname, "Return", 6)) {
		free(keyname);
		keyname = strdup("\n");

	/* fixup upper case chars */
	} else if (key_modifiers & EWL_KEY_MODIFIER_SHIFT)
		strupper(keyname);

	ewl_embed_key_down_feed(embed, keyname, ewl_ev_modifiers_get());
	free(keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_key_up_cb(void *data, Evas *e __UNUSED__, 
			Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Key_Down *ev = event_info;
	unsigned int key_modifiers = 0;
	char *keyname = strdup(ev->keyname);

	DENTER_FUNCTION(DLEVEL_STABLE);

	embed = data;
	key_modifiers = ewl_ev_modifiers_get();
	if (!evas_key_modifier_is_set(ev->modifiers, "Shift"))
		key_modifiers &= ~EWL_KEY_MODIFIER_SHIFT;
	else if (!evas_key_modifier_is_set(ev->modifiers, "Alt"))
		key_modifiers &= ~EWL_KEY_MODIFIER_ALT;
	else if (!evas_key_modifier_is_set(ev->modifiers, "Control"))
		key_modifiers &= ~EWL_KEY_MODIFIER_CTRL;
	else if (!evas_key_modifier_is_set(ev->modifiers, "Meta"))
		key_modifiers &= ~EWL_KEY_MODIFIER_MOD;
	else if (!evas_key_modifier_is_set(ev->modifiers, "Super"))
		key_modifiers &= ~EWL_KEY_MODIFIER_WIN;
	else if (!evas_key_modifier_is_set(ev->modifiers, "Hyper"))
		key_modifiers &= ~EWL_KEY_MODIFIER_WIN;

	ewl_ev_modifiers_set(key_modifiers);

	/* fixup the space char */
	if (!strncmp(keyname, "space", 5)) {
		free(keyname);
		keyname = strdup(" ");

	/* fixup the return char */
	} else if (!strncmp(keyname, "Return", 6)) {
		free(keyname);
		keyname = strdup("\n");

	/* fixup upper case chars */
	} else if (key_modifiers & EWL_KEY_MODIFIER_SHIFT) 
		strupper(keyname);

	ewl_embed_key_up_feed(embed, keyname, ewl_ev_modifiers_get());
	free(keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * Uppercase the given string
 */
static void
strupper(char *str)
{
	char *i;
	for(i = str; *i != '\0'; i++) 
		*i = toupper(*i);
}

