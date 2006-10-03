#ifndef EWL_MVC_H
#define EWL_MVC_H

/**
 * @addtogroup Ewl_MVC Ewl_MVC: The base model-view-controller framework
 * Defines a widget to base MVC widgets off of
 *
 * @{
 */

/**
 * @def EWL_MVC_TYPE
 * The type name for the Ewl_MVC widget
 */
#define EWL_MVC_TYPE "mvc"

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
 * Inherits from Ewl_Box and extends to provide mvc functionality
 */
struct Ewl_MVC
{
	Ewl_Box box;		/**< Inherit from Ewl_Box */

	void *data;		/**< The mvc data */
	Ewl_View *view;		/**< The view for the mvc */
	Ewl_Model *model;	/**< The model for the mvc */

	struct {
		void (*view_change)(Ewl_MVC *mvc);
		void (*selected_change)(Ewl_MVC *mvc);
	} cb;

	struct
	{
		int *items;		/**< Selected items */
		int count;		/**< Number of selected */
	} selected;			/**< The selected info */

	unsigned char multiselect:1;	/**< is the widget multiselect capable */
	unsigned char dirty:1;		/**< Is the data dirty */
};

int		 ewl_mvc_init(Ewl_MVC *mvc);

void		 ewl_mvc_view_set(Ewl_MVC *mvc, Ewl_View *view);
Ewl_View 	*ewl_mvc_view_get(Ewl_MVC *mvc);

void		 ewl_mvc_model_set(Ewl_MVC *mvc, Ewl_Model *model);
Ewl_Model	*ewl_mvc_model_get(Ewl_MVC *mvc);

void		 ewl_mvc_data_set(Ewl_MVC *mvc, void *data);
void 		*ewl_mvc_data_get(Ewl_MVC *mvc);

void		 ewl_mvc_dirty_set(Ewl_MVC *mvc, unsigned int dirty);
unsigned int	 ewl_mvc_dirty_get(Ewl_MVC *mvc);

void		 ewl_mvc_multiselect_set(Ewl_MVC *mvc, unsigned int multi);
unsigned int	 ewl_mvc_multiselect_get(Ewl_MVC *mvc);

void		 ewl_mvc_selected_list_set(Ewl_MVC *mvc, int *list);
const int	*ewl_mvc_selected_list_get(Ewl_MVC *mvc);

void		 ewl_mvc_selected_range_set(Ewl_MVC *mvc, int start, int end);

void		 ewl_mvc_selected_set(Ewl_MVC *mvc, int i);
void		 ewl_mvc_selected_add(Ewl_MVC *mvc, int i);
int		 ewl_mvc_selected_get(Ewl_MVC *mvc);

int		 ewl_mvc_selected_count_get(Ewl_MVC *mvc);

/* 
 * internal
 */
void		 ewl_mvc_view_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc));
void		 ewl_mvc_selected_change_cb_set(Ewl_MVC *mvc, void (*cb)(Ewl_MVC *mvc));


#endif

