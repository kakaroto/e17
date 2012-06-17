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

cdef public class Background(LayoutClass) [object PyElementaryBackground, type PyElementaryBackground_Type]:

    """Background widget object

    Used for setting a solid color, image or Edje group as a background to a
    window or any container object.

    The background widget is used for setting (solid) background decorations
    to a window (unless it has transparency enabled) or to any container
    object. It works just like an image, but has some properties useful to a
    background, like setting it to tiled, centered, scaled or stretched.

    Default content parts of the bg widget that you can use for are:
        - B{overlay} - overlay of the bg

    """

    def __init__(self, evasObject parent):
        """Initialize the background

        @param parent: Parent window
        @type parent: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_bg_add(parent.obj))

    def file_set(self, filename, group = ""):
        """Set the file (image or edje collection) to give life for the background.

        This sets the image file used in the background object. If the image
        comes from an Edje group, it will be stretched to completely fill
        the background object. If it comes from a traditional image file, it
        will by default be centered in this widget's are (thus retaining its
        aspect), what could lead to some parts being not visible. You may
        change the mode of exhibition for a real image file with
        L{option_set()}.

        @note: Once the image is set, a previously set one will be deleted,
            even if B{file} is C{None}.

        @note: This will only affect the contents of one of the background's
            swallow spots, namely C{"elm.swallow.background"}. If you want to
            achieve the L{Layout}'s file setting behavior, you'll have to call
            that method on this object.

        @param file: The file path
        @type file: string
        @param group: Optional key (group in Edje) within the file
        @type group: string
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_bg_file_set(self.obj, _cfruni(filename), _cfruni(group)))

    def file_get(self):
        """Get the file (image or edje) used for the background

        @return: The tuple (filename, group)
        @rtype: tuple

        """
        cdef const_char_ptr filename, group

        elm_bg_file_get(self.obj, &filename, &group)
        if filename == NULL:
            filename = ""
        if group == NULL:
            group = ""
        return (_ctouni(filename), _ctouni(group))

    property file:
        """The file (image or edje collection) giving life for the background.

        This property contains the image file name (and edje group) used in
        the background object. If the image comes from an Edje group, it
        will be stretched to completely fill the background object. If it
        comes from a traditional image file, it will by default be centered
        in this widget's are (thus retaining its aspect), what could lead to
        some parts being not visible. You may change the mode of exhibition
        for a real image file with L{option}.

        @note: Once the image is set, a previously set one will be deleted,
            even if B{file} is C{None}.

        @note: This will only affect the contents of one of the background's
            swallow spots, namely C{"elm.swallow.background"}. If you want to
            achieve the L{Layout}'s file setting behavior, you'll have to call
            that method on this object.

        @type: string file, optional string group

        """
        def __get__(self):
            cdef const_char_ptr filename, group
            elm_bg_file_get(self.obj, &filename, &group)
            if filename == NULL:
                filename = ""
            if group == NULL:
                group = ""
            return (_ctouni(filename), _ctouni(group))

        def __set__(self, value):
            if isinstance(value, tuple) or isinstance(value, list):
                filename, group = value
            else:
                filename = value
                group = ""
            elm_bg_file_set(self.obj, _cfruni(filename), _cfruni(group))

    def option_set(self, option):
        """Set the mode of display for a given background widget's image.

        This sets how the background widget will display its image. This
        will only work if L{file_set()} was previously called with
        an image file. The image can be displayed tiled, scaled,
        centered or stretched.

        @param option: The desired background option
        @type option: Elm_Bg_Option

        """
        elm_bg_option_set(self.obj, option)

    def option_get(self):
        """Get the mode of display for a given background widget's image.

        @return: The image displaying mode in use for obj or ELM_BG_OPTION_LAST
            on errors.
        @rtype: Elm_Bg_Option

        """
        return elm_bg_option_get(self.obj)

    property option:
        """The mode of display for a given background widget's image.

        This property reflects how the background widget will display its
        image. This will only work if L{file} was previously called with an
        image file. The image can be displayed tiled, scaled, centered or
        stretched.

        @type: Elm_Bg_Option

        """
        def __get__(self):
            return elm_bg_option_get(self.obj)

        def __set__(self, value):
            elm_bg_option_set(self.obj, value)

    def color_set(self, r, g, b):
        """Set the color on a given background widget.

        This sets the color used for the background rectangle, in RGB
        format. Each color component's range is from 0 to 255.

        @note: You probably only want to use this function if you haven't
            previously called L{file_set()}, so that you just want a solid
            color background.

        @param r: The red color component's value
        @type r: int
        @param g: The green color component's value
        @type g: int
        @param b: The blue color component's value
        @type b: int

        """
        elm_bg_color_set(self.obj, r, g, b)

    def color_get(self):
        """Get the color set on a given background widget.

        @return: The tuple (r, g, b)
        @rtype: tuple of ints

        """
        cdef int r, g, b

        elm_bg_color_get(self.obj, &r, &g, &b)
        return (r, g, b)

    property color:
        """The color on a given background widget.

        This property reflects the color used for the background rectangle,
        in RGB format. Each color component's range is from 0 to 255.

        @note: You probably only want to use this property if you haven't
            previously set L{file}, so that you just want a solid color
            background.

        @type: (int r, int g, int b)

        """
        def __get__(self):
            cdef int r, g, b
            elm_bg_color_get(self.obj, &r, &g, &b)
            return (r, g, b)

        def __set__(self, value):
            cdef int r, g, b
            r, g, b = value
            elm_bg_color_set(self.obj, r, g, b)

    def load_size_set(self, w, h):
        """Set the size of the pixmap representation of the image set on a given
        background widget.

        This function sets a new size for pixmap representation of the
        given bg image. It allows for the image to be loaded already in the
        specified size, reducing the memory usage and load time (for
        example, when loading a big image file with its load size set to a
        smaller size)

        @note: This is just a hint for the underlying system. The real size
            of the pixmap may differ depending on the type of image being
            loaded, being bigger than requested.

        @warning: This function just makes sense if an image file was set with
            L{file_set()}.

        @param w: The new width of the image pixmap representation.
        @type w: Evas_Coord
        @param h: The new height of the image pixmap representation.
        @type h: Evas_Coord

        """
        elm_bg_load_size_set(self.obj, w, h)

    property load_size:
        """The size of the pixmap representation of the image set on a given
        background widget.

        This property sets a new size for pixmap representation of the
        given bg image. It allows for the image to be loaded already in the
        specified size, reducing the memory usage and load time (for
        example, when loading a big image file with its load size set to a
        smaller size)

        @note: This is just a hint for the underlying system. The real size
            of the pixmap may differ depending on the type of image being
            loaded, being bigger than requested.

        @warning: This function just makes sense if an image file was set with
            L{file}.

        @type: (Evas_Coord w, Evas_Coord h)

        """
        def __set__(self, value):
            cdef Evas_Coord w, h
            w, h = value
            elm_bg_load_size_set(self.obj, w, h)

_elm_widget_type_register("bg", Background)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryBackground_Type # hack to install metaclass
_install_metaclass(&PyElementaryBackground_Type, ElementaryObjectMeta)
