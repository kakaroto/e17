#ifndef EWL_PROGRESSBAR_H
#define EWL_PROGRESSBAR_H

/**
 * @addtogroup Ewl_Progressbar Ewl_Progressbar: A progressbar from a value
 * A progress bar widget
 *
 * @{
 */

/**
 * @themekey /progressbar/file
 * @themekey /progressbar/group
 * @themekey /bar/file
 * @themekey /bar/group
 */

/**
 * @def EWL_PROGRESSBAR_TYPE
 * The type name for the Ewl_Progressbar widget
 */
#define EWL_PROGRESSBAR_TYPE "progressbar"

/**
 * The Ewl_Progressbar provides a statusbar from a given value
 */
typedef struct Ewl_Progressbar Ewl_Progressbar;

/**
 * @def EWL_PROGRESSBAR(seeker)
 * Typecasts a pointer to an Ewl_Progressbar pointer.
 */
#define EWL_PROGRESSBAR(progressbar) ((Ewl_Progressbar *) progressbar)

/**
 * Inherits from Ewl_Range and creates an internal Ewl_Widget, the
 * statusbar itself.
 */
struct Ewl_Progressbar
{
	Ewl_Range        range;     /**< Inherit from Ewl_Range */
	Ewl_Widget      *bar;       /**< The moving bar on top */
	Ewl_Widget      *label;     /**< text label on the bar */
	int              auto_label;  /**< flag if user is setting label or not */
};


Ewl_Widget 	*ewl_progressbar_new(void);
int  	  	 ewl_progressbar_init(Ewl_Progressbar *p);

void 		 ewl_progressbar_label_set(Ewl_Progressbar *p, char *label);
void 		 ewl_progressbar_custom_label_set(Ewl_Progressbar *p, 
							char *format_string);

void 		 ewl_progressbar_label_show(Ewl_Progressbar *p);
void 		 ewl_progressbar_label_hide(Ewl_Progressbar *p);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_progressbar_configure_cb(Ewl_Widget *w, void *ev_data,
				  void *user_data);
void ewl_progressbar_value_changed_cb(Ewl_Widget *w, void *ev_data,
				  void *user_data);
void ewl_progressbar_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_progressbar_child_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
				     Ewl_Orientation o);

/**
 * @}
 */

#endif
