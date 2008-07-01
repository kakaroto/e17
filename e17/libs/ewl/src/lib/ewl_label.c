/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static Ewl_Widget *ewl_label_view_cb_widget_fetch(void *data, unsigned int row,
                                                        unsigned int col);
static Ewl_Widget *ewl_label_view_cb_header_fetch(void *data, unsigned int col);

/**
 * @return Returns a new Ewl_Widget if successful, NULL on failure
 * @brief Creates a new Ewl_Label widget with the @a text text in it
 */
Ewl_Widget *
ewl_label_new(void)
{
        Ewl_Label *label;

        DENTER_FUNCTION(DLEVEL_STABLE);

        label = NEW(Ewl_Label, 1);
        if (!label) {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!ewl_label_init(label)) {
                ewl_widget_destroy(EWL_WIDGET(label));
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(EWL_WIDGET(label), DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Label to initialize
 * @return Returns TRUE on success, FALSE on falure
 * @brief Initializes the @a la widget
 */
int
ewl_label_init(Ewl_Label *la)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(la, FALSE);

        w = EWL_WIDGET(la);
        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_LABEL_TYPE);
        ewl_widget_inherit(w, EWL_LABEL_TYPE);
        ewl_object_alignment_set(EWL_OBJECT(la), EWL_FLAG_ALIGN_LEFT);
        ewl_object_fill_policy_set(EWL_OBJECT(la), EWL_FLAG_FILL_NONE);
        ewl_widget_focusable_set(w, FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Widget to set the text on
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Sets the given @a text into the widget @a la
 */
void
ewl_label_text_set(Ewl_Label *la, const char *text)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(la);
        DCHECK_TYPE(la, EWL_LABEL_TYPE);

        ewl_widget_appearance_text_set(EWL_WIDGET(la), (char *)text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param la: The Ewl_Label to get the text from
 * @return Returns no value.
 * @brief Gets the current text set into the label. 
 *
 * @note This pointer will only be valid until the next time 
 * ewl_label_text_set() is called.
 */
const char *
ewl_label_text_get(Ewl_Label *la)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(la, NULL);
        DCHECK_TYPE_RET(la, EWL_LABEL_TYPE, NULL);

        DRETURN_PTR(ewl_widget_appearance_text_get(EWL_WIDGET(la)), DLEVEL_STABLE);
}

/**
 * @return Returns a view that can be used to display Ewl_Label widgets
 * @brief Creates and returns a view to be used by Ewl_Label widgets
 */
Ewl_View *
ewl_label_view_get(void)
{
        Ewl_View *view;

        DENTER_FUNCTION(DLEVEL_STABLE);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, ewl_label_view_cb_widget_fetch);
        ewl_view_header_fetch_set(view, ewl_label_view_cb_header_fetch);

        DRETURN_PTR(view, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_label_view_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                        unsigned int col __UNUSED__)
{
        Ewl_Widget *label;

        DENTER_FUNCTION(DLEVEL_STABLE);

        label = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(label), data);

        DRETURN_PTR(label, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_label_view_cb_header_fetch(void *data, unsigned int col __UNUSED__)
{
        Ewl_Widget *label;

        DENTER_FUNCTION(DLEVEL_STABLE);

        label = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(label), data);

        DRETURN_PTR(label, DLEVEL_STABLE);
}
