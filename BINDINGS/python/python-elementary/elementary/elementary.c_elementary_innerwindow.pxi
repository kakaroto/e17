# Copyright (c) 2008-2009 Simon Busch
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cdef class InnerWindow(Window):

    """An inwin is a window inside a window that is useful for a quick popup.

    There are three styles available in the default theme. These are:

    default: The inwin is sized to take over most of the window it's placed in.
    minimal: The size of the inwin will be the minimum necessary to show its
    contents.
    minimal_vertical: Horizontally, the inwin takes as much space as possible,
    vertically only as much as it needs to fit its contents.
    """

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_win_inwin_add(parent.obj))

    def activate(self):
        """Activates an inwin object, ensuring its visibility"""
        elm_win_inwin_activate(self.obj)

    def content_set(self, c_evas.Object content):
        """Set the content of an inwin object.

        @parm: B{content:} The object to set as content
        """
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_win_inwin_content_set(self.obj, o)

    def content_get(self):
        """Get the content of an inwin object.

        @return: The content that was being used
        """
        cdef c_evas.Evas_Object *o
        o = elm_win_inwin_content_get(self.obj)
        return <Object>o

    property content:
        def __get__(self):
            return self.content_get()
        def __set__(self, content):
            self.content_set(content)

    def content_unset(self):
        """Unset the content of an inwin object.

        @return: The content that was being used
        """
        cdef c_evas.Evas_Object *o
        o = elm_win_inwin_content_unset(self.obj)
        return <Object>o

_elm_widget_type_register("inwin", InnerWindow)
