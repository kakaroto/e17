#ifndef __EWL_DATEPICKER_H__
#define __EWL_DATEPICKER_H__



/**
 * @file ewl_datepicker.h
 * @defgroup Ewl_DatePicker datepicker: The EWL Datepicker widget
 * @brief Defines the Ewl_DatePicker class,  
 *
 */

/** 
 * @themekey /datepicker/file
 * @themekey /datepicker/group
 */


typedef struct Ewl_DatePicker Ewl_DatePicker;

/**
 * @def EWL_DATEPICKER(datepicker)
 * Typecast a pointer to an Ewl_DatePicker pointer
 */
#define EWL_DATEPICKER(datepicker) ((Ewl_DatePicker *) datepicker)

/** 
 * @struct Ewl_DatePicker
 * Inherits from an Ewl_Text 
 * 
 */
struct Ewl_DatePicker
{
	Ewl_Text  text; /**< Inherit from Ewl_Text */

	Ewl_Widget* calendar;
	Ewl_Widget* calendar_window;

};


Ewl_Widget	*ewl_datepicker_new(void);
int		ewl_datepicker_init(Ewl_DatePicker* datepicker);


#endif
