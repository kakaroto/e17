/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_dialog.h"
#include "ewl_icondialog.h"
#include "ewl_icon_theme.h"
#include "ewl_box.h"
#include "ewl_image.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a pointer to a new icondialog on success, NULL on failure.
 * @brief Create a new internal icondialog
 */
Ewl_Widget *
ewl_icondialog_new(void)
{
        Ewl_Icondialog *d;

        DENTER_FUNCTION(DLEVEL_STABLE);

        d = NEW(Ewl_Icondialog, 1);
        if (!d) {
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        if (!ewl_icondialog_init(d)) {
                ewl_widget_destroy(EWL_WIDGET(d));
                d = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(d), DLEVEL_STABLE);
}

/**
 * @param dialog: the icondialog to initialize.
 * @return Return TRUE on success, FALSE otherwise.
 * @brief Initialize an internal icondialog to starting values
 */
int
ewl_icondialog_init(Ewl_Icondialog *dialog)
{
        Ewl_Widget *w;
        Ewl_Widget *box;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dialog, FALSE);

        w = EWL_WIDGET(dialog);

        if (!ewl_dialog_init(EWL_DIALOG(dialog))) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_widget_appearance_set(w, EWL_ICONDIALOG_TYPE);
        ewl_widget_inherit(w, EWL_ICONDIALOG_TYPE);

        /* get the parent vbox */
        box = dialog->dialog.vbox;
        /* we need a hbox */
        ewl_box_orientation_set(EWL_BOX(box), EWL_ORIENTATION_HORIZONTAL);

        /*
         * Setup a vertical box for the displayed window contents.
         */
        dialog->vbox = ewl_vbox_new();
        if (!dialog->vbox) {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_container_child_append(EWL_CONTAINER(box), dialog->vbox);
        ewl_widget_internal_set(dialog->vbox, TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(dialog->vbox), EWL_FLAG_FILL_ALL);
        ewl_widget_show(dialog->vbox);

        ewl_container_redirect_set(EWL_CONTAINER(box), 
                                        EWL_CONTAINER(dialog->vbox));

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param d: dialog to change the icon
 * @param theme_icon: the name of the theme icon
 * @return Returns no value.
 * @brief Changes the icon of the dialog. 
 *
 * Changes the icon to the given string. @p theme_icon can be any icon that
 * is defined in the ewl_theme_icon.h file.  If @p theme_icon is @c NULL
 * no icon will be create or an existing icon will be destroyed.
 */
void
ewl_icondialog_icon_set(Ewl_Icondialog *d, const char *theme_icon)
{
        const char *image;
        int size;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(d);
        DCHECK_TYPE(d, EWL_ICONDIALOG_TYPE);

        /* 
         * if there is no icon destroy the existing one and we are done
         */
        if (!theme_icon)
        {
                if (d->icon)
                        ewl_widget_destroy(d->icon);
                DRETURN(DLEVEL_STABLE);
        }

        /* get the size from the theme or set a default value */
        size = ewl_theme_data_int_get(EWL_WIDGET(d), "icon_size");
        if (!size) size = 64;

        /* get the file for the image */
        image = ewl_icon_theme_icon_path_get(theme_icon, size);
        if (!image)
                DRETURN(DLEVEL_STABLE);

        /* and finally create the icon */
        d->icon = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(d->icon), image, NULL);
        ewl_object_minimum_size_set(EWL_OBJECT(d->icon), size, size);
        
        /* we need to reset the container redirect before we can add the icon */
        ewl_container_redirect_set(EWL_CONTAINER(d->dialog.vbox), NULL);
        ewl_container_child_prepend(EWL_CONTAINER(d->dialog.vbox), d->icon);
        ewl_container_redirect_set(EWL_CONTAINER(d->dialog.vbox), 
                                        EWL_CONTAINER(d->vbox));

        ewl_widget_show(d->icon);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

