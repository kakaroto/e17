cdef extern from "etk_message_dialog.h":
    ####################################################################
    # Signals

    ####################################################################
    # Enumerations
    ctypedef enum Etk_Message_Dialog_Buttons:
        ETK_MESSAGE_DIALOG_NONE
        ETK_MESSAGE_DIALOG_OK
        ETK_MESSAGE_DIALOG_CLOSE
        ETK_MESSAGE_DIALOG_CANCEL
        ETK_MESSAGE_DIALOG_YES_NO
        ETK_MESSAGE_DIALOG_OK_CANCEL

    ctypedef enum Etk_Message_Dialog_Type:
        ETK_MESSAGE_DIALOG_INFO
        ETK_MESSAGE_DIALOG_WARNING
        ETK_MESSAGE_DIALOG_QUESTION
        ETK_MESSAGE_DIALOG_ERROR

    ####################################################################
    # Structures
    ctypedef struct Etk_Message_Dialog

    ####################################################################
    # Functions
    Etk_Type* etk_message_dialog_type_get()
    Etk_Widget* etk_message_dialog_new(int message_type, int buttons, char* text)
    int etk_message_dialog_buttons_get(Etk_Message_Dialog* __self)
    void etk_message_dialog_buttons_set(Etk_Message_Dialog* __self, int buttons)
    int etk_message_dialog_message_type_get(Etk_Message_Dialog* __self)
    void etk_message_dialog_message_type_set(Etk_Message_Dialog* __self, int type)
    char* etk_message_dialog_text_get(Etk_Message_Dialog* __self)
    void etk_message_dialog_text_set(Etk_Message_Dialog* __self, char* text)

#########################################################################
# Objects
cdef public class MessageDialog(Dialog) [object PyEtk_Message_Dialog, type PyEtk_Message_Dialog_Type]:
    pass

