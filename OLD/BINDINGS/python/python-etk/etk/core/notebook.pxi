cdef public class Notebook(Container) [object PyEtk_Notebook, type PyEtk_Notebook_Type]:
    def __init__(self, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_notebook_new())
        self._set_common_params(**kargs)

    def current_page_get(self):
        __ret = etk_notebook_current_page_get(<Etk_Notebook*>self.obj)
        return (__ret)

    def current_page_set(self, int page_num):
        etk_notebook_current_page_set(<Etk_Notebook*>self.obj, page_num)

    def num_pages_get(self):
        __ret = etk_notebook_num_pages_get(<Etk_Notebook*>self.obj)
        return (__ret)

    def page_append(self, char* tab_label, Widget page_child):
        __ret = etk_notebook_page_append(<Etk_Notebook*>self.obj, tab_label, <Etk_Widget*>page_child.obj)
        return (__ret)

    def page_child_get(self, int page_num):
        __ret = Object_from_instance(<Etk_Object*>etk_notebook_page_child_get(<Etk_Notebook*>self.obj, page_num))
        return (__ret)

    def page_child_set(self, int page_num, Widget child):
        etk_notebook_page_child_set(<Etk_Notebook*>self.obj, page_num, <Etk_Widget*>child.obj)

    def page_index_get(self, Widget child):
        __ret = etk_notebook_page_index_get(<Etk_Notebook*>self.obj, <Etk_Widget*>child.obj)
        return (__ret)

    def page_insert(self, char* tab_label, Widget page_child, int position):
        __ret = etk_notebook_page_insert(<Etk_Notebook*>self.obj, tab_label, <Etk_Widget*>page_child.obj, position)
        return (__ret)

    def page_next(self):
        __ret = etk_notebook_page_next(<Etk_Notebook*>self.obj)
        return (__ret)

    def page_prepend(self, char* tab_label, Widget page_child):
        __ret = etk_notebook_page_prepend(<Etk_Notebook*>self.obj, tab_label, <Etk_Widget*>page_child.obj)
        return (__ret)

    def page_prev(self):
        __ret = etk_notebook_page_prev(<Etk_Notebook*>self.obj)
        return (__ret)

    def page_remove(self, int page_num):
        __ret = Object_from_instance(<Etk_Object*>etk_notebook_page_remove(<Etk_Notebook*>self.obj, page_num))
        return (__ret)

    def page_tab_label_get(self, int page_num):
        cdef char *__char_ret
        __ret = None
        __char_ret = etk_notebook_page_tab_label_get(<Etk_Notebook*>self.obj, page_num)
        if __char_ret != NULL:
            __ret = __char_ret
        return (__ret)

    def page_tab_label_set(self, int page_num, char* tab_label):
        etk_notebook_page_tab_label_set(<Etk_Notebook*>self.obj, page_num, tab_label)

    def page_tab_widget_get(self, int page_num):
        __ret = Object_from_instance(<Etk_Object*>etk_notebook_page_tab_widget_get(<Etk_Notebook*>self.obj, page_num))
        return (__ret)

    def page_tab_widget_set(self, int page_num, Widget tab_widget):
        etk_notebook_page_tab_widget_set(<Etk_Notebook*>self.obj, page_num, <Etk_Widget*>tab_widget.obj)

    def tabs_homogeneous_get(self):
        __ret = bool(<int> etk_notebook_tabs_homogeneous_get(<Etk_Notebook*>self.obj))
        return (__ret)

    def tabs_homogeneous_set(self, int tabs_homogeneous):
        etk_notebook_tabs_homogeneous_set(<Etk_Notebook*>self.obj, <Etk_Bool>tabs_homogeneous)

    def tabs_visible_get(self):
        __ret = bool(<int> etk_notebook_tabs_visible_get(<Etk_Notebook*>self.obj))
        return (__ret)

    def tabs_visible_set(self, int tabs_visible):
        etk_notebook_tabs_visible_set(<Etk_Notebook*>self.obj, <Etk_Bool>tabs_visible)

    property current_page:
        def __get__(self):
            return self.current_page_get()

        def __set__(self, current_page):
            self.current_page_set(current_page)

    property num_pages:
        def __get__(self):
            return self.num_pages_get()

    property tabs_homogeneous:
        def __get__(self):
            return self.tabs_homogeneous_get()

        def __set__(self, tabs_homogeneous):
            self.tabs_homogeneous_set(tabs_homogeneous)

    property tabs_visible:
        def __get__(self):
            return self.tabs_visible_get()

        def __set__(self, tabs_visible):
            self.tabs_visible_set(tabs_visible)

    def _set_common_params(self, current_page=None, tabs_homogeneous=None, tabs_visible=None, **kargs):
        if current_page is not None:
            self.current_page_set(current_page)
        if tabs_homogeneous is not None:
            self.tabs_homogeneous_set(tabs_homogeneous)
        if tabs_visible is not None:
            self.tabs_visible_set(tabs_visible)

        if kargs:
            Container._set_common_params(self, **kargs)

    property PAGE_CHANGED_SIGNAL:
        def __get__(self):
            return ETK_NOTEBOOK_PAGE_CHANGED_SIGNAL

    def on_page_changed(self, func, *a, **ka):
        self.connect(self.PAGE_CHANGED_SIGNAL, func, *a, **ka)
