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

cdef class Icon(Image):
    """
    A simple icon widget

    If you need a widget which presents a icon and is clickable, this widget
    is the best option for you.
    """
    def __init__(self, c_evas.Object parent):
        """
        @parm: B{parent} Parent Object
        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_icon_add(parent.obj))

    def thumb_set(self, filename, group = None):
        """
        Set the file that will be used, but use a generated thumbnail.

        @parm: B{filename} Filename of the image
        """
        if group == None:
            elm_icon_thumb_set(self.obj, filename, NULL)
        else:
            elm_icon_thumb_set(self.obj, filename, group)

    def standard_set(self, name):
        return bool(elm_icon_standard_set(self.obj, name))

    def standard_get(self):
        return elm_icon_standard_get(self.obj)

    property standard:
        def __get__(self):
            return self.standard_get()
        def __set__(self, standard):
            self.standard_set(standard)

    def order_lookup_set(self, order):
        elm_icon_order_lookup_set(self.obj, order)

    def order_lookup_get(self):
        return elm_icon_order_lookup_get(self.obj)

    property order_lookup:
        def __get__(self):
            return self.order_lookup_get()
        def __set__(self, order_lookup):
            self.order_lookup_set(order_lookup)

    def callback_thumb_done_add(self, func, *args, **kwargs):
        self._callback_add("thumb,done", func, *args, **kwargs)

    def callback_thumb_done_del(self, func):
        self._callback_del("thumb,done", func)

    def callback_thumb_error_add(self, func, *args, **kwargs):
        self._callback_add("thumb,error", func, *args, **kwargs)

    def callback_thumb_error_del(self, func):
        self._callback_del("thumb,error", func)

_elm_widget_type_register("icon", Icon)
