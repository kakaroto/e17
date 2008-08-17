/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_CONTEXT_MENU_H
#define EWL_CONTEXT_MENU_H

#include "ewl_popup.h"
/**
 * @addtogroup Ewl_Context_Menu Ewl_Context_Menu: A context menu
 *
 * @remarks Inherits from Ewl_Popup.
 * @if HAVE_IMAGES
 * @image html Ewl_Context_Menu_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_CONTEXT_MENU_TYPE
 * The type name for the Ewl_Context_Menu widget
 */
#define EWL_CONTEXT_MENU_TYPE "context_menu"

/**
 * @def EWL_CONTEXT_MENU_IS(w)
 * Returns TRUE if the widget is an Ewl_Context_Menu, FALSE otherwise
 */
#define EWL_CONTEXT_MENU_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), \
                                                        EWL_CONTEXT_MENU_TYPE))

/**
 * Ewl_Context_Menu
 */
typedef struct Ewl_Context_Menu Ewl_Context_Menu;

/**
 * @def EWL_CONTEXT_MENU(context_menu)
 * Typecast a pointer to an Ewl_Context_Menu pointer.
 */
#define EWL_CONTEXT_MENU(context_menu) ((Ewl_Context_Menu *) context_menu)

/**
 * @brief Inherits from Ewl_Popup and extends to provide a context menu
 */
struct Ewl_Context_Menu
{
        Ewl_Popup popup; /**< Inherit from Ewl_Popup */
        Ewl_Widget *open_menu; /**< a pointer to the current open submenu */
        Ewl_Widget *container; /**< the container holding the children */
        Ewl_Child_Add child_add; /**< the overridden add cb of the container */
        Ewl_Child_Remove child_remove; /** < the overridden remove cb */
};

Ewl_Widget          *ewl_context_menu_new(void);
int                  ewl_context_menu_init(Ewl_Context_Menu *cm);
void                 ewl_context_menu_attach(Ewl_Context_Menu *cm, Ewl_Widget *w);
void                 ewl_context_menu_detach(Ewl_Context_Menu *cm, Ewl_Widget *w);

void                 ewl_context_menu_container_set(Ewl_Context_Menu *cm,
                                                        Ewl_Container *c);

/*
 * Internal, override at your own risk
 */
void ewl_context_menu_cb_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_context_menu_cb_mouse_down(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_context_menu_cb_mouse_move(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_context_menu_cb_focus_in(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_context_menu_cb_attach_mouse_down(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_context_menu_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_context_menu_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx);
void ewl_context_menu_cb_child_mouse_in(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);
void ewl_context_menu_cb_child_clicked(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);

/**
 * @}
 */

#endif
