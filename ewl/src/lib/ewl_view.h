/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_VIEW_H
#define EWL_VIEW_H

/**
 * @addtogroup Ewl_View Ewl_View: A data view
 * @brief Defines the callbacks for setting up the widgets based on the data
 * returned from the Model. Create widgets, set data on widgets, calculate
 * sizing, minimize number of widgets.
 *
 * @{
 */

/**
 * @def EWL_VIEW_WIDGET_FETCH(f)
 * View callback for the constructor for a data row
 */
#define EWL_VIEW_WIDGET_FETCH(f) ((Ewl_View_Widget_Fetch)f)

/**
 * A typedef to shorten the definition of the view_widget_fetch callbacks.
 */
typedef Ewl_Widget *(*Ewl_View_Widget_Fetch)(void *data, unsigned int row,
                                             unsigned int col, 
                                             void *private_data);

/**
 * @def EWL_VIEW_HEADER_GET(f)
 * View callback to get the header for a given column
 */
#define EWL_VIEW_HEADER_GET(f) ((Ewl_View_Header_Fetch)f)

/**
 * A typedef to shorten the definition of the view_header_fetch callbacks.
 */
typedef Ewl_Widget *(*Ewl_View_Header_Fetch)(void *data, unsigned int column,
                                             void *private_data);

/**
 * @def EWL_VIEW(view)
 * Typecasts a pointer to an Ewl_View pointer.
 */
#define EWL_VIEW(view) ((Ewl_View *)view)

/**
 * The Ewl_View type
 */
typedef struct Ewl_View Ewl_View;

/**
 * @def EWL_VIEW_EXPANSION_VIEW_FETCH_GET(f)
 * View callback to get a view for expanded data on a row
 */
#define EWL_VIEW_EXPANSION_VIEW_FETCH_GET(f) ((Ewl_View_Expansion_View_Fetch)f)

/**
 * A typedef to shorten the definition of the view_expansion_view_fetch
 * callbacks.
 */
typedef Ewl_View *(*Ewl_View_Expansion_View_Fetch)(void *data,
                                                        unsigned int row);

/**
 * @brief The view function pointers
 */
struct Ewl_View
{
        Ewl_View_Widget_Fetch fetch;                  /**< Get a new widget for display */
        Ewl_View_Header_Fetch header_fetch;          /**< Get the header for the given column */
        Ewl_View_Expansion_View_Fetch expansion;  /**< Get a view for displaying expansion data */
};

Ewl_View                        *ewl_view_new(void);
int                              ewl_view_init(Ewl_View *view);
Ewl_View                        *ewl_view_clone(const Ewl_View *src);

void                             ewl_view_widget_fetch_set(Ewl_View *view,
                                                Ewl_View_Widget_Fetch construct);
Ewl_View_Widget_Fetch            ewl_view_widget_fetch_get(const Ewl_View *view);

void                             ewl_view_header_fetch_set(Ewl_View *v,
                                                Ewl_View_Header_Fetch f);
Ewl_View_Header_Fetch            ewl_view_header_fetch_get(const Ewl_View *v);

void                             ewl_view_expansion_view_fetch_set(Ewl_View *v,
                                                Ewl_View_Expansion_View_Fetch f);
Ewl_View_Expansion_View_Fetch    ewl_view_expansion_view_fetch_get(const Ewl_View *v);

/**
 * @}
 */

#endif

