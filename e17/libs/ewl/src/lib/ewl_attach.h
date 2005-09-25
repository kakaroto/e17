#ifndef EWL_ATTACH_H
#define EWL_ATTACH_H

/**
 * @file ewl_attach.h
 * @defgroup Ewl_Attach Attachments: The attachment mechanisms
 * Defines methods for attaching data to widgets
 *
 * @{
 */

#define EWL_ATTACH(attach) ((Ewl_Attach *) attach)

typedef struct Ewl_Attach Ewl_Attach;
struct Ewl_Attach
{
	Ewl_Attach_Type type;
	Ewl_Attach_Data_Type data_type;

	void *data;
	void *priv_data;
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

void ewl_attach_text_set(Ewl_Widget *w, Ewl_Attach_Type t, const char *data);
void ewl_attach_widget_set(Ewl_Widget *w, Ewl_Attach_Type t, Ewl_Widget *data);

/* 
 * internal stuff 
 */
Ewl_Attach_List *ewl_attach_list_new(void);
void ewl_attach_list_free(Ewl_Attach_List *list);

void ewl_attach_list_add(Ewl_Attach_List *list, Ewl_Widget *parent, Ewl_Attach *attach);
void ewl_attach_list_del(Ewl_Attach_List *list, Ewl_Attach_Type type);

Ewl_Attach *ewl_attach_new(Ewl_Attach_Type t, Ewl_Attach_Data_Type dt, void *data);
int ewl_attach_init(Ewl_Attach *attach, Ewl_Attach_Type t, Ewl_Attach_Data_Type dt, void *data);
void ewl_attach_free(Ewl_Attach *attach);

/**
 * @}
 */

#endif

