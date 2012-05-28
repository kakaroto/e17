# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
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

cdef class Calendar(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_calendar_add(parent.obj))

    #def weekdays_names_get(self):
        #cdef const_char_ptr *weekdays = elm_calendar_weekdays_names_get(self.obj)
        #return weekdays

    #def weekdays_names_set(self, weekdays):
        #elm_calendar_weekdays_names_set(self.obj, weekdays)

    def min_max_year_set(self, min, max):
        elm_calendar_min_max_year_set(self.obj, min, max)

    def min_max_year_get(self):
        cdef int min, max
        elm_calendar_min_max_year_get(self.obj, &min, &max)
        return (min, max)

    def selected_mode_set(self, mode):
        elm_calendar_select_mode_set(self.obj, mode)

    def selected_mode_get(self):
        return elm_calendar_select_mode_get(self.obj)

    #def selected_time_set(self, selected_time):
        #elm_calendar_selected_time_set(self.obj, selected_time)

    #def selected_time_get(self):
        #cdef tm *selected_time
        #elm_calendar_selected_time_get(self.obj, selected_time)
        #return bool(selected_time)

    #def format_function_set(self, format_func):
        #elm_calendar_format_function_set(self.obj, format_func)

    #def mark_add(self, mark_type, mark_time, repeat):
        #Elm_Calendar_Mark       *elm_calendar_mark_add(self.obj, const_char_ptr mark_type, struct tm *mark_time, Elm_Calendar_Mark_Repeat_Type repeat)

    #def mark_del(self, mark):
        #elm_calendar_mark_del(mark)

    def marks_clear(self):
        elm_calendar_marks_clear(self.obj)

    #def marks_get(self):
        #const_Eina_List         *elm_calendar_marks_get(self.obj)

    def marks_draw(self):
        elm_calendar_marks_draw(self.obj)

    def interval_set(self, interval):
        elm_calendar_interval_set(self.obj, interval)

    def interval_get(self):
        return elm_calendar_interval_get(self.obj)

    def first_day_of_week_set(self, day):
        elm_calendar_first_day_of_week_set(self.obj, day)

    def first_day_of_week_get(self):
        return elm_calendar_first_day_of_week_get(self.obj)

    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)
