/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#include <Evas.h>
#include <Edje.h>

static Ecore_Hash *ewl_widget_name_table = NULL;
static Ecore_Hash *ewl_widget_data_table = NULL;
static Evas_Smart *widget_smart = NULL;

static void ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r,
                                                int *t, int *b);
static void ewl_widget_theme_insets_get(Ewl_Widget *w, int *l, int *r,
                                                int *t, int *b);
static void ewl_widget_appearance_part_text_apply(Ewl_Widget *w,
                                                  const char *part, const char *text);
static void ewl_widget_layer_stack_add(Ewl_Widget *w);
static void ewl_widget_layer_update(Ewl_Widget *w);
static Evas_Object *ewl_widget_layer_neighbor_find_above(Ewl_Widget *w);
static Evas_Object *ewl_widget_layer_neighbor_find_below(Ewl_Widget *w);

static void ewl_widget_name_table_shutdown(void);

/**
 * @return Returns a newly allocated widget on success, NULL on failure.
 * @brief Allocate a new widget.
 *
 * Do not use this function unless you know what you are doing! It is only
 * intended to easily create custom widgets that are not containers.
 */
Ewl_Widget *
ewl_widget_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Widget, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_widget_init(w)) {
                FREE(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param w: the widget to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widget to default values and callbacks
 *
 * The widget w is initialized to default values and is
 * assigned the default callbacks.
 */
int
ewl_widget_init(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);

        ewl_widget_inherit(w, EWL_WIDGET_TYPE);

        /*
         * Set size fields on the object
         */
        if (!ewl_object_init(EWL_OBJECT(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        /*
         * Set up the necessary theme structures
         */
        if (!ewl_theme_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_state_remove(w, EWL_FLAGS_STATE_MASK);

        /*
         * Add the common callbacks that all widgets must perform
         */
        ewl_callback_append(w, EWL_CALLBACK_SHOW, ewl_widget_cb_show, NULL);
        ewl_callback_append(w, EWL_CALLBACK_HIDE, ewl_widget_cb_hide, NULL);
        ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_widget_cb_reveal, NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_widget_cb_obscure,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_REALIZE, ewl_widget_cb_realize,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, ewl_widget_cb_unrealize,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_widget_cb_configure,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_REPARENT, ewl_widget_cb_reparent,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_ENABLE, ewl_widget_cb_enable,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_DISABLE,
                                ewl_widget_cb_disable, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN, ewl_widget_cb_focus_in,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT, ewl_widget_cb_focus_out,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_IN, ewl_widget_cb_mouse_in,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_OUT, ewl_widget_cb_mouse_out,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_widget_cb_mouse_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, ewl_widget_cb_mouse_up,
                                NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_widget_cb_mouse_move, NULL);

        /* widgets can take focus by default */
        ewl_widget_focusable_set(w, TRUE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to name
 * @param name: the new name for the widget
 * @return Returns no value.
 * @brief Name the specified widget.
 */
void
ewl_widget_name_set(Ewl_Widget *w, const char *name)
{
        char *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!ewl_widget_name_table)
        {
                ewl_widget_name_table = ecore_hash_new(ecore_str_hash,
                                                        ecore_str_compare);
                ecore_hash_free_key_cb_set(ewl_widget_name_table,
                                                ECORE_FREE_CB(free));
                ewl_shutdown_add(ewl_widget_name_table_shutdown);
        }

        t = (name ? strdup(name) : NULL);
        ewl_attach_name_set(w, t);

        if (ewl_widget_name_table)
                ecore_hash_set(ewl_widget_name_table, t, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the name
 * @return Returns an pointer to an allocated name string on success.
 * @brief Get the name for the specified widget.
 */
const char *
ewl_widget_name_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        DRETURN_PTR(ewl_attach_name_get(w), DLEVEL_STABLE);
}

/**
 * @param name: the name of the widget to retrieve
 * @return Returns an pointer a matched widget on success.
 * @brief Find a widget identified by a name.
 */
Ewl_Widget *
ewl_widget_name_find(const char *name)
{
        Ewl_Widget *match = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(name, NULL);

        if (ewl_widget_name_table)
                match = EWL_WIDGET(ecore_hash_get(ewl_widget_name_table,
                                                        (void *)name));

        DRETURN_PTR(match, DLEVEL_STABLE);
}

/**
 * @param w: the widget to realize
 * @return Returns no value.
 * @brief Realize the specified widget.
 *
 * The specified widget is realized, ie. actually displayed to the screen.
 */
void
ewl_widget_realize(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (REALIZED(w))
                DRETURN(DLEVEL_STABLE);

        if (ewl_widget_queued_has(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL)
                        && !ewl_widget_queued_has(w,
                                                EWL_FLAG_QUEUED_PROCESS_REVEAL))
                ewl_realize_cancel_request(w);

        /*
         * The parent's realize function will get us here again.
         */
        if (w->parent && !REALIZED(w->parent))
                ewl_widget_realize(w->parent);

        /*
         * The parent should be realized at this point, and we can handle
         * realizing ourselves.
         */
        if (w->parent || ewl_widget_toplevel_get(w)) {
                ewl_widget_queued_add(w, EWL_FLAG_QUEUED_PROCESS_REVEAL);
                ewl_callback_call(w, EWL_CALLBACK_REALIZE);
                ewl_widget_queued_remove(w, EWL_FLAG_QUEUED_PROCESS_REVEAL);

                ewl_widget_visible_add(w, EWL_FLAG_VISIBLE_REALIZED);
                ewl_widget_obscure(w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to unrealize
 * @return Returns no value.
 * @brief Unrealize the specified widget
 *
 * The specified widget is unrealized, ie. no longer displayed to the screen.
 */
void
ewl_widget_unrealize(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ewl_widget_queued_has(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL))
                ewl_realize_cancel_request(w);

        if (!REALIZED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_obscure(w);

        ewl_callback_call(w, EWL_CALLBACK_UNREALIZE);
        ewl_widget_visible_remove(w, EWL_FLAG_VISIBLE_REALIZED);
        
        if (VISIBLE(w) && w->parent)
                ewl_container_child_hide_call(EWL_CONTAINER(w->parent), w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as revealed
 * @return Returns no value.
 * @brief Indicate a widget is revealed.
 */
void
ewl_widget_reveal(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Already revealed widgets can be skipped, as can unrealized widgets,
         * unless the are already queued for reveal, ie. in the realize process.
         */
        if (REVEALED(w) || (!REALIZED(w) && !ewl_widget_queued_has(w,
                                        EWL_FLAG_QUEUED_PROCESS_REVEAL)))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_visible_add(w, EWL_FLAG_VISIBLE_REVEALED);

        emb = ewl_embed_widget_find(w);
        if (emb && emb->canvas)
                ewl_callback_call(w, EWL_CALLBACK_REVEAL);

        ewl_widget_configure(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as obscured
 * @return Returns no value.
 * @brief Indicate a widget is obscured.
 */
void
ewl_widget_obscure(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        if (!REVEALED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_visible_remove(w, EWL_FLAG_VISIBLE_REVEALED);

        if (REALIZED(w) || ewl_widget_queued_has(w,
                                EWL_FLAG_QUEUED_SCHEDULED_REVEAL))
                ewl_callback_call(w, EWL_CALLBACK_OBSCURE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be marked as visible
 * @return Returns no value.
 * @brief mark a widget as visible
 *
 * Marks the widget as visible so that it will be displayed the next time
 * through the rendering loop. Note that the show callback may be delayed
 * until the widget has been realized.
 */
void
ewl_widget_show(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Nothing to do if already visible.
         */
        if (VISIBLE(w))
                DRETURN(DLEVEL_STABLE);

        /*
         * Flag that this is a visible widget
         */
        ewl_widget_visible_add(w, EWL_FLAG_VISIBLE_SHOWN);

        /*
         * If realized, go about our business, otherwise queue for realize.
         */
        if (REALIZED(w))
                ewl_callback_call(w, EWL_CALLBACK_SHOW);
        else
                ewl_realize_request(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be marked as invisible
 * @return Returns no value.
 * @brief Mark a widget as invisible
 *
 * Marks the widget as invisible so that it will not be displayed the next time
 * through the rendering loop.
 */
void
ewl_widget_hide(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (ewl_widget_queued_has(w, EWL_FLAG_QUEUED_SCHEDULED_REVEAL))
                ewl_realize_cancel_request(w);

        /*
         * Already hidden? Then no work to be done.
         */
        if (HIDDEN(w))
                DRETURN(DLEVEL_STABLE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_info_widgets_cleanup(emb, w);

        ewl_widget_visible_remove(w, EWL_FLAG_VISIBLE_SHOWN);

        if (REALIZED(w))
                ewl_callback_call(w, EWL_CALLBACK_HIDE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be destroyed
 * @return Returns no value.
 * @brief Destroy the specified widget
 *
 * The widget calls it's destroy callback to do any clean up necessary and then
 * free's the widget.
 */
void
ewl_widget_destroy(Ewl_Widget *w)
{
        int i;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DESTROYED(w))
                DRETURN(DLEVEL_STABLE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_info_widgets_cleanup(emb, w);

        /*
         * Request prior to hiding so we can skip resizing for child widgets
         * being hidden.
         */
        ewl_destroy_request(w);

        ewl_widget_hide(w);
        ewl_widget_unrealize(w);

        /*
         * remove ourselves from any containers
         */
        ewl_widget_parent_set(w, NULL);

        /*
         * Clear out the callbacks, this is a bit tricky because we don't want
         * to continue using this widget after the callbacks have been
         * deleted. Clear all callbacks except for the destroy callbacks.
         * This preserves the list of the destroy type so we don't get a segfault.
         *
         * We delete these now so that we can't possibly get any callbacks before the
         * idler kicks in
         */
        for (i = 0; i < (EWL_CALLBACK_MAX + 1); i++)
        {
                if (i == EWL_CALLBACK_DESTROY) continue;
                ewl_callback_del_type(w, i);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to configure
 * @return Returns no value.
 * @brief Initiate configuring of the specified widget
 *
 * The configure callback is triggered for the specified widget, this should
 * adjust the widgets size and position.
 */
void
ewl_widget_configure(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if ((!VISIBLE(w)) || (w->parent && !REVEALED(w->parent)))
                DRETURN(DLEVEL_STABLE);

        ewl_configure_request(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to reparent
 * @return Returns no value.
 * @brief initiate reparent of the specified widget
 *
 * The reparent callback is triggered for the specified widget, this should
 * adjust the widgets attributes based on the new parent.
 */
void
ewl_widget_reparent(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_callback_call_with_event_data(w, EWL_CALLBACK_REPARENT, w->parent);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to own the key value pair
 * @param k: the key that is associated with the data
 * @param v: the data that is to be tracked
 * @return Returns no value.
 * @brief Attach the specified key / value pair to the widget
 *
 * Assigns a key / value pair with k as the key and v as the value to the
 * specified widget w.
 */
void
ewl_widget_data_set(Ewl_Widget *w, void *k, void *v)
{
        Ecore_Hash *w_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(k);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!ewl_widget_data_table)
                ewl_widget_data_table = ecore_hash_new(NULL, NULL);

        w_data = ecore_hash_get(ewl_widget_data_table, w);
        if (!w_data) {
                w_data = ecore_hash_new(NULL, NULL);
                ecore_hash_set(ewl_widget_data_table, w, w_data);
        }

        ecore_hash_set(w_data, k, v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns the deleted value.
 * @brief Remove the specified key / value pair from the widget and
 * return the value.
 *
 * Removes a key / value pair with k as the key from the specified
 * widget w and return the value. @c NULL is returned if there is no
 * stored data or if an error occurs.
 */
void *
ewl_widget_data_del(Ewl_Widget *w, void *k)
{
        void *data;
        Ecore_Hash *w_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_PARAM_PTR_RET(k, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        /* none of the widgets have data attached */
        if (!ewl_widget_data_table)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        w_data = ecore_hash_get(ewl_widget_data_table, w);

        /* our widget has no data attached */
        if (!w_data) DRETURN_PTR(NULL, DLEVEL_STABLE);

        data = ecore_hash_remove(w_data, k);

        if (ecore_hash_count(w_data) == 0) {
                ecore_hash_remove(ewl_widget_data_table, w);
                IF_FREE_HASH(w_data);

                if (ecore_hash_count(ewl_widget_data_table) == 0)
                        IF_FREE_HASH(ewl_widget_data_table);
        }

        DRETURN_PTR(data, DLEVEL_STABLE);
}

/**
 * @param w: the widget that owns the key value pair
 * @param k: the key that is associated with the data
 * @return Returns the value associated with k on success, NULL on failure.
 * @brief retrieve the specified key / value pair from the widget
 *
 * Retrieves a key / value pair with k as the key from the specified widget w.
 */
void *
ewl_widget_data_get(Ewl_Widget *w, void *k)
{
        void *data = NULL;
        Ecore_Hash *w_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_PARAM_PTR_RET(k, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        /* no widgets have data */
        if (!ewl_widget_data_table)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        w_data = ecore_hash_get(ewl_widget_data_table, w);
        if (w_data) data = ecore_hash_get(w_data, k);

        DRETURN_PTR(data, DLEVEL_STABLE);
}

/**
 * @param w: the widget to change the appearance
 * @param appearance: the new key for the widgets appearance
 * @return Returns no value.
 * @brief Change the appearance of the specified widget
 *
 * Changes the key associated with the widgets appearance and calls the theme
 * update callback to initiate the change.
 */
void
ewl_widget_appearance_set(Ewl_Widget *w, const char *appearance)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(appearance);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* make sure we have something to do */
        if (w->appearance && !strcmp(appearance, w->appearance))
                DRETURN(DLEVEL_STABLE);

        /*
         * The base appearance is used for determining the theme key of the
         * widget. Intentionally lose a reference to the ecore string to keep a
         * reference cached for later re-use.
         */
        w->appearance = ecore_string_instance(appearance);
        if (!w->appearance)
                DRETURN(DLEVEL_STABLE);

        /*
         * Recreate the visible components of the widget if necessary.
         */
        if (REALIZED(w)) {
                ewl_widget_unrealize(w);
                ewl_widget_realize(w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the appearance key
 * @return Returns a pointer to the appearance key string on success, NULL on
 * failure.
 * @brief Retrieve the appearance key of the widget
 */
const char *
ewl_widget_appearance_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        DRETURN_PTR(w->appearance, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to retrieve the full appearance key
 * @param size: pointer to an int indicating the string length
 * @return Returns a pointer to the full appearance path string on success, NULL on
 * failure.
 * @brief Retrieve the appearance path key of the widget
 */
int
ewl_widget_appearance_path_size_get(Ewl_Widget *w)
{
        int size;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        /*
         * Allocate enough for the appearance plus a leading "/"
         */
        size = 0;
        while (w) {
                size += (w->appearance ? strlen(w->appearance) : 0) + 1;
                w = w->parent;
        }
        size++;

        DRETURN_INT(size, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param buf: The buffer to write into
 * @param size: The buffer size
 * @return Returns the amount of buffer space used
 * @brief Copies the widget appearance into the given buffer and return the
 * total amount of space used.
 */
int
ewl_widget_appearance_path_copy(Ewl_Widget *w, char *buf, int size)
{
        int len, used;

        used = 0;
        len = (w->appearance ? strlen(w->appearance) : 0);

        if (w->parent) {
                used += ewl_widget_appearance_path_copy(w->parent, buf,
                                size - len);
        }

        *(buf + used) = '/';
        used++;
        used += ecore_strlcpy(buf + used,
                        (w->appearance ? w->appearance : ""),
                                size - used);

        DRETURN_INT(used, DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve the full appearance key
 * @return Returns a pointer to the full appearance path string on success, NULL on
 * failure.
 * @brief Retrieve the appearance path key of the widget
 */
char *
ewl_widget_appearance_path_get(Ewl_Widget *w)
{
        char *ret = NULL;
        int len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        len = ewl_widget_appearance_path_size_get(w);
        ret = NEW(char *, len);
        ewl_widget_appearance_path_copy(w, ret, len);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to update the appearance
 * @param state: the new state of the widget
 * @param flag: the flag for the state e.g. EWL_STATE_TRANSIENT
 * @return Returns no value.
 * @brief Update the appearance of the widget to a state
 *
 * Changes the appearance of the widget depending on the state string passed by
 * the state parameter.
 */
void
ewl_widget_state_set(Ewl_Widget *w, const char *state, Ewl_State_Type flag)
{
        Ewl_Event_State_Change ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(state);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Intentionally lose a reference to the ecore string to keep a
         * reference cached for later re-use.
         */
        if (flag == EWL_STATE_PERSISTENT)
                w->theme_state = ecore_string_instance((char *)state);

        if (w->theme_object) {
                if (ewl_config_cache.print_signals)
                        printf("Emitting: %s to %p (%s)\n", state, w,
                                                        w->appearance);

                edje_object_signal_emit(w->theme_object, state, "EWL");
        }

        ev.state = state;
        ev.flag = flag;

        ewl_callback_call_with_event_data(w, EWL_CALLBACK_STATE_CHANGED, &ev);

        DRETURN(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change the parent
 * @param p: the new parent of the widget
 * @return Returns no value.
 * @brief change the parent of the specified widget
 *
 * Changes the parent of the widget w, to the container p. The reparent
 * callback is triggered to notify children of w of the change in parent.
 */
void
ewl_widget_parent_set(Ewl_Widget *w, Ewl_Widget *p)
{
        Ewl_Widget *tmp;
        Ewl_Container *op;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* check if our new parent is the same as our old parent */
        op = EWL_CONTAINER(w->parent);
        if (op == EWL_CONTAINER(p))
                DRETURN(DLEVEL_STABLE);

        /* if no parent, obsure the widget */
        if (!p) ewl_widget_obscure(w);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_info_widgets_cleanup(emb, w);

        /*
         * Verify this will not result in recursively nested widgets.
         */
        tmp = p;
        while (tmp) {
                if (tmp == w) {
                        DWARNING("ERROR: Recursivly nested widgets.");
                        DRETURN(DLEVEL_STABLE);
                }
                tmp = tmp->parent;
        }

        /* this is the last point where we can unrealize the widget with
         * a parent, this is important because else the canvas object cannot
         * be cleaned up without problems */
        if (REALIZED(w))
                ewl_widget_unrealize(w);

        /* 
         * set the parent to NULL before doing the child remove, because
         * ewl_container_child_remove() will call this function if there
         * is still a parent set
        */
        w->parent = NULL;

        /*
         * A widget cannot be the child of multiple widgets, so remove it
         * from a previous parent before adding to this parent.
         */
        if (op) ewl_container_child_remove(op, w);

        /*
         * Set this to the new parent here as child_remove will try to call
         * us again if our parent isn't NULL with a NULL parent. This makes
         * sure the parent is set when the remove is done.
         */
        w->parent = p;

        ewl_widget_reparent(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the parent from
 * @return Returns the parent of the given widget, or NULL if none set
 * @brief Retrieves the parent of the given widget
 */
Ewl_Widget *
ewl_widget_parent_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        DRETURN_PTR(w->parent, DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to change
 * @param part: the theme part name whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of a given Edje-define TEXT part.
 */
static void
ewl_widget_appearance_part_text_apply(Ewl_Widget *w, const char *part, const char *text)
{
        Evas_Coord nw, nh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!w->theme_object)
                DRETURN(DLEVEL_STABLE);

        /*
         * Fill in the default part to use when the key is NULL.
         */
        if (!part || !*part)
                part = ewl_theme_data_str_get(w, "textpart");

        /*
         * Set the text to empty if text is NULL. Edje defaults to using the
         * default value specified in the theme.
         */
        edje_object_part_text_set(w->theme_object, part, (text ? text : ""));
        edje_object_size_min_calc(w->theme_object, &nw, &nh);

        ewl_object_preferred_inner_size_set(EWL_OBJECT(w), (int)nw, (int)nh);

        /* XXX: this is a work-around to let edje show the ... for a
         * trucated text, if we changed the text.
         *
         * remove this lines, when the bug is fixed in edje!
         */
        evas_object_resize(w->theme_object, 0, 0);
        evas_object_resize(w->theme_object, CURRENT_W(w), CURRENT_H(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to change
 * @param part: the theme part name whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of a given Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines TEXT parts, and enables
 * each of those text parts to be changed independently.
 * The text value is recorded in a hash and reapplied if the theme
 * is reloaded for this widget.
 */
void
ewl_widget_appearance_part_text_set(Ewl_Widget *w, const char *part, const char *text)
{
        unsigned int i;
        Ewl_Pair *match = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (part && !*part)
                part = NULL;

        /*
         * Check for an existing instance of the part key.
         */
        if (w->theme_text.list) {
                if (w->theme_text.direct) {
                        match = EWL_PAIR(w->theme_text.list);
                        if (part != match->key ||
                                        (part && strcmp(part, match->key)))
                                match = NULL;
                }
                else {
                        for (i = 0; i < w->theme_text.len; i++) {
                                Ewl_Pair *current = w->theme_text.list[i];
                                if (part == current->key ||
                                                (part && !strcmp(current->key, part))) {
                                        match = current;
                                        break;
                                }
                        }
                }
        }

        /*
         * Part key exists and the value is the same as the current value.
         */
        if (match) {
                       if (text && match->value && !strcmp(text, match->value))
                        DRETURN(DLEVEL_STABLE);

                IF_FREE(match->value);
        }
        else {
                match = NEW(Ewl_Pair, 1);
                if (!match) DRETURN(DLEVEL_STABLE);

                if (part) match->key = ecore_string_instance(part);
                else match->key = NULL;
                w->theme_text.len++;

                if (!w->theme_text.list) {
                        w->theme_text.direct = 1;
                        w->theme_text.list = (Ewl_Pair **)match;
                }
                else {
                        if (w->theme_text.direct) {
                                Ewl_Pair *old = EWL_PAIR(w->theme_text.list);
                                w->theme_text.list = NEW(Ewl_Pair *, 2);
                                w->theme_text.list[0] = old;
                        }
                        else {
                                REALLOC(w->theme_text.list, Ewl_Pair,
                                                w->theme_text.len);
                        }
                        w->theme_text.list[w->theme_text.len - 1] = match;
                }
        }

        /*
         * What should be the default if you enter NULL? A blank string?
         * Revert to the text specified in the Edje? Use blank for now.
         */
        match->value = (text ? strdup(text) : NULL);
        ewl_widget_appearance_part_text_apply(w, match->key, match->value);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to retrieve
 * @param part: the theme part name whose text to retrieve
 * @return Returns NULL on failure, a copy of the current text on success.
 * @brief Retrieve a copy of a parts current text.
 *
 * Get the text of a given Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines TEXT parts, and enables
 * each of those text parts to be retrieved independently.
 *
 * The returned string will only be valid until the next time text is set on
 * this part.
 */
const char *
ewl_widget_appearance_part_text_get(Ewl_Widget *w, const char *part)
{
        unsigned int i;
        Ewl_Pair *match = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        /*
         * Check for an existing instance of the part key.
         */
        if (w->theme_text.list) {
                if (w->theme_text.direct) {
                        match = EWL_PAIR(w->theme_text.list);
                        if (part != match->key &&
                                        (part && match->key &&
                                         strcmp(part, match->key)))
                                match = NULL;
                }
                else {
                        for (i = 0; i < w->theme_text.len; i++) {
                                Ewl_Pair *current = w->theme_text.list[i];
                                if (part == match->key ||
                                                (part && match->key &&
                                                 !strcmp(part, match->key))) {
                                        match = current;
                                        break;
                                }
                        }
                }
        }

        DRETURN_PTR((match ? match->value : NULL), DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to change
 * @param text: the new text to change to
 * @return Returns no value.
 * @brief Change the text of the given theme part of a widget
 *
 * Changes the text of an Edje-define TEXT part.  This is for
 * widgets whose Edje appearance defines a TEXT part, and identifies
 * it with with a data item called "/WIDGET/textpart".
 * The text value is recorded in a hash and reapplied if the theme
 * is reloaded for this widget.
 */
void
ewl_widget_appearance_text_set(Ewl_Widget *w, const char *text)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_appearance_part_text_set(w, NULL, text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget whose text to retrieve
 * @return Returns the current text on success, NULL on failure.
 * @brief Retrieve the text of the given theme part of a widget
 *
 * @note The returned value will only be valid until the next time
 * ewl_widget_appearance_text_set() is called on this widget.
 */
const char *
ewl_widget_appearance_text_get(Ewl_Widget *w)
{
        const char *part, *match = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        part = ewl_theme_data_str_get(w, "textpart");
        if (part) match = ewl_widget_appearance_part_text_get(w, part);
        if (!match) match = ewl_widget_appearance_part_text_get(w, NULL);

        DRETURN_PTR(match, DLEVEL_STABLE);
}

/**
 * @param w: the widget to re-enable
 * @return Returns no value.
 * @brief Re-enable a disabled widget
 *
 * Re-enables a previously disabled widget.
 */
void
ewl_widget_enable(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w)) {
                ewl_widget_state_remove(w, EWL_FLAGS_STATE_MASK);
                ewl_widget_state_add(w, EWL_FLAG_STATE_NORMAL);
                ewl_callback_call(w, EWL_CALLBACK_WIDGET_ENABLE);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to disable
 * @return Returns no value. Disables a specified widget.
 * @brief Prevent a widget from receiving any events
 *
 * This prevents that widget from receiving any user input events.
 */
void
ewl_widget_disable(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!DISABLED(w)) {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                /* call the callback first, so we give possible child widgets
                 * the way to remove them from the embed info widget list */
                ewl_callback_call(w, EWL_CALLBACK_WIDGET_DISABLE);
                /* and now remove us self from the info widget list */
                ewl_embed_info_widgets_cleanup(emb, w);
                /* finally remove the state flags and set us to disabled*/
                ewl_widget_state_remove(w, EWL_FLAGS_STATE_MASK);
                ewl_widget_state_add(w, EWL_FLAG_STATE_DISABLED);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change relative layers
 * @param layer: the new relative layer of the widget
 * @return Returns no value.
 * @brief Set the relative layer to it's parent
 *
 * Changes the current layer of @a w relative to it's parent. The default
 * value is 0.
 */
void
ewl_widget_layer_priority_set(Ewl_Widget *w, int layer)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        w->layer = layer;
        if (REALIZED(w)) ewl_widget_layer_update(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to retrieve current relative layer
 * @return Returns a widgets current layer relative to it's parent.
 * @brief Retrieve a widgets layer relative to it's parent.
 */
int
ewl_widget_layer_priority_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        DRETURN_INT(w->layer, DLEVEL_STABLE);
}

/**
 * @param w:  the widget to set the top value
 * @param top: TRUE or FALSE
 * @return Returns no value.
 * @brief set the widget to be layered above all other widgets
 *
 * This  set the widget to be layered above all other widgets.
 */
void
ewl_widget_layer_top_set(Ewl_Widget *w, int top)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!!TOPLAYERED(w) == !!top)
                DRETURN(DLEVEL_STABLE);

        if (top)
                ewl_widget_flags_add(w, EWL_FLAG_PROPERTY_TOPLAYERED,
                                        EWL_FLAGS_PROPERTY_MASK);
        else
                ewl_widget_flags_remove(w, EWL_FLAG_PROPERTY_TOPLAYERED,
                                        EWL_FLAGS_PROPERTY_MASK);

        if (REALIZED(w)) {
                ewl_widget_layer_stack_add(w);
                ewl_widget_layer_update(w);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w:  the widget to get the top value
 * @return Returns TRUE or FALSE
 * @brief Returns if the widget will be drawn above all the others
 *
 */
int
ewl_widget_layer_top_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        DRETURN_INT(TOPLAYERED(w), DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the end of the tab order list in the embed
 * that holds it.
 */
void
ewl_widget_tab_order_append(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_append(emb, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the front of the tab order list in the embed
 * that holds it.
 */
void
ewl_widget_tab_order_prepend(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_prepend(emb, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to be moved to the front of the focus list
 * @param idx: The index to insert the tab into
 * @return Returns no value.
 * @brief Changes the order in the embed so @a w receives focus first on tab.
 *
 * This moves the widget @a w to the given index in the tab order list in the embed
 * that holds it.
 */
void
ewl_widget_tab_order_insert(Ewl_Widget *w, unsigned int idx)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_insert(emb, w, idx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to be inserted into the tab order
 * @param before: The widget we are to be inserted before
 * @return Returns no value.
 * @brief Inserts the widget into the tab order before the @a before widget
 */
void
ewl_widget_tab_order_insert_before(Ewl_Widget *w, Ewl_Widget *before)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(before);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(before, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_insert_before(emb, w, before);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to insert into the tab order
 * @param after: The widget to insert after
 * @return Returns no value.
 * @brief Insert the given widget into the tab order after the @a after
 * widget
 */
void
ewl_widget_tab_order_insert_after(Ewl_Widget *w, Ewl_Widget *after)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(after);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(after, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_insert_after(emb, w, after);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to remove from the tab order
 * @return Returns no value.
 * @brief Remove the widget from the tab order
 */
void
ewl_widget_tab_order_remove(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_tab_order_remove(emb, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to set if it accepts or blocks focus changes
 * @param val: TRUE or FALSE on if this widget blocks tabbing off
 * @return Returns no value.
 * @brief Set if the widget should ignore focus changes
 */
void
ewl_widget_ignore_focus_change_set(Ewl_Widget *w, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (val)
                ewl_widget_flags_add(w,
                                EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
                                EWL_FLAGS_PROPERTY_MASK);
        else
                ewl_widget_flags_remove(w,
                                EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
                                EWL_FLAGS_PROPERTY_MASK);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to check if it blocks tab focus
 * @return Returns TRUE if the widget blocks tab focus, FALSE otherwise.
 * @brief Get if the widget is ignoring focus changes
 */
unsigned int
ewl_widget_ignore_focus_change_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        if (ewl_widget_flags_has(EWL_OBJECT(w),
                                        EWL_FLAG_PROPERTY_BLOCK_TAB_FOCUS,
                                        EWL_FLAGS_PROPERTY_MASK))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: The widget to set the focusable values
 * @param val: The focusable value to set
 * @return Returns no value
 * @brief Set if the given widget is focusable or not
 */
void
ewl_widget_focusable_set(Ewl_Widget *w, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (val)
                ewl_widget_flags_add(w,
                                EWL_FLAG_PROPERTY_FOCUSABLE,
                                EWL_FLAGS_PROPERTY_MASK);
        else
                ewl_widget_flags_remove(w,
                                EWL_FLAG_PROPERTY_FOCUSABLE,
                                EWL_FLAGS_PROPERTY_MASK);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the focusable state from
 * @return Returns TRUE if the widget is focusable, FALSE otherwise
 * @brief Checks the focusable state of the widget
 */
unsigned int
ewl_widget_focusable_get(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        if (ewl_widget_flags_has(w, EWL_FLAG_PROPERTY_FOCUSABLE,
                                EWL_FLAGS_PROPERTY_MASK))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to display ancestry tree
 * @return Returns no value.
 * @brief Prints to stdout the tree of widgets that are parents of a widget.
 */
void
ewl_widget_tree_print(Ewl_Widget *w)
{
        int j, i = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        while (w) {
                for (j = 0; j < i; j++)
                        printf("\t");

                ewl_widget_print(w);

                w = w->parent;
                i++;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to print info
 * @return Returns no value.
 * @brief Prints info for debugging a widget's state information.
 */
void
ewl_widget_print(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        printf("%p:%s geometry (%d, %d) %d x %d\n\t%s, %s, %s, %s\n",
                        w, w->appearance,
                        ewl_object_current_x_get(EWL_OBJECT(w)),
                        ewl_object_current_y_get(EWL_OBJECT(w)),
                        ewl_object_current_w_get(EWL_OBJECT(w)),
                        ewl_object_current_h_get(EWL_OBJECT(w)),
                        (VISIBLE(w) ? "visible" : "not visible"),
                        (REALIZED(w) ? "realized" : "not realized"),
                        (DESTROYED(w) ? "destroyed" : "not destroyed"),
                        (DISABLED(w) ? "disabled" : "enabled"));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to print verbose info
 * @return Returns no value.
 * @brief Prints verbose info for debugging a widget's state information.
 */
void
ewl_widget_print_verbose(Ewl_Widget *w)
{
        unsigned int flags;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        flags = ewl_object_fill_policy_get(EWL_OBJECT(w));

        ewl_widget_print(w);
        printf("\tPreferred size: %dx%d",
                        ewl_object_preferred_w_get(EWL_OBJECT(w)),
                        ewl_object_preferred_h_get(EWL_OBJECT(w)));
        printf("\tMinimum size: %dx%d",
                        ewl_object_minimum_w_get(EWL_OBJECT(w)),
                        ewl_object_minimum_h_get(EWL_OBJECT(w)));
        printf("\tMaximum size: %dx%d\n",
                        ewl_object_maximum_w_get(EWL_OBJECT(w)),
                        ewl_object_maximum_h_get(EWL_OBJECT(w)));
        printf("\tInsets: %d, %d, %d, %d\n",
                        ewl_object_insets_left_get(EWL_OBJECT(w)),
                        ewl_object_insets_right_get(EWL_OBJECT(w)),
                        ewl_object_insets_top_get(EWL_OBJECT(w)),
                        ewl_object_insets_bottom_get(EWL_OBJECT(w)));
        printf("\tPadding: %d, %d, %d, %d\n",
                        ewl_object_padding_left_get(EWL_OBJECT(w)),
                        ewl_object_padding_right_get(EWL_OBJECT(w)),
                        ewl_object_padding_top_get(EWL_OBJECT(w)),
                        ewl_object_padding_bottom_get(EWL_OBJECT(w)));
        printf("\tFill policy:");

        if (!flags)
                printf(" NONE");

        else {
                if (flags & EWL_FLAG_FILL_HSHRINKABLE)
                        printf(" HSHRINK");

                if (flags & EWL_FLAG_FILL_HFILL)
                        printf(" HFILL");

                if (flags & EWL_FLAG_FILL_VSHRINKABLE)
                        printf(" VSHRINK");

                if (flags & EWL_FLAG_FILL_VFILL)
                        printf(" VFILL");
        }
        printf("\n");

        flags = ewl_object_alignment_get(EWL_OBJECT(w));

        printf("\tAlignment:");
        if (!flags)
                printf(" CENTER");

        else {
                if (flags & EWL_FLAG_ALIGN_LEFT)
                        printf(" LEFT");

                if (flags & EWL_FLAG_ALIGN_RIGHT)
                        printf(" RIGHT");

                if (flags & EWL_FLAG_ALIGN_TOP)
                        printf(" TOP");

                if (flags & EWL_FLAG_ALIGN_BOTTOM)
                        printf(" BOTTOM");
        }
        printf("\n");

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to mark as internally used
 * @param val: a boolean to indicate the state of the internal flag
 * @return Returns no value.
 * @brief Marks a widget as one used internally to another container.
 *
 * Indicate whether a widget is to be used as an internal decoration on
 * another widget. This allows for using some of the standard container
 * functions to access the contents of complex widgets w/o fear of damaging
 * internal layout structure.
 */
void
ewl_widget_internal_set(Ewl_Widget *w, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (val)
                ewl_widget_flags_add(w, EWL_FLAG_PROPERTY_INTERNAL,
                                EWL_FLAGS_PROPERTY_MASK);
        else
                ewl_widget_flags_remove(w, EWL_FLAG_PROPERTY_INTERNAL,
                                EWL_FLAGS_PROPERTY_MASK);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to mark as unmanaged
 * @param val: a boolean to indicate the state of the unmanaged flag
 * @return Returns no value.
 * @brief Marks a widget to be ignored by the parent container.
 *
 * A widget marked as unmanaged will not be placed by the container. This
 * is important for widget that manage to place them self like a floater.
 * Nevertheless the widget will recieve events through the parent and shares
 * the same layer with its sibling widgets. In most cases you also want to
 * set it as internal.
 *
 * @note You cannot change the unmanaged state if the widget already has
 * a parent.
 */
void
ewl_widget_unmanaged_set(Ewl_Widget *w, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->parent) {
                DWARNING("It is not possible to change the unmanage state "
                                "of a widget that has already a parent!\n");
                DRETURN(DLEVEL_STABLE);
        }
        
        if (val)
                ewl_widget_flags_add(w, EWL_FLAG_PROPERTY_UNMANAGED,
                                        EWL_FLAGS_PROPERTY_MASK);
        else
                ewl_widget_flags_remove(w, EWL_FLAG_PROPERTY_UNMANAGED,
                                        EWL_FLAGS_PROPERTY_MASK);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: the widget to set the inheritance on
 * @param inherit: the string to append to the inheritance
 * @return Returns no value.
 * @brief Appends the given inheritance to this widgets inheritance string.
 */
void
ewl_widget_inherit(Ewl_Widget *widget, const char *inherit)
{
        size_t len;
        char *tmp = NULL;
        const char *tmp2 = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(widget);
        DCHECK_PARAM_PTR(inherit);
        /* can't check the inheritence here as we don't know if the widget
         * has any inheritence yet (ewl_widget_init() calls this function to
         * set the inheritence) */

        len = strlen(inherit) +  3;

        tmp2 = widget->inheritance;
        if (tmp2) len += strlen(tmp2);

        tmp = alloca(sizeof(char) * len);
        if (tmp) {
                size_t used = 0;

                /* Copy the existing inherited types */
                if (tmp2) used = ecore_strlcpy(tmp, tmp2, len);

                /* Insert the leading colon */
                if (used < len - 1) {
                        *(tmp + used) = ':';
                        used++;
                }

                /* Copy newly inherited type */
                if (used < len)
                        used += ecore_strlcpy(tmp + used, inherit, len - used);

                /* Insert trailing colon */
                if (used < len - 1) {
                        *(tmp + used) = ':';
                        used++;
                }

                /* Terminate the string */
                *(tmp + used) = '\0';
        }

        /*
         * Intentionally lose a reference to the ecore string to keep a
         * reference cached for later re-use.
         */
        widget->inheritance = ecore_string_instance(tmp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param widget: the widget to determine if a type is inherited
 * @param type: the type to check for inheritance in the widget
 * @return Returns TRUE if @a w inherited the type @a t, otherwise FALSE.
 * @brief Determine if the widget @a w has inherited from the type @a t.
 */
unsigned int
ewl_widget_type_is(Ewl_Widget *widget, const char *type)
{
        int found = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(widget, FALSE);
        DCHECK_PARAM_PTR_RET(type, FALSE);
        /* we don't check the type of the widget in here or we'll get into
         * an infinate loop ... */

        if (widget->inheritance)
        {
                char *match, *end;

                match = (char *)widget->inheritance;
                end = match + 1;

                while (*end != '\0')
                {
                        int count = 0;

                        /* while the type still matches the current part of
                         * the string */
                        while ((*end != ':') && (*(type + count) == *end))
                        {
                                count ++;
                                end ++;
                        }

                        /* if we ended with the : then this is a match */
                        if ((*end == ':') && (*(type + count) == '\0'))
                        {
                                found = TRUE;
                                break;
                        }

                        /* move to the next set of :s and then move past it */
                        while ((*end != '\0') && (*end != ':')) end ++;
                        if ((*end == ':') && (*(++end) == ':')) end++;
                }
        }

        DRETURN_INT(found, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to query the state of the internal flag
 * @return Returns TRUE if the widget is marked internal, otherwise FALSE.
 * @brief Checks the widget for the internal flag.
 */
unsigned int
ewl_widget_internal_is(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        if (ewl_widget_flags_has(w, EWL_FLAG_PROPERTY_INTERNAL,
                                EWL_FLAGS_PROPERTY_MASK))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to query the state of the unmanaged flag
 * @return Returns TRUE if the widget is marked unmanaged, otherwise FALSE.
 * @brief Checks the widget for the internal flag.
 */
unsigned int
ewl_widget_unmanaged_is(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        DRETURN_INT(UNMANAGED(w), DLEVEL_STABLE);
}

/**
 * @param w: The widget to check
 * @return Returns TRUE if the widget is onscreen
 * @brief Checks if the given widget is currently on screen
 */
unsigned int
ewl_widget_onscreen_is(Ewl_Widget *w)
{
        int onscreen = FALSE;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        /*
         * Until an embed is present, the widget is off screen by default.
         */
        emb = ewl_embed_widget_find(w);
        if (emb) onscreen = TRUE;

        /*
         * We don't need to configure if it's outside the viewable space in
         * it's parent widget. This does not recurse upwards to determine
         * ancestor clipped areas, but does obscure items that are outside the
         * top level container.
         */
        if (w->parent) {
                int x = 0, y = 0;
                int width = 0, height = 0;
                Ewl_Widget *p = w->parent;

                /*
                 * Check if widget is right of the visible area.
                 */
                x = ewl_object_current_x_get(EWL_OBJECT(w));
                if (x > (CURRENT_X(p) + CURRENT_W(p)))
                        onscreen = FALSE;

                if (onscreen) {
                        if (x > (CURRENT_X(emb) + CURRENT_W(emb)))
                                onscreen = FALSE;
                }

                /*
                 * Check if widget is below the visible area.
                 */
                if (onscreen) {
                        y = ewl_object_current_y_get(EWL_OBJECT(w));
                        if (y > (CURRENT_Y(p) + CURRENT_H(p)))
                                onscreen = FALSE;
                }

                if (onscreen) {
                        if (y > (CURRENT_Y(emb) + CURRENT_H(emb)))
                                onscreen = FALSE;
                }

                /*
                 * Check if widget is left of visible area.
                 */
                if (onscreen) {
                        width = ewl_object_current_w_get(EWL_OBJECT(w));
                        if ((x + width) < CURRENT_X(p))
                                onscreen = FALSE;
                }

                if (onscreen) {
                        if ((x + width) < CURRENT_X(emb))
                                onscreen = FALSE;
                }

                /*
                 * Check if widget is above visible area.
                 */
                if (onscreen) {
                        height = ewl_object_current_h_get(EWL_OBJECT(w));
                        if ((y + height) < CURRENT_Y(p))
                                onscreen = FALSE;
                }

                if (onscreen) {
                        if ((y + height) < CURRENT_Y(emb))
                                onscreen = FALSE;
                }
        }

        if (onscreen == TRUE && w->parent) {
                if ((ewl_widget_onscreen_is(w->parent) == FALSE)) onscreen = FALSE;
        }

        DRETURN_INT(onscreen, DLEVEL_STABLE);
}

/**
 * @param w: the widget to mark as unclipped
 * @param val: the state of the clipping flag
 * @return Returns no value.
 * @brief Marks whether the widget should be clipped at it's boundaries.
 */
void
ewl_widget_clipped_set(Ewl_Widget *w, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (val)
                ewl_widget_flags_remove(w, EWL_FLAG_VISIBLE_NOCLIP,
                                        EWL_FLAGS_VISIBLE_MASK);
        else
                ewl_widget_flags_add(w, EWL_FLAG_VISIBLE_NOCLIP,
                                        EWL_FLAGS_VISIBLE_MASK);

        if (!REALIZED(w) || (val && w->fx_clip_box) ||
                        (!val && !w->fx_clip_box))
                DRETURN(DLEVEL_STABLE);

        if (val) {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                if (!emb || !emb->canvas)
                        DRETURN(DLEVEL_STABLE);

                w->fx_clip_box = evas_object_rectangle_add(emb->canvas);
                evas_object_pass_events_set(w->fx_clip_box, TRUE);
                ewl_widget_configure(w);
        }
        else {
                ewl_canvas_object_destroy(w->fx_clip_box);
                w->fx_clip_box = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to check if it clips it's theme object
 * @return Returns TRUE if the widget clips, otherwise FALSE.
 * @brief Checks if a widget clips it's theme object.
 */
unsigned int
ewl_widget_clipped_is(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        if (ewl_widget_flags_has(w, EWL_FLAG_VISIBLE_NOCLIP,
                                        EWL_FLAGS_VISIBLE_MASK))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param c: the widget to compare against
 * @param w: the widget to check parentage
 * @return Returns TRUE if @a c is a parent of @a w, otherwise returns FALSE.
 * @brief Determine if a widget is a parent of another widget
 */
int
ewl_widget_parent_of(Ewl_Widget *c, Ewl_Widget *w)
{
        Ewl_Widget *parent;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);
        DCHECK_TYPE_RET(c, EWL_WIDGET_TYPE, FALSE);

        if (!w) DRETURN_INT(FALSE, DLEVEL_STABLE);

        parent = w;
        while ((parent = parent->parent)) {
                if (parent == c)
                        DRETURN_INT(TRUE, DLEVEL_STABLE);
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to set the specified widget flags
 * @param flags: a bitmask of new flags to be set in the widget
 * @param mask: a bitmask limiting added flags to a certain set
 * @return Returns no value.
 * @brief Add the set of flags specified in @a flags to @a w.
 */
void
ewl_widget_flags_add(Ewl_Widget *w, unsigned int flags, unsigned int mask)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        w->flags |= (flags & mask);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to remove specified state flags
 * @param flags: a bitmask of flags to be removed from the widget
 * @param mask: a bitmask limiting removed flags to a certain set
 * @return Returns no value.
 * @brief Removes the set of state flags specified in @a flags from @a w.
 */
void
ewl_widget_flags_remove(Ewl_Widget *w, unsigned int flags, unsigned int mask)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        w->flags &= ~(flags & mask);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to receive keyboard focus
 * @return Returns no value.
 * @brief Changes the keyboard focus to the widget @a w.
 *
 * This function will send the keyboard focus to the given widget.
 *
 * @note Since the focus handling is done by the embed, this function will only
 *       work proper if the widget has already an embed (in most cases a window)
 *       as an (indirect) parent.
 */
void
ewl_widget_focus_send(Ewl_Widget *w)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);
        if (!emb) DRETURN(DLEVEL_STABLE);

        ewl_embed_active_set(emb, TRUE);
        ewl_embed_focused_widget_set(emb, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the currnetly focused widget.
 * @brief Retrieve the currently focused widget.
 */
Ewl_Widget *
ewl_widget_focused_get(void)
{
        Ewl_Embed *emb;
        Ewl_Widget *w = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        emb = ewl_embed_active_embed_get();
        if (emb) w = ewl_embed_focused_widget_get(emb);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param w: The widget to set the color of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief sets the colour of the widget
 */
void
ewl_widget_color_set(Ewl_Widget *w, unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a)
{
        Ewl_Color_Set *color;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        color = NEW(Ewl_Color_Set, 1);
        color->r = r;
        color->g = g;
        color->b = b;
        color->a = a;
        ewl_attach_color_set(w, color);

        if (w->fx_clip_box)
                evas_object_color_set(w->fx_clip_box, r, g, b, a);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: The widget to get the colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @return Returns no value
 * @brief Gets the colour settings of the widget
 */
void
ewl_widget_color_get(Ewl_Widget *w, unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a)
{
        Ewl_Color_Set *color;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        color = ewl_attach_color_get(w);
        if (!color) DRETURN(DLEVEL_STABLE);

        if (r) *r = color->r;
        if (g) *g = color->g;
        if (b) *b = color->b;
        if (a) *a = color->a;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_layer_stack_add(Ewl_Widget *w)
{
        Evas_Object *smart_parent;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->parent && !REVEALED(w->parent))
                DRETURN(DLEVEL_STABLE);

        if (w->parent && !TOPLAYERED(w))
                smart_parent = w->parent->smart_object;
        else {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                smart_parent = emb->smart;
        }

        evas_object_smart_member_add(w->smart_object, smart_parent);

        if (w->theme_object)
                evas_object_smart_member_add(w->theme_object, w->smart_object);

        if (w->fx_clip_box)
                evas_object_smart_member_add(w->fx_clip_box, w->smart_object);

        if (w->theme_object && w->fx_clip_box)
                evas_object_stack_below(w->theme_object, w->fx_clip_box);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_layer_update(Ewl_Widget *w)
{
        Ewl_Widget *p;
        int layer;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!(p = w->parent))
                DRETURN(DLEVEL_STABLE);

        /* check first if the widget should be on the top */
        if (TOPLAYERED(w))
        {
                evas_object_raise(w->smart_object);
                DRETURN(DLEVEL_STABLE);
        }

        layer = ewl_widget_layer_priority_get(w);
        if (layer == 0)
                evas_object_stack_above(w->smart_object, p->fx_clip_box);

        else if (layer > 0) {
                Evas_Object *above;

                if (!(above = ewl_widget_layer_neighbor_find_above(w)))
                {
                        DWARNING("No object to stack above.");
                        DRETURN(DLEVEL_STABLE);
                }
                evas_object_stack_above(w->smart_object, above);
        }
        else {
                Evas_Object *below;

                if (!(below = ewl_widget_layer_neighbor_find_below(w)))
                {
                        DWARNING("No object to stack below.");
                        DRETURN(DLEVEL_STABLE);
                }
                evas_object_stack_below(w->smart_object, below);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Evas_Object *
ewl_widget_layer_neighbor_find_above(Ewl_Widget *w)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        if (!w->parent)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        o = ol = w->parent->fx_clip_box;

        while ((o = evas_object_above_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found) continue;
                        if (ewl_widget_layer_priority_get(w) <=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}

static Evas_Object *
ewl_widget_layer_neighbor_find_below(Ewl_Widget *w)
{
        Evas_Object *o, *ol;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        if (!w->parent)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        o = ol = w->parent->fx_clip_box;

        while ((o = evas_object_below_get(o)))
        {
                Ewl_Widget *found;

                found = evas_object_data_get(o, "EWL");
                /*
                 * Perhaps it is a cached object so no reason to stop iterating
                 */
                if (found) {
                        /* ignore the widget itself */
                        if (w == found) continue;
                        if (ewl_widget_layer_priority_get(w) >=
                                        ewl_widget_layer_priority_get(found))
                                break;
                        ol = o;
                }
        }

        DRETURN_PTR(ol, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @return Returns no value
 * @brief Free all the widget data
 */
void
ewl_widget_free(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * cleanup the attachment lists
         */
        if (w->attach)
        {
                ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_TOOLTIP);
                ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_COLOR);
                ewl_attach_list_del(w->attach, EWL_ATTACH_TYPE_NAME);
        }

        /*
         * Free up appearance related information
         */
        ewl_theme_widget_shutdown(w);

        IF_RELEASE(w->appearance);
        IF_RELEASE(w->inheritance);
        IF_RELEASE(w->theme_state);

        if (w->theme_text.list) {
                if (w->theme_text.direct) {
                        Ewl_Pair *pair = EWL_PAIR(w->theme_text.list);
                        IF_RELEASE(pair->key);
                        FREE(EWL_PAIR(w->theme_text.list)->value);
                }
                else {
                        unsigned int i;
                        for (i = 0; i < w->theme_text.len; i++) {
                                Ewl_Pair *pair = EWL_PAIR(w->theme_text.list[i]);
                                IF_RELEASE(pair->key);
                                FREE(w->theme_text.list[i]->value);
                                FREE(w->theme_text.list[i]);
                        }
                }

                FREE(w->theme_text.list);
                w->theme_text.len = 0;
        }

        if (ewl_widget_data_table) {
                Ecore_Hash *w_data;

                w_data = ecore_hash_remove(ewl_widget_data_table, w);
                IF_FREE_HASH(w_data);
        }

        FREE(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Every widget must show it's fx_clip_box to be seen
 */
void
ewl_widget_cb_show(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        Ewl_Container *pc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->smart_object) evas_object_show(w->smart_object);
        if (w->fx_clip_box) evas_object_show(w->fx_clip_box);
        if (w->theme_object) evas_object_show(w->theme_object);

        pc = EWL_CONTAINER(w->parent);
        if (pc) ewl_container_child_show_call(pc, w);

        ewl_widget_tab_order_prepend(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Every widget must hide it's fx_clip_box in order to hide
 */
void
ewl_widget_cb_hide(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        Ewl_Container *pc;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_obscure(w);

        /*
         * Notify parent of hidden state.
         */
        pc = EWL_CONTAINER(w->parent);
        if (pc) ewl_container_child_hide_call(pc, w);

        emb = ewl_embed_widget_find(w);
        if (emb) ewl_embed_info_widgets_cleanup(emb, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Request a new set of evas objects now that we're back on screen
 */
void
ewl_widget_cb_reveal(Ewl_Widget *w, void *ev_data __UNUSED__,
                                          void *user_data __UNUSED__)
{
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        emb = ewl_embed_widget_find(w);
        if (!emb || !emb->canvas)
                DRETURN(DLEVEL_STABLE);

        /*
         * Increment the dnd awareness counter on the embed.
         */
        if (ewl_widget_flags_has(w, EWL_FLAG_PROPERTY_DND_TARGET,
                                EWL_FLAGS_PROPERTY_MASK))
                ewl_embed_dnd_aware_set(emb);

        /*
         * Smart Object allocation
         */
        if (!w->smart_object) {
                /*
                 * Attempt to load a cached object first, fallback to adding a
                 * new one.
                 */
                w->smart_object = ewl_embed_object_request(emb, "Ewl Widget Smart Object");
                if (!w->smart_object) {
                        if (!widget_smart) {
                                static const Evas_Smart_Class sc = {
                                        "Ewl Widget Smart Object",
                                        EVAS_SMART_CLASS_VERSION,
                                        NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL
                                };
                                widget_smart = evas_smart_class_new(&sc);
                        }
                        w->smart_object = evas_object_smart_add(emb->canvas, widget_smart);
                }
                evas_object_data_set(w->smart_object, "EWL", w);
        }

        /*
         * No object allocated yet for this widget
         */
        if (!w->theme_object && w->theme_path && w->theme_group) {
                /*
                 * Attempt to load a cached object first, fallback to adding a
                 * new one.
                 */
                w->theme_object = ewl_embed_object_request(emb, "edje");
                if (!w->theme_object)
                        w->theme_object = edje_object_add(emb->canvas);

                /*
                 * Attempt to load the theme object
                 */
                evas_object_repeat_events_set(w->theme_object, 1);
                if (!edje_object_file_set(w->theme_object, w->theme_path, w->theme_group))
                        DWARNING("Error setting edje object %s, %s.",
                                        w->theme_path, w->theme_group);

                /*
                 * If the file failed to load, destroy the unnecessary evas
                 * object.
                 */
                if (edje_object_load_error_get(w->theme_object)) {
                        evas_object_del(w->theme_object);
                        w->theme_object = NULL;
                }

                /*
                 * Set the state of the theme object
                 */
                if (w->theme_state)
                        ewl_widget_state_set(w, (char *)w->theme_state,
                                                EWL_STATE_PERSISTENT);

                if (DISABLED(w))
                        ewl_widget_state_set(w, "disabled",
                                                EWL_STATE_PERSISTENT);

                /*
                 * Apply any text overrides
                 */
                if (w->theme_object && w->theme_text.list) {
                        const char *key;
                               char *value;

                        if (w->theme_text.direct) {
                                key = EWL_PAIR(w->theme_text.list)->key;
                                value = EWL_PAIR(w->theme_text.list)->value;
                                ewl_widget_appearance_part_text_apply(w, key, value);
                        }
                        else {
                                unsigned int i;
                                for (i = 0; i < w->theme_text.len; i++) {
                                        key = w->theme_text.list[i]->key;
                                        value = w->theme_text.list[i]->value;
                                        ewl_widget_appearance_part_text_apply(w, key, value);
                                }
                        }
                }
        }

        /*
         * Create clip box if necessary
         */
        if (!w->fx_clip_box && !ewl_widget_flags_get(w,
                                                EWL_FLAG_VISIBLE_NOCLIP)) {
                w->fx_clip_box = ewl_embed_object_request(emb, "rectangle");
                if (!w->fx_clip_box)
                        w->fx_clip_box = evas_object_rectangle_add(emb->canvas);

                evas_object_pass_events_set(w->fx_clip_box, TRUE);
        }

        if (w->theme_object && w->fx_clip_box)
                evas_object_clip_set(w->theme_object, w->fx_clip_box);

        /*
         * Setup the appropriate clippings.
         */
        if (w->parent && EWL_CONTAINER(w->parent)->clip_box && w->fx_clip_box) {
                evas_object_clip_set(w->fx_clip_box,
                                EWL_CONTAINER(w->parent)->clip_box);
                evas_object_show(EWL_CONTAINER(w->parent)->clip_box);
        }

        /*
         * Set the layer of the clip box and theme object
         */
        ewl_widget_layer_stack_add(w);

        if (w->parent && REVEALED(w->parent))
                ewl_widget_layer_update(w);

        if (w->fx_clip_box) {
                Ewl_Color_Set *color;

                color = ewl_attach_color_get(w);
                if (color)
                        evas_object_color_set(w->fx_clip_box, color->r,
                                                color->g, color->b, color->a);
        }

        /*
         * Show the theme and clip box if widget is visible
         */
        if (VISIBLE(w)) {
                if (w->fx_clip_box) evas_object_show(w->fx_clip_box);
                if (w->theme_object) evas_object_show(w->theme_object);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Give up unnecessary objects when we're offscreen
 */
void
ewl_widget_cb_obscure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                           void *user_data __UNUSED__)
{
        Ewl_Embed *emb;
        Ewl_Container *pc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);

        emb = ewl_embed_widget_find(w);
        if (!emb) DRETURN(DLEVEL_STABLE);

        pc = EWL_CONTAINER(w->parent);

        /*
         * Decrement the dnd awareness counter on the embed.
         */
        if (ewl_widget_flags_has(w, EWL_FLAG_PROPERTY_DND_TARGET,
                                EWL_FLAGS_PROPERTY_MASK))
                ewl_embed_dnd_aware_remove(emb);

        /*
         * Remove all properties on the edje and hand it back to the embed for
         * caching.
         */
        if (w->theme_object) {
                ewl_embed_object_cache(emb, w->theme_object);
                w->theme_object = NULL;
        }

        /*
         * Repeat the process for the clip rect, but also hide the parent clip
         * box if there are no visible children. If we don't hide it, there
         * will be a white rectangle displayed.
         */
        if (w->fx_clip_box) {
                ewl_embed_object_cache(emb, w->fx_clip_box);
                w->fx_clip_box = NULL;
        }

        if (w->smart_object) {
                evas_object_data_del(w->smart_object, "EWL");
                ewl_embed_object_cache(emb, w->smart_object);
                w->smart_object = NULL;
        }

        /*
         * This has to happen last to be sure we've removed all clipped parts
         */
        if (pc && pc->clip_box) {
                if (!evas_object_clipees_get(pc->clip_box))
                        evas_object_hide(pc->clip_box);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Perform the basic operations necessary for realizing a widget
 */
void
ewl_widget_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        int l = 0, r = 0, t = 0, b = 0;
        int i_l = 0, i_r = 0, i_t = 0, i_b = 0;
        int p_l = 0, p_r = 0, p_t = 0, p_b = 0;
        char *i = NULL;
        const char *group = NULL;
        Evas_Coord width, height;
        Ewl_Embed *emb = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        emb = ewl_embed_widget_find(w);

        /*
         * Retrieve the path to the theme file that will be loaded
         * return if no file to be loaded.
         */
        i = ewl_theme_image_get(w, "file");
        if (i) {
                const char *t;

                t = w->theme_path;
                w->theme_path = ecore_string_instance(i);

                /* free this after, if it's the same string we dont' want to
                 * release all references on it */
                IF_RELEASE(t);

        } else {
                IF_RELEASE(w->theme_path);
        }

        /*
         * Defer group loading until the theme is loaded.
         */
        group = ewl_theme_data_str_get(w, "group");
        if (group) {
                const char *t;

                t = w->theme_group;
                w->theme_group = ecore_string_instance(group);

                /* free this after, if it's the same string we dont' want to
                 * release all references on it */
                IF_RELEASE(t);

        } else {
                IF_RELEASE(w->theme_group);
        }

        IF_FREE(i);

        /*
         * Reveal is done in this part of the callback to avoid duplicate code
         * for creating the evas objects. Must be done in the callback so that
         * prepended callbacks in the embed can create the evas, windows,
         * etc.
         */
        ewl_widget_reveal(w);

        /*
         * Set up the theme object on the widgets evas
         */
        if (w->theme_object) {
                ewl_widget_theme_insets_get(w, &i_l, &i_r, &i_t, &i_b);
                ewl_widget_theme_padding_get(w, &p_l, &p_r, &p_t, &p_b);

                ewl_object_insets_get(EWL_OBJECT(w), &l, &r, &t, &b);

                /*
                 * Use previously set insets and padding if available.
                 */
                if (l) i_l = l;
                if (r) i_r = r;
                if (t) i_t = t;
                if (b) i_b = b;

                ewl_object_padding_get(EWL_OBJECT(w), &l, &r, &t, &b);

                if (l) p_l = l;
                if (r) p_r = r;
                if (t) p_t = t;
                if (b) p_b = b;

                /*
                 * Assign the relevant insets and padding.
                 */
                ewl_object_insets_set(EWL_OBJECT(w), i_l, i_r, i_t, i_b);
                ewl_object_padding_set(EWL_OBJECT(w), p_l, p_r, p_t, p_b);

                i_l = CURRENT_X(w);
                i_t = CURRENT_Y(w);
                ewl_object_x_request(EWL_OBJECT(w), i_l);
                ewl_object_y_request(EWL_OBJECT(w), i_t);

                /*
                 * Propagate minimum sizes from the bit theme to the widget.
                 */
                edje_object_size_min_get(w->theme_object, &width, &height);
                i_l = (int)(width);
                i_t = (int)(height);

                if (i_l > 0 && MINIMUM_W(w) == EWL_OBJECT_MIN_SIZE
                                && i_l > EWL_OBJECT_MIN_SIZE
                                && i_l <= EWL_OBJECT_MAX_SIZE)
                        ewl_object_minimum_w_set(EWL_OBJECT(w), i_l);

                if (i_t > 0 && MINIMUM_H(w) == EWL_OBJECT_MIN_SIZE
                                && i_t > EWL_OBJECT_MIN_SIZE
                                && i_t <= EWL_OBJECT_MAX_SIZE)
                        ewl_object_minimum_h_set(EWL_OBJECT(w), i_t);

                /*
                 * Propagate maximum sizes from the bit theme to the widget.
                 */
                edje_object_size_max_get(w->theme_object, &width, &height);
                i_l = (int)(width);
                i_t = (int)(height);

                if (i_l > 0 && MAXIMUM_W(w) == EWL_OBJECT_MAX_SIZE
                                && i_l >= EWL_OBJECT_MIN_SIZE
                                && i_l < EWL_OBJECT_MAX_SIZE)
                        ewl_object_maximum_w_set(EWL_OBJECT(w), i_l);

                if (i_t > 0 && MAXIMUM_H(w) == EWL_OBJECT_MAX_SIZE
                                && i_t >= EWL_OBJECT_MIN_SIZE
                                && i_t < EWL_OBJECT_MAX_SIZE)
                        ewl_object_maximum_h_set(EWL_OBJECT(w), i_t);
        }

        DRETURN(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Perform the basic operations necessary for unrealizing a widget
 */
void
ewl_widget_cb_unrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (w->theme_object) {
                int i_l, i_r, i_t, i_b;
                int p_l, p_r, p_t, p_b;
                int l, r, t, b;

                printf("**********\n\n\tunrealize with theme_object\n"
                       "this seems to be dead code, please report when"
                       "you see these lines"         
                       "***********\n\n");
                ewl_widget_theme_insets_get(w, &l, &r, &t, &b);

                ewl_object_insets_get(EWL_OBJECT(w), &i_l, &i_r, &i_t, &i_b);
                ewl_object_padding_get(EWL_OBJECT(w), &p_l, &p_r, &p_t, &p_b);

                /*
                 * If the inset/padding values have been changed in code we
                 * want to leave the code set values. Otherwise, if the
                 * widget is using the theme set values, we reset to the
                 * default of 0 for padding/insets
                 */
                if (l == i_l) i_l = 0;
                if (r == i_r) i_r = 0;
                if (t == i_t) i_t = 0;
                if (b == i_b) i_b = 0;

                ewl_widget_theme_padding_get(w, &l, &r, &t, &b);
                if (l == p_l) p_l = 0;
                if (r == p_r) p_r = 0;
                if (t == p_t) p_t = 0;
                if (b == p_b) p_b = 0;

                /*
                 * Assign the relevant insets and padding.
                 */
                ewl_object_insets_set(EWL_OBJECT(w), i_l, i_r, i_t, i_b);
                ewl_object_padding_set(EWL_OBJECT(w), p_l, p_r, p_t, p_b);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Perform the basic operations necessary for configuring a widget
 */
void
ewl_widget_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Adjust the clip box to display the widget.
         */
        if (w->fx_clip_box) {
                evas_object_move(w->fx_clip_box,
                                CURRENT_X(w) - INSET_LEFT(w),
                                CURRENT_Y(w) - INSET_TOP(w));
                evas_object_resize(w->fx_clip_box,
                                CURRENT_W(w) + INSET_LEFT(w) + INSET_RIGHT(w),
                                CURRENT_H(w) + INSET_TOP(w) + INSET_BOTTOM(w));
        }

        /*
         * Move the base theme object to the correct size and position
         */
        if (w->theme_object) {
                evas_object_move(w->theme_object,
                                CURRENT_X(w) - INSET_LEFT(w),
                                CURRENT_Y(w) - INSET_TOP(w));
                evas_object_resize(w->theme_object,
                                CURRENT_W(w) + INSET_LEFT(w) + INSET_RIGHT(w),
                                CURRENT_H(w) + INSET_TOP(w) + INSET_BOTTOM(w));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Perform the basic operations necessary for reparenting a widget
 */
void
ewl_widget_cb_reparent(Ewl_Widget *w, void *ev_data __UNUSED__,
                        void *user_data __UNUSED__)
{
        Ewl_Container *pc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        pc = EWL_CONTAINER(w->parent);

        if (pc && REALIZED(pc) && VISIBLE(w) && !REALIZED(w))
                ewl_realize_request(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The enable callback
 */
void
ewl_widget_cb_enable(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_state_set(w, "enabled", EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The disable callback
 */
void
ewl_widget_cb_disable(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_state_set(w, "disabled", EWL_STATE_PERSISTENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The focus in callback
 */
void
ewl_widget_cb_focus_in(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_state_set(w, "focus,in", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The focus out callback
 */
void
ewl_widget_cb_focus_out(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_state_set(w, "focus,out", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse in callback
 */
void
ewl_widget_cb_mouse_in(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_state_set(w, "mouse,in", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse out callback
 */
void
ewl_widget_cb_mouse_out(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        ewl_widget_state_set(w, "mouse,out", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_widget_cb_mouse_down(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Event_Mouse_Down *e = ev_data;
        char state[14];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        snprintf(state, sizeof(state), "mouse,down,%i", e->button);
        ewl_widget_state_set(w, state, EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Up data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_widget_cb_mouse_up(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Event_Mouse_Up *e = ev_data;
        char state[14];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (DISABLED(w))
                DRETURN(DLEVEL_STABLE);

        if (ewl_widget_state_has(w, EWL_FLAG_STATE_DND)) {
                ewl_widget_state_remove(w, EWL_FLAG_STATE_DND);
                ewl_dnd_drag_drop(w);
        }

        snprintf(state, sizeof(state), "mouse,up,%i", e->button);
        ewl_widget_state_set(w, state, EWL_STATE_TRANSIENT);

        if (ewl_widget_state_has(w, EWL_FLAG_STATE_MOUSE_IN)) {
                int x, y;

                ewl_widget_state_set(w, "mouse,in", EWL_STATE_TRANSIENT);
                x = e->base.x - (CURRENT_X(w) - INSET_LEFT(w));
                y = e->base.y - (CURRENT_Y(w) - INSET_TOP(w));
                if ((x > 0) && (x < CURRENT_W(w) + INSET_HORIZONTAL(w)) &&
                                (y > 0) && (y < CURRENT_H(w) + INSET_VERTICAL(w)))
                {
                        Ewl_Event_Mouse_Down send;
                        send.base = e->base;
                        send.button = e->button;
                        send.clicks = 1;

                        ewl_callback_call_with_event_data(w,
                                        EWL_CALLBACK_CLICKED, &send);

                }
                else
                        ewl_embed_mouse_move_feed(ewl_embed_widget_find(w),
                                        e->base.x, e->base.y, e->base.modifiers);
        } else
                ewl_widget_state_set(w, "mouse,out", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_widget_cb_mouse_move(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Embed *embed;
        Ewl_Event_Mouse *ev = ev_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_widget_state_set(w, "mouse,move", EWL_STATE_TRANSIENT);
        if (ewl_widget_state_has(w, EWL_FLAG_STATE_PRESSED) &&
                        ewl_widget_flags_has(w, EWL_FLAG_PROPERTY_DND_SOURCE,
                                EWL_FLAGS_PROPERTY_MASK)) {

                embed = ewl_embed_widget_find(w);
                if (!ewl_widget_state_has(w, EWL_FLAG_STATE_DND)) {
                        ewl_widget_state_add(w, EWL_FLAG_STATE_DND);
                        embed->last.drag_widget = w;
                        ewl_dnd_internal_drag_start(w);
                }

                if (ev && (ev->x > CURRENT_X(embed) &&
                                ev->y > CURRENT_Y(embed) &&
                                ev->x < CURRENT_X(embed) + CURRENT_W(embed) &&
                                ev->y < CURRENT_Y(embed) + CURRENT_H(embed))) {
                        ewl_embed_dnd_position_feed(EWL_EMBED(embed),
                                                        ev->x, ev->y,
                                                        NULL, NULL,
                                                        NULL, NULL);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_theme_padding_get(Ewl_Widget *w, int *l, int *r, int *t, int *b)
{
        const char *key;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Read in the padding values from the edje file
         */
        key = edje_object_data_get(w->theme_object, "pad");
        if (key) {
                int val = atoi(key);

                if (l) *l = val;
                if (r) *r = val;
                if (t) *t = val;
                if (b) *b = val;
        }

        key = edje_object_data_get(w->theme_object, "pad/left");
        if (key && l) *l = atoi(key);

        key = edje_object_data_get(w->theme_object, "pad/right");
        if (key && r) *r = atoi(key);

        key = edje_object_data_get(w->theme_object, "pad/top");
        if (key && t) *t = atoi(key);

        key = edje_object_data_get(w->theme_object, "pad/bottom");
        if (key && b) *b = atoi(key);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_theme_insets_get(Ewl_Widget *w, int *l, int *r, int *t, int *b)
{
        const char *key;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * Read in the inset values from the edje file
         */
        key = edje_object_data_get(w->theme_object, "inset");
        if (key) {
                int val = atoi(key);

                if (l) *l = val;
                if (r) *r = val;
                if (t) *t = val;
                if (b) *b = val;
        }

        key = edje_object_data_get(w->theme_object, "inset/left");
        if (key && l) *l = atoi(key);

        key = edje_object_data_get(w->theme_object, "inset/right");
        if (key && r) *r = atoi(key);

        key = edje_object_data_get(w->theme_object, "inset/top");
        if (key && t) *t = atoi(key);

        key = edje_object_data_get(w->theme_object, "inset/bottom");
        if (key && b) *b = atoi(key);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_widget_name_table_shutdown(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        IF_FREE_HASH(ewl_widget_name_table);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


