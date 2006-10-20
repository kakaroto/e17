#ifndef EWL_TREE2_VIEW_SCROLLED_H
#define EWL_TREE2_VIEW_SCROLLED_H

/**
 * @addtogroup Ewl_Tree2_View_Scrolled Ewl_Tree2_View_Scrolled The scrolled tree view 
 * @brief A scrolled view for the tree
 * 
 * @{
 */

/**
 * @def EWL_TREE2_VIEW_SCROLLED_TYPE
 * The type name
 */
#define EWL_TREE2_VIEW_SCROLLED_TYPE "tree2_view_scrolled"

/**
 * Ewl_Tree2_View_Scrolled
 */
typedef struct Ewl_Tree2_View_Scrolled Ewl_Tree2_View_Scrolled;

/**
 * @def EWL_TREE2_VIEW_SCROLLED(tv)
 * Typecasts a pointer to an Ewl_Tree2_View_Scrolled pointer
 */
#define EWL_TREE2_VIEW_SCROLLED(tv) ((Ewl_Tree2_View_Scrolled*)tv)

/**
 * A scrolled view on the tree
 */
struct Ewl_Tree2_View_Scrolled
{
	Ewl_Box box;			/**< Inherit from Ewl_Box */
	Ewl_Widget *scroll;		/**< The scroll region */

	unsigned char scroll_header:1;	/**< Do we scroll headers */
	unsigned char scroll_visible:1;	/**< Do we we show the headers */
};

Ewl_View	*ewl_tree2_view_scrolled_get(void);

Ewl_Widget	*ewl_tree2_view_scrolled_new(void);
int		 ewl_tree2_view_scrolled_init(Ewl_Tree2_View_Scrolled *tv);

void		 ewl_tree2_view_scrolled_scroll_headers_set(Ewl_Tree2_View *view,
							unsigned int scroll);
unsigned int	 ewl_tree2_view_scrolled_scroll_headers_get(Ewl_Tree2_View *view);

/**
 * @}
 */

#endif


