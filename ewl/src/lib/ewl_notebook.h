/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_NOTEBOOK_H
#define EWL_NOTEBOOK_H

#include "ewl_box.h"

/**
 * @addtogroup Ewl_Notebook Ewl_Notebook: A notebook widget
 * Defines a class for notebook layout
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_Notebook_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /notebook
 * @themekey /notebook/tabbar
 * @themekey /notebook/tabbar/
 * @themekey /notebook/tabbar/tab
 * @themekey /notebook/tabbar/tab/label
 * @themekey /notebook/pages
 */

/**
 * @def EWL_NOTEBOOK_TYPE
 * The type name for the Ewl_Notebook widget
 */
#define EWL_NOTEBOOK_TYPE "notebook"

/**
 * @def EWL_NOTEBOOK_IS(w)
 * Returns TRUE if the widget is an Ewl_Notebook, FALSE otherwise
 */
#define EWL_NOTEBOOK_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_NOTEBOOK_TYPE))

/**
 * @def EWL_NOTEBOOK(n)
 * Typecasts a pointer to an Ewl_Notebook pointer.
 */
#define EWL_NOTEBOOK(n) ((Ewl_Notebook *)n)

/**
 * Holds the notebook information
 */
typedef struct Ewl_Notebook Ewl_Notebook;

/**
 * @brief Inherits from Ewl_Box and extends to provide for a notebook style layout.
 */
struct Ewl_Notebook
{
        Ewl_Box box;                    /**< Inherit from Ewl_Box */

        struct {
                Ewl_Widget *tabbar;     /**< Holds the tabs */
                Ewl_Widget *pages;      /**< Holds the notebook pages */
        } body;                         /**< The body of the notebook */

        Ewl_Widget *cur_page;           /**< The currently active page */
        Ewl_Position tabbar_position;   /**< The position of the tabbar in the notebook */
};

Ewl_Widget      *ewl_notebook_new(void);
int              ewl_notebook_init(Ewl_Notebook *n);

void             ewl_notebook_tabbar_alignment_set(Ewl_Notebook *n,
                                                unsigned int align);
unsigned int     ewl_notebook_tabbar_alignment_get(Ewl_Notebook *n);

void             ewl_notebook_tabbar_position_set(Ewl_Notebook *n,
                                                Ewl_Position pos);
Ewl_Position     ewl_notebook_tabbar_position_get(Ewl_Notebook *n);

void             ewl_notebook_tabbar_visible_set(Ewl_Notebook *n,
                                                unsigned int visible);
unsigned int     ewl_notebook_tabbar_visible_get(Ewl_Notebook *n);
void             ewl_notebook_tabbar_homogeneous_set(Ewl_Notebook *n,
                                                unsigned int h);
unsigned int     ewl_notebook_tabbar_homogeneous_get(Ewl_Notebook *n);

void             ewl_notebook_visible_page_set(Ewl_Notebook *n,
                                                Ewl_Widget *page);
Ewl_Widget      *ewl_notebook_visible_page_get(Ewl_Notebook *n);

void             ewl_notebook_page_tab_text_set(Ewl_Notebook *n,
                                                Ewl_Widget *page,
                                                const char *text);
const char      *ewl_notebook_page_tab_text_get(Ewl_Notebook *n,
                                                Ewl_Widget *page);

void             ewl_notebook_page_tab_widget_set(Ewl_Notebook *n,
                                                Ewl_Widget *page,
                                                Ewl_Widget *tab);
Ewl_Widget      *ewl_notebook_page_tab_widget_get(Ewl_Notebook *n,
                                                Ewl_Widget *page);

/*
 * Internal stuff.
 */
void ewl_notebook_cb_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_notebook_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int rem_idx);

void ewl_notebook_cb_tab_clicked(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

