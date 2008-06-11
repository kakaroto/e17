/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_menubar.h"
#include "ewl_menu.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns NULL on failure, or a pointer to a new menubar on success.
 * @brief Allocate and initialize a new menubar widget
 */
Ewl_Widget *
ewl_menubar_new(void)
{
        Ewl_Menubar *mb = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        mb = NEW(Ewl_Menubar, 1);
        if (!mb)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_menubar_init(mb)) {
                ewl_widget_destroy(EWL_WIDGET(mb));
                mb = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(mb), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure or a pointer to a new horizontal menubar
 * on success
 * @brief Creates a new Ewl_Menubar in a horizontal orientation
 */
Ewl_Widget *
ewl_hmenubar_new(void)
{
        Ewl_Widget *mb = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        mb = ewl_menubar_new();
        if (!mb)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_menubar_orientation_set(EWL_MENUBAR(mb),
                                EWL_ORIENTATION_HORIZONTAL);

        DRETURN_PTR(mb, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure or a pointer to a new vertical menubar on
 * success
 * @brief Creates a new Ewl_Menubar in a vertical orientation
 */
Ewl_Widget *
ewl_vmenubar_new(void)
{
        Ewl_Widget *mb = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        mb = ewl_menubar_new();
        if (!mb)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_menubar_orientation_set(EWL_MENUBAR(mb),
                                EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(mb, DLEVEL_STABLE);
}

/**
 * @param mb: the menubar to initialize
 * @return Returns TRUE on success and FALSE on failure
 * @brief Initialize a menubar to default values
 */
int
ewl_menubar_init(Ewl_Menubar *mb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mb, FALSE);

        if (!ewl_box_init(EWL_BOX(mb))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        ewl_widget_appearance_set(EWL_WIDGET(mb), EWL_MENUBAR_TYPE);
        ewl_widget_inherit(EWL_WIDGET(mb), EWL_MENUBAR_TYPE);

        mb->inner_box = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(mb),
                                        EWL_WIDGET(mb->inner_box));
        ewl_widget_internal_set(EWL_WIDGET(mb->inner_box), TRUE);
        ewl_widget_show(EWL_WIDGET(mb->inner_box));

        ewl_container_redirect_set(EWL_CONTAINER(mb),
                                        EWL_CONTAINER(mb->inner_box));

        ewl_container_add_notify_set(EWL_CONTAINER(mb->inner_box),
                                        ewl_menubar_cb_child_add);

        ewl_menubar_orientation_set(mb, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param mb: The menubar to work with
 * @param info: The menubar info to work with
 * @return Returns  no value
 * @brief Sets up the menubar @a mb with the given @a info
 */
void
ewl_menubar_from_info(Ewl_Menubar *mb, Ewl_Menubar_Info *info)
{
        int i = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mb);
        DCHECK_PARAM_PTR(info);
        DCHECK_TYPE(mb, EWL_MENUBAR_TYPE);

        for (i = 0; info[i].name != NULL; i++)
        {
                Ewl_Widget *menu;

                menu = ewl_menu_new();
                ewl_button_label_set(EWL_BUTTON(menu), info[i].name);
                ewl_menu_from_info(EWL_MENU(menu), info[i].menu);
                ewl_container_child_append(EWL_CONTAINER(mb), menu);
                ewl_object_fill_policy_set(EWL_OBJECT(menu),
                                EWL_FLAG_FILL_HSHRINKABLE | EWL_FLAG_FILL_VFILL);
                ewl_widget_show(menu);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mb: The menubar to set the orientation on
 * @param o: The orientation to set onto the menubar
 * @return Returns no value.
 * @brief Sets the orientation of the menubar
 */
void
ewl_menubar_orientation_set(Ewl_Menubar *mb, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mb);
        DCHECK_TYPE(mb, EWL_MENUBAR_TYPE);

        ewl_box_orientation_set(EWL_BOX(mb), o);
        if (o == EWL_ORIENTATION_HORIZONTAL) {
                ewl_object_fill_policy_set(EWL_OBJECT(mb),
                                EWL_FLAG_FILL_HFILL);
                ewl_box_orientation_set(EWL_BOX(mb->inner_box),
                                EWL_ORIENTATION_HORIZONTAL);
                ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
                                EWL_FLAG_FILL_HFILL);

        } else if (o == EWL_ORIENTATION_VERTICAL) {
                ewl_object_fill_policy_set(EWL_OBJECT(mb),
                                EWL_FLAG_FILL_VFILL);
                ewl_box_orientation_set(EWL_BOX(mb->inner_box),
                                EWL_ORIENTATION_VERTICAL);
                ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
                                EWL_FLAG_FILL_VFILL);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mb: The menubar to get the orientation from
 * @return Returns the orientation of the menubar
 * @brief Gets the orientation of the menubar
 */
Ewl_Orientation
ewl_menubar_orientation_get(Ewl_Menubar *mb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mb, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(mb, EWL_MENUBAR_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(ewl_box_orientation_get(EWL_BOX(mb)), DLEVEL_STABLE);
}


/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_menubar_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        if (EWL_MENU_IS(w))
                EWL_MENU(w)->menubar_parent = EWL_WIDGET(c);

        DLEAVE_FUNCTION(DLEVEL_STABLE);

}

