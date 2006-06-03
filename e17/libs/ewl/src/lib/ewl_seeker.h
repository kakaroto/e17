#ifndef EWL_SEEKER_H
#define EWL_SEEKER_H

/**
 * @addtogroup Ewl_Seeker Ewl_Seeker: A Value Selector from a Range
 * Defines an Ewl_Widget with a draggable button enclosed, used to select a
 * value from a range.
 *
 * @{
 */

/**
 * @themekey /hseeker/file
 * @themekey /hseeker/group
 * @themekey /vseeker/file
 * @themekey /vseeker/group
 */

/**
 * @def EWL_SEEKER_TYPE
 * The type name for the Ewl_Seeker widget
 */
#define EWL_SEEKER_TYPE "seeker"

/**
 * The Ewl_Seeker provides a means to select a value from a range using a
 * draggable button.
 */
typedef struct Ewl_Seeker Ewl_Seeker;

/**
 * @def EWL_SEEKER(seeker)
 * Typecasts a pointer to an Ewl_Seeker pointer.
 */
#define EWL_SEEKER(seeker) ((Ewl_Seeker *) seeker)

/**
 * Inherits from Ewl_Container and creates internal Ewl_Widget's to provide a
 * range and a draggable area to select a value from that range.
 */
struct Ewl_Seeker
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */
	Ewl_Orientation orientation; /**< Indicates layout direction */
	Ewl_Widget     *button; /**< Draggable widget for selecting value */
	double          value; /**< Currently chosen value */
	double          range; /**< Total selectable range */
	double          step; /**< Size of increments in the range */
	int             invert; /**< Invert the axis */
	int             dragstart; /**< The coordinate where the drag starts */
	int             autohide; /**< Indicator to hide when not scrollable */
	Ecore_Timer    *timer; /**< Timer for scroll repeating */
	double          start_time; /**< Time the timer was started */
};

Ewl_Widget     *ewl_seeker_new(void);
Ewl_Widget     *ewl_hseeker_new(void);
Ewl_Widget     *ewl_vseeker_new(void);
int             ewl_seeker_init(Ewl_Seeker *s);

void            ewl_seeker_orientation_set(Ewl_Seeker *s, Ewl_Orientation o);
Ewl_Orientation ewl_seeker_orientation_get(Ewl_Seeker *s);

void            ewl_seeker_value_set(Ewl_Seeker *s, double v);
double          ewl_seeker_value_get(Ewl_Seeker *s);

void            ewl_seeker_range_set(Ewl_Seeker *s, double r);
double          ewl_seeker_range_get(Ewl_Seeker *s);

void            ewl_seeker_step_set(Ewl_Seeker *s, double step);
double          ewl_seeker_step_get(Ewl_Seeker *s);

void            ewl_seeker_autohide_set(Ewl_Seeker *s, int v);
int             ewl_seeker_autohide_get(Ewl_Seeker *s);

void            ewl_seeker_invert_set(Ewl_Seeker *s, int invert);
int             ewl_seeker_invert_get(Ewl_Seeker *s);

void            ewl_seeker_decrease(Ewl_Seeker *s);
void            ewl_seeker_increase(Ewl_Seeker *s);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_seeker_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_button_mouse_down_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_seeker_button_mouse_up_cb(Ewl_Widget *w, void *ev_data,
					      void *user_data);
void ewl_seeker_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_key_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_seeker_child_show_cb(Ewl_Container *p, Ewl_Widget * w);

/**
 * @}
 */

#endif
