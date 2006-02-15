#ifndef EWL_ATTACH_H
#define EWL_ATTACH_H

/**
 * @defgroup Ewl_Attach Ewl_Attach: A mechanism to attach data to widgets.
 * @brief Defines methods for attaching data to widgets
 *
 * @{
 */

#define EWL_ATTACH_TOOLTIP_TYPE "tooltip"

/**
 * @def EWL_ATTACH(attach)
 * Typecast a pointer to an Ewl_Attach pointer 
 */
#define EWL_ATTACH(attach) ((Ewl_Attach *) attach)

/**
 * Ewl_Attach provides a way to attach data to widgets
 */
typedef struct Ewl_Attach Ewl_Attach;

/**
 * @struct Ewl_Attach
 * Contains the attachment information
 */
struct Ewl_Attach
{
	Ewl_Attach_Type type;		/**< The type of the attachment */
	Ewl_Attach_Data_Type data_type; /**< The data type of the attachment */

	void *data;			/**< The attachment data */
	void *priv_data;		/**< Any private data in the attachment */
};

/**
 * @def ewl_attach_tooltip_text_set(w, data)
 * Conviencice method to set a tooltip text attachment
 */
#define ewl_attach_tooltip_text_set(w, data) \
	ewl_attach_text_set(w, EWL_ATTACH_TYPE_TOOLTIP, data)

/**
 * @def ewl_attach_tooltip_widget_set(w, data)
 * Convience method to set a tooltip widget attachement
 */
#define ewl_attach_tooltip_widget_set(w, data) \
	ewl_attach_widget_set(w, EWL_ATTACH_TYPE_TOOLTIP, data)


/**
 * @def ewl_attach_mouse_pointer_set(w, data)
 * Convience method to set a pointer widget attachment
 */
#define ewl_attach_mouse_cursor_set(w, data) \
	ewl_attach_other_set(w, EWL_ATTACH_TYPE_MOUSE_CURSOR, \
			(int *)ecore_x_cursor_shape_get(data))

/**
 * @def ewl_attach_color_set(w, data)
 * Convience method to set a colour widget attachment
 */
#define ewl_attach_color_set(w, data) \
	ewl_attach_other_set(w, EWL_ATTACH_TYPE_COLOR, data)

/**
 * @def ewl_attach_color_get(w) 
 * Convience method to get a colour attachment back
 */
#define ewl_attach_color_get(w) \
	ewl_attach_get(w, EWL_ATTACH_TYPE_COLOR)

/**
 * @def ewl_attach_name_set(w, data)
 * Convience method to set a widget name attachment
 */
#define ewl_attach_name_set(w, data) \
	ewl_attach_text_set(w, EWL_ATTACH_TYPE_NAME, data)

/**
 * @def ewl_attach_name_get(w)
 * Convience method to get a widgets name attachment
 */
#define ewl_attach_name_get(w) \
	ewl_attach_get(w, EWL_ATTACH_TYPE_NAME)

/**
 * @def ewl_attach_notebook_data_set(w, data)
 * Convience method to attach data to a notebook item
 */
#define ewl_attach_notebook_data_set(w, data) \
	ewl_attach_other_set(w, EWL_ATTACH_TYPE_NOTEBOOK_DATA, data)

/**
 * @def ewl_attach_notebook_data_get(w)
 * Convience method to get the data attached to a notebook item
 */
#define ewl_attach_notebook_data_get(w) \
	ewl_attach_get(w, EWL_ATTACH_TYPE_NOTEBOOK_DATA)

void ewl_attach_text_set(Ewl_Widget *w, Ewl_Attach_Type t, const char *data);
void ewl_attach_widget_set(Ewl_Widget *w, Ewl_Attach_Type t, Ewl_Widget *data);
void ewl_attach_other_set(Ewl_Widget *w, Ewl_Attach_Type t, void *data);
void *ewl_attach_get(Ewl_Widget *w, Ewl_Attach_Type t);

void ewl_attach_list_del(Ewl_Attach_List *list, Ewl_Attach_Type type);

/**
 * @}
 */

#endif

