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
        self._set_obj(elm_clock_add(parent.obj))
        
    def time_set(self, hours, minutes, seconds):
        elm_clock_time_set(self.obj, hours, minutes, seconds)
        
    def time_get(self):
        """
        Returns the current time of the clock

        @rtype: list of int
        """
        cdef int hrs
        cdef int min
        cdef int sec

        elm_clock_time_get(self.obj, &hrs, &min, &sec)

        return (hrs, min, sec)
        
    def edit_set(self, edit):
        elm_clock_edit_set(self.obj, edit)
        
    def show_am_pm_set(self, am_pm):
        elm_clock_show_am_pm_set(self.obj, am_pm)
        
    def show_seconds_set(self, seconds):
        elm_clock_show_seconds_set(self.obj, seconds)


