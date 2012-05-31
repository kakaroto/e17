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

def _image_callback_conv(long addr):
    cdef const_char_ptr s = <const_char_ptr>addr
    if s == NULL:
        return None
    else:
        return s

cdef public class Image(Object) [object PyElementaryImage, type PyElementaryImage_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_image_add(parent.obj))

    #def memfile_set(self, img, size, format, key):
        #return bool(elm_image_memfile_set(self.obj, img, size, format, key))

    def file_set(self, filename, group = None):
        """
        Set the path of the image for the icon

        @param filename: Filename of the image
        """
        if group == None:
            elm_image_file_set(self.obj, filename, NULL)
        else:
            elm_image_file_set(self.obj, filename, group)

    def file_get(self):
        """Get the path of the image for the icon"""
        cdef const_char_ptr filename, group
        elm_image_file_get(self.obj, &filename, &group)
        return (filename, group)

    def smooth_set(self, smooth):
        elm_image_smooth_set(self.obj, smooth)

    def smooth_get(self):
        return bool(elm_image_smooth_get(self.obj))

    property smooth:
        def __get__(self):
            return self.smooth_get()
        def __set__(self, smooth):
            self.smooth_set(smooth)

    def object_size_get(self):
        cdef int width, height
        elm_image_object_size_get(self.obj, &width, &height)
        return (width, height)

    property object_size:
        def __get__(self):
            return self.object_size_get()

    def no_scale_set(self, no_scale):
        elm_image_no_scale_set(self.obj, no_scale)

    def no_scale_get(self):
        return bool(elm_image_no_scale_get(self.obj))

    property no_scale:
        def __get__(self):
            return self.no_scale_get()
        def __set__(self, no_scale):
            self.no_scale_set(no_scale)

    def resizable_set(self, size_up, size_down):
        elm_image_resizable_set(self.obj, size_up, size_down)

    def resizable_get(self):
        cdef evas.c_evas.Eina_Bool size_up, size_down
        elm_image_resizable_get(self.obj, &size_up, &size_down)
        return (size_up, size_down)

    property resizable:
        def __get__(self):
            return self.resizable_get()
        def __set__(self, value):
            self.resizable_set(*value)

    def fill_outside_set(self, fill_outside):
        elm_image_fill_outside_set(self.obj, fill_outside)

    def fill_outside_get(self):
        return bool(elm_image_fill_outside_get(self.obj))

    property fill_outside:
        def __get__(self):
            return self.fill_outside_get()
        def __set__(self, fill_outside):
            self.fill_outside_set(fill_outside)

    def preload_disabled_set(self, disabled):
        elm_image_preload_disabled_set(self.obj, disabled)

    def prescale_set(self, size):
        elm_image_prescale_set(self.obj, size)

    def prescale_get(self):
        return elm_image_prescale_get(self.obj)

    property prescale:
        def __get__(self):
            return self.prescale_get()
        def __set__(self, prescale):
            self.prescale_set(prescale)

    def orient_set(self, orientation):
        elm_image_orient_set(self.obj, orientation)

    def orient_get(self):
        return elm_image_orient_get(self.obj)

    property orient:
        def __get__(self):
            return self.orient_get()
        def __set__(self, orient):
            self.orient_set(orient)

    def editable_set(self, editable):
        elm_image_editable_set(self.obj, editable)

    def editable_get(self):
        return bool(elm_image_editable_get(self.obj))

    property editable:
        def __get__(self):
            return self.editable_get()
        def __set__(self, editable):
            self.editable_set(editable)

    def object_get(self):
        cdef c_evas.Evas_Object *o
        o = elm_image_object_get(self.obj)
        return <Object>o

    def aspect_fixed_set(self, fixed):
        elm_image_aspect_fixed_set(self.obj, fixed)

    def aspect_fixed_get(self):
        return bool(elm_image_aspect_fixed_get(self.obj))

    property aspect_fixed:
        def __get__(self):
            return self.aspect_fixed_get()
        def __set__(self, aspect_fixed):
            self.aspect_fixed_set(aspect_fixed)

    def animated_available_get(self):
        return bool(elm_image_animated_available_get(self.obj))

    property animated_available:
        def __get__(self):
            return self.animated_available_get()

    def animated_set(self, animated):
        elm_image_animated_set(self.obj, animated)

    def animated_get(self):
        return bool(elm_image_animated_get(self.obj))

    property animated:
        def __get__(self):
            return self.animated_get()
        def __set__(self, animated):
            self.animated_set(animated)

    def animated_play_set(self, play):
        elm_image_animated_play_set(self.obj, play)

    def animated_play_get(self):
        return bool(elm_image_animated_play_get(self.obj))

    property animated_play:
        def __get__(self):
            return self.animated_play_get()
        def __set__(self, animated_play):
            self.animated_play_set(animated_play)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_drop_add(self, func, *args, **kwargs):
        self._callback_add_full("drop", _image_callback_conv, func, *args, **kwargs)

    def callback_drop_del(self, func):
        self._callback_del_full("drop", _image_callback_conv, func)

_elm_widget_type_register("image", Image)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryImage_Type # hack to install metaclass
_install_metaclass(&PyElementaryImage_Type, ElementaryObjectMeta)
