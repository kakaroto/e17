#ifndef EWL_ICON_H
#define EWL_ICON_H

/**
 * @addtogroup Ewl_Icon Ewl_Icon: An icon widget
 * @brief Describes a widget to layout and manipulate icons
 *
 * @{
 */

/**
 * @def EWL_ICON_TYPE
 * The type name for the Ewl_Icon widget
 */
#define EWL_ICON_TYPE "icon"

/**
 * @def EWL_ICON(icon)
 * Typecasts a pointer to an Ewl_Icon pointer.
 */
#define EWL_ICON(icon) ((Ewl_Icon *)icon)

/**
 * A widget to display and manipluate an icon
 */
typedef struct Ewl_Icon Ewl_Icon;

/**
 * Inherits from Ewl_Box and extends to provide for an icon layout
 */
struct Ewl_Icon
{
	Ewl_Box box;		/**< Inherit from Ewl_Box */
	Ewl_Widget *label;	/**< The icons label */
	Ewl_Widget *preview;	/**< The icons preview */
	Ewl_Widget *menu;	/**< The icons menu */
	Ewl_Widget *extended;	/**< The icons extended information */

	Ewl_Icon_Type type;	/**< The icons type */
	unsigned int editable;	/**< Is the icon editable? */
};

Ewl_Widget	*ewl_icon_new(void);
int		 ewl_icon_init(Ewl_Icon *icon);

void		 ewl_icon_type_set(Ewl_Icon *icon, Ewl_Icon_Type type);
Ewl_Icon_Type	 ewl_icon_type_get(Ewl_Icon *icon);

void		 ewl_icon_image_set(Ewl_Icon *icon, const char *file, 
						const char *key);
const char 	*ewl_icon_image_file_get(Ewl_Icon *icon);

void		 ewl_icon_editable_set(Ewl_Icon *icon, unsigned int e);
unsigned int	 ewl_icon_editable_get(Ewl_Icon *icon);

void		 ewl_icon_label_set(Ewl_Icon *icon, const char *label);
const char	*ewl_icon_label_get(Ewl_Icon *icon);

void		 ewl_icon_extended_data_set(Ewl_Icon *icon, Ewl_Widget *ext);
Ewl_Widget	*ewl_icon_extended_data_get(Ewl_Icon *icon);

void		 ewl_icon_menu_set(Ewl_Icon *icon, Ewl_Widget *menu);
Ewl_Widget	*ewl_icon_menu_get(Ewl_Icon *icon);

void		 ewl_icon_constrain_set(Ewl_Icon *icon, unsigned int val);
unsigned int	 ewl_icon_constrain_get(Ewl_Icon *icon);

/**
 * @}
 */

#endif

