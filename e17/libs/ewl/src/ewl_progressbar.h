
#ifndef __EWL_PROGRESSBAR_H__
#define __EWL_PROGRESSBAR_H__

/**
 * @defgroup Ewl_Progressbar Progressbar: A progressbar from a value
 *
 */

/**
 * @themekey /progressbar/file
 * @themekey /progressbar/group
 * @themekey /bar/file
 * @themekey /bar/group
 */

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
 * @struct Ewl_Progressbar
 * Inherits from Ewl_Container and creates an internal Ewl_Widget, the
 * statusbar itself.
 * 
 */
struct Ewl_Progressbar
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */
	Ewl_Widget      *bar;      /**< The moving bar on top */
	Ewl_Widget      *text;     /**< text label on the bar */
	double          value;     /**< current value of the progressbar */
};


Ewl_Widget *ewl_progressbar_new();
void ewl_progressbar_init (Ewl_Progressbar * p);

void ewl_progressbar_set_value (Ewl_Progressbar * p, double v);
double ewl_progressbar_get_value(Ewl_Progressbar * p);

void ewl_progressbar_set_text (Ewl_Progressbar * p, char *text);

#endif				/* __EWL_PROGRESSBAR_H__ */
