/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TREE_H
#define EWL_TREE_H

#include "ewl_mvc.h"
#include "ewl_row.h"

/**
 * @addtogroup Ewl_Tree Ewl_Tree: A Widget for List or Tree Layout
 * @brief Defines a widget for laying out other widgets in a tree or list
 * like manner. This also fulfills the functionality often seen in a table
 * widget.
 *
 * Model:
 * Defines communication callbacks for views and controllers. Query row/column
 * data, indicate expansion points, notify views and controllers of changes,
 * trigger sorting on a row/column combination.
 *
 * Data:
 * Provides a wrapper to the data which allows for observer registration and a
 * reference to a model for data access.
 *
 * View:
 * Defines the callbacks for setting up the widgets based on the data returned
 * from the Model. Create widgets, set data on widgets, calculate sizing,
 * minimize number of widgets.
 *
 * Controller:
 * The tree defines the entire controller and registers with the model.
 * Provides controls to sort columns, expand/collapse branching points, reacts
 * to changes from data model.
 *
 * Simple case:
 * Widgets packed using standard container functions. This should result in a
 * simple list of packed widgets.
 *
 * Features:
 * * Default data provider, so standard container functions are supported.
 * * MVC pattern of data access for more powerful layouts.
 * * Packing of arbitrary widget types in cells.
 * * Theming based on repeating row counts.
 * * Number of columns dependant on number of providers, not a fixed number.
 * * When using data providers, can reap widgets to save memory and decrease
 *   layout time.
 *
 * Issues:
 * * How do we support click callbacks on full rows? Special notifier
 *   necessary? Proposal: Tree value change callback on row selection. Passes
 *   event structure with array of selected row numbers. Simple case handled
 *   by callback on packed widgets.
 * * Column or row layout? Column allows for redirecting to a container to
 *   handle the case of container functions used to add widgets and to use a
 *   fixed number of rows in display region. It would also allow for skipping
 *   configure of an entire off-screen column, rows would lay out the cells
 *   off-screen. How do we implement column layout? Proposal: Row height
 *   cache, easy for fixed height rows, more complex for variable height.
 *
 * @remarks Inherits from Ewl_MVC.
 * @if HAVE_IMAGES
 * @image html Ewl_Tree_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /tree/file
 * @themekey /tree/group
 */

/**
 * @def EWL_TREE_TYPE
 * The type name for the Ewl_ widget
 */
#define EWL_TREE_TYPE "tree"

/**
 * @def EWL_TREE_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree, FALSE otherwise
 */
#define EWL_TREE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_TYPE))

/**
 * @def EWL_TREE(t)
 * Typecasts a pointer to an Ewl_Tree pointer.
 */
#define EWL_TREE(t) ((Ewl_Tree *)t)

/**
 * The Ewl_Tree widget
 */
typedef struct Ewl_Tree Ewl_Tree;

/**
 * @brief Inherits from Ewl_MVC and extends to provide a tree widget.
 */
struct Ewl_Tree
{
        Ewl_MVC mvc;                 /**< Inherit from ewl_mvc. */

        const Ewl_View *content_view;        /**< View for the content widget */

        Ewl_Widget *header;         /**< The tree header */
        Ewl_Widget *rows;         /**< The rows of the tree */

        unsigned int *rowcache; /**< Cache of row sizes */

        Ewl_Tree_Selection_Type type;         /**< The selection type of the tree */

        Ecore_Hash *expansions;        /**< Hash of expanded rows */

        struct
        {
                unsigned int column;        /**< The sort column */
                Ewl_Sort_Direction direction; /**< Sort direction */
        } sort;                        /**< The sort information */

        unsigned int columns;        /**< Number of columns in the tree */
        unsigned char fixed:1;    /**< Rows are fixed height */
        unsigned char headers_visible:1; /**< Are the headers visible? */
        unsigned char row_color_alternate:1; /**< Are the rows alternating? */
};

/*
 * Tree view/controller manipulation
 */
Ewl_Widget              *ewl_tree_new(void);
int                      ewl_tree_init(Ewl_Tree *tree);

void                     ewl_tree_column_count_set(Ewl_Tree *tree,
                                        unsigned int count);
unsigned int             ewl_tree_column_count_get(Ewl_Tree *tree);
void                     ewl_tree_column_fixed_size_set(Ewl_Tree *tree, 
                                        unsigned int col, unsigned int fixed);
unsigned int             ewl_tree_column_fixed_size_get(Ewl_Tree *tree, 
                                        unsigned int col);
void                     ewl_tree_column_initial_size_set(Ewl_Tree *tree, 
                                        unsigned int col, int size);
int                      ewl_tree_column_initial_size_get(Ewl_Tree *tree, 
                                        unsigned int col);

void                     ewl_tree_headers_visible_set(Ewl_Tree *tree,
                                        unsigned char visible);
unsigned int             ewl_tree_headers_visible_get(Ewl_Tree *tree);

void                     ewl_tree_content_view_set(Ewl_Tree *tree,
                                        const Ewl_View *view);
const Ewl_View          *ewl_tree_content_view_get(Ewl_Tree *tree);

Ewl_Tree_Selection_Type  ewl_tree_selection_type_get(Ewl_Tree *tree);
void                     ewl_tree_selection_type_set(Ewl_Tree *tree,
                                        Ewl_Tree_Selection_Type type);

void                     ewl_tree_fixed_rows_set(Ewl_Tree *tree,
                                        unsigned int fixed);
unsigned int             ewl_tree_fixed_rows_get(Ewl_Tree *tree);

void                     ewl_tree_alternate_row_colors_set(Ewl_Tree *tree,
                                        unsigned char alternate);
unsigned int             ewl_tree_alternate_row_colors_get(Ewl_Tree *tree);

Ewl_Widget              *ewl_tree_content_widget_get(Ewl_Tree *tree);

void                     ewl_tree_row_expand(Ewl_Tree *tree, void *data,
                                        unsigned int row);
void                     ewl_tree_row_collapse(Ewl_Tree *tree, void *data,
                                        unsigned int row);

unsigned int             ewl_tree_row_expanded_is(Ewl_Tree *tree, void *data,
                                        unsigned int row);

void                     ewl_tree_row_visible_ensure(Ewl_Tree *tree,
                                        void *data, unsigned int row);

void                     ewl_tree_kinetic_scrolling_set(Ewl_Tree *tree,
                                        Ewl_Kinetic_Scroll type);
Ewl_Kinetic_Scroll       ewl_tree_kinetic_scrolling_get(Ewl_Tree *tree);
void                     ewl_tree_kinetic_max_velocity_set(Ewl_Tree *tree,
                                        double v);
double                   ewl_tree_kinetic_max_velocity_get(Ewl_Tree *tree);
void                     ewl_tree_kinetic_min_velocity_set(Ewl_Tree *tree,
                                        double v);
double                   ewl_tree_kinetic_min_velocity_get(Ewl_Tree *tree);
void                     ewl_tree_kinetic_dampen_set(Ewl_Tree *tree, double d);
double                   ewl_tree_kinetic_dampen_get(Ewl_Tree *tree);
void                     ewl_tree_kinetic_fps_set(Ewl_Tree *tree, int fps); 
int                      ewl_tree_kinetic_fps_get(Ewl_Tree *tree);

/*
 * Internal stuff.
 */
void ewl_tree_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_tree_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_tree_cb_column_sort(Ewl_Widget *w, void *ev, void *data);

/*
 * Ewl_Tree_Node stuff
 */
#define EWL_TREE_NODE_TYPE "node"

/**
 * @def EWL_TREE_NODE_IS(w)
 * Returns TRUE if the widget is an Ewl_Tree_Node, FALSE otherwise
 */
#define EWL_TREE_NODE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TREE_NODE_TYPE))

#define EWL_TREE_NODE(n) ((Ewl_Tree_Node *)n)

/**
 * Ewl_Tree_Node
 */
typedef struct Ewl_Tree_Node Ewl_Tree_Node;

/**
 * @brief Inherits from Ewl_Container and extends to hold information on a
 * row of the tree
 */
struct Ewl_Tree_Node
{
        Ewl_MVC mvc;

        Ewl_Widget *tree;                /**< The parent tree */
        Ewl_Widget *handle;                /**< the expansion handle */
        Ewl_Row *row;                        /**< The row this node is for */

        struct
        {
                const Ewl_Model *model;        /**< The model of the expansion */
                void *data;                /**< The data of the expansion */
        } expansion;

        unsigned int row_num;                /**< The row number of this row */
        Ewl_Tree_Node_Flags expanded;

        unsigned char built_children:1;        /**< Have we generated child nodes */
};

Ewl_Widget        *ewl_tree_node_new(void);
int                 ewl_tree_node_init(Ewl_Tree_Node *node);

void                 ewl_tree_node_expandable_set(Ewl_Tree_Node *node, 
                                                unsigned int expandable);
unsigned int         ewl_tree_node_expandable_get(Ewl_Tree_Node *node);

void                 ewl_tree_node_expand(Ewl_Tree_Node *node);
void                 ewl_tree_node_collapse(Ewl_Tree_Node *node);

unsigned int         ewl_tree_node_expanded_is(Ewl_Tree_Node *node);
void                 ewl_tree_node_row_set(Ewl_Tree_Node *node, Ewl_Row *row);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_tree_cb_node_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_node_data_unref(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_tree_cb_node_realize(Ewl_Widget *w, void *ev, void *data);
void ewl_tree_cb_node_toggle(Ewl_Widget *w, void *ev_data, void *user_data);

void ewl_tree_cb_node_child_show(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_child_hide(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_resize(Ewl_Container *c, Ewl_Widget *w, int size,
                                                     Ewl_Orientation o);
void ewl_tree_cb_node_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_tree_cb_node_child_del(Ewl_Container *c, Ewl_Widget *w, int idx);

/**
 * @}
 */

#endif

