/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_DATEPICKER_H
#define EWL_DATEPICKER_H

/**
 * @addtogroup Ewl_Datepicker Ewl_Datepicker: The EWL Datepicker widget
 * @brief Defines the Ewl_Datepicker class,  
 *
 * @remarks Inherits from Ewl_Text.
 * @image html Ewl_Datepicker_inheritance.png
 *
 * @{
 */

/** 
 * @themekey /datepicker/file
 * @themekey /datepicker/group
 */

/**
 * @def EWL_DATEPICKER_TYPE
 * The type name for the Ewl_Datepicker widget
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
void ewl_datepicker_cb_destroy(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_cb_value_changed(Ewl_Widget *w, void *ev, void *user_data);
void ewl_datepicker_cb_window_mouse_down(Ewl_Widget *w, void *ev, 
							void *user_data);

/**
 * @}
 */

#endif
