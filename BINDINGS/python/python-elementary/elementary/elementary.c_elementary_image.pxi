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

cdef class Image(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_image_add(parent.obj))

    def file_set(self, filename, group):
        elm_image_file_set(self.obj, filename, group)

    def smooth_set(self, smooth):
        elm_image_smooth_set(self.obj, smooth)

    def no_scale_set(self, no_scale):
        elm_image_no_scale_set(self.obj, no_scale)

    def scale_set(self, scale_up, scale_down):
        elm_image_scale_set(self.obj, scale_up, scale_down)

    def fill_outside_set(self, fill_outside):
        elm_image_fill_outside_set(self.obj, fill_outside)

    def prescale_set(self, size):
        elm_image_prescale_set(self.obj, size)

    def orient_set(self, orientation):
        elm_image_orient_set(self.obj, orientation)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)


_elm_widget_type_register("image", Image)
