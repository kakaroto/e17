# This file is included verbatim by c_ecore_evas.pyx

# cdef class DirectFBCursor:
#     cdef _set_obj(self, Ecore_DirectFB_Cursor *obj):
#         self.obj = obj

#     property surface:
#         def __get__(self):
#             return <long>self.obj.surface

#     property hot_x:
#         def __get__(self):
#             return self.obj.hot_x

#     property hot_y:
#         def __get__(self):
#             return self.obj.hot_y


# cdef class DirectFBWindow:
#     cdef _set_obj(self, Ecore_DirectFB_Window *obj):
#         self.obj = obj
#         self._cursor = DirectFBCursor()
#         self._cursor._set_obj(self.obj.cursor)

#     property id:
#         def __get__(self):
#             return self.obj.id

#     property window:
#         def __get__(self):
#             return <long>self.obj.window

#     property surface:
#         def __get__(self):
#             return <long>self.obj.surface

#     property cursor:
#         def __get__(self):
#             return self._cursor



cdef class DirectFB(EcoreEvas):
    def __init__(self, char *display=NULL, int windowed=1, int x=0, int y=0,
                 int w=320, int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_directfb_new(display, windowed, x, y, w, h)
            self._set_obj(obj)

#     def window_get(self):
#         cdef DirectFBWindow w
#         w = DirectFBWindow()
#         w._set_obj(ecore_evas_directfb_window_get(self.obj))
#         return w

#     property window:
#         def __get__(self):
#             return self.window_get()
