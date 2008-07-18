/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ATTACH_H
#define EWL_ATTACH_H

/**
 * @addtogroup Ewl_Attach Ewl_Attach: A mechanism to attach data to widgets.
 * @brief Defines methods for attaching data to widgets
 *
 * @{
 */

/**
 * @def EWL_ATTACH_TOOLTIP_TYPE
 * The type name for the Ewl_Attach tooltip
 */
#define EWL_ATTACH_TOOLTIP_TYPE "tooltip"

/**
 * @def EWL_ATTACH_IS(w)
 * Returns TRUE if the widget is an Ewl_Attach widget, FALSE otherwise
 */
#define EWL_ATTACH_IS(w) (ewl_widget_type_is(EWL_WIDGET(w)), EWL_ATTACH_TYPE)

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
 * @brief Contains information about widget attachments.
 */
struct Ewl_Attach
{
        Ewl_Attach_Type type;                /**< The type of the attachment */
        Ewl_Attach_Data_Type data_type; /**< The data type of the attachment */

        void *data;                        /**< The attachment data */
        void *priv_data;                /**< Any private data in the attachment */
};

/**
 * Ewl_Attach_Dnd provides a way to attach DND data to widgets
 */
typedef struct Ewl_Attach_Dnd Ewl_Attach_Dnd;

/**
 * @brief DND data associated with the widget.
 */
struct Ewl_Attach_Dnd
{
        Ewl_Widget *cursor;        /**< Cursor displayed for DND */
        void *data;                /**< The attachment data */
        int size;                /**< Any private data in the attachment */
};

/**
 * @def ewl_attach_tooltip_text_set(w, data)
 * Convenience method to set a tooltip text attachment
 */
#define ewl_attach_tooltip_text_set(w, data) \
        ewl_attach_text_set(EWL_WIDGET(w), EWL_ATTACH_TYPE_TOOLTIP, data)

/**
 * @def ewl_attach_tooltip_widget_set(w, data)
 * Convenience method to set a tooltip widget attachement
 */
#define ewl_attach_tooltip_widget_set(w, data) \
        ewl_attach_widget_set(EWL_WIDGET(w), EWL_ATTACH_TYPE_TOOLTIP, data)


/**
 * @def ewl_attach_mouse_cursor_set(w, cursor)
 * Convenience method to set a cursor widget attachment
 */
#define ewl_attach_mouse_cursor_set(w, cursor) \
        ewl_attach_other_set(EWL_WIDGET(w), EWL_ATTACH_TYPE_MOUSE_CURSOR, \
                        INT_TO_INTPTR(cursor))
/**
 * @def ewl_attach_mouse_cursor_get(w)
 * Convenience method to get the widget's cursor attachment
 */
#define ewl_attach_mouse_cursor_get(w) \
        INTPTR_TO_INT(ewl_attach_get(EWL_WIDGET(w), \
                        EWL_ATTACH_TYPE_MOUSE_CURSOR))

/**
 * @def ewl_attach_mouse_argb_cursor_set(w, cursor)
 * Convenience method to attach data to a notebook item
 */
#define ewl_attach_mouse_argb_cursor_set(w, cursor) \
        ewl_attach_other_set(EWL_WIDGET(w), \
                        EWL_ATTACH_TYPE_MOUSE_ARGB_CURSOR, cursor)

/**
 * @def ewl_attach_color_set(w, data)
 * Convenience method to set a colour widget attachment
 */
#define ewl_attach_color_set(w, data) \
        ewl_attach_other_set(EWL_WIDGET(w), EWL_ATTACH_TYPE_COLOR, data)

/**
 * @def ewl_attach_color_get(w)
 * Convenience method to get a colour attachment back
 */
#define ewl_attach_color_get(w) \
        ewl_attach_get(EWL_WIDGET(w), EWL_ATTACH_TYPE_COLOR)

/**
 * @def ewl_attach_name_set(w, data)
 * Convenience method to set a widget name attachment
 */
#define ewl_attach_name_set(w, data) \
        ewl_attach_text_set(EWL_WIDGET(w), EWL_ATTACH_TYPE_NAME, data)

/**
 * @def ewl_attach_name_get(w)
 * Convenience method to get a widgets name attachment
 */
#define ewl_attach_name_get(w) \
        ewl_attach_get(EWL_WIDGET(w), EWL_ATTACH_TYPE_NAME)

/**
 * @def ewl_attach_widget_association_set(w, data)
 * Convenience method to attach data to a notebook item
 */
#define ewl_attach_widget_association_set(w, data) \
        ewl_attach_other_set(EWL_WIDGET(w), \
                        EWL_ATTACH_TYPE_WIDGET_ASSOCIATION, data)

/**
 * @def ewl_attach_widget_association_get(w)
 * Convenience method to get the data attached to a notebook item
 */
#define ewl_attach_widget_association_get(w) \
        ewl_attach_get(EWL_WIDGET(w), EWL_ATTACH_TYPE_WIDGET_ASSOCIATION)

void  ewl_attach_text_set(Ewl_Widget *w, Ewl_Attach_Type t, const char *data);
void  ewl_attach_widget_set(Ewl_Widget *w, Ewl_Attach_Type t, Ewl_Widget *data);
void  ewl_attach_other_set(Ewl_Widget *w, Ewl_Attach_Type t, void *data);
void *ewl_attach_get(Ewl_Widget *w, Ewl_Attach_Type t);

void  ewl_attach_list_del(Ewl_Attach_List *list, Ewl_Attach_Type type);

/**
 * @}
 */

#endif

