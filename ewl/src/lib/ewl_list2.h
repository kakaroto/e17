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

typedef struct Ewl_List2_Cell_Info Ewl_List2_Cell_Info;
struct Ewl_List2_Cell_Info
{
        void *cell;
        int size;
};

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
        Ewl_Widget *port;               /**< Scrollport used in optimization */
        Ewl_List2_Cell_Info *cinfo;     /**< Preferred width */
        unsigned int info_count;
        unsigned int info_size;
        Ewl_Size_Acquisition sizing;
};

Ewl_Widget      *ewl_list2_new(void);
int              ewl_list2_init(Ewl_List2 *list);
void             ewl_list2_size_acquisition_set(Ewl_List2 *list,
                                                Ewl_Size_Acquisition sizing);
Ewl_Size_Acquisition    ewl_list2_size_acquisition_get(Ewl_List2 *list);

/*
 * Internal stuff.
 */
void ewl_list2_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_list2_cb_area_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_list2_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_list2_cb_selected_change(Ewl_MVC *mvc);

/**
 * @}
 */

#endif

