cdef extern from "etk_dialog.h":
    ####################################################################
    # Signals
    int ETK_DIALOG_RESPONSE_SIGNAL

    ####################################################################
    # Enumerations
    ctypedef enum Etk_Dialog_Response_ID:
        ETK_RESPONSE_NONE
        ETK_RESPONSE_REJECT
        ETK_RESPONSE_ACCEPT
        ETK_RESPONSE_DELETE_EVENT
        ETK_RESPONSE_OK
        ETK_RESPONSE_CANCEL
        ETK_RESPONSE_CLOSE
        ETK_RESPONSE_YES
        ETK_RESPONSE_NO
        ETK_RESPONSE_APPLY
        ETK_RESPONSE_HELP

    ####################################################################
    # Structures
    ctypedef struct Etk_Dialog

    ####################################################################
    # Functions
    int etk_dialog_button_response_id_get(Etk_Button* button)
    Etk_Type* etk_dialog_type_get()
    Etk_Widget* etk_dialog_new()
    float etk_dialog_action_area_alignment_get(Etk_Dialog* __self)
    void etk_dialog_action_area_alignment_set(Etk_Dialog* __self, float align)
    Etk_Widget* etk_dialog_action_area_hbox_get(Etk_Dialog* __self)
    int etk_dialog_action_area_homogeneous_get(Etk_Dialog* __self)
    void etk_dialog_action_area_homogeneous_set(Etk_Dialog* __self, int homogeneous)
    Etk_Widget* etk_dialog_button_add(Etk_Dialog* __self, char* label, int response_id)
    Etk_Widget* etk_dialog_button_add_from_stock(Etk_Dialog* __self, int stock_id, int response_id)
    void etk_dialog_button_response_id_set(Etk_Dialog* __self, Etk_Button* button, int response_id)
    int etk_dialog_has_separator_get(Etk_Dialog* __self)
    void etk_dialog_has_separator_set(Etk_Dialog* __self, int has_separator)
    Etk_Widget* etk_dialog_main_area_vbox_get(Etk_Dialog* __self)
    void etk_dialog_pack_button_in_action_area(Etk_Dialog* __self, Etk_Button* button, int response_id, int group, int fill_policy, int padding)
    void etk_dialog_pack_in_main_area(Etk_Dialog* __self, Etk_Widget* widget, int group, int fill_policy, int padding)
    void etk_dialog_pack_widget_in_action_area(Etk_Dialog* __self, Etk_Widget* widget, int group, int fill_policy, int padding)

#########################################################################
# Objects
cdef public class Dialog(Window) [object PyEtk_Dialog, type PyEtk_Dialog_Type]:
    pass
