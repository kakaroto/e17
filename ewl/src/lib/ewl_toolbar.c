/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_toolbar.h"
#include "ewl_icon.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns pointer to new toolbar widget on success, NULL on failure.
 * @brief Allocate a new toolbar widget with default (horizontal) orientation
 */
Ewl_Widget *
ewl_toolbar_new(void)
{
        Ewl_Toolbar  *t;

        DENTER_FUNCTION(DLEVEL_STABLE);

        t = NEW(Ewl_Toolbar, 1);
        if (!t)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_toolbar_init(t)) {
                ewl_widget_destroy(EWL_WIDGET(t));
                t = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * @return Returns pointer to new toolbar widget on success, NULL on failure.
 * @brief Allocate a new toolbar widget with horizontal orientation
 */
Ewl_Widget *
ewl_htoolbar_new(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(ewl_toolbar_new(), DLEVEL_STABLE);
}

/**
 * @return Returns pointer to new toolbar widget on success, NULL on failure.
 * @brief Allocate a new toolbar widget with vertical orientation
 */
Ewl_Widget *
ewl_vtoolbar_new(void)
{
        Ewl_Widget *t;

        DENTER_FUNCTION(DLEVEL_STABLE);

        t = ewl_toolbar_new();
        if (t)
                ewl_toolbar_orientation_set(EWL_TOOLBAR(t),
                                EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(t, DLEVEL_STABLE);
}

/**
 * @param t: the toolbar to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the toolbar and inherited fields
 *
 * Clears the contents of the toolbar and stores the
 * default values.
 */
int
ewl_toolbar_init(Ewl_Toolbar *t)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, FALSE);

        w = EWL_WIDGET(t);
        if (!ewl_menubar_init(EWL_MENUBAR(t)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_TOOLBAR_TYPE);
        ewl_widget_appearance_set(w, "htoolbar");

        ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HFILL |
                                                  EWL_FLAG_FILL_SHRINKABLE);
        ewl_object_alignment_set(EWL_OBJECT(t), EWL_FLAG_ALIGN_LEFT |
                                                        EWL_FLAG_ALIGN_TOP);

        ewl_container_add_notify_set(EWL_CONTAINER(EWL_MENUBAR(t)->inner_box),
                                        ewl_toolbar_cb_child_add);
        t->hidden = EWL_ICON_PART_NONE;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param t: The toolbar to set the orientation on
 * @param o: The orientation to set on the toolbar
 * @return Returns no value
 * @brief Set the orientation of the toolbar
 */
void
ewl_toolbar_orientation_set(Ewl_Toolbar *t, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TOOLBAR_TYPE);

        ewl_menubar_orientation_set(EWL_MENUBAR(t), o);
        if (o == EWL_ORIENTATION_HORIZONTAL)
                ewl_widget_appearance_set(EWL_WIDGET(t), "htoolbar");
        else
                ewl_widget_appearance_set(EWL_WIDGET(t), "vtoolbar");

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The toolbar to get the orientation from
 * @return Returns the orientation currently set on the toolbar
 * @brief This retrieves the current orientation of the toolbar
 */
Ewl_Orientation
ewl_toolbar_orientation_get(Ewl_Toolbar *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(t, EWL_TOOLBAR_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(ewl_menubar_orientation_get(EWL_MENUBAR(t)), DLEVEL_STABLE);
}

/**
 * @param t: The toolbar to set the orientation on
 * @param part: The Ewl_Icon_Part to hide
 * @return Returns no value
 * @brief Set the orientation of the toolbar
 */
void
ewl_toolbar_icon_part_hide(Ewl_Toolbar *t, Ewl_Icon_Part part)
{
        Ewl_Widget *child;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TOOLBAR_TYPE);

        if (t->hidden == part)
                DRETURN(DLEVEL_STABLE);

        t->hidden = part;

        ewl_container_child_iterate_begin(EWL_CONTAINER(t));
        while ((child = ewl_container_child_next(EWL_CONTAINER(t)))) {
                if (EWL_ICON_IS(child))
                        ewl_icon_part_hide(EWL_ICON(child), part);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_toolbar_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        Ewl_Toolbar *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_menubar_cb_child_add(c, w);
        t = EWL_TOOLBAR(EWL_WIDGET(c)->parent);

        if (EWL_ICON_IS(w))
                ewl_icon_part_hide(EWL_ICON(w), t->hidden);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

