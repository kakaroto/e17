# This file is included verbatim by c_ecore_evas.pyx

cdef class GLX11(EcoreEvas):
    def __init__(self, char *display=NULL, long parent_xid=0, int x=0, int y=0,
                 int w=320, int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_gl_x11_new(display, parent_xid, x, y, w, h)
            self._set_obj(obj)

    def window_get(self):
        return ecore_evas_gl_x11_window_get(self.obj)

    property window:
        def __get__(self):
            return self.window_get()

    def subwindow_get(self):
        return ecore_evas_gl_x11_subwindow_get(self.obj)

    property subwindow:
        def __get__(self):
            return self.subwindow_get()

    def direct_resize_set(self, int on):
        ecore_evas_gl_x11_direct_resize_set(self.obj, on)

    def direct_resize_get(self):
        return bool(ecore_evas_gl_x11_direct_resize_get(self.obj))

    property direct_resize:
        def __get__(self):
            return self.direct_resize_get()

        def __set__(self, int on):
            self.direct_resize_set(on)

    def extra_event_window_add(self, long win_xid):
        ecore_evas_gl_x11_extra_event_window_add(self.obj, win_xid)

