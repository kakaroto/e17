/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/*
 * Note: The callback list is either a single callback or a
 * pointer to an array of callbacks. This can be checked with the
 * EWL_CALLBACK_TYPE_DIRECT flag. If the list is set to direct then the list
 * itself is the callback. If it isn't direct then the list points to an
 * array of callbacks. (You can't use the length to determine this as if
 * the list has several items and you remove them down to the first item in
 * the array the list won't be direct, but will have only one item.)
 */

static unsigned int ewl_callback_hash(const void *key);
static int ewl_callback_compare(const void *key1, const void *key2);
static void ewl_callback_unregister(Ewl_Callback * cb);
static Ewl_Callback *ewl_callback_get(Ewl_Widget *w, unsigned int type,
                                                unsigned int idx);

static void ewl_callback_rm(Ewl_Widget *w, unsigned int t,
                                                unsigned int pos);
static int ewl_callback_insert(Ewl_Widget *w, unsigned int t,
                                Ewl_Callback *cb, unsigned int pos);

static int callback_type_count;
static int callback_id = 0;
static Ecore_Hash *cb_registration = NULL;

/**
 * @return Returns no value.
 * @brief Setup internal registration variables for callbacks
 *
 * Sets up some important variables for tracking callbacks that allow shared
 * callbacks.
 *
 * W/o shared callbacks ewl_test with all windows open has a top line of:
 * 21279 ningerso  19   0 22972  22M  9412 R     6.0  8.0   0:40 ewl_test
 * With shared callbacks ewl_test with all windows open has a top line of:
 * 15901 ningerso  10   0 20120  19M  9148 S     0.0  7.0   0:34 ewl_test
 *
 * So using shared callbacks saves us over 2 MB of memory in this case.
 *
 *
 * Ecore_list as the callback storage with all tests open has a top line of:
 *  9121 dsinclai  15   0 71156  17m 4276 S  0.0  1.8   0:11.06 ewl_test
 * Using an array as the callback storage with all tests open has a top line of:
 * 21727 dsinclai  15   0 68360  15m 4304 S  0.0  1.5   0:09.73 ewl_test
 *
 * So using an array for the callbacks saves us about 2MB of memory in this
 * case.
 */
int
ewl_callbacks_init(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        cb_registration = ecore_hash_new(ewl_callback_hash,
                                       ewl_callback_compare);
        callback_type_count = EWL_CALLBACK_MAX + 1;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Destroy internal registration variables for callbacks
 *
 * Destroys some important variables for tracking callbacks that allow shared
 * callbacks.
 */
void
ewl_callbacks_shutdown(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        IF_FREE_HASH(cb_registration);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * ewl_callback_unregister - unreference a callback and free if appropriate
 * @cb: the callback to unregister
 *
 * Returns no value. Checks to see if @cb has any remaining references, if not
 * it is removed from the registration system and freed.
 */
static void
ewl_callback_unregister(Ewl_Callback *cb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cb);

        cb->references--;
        if (cb->references < 1) {
                ecore_hash_remove(cb_registration, cb);
                FREE(cb);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_callback_rm(Ewl_Widget *w, unsigned int t, unsigned int pos)
{
        unsigned int place;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        /* don't type check here as this will get called after most of a
         * widget is already destroyed */

        if (t > EWL_CALLBACK_MAX)
                place = EWL_CALLBACK_MAX;
        else
                place = t;

        /* deal with the direct case first */
        if (w->callbacks[place].mask & EWL_CALLBACK_TYPE_DIRECT)
        {
                ewl_callback_unregister((Ewl_Callback *)w->callbacks[place].list);

                w->callbacks[place].len = 0;
                w->callbacks[place].list = NULL;
                EWL_CALLBACK_SET_NODIRECT(w, place);

                DRETURN(DLEVEL_STABLE);
        }
        ewl_callback_unregister(w->callbacks[place].list[pos]);

        /* if this will empty the list (we've already handled direct) */
        if ((EWL_CALLBACK_LEN(w, place) - 1) == 0)
        {
                w->callbacks[place].len = 0;
                w->callbacks[place].list[0] = NULL;
                FREE(w->callbacks[place].list);

                DRETURN(DLEVEL_STABLE);
        }

        /* not the last position */
        if ((int)pos != (EWL_CALLBACK_LEN(w, place) - 1))
        {
                memmove(w->callbacks[place].list + pos,
                        w->callbacks[place].list + (pos + 1),
                        (w->callbacks[place].len - pos - 1) * sizeof(void *));
        }

        w->callbacks[place].len  -= 1;
        w->callbacks[place].list[EWL_CALLBACK_LEN(w, place)] = NULL;
        REALLOC(w->callbacks[place].list, void *, w->callbacks[place].len);

        if (pos < EWL_CALLBACK_POS(w, place))
                EWL_CALLBACK_POS(w, place)--;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_callback_insert(Ewl_Widget *w, unsigned int t,
                                Ewl_Callback *cb, unsigned int pos)
{
        unsigned int place;
        Ewl_Callback *old = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_PARAM_PTR_RET(cb, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        if (EWL_CALLBACK_LEN(w, t) == 255) {
                DERROR("Maximum number of callbacks of one type "
                        "exceeded on a widget\n");
                DRETURN_INT(0, DLEVEL_STABLE);
        }

        if (t > EWL_CALLBACK_MAX)
                place = EWL_CALLBACK_MAX;
        else
                place = t;

        /* set direct if possible */
        if (!EWL_CALLBACK_LEN(w, t))
        {
                w->callbacks[place].list = (void *)cb;
                w->callbacks[place].len = 1;
                EWL_CALLBACK_SET_DIRECT(w, t);

                DRETURN_INT(cb->id, DLEVEL_STABLE);
        }
        w->callbacks[place].len ++;

        /* if we have a type direct then we need to save off the direct
         * pointer and set the list to NULL so it'll be allocd' correctly */
        if (w->callbacks[place].mask & EWL_CALLBACK_TYPE_DIRECT)
        {
                old = (Ewl_Callback *)w->callbacks[place].list;
                w->callbacks[place].list = NULL;
                EWL_CALLBACK_SET_NODIRECT(w, t);
        }

        REALLOC(w->callbacks[place].list, void *, w->callbacks[place].len);

        /* if old is set this was a direct so we can just set 0, 1 and be
         * done with it */
        if (old)
        {
                w->callbacks[place].list[0] = (!pos ? cb : old);
                w->callbacks[place].list[1] = ( pos ? cb : old);
        }
        else
        {
                /* only have to move if we aren't at the end (of the
                 * original lenth already */
                if ((int)pos != (w->callbacks[place].len - 1))
                {
                        memmove(w->callbacks[place].list + (pos + 1),
                                w->callbacks[place].list + pos,
                                (w->callbacks[place].len - pos - 1)
                                        * sizeof(void *));
                }
                w->callbacks[place].list[pos] = cb;
        }

        if (pos <= EWL_CALLBACK_POS(w, t))
                EWL_CALLBACK_POS(w, t)++;

        DRETURN_INT(cb->id, DLEVEL_STABLE);
}

static Ewl_Callback *
ewl_callback_get(Ewl_Widget *w, unsigned int t, unsigned int i)
{
        Ewl_Callback *cb = NULL;
        Ewl_Callback_Custom *ccb = NULL;
        Ewl_Callback_Chain *chain = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        chain = &(w->callbacks[EWL_CALLBACK_INDEX(t)]);

        if (chain->mask & EWL_CALLBACK_TYPE_DIRECT)
                cb = EWL_CALLBACK(chain->list);

        else if (chain->list)
                cb = chain->list[i];

        if (t >= EWL_CALLBACK_MAX)
                ccb = EWL_CALLBACK_CUSTOM(cb);

        /* make sure the event id's match (if this is a custom callback */
        if (ccb && (ccb->event_id != t))
                cb = NULL;

        DRETURN_PTR(cb, DLEVEL_STABLE);
}

/**
 * @return Returns a new callback identifier
 * @brief Creates and returns a new callback identifier
 */
unsigned int
ewl_callback_type_add(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_INT(++callback_type_count, DLEVEL_STABLE);
}

static int
ewl_callback_position_insert(Ewl_Widget *w, unsigned int type,
                                Ewl_Callback_Function func,
                                unsigned int pos, void *user_data)
{
        int ret;
        Ewl_Callback *cb, *found;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_PARAM_PTR_RET(func, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        if (type < EWL_CALLBACK_MAX)
                cb = alloca(sizeof(Ewl_Callback));
        else
                cb = alloca(sizeof(Ewl_Callback_Custom));

        cb->func = func;
        cb->user_data = user_data;

        found = ecore_hash_get(cb_registration, cb);
        if (!found) {
                if (type < EWL_CALLBACK_MAX) {
                        found = NEW(Ewl_Callback, 1);
                }
                else {
                        found = NEW(Ewl_Callback_Custom, 1);
                        EWL_CALLBACK_CUSTOM(found)->event_id = type;
                }
                found->func = func;
                found->user_data = user_data;
                found->id = ++callback_id;
                ecore_hash_set(cb_registration, found, found);
        }

        found->references++;

        ret = ewl_callback_insert(w, type, found, pos);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to attach the callback
 * @param t: the type of the callback that is being attached
 * @param f: the function to attach as a callback
 * @param user_data: the data to be passed to the callback function
 * @return Returns 0 on failure, the id of the new callback on success.
 * @brief Append a callback of the specified type
 *
 * Allocates a new callback for the specified widget that calls @a f with @a
 * user_data as the data parameter when event @a ta  occurs. This event is
 * placed at the end of the callback chain.
 */
int
ewl_callback_append(Ewl_Widget *w, unsigned int t,
                    Ewl_Callback_Function f, void *user_data)
{
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_PARAM_PTR_RET(f, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        ret = ewl_callback_position_insert(w, t, f,
                                EWL_CALLBACK_LEN(w, t), user_data);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to attach the callback
 * @param t: the type of the callback that is being attached
 * @param f: the function to attach as a callback
 * @param user_data: the data to be passed to the callback function
 * @return Returns 0 on failure, the id of the new callback on success.
 * @brief prepend a callback of the specified type
 *
 * Same functionality as ewl_callback_append, but the callback is placed at the
 * beginning of the callback chain.
 */
int
ewl_callback_prepend(Ewl_Widget *w, unsigned int t,
                     Ewl_Callback_Function f, void *user_data)
{
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_PARAM_PTR_RET(f, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        ret = ewl_callback_position_insert(w, t, f, 0, user_data);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to insert the callback
 * @param t: the type of the callback that is being attached
 * @param f: the function to attach as a callback
 * @param user_data: the data to be passed to the callback function
 * @param after: the function of the callback to append after
 * @param after_data: the user data of the callback to append after
 * @return Returns 0 on failure, the id of the new callback on success.
 * @brief Add a callback after a previous callback in list
 *
 * Same functionality as ewl_callback_append, but the callback is placed after
 * the specified callback on the callback chain.
 */
int
ewl_callback_insert_after(Ewl_Widget *w, unsigned int t,
                          Ewl_Callback_Function f, void *user_data,
                          Ewl_Callback_Function after, void *after_data)
{
        Ewl_Callback *search;
        int ret;
        unsigned int pos = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, 0);
        DCHECK_PARAM_PTR_RET(f, 0);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, 0);

        /*
         * position past the callback we want to insert after.
         */
        for (pos = 0; pos < EWL_CALLBACK_LEN(w, t); pos++)
        {
                search = ewl_callback_get(w, t, pos);
                if (search && (search->func == after) &&
                                (search->user_data == after_data))
                {
                        pos ++;
                        break;
                }
        }

        ret = ewl_callback_position_insert(w, t, f, pos, user_data);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to execute the callbacks
 * @param t: the type of the callbacks to be executed
 * @return Returns no value.
 * @brief Execute callbacks of specified types for the widget
 *
 * Executes the callback chain for the specified widget @a w, with event @a t.
 */
void
ewl_callback_call(Ewl_Widget *w, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_callback_call_with_event_data(w, t, NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to execute the callbacks
 * @param t: the type of the callbacks to be executed
 * @param ev_data: the event data to pass to the callbacks
 * @return Returns no value.
 * @brief Execute callbacks with event data
 *
 * Similar to ewl_callback_call, but the event data is substituted by @a
 * ev_data.
 */
void
ewl_callback_call_with_event_data(Ewl_Widget *w, unsigned int t,
                                  void *ev_data)
{
        Ewl_Callback *cb;
        Ewl_Widget *parent, *top = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * First search up the tree to find the topmost intercepting widget.
         */
        parent = w->parent;
        while (parent) {
                if (EWL_CALLBACK_FLAGS(parent, t) & EWL_CALLBACK_NOTIFY_INTERCEPT)
                        top = parent;
                parent = parent->parent;
        }

        if (top)
                w = top;

        /*
         * Now search up the tree to find the first notified widget. This may
         * result in some recursion.
         */
        top = NULL;
        parent = w->parent;
        while (parent) {
                if (EWL_CALLBACK_FLAGS(parent, t) & EWL_CALLBACK_NOTIFY_NOTIFY)
                {
                        top = parent;
                        break;
                }
                parent = parent->parent;
        }

        if (top)
                ewl_callback_call_with_event_data(top, t, ev_data);

        /*
         * Make sure the widget has callbacks of the given type
         */
        if (!EWL_CALLBACK_LEN(w, t))
                DRETURN(DLEVEL_STABLE);

        /*
         * Loop through and execute each of the callbacks of a certain type for
         * the specified widget.
         */
        EWL_CALLBACK_POS(w, t) = 0;
        while (EWL_CALLBACK_POS(w, t) < EWL_CALLBACK_LEN(w, t))
        {
                Ewl_Callback *newcb = NULL;

                cb = ewl_callback_get(w, t, EWL_CALLBACK_POS(w, t));

                /* keep going if there is no callback at this spot. This can
                 * happen with hte custom array */
                if (!cb)
                {
                        EWL_CALLBACK_POS(w, t)++;
                        continue;
                }

                if (cb->func)
                        cb->func(w, ev_data, cb->user_data);

                newcb = ewl_callback_get(w, t, EWL_CALLBACK_POS(w, t));
                if (cb == newcb)
                        EWL_CALLBACK_POS(w, t)++;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to delete the callbacks
 * @param t: the type of the callbacks to be deleted
 * @return Returns no value.
 * @brief Delete all callbacks of the specified type
 *
 * Delete all callbacks of type @a t from widget @a w.
 */
void
ewl_callback_del_type(Ewl_Widget *w, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        /* don't type check this as most of the widget will probably be
         * destroyed by the time we get here */

        if (!EWL_CALLBACK_LEN(w, t))
                DRETURN(DLEVEL_STABLE);

        if (t > EWL_CALLBACK_MAX) {
                unsigned int i = 0;

                /*
                 * Handle custom types by verifying that the event id matches
                 * the specified callback type.
                 */
                while (i < EWL_CALLBACK_LEN(w, t)) {
                        Ewl_Callback_Custom *cb;

                        cb = (Ewl_Callback_Custom *)ewl_callback_get(w, t, i);
                        if (cb && (cb->event_id == t))
                                ewl_callback_rm(w, t, i);
                        else
                                i++;
                }
        }
        else {
                while (EWL_CALLBACK_LEN(w, t))
                        ewl_callback_rm(w, t, EWL_CALLBACK_LEN(w, t) - 1);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to delete the id
 * @param t: the type of event the callback is attached to
 * @param cb_id: the id of the callback to delete
 * @return Returns no value.
 * @brief Delete the specified callback id from the widget
 *
 * Delete the specified callback id from the widget @a w.
 */
void
ewl_callback_del_cb_id(Ewl_Widget *w, unsigned int t, int cb_id)
{
        Ewl_Callback *cb;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!EWL_CALLBACK_LEN(w, t) || cb_id > callback_id)
                DRETURN(DLEVEL_STABLE);

        for (i = 0; i < EWL_CALLBACK_LEN(w, t); i++)
        {
                cb = ewl_callback_get(w, t, i);
                if (cb && (cb->id == cb_id)) {
                        ewl_callback_rm(w, t, i);
                        break;
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @internal
 * @param w: the widget to remove the callbacks
 * @return Returns no value.
 * @brief Remove all callbacks from the specified widget
 *
 * Removes and frees all callbacks associated with widget @a w.
 */
void
ewl_callback_clear(Ewl_Widget *w)
{
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * First clear out all standard callback types.
         */
        for (i = 0; i < EWL_CALLBACK_MAX; i++)
                ewl_callback_del_type(w, i);

        /*
         * Finally clear out all custom callbacks.
         */
        while (EWL_CALLBACK_LEN(w, i))
                ewl_callback_rm(w, i, EWL_CALLBACK_LEN(w, i) - 1);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to delete the callback
 * @param t: the type of event associated with the callback
 * @param f: the function called by the callback
 * @brief Delete the specified callback function from the widget
 *
 * @return Returns no value.
 * Delete and frees the callback that calls function @a f when event @a t occurs
 * to widget @a w.
 */
void
ewl_callback_del(Ewl_Widget *w, unsigned int t, Ewl_Callback_Function f)
{
        Ewl_Callback *cb;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!EWL_CALLBACK_LEN(w, t))
                DRETURN(DLEVEL_STABLE);

        for (i = 0; i < EWL_CALLBACK_LEN(w, t); i++)
        {
                cb = ewl_callback_get(w, t, i);
                if (cb && (cb->func == f)) {
                        ewl_callback_rm(w, t, i);
                        break;
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to delete the callback
 * @param t: the type of event associated with the callback
 * @param f: the function called by the callback
 * @param d: the data passed to the callback
 * @brief Delete the specified callback function from the widget
 *
 * @return Returns no value.
 * Delete and frees the callback that calls function @a f when event @a t occurs
 * to widget @a w.
 */
void
ewl_callback_del_with_data(Ewl_Widget *w, unsigned int t,
                           Ewl_Callback_Function f, void *d)
{
        Ewl_Callback *cb;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (!EWL_CALLBACK_LEN(w, t))
                DRETURN(DLEVEL_STABLE);

        for (i = 0; i < EWL_CALLBACK_LEN(w, t); i++)
        {
                cb = ewl_callback_get(w, t, i);
                if (cb && (cb->func == f) && (cb->user_data == d)) {
                        ewl_callback_rm(w, t, i);
                        break;
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Hashes the value of a callback based on it's type, function, and user data.
 */
static unsigned int
ewl_callback_hash(const void *key)
{
        Ewl_Callback *cb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(key, 0);

        cb = EWL_CALLBACK(key);

        DRETURN_INT(ecore_direct_hash(cb->func) 
                        ^ ecore_direct_hash(cb->user_data), DLEVEL_STABLE);
}

/*
 * Simple comparison of callbacks, always returns -1 unless there is an exact
 * match, in which case it returns 0.
 */
static int
ewl_callback_compare(const void *key1, const void *key2)
{
        Ewl_Callback *cb1;
        Ewl_Callback *cb2;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(key1, -1);
        DCHECK_PARAM_PTR_RET(key2, -1);

        cb1 = EWL_CALLBACK(key1);
        cb2 = EWL_CALLBACK(key2);

        if ((cb1->func == cb2->func) && (cb1->user_data == cb2->user_data))
                DRETURN_INT(0, DLEVEL_STABLE);

        DRETURN_INT(-1, DLEVEL_STABLE);
}

