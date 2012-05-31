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

cdef public class Background(Object) [object PyElementaryBackground, type PyElementaryBackground_Type]:
    """
    Background widget object

    This widget represent a background of a window. It holds a
    image as background.
    """

    def __init__(self, c_evas.Object parent):
        """Initialize the background

        @param parent: Parent window
        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_bg_add(parent.obj))

    def file_set(self, filename, group = ""):
        """
        Set the file (image or edje) used for the background

        @param filename: The file path
        @param group: Optional key (group in Edje) within the file

        This sets the image file used in the background object. The image (or edje)
        will be stretched (retaining aspect if its an image file) to completely fill
        the bg object. This may mean some parts arte not visible.
        """
        elm_bg_file_set(self.obj, filename, group)

    def file_get(self):
        """
        Get the file (image or edje) used for the background

        @return: The tuple (filename, group)
        """
        cdef const_char_ptr filename, group

        elm_bg_file_get(self.obj, &filename, &group)
        if filename == NULL:
            filename = ""
        if group == NULL:
            group = ""
        return (filename, group)

    property file:
        def __get__(self):
            return self.file_get()

        def __set__(self, value):
            self.file_set(value)

    def option_set(self, option):
        """
        Set the mode of display of the background

        @param option: choose from Elm_Bg_Option
        """
        elm_bg_option_set(self.obj, option)

    def option_get(self):
        """
        Get the mode of display of the background

        @return: the current mode
        """
        return elm_bg_option_get(self.obj)

    property option:
        def __get__(self):
            return self.option_get()

        def __set__(self, value):
            self.option_set(value)

    def color_set(self, r, g, b):
        """
        Set the color of the bg

        @param r: the red component (range: 0 to 255)
        @param g: the green component (range: 0 to 255)
        @param b: the blue component (range: 0 to 255)
        """
        elm_bg_color_set(self.obj, r, g, b)

    def color_get(self):
        """
        Get the color of the bg

        @return: the tuple (r, g, b)
        """
        cdef int r, g, b

        elm_bg_color_get(self.obj, &r, &g, &b)
        return (r, g, b)

    property color:
        def __get__(self):
            return self.color_get()

        def __set__(self, value):
            self.color_set(*value)

    def load_size_set(self, w, h):
        """
        Set the load size of the background image

        @param w: width
        @param h: height
        """
        elm_bg_load_size_set(self.obj, w, h)

    property load_size:
        def __set__(self, value):
            self.load_size_set(*value)


_elm_widget_type_register("bg", Background)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryBackground_Type # hack to install metaclass
_install_metaclass(&PyElementaryBackground_Type, ElementaryObjectMeta)
