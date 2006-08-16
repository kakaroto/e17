#include "Ewl.h"
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static int ewl_icon_theme_is_edje = 0;

/**
 * @return Returns no value
 * @brief Called when the icon theme is changed so we can clean up any
 * caching we have in place
 */
void
ewl_icon_theme_theme_change(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* check if this is an edje theme */
	if (ewl_config.theme.icon_theme && 
			(!strncasecmp(ewl_config.theme.icon_theme +
				(strlen(ewl_config.theme.icon_theme) - 4),
				".edj", 4)))
		ewl_icon_theme_is_edje = 1;
	else
		ewl_icon_theme_is_edje = 0;

	/* XXX Need to clear out any caches we have here */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The Icon Spec icon name to lookup
 * @param size: The size of the icon to retrieve
 * @return Returns the path to the icon we are looking for or NULL if none found
 * @brief Retrives the full path to the specified icon, or NULL if none found
 */
const char *
ewl_icon_theme_icon_path_get(const char *icon, const char *size)
{
	char *ret;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, NULL);

	/* make sure we have an icon theme */
	if (!ewl_config.theme.icon_theme)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	/* if our theme is an edje just return the .edj file */
	if (ewl_icon_theme_is_edje)
		DRETURN_PTR(ewl_config.theme.icon_theme, DLEVEL_STABLE);;

	/* XXX Should store a hash of these here so we don't have to keep
	 * looking it up. Just reset the hash on theme change */

	ret = ecore_desktop_icon_find((char *)icon, (char *)size, 
						ewl_config.theme.icon_theme);
	DRETURN_PTR(ret, DLEVEL_STABLE);
}

