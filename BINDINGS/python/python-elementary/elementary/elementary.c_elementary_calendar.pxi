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

from datetime import date

cdef class CalendarMark(object):
    cdef Elm_Calendar_Mark *obj

    def __init__(self, evasObject cal, mark_type, mark_time, repeat):
        """@see: L{Calendar.mark_add()}"""
        cdef tm time
        tmtup = mark_time.timetuple()
        time.tm_mday = tmtup.tm_mday
        time.tm_mon = tmtup.tm_mon - 1
        time.tm_year = tmtup.tm_year - 1900
        time.tm_wday = tmtup.tm_wday
        time.tm_yday = tmtup.tm_yday
        time.tm_isdst = tmtup.tm_isdst
        self.obj = elm_calendar_mark_add(cal.obj, _cfruni(mark_type), &time, repeat)

    def delete(self):
        """Delete a mark from the calendar.

        If deleting all calendar marks is required, L{marks_clear()}
        should be used instead of getting marks list and deleting each one.

        @see: L{mark_add()}

        @param mark: The mark to be deleted.
        @type mark: L{CalendarMark}

        """
        elm_calendar_mark_del(self.obj)

cdef public class Calendar(LayoutClass) [object PyElementaryCalendar, type PyElementaryCalendar_Type]:

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

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_calendar_add(parent.obj))

    property weekdays_names:
        """The weekdays' names to be displayed by the calendar.

        By default, weekdays abbreviations get from system are displayed:
        E.g. for an en_US locale: "Sun, Mon, Tue, Wed, Thu, Fri, Sat"

        The first string should be related to Sunday, the second to Monday...

        The usage should be like this::
            weekdays =
            (
              "Sunday", "Monday", "Tuesday", "Wednesday",
              "Thursday", "Friday", "Saturday"
            )
            calendar.weekdays_names = weekdays

        @type: tuple of strings
        @warning: It must have 7 elements, or it will access invalid memory.

        """
        def __get__(self):
            cdef const_char_ptr *lst
            cdef const_char_ptr weekday
            ret = []
            lst = elm_calendar_weekdays_names_get(self.obj)
            for i from 0 <= i < 7:
                weekday = lst[i]
                if weekday != NULL:
                    ret.append(_ctouni(weekday))
            return ret

        def __set__(self, weekdays):
            cdef int i, day_len
            cdef char **days, *weekday
            days = <char **>PyMem_Malloc(7 * sizeof(char*))
            for i from 0 <= i < 7:
                weekday = _fruni(weekdays[i])
                day_len = len(weekday)
                days[i] = <char *>PyMem_Malloc(day_len + 1)
                memcpy(days[i], weekday, day_len + 1)
            elm_calendar_weekdays_names_set(self.obj, <const_char_ptr *>days)

    property min_max_year:
        """The minimum and maximum values for the year

        Maximum must be greater than minimum, except if you don't want to set
        maximum year.
        Default values are 1902 and -1.

        If the maximum year is a negative value, it will be limited depending
        on the platform architecture (year 2037 for 32 bits)

        @see: L{min_max_year_get()}

        @type: tuple of ints

        """
        def __get__(self):
            cdef int min, max
            elm_calendar_min_max_year_get(self.obj, &min, &max)
            return (min, max)
        def __set__(self, value):
            cdef int min, max
            min, max = value
            elm_calendar_min_max_year_set(self.obj, min, max)

    property select_mode:
        """The day selection mode used.

        @type: Elm_Calendar_Select_Mode

        """
        def __get__(self):
            return elm_calendar_select_mode_get(self.obj)
        def __set__(self, mode):
            elm_calendar_select_mode_set(self.obj, mode)

    property selected_time:
        """Selected date on the calendar.

        Setting this changes the displayed month if needed.
        Selected date changes when the user goes to next/previous month or
        select a day pressing over it on calendar.

        @type: datetime.date

        """
        def __get__(self):
            cdef tm time
            elm_calendar_selected_time_get(self.obj, &time)
            ret = date( time.tm_year + 1900,
                        time.tm_mon + 1,
                        time.tm_mday)
            return ret

        def __set__(self, selected_time):
            cdef tm time
            tmtup = selected_time.timetuple()
            time.tm_mday = tmtup.tm_mday
            time.tm_mon = tmtup.tm_mon - 1
            time.tm_year = tmtup.tm_year - 1900
            time.tm_wday = tmtup.tm_wday
            time.tm_yday = tmtup.tm_yday
            time.tm_isdst = tmtup.tm_isdst
            elm_calendar_selected_time_set(self.obj, &time)

    def format_function_set(self, format_func):
        """Set a function to format the string that will be used to display
        month and year.

        By default it uses strftime with "%B %Y" format string.
        It should allocate the memory that will be used by the string,
        that will be freed by the widget after usage.
        A pointer to the string and a pointer to the time struct will be provided.

        Example::
            static char *
            _format_month_year(struct tm selected_time)
            {
                char buf[32];
                if (!strftime(buf, sizeof(buf), "%B %Y", selected_time)) return NULL;
                return strdup(buf);
            }

            elm_calendar_format_function_set(calendar, _format_month_year);

        @param format_func: Function to set the month-year string given
            the selected date
        @type format_func: function

        """
        pass
        #elm_calendar_format_function_set(self.obj, format_func)

    def mark_add(self, mark_type, mark_time, repeat):
        """Add a new mark to the calendar

        Add a mark that will be drawn in the calendar respecting the insertion
        time and periodicity. It will emit the type as signal to the widget theme.
        Default theme supports "holiday" and "checked", but it can be extended.

        It won't immediately update the calendar, drawing the marks.
        For this, call L{marks_draw()}. However, when user selects
        next or previous month calendar forces marks drawn.

        Marks created with this method can be deleted with L{mark_del()}.

        Example::
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
        return CalendarMark(self, mark_type, mark_time, repeat)

    property marks:
        """Calendar marks.

        @type: tuple of L{CalendarMark}s

        """
        #def __get__(self):
            #const_Eina_List         *elm_calendar_marks_get(self.obj)
        #def __set__(self, value):
        def __del__(self):
            elm_calendar_marks_clear(self.obj)

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

    property interval:
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

        @type: float

        """
        def __get__(self):
            return elm_calendar_interval_get(self.obj)
        def __set__(self, interval):
            elm_calendar_interval_set(self.obj, interval)

    property first_day_of_week:
        """The first day of week to use on the calendar widget.

        @type: int

        """
        def __get__(self):
            return elm_calendar_first_day_of_week_get(self.obj)
        def __set__(self, day):
            elm_calendar_first_day_of_week_set(self.obj, day)

    def callback_changed_add(self, func, *args, **kwargs):
        """Emitted when the date in the calendar is changed."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

_elm_widget_type_register("calendar", Calendar)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryCalendar_Type # hack to install metaclass
_install_metaclass(&PyElementaryCalendar_Type, ElementaryObjectMeta)
