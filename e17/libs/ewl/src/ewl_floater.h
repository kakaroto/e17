
#ifndef __EWL_FLOATER_H__
#define __EWL_FLOATER_H__

/**
 * @defgroup Ewl_Floater A Floating Container
 * @brief Defines a widget for layering above other widgets in EWL's drawing
 * area, with the ability to follow the movement of another widget.
 *
 * @{
 */

/**
 * The floater is a container for widgets that need to be drawn on top
 * of multiple widgets (ie. dropdownbox)
 */
typedef struct Ewl_Floater Ewl_Floater;

/**
 * @def EWL_FLOATER(floater)
 * Typecasts a pointer to an Ewl_Floater pointer.
 */
#define EWL_FLOATER(floater) ((Ewl_Floater *) floater)

/**
 * @struct Ewl_Floater
 * Inherits from Ewl_Box for layout purposes. Provides a means to layer above
 * other widgets and to follow the movement of another widget.
 */
struct Ewl_Floater
{
	Ewl_Box         box; /**< Inherit from Ewl_Box */
	Ewl_Widget     *follows; /**< The widget to position relative to */
	int             x; /**< The x offset from follows x positon */
	int             y; /**< The y offset from follows y positon */
};


Ewl_Widget     *ewl_floater_new(Ewl_Widget * parent);
void            ewl_floater_set_position(Ewl_Floater * parent, int x, int y);
void            ewl_floater_init(Ewl_Floater * f, Ewl_Widget * parent);

/**
 * @}
 */

#endif				/* __EWL_FLOATER_H__ */
