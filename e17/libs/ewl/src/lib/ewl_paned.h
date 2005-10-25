#ifndef __EWL_PANED_H__
#define __EWL_PANED_H__

/**
 * @file ewl_paned.h
 * @defgroup Ewl_Paned Paned: A paned widget
 * @brief Provides a widget to have a resizable paned container
 *
 * @{
 */

/**
 * @themekey /paned/file
 * @themekey /paned/group
 */

/**
 */
typedef struct Ewl_Paned Ewl_Paned;

/**
 * @def EWL_PANED(pane)
 * Typecasts a pointer to a Ewl_Paned pointer
 */
#define EWL_PANED(paned) ((Ewl_Paned *) paned)

/**
 * @struct Ewl_Paned
 * Inherits from Ewl_Widget and extends to provided the paned widget
 */
struct Ewl_Paned
{
	Ewl_Box			 base;
	Ewl_Box			*first;
	Ewl_Widget              *grabber;
	Ewl_Box			*second;
	Ewl_Position		 active;
	Ewl_Orientation		 orientation;
	int			 grabbed;
};

Ewl_Widget		*ewl_paned_new(void);
Ewl_Widget		*ewl_hpaned_new(void);
Ewl_Widget		*ewl_vpaned_new(void);
int			 ewl_paned_init(Ewl_Paned *p);

void			 ewl_paned_active_area_set(Ewl_Paned *p, Ewl_Position pos);
Ewl_Position	 	 ewl_paned_active_area_get(Ewl_Paned *p);

/*
 * Internally used callbacks, override at your own risk.
 */
void                     ewl_paned_configure_cb(Ewl_Widget *w, void *ev, void *user_data);

/**
 * @}
 */

#endif


