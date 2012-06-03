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

cdef class CalendarMark(object):
    #cdef Elm_Calendar_Mark *obj

    def __init__(self, mark_type, mark_time, repeat):
        """@see: L{Calendar.mark_add()}"""
        #self.obj = elm_calendar_mark_add(self.obj, const_char_ptr mark_type, struct tm *mark_time, Elm_Calendar_Mark_Repeat_Type repeat)
        pass

    #def delete(self, mark):
        """Delete a mark from the calendar.

        If deleting all calendar marks is required, L{marks_clear()}
        should be used instead of getting marks list and deleting each one.

        @see: L{mark_add()}

        @param mark: The mark to be deleted.
        @type mark: L{CalendarMark}

        """
        #elm_calendar_mark_del(mark)

cdef public class Calendar(Object) [object PyElementaryCalendar, type PyElementaryCalendar_Type]:

    """This is a calendar widget.

    It helps applications to flexibly display a calender with day of the week,
    date, year and month. Applications are able to set specific dates to be
    reported back, when selected, in the smart callbacks of the calendar widget.
    The API of this widget lets the applications perform other functions, like:
      - placing marks on specific dates
      - setting the bounds for the calendar (minimum and maximum years)
      - setting the day names of the week (e.g. "Thu" or "Thursday")
      - setting the year and month format.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
      - C{changed} - emitted when the date in the calendar is changed.

    @group Callbacks: callback_*

    """

    def __init__(self, c_evas.Object parent):
        """Add a new calendar widget to the given parent Elementary (container)
        object.

        This function inserts a new calendar widget on the canvas.

        @param parent: The parent object.
        @type parent: L{Object}
        @return: a new calendar widget handle or C{None}, on errors.
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_calendar_add(parent.obj))

    #def weekdays_names_get(self):
        """Get weekdays names displayed by the calendar.

        By default, weekdays abbreviations get from system are displayed:
        E.g. for an en_US locale: "Sun, Mon, Tue, Wed, Thu, Fri, Sat"
        The first string is related to Sunday, the second to Monday...

        @see: L{weekdays_name_set()}

        @return: Array of seven strings to be used as weekday names.
        @rtype: tuple of strings

        """
        #cdef const_char_ptr *weekdays = elm_calendar_weekdays_names_get(self.obj)
        #return weekdays

    #def weekdays_names_set(self, weekdays):
        """Set weekdays names to be displayed by the calendar.

        By default, weekdays abbreviations get from system are displayed:
        E.g. for an en_US locale: "Sun, Mon, Tue, Wed, Thu, Fri, Sat"

        The first string should be related to Sunday, the second to Monday...

        The usage should be like this:
        C{
        const char *weekdays[] =
        {
          "Sunday", "Monday", "Tuesday", "Wednesday",
          "Thursday", "Friday", "Saturday"
        };
        elm_calendar_weekdays_names_set(calendar, weekdays);
        }

        @see: L{weekdays_name_get()}

        @param weekdays: A tuple of seven strings to be used as weekday names.
        @type weekdays: tuple of strings
        @warning: It must have 7 elements, or it will access invalid memory.

        """
        #elm_calendar_weekdays_names_set(self.obj, weekdays)

    def min_max_year_set(self, min, max):
        """Set the minimum and maximum values for the year

        Maximum must be greater than minimum, except if you don't want to set
        maximum year.
        Default values are 1902 and -1.

        If the maximum year is a negative value, it will be limited depending
        on the platform architecture (year 2037 for 32 bits)

        @see: L{min_max_year_get()}

        @param min: The minimum year, greater than 1901
        @type min: int
        @param max: The maximum year
        @type max: int

        """
        elm_calendar_min_max_year_set(self.obj, min, max)

    def min_max_year_get(self):
        """Get the minimum and maximum values for the year

        Default values are 1902 and -1.

        @see: L{min_max_year_get()} for more details.

        @return: The minimum and maximum year.
        @rtype: tuple of ints

        """
        cdef int min, max
        elm_calendar_min_max_year_get(self.obj, &min, &max)
        return (min, max)

    def selected_mode_set(self, mode):
        """Set the day selection mode used.

        @param mode: The select mode to use.
        @type mode: Elm_Calendar_Select_Mode

        """
        elm_calendar_select_mode_set(self.obj, mode)

    def selected_mode_get(self):
        """Get the day selection mode used.

        @see: L{select_mode_set()} for more details

        @return: the selected mode
        @rtype: Elm_Calendar_Select_Mode

        """
        return elm_calendar_select_mode_get(self.obj)

    #def selected_time_set(self, selected_time):
        """Set selected date to be highlighted on calendar.

        Set the selected date, changing the displayed month if needed.
        Selected date changes when the user goes to next/previous month or
        select a day pressing over it on calendar.

        @see: L{selected_time_get()}

        @param selected_time: A B{tm} struct to represent the selected date.
        @type selected_time: tm struct

        """
        #elm_calendar_selected_time_set(self.obj, selected_time)

    #def selected_time_get(self):
        """Get selected date.

        Get date selected by the user or set by function L{selected_time_set()}.
        Selected date changes when the user goes to next/previous month or
        select a day pressing over it on calendar.

        @see: L{selected_time_get()}

        @return: A B{tm} struct to represent the selected date.
        @rtype: tm struct

        """
        #cdef tm *selected_time
        #ret = elm_calendar_selected_time_get(self.obj, &selected_time)
        #if ret == EINA_FALSE:
        #    raise ValueError
        #return selected_time

    #def format_function_set(self, format_func):
        """Set a function to format the string that will be used to display
        month and year.

        By default it uses strftime with "%B %Y" format string.
        It should allocate the memory that will be used by the string,
        that will be freed by the widget after usage.
        A pointer to the string and a pointer to the time struct will be provided.

        Example:
        C{
        static char *
        _format_month_year(struct tmselected_time)
        {
            char buf[32];
            if (!strftime(buf, sizeof(buf), "%B %Y", selected_time)) return NULL;
            return strdup(buf);
        }

        elm_calendar_format_function_set(calendar, _format_month_year);
        }

        @param format_func: Function to set the month-year string given
            the selected date
        @type format_func: function

        """
        #elm_calendar_format_function_set(self.obj, format_func)

    #def mark_add(self, mark_type, mark_time, repeat):
        """Add a new mark to the calendar

        Add a mark that will be drawn in the calendar respecting the insertion
        time and periodicity. It will emit the type as signal to the widget theme.
        Default theme supports "holiday" and "checked", but it can be extended.

        It won't immediately update the calendar, drawing the marks.
        For this, call L{marks_draw()}. However, when user selects
        next or previous month calendar forces marks drawn.

        Marks created with this method can be deleted with L{mark_del()}.

        Example:
        C{
        struct tm selected_time;
        time_t current_time;

        current_time = time(NULL) + 5 84600;
        localtime_r(&current_time, &selected_time);
        elm_calendar_mark_add(cal, "holiday", selected_time,
         ELM_CALENDAR_ANNUALLY);

        current_time = time(NULL) + 1 84600;
        localtime_r(&current_time, &selected_time);
        elm_calendar_mark_add(cal, "checked", selected_time, ELM_CALENDAR_UNIQUE);

        elm_calendar_marks_draw(cal);
        }

        @see: L{marks_draw()}
        @see L{mark_del()}

        @param mark_type: A string used to define the type of mark. It will be
            emitted to the theme, that should display a related modification on these
            days representation.
        @type mark_type: string
        @param mark_time: A time struct to represent the date of inclusion of the
            mark. For marks that repeats it will just be displayed after the inclusion
            date in the calendar.
        @type mark_time: tm struct
        @param repeat: Repeat the event following this periodicity. Can be a unique
            mark (that don't repeat), daily, weekly, monthly or annually.
        @type repeat: Elm_Calendar_Mark_Repeat_Type
        @return: The created mark or C{None} upon failure.
        @rtype: L{CalendarMark}

        """
        #return CalendarMark(mark_type, mark_time, repeat)

    def marks_clear(self):
        """Remove all calendar's marks

        @see: L{mark_add()}
        @see: L{mark_del()}

        """
        elm_calendar_marks_clear(self.obj)

    #def marks_get(self):
        """Get a list of all the calendar marks.

        @see: L{mark_add()}
        @see: L{mark_del()}
        @see: L{marks_clear()}

        @return: A tuple of calendar marks objects, or C{None} on failure.
        @rtype: tuple of L{CalendarMark}s

        """
        #const_Eina_List         *elm_calendar_marks_get(self.obj)

    def marks_draw(self):
        """Draw calendar marks.

        Should be used after adding, removing or clearing marks.
        It will go through the entire marks list updating the calendar.
        If lots of marks will be added, add all the marks and then call
        this function.

        When the month is changed, i.e. user selects next or previous month,
        marks will be drawn.

        @see: L{mark_add()}
        @see: L{mark_del()}
        @see: L{marks_clear()}

        """
        elm_calendar_marks_draw(self.obj)

    def interval_set(self, interval):
        """Set the interval on time updates for an user mouse button hold
        on calendar widgets' month selection.

        This interval value is B{decreased} while the user holds the
        mouse pointer either selecting next or previous month.

        This helps the user to get to a given month distant from the
        current one easier/faster, as it will start to change quicker and
        quicker on mouse button holds.

        The calculation for the next change interval value, starting from
        the one set with this call, is the previous interval divided by
        1.05, so it decreases a little bit.

        The default starting interval value for automatic changes is
        B{0.85} seconds.

        @see: L{interval_get()}

        @param interval: The (first) interval value in seconds
        @type interval: double

        """
        elm_calendar_interval_set(self.obj, interval)

    def interval_get(self):
        """Get the interval on time updates for an user mouse button hold
        on calendar widgets' month selection.

        @see: L{interval_set()} for more details

        @return: The (first) interval value, in seconds, set on it
        @rtype: double

        """
        return elm_calendar_interval_get(self.obj)

    def first_day_of_week_set(self, day):
        """Set the first day of week to use on the calendar widget.

        @param day: An int which correspond to the first day of the week
            (Sunday = 0, monday = 1, ..., saturday = 6)
        @type day: int

        """
        elm_calendar_first_day_of_week_set(self.obj, day)

    def first_day_of_week_get(self):
        """Get the first day of week to use on the calendar widget.

        @see: L{first_day_of_week_set()} for more details

        @return: An int which correspond to the first day of the week
            (Sunday = 0, monday = 1, ..., saturday = 6)
        @rtype: int

        """
        return elm_calendar_first_day_of_week_get(self.obj)

    def callback_changed_add(self, func, *args, **kwargs):
        """Emitted when the date in the calendar is changed."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

_elm_widget_type_register("calendar", Calendar)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryCalendar_Type # hack to install metaclass
_install_metaclass(&PyElementaryCalendar_Type, ElementaryObjectMeta)
