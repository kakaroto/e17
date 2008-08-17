/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_menu_item.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/**
 * @return Returns a pointer to a new menu item on success, NULL on failure.
 * @brief Create a new menu item to place in a menu
 */
Ewl_Widget *
ewl_menu_item_new(void)
{
        Ewl_Menu_Item *item;

        DENTER_FUNCTION(DLEVEL_STABLE);

        item = NEW(Ewl_Menu_Item, 1);
        if (!item)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_menu_item_init(item)) {
                ewl_widget_destroy(EWL_WIDGET(item));
                item = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(item), DLEVEL_STABLE);
}

/**
 * @param item: the item to be initialized
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize the fields of a menu item to their defaults
 */
int
ewl_menu_item_init(Ewl_Menu_Item *item)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(item, FALSE);

        /*
         * Initialize the inherited container fields.
         */
        if (!ewl_button_init(EWL_BUTTON(item)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_button_fill_policy_set(EWL_BUTTON(item), EWL_FLAG_FILL_HFILL);
        ewl_button_alignment_set(EWL_BUTTON(item), EWL_FLAG_ALIGN_LEFT);
        ewl_button_label_set(EWL_BUTTON(item), "");
        ewl_button_image_size_set(EWL_BUTTON(item), 16, 16);
        ewl_widget_appearance_set(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);
        ewl_widget_inherit(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_HFILL);
        ewl_object_fill_policy_set(EWL_OBJECT(EWL_BUTTON(item)->label_object),
                        EWL_FLAG_FILL_HFILL);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

