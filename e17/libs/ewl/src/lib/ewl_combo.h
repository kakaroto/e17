/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_COMBO_H
#define EWL_COMBO_H

#include "ewl_mvc.h"
#include "ewl_cell.h"

/**
 * @addtogroup Ewl_Combo Ewl_Combo: A Simple Combo Box
 * Defines a combo box used internally. The contents on the box are not drawn
 * outside of the Evas.
 *
 * @remarks Inherits from Ewl_MVC.
 * @if HAVE_IMAGES
 * @image html Ewl_Combo_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /combo/file
 * @themekey /combo/group
 */

/**
 * @def EWL_COMBO_TYPE
 * The type name for the Ewl_Combo widget
 */
#define EWL_COMBO_TYPE "combo"

/**
 * @def EWL_COMBO_IS(w)
 * Returns TRUE if the widget is an Ewl_Combo, FALSE otherwise
 */
#define EWL_COMBO_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_COMBO_TYPE))

/**
 * A simple internal combo box, it is limited to drawing within the current
 * evas.
 */
typedef struct Ewl_Combo Ewl_Combo;

/**
 * @def EWL_COMBO(combo)
 * Typecasts a pointer to an Ewl_Combo pointer.
 */
#define EWL_COMBO(combo) ((Ewl_Combo *) combo)

/**
 * @brief Inherits from the Ewl_MVC widget to provide a combo widget
 */
struct Ewl_Combo
{
        Ewl_MVC mvc;                    /**< Inherit from Ewl_MVC */

        Ewl_Widget *popup;              /**< Use a popup to display with. */
        Ewl_Widget *button;             /**< expand/contract button */
        Ewl_Widget *header;             /**< The combo header widget */
        unsigned char editable:1;       /**< Is the combo editable */
        unsigned char scrollable:1;     /**< Is the combo scrollable */
};

Ewl_Widget       *ewl_combo_new(void);
int               ewl_combo_init(Ewl_Combo *combo);

void              ewl_combo_editable_set(Ewl_Combo *combo,
                                                unsigned int editable);
unsigned int      ewl_combo_editable_get(Ewl_Combo *combo);
void              ewl_combo_scrollable_set(Ewl_Combo *combo,
                                                unsigned int scrollable);
unsigned int      ewl_combo_scrollable_get(Ewl_Combo *combo);

void              ewl_combo_popup_container_set(Ewl_Combo *combo,
                                                Ewl_Container *c);


/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_combo_cb_decrement_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_combo_cb_popup_hide(Ewl_Widget *w, void *ev, void *data);

/*
 * Ewl_Combo_Cell
 */

/**
 * @def EWL_COMBO_CELL
 * The type name for the Ewl_Combo_Cell widget
 */
#define EWL_COMBO_CELL_TYPE "combo_cell"

/**
 * @def EWL_COMBO_CELL_IS(w)
 * Returns TRUE if the widget is an Ewl_Combo_Cell, FALSE otherwise
 */
#define EWL_COMBO_CELL_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_COMBO_CELL_TYPE))

/**
 * Typedef for the Ewl_Combo_Cell struct
 */
typedef struct Ewl_Combo_Cell Ewl_Combo_Cell;

/**
 * @def EWL_COMBO_CELL(c)
 * Typecasts a pointer to an Ewl_Combo_Cell pointer
 */
#define EWL_COMBO_CELL(c) ((Ewl_Combo_Cell *)c)

/**
 * @brief Inherits from Ewl_Cell and is used internally for the combo box
 */
struct Ewl_Combo_Cell
{
        Ewl_Cell cell;           /**< Inherit from the cell */
        Ewl_Combo *combo;        /**< The parent combo */
        const Ewl_Model *model;  /**< The model that was used to build the
                                        content of the cell */
        void *mvc_data;          /**< The mvc data that was used to build the
                                        content of the cell */
};

Ewl_Widget       *ewl_combo_cell_new(void);
int               ewl_combo_cell_init(Ewl_Combo_Cell *c);
void              ewl_combo_cell_combo_set(Ewl_Combo_Cell *c, Ewl_Combo *combo);
Ewl_Combo        *ewl_combo_cell_combo_get(Ewl_Combo_Cell *c);

void              ewl_combo_cell_model_set(Ewl_Combo_Cell *c, 
                                                const Ewl_Model *model);
const Ewl_Model  *ewl_combo_cell_model_get(Ewl_Combo_Cell *c);

void              ewl_combo_cell_data_set(Ewl_Combo_Cell *c, void *mvc_data);
void             *ewl_combo_cell_data_get(Ewl_Combo_Cell *c);

/**
 * @}
 */
void ewl_combo_cell_cb_clicked(Ewl_Widget *w, void *ev, void *data);

#endif
