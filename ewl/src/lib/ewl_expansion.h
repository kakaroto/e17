/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_EXPANSION_H
#define EWL_EXPANSION_H

/**
 * @addtogroup Ewl_Expansion Ewl_Expansion: The Expansion node for Tree
 * Defines a widget for tree's expansion node
 *
 * @remarks Inherits from Ewl_Check.
 * @if HAVE_IMAGES
 * @image html Ewl_Expansion_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /expansion/group
 */

/**
 * @def EWL_EXPANSION_TYPE
 * The type name for the Ewl_Expansion widget
 */
#define EWL_EXPANSION_TYPE "expansion"

/**
 * @def EWL_EXPANSION_IS(w)
 * Returns TRUE if the widget is an Ewl_Expansion, FALSE otherwise
 */
#define EWL_EXPANSION_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_EXPANSION_TYPE))

/**
 * A tree expansion handle.
 */
typedef struct Ewl_Expansion Ewl_Expansion;

/**
 * @def EWL_EXPANSION(c)
 * Typecasts a pointer to an Ewl_Expansion pointer.
 */
#define EWL_EXPANSION(c) ((Ewl_Expansion *)c)

/**
 * @brief Inherits from Ewl_Check and extends it to provide a expanded state.
 */
struct Ewl_Expansion
{
        Ewl_Check w; /**< Inherit from Ewl_Check */
        int expandable; /**< Indicates if this node is expandable or not */
};

Ewl_Widget      *ewl_expansion_new(void);
int              ewl_expansion_init(Ewl_Expansion *c);

int              ewl_expansion_is_expandable(Ewl_Expansion *c);
void             ewl_expansion_expandable_set(Ewl_Expansion *c, int expandable);
/*
 * Internally used callbacks.
 */
void ewl_expansion_cb_reveal(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

