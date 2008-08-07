# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

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
    "DirectFB render."
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
