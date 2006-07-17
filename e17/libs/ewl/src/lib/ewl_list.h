#ifndef EWL_LIST_H
#define EWL_LIST_H

/**
 * @addtogroup Ewl_List Ewl_List: A list widget
 * @brief Defines a class to layout data in a list
 *
 * @{
 */

/**
 * @def EWL_LIST_TYPE
 * The type name for the Ewl_List widget
 */
#define EWL_LIST_TYPE "list"

/**
 * @def EWL_LIST(list)
 * Typecasts a pointer to an Ewl_List pointer
 */
#define EWL_LIST(list) ((Ewl_List *)list)

/**
 * The list strucutre 
 */
typedef struct Ewl_List Ewl_List;

/**
 * Inherits from Ewl_Box and extends to provide a list widget
 */
struct Ewl_List
{
	Ewl_Box box;		/**< The box parent */

	Ewl_Model *model;	/**< The model for the list */
	Ewl_View *view;		/**< The view for the list */
	void *data;		/**< The data for the list */

	Ewl_Widget *selected;	/**< The selected widget */
	unsigned char dirty:1;	/**< Has the model changed? */
};

Ewl_Widget 	*ewl_list_new(void);
int 		 ewl_list_init(Ewl_List *list);

void		 ewl_list_model_set(Ewl_List *list, Ewl_Model *model);
Ewl_Model	*ewl_list_model_get(Ewl_List *list);

void		 ewl_list_view_set(Ewl_List *list, Ewl_View *view);
Ewl_View	*ewl_list_view_get(Ewl_List *list);

void		 ewl_list_data_set(Ewl_List *list, void *data);
void		*ewl_list_data_get(Ewl_List *list);

void		 ewl_list_dirty_set(Ewl_List *list, unsigned int dirty);
unsigned int	 ewl_list_dirty_get(Ewl_List *list);

void		 ewl_list_selected_widget_set(Ewl_List *list, Ewl_Widget *w);
Ewl_Widget	*ewl_list_selected_widget_get(Ewl_List *list);

void		 ewl_list_selected_index_set(Ewl_List *list, int idx);
int		 ewl_list_selected_index_get(Ewl_List *list);

/*
 * Internal stuff.
 */
void ewl_list_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_list_cb_item_clicked(Ewl_Widget *w, void *ev, void *data);
void ewl_list_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_list_cb_child_del(Ewl_Container *c, Ewl_Widget *w, int idx);
void ewl_list_cb_child_hide(Ewl_Container *c, Ewl_Widget *w);

/**
 * @}
 */

#endif

