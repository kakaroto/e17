/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_EXPANSION_H
#define EWL_EXPANSION_H

/**
 * @addtogroup Ewl_Expansion Ewl_Expansion: The Expansion node for Tree2
 * Defines a widget for tree2's expansion node 
 * 
 * @remarks Inherits from Ewl_Check.
 * @image html Ewl_Expansion_inheritance.png
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
 * A tree expansion handle.
 */
typedef struct Ewl_Expansion Ewl_Expansion;

/**
 * @def EWL_EXPANSION(c)
 * Typecasts a pointer to an Ewl_Expansion pointer.
 */
#define EWL_EXPANSION(c) ((Ewl_Expansion *)c)

/**
 * Inherits from Ewl_Widget and extends it to provide a expansioned state.
 */
struct Ewl_Expansion
{
	Ewl_Check w; /**< Inherit from Ewl_Check */
	int expandable; /**< Indicates if this node is expandable or not */
};

Ewl_Widget 	*ewl_expansion_new(void);
int 		 ewl_expansion_init(Ewl_Expansion *c);

int 		 ewl_expansion_is_expandable(Ewl_Expansion *c);
void 		 ewl_expansion_expandable_set(Ewl_Expansion *c, 
						int expandable);

int 		 ewl_expansion_is_expanded(Ewl_Expansion *c);
void 		 ewl_expansion_expanded_set(Ewl_Expansion *c, int expanded);

/*
 * Internally used callbacks.
 */
void ewl_expansion_cb_update_expandable(Ewl_Widget *w, void *ev_data, 
							void *user_data);
void ewl_expansion_cb_reveal(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif

