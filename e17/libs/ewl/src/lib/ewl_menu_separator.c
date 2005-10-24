#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new menu item separator on success, NULL on failure.
 * @brief Create a separator menu item
 */
Ewl_Widget *
ewl_menu_separator_new(void)
{
	Ewl_Menu_Separator *sep;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sep = NEW(Ewl_Menu_Separator, 1);
	if (!sep)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_menu_separator_init(sep)) {
		ewl_widget_destroy(EWL_WIDGET(sep));
		sep = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(sep), DLEVEL_STABLE);
}

/**
 * @param sep: the menu separator item to initialize
 * @return Returns TRUE on success or FALSE on failure.
 * @brief Initialize a menu separator item
 *
 * Sets up the internal fields of the menu separator item to
 * some sane defaults.
 */
int
ewl_menu_separator_init(Ewl_Menu_Separator *sep)
{
	Ewl_Widget *separator;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("sep", sep, FALSE);

	if (!ewl_menu_item_init(EWL_MENU_ITEM(sep)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	separator = ewl_separator_new(EWL_ORIENTATION_HORIZONTAL);
	if (!separator)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_container_child_append(EWL_CONTAINER(sep), separator);
	ewl_widget_show(separator);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

