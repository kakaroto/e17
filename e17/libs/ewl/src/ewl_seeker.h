
#ifndef __EWL_SEEKER_H__
#define __EWL_SEEKER_H__

/**
 * @defgroup Ewl_Seeker Seeker: A Value Selector from a Range
 * Defines an Ewl_Widget with a draggable button enclosed, used to select a
 * value from a range.
 *
 * @todo Add value labels and hash marks.
 */

/**
 * @themekey /hseeker/file
 * @themekey /hseeker/group
 * @themekey /vseeker/file
 * @themekey /vseeker/group
 */

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
 * @struct Ewl_Seeker
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
	int             dragstart; /**< The coordinate where the drag starts */
	int             autohide; /**< Indicator to hide when not scrollable */
};

/**
 * @def ewl_vseeker_new()
 * Shorthand to allocate a vertical seeker.
 */
#define ewl_vseeker_new() ewl_seeker_new(EWL_ORIENTATION_VERTICAL);

/**
 * @def ewl_hseeker_new()
 * Shorthand to allocate a horizontal seeker.
 */
#define ewl_hseeker_new() ewl_seeker_new(EWL_ORIENTATION_HORIZONTAL);

Ewl_Widget     *ewl_seeker_new(Ewl_Orientation orientation);
void            ewl_seeker_init(Ewl_Seeker * s, Ewl_Orientation orientation);

void            ewl_seeker_set_value(Ewl_Seeker * s, double v);
double          ewl_seeker_get_value(Ewl_Seeker * s);

void            ewl_seeker_set_range(Ewl_Seeker * s, double r);
double          ewl_seeker_get_range(Ewl_Seeker * s);

void            ewl_seeker_set_step(Ewl_Seeker * s, double step);
double          ewl_seeker_get_step(Ewl_Seeker * s);

void            ewl_seeker_set_autohide(Ewl_Seeker *s, int v);
int             ewl_seeker_get_autohide(Ewl_Seeker *s);

void            ewl_seeker_decrease(Ewl_Seeker * s);
void            ewl_seeker_increase(Ewl_Seeker * s);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_seeker_configure_cb(Ewl_Widget * w, void *ev_data,
				        void *user_data);
void            ewl_seeker_button_mouse_down_cb(Ewl_Widget * w, void *ev_data,
						void *user_data);
void            ewl_seeker_button_mouse_up_cb(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            ewl_seeker_button_mouse_move_cb(Ewl_Widget * w, void *ev_data,
						void *user_data);
void            ewl_seeker_mouse_down_cb(Ewl_Widget * w, void *ev_data,
					 void *user_data);

#endif				/* __EWL_SEEKER_H__ */
