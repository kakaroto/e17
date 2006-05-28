#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

Ecore_List *ewl_embed_list = NULL;
static Evas_Smart *embedded_smart = NULL;
static Ewl_Embed *ewl_embed_active_embed = NULL;

static void ewl_embed_smart_add_cb(Evas_Object *obj);
static void ewl_embed_smart_del_cb(Evas_Object *obj);
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

static void ewl_embed_tab_order_change(Ewl_Embed *e, 
					 void *(*change)(Ecore_DList *list),
					 void *(*cycle)(Ecore_DList *list));

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
Ewl_Widget *
ewl_embed_new(void)
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
int
ewl_embed_init(Ewl_Embed *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Initialize the fields of the inherited container class
	 */
	if (!ewl_overlay_init(EWL_OVERLAY(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	ewl_widget_appearance_set(EWL_WIDGET(w), EWL_EMBED_TYPE);
	ewl_widget_inherit(EWL_WIDGET(w), EWL_EMBED_TYPE);

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
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_FOCUS_OUT,
			     ewl_embed_focus_out_cb, NULL);

	ecore_list_append(ewl_embed_list, w);

	w->tab_order = ecore_dlist_new();
	w->obj_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @brief Shutdown the embed 
 */
void
ewl_embed_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (embedded_smart) {
		evas_smart_free(embedded_smart);
		embedded_smart = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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
	Ecore_List *paths;
	char *font_path;
	char *name = "EWL Embedded Smart Object";

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("emb", emb, NULL);
	DCHECK_PARAM_PTR_RET("evas", evas, NULL);
	DCHECK_TYPE_RET("emb", emb, EWL_EMBED_TYPE, NULL);

	emb->evas = evas;
	emb->evas_window = evas_window;

	if (!embedded_smart) {
		embedded_smart = evas_smart_new(name,
			ewl_embed_smart_add_cb,
			ewl_embed_smart_del_cb,
			NULL, NULL, NULL, NULL, NULL,
			ewl_embed_smart_move_cb,
			ewl_embed_smart_resize_cb,
			ewl_embed_smart_show_cb,
			ewl_embed_smart_hide_cb,
			ewl_embed_smart_color_set_cb,
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
void
ewl_embed_focus_set(Ewl_Embed *embed, int f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	embed->focus = f;
	if (embed->smart)
		evas_object_focus_set(embed->smart, f);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to indicate if it takes focus
 * @return Returns a boolean indicating if the embed takes focus.
 * @brief Retrieve the boolean value that indicates if the emebd takes focus.
 */
int
ewl_embed_focus_get(Ewl_Embed *embed)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("embed", embed, FALSE);
	DCHECK_TYPE_RET("embed", embed, EWL_EMBED_TYPE, FALSE);

	DRETURN_INT(embed->focus, DLEVEL_STABLE);
}

/**
 * @param embed: The embed to set the active value on
 * @param act: The active value to set
 * @return Returns no value.
 * @brief Sets if the given embed @p embed is active or not
 */
void
ewl_embed_active_set(Ewl_Embed *embed, unsigned int act)
{
	Ewl_Embed *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	/* return if we're seting active and we're already the active embed */
	if (act && (embed == ewl_embed_active_embed))
		DRETURN(DLEVEL_STABLE);

	if (!act)
	{
		/* if you said false and we aren't actually the active embed
		 * just exit */
		if (embed != ewl_embed_active_embed)
			DRETURN(DLEVEL_STABLE);

		e = embed;
		ewl_embed_active_embed = NULL;
	}
	else
	{
		e = ewl_embed_active_embed;	
		ewl_embed_active_embed = embed;
	}

	if (e && e->last.clicked)
	{
		Ewl_Widget *temp;

		ewl_object_state_remove(EWL_OBJECT(e->last.clicked),
						EWL_FLAG_STATE_FOCUSED);
		ewl_object_state_remove(EWL_OBJECT(e->last.clicked),
						EWL_FLAG_STATE_PRESSED);
		
		ewl_callback_call(e->last.clicked, EWL_CALLBACK_FOCUS_OUT);

		/* Clean the last.clicked up recursively.. */
		temp = e->last.clicked;
		while (temp) {
			if (!DISABLED(temp))
				ewl_object_state_remove(EWL_OBJECT(temp),
							EWL_FLAG_STATE_PRESSED);
			temp = temp->parent;
		}

		e->last.clicked = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the currently active embed
 * @brief Returns the currently active embed widget
 */
Ewl_Embed *
ewl_embed_active_embed_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_embed_active_embed, DLEVEL_STABLE);
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
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	/*
	 * check if this is a focus change key press and we are not ignoring
	 * focus change events
	 */
	if ((!(embed->last.focused 
			&& ewl_widget_ignore_focus_change_get(embed->last.focused))) 
				&& (!strcmp(keyname, "Tab")))
	{
		if (mods & EWL_KEY_MODIFIER_SHIFT)
			ewl_embed_tab_order_previous(embed);
		else
			ewl_embed_tab_order_next(embed);

		DRETURN(DLEVEL_STABLE);
	}

	/* 
	 * setup the event struct 
	 */
	ev.modifiers = mods;
	ev.keyname = strdup(keyname);

	/*
	 * If a widget has been selected then we send the keystroke to the
	 * appropriate widget.
	 */
	if (!embed->last.focused) {
		if (embed->last.clicked)
			ewl_embed_focused_widget_set(embed, 
						embed->last.clicked);
		else
		{
			ewl_embed_focused_widget_set(embed, 
				ecore_dlist_goto_first(embed->tab_order));

			if (!embed->last.focused)
				ewl_embed_focused_widget_set(embed, 
							EWL_WIDGET(embed));
		}
	}

	/*
	 * Dispatcher of key down events, these get sent to the last widget
	 * selected, and every parent above it.
	 */
	temp = embed->last.focused;
	while (temp) {
		if (!DISABLED(temp))
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
void
ewl_embed_key_up_feed(Ewl_Embed *embed, char *keyname, unsigned int mods)
{
	Ewl_Widget *temp;
	Ewl_Event_Key_Up ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_PARAM_PTR("keyname", keyname);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	/* handle tab focus change (we just drop it on the floor as the
	 * actual focus change was done in key down */
	if ((embed->last.focused 
			&& (!ewl_widget_ignore_focus_change_get(embed->last.focused))) 
			&& (!strcmp(keyname, "Tab")))
		DRETURN(DLEVEL_STABLE);

	ev.modifiers = mods;
	ev.keyname = strdup(keyname);

	/*
	 * Dispatcher of key up events, these get sent to the last widget
	 * selected, and every parent above it.
	 */
	temp = embed->last.focused;
	while (temp) {
		if (!DISABLED(temp))
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
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);
	widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
	if (!widget)
		widget = EWL_WIDGET(embed);

	/*
	 * Save the last selected widget for further reference, do this prior
	 * to triggering the callback to avoid funkiness if the callback
	 * causes the widget to be destroyed.
	 */
	deselect = embed->last.clicked;

	/* we want the focused and last clicked to be the parent widget, not
	 * the internal children */
	temp = widget;
	while (temp && temp->parent && ewl_widget_internal_is(temp))
		temp = temp->parent;

	ewl_embed_focused_widget_set(embed, temp);

	/*
	 * Make sure we set this _BEFORE_ doing the callbacks below because
	 * the CLICK or DOWN callback may trigger the widget itself to be
	 * deleted which will trigger a cleanup of the info widgets. if we
	 * cleanup this widget we don't want to be setting it as
	 * last.clicked after that.
	 */
	embed->last.clicked = widget;

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
		if (!DISABLED(temp)) {
			ewl_object_state_add(EWL_OBJECT(temp),
					EWL_FLAG_STATE_PRESSED);

			ewl_callback_call_with_event_data(temp,
					EWL_CALLBACK_MOUSE_DOWN, &ev);

			if (ev.clicks > 1) {
				ewl_callback_call_with_event_data(temp,
						EWL_CALLBACK_CLICKED,
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
		/* 
		 * Make sure these widgets haven't been scheduled for
		 * deletion before we send their callbacks. 
		 */
		if (deselect && !DESTROYED(deselect)) {
			ewl_object_state_remove(EWL_OBJECT(deselect),
						EWL_FLAG_STATE_FOCUSED);
			ewl_callback_call(deselect, EWL_CALLBACK_FOCUS_OUT);
		}

		if (widget && !DISABLED(widget) && !DESTROYED(widget)) {
			ewl_object_state_add(EWL_OBJECT(widget),
					EWL_FLAG_STATE_FOCUSED);
			ewl_callback_call(widget, EWL_CALLBACK_FOCUS_IN);
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
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;
	ev.button = b;

	/*
	 * When the mouse is released the widget no longer has a pressed state,
	 * the widget and its parents are notified in this change of state.
	 */
	temp = embed->last.clicked;
	while (temp) {
		if (!DISABLED(temp)) {
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
	Ewl_Widget *widget = NULL;
	Ewl_Event_Mouse_Move ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;

	/*
	 * Focus a new widget if the mouse isn't pressed on the currently
	 * focused widget.
	 */
	if (!embed->last.mouse_in 
			|| !ewl_object_state_has(EWL_OBJECT(embed->last.mouse_in), 
						 EWL_FLAG_STATE_PRESSED)) {

		widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed),
				x, y);
		if (!widget)
			widget = EWL_WIDGET(embed);
	}
	else
		widget = embed->last.mouse_in;

	/*
	 * Defocus all widgets up to the level of a shared parent of
	 * old and newly focused widgets.
	 */
	while (embed->last.mouse_in && (widget != embed->last.mouse_in) 
			&& !ewl_widget_parent_of(embed->last.mouse_in, widget)) {
		ewl_embed_mouse_cursor_set(embed->last.mouse_in);

		ewl_object_state_remove(EWL_OBJECT(embed->last.mouse_in),
				EWL_FLAG_STATE_MOUSE_IN);
		ewl_callback_call(embed->last.mouse_in, EWL_CALLBACK_MOUSE_OUT);

		embed->last.mouse_in = embed->last.mouse_in->parent;
	}

	/*
	 * Pass out the movement event up the chain, allows parents to
	 * react to mouse movement in their children.
	 */
	embed->last.mouse_in = widget;
	while (embed->last.mouse_in) {
		if (!DISABLED(embed->last.mouse_in)) {

			/*
			 * First mouse move event in a widget marks it focused.
			 */
			if (!(ewl_object_state_has(EWL_OBJECT(embed->last.mouse_in),
						EWL_FLAG_STATE_MOUSE_IN))) {
				ewl_embed_mouse_cursor_set(embed->last.mouse_in);
				
				ewl_object_state_add(EWL_OBJECT(embed->last.mouse_in),
						EWL_FLAG_STATE_MOUSE_IN);
				ewl_callback_call_with_event_data(embed->last.mouse_in,
						EWL_CALLBACK_MOUSE_IN, &ev);
			}

			ewl_callback_call_with_event_data(embed->last.mouse_in,
					EWL_CALLBACK_MOUSE_MOVE, &ev);
		}
		
		/*It's possible that the call to MOUSE_IN caused the 'embed->last.mouse_in'
		 * to have become null.  Make sure this pointer is still here*/
		if (embed->last.mouse_in)
			embed->last.mouse_in = embed->last.mouse_in->parent;
	}

	embed->last.mouse_in = widget;

	if (embed->dnd_widget && ewl_object_state_has(EWL_OBJECT(embed->dnd_widget),
								EWL_FLAG_STATE_DND))
		ewl_callback_call_with_event_data(embed->dnd_widget,
						  EWL_CALLBACK_MOUSE_MOVE, &ev);
	else
		embed->dnd_widget = NULL;

	if (embed->last.clicked && ewl_object_state_has(EWL_OBJECT(embed->last.clicked),
								EWL_FLAG_STATE_PRESSED))
		ewl_callback_call_with_event_data(embed->last.clicked,
						  EWL_CALLBACK_MOUSE_MOVE, &ev);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the DND drop event is to occur
 * @param x: the x coordinate of the mouse drop
 * @param y: the y coordinate of the mouse drop
 * @param internal: Is this an internal drop?
 * @return Returns no value.
 * @brief Sends the event for a DND drop into an embed.
 */
void
ewl_embed_dnd_drop_feed(Ewl_Embed *embed, int x, int y, int internal)
{
	Ewl_Widget *widget = NULL;
	Ewl_Event_Dnd_Drop ev;
	void *drop_data = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);

	ev.x = x;
	ev.y = y;

	widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
	if (widget) {
		Ewl_Widget *parent;
		
		
		if (internal) {
			Ewl_Widget_Drag cb;
			
			/* Retrieve the callback for this widget's data */
			cb = (Ewl_Widget_Drag)ewl_widget_data_get(widget, "DROP_CB");
			if (cb) { 
				drop_data = (*cb)();
				ev.data = drop_data;
			}
		} else {
			/* Handle external drops */
			ev.data = NULL;
		}
		
		parent = widget;
		while (parent) {
			ewl_callback_call_with_event_data(parent,
				EWL_CALLBACK_DND_DROP, &ev);
			parent = parent->parent;
		}

		ewl_dnd_drag_widget_clear();
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the DND position event is to occur
 * @param x: the x coordinate of the mouse move
 * @param y: the y coordinate of the mouse move
 * @param px: Where to store the position x value
 * @param py: Where to store the position y value
 * @param pw: Where to store the position width
 * @param ph: Where to store the position height
 * @return Returns no value.
 * @brief Sends the event for a DND position into an embed.
 */
void
ewl_embed_dnd_position_feed(Ewl_Embed *embed, int x, int y, int* px, int* py, int* pw, int* ph)
{
	Ewl_Widget *widget = NULL;
	Ewl_Event_Mouse_Move ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ev.x = x;
	ev.y = y;

	ewl_embed_active_set(embed, TRUE);

	widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
	if (widget) {
		Ewl_Widget *parent;
		Ewl_Window *window;

		/* First see if we need to send an 'enter' to this widget */
		window = ewl_window_window_find(embed->evas_window);

		/* If the last position event was over a different widget,
		 * feed the leaving widget a 'null' */
		if (window->dnd_last_position != widget) {
			if (window->dnd_last_position) {

				parent = window->dnd_last_position;
				while (parent) {
					ewl_callback_call_with_event_data(parent,
						EWL_CALLBACK_DND_LEAVE, &ev);
					parent = parent->parent;
				}
			}

			parent = widget;
			while (parent) {
				ewl_callback_call_with_event_data(parent,
					EWL_CALLBACK_DND_ENTER, &ev);
				parent = parent->parent;
			}
		}
		
		/*
		 * Pass the position event up the chain
		 */ 
		parent = widget;
		
		while (parent) {
			ewl_callback_call_with_event_data(parent,
					EWL_CALLBACK_DND_POSITION, &ev);
			parent = parent->parent;
		}

		ewl_dnd_position_windows_set(EWL_WIDGET(window));
		window->dnd_last_position = widget;

		*px = CURRENT_X(widget);
		*py = CURRENT_Y(widget);
		*pw = CURRENT_W(widget);
		*ph = CURRENT_H(widget);
	} else {
		DWARNING("Could not find widget for dnd position event");
	}

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
ewl_embed_mouse_out_feed(Ewl_Embed *embed, int x, int y, unsigned int mods)
{
	Ewl_Event_Mouse_Out ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;

	while (embed->last.mouse_in) {
		ewl_object_state_remove(EWL_OBJECT(embed->last.mouse_in), 
					EWL_FLAG_STATE_MOUSE_IN);
		ewl_callback_call_with_event_data(embed->last.mouse_in,
						  EWL_CALLBACK_MOUSE_OUT, &ev);
		embed->last.mouse_in = embed->last.mouse_in->parent;
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
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);

	ewl_embed_active_set(embed, TRUE);

	ev.modifiers = mods;
	ev.x = x;
	ev.y = y;
	ev.z = z;
	ev.dir = dir;

	w = embed->last.mouse_in;
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
void
ewl_embed_font_path_add(char *path)
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
Ewl_Embed *
ewl_embed_evas_window_find(Ewl_Embed_Evas_Window *window)
{
	Ewl_Embed *retemb;

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
Ewl_Embed *
ewl_embed_widget_find(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	while (w->parent)
		w = w->parent;

	if (!ewl_object_toplevel_get(EWL_OBJECT(w)))
		w = NULL;

	DRETURN_PTR(EWL_EMBED(w), DLEVEL_STABLE);
}

/**
 * @param e: embed to cache the specified object
 * @param obj: the object to keep cached for reuse
 * @return Returns no value.
 * @brief Caches the specified object for later reuse.
 */
void
ewl_embed_object_cache(Ewl_Embed *e, Evas_Object *obj)
{
	const char *type;
	Ecore_List *obj_list;
	const Evas_List *clippees;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("obj", obj);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

	/* Sanitize the color of this evas pre-cache */
	evas_object_color_set(obj, 255, 255, 255, 255);

	evas_object_smart_member_del(obj);
	evas_object_clip_unset(obj);
	evas_object_hide(obj);

	/* we have to unclip all of the clippees so that we don't end up
	 * getting into an infinite loop resetting the clip later */
	while ((clippees = evas_object_clipees_get(obj)))
		evas_object_clip_unset(clippees->data);

	if (e->obj_cache) {
		type = evas_object_type_get(obj);
		obj_list = ecore_hash_get(e->obj_cache, (void *)type);
		if (!obj_list) {
			obj_list = ecore_list_new();
			ecore_hash_set(e->obj_cache, (void *)type, obj_list);
		}
		ecore_list_prepend(obj_list, obj);

		/* printf("%d nodes cached of type %s\n",
				ecore_list_nodes(obj_list), type); */
	}
	else {
		ewl_evas_object_destroy(obj);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: embed to request a cached object
 * @param type: the type of object requested
 * @return Returns an Evas_Object of the specified type on success.
 */
Evas_Object *
ewl_embed_object_request(Ewl_Embed *e, char *type)
{
	Evas_Object *obj = NULL;
	Ecore_List *obj_list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);
	DCHECK_PARAM_PTR_RET("type", type, NULL);
	DCHECK_TYPE_RET("e", e, EWL_EMBED_TYPE, NULL);

	obj_list = ecore_hash_get(e->obj_cache, type);
	if (obj_list) {
		obj = ecore_list_remove_first(obj_list);

		/* printf("%d nodes remain cached of type %s\n",
				ecore_list_nodes(obj_list), type); */
	}

	DRETURN_PTR(obj, DLEVEL_STABLE);
}

/**
 * @param e: the embed that holds widgets 
 * @param w: the widget that will be moved to the front of the tab order list
 * @return Returns no value.
 * @brief Moves the widget @a w to the front of the tab order list.
 */
void
ewl_embed_tab_order_prepend(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_embed_tab_order_insert(e, w, 0);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The embed that holds the widgets
 * @param w: The widget to be append to the tab order
 * @return Returns no value.
 * @brief Moves the widget @a w to the end of the tab order
 */
void
ewl_embed_tab_order_append(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_embed_tab_order_insert(e, w, ecore_list_nodes(e->tab_order));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The embed that holds the widgets
 * @param w: The widget to insert into the tab order
 * @param idx: The index to insert into
 * @return Returns no value.
 * @brief Moves the given widget @a w to the position @a idx
 */
void
ewl_embed_tab_order_insert(Ewl_Embed *e, Ewl_Widget *w, unsigned int idx)
{
	int current_idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (!ewl_widget_parent_of(EWL_WIDGET(e), w))
		DRETURN(DLEVEL_STABLE);

	/* do nothing if this widget isn't focusable */
	if (!ewl_widget_focusable_get(w))
		DRETURN(DLEVEL_STABLE);

	current_idx = ecore_dlist_index(e->tab_order);

	/* make sure this widget isn't already in the list */
	if (ecore_dlist_goto(e->tab_order, w)) {
		int del_idx;

		/* if this widget was before or at our current focused
		 * widget then we need to decrement our counter */
		del_idx = ecore_dlist_index(e->tab_order);
		if (del_idx <= current_idx) current_idx --;

		ecore_dlist_remove(e->tab_order);
	}
	
	ecore_dlist_goto_index(e->tab_order, idx);
	ecore_dlist_insert(e->tab_order, w);

	/* if we inserted before or at our currently focused item then we
	 * need to advance our current item to the correct spot */
	if (current_idx <= (int)idx) current_idx ++;
	ecore_dlist_goto_index(e->tab_order, current_idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The embed to insert the widget into the tab order
 * @param w: The widget ot insert into the tab order
 * @param after: The widget to insert after
 * @return Returns no value.
 * @brief Inserts the @a w widget into the tab order after the @a after widget
 */
void
ewl_embed_tab_order_insert_after(Ewl_Embed *e, Ewl_Widget *w, 
					Ewl_Widget *after)
{
	int cur_idx, idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("after", after);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("after", after, EWL_WIDGET_TYPE);

	cur_idx = ecore_dlist_index(e->tab_order);
	if (!ecore_dlist_goto(e->tab_order, after))
	{
		ewl_embed_tab_order_append(e, w);
		DRETURN(DLEVEL_STABLE);
	}

	idx = ecore_dlist_index(e->tab_order);
	ecore_dlist_goto_index(e->tab_order, cur_idx);

	ewl_embed_tab_order_insert(e, w, idx + 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The embed to insert the widget into the tab order
 * @param w: The widget ot insert into the tab order
 * @param before: The widget to insert after
 * @return Returns no value.
 * @brief Inserts the @a w widget into the tab order before the @a before widget
 */
void
ewl_embed_tab_order_insert_before(Ewl_Embed *e, Ewl_Widget *w, 
					Ewl_Widget *before)
{
	int cur_idx, idx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("before", before);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("before", before, EWL_WIDGET_TYPE);

	cur_idx = ecore_dlist_index(e->tab_order);
	if (!ecore_dlist_goto(e->tab_order, before))
	{
		ewl_embed_tab_order_prepend(e, w);
		DRETURN(DLEVEL_STABLE);
	}

	idx = ecore_dlist_index(e->tab_order);
	ecore_dlist_goto_index(e->tab_order, cur_idx);

	ewl_embed_tab_order_insert(e, w, idx);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed containing a widget to remove from the tab order
 * @param w: the widget to remove from the tab order list
 * @return Returns no value.
 * @brief Removes the widget @a w from the tab order list for @a e.
 */
void
ewl_embed_tab_order_remove(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (ecore_dlist_goto(e->tab_order, w))
		ecore_dlist_remove(e->tab_order);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to change focus of it's contained widgets
 * @return Returns no value.
 * @brief Changes focus to the next widget in the circular tab order list.
 */
void ewl_embed_tab_order_next(Ewl_Embed *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

	ewl_embed_tab_order_change(e, ecore_dlist_next, 
					ecore_dlist_goto_first);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to change focus of it's contained widgets
 * @return Returns no value.
 * @brief Changes focus to the next widget in the circular tab order list.
 */
void
ewl_embed_tab_order_previous(Ewl_Embed *e)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

	ewl_embed_tab_order_change(e, ecore_dlist_previous, 
					ecore_dlist_goto_last);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * @internal
 * @param e: The embed
 * @param change: Function pointer to move one node
 * @param cycle: Function pointer to move to the other end of the list
 * @return Returns no value
 * @brief Get the next item in the tab order
 */
static void
ewl_embed_tab_order_change(Ewl_Embed *e, void *(*change)(Ecore_DList *list),
					 void *(*cycle)(Ecore_DList *list))
{
	Ewl_Widget *w, *start;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("change", change);
	DCHECK_PARAM_PTR("cycle", cycle);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

	/* make sure the list is at the last focused widget */
	if (e->last.focused)
	{
		ecore_dlist_goto(e->tab_order, e->last.focused);
		change(e->tab_order);
		if (!ecore_dlist_current(e->tab_order))
			cycle(e->tab_order);
	}
	else
		cycle(e->tab_order);

	w = ecore_dlist_current(e->tab_order);
	if (!w) DRETURN(DLEVEL_STABLE);

	start = w;
	while (!VISIBLE(w) || !ewl_widget_focusable_get(w)
			|| ewl_widget_internal_is(w)
			|| DISABLED(w))
	{
		change(e->tab_order);
		w = ecore_dlist_current(e->tab_order);

		/* check if we hit the end of the list and loop to the start */
		if (!w) 
		{
			cycle(e->tab_order);
			w = ecore_dlist_current(e->tab_order);
		}

		/* make sure we don't cycle */
		if (w == start) 
			DRETURN(DLEVEL_STABLE);
	}

	if (w) ewl_embed_focused_widget_set(e, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: The embed to set the focused widget too
 * @param w: The widget to set as having focus
 * @return Returns no value.
 * @brief Set the current focused widget in the embed
 */
void
ewl_embed_focused_widget_set(Ewl_Embed *embed, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("embed", embed);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("embed", embed, EWL_EMBED_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (embed->last.focused)
		ewl_callback_call(embed->last.focused, EWL_CALLBACK_FOCUS_OUT);

	embed->last.focused = w;

	if (embed->last.focused)
		ewl_callback_call(embed->last.focused, EWL_CALLBACK_FOCUS_IN);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: The embed to get the curerntly focused widget
 * @return Returns the currently focused widget
 * @brief Get the current focused widget in the embed.
 */
Ewl_Widget *
ewl_embed_focused_widget_get(Ewl_Embed *embed)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("embed", embed, NULL);
	DCHECK_TYPE_RET("embed", embed, EWL_EMBED_TYPE, NULL);

	DRETURN_PTR(embed->last.focused, DLEVEL_STABLE);
}

/**
 * @param e: The embed to cleanup
 * @param w: The wiget to check while cleaning up
 * @return Returns no value.
 * @brief This will check to see if the given widget is one of the last
 * selected, clicked, mouse_in or the drag-n-drop widget and if so, set them
 * to NULL
 */
void
ewl_embed_info_widgets_cleanup(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

	ewl_object_state_remove(EWL_OBJECT(w), EWL_FLAG_STATE_PRESSED);

	if ((w == e->last.focused) 
			|| (RECURSIVE(w) 
				&& ewl_widget_parent_of(w, e->last.focused)))
		e->last.focused = NULL;

	if ((w == e->last.clicked) 
			|| (RECURSIVE(w) 
				&& ewl_widget_parent_of(w, e->last.clicked)))
		e->last.clicked = NULL;

	if ((w == e->last.mouse_in) 
			|| (RECURSIVE(w) 
				&& ewl_widget_parent_of(w, e->last.mouse_in)))
		e->last.mouse_in = NULL;

	if ((w == e->dnd_widget) 
			|| (RECURSIVE(w) 
				&& ewl_widget_parent_of(w, e->dnd_widget)))
		e->dnd_widget = NULL;

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
void
ewl_embed_coord_to_screen(Ewl_Embed *e, int xx, int yy, int *x, int *y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

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

/**
 * @param w: The Ewl_Widget to get the pointer from
 * @return Returns no value.
 *
 * @brief This will retrieve the attached cursor off of the given widget and
 * display it. If there is no widget on the widget it will use the default
 * cursor.
 */
void
ewl_embed_mouse_cursor_set(Ewl_Widget *w)
{
	Ecore_X_Cursor pointer;
	Ewl_Embed *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	embed = ewl_embed_widget_find(w);
	if (!embed) DRETURN(DLEVEL_STABLE);

	if ((pointer = (Ecore_X_Cursor)ewl_attach_get(w,
					EWL_ATTACH_TYPE_MOUSE_CURSOR)))
		ecore_x_window_cursor_set((Ecore_X_Window)embed->evas_window, pointer);
	else
		ecore_x_window_cursor_set((Ecore_X_Window)embed->evas_window,
					ecore_x_cursor_shape_get(EWL_MOUSE_CURSOR_LEFT_PTR));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to query for position
 * @param x: a pointer to the integer that should receive the x coordinate
 * @param y: a pointer to the integer that should receive the y coordinate
 * @return Returns no value.
 * @brief Retrieve the position of the embed
 *
 * Stores the embed position into the parameters @a x and @a y.
 */
void
ewl_embed_position_get(Ewl_Embed *e, int *x, int *y)
{
       int sx, sy;

       DENTER_FUNCTION(DLEVEL_STABLE);
       DCHECK_PARAM_PTR("e", e);
       DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

       evas_object_geometry_get(e->smart, &sx, &sy, NULL, NULL);
       if (x) *x = e->x + sx;
       if (y) *y = e->y + sy;

       DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to query for window-position
 * @param x: a pointer to the integer that should receive the x coordinate
 * @param y: a pointer to the integer that should receive the y coordinate
 * @return Returns no value.
 * @brief Retrieve the position of the window
 *
 * Stores the window position into the parameters @a x and @a y.
 */
void
ewl_embed_window_position_get(Ewl_Embed *e, int *x, int *y)
{
       DENTER_FUNCTION(DLEVEL_STABLE);
       DCHECK_PARAM_PTR("e", e);
       DCHECK_TYPE("e", e, EWL_EMBED_TYPE);

       if (x) *x = e->x;
       if (y) *y = e->y;

       DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for the realize event
 */
void
ewl_embed_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = EWL_EMBED(w);

	evas_event_freeze(emb->evas);

	if (!emb->ev_clip) {
		/*
		 * first try to get the ev_clip from the cach
		 */
		emb->ev_clip = ewl_embed_object_request(emb, "rectangle");
		if (!emb->ev_clip)
			emb->ev_clip = evas_object_rectangle_add(emb->evas);
		evas_object_color_set(emb->ev_clip, 0, 0, 0, 0);
		evas_object_smart_member_add(emb->ev_clip, emb->smart);
		evas_object_show(emb->ev_clip);
	}

	if (emb->smart) {
		evas_object_focus_set(emb->smart, emb->focus);
		/*
		 * Catch mouse events processed through the evas
		 */
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_IN,
				ewl_embed_evas_mouse_in_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_OUT,
				ewl_embed_evas_mouse_out_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_DOWN,
				ewl_embed_evas_mouse_down_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_UP,
				ewl_embed_evas_mouse_up_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_MOVE,
				ewl_embed_evas_mouse_move_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_MOUSE_WHEEL,
				ewl_embed_evas_mouse_wheel_cb, emb);

		/*
		 * Catch key events processed through the evas
		 */
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_KEY_DOWN,
				ewl_embed_evas_key_down_cb, emb);
		evas_object_event_callback_add(emb->smart,
				EVAS_CALLBACK_KEY_UP, ewl_embed_evas_key_up_cb,
				emb);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for the unrealize event
 */
void
ewl_embed_unrealize_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = EWL_EMBED(w);
	
	if (emb->ev_clip) {
		ewl_evas_object_destroy(emb->ev_clip);
		emb->ev_clip = NULL;
	}
	if (emb->smart) {
		evas_object_smart_data_set(emb->smart, NULL);
		ewl_evas_object_destroy(emb->smart);
		emb->smart = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_embed_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb;
	Ewl_Object *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = EWL_EMBED(w);
	if (emb->ev_clip) {
		evas_object_move(emb->ev_clip, (Evas_Coord)(CURRENT_X(w)),
				 (Evas_Coord)(CURRENT_Y(w)));
		evas_object_resize(emb->ev_clip, (Evas_Coord)(CURRENT_W(w)),
				   (Evas_Coord)(CURRENT_H(w)));
	}
	
	/*
	 * Configure each of the child widgets.
	 */
	ecore_dlist_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_dlist_next(EWL_CONTAINER(w)->children))) {
		int x, y;
		int size;

		/*
		 * Try to give the child the full size of the window from it's
		 * base position. The object will constrict it based on the
		 * fill policy. Don't add the TOP and LEFT insets since
		 * they've already been accounted for.
		 */
		x = ewl_object_current_x_get(EWL_OBJECT(child));
		y = ewl_object_current_y_get(EWL_OBJECT(child));

		if (x < CURRENT_X(w)) {
			x = CURRENT_X(w);
			size = ewl_object_preferred_w_get(EWL_OBJECT(child));
			if (size > PREFERRED_W(w))
				ewl_object_preferred_inner_w_set(EWL_OBJECT(w),
						size);
		}
		if (y < CURRENT_Y(w)) {
			y = CURRENT_Y(w);
			size = ewl_object_preferred_h_get(EWL_OBJECT(child));
			if (size > PREFERRED_H(w))
				ewl_object_preferred_inner_h_set(EWL_OBJECT(w),
						size);
		}

		ewl_object_place(child, x, y, CURRENT_W(w) - (x - CURRENT_X(w)),
				 CURRENT_H(w) - (y - CURRENT_Y(w)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Callback for when the embed loses focus
 */
void
ewl_embed_focus_out_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = EWL_EMBED(w);
	if (!emb->last.focused)
		DRETURN(DLEVEL_STABLE);

	if (ewl_object_state_has(EWL_OBJECT(emb->last.focused),
				EWL_FLAG_STATE_PRESSED)) {
		ewl_embed_mouse_up_feed(emb, 1, 0, 0, ewl_ev_modifiers_get());
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void ewl_embed_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	Ewl_Embed *emb;
	Evas_Object *obj;
	Ecore_List *key_list;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = EWL_EMBED(w);
	if (ecore_list_goto(ewl_embed_list, w))
		ecore_list_remove(ewl_embed_list);

	key_list = ecore_hash_keys(emb->obj_cache);
	if (key_list) {
		char *key;
		/*
		 * Iterate over all object types destroying them as we go. No
		 * need to free the key string.
		 */
		while ((key = ecore_list_remove_first(key_list))) {
			Ecore_List *obj_list;

			/*
			 * Now queue all objects for destruction.
			 */
			obj_list = ecore_hash_remove(emb->obj_cache, key);
			while ((obj = ecore_list_remove_first(obj_list)))
				ewl_evas_object_destroy(obj);
			ecore_list_destroy(obj_list);
		}

		ecore_list_destroy(key_list);
	}

	ecore_hash_destroy(emb->obj_cache);
	emb->obj_cache = NULL;

	ecore_dlist_destroy(emb->tab_order);
	emb->tab_order = NULL;

	if (emb == ewl_embed_active_embed)
		ewl_embed_active_embed = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_add_cb(Evas_Object *obj __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Nothing to see here! Move along...
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_del_cb(Evas_Object *obj)
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

static void
ewl_embed_smart_show_cb(Evas_Object *obj)
{
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb)
		ewl_widget_show(EWL_WIDGET(emb));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_hide_cb(Evas_Object *obj)
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

static void
ewl_embed_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
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

static void
ewl_embed_smart_clip_unset_cb(Evas_Object *obj)
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

	if (ev->canvas.x < CURRENT_X(embed) 
		|| ev->canvas.x >= CURRENT_X(embed) + CURRENT_W(embed)
		|| ev->canvas.y < CURRENT_Y(embed)
		|| ev->canvas.y >= CURRENT_Y(embed) + CURRENT_H(embed))
	{
		ewl_embed_mouse_out_feed(embed, ev->canvas.x, ev->canvas.y,
					 ewl_ev_modifiers_get());
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_down_cb(void *data, Evas *e __UNUSED__, 
				Evas_Object *obj __UNUSED__, void *event_info)
{
	Ewl_Embed *embed;
	Evas_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = event_info;
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
	Evas_Event_Mouse_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	ev = event_info;
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
	Evas_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = event_info;
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
	Evas_Event_Key_Down *ev;
	unsigned int key_modifiers = 0;
	char *keyname;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ev = event_info;
	keyname = strdup(ev->keyname);
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
	} else if ((key_modifiers & EWL_KEY_MODIFIER_SHIFT)
	 		&& (strlen(keyname) == 1))
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
	} else if ((key_modifiers & EWL_KEY_MODIFIER_SHIFT) 
			&& (strlen(keyname) == 1))
		strupper(keyname);

	ewl_embed_key_up_feed(embed, keyname, ewl_ev_modifiers_get());
	free(keyname);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Uppercase the given string
 */
static void
strupper(char *str)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("str", str);

	char *i;
	for(i = str; *i != '\0'; i++) 
		*i = toupper(*i);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

