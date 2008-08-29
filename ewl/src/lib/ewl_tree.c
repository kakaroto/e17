/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_tree.h"
#include "ewl_button.h"
#include "ewl_cell.h"
#include "ewl_check.h"
#include "ewl_expansion.h"
#include "ewl_label.h"
#include "ewl_paned.h"
#include "ewl_tree_view_scrolled.h"
#include "ewl_scrollpane.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#define EWL_TREE_EXPANSIONS_LIST(el) ((Ewl_Tree_Expansions_List *)(el))

typedef struct Ewl_Tree_Expansions_List Ewl_Tree_Expansions_List;

/**
 * @brief Stores informtion on tree expansion points
 */
struct Ewl_Tree_Expansions_List
{
        Ewl_Container *c;
        unsigned int *expanded;
        unsigned int size;
};

static void ewl_tree_build_tree(Ewl_Tree *tree);
static void ewl_tree_build_tree_rows(Ewl_Tree *tree,
                        const Ewl_Model *model, const Ewl_View *view, 
                        void *data, int colour, Ewl_Container *parent,
                        int hidden);
static void ewl_tree_headers_build(Ewl_Tree *tree, const Ewl_Model *model, 
                        void *mvc_data);
static void ewl_tree_cb_header_changed(Ewl_Widget *w, void *ev,
                                                        void *data);

static void ewl_tree_cb_row_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_row_highlight(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_row_unhighlight(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_cell_clicked(Ewl_Widget *w, void *ev, void *data);
static void ewl_tree_cb_selected_change(Ewl_MVC *mvc);
static Ewl_Widget *ewl_tree_widget_at(Ewl_MVC *mvc, void *data,
                                        unsigned int row, unsigned int column);

static void ewl_tree_expansions_hash_create(Ewl_Tree *tree);

static Ewl_Tree_Expansions_List *ewl_tree_expansions_list_new(void);
static void ewl_tree_expansions_list_destroy(Ewl_Tree_Expansions_List *el);
Ewl_Scrollpane *ewl_tree_kinetic_scrollpane_get(Ewl_Tree *tree);

/**
 * @return Returns NULL on failure, a new tree widget on success.
 * @brief Allocate and initialize a new tree widget
 */
Ewl_Widget *
ewl_tree_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Tree, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_tree_init(EWL_TREE(w)))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param tree: the tree widget to be initialized
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the contents of a tree widget
 *
 * The contents of the tree widget @a tree are initialized to their defaults.
 */
int
ewl_tree_init(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, FALSE);

        if (!ewl_mvc_init(EWL_MVC(tree)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(tree), EWL_TREE_TYPE);
        ewl_widget_inherit(EWL_WIDGET(tree), EWL_TREE_TYPE);

        ewl_mvc_selected_change_cb_set(EWL_MVC(tree),
                                        ewl_tree_cb_selected_change);

        ewl_object_fill_policy_set(EWL_OBJECT(tree), EWL_FLAG_FILL_ALL);

        tree->type = EWL_TREE_SELECTION_TYPE_CELL;

        tree->header = ewl_hpaned_new();
        ewl_container_child_append(EWL_CONTAINER(tree), tree->header);
        ewl_widget_appearance_set(EWL_WIDGET(tree->header), "tree_header");
        ewl_object_fill_policy_set(EWL_OBJECT(tree->header),
                                                        EWL_FLAG_FILL_HFILL);
        ewl_callback_append(tree->header, EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_tree_cb_header_changed, tree);
        ewl_widget_show(tree->header);

        /* set the default row view */
        ewl_tree_content_view_set(tree, ewl_tree_view_scrolled_get());

        ewl_tree_column_count_set(tree, 1);
        ewl_tree_headers_visible_set(tree, TRUE);
        ewl_tree_fixed_rows_set(tree, FALSE);
        ewl_tree_alternate_row_colors_set(tree, TRUE);

        ewl_callback_append(EWL_WIDGET(tree), EWL_CALLBACK_CONFIGURE,
                                        ewl_tree_cb_configure, NULL);
        ewl_callback_prepend(EWL_WIDGET(tree), EWL_CALLBACK_DESTROY,
                                        ewl_tree_cb_destroy, NULL);

        ewl_widget_focusable_set(EWL_WIDGET(tree), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to toggle the header visibility
 * @param visible: The visiblity to set the tree to (TRUE == on, FALSE == off)
 * @return Returns no value
 * @brief Toggle if the header is visible in the tree
 */
void
ewl_tree_headers_visible_set(Ewl_Tree *tree, unsigned char visible)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->headers_visible == !!visible)
                DRETURN(DLEVEL_STABLE);

        tree->headers_visible = !!visible;

        if (!tree->headers_visible)
                ewl_widget_hide(tree->header);
        else
                ewl_widget_show(tree->header);

        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the header visiblity from
 * @return Returns the current header visiblity of the tree
 * @brief Retrieve if the header is visible in the tree
 */
unsigned int
ewl_tree_headers_visible_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, FALSE);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, FALSE);

        DRETURN_INT(tree->headers_visible, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to toggle the alternating row colour
 * @param alternate: The boolean for alternating the row colour (TRUE == yes, FALSE == no)
 * @return Returns no value
 * @brief Toggle if the rows alternate in colour
 */
void
ewl_tree_alternate_row_colors_set(Ewl_Tree *tree, unsigned char alternate)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->row_color_alternate == !!alternate)
                DRETURN(DLEVEL_STABLE);

        tree->row_color_alternate = !!alternate;
        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns if the row colours are currently being alternated
 * @brief Retrieve if the row colours are being alternated
 */
unsigned int
ewl_tree_alternate_row_colors_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, FALSE);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, FALSE);

        DRETURN_INT(tree->row_color_alternate, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param count: The number of columns in the tree
 * @return Returns no value
 * @brief Sets the number of columns in the tree
 */
void
ewl_tree_column_count_set(Ewl_Tree *tree, unsigned int count)
{
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->columns == count)
                DRETURN(DLEVEL_STABLE);
        
        tree->columns = count;
        ewl_container_reset(EWL_CONTAINER(tree->header));

        for (i = 0; i < tree->columns; i++) {
                Ewl_Widget *h;

                h = ewl_hbox_new();
                ewl_container_child_append(EWL_CONTAINER(tree->header), h);
                ewl_widget_appearance_set(h, "header");
                ewl_widget_show(h);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param col: The number of the column to change the fixed size flag
 * @param fixed: The fixed size flag to set
 * @return Returns no value
 * @brief Set the fixed size flag of the give column
 */
void
ewl_tree_column_fixed_size_set(Ewl_Tree *tree, unsigned int col, unsigned int fixed)
{
        Ewl_Widget *box;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (col >= tree->columns)
        {
                DWARNING("parameter col is out of bounds (%i >= %i)",
                                col, tree->columns);
                DRETURN(DLEVEL_STABLE);
        }

        box = ewl_container_child_get(EWL_CONTAINER(tree->header), col);        
        ewl_paned_fixed_size_set(EWL_PANED(tree->header), box, fixed);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param col: the column to get the fixed size flag
 * @return Retrieve fixed size flag of the given column
 */
unsigned int
ewl_tree_column_fixed_size_get(Ewl_Tree *tree, unsigned int col)
{
        unsigned int ret = 0;
        Ewl_Widget *box;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, 0);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, 0);

        if (col >= tree->columns)
        {
                DWARNING("parameter col is out of bounds (%i >= %i)",
                                col, tree->columns);
                DRETURN_INT(ret, DLEVEL_STABLE);
        }

        box = ewl_container_child_get(EWL_CONTAINER(tree->header), col);        
        ret = ewl_paned_fixed_size_get(EWL_PANED(tree->header), box);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param col: The number of the column to change the initial size
 * @param size: The initial column size to set
 * @return Returns no value
 * @brief Set the initial size of the give column
 */
void
ewl_tree_column_initial_size_set(Ewl_Tree *tree, unsigned int col, int size)
{
        Ewl_Widget *box;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (col >= tree->columns)
        {
                DWARNING("parameter col is out of bounds (%i >= %i)",
                                col, tree->columns);
                DRETURN(DLEVEL_STABLE);
        }

        box = ewl_container_child_get(EWL_CONTAINER(tree->header), col);        
        ewl_paned_initial_size_set(EWL_PANED(tree->header), box, size);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param col: the column to get the initial size
 * @return Retrieve initial size of the given column
 */
int
ewl_tree_column_initial_size_get(Ewl_Tree *tree, unsigned int col)
{
        int ret = 0;
        Ewl_Widget *box;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, 0);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, 0);

        if (col >= tree->columns)
        {
                DWARNING("parameter col is out of bounds (%i >= %i)",
                                col, tree->columns);
                DRETURN_INT(ret, DLEVEL_STABLE);
        }

        box = ewl_container_child_get(EWL_CONTAINER(tree->header), col);        
        ret = ewl_paned_initial_size_get(EWL_PANED(tree->header), box);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the number of columns in the tree
 * @brief Retrives the number of columns in the tree
 */
unsigned int
ewl_tree_column_count_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, 0);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, 0);

        DRETURN_INT(tree->columns, DLEVEL_STABLE);
}

/**
 * @param tree: the tree to work with
 * @param view: The view to set to generate the content area
 * @return Returns no value
 * @brief Sets the view to use to generate the content area
 */
void
ewl_tree_content_view_set(Ewl_Tree *tree, const Ewl_View *view)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_PARAM_PTR(view);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->content_view == view)
                DRETURN(DLEVEL_STABLE);

        tree->content_view = view;

        /* destroy the old view, create a new one and redisplay the tree */
        if (tree->rows) ewl_widget_destroy(tree->rows);

        tree->rows = view->fetch(NULL, 0, 0, NULL);
        ewl_tree_view_tree_set(EWL_TREE_VIEW(tree->rows), tree);
        ewl_container_child_append(EWL_CONTAINER(tree), tree->rows);
        ewl_widget_show(tree->rows);

        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the view used to generate the content area
 * @brief Retrives the view used to generate the tree content area
 */
Ewl_View *
ewl_tree_content_view_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, NULL);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, NULL);

        DRETURN_PTR(tree->content_view, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the mode from
 * @return Returns the current Ewl_Tree_Selection_Type of the tree
 * @brief Get the selection type from the tree
 */
Ewl_Tree_Selection_Type
ewl_tree_selection_type_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, EWL_TREE_SELECTION_TYPE_CELL);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE,
                                        EWL_TREE_SELECTION_TYPE_CELL);

        DRETURN_INT(tree->type, DLEVEL_STABLE);
}

/**
 * @param tree: The Ewl_Tree to set the mode into
 * @param type: The Ewl_Tree_Selection_Mode to set into the tree
 * @return Returns no value.
 * @brief Set the mode of the tree
 */
void
ewl_tree_selection_type_set(Ewl_Tree *tree, Ewl_Tree_Selection_Type type)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->type == type)
                DRETURN(DLEVEL_STABLE);

        tree->type = type;

        /* if we switched types then the current set of selections isn't
         * valid anymore so we clear them out */
        ewl_mvc_selected_clear(EWL_MVC(tree));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the fixed row flag into
 * @param fixed: The fixed row flag to set into the tree
 * @return Returns no value.
 * @brief Set the fixed row size of the tree
 */
void
ewl_tree_fixed_rows_set(Ewl_Tree *tree, unsigned int fixed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->fixed == !!fixed)
                DRETURN(DLEVEL_STABLE);

        tree->fixed = !!fixed;
        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to get the fixed row flag from
 * @return Returns the current fixed row flag of the tree
 * @brief Retrieve the fixed row size of the tree
 */
unsigned int
ewl_tree_fixed_rows_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, FALSE);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, FALSE);

        DRETURN_INT(tree->fixed, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the widget that contains the tree rows
 * @brief Retrieves the widget containing the tree rows
 */
Ewl_Widget *
ewl_tree_content_widget_get(Ewl_Tree *tree)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, NULL);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, NULL);

        DRETURN_PTR(tree->rows, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to set the expansion into
 * @param data: The data that contains the expansion
 * @param row: The row to expand
 * @return Returns no value
 * @brief When the tree displays the data in @a data it will expand the give
 * @a row. This @a data is the parent of the expansion row.
 */
void
ewl_tree_row_expand(Ewl_Tree *tree, void *data, unsigned int row)
{
        Ewl_Tree_Expansions_List *exp;
        Ewl_Widget *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        /* nothing to do if already expanded */
        if (ewl_tree_row_expanded_is(tree, data, row)) DRETURN(DLEVEL_STABLE);

        if (!tree->expansions)
                ewl_tree_expansions_hash_create(tree);

        exp = ecore_hash_get(tree->expansions, data);
        if (!exp)
        {
                DWARNING("We did not find a expansion list. This should not"
                                " happen, ever.");
                DRETURN(DLEVEL_STABLE);
        }

        exp->expanded = realloc(exp->expanded,
                        (++exp->size) * sizeof(unsigned int));
        exp->expanded[exp->size - 1] = row;

        node = ewl_container_child_get(exp->c, row);
        ewl_tree_node_expand(EWL_TREE_NODE(node));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to collapse the row of
 * @param data: The data that contains the collapsed row
 * @param row: The row to collapse
 * @return Returns no value
 * @brief Sets the given @a row to collapsed for the given @a data in @a
 * tree
 */
void
ewl_tree_row_collapse(Ewl_Tree *tree, void *data, unsigned int row)
{
        Ewl_Tree_Expansions_List *exp;
        Ewl_Widget *node;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);


        /* if this tree has no expansions we're done */
        if (!tree->expansions) DRETURN(DLEVEL_STABLE);

        exp = ecore_hash_get(tree->expansions, data);

        /* if no expansion points for this data we're done */
        if (!exp) DRETURN(DLEVEL_STABLE);

        /* nothing to do if the row isn't expanded */
        if (!ewl_tree_row_expanded_is(tree, data, row)) DRETURN(DLEVEL_STABLE);

        /* we found the item so we can remove it */
        for (i = 0; exp->expanded[i] != row; i++)
                ;

        memmove(exp->expanded + i, exp->expanded + i + 1,
                        sizeof(unsigned int) * (exp->size - i - 1));
        exp->size--;
        exp->expanded = realloc(exp->expanded,
                                sizeof(unsigned int) * exp->size);

        node = ewl_container_child_get(exp->c, row);
        ewl_tree_node_collapse(EWL_TREE_NODE(node));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param data: The set of data to work with
 * @param row: The row to check
 * @return Returns TRUE if the given row is expanded, FALSE otherwise
 * @brief Checks if @a row is expanded in @a data of @a tree
 */
unsigned int
ewl_tree_row_expanded_is(Ewl_Tree *tree, void *data, unsigned int row)
{
        Ewl_Tree_Expansions_List *exp;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, FALSE);
        DCHECK_PARAM_PTR_RET(data, FALSE);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, FALSE);

        /* no expansions in the tree */
        if (!tree->expansions) DRETURN_INT(FALSE, DLEVEL_STABLE);

        exp = ecore_hash_get(tree->expansions, data);

        /* no expansions in this data we're done */
        if (!exp) DRETURN_INT(FALSE, DLEVEL_STABLE);

        /* search for this row in the expansions */
        for (i = 0; i < exp->size && exp->expanded[i] != row; i++)
                ;

        DRETURN_INT(exp->expanded && exp->expanded[i] == row, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_tree_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Tree *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TREE_TYPE);

        t = EWL_TREE(w);
        IF_FREE_HASH(t->expansions);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_tree_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Tree *tree;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TREE_TYPE);

        tree = EWL_TREE(w);

        if (!tree->headers_visible)
        {
                /* Since the header is invisible we need to tell it its
                 * position and its size to arrange correctly */
                ewl_object_x_request(EWL_OBJECT(tree->header), CURRENT_X(w));
                ewl_object_w_request(EWL_OBJECT(tree->header), CURRENT_W(w));
                ewl_paned_arrange(EWL_PANED(tree->header), tree->columns);
        }

        /* if the tree isn't dirty we're done */
        if (!ewl_mvc_dirty_get(EWL_MVC(tree)))
                DRETURN(DLEVEL_STABLE);

        ewl_tree_build_tree(tree);
        ewl_tree_cb_selected_change(EWL_MVC(tree));
        ewl_mvc_dirty_set(EWL_MVC(tree), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The header that was clicked
 * @param ev: UNUSED
 * @param data: The column related to this header
 * @return Returns no value
 * @brief Sorts the tree by the given column
 */
void
ewl_tree_cb_column_sort(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Tree *tree;
        const Ewl_Model *model;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* XXX This is kind of a nasty way to get the tree ... */
        tree = EWL_TREE(w->parent->parent);
        model = ewl_mvc_model_get(EWL_MVC(tree));

        /* sanity check */
        if (!model || !model->sort)
        {
                DWARNING("In ewl_tree_cb_column_sort without a sort cb.");
                DRETURN(DLEVEL_STABLE);
        }

        /* update our sort information and call the sort function, skipping
         * over SORT_NONE */
        tree->sort.column = INTPTR_TO_INT(data);
        tree->sort.direction = ((tree->sort.direction + 1) % EWL_SORT_DIRECTION_MAX);
        if (!tree->sort.direction) tree->sort.direction ++;

        model->sort(ewl_mvc_data_get(EWL_MVC(tree)), tree->sort.column,
                                        tree->sort.direction);
        ewl_mvc_dirty_set(EWL_MVC(tree), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_header_build(Ewl_Tree *tree, Ewl_Container *box,  
                        const Ewl_Model *model, const Ewl_View *view,
                        void *mvc_data, unsigned int column)
{
        Ewl_Widget *c;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_PARAM_PTR(model);
        DCHECK_PARAM_PTR(view);
        DCHECK_PARAM_PTR(box);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);
        DCHECK_TYPE(box, EWL_CONTAINER_TYPE);

        if (!tree->headers_visible) DRETURN(DLEVEL_STABLE);

        if (!view->header_fetch)
        {
                DWARNING("Missing header_fetch callback.");
                DRETURN(DLEVEL_STABLE);
        }

        if (model->header)
                c = view->header_fetch(model->header(mvc_data, column),
                                        column, EWL_MVC(tree)->private_data);
        else
                c = view->header_fetch(NULL, column, 
                                        EWL_MVC(tree)->private_data);

        /* XXX is this really a good idea to override the user's flags ? */
        ewl_object_fill_policy_set(EWL_OBJECT(c),
                        EWL_FLAG_FILL_HSHRINKABLE | EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(box, c);
        ewl_widget_show(c);

        /* display the sort arrow if needed */
        if (model->sortable && model->sortable(mvc_data, column))
        {
                const char *state_str;

                ewl_callback_append(EWL_WIDGET(box), EWL_CALLBACK_CLICKED,
                                        ewl_tree_cb_column_sort,
                                        INT_TO_INTPTR(column));

                c = ewl_button_new();
                ewl_container_child_append(box, c);

                if ((column == tree->sort.column)
                                && (tree->sort.direction == EWL_SORT_DIRECTION_ASCENDING))
                        state_str = "ascending";
                else if ((column == tree->sort.column)
                                && (tree->sort.direction == EWL_SORT_DIRECTION_DESCENDING))
                        state_str = "descending";
                else
                        state_str = "default";

                ewl_widget_state_set(c, state_str, EWL_STATE_PERSISTENT);
                ewl_object_fill_policy_set(EWL_OBJECT(c), EWL_FLAG_FILL_SHRINKABLE);
                ewl_object_alignment_set(EWL_OBJECT(c), EWL_FLAG_ALIGN_RIGHT);
                ewl_widget_show(c);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_column_build(Ewl_Row *row, const Ewl_Model *model, 
                                const Ewl_View *view,
                                void *mvc_data, unsigned int r,
                                unsigned int c, void *pr_data, Ewl_Widget *node)
{
        Ewl_Widget *cell;
        Ewl_Widget *child;
        void *val;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(row);
        DCHECK_PARAM_PTR(model);
        DCHECK_PARAM_PTR(view);
        DCHECK_TYPE(row, EWL_ROW_TYPE);

        cell = ewl_cell_new();
        ewl_cell_state_change_cb_add(EWL_CELL(cell));
        ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_ALL);
        ewl_container_child_append(EWL_CONTAINER(row), cell);
        ewl_callback_append(cell, EWL_CALLBACK_CLICKED,
                                ewl_tree_cb_cell_clicked, node);
        ewl_widget_show(cell);

        val = model->fetch(mvc_data, r, c);
        if (!val)
        {
                child = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(child), " ");
        }
        else
                child = view->fetch(val, r, c, pr_data);

        ewl_container_child_append(EWL_CONTAINER(cell), child);
        ewl_widget_show(child);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_build_tree(Ewl_Tree *tree)
{
        void *mvc_data;
        const Ewl_Model *model;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        mvc_data = ewl_mvc_data_get(EWL_MVC(tree));
        model = ewl_mvc_model_get(EWL_MVC(tree));
        if (!model) DRETURN(DLEVEL_STABLE);

        /* setup the headers */
        ewl_tree_headers_build(tree, model, mvc_data);

        /* setup the content */
        ewl_container_reset(EWL_CONTAINER(tree->rows));
        ewl_tree_build_tree_rows(tree, model,
                                ewl_mvc_view_get(EWL_MVC(tree)), mvc_data,
                                0, EWL_CONTAINER(tree->rows), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_headers_build(Ewl_Tree *tree, const Ewl_Model *model, void *mvc_data)
{
        Ewl_Container *header;
        unsigned int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        header = EWL_CONTAINER(tree->header);

        ewl_container_child_iterate_begin(header);
        for (i = 0; i < tree->columns; i++)
        {
                Ewl_Widget *h = ewl_container_child_next(header);

                ewl_container_reset(EWL_CONTAINER(h));
                /* remove it here since the column may be not sortable
                 * anymore */
                ewl_callback_del(h, EWL_CALLBACK_CLICKED,
                                        ewl_tree_cb_column_sort);
                ewl_tree_header_build(tree, EWL_CONTAINER(h), model,
                                ewl_mvc_view_get(EWL_MVC(tree)),
                                mvc_data, i);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_build_tree_rows(Ewl_Tree *tree, const Ewl_Model *model, 
                                const Ewl_View *view, void *data, int colour, 
                                Ewl_Container *parent, int hidden)
{
        unsigned int i = 0, row_count = 0;
        unsigned int column;
        void *pr_data;

        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);
        DCHECK_PARAM_PTR(parent);
        DCHECK_TYPE(parent, EWL_CONTAINER_TYPE);

        pr_data = ewl_mvc_private_data_get(EWL_MVC(tree));
        row_count = model->count(data);
        if (row_count == 0) DRETURN(DLEVEL_STABLE);

        while (TRUE)
        {
                Ewl_Widget *row, *node;

                node = ewl_tree_node_new();
                EWL_TREE_NODE(node)->tree = EWL_WIDGET(tree);
                EWL_TREE_NODE(node)->row_num = i;
                ewl_mvc_model_set(EWL_MVC(node), model);
                ewl_mvc_data_set(EWL_MVC(node), data);
                ewl_mvc_view_set(EWL_MVC(node), view);

                ewl_container_child_append(parent, node);
                if (!hidden) ewl_widget_show(node);

                row = ewl_row_new();
                ewl_row_header_set(EWL_ROW(row), EWL_CONTAINER(tree->header));
                ewl_tree_node_row_set(EWL_TREE_NODE(node), EWL_ROW(row));
                ewl_container_child_append(EWL_CONTAINER(node), row);
                ewl_callback_append(row, EWL_CALLBACK_CLICKED,
                                        ewl_tree_cb_row_clicked, node);
                ewl_widget_show(row);

                if (!model->highlight || model->highlight(data, i))
                {
                        ewl_callback_append(row, EWL_CALLBACK_MOUSE_IN,
                                        ewl_tree_cb_row_highlight, NULL);
                        ewl_callback_append(row, EWL_CALLBACK_MOUSE_OUT,
                                        ewl_tree_cb_row_unhighlight, NULL);
                }

                if (tree->row_color_alternate)
                {
                        if (colour)
                                ewl_widget_state_set(row, "odd",
                                                        EWL_STATE_PERSISTENT);
                        else
                                ewl_widget_state_set(row, "even",
                                                        EWL_STATE_PERSISTENT);
                }

                colour = (colour + 1) % 2;

                /* do the current branch */
                for (column = 0; column < tree->columns; column++)
                        ewl_tree_column_build(EWL_ROW(row), model, view,
                                                data, i, column, pr_data, node);

                /* check if this is an expansion point */
                if (model->expansion.is && model->expansion.is(data, i))
                {
                        int hidden = TRUE;

                        if (!model->expansion.data)
                        {
                                DWARNING("In ewl_tree_build_tree_rows, "
                                        "model expandable but without "
                                        "expansion_data_fetch cb.");
                                DRETURN(DLEVEL_STABLE);
                        }

                        ewl_tree_node_expandable_set(EWL_TREE_NODE(node), TRUE);

                        if (model->expansion.data &&
                                        ewl_tree_row_expanded_is(tree, data, i))
                        {
                                ewl_tree_node_expand(EWL_TREE_NODE(node));
                                hidden = FALSE;
                        }
                }
                else
                        ewl_tree_node_expandable_set(EWL_TREE_NODE(node), FALSE);

                i++;

                /*
                 * Finished the rows at this level? Jump back up a level.
                 */
                if (i >= row_count) break;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_header_changed(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Tree *tree;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_TREE_TYPE);

        tree = data;
        ewl_widget_configure(EWL_WIDGET(tree->rows));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_row_clicked(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                void *data)
{
        Ewl_Tree *tree;
        Ewl_Tree_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_TREE_NODE_TYPE);

        node = data;
        tree = EWL_TREE(node->tree);
        if (tree->type != EWL_TREE_SELECTION_TYPE_ROW)
                DRETURN(DLEVEL_STABLE);

        ewl_mvc_handle_click(EWL_MVC(tree), ewl_mvc_model_get(EWL_MVC(node)),
                                ewl_mvc_data_get(EWL_MVC(node)),
                                node->row_num, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_row_highlight(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ROW_TYPE);

        ewl_widget_state_set(w, "highlight,on", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_row_unhighlight(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ROW_TYPE);

        ewl_widget_state_set(w, "highlight,off", EWL_STATE_TRANSIENT);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_cell_clicked(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Row *row;
        Ewl_Tree *tree;
        Ewl_Tree_Node *node;
        int column;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_TREE_NODE_TYPE);

        row = EWL_ROW(w->parent);
        node = EWL_TREE_NODE(data);
        tree = EWL_TREE(node->tree);
        if (tree->type != EWL_TREE_SELECTION_TYPE_CELL)
                DRETURN(DLEVEL_STABLE);

        column = ewl_container_child_index_get(EWL_CONTAINER(row), w);
        if (column < 0) DRETURN(DLEVEL_STABLE);

        ewl_mvc_handle_click(EWL_MVC(node->tree),
                        ewl_mvc_model_get(EWL_MVC(node)),
                        ewl_mvc_data_get(EWL_MVC(node)),
                        node->row_num, column);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_tree_cb_selected_change(Ewl_MVC *mvc)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(mvc);
        DCHECK_TYPE(mvc, EWL_MVC_TYPE);

        ewl_mvc_highlight(mvc, EWL_CONTAINER(EWL_TREE(mvc)->rows),
                                                ewl_tree_widget_at);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_tree_widget_at(Ewl_MVC *mvc, void *data, unsigned int row,
                        unsigned int column)
{
        Ewl_Widget *r, *w;
        Ewl_Container *c;
        Ewl_Tree *tree;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(mvc, NULL);
        DCHECK_PARAM_PTR_RET(data, NULL);
        DCHECK_TYPE_RET(mvc, EWL_TREE_TYPE, NULL);

        tree = EWL_TREE(mvc);

        /* get the container that holds the wished row */
        if (ewl_mvc_data_get(mvc) == data)
                c = EWL_CONTAINER(tree->rows);
        else
        {
                Ewl_Tree_Expansions_List *exp;

                exp = ecore_hash_get(tree->expansions, data);
                if (!exp)
                        DRETURN_PTR(NULL, DLEVEL_STABLE);

                c = exp->c;
        }

        /* find the row in the container */
        r = ewl_container_child_get(c, row);
        r = EWL_WIDGET(EWL_TREE_NODE(r)->row);

        if (tree->type == EWL_TREE_SELECTION_TYPE_ROW)
                w = r;
        else
        {
                /* infact our row is a node so we have to get the row
                 * to search for the right container */
                w = ewl_container_child_get(EWL_CONTAINER(r), column);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

static void
ewl_tree_expansions_hash_create(Ewl_Tree *tree)
{
        Ewl_Tree_Expansions_List *exp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (tree->expansions)
                DRETURN(DLEVEL_STABLE);

        tree->expansions = ecore_hash_new(NULL, NULL);
        ecore_hash_free_value_cb_set(tree->expansions,
                        ECORE_FREE_CB(ewl_tree_expansions_list_destroy));

        exp = ewl_tree_expansions_list_new();
        exp->c = EWL_CONTAINER(tree->rows);
        ecore_hash_set(tree->expansions, ewl_mvc_data_get(EWL_MVC(tree)), exp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Tree_Expansions_List *
ewl_tree_expansions_list_new(void)
{
        Ewl_Tree_Expansions_List *el;

        DENTER_FUNCTION(DLEVEL_STABLE);

        el = NEW(Ewl_Tree_Expansions_List, 1);
        if (!el)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        DRETURN_PTR(el, DLEVEL_STABLE);
}

static void
ewl_tree_expansions_list_destroy(Ewl_Tree_Expansions_List *el)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(el);

        IF_FREE(el->expanded);
        FREE(el);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Ewl_Tree_Node Stuff
 */
Ewl_Widget *
ewl_tree_node_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Tree_Node, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_tree_node_init(EWL_TREE_NODE(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

int
ewl_tree_node_init(Ewl_Tree_Node *node)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(node, FALSE);

        if (!ewl_mvc_init(EWL_MVC(node)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(node), EWL_TREE_NODE_TYPE);
        ewl_widget_inherit(EWL_WIDGET(node), EWL_TREE_NODE_TYPE);

        ewl_container_show_notify_set(EWL_CONTAINER(node),
                                ewl_tree_cb_node_child_show);
        ewl_container_hide_notify_set(EWL_CONTAINER(node),
                                ewl_tree_cb_node_child_hide);
        ewl_container_resize_notify_set(EWL_CONTAINER(node),
                                ewl_tree_cb_node_resize);
        ewl_container_add_notify_set(EWL_CONTAINER(node),
                                ewl_tree_cb_node_child_add);
        ewl_container_remove_notify_set(EWL_CONTAINER(node),
                                ewl_tree_cb_node_child_del);

        /* we don't want the mvc to unref our data since the data is owned
         * by the tree or the parent node */
        ewl_callback_del(EWL_WIDGET(node), EWL_CALLBACK_DESTROY,
                                                ewl_mvc_cb_data_unref);

        ewl_object_fill_policy_set(EWL_OBJECT(node),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINKABLE);

        ewl_callback_del(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
                                                        ewl_box_cb_configure);
        ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_CONFIGURE,
                                        ewl_tree_cb_node_configure, NULL);

        node->expanded = EWL_TREE_NODE_COLLAPSED;
        ewl_widget_focusable_set(EWL_WIDGET(node), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_tree_node_expandable_set(Ewl_Tree_Node *node, unsigned int expandable)
{
        const Ewl_Model *model;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(node);

        model = ewl_mvc_model_get(EWL_MVC(node));
        /* we only create the handle if it doesn't exist and the model has
         * a function to query for expandability */
        if (!node->handle && model->expansion.is)
        {
                node->handle = ewl_expansion_new();
                ewl_container_child_prepend(EWL_CONTAINER(node), node->handle);
                ewl_widget_internal_set(EWL_WIDGET(node->handle), TRUE);
                ewl_widget_show(node->handle);
        }

        if (!node->handle)
                DRETURN(DLEVEL_STABLE);

        if (expandable)
        {
                ewl_callback_append(node->handle, EWL_CALLBACK_VALUE_CHANGED,
                                                ewl_tree_cb_node_toggle, node);
                ewl_callback_append(EWL_WIDGET(node), EWL_CALLBACK_DESTROY,
                                        ewl_tree_cb_node_data_unref, NULL);
                ewl_widget_enable(node->handle);
                ewl_expansion_expandable_set(EWL_EXPANSION(node->handle), TRUE);
        }
        else
        {
                ewl_callback_del(node->handle, EWL_CALLBACK_VALUE_CHANGED,
                                                ewl_tree_cb_node_toggle);
                ewl_callback_del(EWL_WIDGET(node), EWL_CALLBACK_DESTROY,
                                                ewl_tree_cb_node_data_unref);
                ewl_widget_disable(node->handle);
                ewl_expansion_expandable_set(EWL_EXPANSION(node->handle), FALSE);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_tree_node_expandable_get(Ewl_Tree_Node *node)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(node, FALSE);
        DCHECK_TYPE_RET(node, EWL_TREE_NODE_TYPE, FALSE);

        DRETURN_INT((node->handle) &&
                        ewl_expansion_is_expandable(EWL_EXPANSION(node->handle)), DLEVEL_STABLE);
}

void
ewl_tree_node_expand(Ewl_Tree_Node *node)
{
        Ewl_Widget *child;
        Ecore_List *tmp;
        const Ewl_Model *model;
        void *data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(node);
        DCHECK_TYPE(node, EWL_TREE_NODE_TYPE);

        if (node->expanded == EWL_TREE_NODE_EXPANDED)
                DRETURN(DLEVEL_STABLE);

        /*
         * Queue the parent tree for configure, this handles the issue of
         * redrawing the alternating colors on expand and doing it early
         * avoids duplicate or long list walks for queueing child widgets.
         */
        ewl_widget_configure(node->tree);

        /* This is needed as in the child_show cb we will case a list walk.
         * We need this till the lists get iterators */
        tmp = ecore_list_new();

        ecore_dlist_first_goto(EWL_CONTAINER(node)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(node)->children)))
        {
                if ((child != node->handle) && (child != EWL_WIDGET(node->row))
                                && !UNMANAGED(child))
                        ecore_list_append(tmp, child);
        }

        while ((child = ecore_list_first_remove(tmp)))
                ewl_widget_show(child);

        IF_FREE_LIST(tmp);

        model = ewl_mvc_model_get(EWL_MVC(node));
        data = ewl_mvc_data_get(EWL_MVC(node));
        if (model->expansion.data && !node->built_children)
        {
                const Ewl_View *view, *tmp_view = NULL;
                const Ewl_Model *tmp_model;

                if (!node->expansion.data)
                {
                        Ewl_Tree_Expansions_List *exp;

                        node->expansion.data =
                                model->expansion.data(data, node->row_num);

                        /* Put the node into the expansions hash */
                        exp = ewl_tree_expansions_list_new();
                        exp->c = EWL_CONTAINER(node);
                        ecore_hash_set(EWL_TREE(node->tree)->expansions,
                                        node->expansion.data, exp);
                }

                if (!node->expansion.model && model->expansion.model)
                        node->expansion.model =
                                model->expansion.model(data, node->row_num);

                /* We only save the model reference here to unref it on destroy.
                 * We don't need to save the parent model to unref */
                if (!node->expansion.model) tmp_model = model;
                else tmp_model = node->expansion.model;

                view = ewl_mvc_view_get(EWL_MVC(node));
                if (view->expansion)
                        tmp_view = view->expansion(data, node->row_num);

                if (!tmp_view) tmp_view = view;

                ewl_tree_build_tree_rows(EWL_TREE(node->tree), tmp_model,
                                                tmp_view, node->expansion.data,
                                                0, EWL_CONTAINER(node), FALSE);
                node->built_children = TRUE;
        }

        node->expanded = EWL_TREE_NODE_EXPANDED;
        ewl_check_checked_set(EWL_CHECK(node->handle), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_node_collapse(Ewl_Tree_Node *node)
{
        Ewl_Widget *child;
        Ecore_List *tmp;

        DENTER_FUNCTION(DLEVEL_STABLE);

        if (node->expanded == EWL_TREE_NODE_COLLAPSED)
                DRETURN(DLEVEL_STABLE);

        /*
         * Queue the parent tree for configure, this handles the issue of
         * redrawing the alternating colors on expand and doing it early
         * avoids duplicate or long list walks for queueing child widgets.
         */
        ewl_widget_configure(node->tree);

        /* This is needed becase in our child_hide callback we will cause a
         * list iteration. Until we get iterators we need this */
        tmp = ecore_list_new();

        ecore_dlist_first_goto(EWL_CONTAINER(node)->children);
        while ((child = ecore_dlist_next(EWL_CONTAINER(node)->children)))
        {
                if ((child != node->handle) && (child != EWL_WIDGET(node->row))
                                && !UNMANAGED(child))
                        ecore_list_append(tmp, child);
        }

        while ((child = ecore_list_first_remove(tmp)))
                ewl_widget_hide(child);

        IF_FREE_LIST(tmp);

        node->expanded = EWL_TREE_NODE_COLLAPSED;
        ewl_check_checked_set(EWL_CHECK(node->handle), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_tree_node_expanded_is(Ewl_Tree_Node *node)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(node, FALSE);

        DRETURN_INT((node->expanded == EWL_TREE_NODE_EXPANDED),        DLEVEL_STABLE);
}

void
ewl_tree_node_row_set(Ewl_Tree_Node *node, Ewl_Row *row)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(node);
        DCHECK_PARAM_PTR(row);
        DCHECK_TYPE(node, EWL_TREE_NODE_TYPE);
        DCHECK_TYPE(row, EWL_ROW_TYPE);

        node->row = row;
        ewl_widget_internal_set(EWL_WIDGET(row), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Tree_Node *node;
        Ewl_Container *c;
        Ewl_Object *child;
        int x, y, hw = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TREE_NODE_TYPE);

        node = EWL_TREE_NODE(w);
        if (!node->tree) DRETURN(DLEVEL_STABLE);

        c = EWL_CONTAINER(w);
        if (!c->children) DRETURN(DLEVEL_STABLE);

        x = CURRENT_X(w);
        y = CURRENT_Y(w);

        if (node->handle)
        {
                ewl_object_geometry_request(EWL_OBJECT(node->handle),
                                CURRENT_X(w), CURRENT_Y(w), CURRENT_W(w),
                                CURRENT_H(w));
                hw = ewl_object_current_w_get(EWL_OBJECT(node->handle));
                x += hw;
        }

        /*
         * All subsequent children are lower nodes and rows.
         */
        ecore_dlist_first_goto(c->children);
        while ((child = ecore_dlist_next(c->children)))
        {
                if (VISIBLE(child) && EWL_WIDGET(child) != node->handle
                                && !UNMANAGED(child))
                {
                        ewl_object_geometry_request(child, x, y, CURRENT_W(w) - hw,
                                                    ewl_object_preferred_h_get(child));
                        y += ewl_object_current_h_get(child);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_data_unref(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Tree_Node *node;
        const Ewl_Model *model;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TREE_NODE_TYPE);

        node = EWL_TREE_NODE(w);

        if (!node->expansion.data)
                DRETURN(DLEVEL_STABLE);

        if (node->expansion.model)
                model = node->expansion.model;
        else
                model = ewl_mvc_model_get(EWL_MVC(w));

        if (model->unref)
                model->unref(node->expansion.data);

        node->expansion.data = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_toggle(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                                        void *data)
{
        Ewl_Tree_Node *node;
        void *parent_data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_TREE_NODE_TYPE);

        node = EWL_TREE_NODE(data);
        parent_data = ewl_mvc_data_get(EWL_MVC(node));

        if (!ewl_tree_node_expandable_get(node))
                DRETURN(DLEVEL_STABLE);

        if (ewl_tree_node_expanded_is(node))
                ewl_tree_row_collapse(EWL_TREE(node->tree), parent_data,
                                node->row_num);
        else
                ewl_tree_row_expand(EWL_TREE(node->tree), parent_data,
                                node->row_num);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_child_show(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
        Ewl_Tree_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_TREE_NODE_TYPE);

        node = EWL_TREE_NODE(c);
        if (node->handle && node->expanded) {
                ewl_container_sum_prefer(c, EWL_ORIENTATION_VERTICAL);
                if (REALIZED(node->handle) && VISIBLE(node->handle))
                        ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
                                        PREFERRED_H(c) -
                                        ewl_object_preferred_h_get(EWL_OBJECT(node->handle)));
        }
        else
                ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
                                           ewl_object_preferred_h_get(EWL_OBJECT(node->row)));

        ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);
        if (node->handle && REALIZED(node->handle) && VISIBLE(node->handle))
                ewl_object_preferred_inner_w_set(EWL_OBJECT(c), PREFERRED_W(c) +
                        ewl_object_preferred_w_get(EWL_OBJECT(node->handle)));

        if (!node->expanded && node->handle)
                ewl_widget_hide(node->handle);

        ewl_widget_configure(node->tree);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_child_hide(Ewl_Container *c, Ewl_Widget *w)
{
        int width;
        Ewl_Tree_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_TREE_NODE_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        node = EWL_TREE_NODE(c);
        if (w == node->handle)
                DRETURN(DLEVEL_STABLE);

        if (ecore_dlist_count(c->children) < 3)
        {
                if (node->handle && VISIBLE(node->handle))
                        ewl_widget_hide(node->handle);
        }

        ewl_object_preferred_inner_h_set(EWL_OBJECT(c),
                PREFERRED_H(c) - ewl_object_preferred_h_get(EWL_OBJECT(w)));

        width = ewl_object_preferred_w_get(EWL_OBJECT(w));
        if (PREFERRED_W(c) >= width)
                ewl_container_largest_prefer(c, EWL_ORIENTATION_HORIZONTAL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_resize(Ewl_Container *c, Ewl_Widget *w, int size __UNUSED__,
                                                     Ewl_Orientation o __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        ewl_tree_cb_node_child_show(c, w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_child_add(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
        Ewl_Tree_Node *node;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_TREE_NODE_TYPE);

        node = EWL_TREE_NODE(c);

        if (ecore_dlist_count(c->children) > 2)
        {
                /* XXX what do we do if !node->handle? */
                if (node->handle && HIDDEN(node->handle))
                        ewl_widget_show(node->handle);
        }
        else if (node->handle && VISIBLE(node->handle))
                ewl_widget_hide(node->handle);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tree_cb_node_child_del(Ewl_Container *c, Ewl_Widget *w, int idx __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_CONTAINER_TYPE);

        ewl_tree_cb_node_child_add(c, w);
}

/**
 * @param tree: The tree to set kinetic scrolling for
 * @param type: The type of kinetic scrolling to use
 * @return Returns no value
 * @brief Sets up the tree to use kinetic scrolling
 */
void
ewl_tree_kinetic_scrolling_set(Ewl_Tree *tree, Ewl_Kinetic_Scroll type)
{
        Ewl_Scrollpane *scroll;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        if (!type)
                DRETURN(DLEVEL_STABLE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ewl_scrollpane_kinetic_scrolling_set(EWL_SCROLLPANE(scroll), type);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to use
 * @return Returns the type of kinetic scrolling used
 * @brief Gets the type of kinetic scrolling used
 */
Ewl_Kinetic_Scroll
ewl_tree_kinetic_scrolling_get(Ewl_Tree *tree)
{
        Ewl_Scrollpane *scroll;
        Ewl_Kinetic_Scroll type = EWL_KINETIC_SCROLL_NONE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, EWL_KINETIC_SCROLL_NONE);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, EWL_KINETIC_SCROLL_NONE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                type = ewl_scrollpane_kinetic_scrolling_get(scroll);

        DRETURN_INT(type, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param v: The maximum velocity
 * @return Returns no value
 * @brief Sets the maximum velocity for kinetic scrolling
 */
void
ewl_tree_kinetic_max_velocity_set(Ewl_Tree *tree, double v)
{
        Ewl_Scrollpane *scroll;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ewl_scrollpane_kinetic_max_velocity_set(scroll, v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the maximum velocity of kinetic scrolling
 * @brief Gets the maximum velocity for kinetic scrolling
 */
double
ewl_tree_kinetic_max_velocity_get(Ewl_Tree *tree)
{
        Ewl_Scrollpane *scroll;
        double ret = -1.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, -1);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, -1);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ret = ewl_scrollpane_kinetic_max_velocity_get(scroll);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param v: The minimum velocity
 * @return Returns no value
 * @brief Sets the minimum velocity for kinetic scrolling
 */
void
ewl_tree_kinetic_min_velocity_set(Ewl_Tree *tree, double v)
{
        Ewl_Scrollpane *scroll;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ewl_scrollpane_kinetic_min_velocity_set(scroll, v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the minimum velocity of kinetic scrolling
 * @brief Gets the minimum velocity for kinetic scrolling
 */
double
ewl_tree_kinetic_min_velocity_get(Ewl_Tree *tree)
{
        Ewl_Scrollpane *scroll;
        double ret = -1.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, -1);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, -1);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ret = ewl_scrollpane_kinetic_min_velocity_get(scroll);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param d: The multiplier to reduce velocity
 * @return Returns no value
 * @brief Sets the multiplier to reduce the velocity of kinetic scrolling
 */
void
ewl_tree_kinetic_dampen_set(Ewl_Tree *tree, double d)
{
        Ewl_Scrollpane *scroll;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ewl_scrollpane_kinetic_dampen_set(scroll, d);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the multiplier used to dampen in kinetic scrolling
 * @brief Gets the multiplier used to reduce the velocity of kinetic scrolling
 */
double
ewl_tree_kinetic_dampen_get(Ewl_Tree *tree)
{
        Ewl_Scrollpane *scroll;
        double ret = -1.0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, -1);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, -1);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ret = ewl_scrollpane_kinetic_dampen_get(scroll);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @param fps: The number of frames per second 
 * @return Returns no value
 * @brief Sets the number of times per second the tree scrolling is updated
 */
void
ewl_tree_kinetic_fps_set(Ewl_Tree *tree, int fps)
{
        Ewl_Scrollpane *scroll;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tree);
        DCHECK_TYPE(tree, EWL_TREE_TYPE);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ewl_scrollpane_kinetic_fps_set(scroll, fps);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param tree: The tree to work with
 * @return Returns the number of frames per second
 * @brief Gets the number of times per second the tree scrolling is updated
 */
int
ewl_tree_kinetic_fps_get(Ewl_Tree *tree)
{
        Ewl_Scrollpane *scroll;
        int ret = -1;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, -1);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, -1);

        scroll = ewl_tree_kinetic_scrollpane_get(tree);
        if (scroll)
                ret = ewl_scrollpane_kinetic_fps_get(scroll);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @internal
 * @param tree: The tree to work with
 * @return Returns the scrollpane used in the view
 * @brief A helper function for setting kinetic scrolling variables
 */
Ewl_Scrollpane *
ewl_tree_kinetic_scrollpane_get(Ewl_Tree *tree)
{
        Ewl_Widget *s;
        Ewl_Container *scroll, *temp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(tree, NULL);
        DCHECK_TYPE_RET(tree, EWL_TREE_TYPE, NULL);

        s = ewl_tree_content_widget_get(tree);
        scroll = ewl_container_redirect_get(EWL_CONTAINER(s));

        while (!ewl_widget_type_is(EWL_WIDGET(scroll), EWL_SCROLLPANE_TYPE))
        {
                temp = scroll;
                scroll = ewl_container_redirect_get(temp);

                if (!scroll)
                {
                        DWARNING("No scrollpane to use for kinetic scrolling");
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
                }
        }

        DRETURN_PTR(EWL_SCROLLPANE(scroll), DLEVEL_STABLE);
}

        
