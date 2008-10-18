cdef public class MessageDialog(Dialog) [object PyEtk_Message_Dialog, type PyEtk_Message_Dialog_Type]:
    def __init__(self, message_type=0, buttons=0, text="None", **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_message_dialog_new(message_type, buttons, text))
        self._set_common_params(**kargs)

    def buttons_get(self):
        __ret = <int> etk_message_dialog_buttons_get(<Etk_Message_Dialog*>self.obj)
        return (__ret)

    def buttons_set(self, int buttons):
        etk_message_dialog_buttons_set(<Etk_Message_Dialog*>self.obj, <Etk_Message_Dialog_Buttons>buttons)

    def message_type_get(self):
        __ret = <int> etk_message_dialog_message_type_get(<Etk_Message_Dialog*>self.obj)
        return (__ret)

    def message_type_set(self, int type):
        etk_message_dialog_message_type_set(<Etk_Message_Dialog*>self.obj, <Etk_Message_Dialog_Type>type)

    def text_get(self):
        cdef char *__char_ret
        __ret = None
        __char_ret = etk_message_dialog_text_get(<Etk_Message_Dialog*>self.obj)
        if __char_ret != NULL:
            __ret = __char_ret
        return (__ret)

    def text_set(self, char* text):
        etk_message_dialog_text_set(<Etk_Message_Dialog*>self.obj, text)

    property buttons:
        def __get__(self):
            return self.buttons_get()

        def __set__(self, buttons):
            self.buttons_set(buttons)

    property message_type:
        def __get__(self):
            return self.message_type_get()

        def __set__(self, message_type):
            self.message_type_set(message_type)

    property text:
        def __get__(self):
            return self.text_get()

        def __set__(self, text):
            self.text_set(text)

    def _set_common_params(self, **kargs):
        if kargs:
            Dialog._set_common_params(self, **kargs)


class MessageDialogEnums:
    NONE = ETK_MESSAGE_DIALOG_NONE
    OK = ETK_MESSAGE_DIALOG_OK
    CLOSE = ETK_MESSAGE_DIALOG_CLOSE
    CANCEL = ETK_MESSAGE_DIALOG_CANCEL
    YES_NO = ETK_MESSAGE_DIALOG_YES_NO
    OK_CANCEL = ETK_MESSAGE_DIALOG_OK_CANCEL
    INFO = ETK_MESSAGE_DIALOG_INFO
    WARNING = ETK_MESSAGE_DIALOG_WARNING
    QUESTION = ETK_MESSAGE_DIALOG_QUESTION
    ERROR = ETK_MESSAGE_DIALOG_ERROR
