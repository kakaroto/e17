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

cdef public class Photo(Object) [object PyElementaryPhoto, type PyElementaryPhoto_Type]:

    """An Elementary photo widget is intended for displaying a photo, for
    ex., a person's image (contact).

    Simple, yet with a very specific purpose. It has a decorative frame
    around the inner image itself, on the default theme.

    This widget relies on an internal L{Icon}, so that the APIs of these two
    widgets are similar (drag and drop is also possible here, for example).

    Signals that you can add callbacks for are:
        - C{"clicked"} - This is called when a user has clicked the photo
        - C{"drag,start"} - One has started dragging the inner image out of the
                            photo's frame
        - C{"drag,end"} - One has dropped the dragged image somewhere

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_photo_add(parent.obj))

    def file_set(self, filename):
        """Set the file that will be used as photo

        @param file: The path to file that will be used as photo
        @type file: string

        @return: (True = success, False = error)
        @rtype: bool

        """
        if filename:
           return bool(elm_photo_file_set(self.obj, _cfruni(filename)))
        else:
           return bool(elm_photo_file_set(self.obj, NULL))

    property file:
        """Set the file that will be used as photo

        @type: string

        """
        def __set__(self, filename):
            # TODO: check return status
            if filename:
               elm_photo_file_set(self.obj, _cfruni(filename))
            else:
               elm_photo_file_set(self.obj, NULL)

    def thumb_set(self, filename, group = None):
        """Set the file that will be used as thumbnail in the photo.

        @param file: The path to file that will be used as thumbnail.
        @type file: string
        @param group: The key used in case of an EET file.
        @type group: string

        """
        elm_photo_thumb_set(self.obj, _cfruni(filename), _cfruni(group))

    property thumb:
        """Set the file that will be used as thumbnail in the photo.

        @type: string or tuple of strings

        """
        def __set__(self, value):
            if isinstance(value, tuple):
                filename, group = value
            else:
                filename = value
                group = None
            elm_photo_thumb_set(self.obj, _cfruni(filename), _cfruni(group))

    def size_set(self, size):
        """Set the size that will be used on the photo.

        @param size: The size of the photo
        @type size: int

        """
        elm_photo_size_set(self.obj, size)

    property size:
        """Set the size that will be used on the photo.

        @type: int

        """
        def __set__(self, size):
            elm_photo_size_set(self.obj, size)

    def fill_inside_set(self, fill):
        """Set if the photo should be completely visible or not.

        @param fill: If True the photo will be completely visible.
        @type fill: bool

        """
        elm_photo_fill_inside_set(self.obj, fill)

    property fill_inside:
        """Set if the photo should be completely visible or not.

        @type: bool

        """
        def __set__(self, fill):
            elm_photo_fill_inside_set(self.obj, fill)

    def editable_set(self, fill):
        """Set editability of the photo.

        An editable photo can be dragged to or from, and can be cut or
        pasted too.  Note that pasting an image or dropping an item on
        the image will delete the existing content.

        @param set: To set of clear editability.
        @type set: bool

        """
        elm_photo_editable_set(self.obj, fill)

    property editable:
        """Set editability of the photo.

        An editable photo can be dragged to or from, and can be cut or
        pasted too.  Note that pasting an image or dropping an item on
        the image will delete the existing content.

        @type: bool

        """
        def __set__(self, fill):
            elm_photo_editable_set(self.obj, fill)

    def aspect_fixed_set(self, fixed):
        """Set whether the original aspect ratio of the photo should be kept on resize.

        The original aspect ratio (width / height) of the photo is usually
        distorted to match the object's size. Enabling this option will fix
        this original aspect, and the way that the photo is fit into
        the object's area

        @see: L{aspect_fixed_get()}

        @param fixed: C{True} if the photo should fix the aspect,
            C{False} otherwise.
        @type fixed: bool

        """
        elm_photo_aspect_fixed_set(self.obj, fixed)

    def aspect_fixed_get(self):
        """Get if the object keeps the original aspect ratio.

        @return: C{True} if the object keeps the original aspect, C{False}
            otherwise.
        @rtype: bool

        """
        return elm_photo_aspect_fixed_get(self.obj)

    property aspect_fixed:
        """Whether the original aspect ratio of the photo should be kept on resize.

        The original aspect ratio (width / height) of the photo is usually
        distorted to match the object's size. Enabling this option will fix
        this original aspect, and the way that the photo is fit into
        the object's area

        @type: bool

        """
        def __get__(self):
            return elm_photo_aspect_fixed_get(self.obj)

        def __set__(self, fixed):
            elm_photo_aspect_fixed_set(self.obj, fixed)

    def callback_clicked_add(self, func, *args, **kwargs):
        """This is called when a user has clicked the photo."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_drag_start_add(self, func, *args, **kwargs):
        """One has started dragging the inner image out of the photo's
        frame.

        """
        self._callback_add("drag,start", func, *args, **kwargs)

    def callback_drag_start_del(self, func):
        self._callback_del("drag,start", func)

    def callback_drag_end_add(self, func, *args, **kwargs):
        """One has dropped the dragged image somewhere."""
        self._callback_add("drag,end", func, *args, **kwargs)

    def callback_drag_end_del(self, func):
        self._callback_del("drag,end", func)

_elm_widget_type_register("photo", Photo)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPhoto_Type # hack to install metaclass
_install_metaclass(&PyElementaryPhoto_Type, ElementaryObjectMeta)
