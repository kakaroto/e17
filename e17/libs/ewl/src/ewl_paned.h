#ifndef __EWL_PANED_H__
#define __EWL_PANED_H__

/**
 * @defgroup Ewl_Paned Paned: A paned widget
 * @brief Provides a widget to have a resizable paned container
 *
 * @{
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
	Ewl_Box			base;
	Ewl_Box			*first;
	Ewl_Box			*grabber;
	Ewl_Box			*second;
	Ewl_Position		active;
	Ewl_Orientation		orientation;
	int			grabbed;
};

/**
 * @def ewl_hpaned_new()
 * Shortcut to allocate a new horizontal Ewl_Paned
 */
#define ewl_hpaned_new() ewl_paned_new(EWL_ORIENTATION_HORIZONTAL)

 /**
  * @def ewl_vpaned_new()
  * Shortcut to allocate a new vertial Ewl_Paned
  */
#define ewl_vpaned_new() ewl_paned_new(EWL_ORIENTATION_VERTICAL)

Ewl_Widget		*ewl_paned_new(Ewl_Orientation orient);
int			 ewl_paned_init(Ewl_Paned *p, Ewl_Orientation orient);

void			 ewl_paned_active_set(Ewl_Paned *p, Ewl_Position pos);
Ewl_Position	 	 ewl_paned_active_get(Ewl_Paned *p);

void			 ewl_paned_first_pane_append(Ewl_Paned *p, Ewl_Widget *w);
void			 ewl_paned_first_pane_prepend(Ewl_Paned *p, Ewl_Widget *w);
void			 ewl_paned_second_pane_append(Ewl_Paned *p, Ewl_Widget *w);
void			 ewl_paned_second_pane_prepend(Ewl_Paned *p, Ewl_Widget *w);

#endif


