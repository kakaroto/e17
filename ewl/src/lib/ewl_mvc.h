/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MVC_H
#define EWL_MVC_H

#include "ewl_box.h"
#include "ewl_model.h"
#include "ewl_view.h"

/**
 * @addtogroup Ewl_MVC Ewl_MVC: The base model-view-controller framework
 * Defines a widget to base MVC widgets off of
 *
 * @remarks Inherits from Ewl_Box.
 * @if HAVE_IMAGES
 * @image html Ewl_MVC_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_MVC_TYPE
 * The type name for the Ewl_MVC widget
 */
#define EWL_MVC_TYPE "mvc"

/**
 * @def EWL_MVC_IS(w)
 * Returns TRUE if the widget is an Ewl_Mvc, FALSE otherwise
 */
#define EWL_MVC_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MVC_TYPE))

/**
 * @def EWL_SELECTION(x)
 * Typecast a pointer to an Ewl_Selection pointer
 */
#define EWL_SELECTION(x) ((Ewl_Selection *)x)

/**
 * Ewl_Selection
 */
typedef struct Ewl_Selection Ewl_Selection;

/**
 * @brief Structure to store information on selections
 */
struct Ewl_Selection
{
        Ewl_Selection_Type type;        /**< The type of selection */
        const Ewl_Model *model;                /**< The model to work with this
                                                                selection */
        void *highlight;                /**< highlight widgets for the MVC */
        void *data;                        /**< Data containing the selection */
};

/**
 * @def EWL_SELECTION_IDX(x)
 * Typecast a pointer to an Ewl_Selection_Idx pointer
 */
#define EWL_SELECTION_IDX(x) ((Ewl_Selection_Idx *)x)

 /**
  * Ewl_Selection_Idx
  */
typedef struct Ewl_Selection_Idx Ewl_Selection_Idx;

/**
 * @brief Structure to store information on a single index selection
 */
struct Ewl_Selection_Idx
{
        Ewl_Selection sel;                /**< Inherit from Ewl_Selection */

        unsigned int row;                /**< Index row */
        unsigned int column;                /**< Index column */
};

/**
 * @def EWL_SELECTION_RANGE(x)
 * Typecasts a pointer to an Ewl_Selection_Range
 */
#define EWL_SELECTION_RANGE(x) ((Ewl_Selection_Range *)x)

/**
 * Ewl_Selection_Range
 */
typedef struct Ewl_Selection_Range Ewl_Selection_Range;

/**
 * @brief Structure to store a range of selected cells
 */
struct Ewl_Selection_Range
{
        Ewl_Selection sel;        /**< Inherit from Ewl_Selection */

        struct
        {
                unsigned int row;/**< Index row */
                unsigned int column;/**< Index column */
        } start,                /**< Start of range */
          end;                        /**< End of range */
};

/**
 * A simple mvc base class
 */
typedef struct Ewl_MVC Ewl_MVC;

/**
 * @def EWL_MVC(mvc)
 * Typecasts a pointer to an Ewl_MVC pointer.
 */
#define EWL_MVC(mvc) ((Ewl_MVC *)mvc)

/**
 * @brief Inherits from Ewl_Box and extends to provide mvc functionality
 */
struct Ewl_MVC
{
        Ewl_Box box;                /**< Inherit from Ewl_Box */

        void *data;                /**< The mvc data */
        const Ewl_View *view;        /**< The view for the mvc */
        const Ewl_Model *model;        /**< The model for the mvc */

        struct {
                void (*view_change)(Ewl_MVC *mvc);         /**< View change callback */
                void (*selected_change)(Ewl_MVC *mvc);        /**< Selected change callback */
        } cb;                        /**< Callbacks to the inheriting widgets */

        Ecore_List *selected;                /**< The selected cells */

        Ewl_Selection_Mode selection_mode;        /**< The widget selection mode*/
        unsigned char dirty:1;                /**< Is the data dirty */
};

int                      ewl_mvc_init(Ewl_MVC *mvc);

void                     ewl_mvc_view_set(Ewl_MVC *mvc, const Ewl_View *view);
const Ewl_View          *ewl_mvc_view_get(Ewl_MVC *mvc);

void                     ewl_mvc_model_set(Ewl_MVC *mvc, const Ewl_Model *model);
const Ewl_Model         *ewl_mvc_model_get(Ewl_MVC *mvc);

void                     ewl_mvc_data_set(Ewl_MVC *mvc, void *data);
void                    *ewl_mvc_data_get(Ewl_MVC *mvc);

void                     ewl_mvc_dirty_set(Ewl_MVC *mvc, unsigned int dirty);
unsigned int             ewl_mvc_dirty_get(Ewl_MVC *mvc);

void                     ewl_mvc_selection_mode_set(Ewl_MVC *mvc,
                                        Ewl_Selection_Mode mode);
Ewl_Selection_Mode       ewl_mvc_selection_mode_get(Ewl_MVC *mvc);

void                     ewl_mvc_selected_clear(Ewl_MVC *mvc);

void                     ewl_mvc_selected_list_set(Ewl_MVC *mvc,
                                        Ecore_List *list);
Ecore_List              *ewl_mvc_selected_list_get(Ewl_MVC *mvc);

void                     ewl_mvc_selected_range_add(Ewl_MVC *mvc, 
                                        const Ewl_Model * model,
                                        void *data, unsigned int srow,
                                        unsigned int scolumn,
                                        unsigned int erow,
                                        unsigned int ecolumn);

void                     ewl_mvc_selected_set(Ewl_MVC *mvc,
                                        const Ewl_Model *model,
                                        void *data, unsigned int row,
                                        unsigned int column);
void                     ewl_mvc_selected_add(Ewl_MVC *mvc,
                                        const Ewl_Model *model,
                                        void *data, unsigned int row,
                                        unsigned int column);
Ewl_Selection_Idx       *ewl_mvc_selected_get(Ewl_MVC *mvc);
void                     ewl_mvc_selected_rm(Ewl_MVC *mvc, void *data,
                                        unsigned int row,
                                        unsigned int column);

unsigned int             ewl_mvc_selected_count_get(Ewl_MVC *mvc);
unsigned int             ewl_mvc_selected_is(Ewl_MVC *mvc, void *data,
                                        unsigned int row,
                                        unsigned int column);

Ewl_Selection           *ewl_mvc_selection_index_new(const Ewl_Model *model,
                                        void *data,
                                        unsigned int row,
                                        unsigned int column);
Ewl_Selection           *ewl_mvc_selection_range_new(const Ewl_Model *model,
                                        void *data, unsigned int srow,
                                        unsigned int scolumn,
                                        unsigned int erow,
                                        unsigned int ecolumn);

void                     ewl_mvc_highlight(Ewl_MVC *mvc, Ewl_Container *c,
                                        Ewl_Widget *(*widget)(Ewl_MVC *mvc,
                                                void *data, unsigned int row,
                                                unsigned int column));

/*
 * internal
 */
void                 ewl_mvc_view_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc));
void                 ewl_mvc_selected_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc));

void                 ewl_mvc_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void                 ewl_mvc_cb_data_unref(Ewl_Widget *w, void *ev, void *data);

void                 ewl_mvc_handle_click(Ewl_MVC *mvc, const Ewl_Model *model,
                                        void *data, unsigned int row,
                                        unsigned int column);

/**
 * @}
 */

#endif

