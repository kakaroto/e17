/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#include <Evas.h>
#include <Edje.h>

static int ewl_embed_last_mouse_x = 0;
static int ewl_embed_last_mouse_y = 0;

Ecore_List *ewl_embed_list = NULL;
static Evas_Smart *embedded_smart = NULL;
static Ewl_Embed *ewl_embed_active_embed = NULL;

static Ewl_Widget *ewl_embed_primary_selection_widget = NULL;

static void ewl_embed_selection_cb_destroy(Ewl_Widget *w, void *ev, void *data);

static void ewl_embed_smart_cb_del(Evas_Object *obj);
static void ewl_embed_smart_cb_move(Evas_Object *obj, Evas_Coord x,
                                    Evas_Coord y);
static void ewl_embed_smart_cb_resize(Evas_Object *obj, Evas_Coord w,
                                      Evas_Coord h);
static void ewl_embed_smart_cb_show(Evas_Object *obj);
static void ewl_embed_smart_cb_hide(Evas_Object *obj);
static void ewl_embed_smart_cb_clip_set(Evas_Object *obj, Evas_Object *clip);
static void ewl_embed_smart_cb_clip_unset(Evas_Object *obj);

static void ewl_embed_tab_order_change(Ewl_Embed *e,
                                 void *(*change)(Ecore_DList *list),
                                 void *(*cycle)(Ecore_DList *list));

/*
 * Catch mouse events processed through the evas
 */
static void ewl_embed_evas_cb_mouse_out(void *data, Evas *e, Evas_Object *obj,
                                        void *event_info);
static void ewl_embed_evas_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
                                         void *event_info);
static void ewl_embed_evas_cb_mouse_up(void *data, Evas *e, Evas_Object *obj,
                                       void *event_info);
static void ewl_embed_evas_cb_mouse_move(void *data, Evas *e, Evas_Object *obj,
                                         void *event_info);
static void ewl_embed_evas_cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj,
                                          void *event_info);

/*
 * Catch key events processed through the evas
 */
static void ewl_embed_evas_cb_key_down(void *data, Evas *e, Evas_Object *obj,
                                       void *event_info);
static void ewl_embed_evas_cb_key_up(void *data, Evas *e, Evas_Object *obj,
                                     void *event_info);

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
 * @param emb: the embed to be initialized to default values and callbacks
 * @return Returns TRUE or FALSE depending on if initialization succeeds.
 * @brief initialize a embed to default values and callbacks
 *
 * Sets the values and callbacks of a embed @a w to their defaults.
 */
int
ewl_embed_init(Ewl_Embed *emb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(emb, FALSE);

        /*
         * Initialize the fields of the inherited container class
         */
        if (!ewl_cell_init(EWL_CELL(emb)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(emb), EWL_EMBED_TYPE);
        ewl_widget_inherit(EWL_WIDGET(emb), EWL_EMBED_TYPE);

        if (!ewl_embed_engine_name_set(emb, ewl_config_string_get(ewl_config,
                                EWL_CONFIG_ENGINE_NAME)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_object_fill_policy_set(EWL_OBJECT(emb), EWL_FLAG_FILL_NONE);
        ewl_widget_toplevel_set(EWL_WIDGET(emb), TRUE);
        
        /* remove the notify flags set by the cell */
        ewl_container_callback_nonotify(EWL_CONTAINER(emb),
                                                        EWL_CALLBACK_FOCUS_IN);
        ewl_container_callback_nonotify(EWL_CONTAINER(emb),
                                                        EWL_CALLBACK_FOCUS_OUT);

        ewl_callback_append(EWL_WIDGET(emb), EWL_CALLBACK_REALIZE,
                             ewl_embed_cb_realize, NULL);
        ewl_callback_append(EWL_WIDGET(emb), EWL_CALLBACK_UNREALIZE,
                             ewl_embed_cb_unrealize, NULL);
        ewl_callback_prepend(EWL_WIDGET(emb), EWL_CALLBACK_DESTROY,
                             ewl_embed_cb_destroy, NULL);
        ewl_callback_prepend(EWL_WIDGET(emb), EWL_CALLBACK_CONFIGURE,
                             ewl_embed_cb_configure, NULL);
        ewl_callback_prepend(EWL_WIDGET(emb), EWL_CALLBACK_FOCUS_OUT,
                             ewl_embed_cb_focus_out, NULL);

        ecore_list_append(ewl_embed_list, emb);

        emb->tab_order = ecore_dlist_new();
        emb->obj_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param embed: Embed to change engines
 * @param engine: Name of the new engine to use for the embed.
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Changes the current engine on an embed to the specified engine.
 */
int
ewl_embed_engine_name_set(Ewl_Embed *embed, const char *engine)
{
        int realize = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_PARAM_PTR_RET(engine, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

        if (REALIZED(embed)) {
                ewl_widget_unrealize(EWL_WIDGET(embed));
                realize = TRUE;
        }

        IF_RELEASE(embed->engine_name);
        embed->engine_name = ecore_string_instance(engine);

        embed->engine = ewl_engine_new(engine, NULL, NULL);
        if (!embed->engine)
        {
                DERROR("Error creating engine ...\n");
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        if (realize)
                ewl_widget_realize(EWL_WIDGET(embed));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param embed: Embed to get engine name
 * @return Returns the name of the engine to use for the embed.
 * @brief Get the current engine on an embed.
 */
const char *
ewl_embed_engine_name_get(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        DRETURN_PTR(embed->engine_name, DLEVEL_STABLE);
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
 * @param emb: the embedded container to change the target canvas
 * @param canvas: the new canvas to draw the container and it's contents
 * @param canvas_window: the window containing the canvas, for event dispatching
 * @return Returns an canvas object on success, NULL on failure.
 * @brief Change the canvas used by the embedded container
 *
 * The returned smart object can be used to manipulate the area used by EWL
 * through standard canvas functions.
 */
void *
ewl_embed_canvas_set(Ewl_Embed *emb, void *canvas, Ewl_Embed_Window *canvas_window)
{
        Ecore_List *paths;
        char *font_path;
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(emb, NULL);
        DCHECK_PARAM_PTR_RET(canvas, NULL);
        DCHECK_TYPE_RET(emb, EWL_EMBED_TYPE, NULL);

        emb->canvas = canvas;
        emb->canvas_window = canvas_window;

        if (!embedded_smart) {
                static const Evas_Smart_Class sc = {
                        "EWL Embedded Smart Object",
                        EVAS_SMART_CLASS_VERSION,
                        NULL,
                        ewl_embed_smart_cb_del,
                        ewl_embed_smart_cb_move,
                        ewl_embed_smart_cb_resize,
                        ewl_embed_smart_cb_show,
                        ewl_embed_smart_cb_hide,
                        NULL,
                        ewl_embed_smart_cb_clip_set,
                        ewl_embed_smart_cb_clip_unset,
			NULL,
			NULL,
                        NULL,
                        NULL
                };
                embedded_smart = evas_smart_class_new(&sc);
        }

        if (emb->smart) {
                ewl_canvas_object_destroy(emb->smart);
                emb->smart = NULL;
        }

        emb->smart = evas_object_smart_add(emb->canvas, embedded_smart);
        evas_object_smart_data_set(emb->smart, emb);

        if (VISIBLE(emb))
                ewl_realize_request(EWL_WIDGET(emb));

        paths = ewl_theme_font_path_get();
        ecore_list_first_goto(paths);
        while ((font_path = ecore_list_next(paths)))
                evas_font_path_append(canvas, font_path);

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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        embed->focus = !!f;
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
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

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
                ewl_embed_focus_set(embed, FALSE);
        }
        else
        {
                e = ewl_embed_active_embed;
                ewl_embed_active_embed = embed;
                ewl_embed_focus_set(embed, TRUE);
        }

        if (e && e->last.clicked)
        {
                Ewl_Widget *temp;

                ewl_widget_state_remove(e->last.clicked,
                                                EWL_FLAG_STATE_FOCUSED);
                ewl_widget_state_remove(e->last.clicked,
                                                EWL_FLAG_STATE_PRESSED);

                ewl_callback_call(e->last.clicked, EWL_CALLBACK_FOCUS_OUT);

                /* Clean the last.clicked up recursively */
                temp = e->last.clicked;
                while (temp) {
                        if (!DISABLED(temp))
                                ewl_widget_state_remove(temp,
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
 * @param x: a pointer to the location to save the x coordinate
 * @param y: a pointer to the location to save the y coordinate
 * @return Returns no value.
 * @brief Get the last tracked mouse position
 *
 * Get the last tracked mouse position. Ewl only tracks mouse postion, which
 * are recognized by EWL that mean it only gives you the last mouse positon
 * that is recognized inside of an Ewl_Embed.
 *
 * The x and y pointer may be NULL.
 */
void
ewl_embed_last_mouse_position_get(int *x, int *y)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (x) *x = ewl_embed_last_mouse_x;
        if (y) *y = ewl_embed_last_mouse_y;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the key event is to occur
 * @param keyname: the key press to trigger
 * @param mods: the mask of key modifiers currently pressed
 * @return Returns no value.
 * @brief Sends the event for a key press into an embed.
 */
void
ewl_embed_key_down_feed(Ewl_Embed *embed, const char *keyname,
                        unsigned int mods)
{
        Ewl_Widget *temp;
        Ewl_Event_Key_Down ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(keyname);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /*
         * check if this is a focus change key press and we are not ignoring
         * focus change events
         */
        /* FIXME This "Tab" should probably be made a config variable */
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
        ev.base.modifiers = mods;
        ev.base.keyname = strdup(keyname);

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
                                ecore_dlist_first_goto(embed->tab_order));

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

        FREE(ev.base.keyname);

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
ewl_embed_key_up_feed(Ewl_Embed *embed, const char *keyname,
                        unsigned int mods)
{
        Ewl_Widget *temp;
        Ewl_Event_Key_Up ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(keyname);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /* handle tab focus change (we just drop it on the floor as the
         * actual focus change was done in key down */
        /* FIXME Should probably make this "Tab" a config variable */
        if ((embed->last.focused
                        && (!ewl_widget_ignore_focus_change_get(embed->last.focused)))
                        && (!strcmp(keyname, "Tab")))
                DRETURN(DLEVEL_STABLE);

        ev.base.modifiers = mods;
        ev.base.keyname = strdup(keyname);

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

        FREE(ev.base.keyname);

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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /*
         * Keep track on the mouse position
         */
        ewl_embed_last_mouse_x = x + embed->x;
        ewl_embed_last_mouse_y = y + embed->y;

        ewl_embed_active_set(embed, TRUE);

        widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
        if (!widget) widget = EWL_WIDGET(embed);

        /*
         * Save the last focused widget for further reference, do this prior
         * to triggering the callback to avoid funkiness if the callback
         * causes the widget to be destroyed.
         */
        deselect = embed->last.focused;

        /* we want the focused and last clicked to be the parent widget, not
         * the internal children */
        temp = widget;
        while (temp && temp->parent && ewl_widget_internal_is(temp))
                temp = temp->parent;

        /* Set last focused to new widget */
        embed->last.focused = temp;

        /*
         * Make sure we set this _BEFORE_ doing the callbacks below because
         * the CLICK or DOWN callback may trigger the widget itself to be
         * deleted which will trigger a cleanup of the info widgets. if we
         * cleanup this widget we don't want to be setting it as
         * last.clicked after that.
         */
        embed->last.clicked = widget;

        ev.base.modifiers = mods;
        ev.base.x = x;
        ev.base.y = y;
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
                        ewl_widget_state_add(temp, EWL_FLAG_STATE_PRESSED);

                        ewl_callback_call_with_event_data(temp,
                                        EWL_CALLBACK_MOUSE_DOWN, &ev);

                        if (ev.clicks > 1)
                                ewl_callback_call_with_event_data(temp,
                                        EWL_CALLBACK_CLICKED, &ev);
                }
                temp = temp->parent;
        }

        /* Set to upper widget */
        widget = embed->last.focused;

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
                if (deselect && !DESTROYED(deselect) &&
                                !ewl_widget_parent_of(deselect, widget)) {
                        ewl_widget_state_remove(deselect,
                                        EWL_FLAG_STATE_FOCUSED);
                        ewl_callback_call_with_event_data(deselect,
                                        EWL_CALLBACK_FOCUS_OUT, widget);
                }

                if (widget && !DISABLED(widget) && !DESTROYED(widget)) {
                        ewl_widget_state_add(widget,
                                        EWL_FLAG_STATE_FOCUSED);
                        ewl_callback_call_with_event_data(widget,
                                        EWL_CALLBACK_FOCUS_IN, deselect);
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
ewl_embed_mouse_up_feed(Ewl_Embed *embed, int b, int clicks,
                        int x, int y, unsigned int mods)
{
        Ewl_Widget *temp;
        Ewl_Event_Mouse_Up ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /*
         * Keep track on the mouse position
         */
        ewl_embed_last_mouse_x = x + embed->x;
        ewl_embed_last_mouse_y = y + embed->y;


        ewl_embed_active_set(embed, TRUE);

        ev.base.modifiers = mods;
        ev.base.x = x;
        ev.base.y = y;
        ev.button = b;
        ev.clicks = clicks;

        /*
         * When the mouse is released the widget no longer has a pressed state,
         * the widget and its parents are notified in this change of state.
         */
        temp = embed->last.clicked;
        while (temp) {
                if (!DISABLED(temp)) {
                        ewl_widget_state_remove(temp,
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
        Ewl_Widget *check;
        Ewl_Widget *temp;
        Ewl_Widget *widget = NULL;
        Ewl_Event_Mouse_Move ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /*
         * Keep track on the mouse position
         */
        ewl_embed_last_mouse_x = x + embed->x;
        ewl_embed_last_mouse_y = y + embed->y;

        ewl_embed_active_set(embed, TRUE);

        ev.base.modifiers = mods;
        ev.base.x = x;
        ev.base.y = y;

        /*
         * Focus a new widget if the mouse isn't pressed on the currently
         * focused widget.
         */
        check = embed->last.mouse_in;
        if (!check || !ewl_widget_state_has(check, EWL_FLAG_STATE_PRESSED)) {

                widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
                if (!widget) widget = EWL_WIDGET(embed);
        }
        else
                widget = embed->last.mouse_in;


        /* Cycle through all parents of the widget to see if any have
         * a non-default cursor
         */
        temp = check;
        check = NULL;
        while (temp)
        {
                if (ewl_attach_get(temp, EWL_ATTACH_TYPE_MOUSE_ARGB_CURSOR) ||
                                ewl_attach_get(temp, EWL_ATTACH_TYPE_MOUSE_CURSOR))
                {
                        check = temp;
                        break;
                }

                temp = temp->parent;
        }

        /* Set the cursor to first parent if possible */
        if (check)
                ewl_embed_mouse_cursor_set(check);
        else if (embed->last.mouse_in && embed->last.mouse_in->parent)
                ewl_embed_mouse_cursor_set(embed->last.mouse_in->parent);

        /*
         * Defocus all widgets up to the level of a shared parent of
         * old and newly focused widgets.
         */
        check = embed->last.mouse_in;
        while (check && (widget != check)
                        && !ewl_widget_parent_of(check, widget)) {

                ewl_widget_state_remove(check, EWL_FLAG_STATE_MOUSE_IN);
                ewl_callback_call(check, EWL_CALLBACK_MOUSE_OUT);
                check = check->parent;
        }

        /*
         * Pass out the movement event up the chain, allows parents to
         * react to mouse movement in their children.
         */
        embed->last.mouse_in = widget;
        check = widget;
        while (check) {
                if (!DISABLED(check)) {

                        /*
                         * First mouse move event in a widget marks it focused.
                         */
                        if (!(ewl_widget_state_has(check,
                                                EWL_FLAG_STATE_MOUSE_IN))) {

                                /* Only set the cursor different if it is non-default */
                                if (ewl_attach_get(check,
                                                        EWL_ATTACH_TYPE_MOUSE_ARGB_CURSOR) ||
                                                ewl_attach_get(check,
                                                        EWL_ATTACH_TYPE_MOUSE_CURSOR))
                                        ewl_embed_mouse_cursor_set(check);

                                ewl_widget_state_add(check,
                                                EWL_FLAG_STATE_MOUSE_IN);
                                ewl_callback_call_with_event_data(check,
                                                EWL_CALLBACK_MOUSE_IN, &ev);
                        }

                        ewl_callback_call_with_event_data(check,
                                        EWL_CALLBACK_MOUSE_MOVE, &ev);
                }

                /*
                 * It's possible that the call to MOUSE_IN caused the
                 * 'embed->last.mouse_in' to have become null.  Make sure this
                 * pointer is still here
                 */
                if (check) check = check->parent;
        }

        /*
         * Determine if the drag widget should be receiving the mouse movement
         * events.
         */
        check = embed->last.drag_widget;
        if (check && ewl_widget_state_has(check, EWL_FLAG_STATE_DND))
                ewl_callback_call_with_event_data(check,
                                                  EWL_CALLBACK_MOUSE_MOVE, &ev);

        check = embed->last.clicked;
        if (check && ewl_widget_state_has(check, EWL_FLAG_STATE_PRESSED))
                ewl_callback_call_with_event_data(check,
                                                  EWL_CALLBACK_MOUSE_MOVE, &ev);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the DND drop event is to occur
 * @param x: the x coordinate of the mouse drop
 * @param y: the y coordinate of the mouse drop
 * @param internal: Is this an internal drop?
 * @return Returns the DND drop data type.
 * @brief Sends the event for a DND drop into an embed.
 */
const char *
ewl_embed_dnd_drop_feed(Ewl_Embed *embed, int x, int y)
{
        Ewl_Widget *widget = NULL, *parent = NULL;
        const char *result = NULL;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        ewl_embed_active_set(embed, TRUE);

        widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
        /*
         * Find the lowest DND aware widget
         */
        while (widget) {
                if (ewl_widget_flags_has(widget,
                                        EWL_FLAG_PROPERTY_DND_TARGET,
                                        EWL_FLAGS_PROPERTY_MASK))
                        break;
                widget = widget->parent;
        }
        if (!widget) DRETURN_PTR(NULL, DLEVEL_STABLE);

        /* Request a DND data request */
        for (i = 0; i < embed->dnd_types.num_types; i++) {
                if (ewl_dnd_accepted_types_contains(widget, embed->dnd_types.types[i])) {
                        result = embed->dnd_types.types[i];
                        break;
                }
        }

        if (result) {
                Ewl_Event_Dnd_Drop ev;

                ev.x = x;
                ev.y = y;
                ev.data = NULL;

                embed->last.drop_widget = widget;
                parent = widget;
                while (parent) {
                        ewl_callback_call_with_event_data(parent,
                                EWL_CALLBACK_DND_DROP, &ev);
                        parent = parent->parent;
                }
        }

        ewl_dnd_drag_widget_clear();

        DRETURN_PTR(result, DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the DND position event is to occur
 * @param x: the x coordinate of the mouse move
 * @param y: the y coordinate of the mouse move
 * @param px: Where to store the position x value
 * @param py: Where to store the position y value
 * @param pw: Where to store the position width
 * @param ph: Where to store the position height
 * @return Returns the type of the DND data
 * @brief Sends the event for a DND position into an embed.
 */
const char *
ewl_embed_dnd_position_feed(Ewl_Embed *embed, int x, int y, int* px, int* py, int* pw, int* ph)
{
        const char *result = NULL;
        Ewl_Widget *widget = NULL, *parent = NULL;
        Ewl_Event_Dnd_Position ev;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        ev.x = x;
        ev.y = y;

        ewl_embed_active_set(embed, TRUE);

        widget = ewl_container_child_at_recursive_get(EWL_CONTAINER(embed), x, y);
        /*
         * Find the lowest DND aware widget
         */
        while (widget) {
                if (ewl_widget_flags_has(widget, EWL_FLAG_PROPERTY_DND_TARGET,
                                        EWL_FLAGS_PROPERTY_MASK))
                        break;
                widget = widget->parent;
        }

        if (!widget) DRETURN_PTR(NULL, DLEVEL_STABLE);

        /* If the last position event was over a different widget,
         * feed the leaving widget a 'null' */
        if (embed->dnd_last_position != widget) {
                if (embed->dnd_last_position) {
                        parent = embed->dnd_last_position;
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

        embed->last.drop_widget = widget;
        embed->dnd_last_position = widget;

        /* Request a DND data request */
        for (i = 0; i < embed->dnd_types.num_types; i++) {
                if (ewl_dnd_accepted_types_contains(widget,
                                        embed->dnd_types.types[i])) {
                        result = embed->dnd_types.types[i];
                        break;
                }
        }

        if (px) *px = CURRENT_X(widget);
        if (py) *py = CURRENT_Y(widget);
        if (pw) *pw = CURRENT_W(widget);
        if (ph) *ph = CURRENT_H(widget);

        DRETURN_PTR(result, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /* Is this call really correct? Because we are probably loosing focus
         * atm */
        ewl_embed_active_set(embed, TRUE);

        ev.base.modifiers = mods;
        ev.base.x = x;
        ev.base.y = y;

        while (embed->last.mouse_in) {
                ewl_widget_state_remove(embed->last.mouse_in,
                                        EWL_FLAG_STATE_MOUSE_IN);
                ewl_callback_call_with_event_data(embed->last.mouse_in,
                                                  EWL_CALLBACK_MOUSE_OUT, &ev);
                embed->last.mouse_in = embed->last.mouse_in->parent;
        }

        if ((embed->last.drag_widget) && (ewl_widget_state_has
                                (embed->last.drag_widget, EWL_FLAG_STATE_DND)))
                ewl_dnd_external_drag_start(embed->last.drag_widget);

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
ewl_embed_mouse_wheel_feed(Ewl_Embed *embed, int x, int y, int z, int dir,
                                                        unsigned int mods)
{
        Ewl_Widget *w;
        Ewl_Event_Mouse_Wheel ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        ewl_embed_active_set(embed, TRUE);

        ev.base.modifiers = mods;
        ev.base.x = x;
        ev.base.y = y;
        ev.z = z;
        ev.dir = dir;

        w = embed->last.mouse_in;
        if (!w)
                ewl_callback_call_with_event_data(EWL_WIDGET(embed),
                                          EWL_CALLBACK_MOUSE_WHEEL, &ev);

        while (w) {
                ewl_callback_call_with_event_data(w,
                                EWL_CALLBACK_MOUSE_WHEEL, &ev);
                w = w->parent;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the selection data event is to occur
 * @param type: The type to feed.
 * @param data: a pointer to the data received that generated the event
 * @param len: length of the data that generated the event
 * @param format: The bit format of the data
 * @return Returns no value.
 * @brief Sends the event for selection data received into an embed.
 */
void
ewl_embed_dnd_data_received_feed(Ewl_Embed *embed, char *type, void *data,
                                        unsigned int len, unsigned int format)
{
        Ewl_Event_Dnd_Data_Received ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /*
         * If a widget is expecting DND data, send the data to the widget
         */
        if (embed->last.drop_widget) {
                if (ewl_dnd_accepted_types_contains(embed->last.drop_widget, type)) {
                        /*
                         * setup the event struct
                         */
                        ev.type = type;
                        ev.data = data;
                        ev.len = len;
                        ev.format = format;
                        ewl_callback_call_with_event_data(embed->last.drop_widget,
                                          EWL_CALLBACK_DND_DATA_RECEIVED, &ev);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed where the selection data request event is to occur
 * @param handle: The DND handle to work with
 * @param type: The type to feed
 * @return Returns no value.
 * @brief Sends the request event for selection data received into an embed.
 */
void
ewl_embed_dnd_data_request_feed(Ewl_Embed *embed, void *handle, char *type)
{
        Ewl_Event_Dnd_Data_Request ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);
        /*
         * If a widget is expecting DND data, send the data to the widget
         */
        if (embed->last.drag_widget) {
                if (ewl_dnd_provided_types_contains(embed->last.drag_widget, type)) {
                        /*
                         * setup the event struct
                         */
                        ev.handle = handle;
                        ev.type = type;
                        ewl_callback_call_with_event_data(embed->last.drag_widget,
                                                  EWL_CALLBACK_DND_DATA_REQUEST, &ev);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Sends a selection clear event to the widget with the text selection
 */
void
ewl_embed_selection_text_clear_feed(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        /* if do not have a widget with selection we are done */
        if (!ewl_embed_primary_selection_widget)
                DRETURN(DLEVEL_STABLE);

        ewl_callback_del(ewl_embed_primary_selection_widget,
                                EWL_CALLBACK_DESTROY,
                                ewl_embed_selection_cb_destroy);

        ewl_callback_call(ewl_embed_primary_selection_widget,
                                EWL_CALLBACK_SELECTION_CLEAR);

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
        DCHECK_PARAM_PTR(path);

        ecore_list_first_goto(ewl_embed_list);
        while ((e = ecore_list_next(ewl_embed_list))) {
                if (REALIZED(e))
                        evas_font_path_append(e->canvas, path);
        }

        ecore_list_append(ewl_theme_font_path_get(), strdup(path));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param window: the evas window to search for on the list of embeds
 * @return Returns the found embed on success, NULL on failure.
 * @brief Find an ewl embed by its evas window
 */
Ewl_Embed *
ewl_embed_canvas_window_find(Ewl_Embed_Window *window)
{
        Ewl_Embed *retemb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(window, NULL);

        ecore_list_first_goto(ewl_embed_list);
        while ((retemb = ecore_list_next(ewl_embed_list)) != NULL) {
                if (retemb->canvas_window == window)
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
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        while (w->parent)
                w = w->parent;

        if (!ewl_widget_toplevel_get(w))
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
ewl_embed_object_cache(Ewl_Embed *e, void *obj)
{
        const Eina_List *clippees;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(obj);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

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
                Ecore_List *obj_list;
                const char *type;

                type = evas_object_type_get(obj);

                /* reset the file and the key of the edje object */
                if (!strcmp(type, "edje"))
                        edje_object_file_set(obj, "", "");

                obj_list = ecore_hash_get(e->obj_cache, (void *)type);
                if (!obj_list) {
                        obj_list = ecore_list_new();
                        ecore_hash_set(e->obj_cache, (void *)type, obj_list);
                }
                ecore_list_prepend(obj_list, obj);
        }
        else
                ewl_canvas_object_destroy(obj);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: embed to request a cached object
 * @param type: the type of object requested
 * @return Returns an Evas_Object of the specified type on success.
 * @brief Retrieves an object of type @a type from the embed cache, or NULL
 * if none found
 */
void *
ewl_embed_object_request(Ewl_Embed *e, char *type)
{
        Evas_Object *obj = NULL;
        Ecore_List *obj_list;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(e, NULL);
        DCHECK_PARAM_PTR_RET(type, NULL);
        DCHECK_TYPE_RET(e, EWL_EMBED_TYPE, NULL);

        if (!e->obj_cache)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        obj_list = ecore_hash_get(e->obj_cache, type);
        if (obj_list) obj = ecore_list_first_remove(obj_list);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_embed_tab_order_insert(e, w, ecore_list_count(e->tab_order));

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!ewl_widget_parent_of(EWL_WIDGET(e), w))
                DRETURN(DLEVEL_STABLE);

        /* do nothing if this widget isn't focusable */
        if (!ewl_widget_focusable_get(w))
                DRETURN(DLEVEL_STABLE);

        current_idx = ecore_dlist_index(e->tab_order);

        /* make sure this widget isn't already in the list */
        if (ewl_widget_in_tab_list_get(EWL_OBJECT(w))
                        && ecore_dlist_goto(e->tab_order, w)) {
                int del_idx;

                /* if this widget was before or at our current focused
                 * widget then we need to decrement our counter */
                del_idx = ecore_dlist_index(e->tab_order);
                if (del_idx <= current_idx) current_idx --;

                ecore_dlist_remove(e->tab_order);
        }

        ecore_dlist_index_goto(e->tab_order, idx);
        ecore_dlist_insert(e->tab_order, w);

        /* if we inserted before or at our currently focused item then we
         * need to advance our current item to the correct spot */
        if (current_idx <= (int)idx) current_idx ++;
        ecore_dlist_index_goto(e->tab_order, current_idx);

        ewl_widget_in_tab_list_set(w, TRUE);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(after);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(after, EWL_WIDGET_TYPE);

        cur_idx = ecore_dlist_index(e->tab_order);
        if (!ecore_dlist_goto(e->tab_order, after))
        {
                ewl_embed_tab_order_append(e, w);
                DRETURN(DLEVEL_STABLE);
        }

        idx = ecore_dlist_index(e->tab_order);
        ecore_dlist_index_goto(e->tab_order, cur_idx);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(before);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(before, EWL_WIDGET_TYPE);

        cur_idx = ecore_dlist_index(e->tab_order);
        if (!ecore_dlist_goto(e->tab_order, before))
        {
                ewl_embed_tab_order_prepend(e, w);
                DRETURN(DLEVEL_STABLE);
        }

        idx = ecore_dlist_index(e->tab_order);
        ecore_dlist_index_goto(e->tab_order, cur_idx);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ecore_dlist_goto(e->tab_order, w))
                ecore_dlist_remove(e->tab_order);

        ewl_widget_in_tab_list_set(w, FALSE);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        ewl_embed_tab_order_change(e, ecore_dlist_next,
                                        ecore_dlist_first_goto);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        ewl_embed_tab_order_change(e, ecore_dlist_previous,
                                        ecore_dlist_last_goto);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(change);
        DCHECK_PARAM_PTR(cycle);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

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
 * @param w: The widget to set as having focus, NULL is valid value and
 *           signifies that no widget has the focus.
 * @return Returns no value.
 * @brief Set the current focused widget in the embed
 */
void
ewl_embed_focused_widget_set(Ewl_Embed *embed, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (embed->last.focused && (embed->last.focused != w))
                ewl_callback_call_with_event_data(embed->last.focused,
                                EWL_CALLBACK_FOCUS_OUT, w);

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
        DCHECK_PARAM_PTR_RET(embed, NULL);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, NULL);

        DRETURN_PTR(embed->last.focused, DLEVEL_STABLE);
}

/**
 * Find a valid parent of the cleaned up widget.
 */
static Ewl_Widget *
ewl_embed_info_parent_find(Ewl_Widget *w)
{
        Ewl_Widget *temp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        for (temp = w->parent; temp; temp = temp->parent) {
                if (VISIBLE(temp) && !DISABLED(temp) && !DESTROYED(temp))
                        break;
        }

        DRETURN_PTR(temp, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(e);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_state_remove(w, EWL_FLAG_STATE_PRESSED);

        if ((w == e->last.focused)
                        || (RECURSIVE(w)
                                && ewl_widget_parent_of(w, e->last.focused)))
        {
                Ewl_Widget *new_focused; 
                
                /* we pass simply the the focus to the next possible parent */
                new_focused = ewl_embed_info_parent_find(w);
                if (!new_focused)
                        e->last.focused = NULL;
                else
                        ewl_widget_focus_send(new_focused);
        }

        if ((w == e->last.clicked)
                        || (RECURSIVE(w)
                                && ewl_widget_parent_of(w, e->last.clicked)))
                e->last.clicked = ewl_embed_info_parent_find(w);

        if ((w == e->last.mouse_in)
                        || (RECURSIVE(w)
                                && ewl_widget_parent_of(w, e->last.mouse_in)))
        {
                Ewl_Event_Mouse_Out ev;

                ev.base.modifiers = ewl_ev_modifiers_get();
                ev.base.x = ewl_embed_last_mouse_x - e->x;
                ev.base.y = ewl_embed_last_mouse_y - e->y;
                /* XXX we probably need to re-adjust the mouse cursor here */
                ewl_callback_call_with_event_data(e->last.mouse_in,
                                                  EWL_CALLBACK_MOUSE_OUT, &ev);
                e->last.mouse_in = ewl_embed_info_parent_find(w);
        }

        if ((w == e->last.drop_widget)
                        || (RECURSIVE(w)
                                && ewl_widget_parent_of(w, e->last.drop_widget)))
                e->last.drop_widget = ewl_embed_info_parent_find(w);

        if ((w == e->last.drag_widget)
                        || (RECURSIVE(w)
                                && ewl_widget_parent_of(w, e->last.drag_widget)))
                e->last.drag_widget = ewl_embed_info_parent_find(w);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        if (e->canvas) {
                if (x) *x = (int)(evas_coord_world_x_to_screen(e->canvas,
                                                        (Evas_Coord)(xx)));
                if (y) *y = (int)(evas_coord_world_y_to_screen(e->canvas,
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
        int pointer = 0;
        Ewl_Cursor *argb;
        Ewl_Embed *embed;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        embed = ewl_embed_widget_find(w);
        if (!embed) DRETURN(DLEVEL_STABLE);

        if ((argb = ewl_attach_get(w, EWL_ATTACH_TYPE_MOUSE_ARGB_CURSOR))) {
                pointer = argb->handle;
        }

        if (!pointer) 
        {
                if (!(pointer = ewl_attach_mouse_cursor_get(w)))
                        pointer = EWL_MOUSE_CURSOR_LEFT_PTR;
        }
        ewl_engine_pointer_set(embed, pointer);
        embed->cursor = pointer;

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        if (x) *x = e->x;
        if (y) *y = e->y;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to query for window-position
 * @param w: a pointer to the integer that should receive the width
 * @param h: a pointer to the integer that should receive the height
 * @return Returns no value.
 * @brief Retrieve the size of the root window
 *
 * Stores the root window size into the parameters @a w and @a h.
 */
void
ewl_embed_desktop_size_get(Ewl_Embed *e, int *w, int *h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        ewl_engine_desktop_size_get(e, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to freeze redraws and canvas events
 * @return Returns no value.
 * @brief Freeze redraws and events on the specified embed.
 */
void
ewl_embed_freeze(Ewl_Embed *e)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        /*
         * Global freeze on theme events while theme's are being manipulated.
         */
        ewl_engine_theme_freeze(e);
        ewl_engine_canvas_freeze(e);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the embed to thaw redraws and canvas events
 * @return Returns no value.
 * @brief Thaw redraws and events on the specified embed.
 */
void
ewl_embed_thaw(Ewl_Embed *e)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_EMBED_TYPE);

        /*
         * Thaw the theme and canvas for this embed.
         */
        ewl_engine_theme_thaw(e);
        ewl_engine_canvas_thaw(e);

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
ewl_embed_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        emb = EWL_EMBED(w);

        ewl_embed_freeze(emb);

        if (!emb->ev_clip) {
                /*
                 * first try to get the ev_clip from the cach
                 */
                emb->ev_clip = ewl_embed_object_request(emb, "rectangle");
                if (!emb->ev_clip)
                        emb->ev_clip = evas_object_rectangle_add(emb->canvas);

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
                                EVAS_CALLBACK_MOUSE_OUT,
                                ewl_embed_evas_cb_mouse_out, emb);
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_MOUSE_DOWN,
                                ewl_embed_evas_cb_mouse_down, emb);
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_MOUSE_UP,
                                ewl_embed_evas_cb_mouse_up, emb);
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_MOUSE_MOVE,
                                ewl_embed_evas_cb_mouse_move, emb);
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_MOUSE_WHEEL,
                                ewl_embed_evas_cb_mouse_wheel, emb);

                /*
                 * Catch key events processed through the evas
                 */
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_KEY_DOWN,
                                ewl_embed_evas_cb_key_down, emb);
                evas_object_event_callback_add(emb->smart,
                                EVAS_CALLBACK_KEY_UP, ewl_embed_evas_cb_key_up,
                                emb);
        }

        if (emb->dnd_count)
                ewl_engine_embed_dnd_aware_set(emb);

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
ewl_embed_cb_unrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        emb = EWL_EMBED(w);
        if (emb->ev_clip) {
                ewl_canvas_object_destroy(emb->ev_clip);
                emb->ev_clip = NULL;
        }

        if (emb->smart) {
                evas_object_smart_data_set(emb->smart, NULL);
                ewl_canvas_object_destroy(emb->smart);
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
ewl_embed_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        emb = EWL_EMBED(w);
        if (emb->ev_clip) {
                evas_object_move(emb->ev_clip, (Evas_Coord)(CURRENT_X(w)),
                                 (Evas_Coord)(CURRENT_Y(w)));
                evas_object_resize(emb->ev_clip, (Evas_Coord)(CURRENT_W(w)),
                                   (Evas_Coord)(CURRENT_H(w)));
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
ewl_embed_cb_focus_out(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        emb = EWL_EMBED(w);
        if (!emb->last.focused)
                DRETURN(DLEVEL_STABLE);

        /* since we lose focus here, it cannot be a double or triple click,
         * hence feed one */
        if (ewl_widget_state_has(emb->last.focused, EWL_FLAG_STATE_PRESSED))
                ewl_embed_mouse_up_feed(emb, 1, 1, 0, 0, ewl_ev_modifiers_get());

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
void ewl_embed_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        emb = EWL_EMBED(w);
        if (ecore_list_goto(ewl_embed_list, w))
                ecore_list_remove(ewl_embed_list);

        ewl_embed_cache_cleanup(emb);

        ecore_dlist_destroy(emb->tab_order);
        emb->tab_order = NULL;

        if (emb == ewl_embed_active_embed)
                ewl_embed_active_embed = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to set dnd aware
 * @return Returns no value.
 * @brief Set an embed as being DND aware
 */
void
ewl_embed_dnd_aware_set(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        if (REALIZED(embed) && (embed->dnd_count == 0))
                ewl_engine_embed_dnd_aware_set(embed);
        embed->dnd_count++;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to remove dnd aware
 * @return Returns no value.
 * @brief Cancels an embed as being DND aware
 */
void
ewl_embed_dnd_aware_remove(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        /* FIXME: Need to remove the XdndAware property on refcount == 0
        if (REALIZED(embed) && (embed->dnd_count == 0))
                ewl_engine_embed_dnd_aware_set(embed); */
        embed->dnd_count--;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param emb: The embed to set the selection on
 * @param w:   The widget holding the selection
 * @param txt: The text to set into the selection
 * @return Returns no value.
 *
 * @brief This will set the given @a txt as the primary selection text on the 
 * window or clear the text if @a txt is NULL
 */
void
ewl_embed_selection_text_set(Ewl_Embed *emb, Ewl_Widget *w, const char *txt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(emb);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(emb, EWL_EMBED_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* no matter what the new text is, we need to tell the previous widget
         * that it no longer holds the selection */
        if (ewl_embed_primary_selection_widget != w)
        {
                ewl_embed_selection_text_clear_feed();

                if (txt)
                {
                        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
                                        ewl_embed_selection_cb_destroy, NULL);
                        ewl_embed_primary_selection_widget = w;
                }
        }
        else if (!txt)
                /* We don't need to inform the widget because it was the
                 * widget itsself that reported use that there is no selection
                 */
                ewl_embed_primary_selection_widget = NULL;

        ewl_engine_embed_selection_text_set(emb, txt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed to inidicate if it is rendered by ewl
 * @param f: boolean to determine if the embed is rendered by ewl or not
 * @return Returns no value.
 * @brief Sets the boolean flag in the embed to determine if the embed is
 *      rendered by ewl
 * 
 * If the flag is set to true, ewl_engine_canvas_render() is called by ewl
 * for this embed during the render phase, other wise the embed or another
 * infrastructure has to render the canvas itself. For example a plain
 * ewl_embed (what you get with ewl_embed_new()) with the evas backend will
 * have this flag set to @c FALSE by default, because the evas_render() call
 * is done by ecore_evas. For a ewl_window, however, the flag is set to
 * @c TRUE by default, so ewl will call the canvas render function.
 *
 * Use this function only if you really know what you are doing. The default
 * setting should work fine in anycase.
 */
void
ewl_embed_render_set(Ewl_Embed *embed, unsigned int f)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        embed->render = !!f;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param embed: the embed
 * @return Returns a boolean indicating if the embed is rendered by ewl
 * @brief Retrieve the boolean value that indicates if the emebd is rendered 
 *      by ewl.
 */
unsigned int
ewl_embed_render_get(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, FALSE);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, FALSE);

        DRETURN_INT(embed->render, DLEVEL_STABLE);
}

/**
 * @param emb: The embed to work with
 * @return Returns no value
 * @brief This will destroy the object cache associated with the embed object
 */
void
ewl_embed_cache_cleanup(Ewl_Embed *emb)
{
        Ecore_List *key_list;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(emb);
        DCHECK_TYPE(emb, EWL_EMBED_TYPE);

        if (!emb->obj_cache) DRETURN(DLEVEL_STABLE);

        key_list = ecore_hash_keys(emb->obj_cache);
        if (key_list) {
                char *key;
                Evas_Object *obj;
                Ecore_List *obj_list;

                /*
                 * Iterate over all object types destroying them as we go. No
                 * need to free the key string.
                 */
                while ((key = ecore_list_first_remove(key_list))) {
                        /*
                         * Now queue all objects for destruction.
                         */
                        obj_list = ecore_hash_remove(emb->obj_cache, key);
                        while ((obj = ecore_list_first_remove(obj_list)))
                                ewl_canvas_object_destroy(obj);

                        IF_FREE_LIST(obj_list);
                }
                IF_FREE_LIST(key_list);
        }
        IF_FREE_HASH(emb->obj_cache);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * Remove the reference of the primary selection widget since it is now
 * destroyed
 */
static void
ewl_embed_selection_cb_destroy(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        ewl_embed_primary_selection_widget = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_del(Evas_Object *obj)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        if (emb) {
                emb->smart = NULL;
                ewl_widget_unrealize(EWL_WIDGET(emb));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        if (emb)
                ewl_object_position_request(EWL_OBJECT(emb),
                                                (int)(x), (int)(y));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        if (emb)
                ewl_object_size_request(EWL_OBJECT(emb), (int)(w), (int)(h));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_show(Evas_Object *obj)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        if (emb) ewl_widget_show(EWL_WIDGET(emb));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_hide(Evas_Object *obj)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        if (emb) ewl_widget_hide(EWL_WIDGET(emb));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_clip_set(Evas_Object *obj, Evas_Object *clip)
{
        Ewl_Embed *emb;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        w = EWL_WIDGET(emb);
        if (emb && w->fx_clip_box && (clip != w->fx_clip_box))
                evas_object_clip_set(w->fx_clip_box, clip);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_smart_cb_clip_unset(Evas_Object *obj)
{
        Ewl_Embed *emb;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(obj);

        emb = evas_object_smart_data_get(obj);
        w = EWL_WIDGET(emb);
        if (emb && w->fx_clip_box)
                evas_object_clip_unset(w->fx_clip_box);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_mouse_out(void *data, Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__, void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Mouse_Out *ev = event_info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        embed = data;

        if (ev->canvas.x < CURRENT_X(embed)
                        || ev->canvas.x >= (CURRENT_X(embed) + CURRENT_W(embed))
                        || ev->canvas.y < CURRENT_Y(embed)
                        || ev->canvas.y >= (CURRENT_Y(embed) + CURRENT_H(embed)))
                ewl_embed_mouse_out_feed(embed, ev->canvas.x, ev->canvas.y,
                                                 ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_mouse_down(void *data, Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__, void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Mouse_Down *ev;
        int clicks;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        ev = event_info;
        embed = data;

        /* Proper number of clicks here */
        if (ev->flags & EVAS_BUTTON_TRIPLE_CLICK)
                clicks = 3;
        else if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
                clicks = 2;
        else
                clicks = 1;

        /* Apparently this only occurs when embedding widgets in an
         * edje using program, so there shouldn't be any changes to normal
         * use
         */
        ewl_embed_mouse_down_feed(embed, ev->button, clicks, ev->canvas.x,
                                  ev->canvas.y, ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_mouse_up(void *data, Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__, void *event_info)
{
        int clicks;
        Ewl_Embed *embed;
        Evas_Event_Mouse_Up *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        ev = event_info;
        embed = data;

        /* Proper number of clicks here */
        if (ev->flags & EVAS_BUTTON_TRIPLE_CLICK)
                clicks = 3;
        else if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)
                clicks = 2;
        else
                clicks = 1;

        ewl_embed_mouse_up_feed(embed, ev->button, clicks, ev->canvas.x,
                                  ev->canvas.y, ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_mouse_move(void *data, Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__, void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Mouse_Move *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        ev = event_info;
        embed = data;
        ewl_embed_mouse_move_feed(embed, ev->cur.canvas.x, ev->cur.canvas.y,
                                                  ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_mouse_wheel(void *data, Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__,
                                void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Mouse_Wheel *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        ev = event_info;
        embed = data;

        ewl_embed_mouse_wheel_feed(embed, ev->canvas.x, ev->canvas.y, ev->z,
                                        ev->direction, ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_key_down(void *data, Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__, void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Key_Down *ev;
        unsigned int key_modifiers = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

        ev = event_info;
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

        if (!ev->string || iscntrl(*ev->string))
                ewl_embed_key_down_feed(embed, ev->keyname,
                                ewl_ev_modifiers_get());
        else
                ewl_embed_key_down_feed(embed, ev->string,
                                ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_embed_evas_cb_key_up(void *data, Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__, void *event_info)
{
        Ewl_Embed *embed;
        Evas_Event_Key_Down *ev = event_info;
        unsigned int key_modifiers = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_EMBED_TYPE);

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

        if (!ev->string || iscntrl(*ev->string))
                ewl_embed_key_up_feed(embed, ev->keyname,
                                        ewl_ev_modifiers_get());
        else
                ewl_embed_key_up_feed(embed, ev->string,
                                        ewl_ev_modifiers_get());

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

