cdef extern from "etk_notebook.h":
    ####################################################################
    # Signals
    int ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Notebook
    ctypedef struct Etk_Notebook_Page

    ####################################################################
    # Functions
    Etk_Type* etk_notebook_type_get()
    Etk_Widget* etk_notebook_new()
    int etk_notebook_current_page_get(Etk_Notebook* __self)
    void etk_notebook_current_page_set(Etk_Notebook* __self, int page_num)
    int etk_notebook_num_pages_get(Etk_Notebook* __self)
    int etk_notebook_page_append(Etk_Notebook* __self, char* tab_label, Etk_Widget* page_child)
    Etk_Widget* etk_notebook_page_child_get(Etk_Notebook* __self, int page_num)
    void etk_notebook_page_child_set(Etk_Notebook* __self, int page_num, Etk_Widget* child)
    int etk_notebook_page_index_get(Etk_Notebook* __self, Etk_Widget* child)
    int etk_notebook_page_insert(Etk_Notebook* __self, char* tab_label, Etk_Widget* page_child, int position)
    int etk_notebook_page_next(Etk_Notebook* __self)
    int etk_notebook_page_prepend(Etk_Notebook* __self, char* tab_label, Etk_Widget* page_child)
    int etk_notebook_page_prev(Etk_Notebook* __self)
    Etk_Widget* etk_notebook_page_remove(Etk_Notebook* __self, int page_num)
    char* etk_notebook_page_tab_label_get(Etk_Notebook* __self, int page_num)
    void etk_notebook_page_tab_label_set(Etk_Notebook* __self, int page_num, char* tab_label)
    Etk_Widget* etk_notebook_page_tab_widget_get(Etk_Notebook* __self, int page_num)
    void etk_notebook_page_tab_widget_set(Etk_Notebook* __self, int page_num, Etk_Widget* tab_widget)
    int etk_notebook_tabs_homogeneous_get(Etk_Notebook* __self)
    void etk_notebook_tabs_homogeneous_set(Etk_Notebook* __self, int tabs_homogeneous)
    int etk_notebook_tabs_visible_get(Etk_Notebook* __self)
    void etk_notebook_tabs_visible_set(Etk_Notebook* __self, int tabs_visible)

#########################################################################
# Objects
cdef public class Notebook(Container) [object PyEtk_Notebook, type PyEtk_Notebook_Type]:
    pass
