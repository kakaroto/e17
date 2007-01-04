/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_POPUP_H
#define EWL_POPUP_H

/**
 * @addtogroup Ewl_Popup Ewl_Popup: A override window for the convenience use
 * Defines the Ewl_Popup class used for override window that need to be
 * placed with some kind of smartness
 *
 * @remarks Inherits from Ewl_Window.
 * @image html Ewl_Popup_inheritance.png
 *
 * @{
 */

/**
 * @def EWL_POPUP_TYPE
 * The type name for the Ewl_Popup widget
 */
#define EWL_POPUP_TYPE "popup"

/**
 * Provides an Ewl_Widget to simply act with override windows
 */
typedef struct Ewl_Popup Ewl_Popup;

/**
 * @def EWL_POPUP(popup)
 * Typecast a pointer to an Ewl_Popup pointer.
 */
#define EWL_POPUP(popup) ((Ewl_Popup *) popup)

/**
 * @brief Inherits from Ewl_Window
 */
struct Ewl_Popup
{
	Ewl_Window window; /**< Inherit from Ewl_Window */

	Ewl_Popup_Type type;    /**< The Popup type */
	Ewl_Widget *follow;	/**< The object to follow */
	int x;			/**< The x position */
	int y;			/**< The y position */
	int fit_to_follow;	/**< if the popup fits the size to its follow*/
};

Ewl_Widget  	*ewl_popup_new(void);
int 		 ewl_popup_init(Ewl_Popup *p);
void 		 ewl_popup_type_set(Ewl_Popup *p, Ewl_Popup_Type type);
Ewl_Popup_Type   ewl_popup_type_get(Ewl_Popup *p);
void 		 ewl_popup_follow_set(Ewl_Popup *p, Ewl_Widget *w);
void 		 ewl_popup_fit_to_follow_set(Ewl_Popup *p, int fit);
int 		 ewl_popup_fit_to_follow_get(Ewl_Popup *p);

void 		 ewl_popup_mouse_position_set(Ewl_Popup *p, int x, int y);

/*
 * Internal, override at your own risk
 */
void ewl_popup_cb_show(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_popup_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_popup_cb_child_resize(Ewl_Container *c, Ewl_Widget *w, int size, 
					Ewl_Orientation o);
void ewl_popup_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_popup_cb_follow_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_popup_cb_follow_configure(Ewl_Widget *w, void *ev_data, 
					void *user_data);

/**
 * @}
 */

#endif
