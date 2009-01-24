/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_LIST2_H
#define EWL_LIST2_H

#include "ewl_mvc.h"

/**
 * @addtogroup Ewl_List Ewl_List: A list widget
 * @brief Defines a class to layout data in a list
 *
 * @remarks Inherits from Ewl_MVC.
 * @if HAVE_IMAGES
 * @image html Ewl_List_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_LIST_TYPE
 * The type name for the Ewl_List widget
 */
#define EWL_LIST2_TYPE "list2"

/**
 * @def EWL_LIST_IS(w)
 * Returns TRUE if the widget is an Ewl_LIST, FALSE otherwise
 */
#define EWL_LIST2_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_LIST2_TYPE))

/**
 * @def EWL_LIST(list)
 * Typecasts a pointer to an Ewl_List pointer
 */
#define EWL_LIST2(list) ((Ewl_List2 *)list)

/**
 * The list strucutre
 */
typedef struct Ewl_List2 Ewl_List2;

/**
 * @brief Inherits from Ewl_MVC and extends to provide a list widget
 */
struct Ewl_List2
{
        Ewl_MVC mvc;                    /**< The mvc parent */
        Ewl_Widget *scrollport;         /**< Scrollport used in optimization */
        Ewl_Widget *box;                /**< Box used to hold the items */
        unsigned char fixed:1;          /**< Widgets fixed height/width */
        int *pref_w;                    /**< Preferred width */
        int *pref_h;                    /**< Preferred height */
        unsigned int set_n;             /**< Previous box offset */
        unsigned int val_n;             /**< Previous size */
};

Ewl_Widget      *ewl_list2_new(void);
int              ewl_list2_init(Ewl_List2 *list);
void             ewl_list2_fixed_size_set(Ewl_List2 *list, unsigned char fixed);
unsigned char    ewl_list2_fixed_size_get(Ewl_List2 *list);

/*
 * Internal stuff.
 */
void ewl_list2_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_list2_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_list2_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_list2_cb_selected_change(Ewl_MVC *mvc);
void ewl_list2_cb_child_show(Ewl_Container *p, Ewl_Widget *c);
void ewl_list2_cb_child_resize(Ewl_Container *p, Ewl_Widget *c, int size,
                                                Ewl_Orientation o);

/**
 * @}
 */

#endif

