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
        self._set_obj(elm_image_add(parent.obj))

    def file_set(self, filename, group):
        elm_image_file_set(self.obj, filename, group)

    def smooth_set(self, smooth):
        if smooth:
            elm_image_smooth_set(self.obj, 1)
        else:
            elm_image_smooth_set(self.obj, 0)

    def no_scale_set(self, no_scale):
        if no_scale:
            elm_image_no_scale_set(self.obj, 1)
        else:
            elm_image_no_scale_set(self.obj, 0)

    def scale_set(self, scale_up, scale_down):
        cdef int su
        cdef int sd
        su = 0
        sd = 0
        if scale_up:
            su = 1
        if scale_down:
            sd = 1
        elm_image_scale_set(self.obj, su, sd)

    def fill_outside_set(self, fill_outside):
        if fill_outside:
            elm_image_fill_outside_set(self.obj, 1)
        else:
            elm_image_fill_outside_set(self.obj, 0)

    def prescale_set(self, size):
        elm_image_prescale_set(self.obj, size)

    def orient_set(self, orientation):
        elm_image_orient_set(self.obj, orientation)

    property clicked:
        def __set__(self, value):
            self._callback_add("clicked", value)
