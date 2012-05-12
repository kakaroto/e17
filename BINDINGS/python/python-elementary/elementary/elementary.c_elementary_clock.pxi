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

cdef class Clock(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_clock_add(parent.obj))

    def time_set(self, hours, minutes, seconds):
        elm_clock_time_set(self.obj, hours, minutes, seconds)

    def time_get(self):
        cdef int hrs, min, sec

        elm_clock_time_get(self.obj, &hrs, &min, &sec)
        return (hrs, min, sec)

    property time:
        def __get__(self):
            return self.time_get()

        def __set__(self, value):
            self.time_set(*value)
    
    def edit_set(self, edit):
        elm_clock_edit_set(self.obj, edit)

    def edit_get(self, edit):
        return elm_clock_edit_get(self.obj)

    property edit:
        def __get__(self):
            return self.edit_get()

        def __set__(self, value):
            self.edit_set(value)

    def edit_mode_set(self, mode):
        elm_clock_edit_mode_set(self.obj, mode)

    def edit_mode_get(self):
        return elm_clock_edit_mode_get(self.obj)

    property edit_mode:
        def __get__(self):
            return self.edit_mode_get()

        def __set__(self, value):
            self.edit_mode_set(value)

    def show_am_pm_set(self, am_pm):
        elm_clock_show_am_pm_set(self.obj, am_pm)

    def show_am_pm_get(self):
        return elm_clock_show_am_pm_get(self.obj)

    property show_am_pm:
        def __get__(self):
            return self.show_am_pm_get()
    
        def __set__(self, value):
            self.show_am_pm_set(value)

    def show_seconds_set(self, seconds):
        elm_clock_show_seconds_set(self.obj, seconds)

    def show_seconds_get(self):
        return elm_clock_show_seconds_get(self.obj)

    property show_seconds:
        def __get__(self):
            return self.show_seconds_get()
    
        def __set__(self, value):
            self.show_seconds_set(value)

    def first_interval_set(self, interval):
        elm_clock_first_interval_set(self.obj, interval)

    def first_interval_get(self):
        return elm_clock_first_interval_get(self.obj)

    property first_interval:
        def __get__(self):
            return self.first_interval_get()
    
        def __set__(self, value):
            self.first_interval_set(value)


_elm_widget_type_register("clock", Clock)
