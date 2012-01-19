# Copyright (C) 2010 Boris Faure
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

# This file is included verbatim by c_ecore_win32.pyx

cdef class Window:
    cdef Ecore_Win32_Window *obj

    def __cinit__(self, *a, **ka):
        self.obj = NULL

    def __init__(self, Window parent=None, int x=0, int y=0, int w=1, int h=1,
                override=False):
        """Create a new Win32 window.

        @parm: B{parent:} window to use as parent, or None to use the
               root window.
        @parm: B{x:} horizontal position.
        @parm: B{y:} vertical position.
        @parm: B{w:} horizontal size.
        @parm: B{h:} vertical size.
        @parm: B{override_redirect:} if override must be used
        """
        if override:
            if parent:
                self.obj = ecore_win32_window_override_new(parent.obj, x, y, w, h)
            else:
                self.obj = ecore_win32_window_override_new(NULL, x, y, w, h)
        else:
            if parent:
                self.obj = ecore_win32_window_new(parent.obj, x, y, w, h)
            else:
                self.obj = ecore_win32_window_new(NULL, x, y, w, h)

    def __dealloc__(self):
        cdef Ecore_Win32_Window *obj

        obj = self.obj
        if obj == NULL:
            return
        self.obj = NULL

        ecore_win32_window_free(obj)

    def delete(self):
        "Deletes the current window."
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        ecore_win32_window_free(self.obj)

    def move(self, int x, int y):
        """Set window position.

        @parm: B{x:} horizontal.
        @parm: B{y:} vertical.
        """
        ecore_win32_window_move(self.obj, x, y)

    def resize(self, int width, int height):
        """Set window size.

        @parm: B{w:} horizontal.
        @parm: B{h:} vertical.
        """
        ecore_win32_window_resize(self.obj, width, height)

    def move_resize(self, int x, int y, int w, int h):
        """Set both position and size.

        @parm: B{x:} horizontal position.
        @parm: B{y:} vertical position.
        @parm: B{w:} horizontal size.
        @parm: B{h:} vertical size.
        """
        ecore_win32_window_move_resize(self.obj, x, y, w, h)

    def geometry_set(self, int x, int y, int w, int h):
        """Set both position and size.

        @parm: B{x:} horizontal position.
        @parm: B{y:} vertical position.
        @parm: B{w:} horizontal size.
        @parm: B{h:} vertical size.
        """
        ecore_win32_window_move_resize(self.obj, x, y, w, h)

    def geometry_get(self):
        "@rtype: tuple of int"
        cdef int x, y, w, h
        ecore_win32_window_geometry_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    property geometry:
        def __set__(self, spec):
            self.geometry_set(*spec)

        def __get__(self):
            return self.geometry_get()

    def size_get(self):
        "@rtype: tuple of int"
        cdef int width, height
        ecore_win32_window_size_get(self.obj, &width, &height)
        return (width, height)

    def size_min_set(self, min_width, min_height):
        """Set minimum window size

        @parm: B{min_width:} horizontal size.
        @parm: B{min_height:} vertical size.
        """
        ecore_win32_window_size_min_set(self.obj, min_width, min_height)

    def size_min_get(self):
        "@rtype: tuple of int"
        cdef unsigned int w, h
        ecore_win32_window_size_min_get(self.obj, &w, &h)
        return (w, h)

    property size_min:
        def __set__(self, spec):
            self.size_min_set(*spec)

        def __get__(self):
            return self.size_min_get()


    def size_max_set(self, max_width, max_height):
        """Set maximum window size

        @parm: B{max_width:} horizontal size.
        @parm: B{max_height:} vertical size.
        """
        ecore_win32_window_size_max_set(self.obj, max_width, max_height)

    def size_max_get(self):
        "@rtype: tuple of int"
        cdef unsigned int w, h
        ecore_win32_window_size_max_get(self.obj, &w, &h)
        return (w, h)

    property size_max:
        def __set__(self, spec):
            self.size_max_set(*spec)

        def __get__(self):
            return self.size_max_get()


    def size_base_set(self, base_width, base_height):
        """Set base window size

        @parm: B{base_width:} horizontal size.
        @parm: B{base_height:} vertical size.
        """
        ecore_win32_window_size_base_set(self.obj, base_width, base_height)

    def size_base_get(self):
        "@rtype: tuple of int"
        cdef unsigned int w, h
        ecore_win32_window_size_base_get(self.obj, &w, &h)
        return (w, h)

    property size_base:
        def __set__(self, spec):
            self.size_base_set(*spec)

        def __get__(self):
            return self.size_base_get()


    def size_step_set(self, step_width, step_height):
        """Set step window size

        @parm: B{step_width:} horizontal size.
        @parm: B{step_height:} vertical size.
        """
        ecore_win32_window_size_step_set(self.obj, step_width, step_height)

    def size_step_get(self):
        "@rtype: tuple of int"
        cdef unsigned int w, h
        ecore_win32_window_size_step_get(self.obj, &w, &h)
        return (w, h)

    property size_step:
        def __set__(self, spec):
            self.size_step_set(*spec)

        def __get__(self):
            return self.size_step_get()


    def show(self):
        ecore_win32_window_show(self.obj)

    def hide(self):
        ecore_win32_window_hide(self.obj)

    def raise_(self):
        ecore_win32_window_raise(self.obj)

    def lower(self):
        ecore_win32_window_lower(self.obj)

    def title_set(self, label):
        ecore_win32_window_title_set(self.obj, label)

    def focus(self):
        "Give focus to this windows."
        ecore_win32_window_focus_set(self.obj)

    def iconified_set(self, on):
        ecore_win32_window_iconified_set(self.obj, on)

    def borderless_set(self, on):
        ecore_win32_window_borderless_set(self.obj, on)

    def fullscreen_set(self, on):
        ecore_win32_window_fullscreen_set(self.obj, on)

    def shape_set(self, unsigned short width, unsigned short height, mask):
        ecore_win32_window_shape_set(self.obj, width, height, mask)

    def state_request(self, int state, unsigned int set):
        ecore_win32_window_state_request_send(self.obj,
                                              <Ecore_Win32_Window_State>state,
                                              set)

    def type_set(self, int type):
        ecore_win32_window_type_set(self.obj,
                                    <Ecore_Win32_Window_Type> type)

