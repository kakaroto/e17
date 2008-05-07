/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_cursor.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

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
        DCHECK_PARAM_PTR_RET(cursor, FALSE);

        if (!ewl_window_init(EWL_WINDOW(cursor)))
                DRETURN_INT(FALSE, DLEVEL_UNSTABLE);

        ewl_widget_appearance_set(w, EWL_CURSOR_TYPE);
        ewl_widget_inherit(w, EWL_CURSOR_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_ALL);
        ewl_object_size_request(EWL_OBJECT(w), 32, 32);
        ewl_embed_engine_name_set(EWL_EMBED(cursor), "evas_buffer");

        ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_cursor_cb_render, NULL);
        ewl_callback_append(w, EWL_CALLBACK_DESTROY,
                                ewl_cursor_cb_destroy, NULL);

        DRETURN_INT(TRUE, DLEVEL_UNSTABLE);
}

static void
ewl_cursor_cb_render(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
               int handle;
        int width, height;
        Ewl_Widget *parent;
        Ewl_Cursor *cursor = EWL_CURSOR(w);

        DENTER_FUNCTION(DLEVEL_UNSTABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CURSOR_TYPE);

        /*
         * Find the widget setting the current cursor.
         */
        parent = ewl_widget_focused_get();
        if (parent)
                parent = EWL_WIDGET(ewl_embed_widget_find(parent));

        if (!parent)
                DRETURN(DLEVEL_UNSTABLE);

        if (cursor->handle)
                ewl_engine_pointer_free(EWL_EMBED(parent), cursor->handle);

        width = ewl_object_current_w_get(EWL_OBJECT(cursor));
        height = ewl_object_current_h_get(EWL_OBJECT(cursor));

        handle = ewl_engine_pointer_data_new(EWL_EMBED(parent),
                        EWL_EMBED(cursor)->canvas_window, width, height);

        if (EWL_EMBED(parent)->cursor == cursor->handle)
                ewl_engine_pointer_set(EWL_EMBED(parent), handle);

        cursor->handle = handle;

        DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

static void
ewl_cursor_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Cursor *cursor = EWL_CURSOR(w);

        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_CURSOR_TYPE);

        /* FIXME: Also needs to be handled for the correct engine refs
        if (cursor->handle)
                ewl_engine_pointer_free(parent, cursor->handle);
                */
        cursor->handle = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

