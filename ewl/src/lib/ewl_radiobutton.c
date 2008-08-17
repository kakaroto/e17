/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_radiobutton.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a pointer to new radio button on success, NULL on failure.
 * @brief Allocate and initialize a new radio button
 */
Ewl_Widget *
ewl_radiobutton_new(void)
{
        Ewl_Radiobutton *b;

        DENTER_FUNCTION(DLEVEL_STABLE);

        b = NEW(Ewl_Radiobutton, 1);
        if (!b)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_radiobutton_init(b)) {
                ewl_widget_destroy(EWL_WIDGET(b));
                b = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param rb: the radio button to initialize
 * @return Returns no value.
 * @brief Initialize the radio button fields and callbacks
 *
 * Sets internal fields of the radio button to default
 * values and sets the label to the specified @a label.
 */
int
ewl_radiobutton_init(Ewl_Radiobutton *rb)
{
        Ewl_Checkbutton *cb;
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(rb, FALSE);

        cb = EWL_CHECKBUTTON(rb);
        w = EWL_WIDGET(rb);

        if (!ewl_checkbutton_init(cb))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_RADIOBUTTON_TYPE);
        ewl_widget_inherit(w, EWL_RADIOBUTTON_TYPE);
        ewl_widget_appearance_set(cb->check, "radio");
        ewl_callback_append(w, EWL_CALLBACK_CLICKED, ewl_radiobutton_cb_clicked,
                            NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_radiobutton_cb_destroy,
                            NULL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/*
 * @param rb: the radio button to set the value
 * @param value: A user defined value. Ewl does't use this value at all.
 * @return Returns no value.
 * @brief Set an user defined value.
 */
void
ewl_radiobutton_value_set(Ewl_Radiobutton *rb, void *value)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(rb);
        DCHECK_TYPE(rb, EWL_RADIOBUTTON_TYPE);

        rb->value = value;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * @param rb: the radio button to get the value
 * @return Returns the value of the radio button.
 * @brief Get the user defined value.
 */
void *
ewl_radiobutton_value_get(Ewl_Radiobutton *rb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(rb, 0);
        DCHECK_TYPE_RET(rb, EWL_RADIOBUTTON_TYPE, 0);

        DRETURN_PTR(rb->value, DLEVEL_STABLE);
}

/**
 * @param rb: the radio button to be added to a chain of radio buttons
 * @param crb: a radio button already in the chain of radio buttons
 * @return Returns no value.
 * @brief Attach the button to a chain of radio buttons
 *
 * Associates @a w with the same chain as @a c, in order to
 * ensure that only one radio button of that group is checked at any time.
 */
void
ewl_radiobutton_chain_set(Ewl_Radiobutton *rb, Ewl_Radiobutton *crb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(rb);
        DCHECK_PARAM_PTR(crb);
        DCHECK_TYPE(rb, EWL_RADIOBUTTON_TYPE);
        DCHECK_TYPE(crb, EWL_RADIOBUTTON_TYPE);

        /*
         * If a chain doesnt exist, create one
         */
        if (!crb->chain) {
                crb->chain = ecore_list_new();

                ecore_list_append(crb->chain, rb);
                ecore_list_append(crb->chain, crb);
        } else {

                if (!ecore_list_goto(crb->chain, rb))
                        ecore_list_append(crb->chain, rb);
        }

        rb->chain = crb->chain;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * @param rb: the radio button to get the checked chain link
 * @return Returns the current checked chain link
 * @brief XXX write me
 */
Ewl_Radiobutton *
ewl_radiobutton_chain_selected_get(Ewl_Radiobutton *rb)
{
        Ewl_Checkbutton *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(rb, NULL);
        DCHECK_TYPE_RET(rb, EWL_RADIOBUTTON_TYPE, NULL);

        /* if there is no chain or the chain is empty we have to
         * treat it special */
        if (!rb->chain || ecore_list_empty_is(rb->chain)) {
                if (ewl_checkbutton_is_checked(EWL_CHECKBUTTON(rb))) {
                        DRETURN_PTR(rb, DLEVEL_STABLE);
                }
                else {
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
                }
        }

        ecore_list_first_goto(rb->chain);
        while ((c = ecore_list_next(rb->chain))) {
                if (ewl_checkbutton_is_checked(c))
                        DRETURN_PTR(c, DLEVEL_STABLE);
        }

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The clicked callback
 */
void
ewl_radiobutton_cb_clicked(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Checkbutton *cb;
        Ewl_Radiobutton *rb;
        int oc;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_RADIOBUTTON_TYPE);

        cb = EWL_CHECKBUTTON(w);
        rb = EWL_RADIOBUTTON(w);
        oc = ewl_checkbutton_is_checked(cb);

        if (rb->chain && !ecore_list_empty_is(rb->chain)) {
                Ewl_Checkbutton *c;

                ecore_list_first_goto(rb->chain);
                while ((c = ecore_list_next(rb->chain))) {
                        ewl_checkbutton_checked_set(c, 0);
                }
        }
        ewl_checkbutton_checked_set(cb, 1);

        if (oc != ewl_checkbutton_is_checked(cb))
                ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

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
void
ewl_radiobutton_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Radiobutton *rb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_RADIOBUTTON_TYPE);

        rb = EWL_RADIOBUTTON(w);

        if (!rb->chain)
                DRETURN(DLEVEL_STABLE);

        ecore_list_goto(rb->chain, w);
        ecore_list_remove(rb->chain);

        if (ecore_list_empty_is(rb->chain))
                IF_FREE_LIST(rb->chain);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

