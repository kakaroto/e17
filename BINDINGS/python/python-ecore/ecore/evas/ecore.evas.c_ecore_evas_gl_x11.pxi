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

cdef class GLX11(BaseX11):
    """X11 window using OpenGL render.

    @ivar window: X11 window id.
    @ivar direct_resize: if direct resize is enabled or not.
    """
    def __init__(self, char *display=NULL, long parent_xid=0, int x=0, int y=0,
                 int w=320, int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_gl_x11_new(display, parent_xid, x, y, w, h)
            self._set_obj(obj)

    def window_get(self):
        """Get X11 window id.

           @rtype: int
        """
        return ecore_evas_gl_x11_window_get(self.obj)

    property window:
        def __get__(self):
            return self.window_get()

    def direct_resize_set(self, int on):
        ecore_evas_gl_x11_direct_resize_set(self.obj, on)

    def direct_resize_get(self):
        "@rtype: bool"
        return bool(ecore_evas_gl_x11_direct_resize_get(self.obj))

    property direct_resize:
        def __get__(self):
            return self.direct_resize_get()

        def __set__(self, int on):
            self.direct_resize_set(on)

    def extra_event_window_add(self, long win_xid):
        ecore_evas_gl_x11_extra_event_window_add(self.obj, win_xid)

