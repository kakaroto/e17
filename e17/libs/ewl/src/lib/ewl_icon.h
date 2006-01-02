#ifndef EWL_ICON_H
#define EWL_ICON_H

/**
 * @file ewl_icon.h
 *
 * @{
 */

#define EWL_ICON(icon) ((Ewl_Icon *)icon)

typedef struct Ewl_Icon Ewl_Icon;
struct Ewl_Icon
{
	Ewl_Box box;
	Ewl_Widget *label;
	Ewl_Widget *preview;
	Ewl_Widget *menu;
	Ewl_Widget *extended;

	Ewl_Icon_Type type;
	unsigned int editable;
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

