/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FREEBOX_MVC_H
#define EWL_FREEBOX_MVC_H

#include "ewl_mvc.h"

/**
 * @addtogroup Ewl_Freebox_MVC Ewl_Freebox_MVC: A free layout widget
 * @brief Defines a class to layout data in a free form
 *
 * @remarks Inherits from Ewl_MVC.
 * 
 * @{
 */

/**
 * @def EWL_FREEBOX_MVC_TYPE
 * The type name of the Ewl_Freebox_MVC widget
 */
#define EWL_FREEBOX_MVC_TYPE "freebox_mvc"

/**
 * @def EWL_FREEBOX_MVC_IS(w)
 * Returns TRUE if the widget is an Ewl_Freebox_MVC, otherwise FALSE
 */
#define EWL_FREEBOX_MVC_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_FREEBOX_MVC_TYPE))

/**
 * @def EWL_FREEBOX_MVC(freebox_mvc)
 * Typecasts a pointer to an Ewl_Freebox_MVC pointer
 */
#define EWL_FREEBOX_MVC(box) ((Ewl_Freebox_MVC *)box)

/**
 * The freebox_mvc structure
 */
typedef struct Ewl_Freebox_MVC Ewl_Freebox_MVC;

/**
 * @brief Inherits from EWL_MVC and extends to provide a free layout widget
 */
struct Ewl_Freebox_MVC
{
        Ewl_MVC mvc;                /**< The mvc parent */

        Ewl_Widget *freebox;        /**< The freebox for the children */
};

Ewl_Widget                *ewl_freebox_mvc_new(void);
Ewl_Widget                *ewl_hfreebox_mvc_new(void);
Ewl_Widget                *ewl_vfreebox_mvc_new(void);
int                        ewl_freebox_mvc_init(Ewl_Freebox_MVC *fb_mvc);        

void                       ewl_freebox_mvc_orientation_set(Ewl_Freebox_MVC *fb_mvc,
                                                Ewl_Orientation orientation);
Ewl_Orientation            ewl_freebox_mvc_orientation_get(Ewl_Freebox_MVC *fb_mvc);

/*
 * Internal stuff.
 */
void ewl_freebox_mvc_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_freebox_mvc_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_freebox_mvc_cb_selected_change(Ewl_MVC *mvc);

/**
 * @}
 */

#endif
