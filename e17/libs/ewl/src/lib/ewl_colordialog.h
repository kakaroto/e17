#ifndef EWL_COLORDIALOG_H
#define EWL_COLORDIALOG_H

/**
 * @defgroup Ewl_Colordialog Ewl_Colordialog: A Simple Colour Dialog widget
 *
 * @{
 */

#define EWL_COLORDIALOG_TYPE "colordialog"

/**
 * @def EWL_COLORDIALOG(cd)
 * Typecast a pointer to an Ewl_Colordialog pointer.
 */
#define EWL_COLORDIALOG(cd) ((Ewl_Colordialog *) cd)

/**
 * The Ewl_Colordialog
 */
typedef struct Ewl_Colordialog Ewl_Colordialog;

/**
 * @struct Ewl_Colordialog
 * Inherit from the Ewl_Dialog to create a color dialog widget
 */
struct Ewl_Colordialog
{ 
	Ewl_Dialog  dialog;	/**< Inherits from Ewl_Dialog */
	Ewl_Widget *picker;	/**< Store the color picker widget */
};

Ewl_Widget 	*ewl_colordialog_new(void);
int 		 ewl_colordialog_init(Ewl_Colordialog *cd);

void             ewl_colordialog_has_alpha_set(Ewl_Colordialog *cp, unsigned int alpha);
unsigned int     ewl_colordialog_has_alpha_get(Ewl_Colordialog *cp);
                                        
void             ewl_colordialog_alpha_set(Ewl_Colordialog *cp, unsigned int alpha);
unsigned int     ewl_colordialog_alpha_get(Ewl_Colordialog *cp);

void             ewl_colordialog_current_rgb_set(Ewl_Colordialog *cp, unsigned int r,
                                                        unsigned int g, unsigned int b);
void             ewl_colordialog_current_rgb_get(Ewl_Colordialog *cp, unsigned int *r,
                                                        unsigned int *g, unsigned int *b);

void             ewl_colordialog_previous_rgb_set(Ewl_Colordialog *cp, unsigned int r,
                                                        unsigned int g, unsigned int b);
void             ewl_colordialog_previous_rgb_get(Ewl_Colordialog *cp, unsigned int *r,
                                                        unsigned int *g, unsigned int *b);

void             ewl_colordialog_color_mode_set(Ewl_Colordialog *cp, Ewl_Color_Mode type);
Ewl_Color_Mode   ewl_colordialog_color_mode_get(Ewl_Colordialog *cp);

/*
 * Internally used callbacks, override at your own risk
*/
void ewl_colordialog_cb_button_click(Ewl_Widget *w, void *ev, void *data);
void ewl_colordialog_cb_delete_window(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

