#include <Ewl.h>

Ewd_List       *ewl_embed_list = NULL;
Evas_Smart     *embedded_smart = NULL;

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
	if (!ewl_container_init(EWL_CONTAINER(w), "embed",
				ewl_embed_child_add_cb,
				ewl_embed_child_resize_cb, NULL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_object_set_toplevel(EWL_OBJECT(w), EWL_FLAG_PROPERTY_TOPLEVEL);

	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_UNREALIZE,
			     ewl_embed_unrealize_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			     ewl_embed_destroy_cb, NULL);

	/*
	 * Override the default configure callbacks since the embed
	 * has special needs for placement.
	 */
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			     ewl_embed_configure_cb, NULL);

	LAYER(w) = -1000;

	ewd_list_append(ewl_embed_list, w);

	w->tab_order = ewd_list_new();

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
ewl_embed_set_evas(Ewl_Embed *emb, Evas *evas, Ecore_X_Window evas_window)
{
	Ewl_Widget *w;
	Ewd_List   *paths;
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

	if (w->fx_clip_box) {
		evas_object_clip_set(emb->smart, w->fx_clip_box);
		evas_object_repeat_events_set(w->fx_clip_box, TRUE);

		/*
		 * Catch mouse events processed through the evas
		 */
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_IN,
				ewl_embed_evas_mouse_in_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_OUT,
				ewl_embed_evas_mouse_out_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_DOWN,
				ewl_embed_evas_mouse_down_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_UP,
				ewl_embed_evas_mouse_up_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_MOVE,
				ewl_embed_evas_mouse_move_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_MOUSE_WHEEL,
				ewl_embed_evas_mouse_wheel_cb, emb);

		/*
		 * Catch key events processed through the evas
		 */
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_KEY_DOWN,
				ewl_embed_evas_key_down_cb, emb);
		evas_object_event_callback_add(w->fx_clip_box,
				EVAS_CALLBACK_KEY_UP, ewl_embed_evas_key_up_cb,
				emb);
	}

	paths = ewl_theme_font_path_get();
	ewd_list_goto_first(paths);
	while ((font_path = ewd_list_next(paths))) {
		evas_font_path_append(evas, font_path);
	}

	DRETURN_PTR(emb->smart, DLEVEL_STABLE);
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

	ewd_list_goto_first(ewl_embed_list);
	while ((e = ewd_list_next(ewl_embed_list)))
		if (REALIZED(e))
			evas_font_path_append(e->evas, path);

	ewd_list_append(ewl_theme_font_path_get(), strdup(path));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param window: the evas window to search for on the list of embeds
 * @return Returns the found embed on success, NULL on failure.
 * @brief Find an ewl embed by its evas window
 */
Ewl_Embed      *ewl_embed_find_by_evas_window(Ecore_X_Window window)
{
	Ewl_Embed      *retemb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_embed_list);

	while ((retemb = ewd_list_next(ewl_embed_list)) != NULL) {
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
Ewl_Embed     *ewl_embed_find_by_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	while (w->parent)
		w = w->parent;

	if (!ewl_object_get_toplevel(EWL_OBJECT(w)))
		w = NULL;

	DRETURN_PTR(EWL_EMBED(w), DLEVEL_STABLE);
}

/**
 * @param e: the embed that holds widgets to change tab order
 * @param w: the widget that will be moved to the front of the tab order list
 * @return Returns no value.
 * @brief Moves the widget @a w to the front of the tab order list.
 */
void ewl_embed_push_tab_order(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);

	if (!ewl_container_parent_of(EWL_WIDGET(e), w))
		DRETURN(DLEVEL_STABLE);

	if (ewd_list_goto(e->tab_order, w))
		ewd_list_remove(e->tab_order);

	ewd_list_prepend(e->tab_order, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed containing a widget to remove from the tab order
 * @param w: the widget to remove from the tab order list
 * @return Returns no value.
 * @brief Removes the widget @a w from the tab order list for @a e.
 */
void ewl_embed_remove_tab_order(Ewl_Embed *e, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("w", w);

	if (ewd_list_goto(e->tab_order, w))
		ewd_list_remove(e->tab_order);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to change focus of it's contained widgets
 * @return Returns no value.
 * @brief Changes focus to the next widget in the circular tab order list.
 */
void ewl_embed_next_tab_order(Ewl_Embed *e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (!(w = ewd_list_next(e->tab_order))) {
		ewd_list_goto_first(e->tab_order);
		w = ewd_list_next(e->tab_order);
	}

	if (w)
		ewl_widget_send_focus(w);

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

void ewl_embed_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Object *o;
	Ewl_Object *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	o = EWL_OBJECT(w);

	/*
	 * Configure each of the child widgets.
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(w)->children))) {
		/*
		 * Try to give the child the full size of the window from it's
		 * base position. The object will constrict it based on the
		 * fill policy. Don't add the TOP and LEFT insets since
		 * they've already been accounted for.
		 */
		ewl_object_request_size(child,
					CURRENT_W(w) -
					ewl_object_get_current_x(child),
					CURRENT_H(w) -
					ewl_object_get_current_y(child));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (EWL_EMBED(w)->smart) {
		evas_object_smart_data_set(EWL_EMBED(w)->smart, NULL);
		ewl_evas_object_destroy(EWL_EMBED(w)->smart);
		EWL_EMBED(w)->smart = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Embed      *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	emb = EWL_EMBED(w);

	if (ewd_list_goto(ewl_embed_list, w))
		ewd_list_remove(ewl_embed_list);

	ewd_list_destroy(emb->tab_order);
	emb->tab_order = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_child_add_cb(Ewl_Container * emb, Ewl_Widget * child)
{
	int size;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Move the child within the bounsd of the embed.
	 */
	if (ewl_object_get_current_x(EWL_OBJECT(child)) < CURRENT_X(emb))
		ewl_object_request_x(EWL_OBJECT(child), CURRENT_X(emb));
	if (ewl_object_get_current_y(EWL_OBJECT(child)) < CURRENT_Y(emb))
		ewl_object_request_y(EWL_OBJECT(child), CURRENT_Y(emb));

	size = ewl_object_get_current_x(EWL_OBJECT(child)) +
		ewl_object_get_preferred_w(EWL_OBJECT(child)) - CURRENT_X(emb);
	if (size > PREFERRED_W(emb))
		ewl_object_set_preferred_w(EWL_OBJECT(emb), size);

	size = ewl_object_get_current_y(EWL_OBJECT(child)) +
		ewl_object_get_preferred_h(EWL_OBJECT(child)) - CURRENT_Y(emb);
	if (size > PREFERRED_H(emb))
		ewl_object_set_preferred_h(EWL_OBJECT(emb), size);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_embed_child_resize_cb(Ewl_Container *c, Ewl_Widget *w,
			       int size, Ewl_Orientation o)
{
	int            maxw = 0, maxh = 0;
	Ewl_Embed     *emb;
	Ewl_Object    *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	child = EWL_OBJECT(w);
	emb = EWL_EMBED(c);

	ewd_list_goto_first(EWL_CONTAINER(emb)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(emb)->children))) {
		int             cs;

		/*
		 * FIXME: Do we really want to do this?
		 * Move children within the bounds of the viewable area
		 */
		if (ewl_object_get_current_x(child) < CURRENT_X(emb))
			ewl_object_request_x(child, CURRENT_X(emb));
		if (ewl_object_get_current_y(child) < CURRENT_Y(emb))
			ewl_object_request_y(child, CURRENT_Y(emb));

		cs = ewl_object_get_current_x(child) +
			ewl_object_get_preferred_w(child);

		/*
		 * Check the width and x position vs. embed width.
		 */
		if (maxw < cs)
			maxw = cs;

		cs = ewl_object_get_current_y(child) +
			ewl_object_get_preferred_h(child);

		/*
		 * Check the height and y position vs. embed height.
		 */
		if (maxh < cs)
			maxh = cs;

	}

	ewl_object_set_preferred_size(EWL_OBJECT(emb), maxw, maxh);
	ewl_object_request_size(EWL_OBJECT(c),
				ewl_object_get_current_w(EWL_OBJECT(c)),
				ewl_object_get_current_h(EWL_OBJECT(c)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_embed_smart_add_cb(Evas_Object *obj)
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
		ewl_widget_set_layer(EWL_WIDGET(emb), l);

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
		ewl_widget_set_layer(EWL_WIDGET(emb), l);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_layer_adjust_rel_cb(Evas_Object *obj, Evas_Object *rel)
{
	int l;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	emb = evas_object_smart_data_get(obj);
	if (emb) {
		l = evas_object_layer_get(obj);
		ewl_widget_set_layer(EWL_WIDGET(emb), l);
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
		ewl_object_request_position(EWL_OBJECT(emb), (int)(x),
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
		ewl_object_request_size(EWL_OBJECT(emb), (int)(w), (int)(h));

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
ewl_embed_smart_color_set_cb(Evas_Object *obj, int r, int g, int b, int a)
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
ewl_embed_evas_mouse_in_cb(void *data, Evas *e, Evas_Object *obj,
			   void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * As we ignore this for the windows, may as well ignore it here.
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_out_cb(void *data, Evas *e, Evas_Object *obj,
			    void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_mouse_out(NULL, ECORE_X_EVENT_MOUSE_OUT,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_down_cb(void *data, Evas *e, Evas_Object *obj,
			     void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_mouse_down(NULL, ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_up_cb(void *data, Evas *e, Evas_Object *obj,
			   void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_mouse_up(NULL, ECORE_X_EVENT_MOUSE_BUTTON_UP,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_move_cb(void *data, Evas *e, Evas_Object *obj,
			     void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_mouse_move(NULL, ECORE_X_EVENT_MOUSE_MOVE,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj,
			      void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_key_down_cb(void *data, Evas *e, Evas_Object *obj,
			   void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_key_down(NULL, ECORE_X_EVENT_KEY_DOWN,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_key_up_cb(void *data, Evas *e, Evas_Object *obj,
			 void *event_info)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_ev_key_up(NULL, ECORE_X_EVENT_KEY_UP,
			ecore_event_current_event_get());

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
