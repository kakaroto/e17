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
 * @themekey /grabber/vertical/file
 * @themekey /grabber/vertical/group
 * @themekey /grabber/horizontal/file
 * @themekey /grabber/horizontal/group
 */

#define EWL_PANED_TYPE "paned"

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
 * Inherits from Ewl_Container and extends to provided the paned widget
 */
struct Ewl_Paned
{
	Ewl_Container container;
	Ewl_Orientation	orientation;
};

Ewl_Widget	*ewl_paned_new(void);
Ewl_Widget	*ewl_hpaned_new(void);
Ewl_Widget	*ewl_vpaned_new(void);
int		 ewl_paned_init(Ewl_Paned *p);

void 		 ewl_paned_orientation_set(Ewl_Paned *p, Ewl_Orientation o);
Ewl_Orientation  ewl_paned_orientation_get(Ewl_Paned *p);

/* 
 * Internal functions. Override at your risk.
 */
void ewl_paned_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_paned_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx);
void ewl_paned_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size,
							Ewl_Orientation o);
void ewl_paned_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_paned_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);

void ewl_paned_cb_configure(Ewl_Widget *w, void *ev, void *data);

/*
 * Ewl_Paned_Grabber stuff
 */

#define EWL_PANED_GRABBER_TYPE "paned_grabber"

typedef struct Ewl_Paned_Grabber Ewl_Paned_Grabber;

#define EWL_PANED_GRABBER(g) ((Ewl_Paned_Grabber *)g)

struct Ewl_Paned_Grabber
{
	Ewl_Separator separator;
	unsigned int placed:1;
};

Ewl_Widget 	*ewl_paned_grabber_new(void);
int 		 ewl_paned_grabber_init(Ewl_Paned_Grabber *g);

void		 ewl_paned_grabber_orientation_set(Ewl_Paned_Grabber *g, 
							Ewl_Orientation o);
Ewl_Orientation  ewl_paned_grabber_orientation_get(Ewl_Paned_Grabber *g);

void		 ewl_paned_grabber_show_cursor_for(Ewl_Paned_Grabber *g, 
							unsigned int dir);

/**
 * @}
 */

#endif


