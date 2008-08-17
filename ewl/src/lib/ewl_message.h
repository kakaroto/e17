/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MESSAGE_H
#define EWL_MESSAGE_H

/**
 * @addtogroup Ewl_Message Ewl_Message: A dialog window for showing a message
 * @brief Defines the Ewl_Message class which extends the Ewl_Icondialog class.
 *
 * @remarks Inherits from Ewl_Icondialog.
 * @if HAVE_IMAGES
 * @image html Ewl_Message_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /message/file
 * @themekey /message/group
 */

/**
 * @def EWL_MESSAGE_TYPE
 * The type name for the Ewl_Message widget
 */
#define EWL_MESSAGE_TYPE "message"

/**
 * @def EWL_MESSAGE_IS(w)
 * Returns TRUE if the widget is an Ewl_Message, FALSE otherwise
 */
#define EWL_MESSAGE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_MESSAGE_TYPE))

/**
 * The message structure extends the dialog with an icon.
 */
typedef struct Ewl_Message Ewl_Message;

/**
 * @def EWL_MESSAGE(dialog)
 * Typecasts a pointer to an Ewl_Message pointer.
 */
#define EWL_MESSAGE(m) ((Ewl_Message *) m)

/**
 * @brief Inherits from Ewl_Dialog and extends to provide a message widget.
 */
struct Ewl_Message
{
        Ewl_Icondialog dialog;   /**< Inherit from an icon dialog */
        
        Ewl_Widget *brief;       /**< The brief text */
        Ewl_Widget *detailed;    /**< The detailed text */
};

Ewl_Widget      *ewl_message_new(void);
int              ewl_message_init(Ewl_Message *m);

void             ewl_message_brief_text_set(Ewl_Message *m, const char *text);
const char      *ewl_message_brief_text_get(Ewl_Message *m);

void             ewl_message_detailed_text_set(Ewl_Message *m, 
                                                const char *text);
const char      *ewl_message_detailed_text_get(Ewl_Message *m);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_message_cb_quit(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif
