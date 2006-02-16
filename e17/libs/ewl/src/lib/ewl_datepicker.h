#ifndef EWL_DATEPICKER_H
#define EWL_DATEPICKER_H

/**
 * @defgroup Ewl_Datepicker Ewl_Datepicker: The EWL Datepicker widget
 * @brief Defines the Ewl_Datepicker class,  
 *
 * @{
 */

/** 
 * @themekey /datepicker/file
 * @themekey /datepicker/group
 */

#define EWL_DATEPICKER_TYPE "datepicker"

/**
 * The Ewl_Datepicker
 */
typedef struct Ewl_Datepicker Ewl_Datepicker;

/**
 * @def EWL_DATEPICKER(datepicker)
 * Typecast a pointer to an Ewl_Datepicker pointer
 */
#define EWL_DATEPICKER(datepicker) ((Ewl_Datepicker *) datepicker)

/** 
 * Inherits from an Ewl_Text 
 */
struct Ewl_Datepicker
{
	Ewl_Text  text; 		/**< Inherit from Ewl_Text */

	Ewl_Widget *calendar;		/**< The calendar */
	Ewl_Widget *calendar_window;	/**< The window */

};

Ewl_Widget	*ewl_datepicker_new(void);
int		 ewl_datepicker_init(Ewl_Datepicker* datepicker);

/*
 * Internally used callbacks, override at your risk
 */
void ewl_datepicker_destroy_cb(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_configure_cb(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_value_changed_cb(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_realize_cb(Ewl_Widget *w, void *ev, void *user_data);

/**
 * @}
 */

#endif
