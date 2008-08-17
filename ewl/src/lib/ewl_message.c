/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_dialog.h"
#include "ewl_icondialog.h"
#include "ewl_message.h"
#include "ewl_text.h"
#include "ewl_button.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a pointer to a new message on success, NULL on failure.
 * @brief Create a new internal message
 */
Ewl_Widget *
ewl_message_new(void)
{
        Ewl_Message *d;

        DENTER_FUNCTION(DLEVEL_STABLE);

        d = NEW(Ewl_Message, 1);
        if (!d) {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!ewl_message_init(d)) {
                ewl_widget_destroy(EWL_WIDGET(d));
                d = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(d), DLEVEL_STABLE);
}

/**
 * @param dialog: the message to initialize.
 * @return Return TRUE on success, FALSE otherwise.
 * @brief Initialize an internal message to starting values
 */
int
ewl_message_init(Ewl_Message *m)
{
        Ewl_Widget *w, *o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(m, FALSE);

        w = EWL_WIDGET(m);

        if (!ewl_icondialog_init(EWL_ICONDIALOG(m))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_widget_appearance_set(w, EWL_MESSAGE_TYPE"/"EWL_ICONDIALOG_TYPE);
        ewl_widget_inherit(w, EWL_MESSAGE_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
        ewl_window_dialog_set(EWL_WINDOW(w), TRUE);
        ewl_callback_append(w, EWL_CALLBACK_DELETE_WINDOW, ewl_message_cb_quit,
                                w);

        ewl_dialog_active_area_set(EWL_DIALOG(m), EWL_POSITION_TOP);

        m->brief = ewl_text_new();
        ewl_container_child_append(EWL_CONTAINER(m), m->brief);
        ewl_widget_show(m->brief);

        m->detailed = ewl_text_new();
        ewl_container_child_append(EWL_CONTAINER(m), m->detailed);
        ewl_widget_show(m->detailed);

        ewl_dialog_active_area_set(EWL_DIALOG(m), EWL_POSITION_BOTTOM);

        o = ewl_button_new();
        ewl_stock_type_set(EWL_STOCK(o), EWL_STOCK_OK);
        ewl_container_child_append(EWL_CONTAINER(m), o);
        ewl_callback_append(o, EWL_CALLBACK_CLICKED, ewl_message_cb_quit, w);
        ewl_widget_show(o);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param m: message to set the brief text
 * @param brief: a brief form of the message
 * @return Returns no value.
 * @brief Set the brief text to the message.
 */
void
ewl_message_brief_text_set(Ewl_Message *m, const char *brief)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MESSAGE_TYPE);

        ewl_text_clear(EWL_TEXT(m->brief));
        ewl_text_font_set(EWL_TEXT(m->brief), "ewl/default/bold");
        ewl_text_text_append(EWL_TEXT(m->brief), brief);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param m: message to set the brief text
 * @param detailed: a detaileded form of the message
 * @return Returns no value.
 * @brief Set the detaileded text to the message.
 */
void
ewl_message_detailed_text_set(Ewl_Message *m, const char *detailed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(m);
        DCHECK_TYPE(m, EWL_MESSAGE_TYPE);

        ewl_text_clear(EWL_TEXT(m->detailed));
        ewl_text_wrap_set(EWL_TEXT(m->detailed), EWL_TEXT_WRAP_WORD);
        ewl_text_text_append(EWL_TEXT(m->detailed), detailed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_message_cb_quit(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *data)
{
        Ewl_Widget *m;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_MESSAGE_TYPE);

        m = EWL_WIDGET(data);

        ewl_widget_destroy(m);
}
