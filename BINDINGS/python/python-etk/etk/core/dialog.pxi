cdef public class Dialog(Window) [object PyEtk_Dialog, type PyEtk_Dialog_Type]:
    def __init__(self, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_dialog_new())
        self._set_common_params(**kargs)

    def action_area_alignment_get(self):
        __ret = etk_dialog_action_area_alignment_get(<Etk_Dialog*>self.obj)
        return (__ret)

    def action_area_alignment_set(self, float align):
        etk_dialog_action_area_alignment_set(<Etk_Dialog*>self.obj, align)

    def action_area_hbox_get(self):
        __ret = Object_from_instance(<Etk_Object*>etk_dialog_action_area_hbox_get(<Etk_Dialog*>self.obj))
        return (__ret)

    def action_area_homogeneous_get(self):
        __ret = bool(<int> etk_dialog_action_area_homogeneous_get(<Etk_Dialog*>self.obj))
        return (__ret)

    def action_area_homogeneous_set(self, int homogeneous):
        etk_dialog_action_area_homogeneous_set(<Etk_Dialog*>self.obj, <Etk_Bool>homogeneous)

    def button_add(self, char* label, int response_id):
        __ret = Object_from_instance(<Etk_Object*>etk_dialog_button_add(<Etk_Dialog*>self.obj, label, response_id))
        return (__ret)

    def button_add_from_stock(self, int stock_id, int response_id):
        __ret = Object_from_instance(<Etk_Object*>etk_dialog_button_add_from_stock(<Etk_Dialog*>self.obj, stock_id, response_id))
        return (__ret)

    def button_response_id_set(self, Button button, int response_id):
        etk_dialog_button_response_id_set(<Etk_Dialog*>self.obj, <Etk_Button*>button.obj, response_id)

    def has_separator_get(self):
        __ret = bool(<int> etk_dialog_has_separator_get(<Etk_Dialog*>self.obj))
        return (__ret)

    def has_separator_set(self, int has_separator):
        etk_dialog_has_separator_set(<Etk_Dialog*>self.obj, <Etk_Bool>has_separator)

    def main_area_vbox_get(self):
        __ret = Object_from_instance(<Etk_Object*>etk_dialog_main_area_vbox_get(<Etk_Dialog*>self.obj))
        return (__ret)

    def pack_button_in_action_area(self, Button button, int response_id, int group, int fill_policy, int padding):
        etk_dialog_pack_button_in_action_area(<Etk_Dialog*>self.obj, <Etk_Button*>button.obj, response_id, <Etk_Box_Group>group, <Etk_Box_Fill_Policy>fill_policy, padding)

    def pack_in_main_area(self, Widget widget, int group, int fill_policy, int padding):
        etk_dialog_pack_in_main_area(<Etk_Dialog*>self.obj, <Etk_Widget*>widget.obj, <Etk_Box_Group>group, <Etk_Box_Fill_Policy>fill_policy, padding)

    def pack_widget_in_action_area(self, Widget widget, int group, int fill_policy, int padding):
        etk_dialog_pack_widget_in_action_area(<Etk_Dialog*>self.obj, <Etk_Widget*>widget.obj, <Etk_Box_Group>group, <Etk_Box_Fill_Policy>fill_policy, padding)

    property action_area_alignment:
        def __get__(self):
            return self.action_area_alignment_get()

        def __set__(self, action_area_alignment):
            self.action_area_alignment_set(action_area_alignment)

    property action_area_hbox:
        def __get__(self):
            return self.action_area_hbox_get()

    property action_area_homogeneous:
        def __get__(self):
            return self.action_area_homogeneous_get()

        def __set__(self, action_area_homogeneous):
            self.action_area_homogeneous_set(action_area_homogeneous)

    property has_separator:
        def __get__(self):
            return self.has_separator_get()

        def __set__(self, has_separator):
            self.has_separator_set(has_separator)

    property main_area_vbox:
        def __get__(self):
            return self.main_area_vbox_get()

    def _set_common_params(self, action_area_alignment=None, action_area_homogeneous=None, has_separator=None, **kargs):
        if action_area_alignment is not None:
            self.action_area_alignment_set(action_area_alignment)
        if action_area_homogeneous is not None:
            self.action_area_homogeneous_set(action_area_homogeneous)
        if has_separator is not None:
            self.has_separator_set(has_separator)

        if kargs:
            Window._set_common_params(self, **kargs)

    property RESPONSE_SIGNAL:
        def __get__(self):
            return ETK_DIALOG_RESPONSE_SIGNAL

    def on_response(self, func, *a, **ka):
        self.connect(self.RESPONSE_SIGNAL, func, *a, **ka)


class DialogEnums:
    NONE = ETK_RESPONSE_NONE
    REJECT = ETK_RESPONSE_REJECT
    ACCEPT = ETK_RESPONSE_ACCEPT
    DELETE_EVENT = ETK_RESPONSE_DELETE_EVENT
    OK = ETK_RESPONSE_OK
    CANCEL = ETK_RESPONSE_CANCEL
    CLOSE = ETK_RESPONSE_CLOSE
    YES = ETK_RESPONSE_YES
    NO = ETK_RESPONSE_NO
    APPLY = ETK_RESPONSE_APPLY
    HELP = ETK_RESPONSE_HELP
