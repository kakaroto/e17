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

cdef public class Dayselector(LayoutClass) [object PyElementaryDayselector, type PyElementaryDayselector_Type]:

    """Dayselector displays all seven days of the week and allows the user to
    select multiple days.

    The selection can be toggle by just clicking on the day.

    Dayselector also provides the functionality to check whether a day is
    selected or not.

    First day of the week is taken from config settings by default. It can be
    altered by using the API L{week_start} API.

    APIs are provided for setting the duration of weekend L{weekend_start}
    and L{weekend_length} does this job.

    Two styles of weekdays and weekends are supported in Dayselector.
    Application can emit signals on individual check objects for setting the
    weekday, weekend styles.

    Once the weekend start day or weekend length changes, all the weekday &
    weekend styles will be reset to default style. It's the application's
    responsibility to set the styles again by sending corresponding signals.

    "day0" indicates Sunday, "day1" indicates Monday etc. continues and so,
    "day6" indicates the Saturday part name.

    Application can change individual day display string by using the API
    L{Object.part_text_set()}.

    L{Object.part_content_set()} API sets the individual day object only if
    the passed one is a Check widget.

    Check object representing a day can be set/get by the application by using
    the elm_object_part_content_set/get APIs thus providing a way to handle
    the different check styles for individual days.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"dayselector,changed"} - when the user changes the state of a day.
        - C{"language,changed"} - the program's language changed

    Available styles for dayselector are:
        - default

    """
    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_dayselector_add(parent.obj))

    def day_selected_set(self, day, selected):
        """Set the state of given Dayselector_Day.

        @see: Elm_Dayselector_Day
        @see: L{day_selected_get()}

        @param day: The day that the user want to set state.
        @type day: Elm_Dayselector_Day
        @param selected: state of the day. C{True} is selected.
        @type selected: bool

        """
        elm_dayselector_day_selected_set(self.obj, day, selected)

    def day_selected_get(self, day):
        """Get the state of given Dayselector_Day.

        @see: Elm_Dayselector_Day
        @see: L{day_selected_set()}

        @param day: The day that the user want to know state.
        @type day: Elm_Dayselector_Day
        @return: C{True}, if the Day is selected
        @rtype: bool

        """
        return bool(elm_dayselector_day_selected_get(self.obj, day))

    property week_start:
        """The starting day of Dayselector.

        @type: Elm_Dayselector_Day

        @see: Elm_Dayselector_Day
        @see: L{week_start_get()}

        """
        def __get__(self):
            return elm_dayselector_week_start_get(self.obj)
        def __set__(self, day):
            elm_dayselector_week_start_set(self.obj, day)

    property weekend_start:
        """The weekend starting day of Dayselector.

        @type: Elm_Dayselector_Day

        @see: Elm_Dayselector_Day
        @see: L{weekend_start_get()}

        """
        def __get__(self):
            return elm_dayselector_weekend_start_get(self.obj)
        def __set__(self, day):
            elm_dayselector_weekend_start_set(self.obj, day)

    property weekend_length:
        """The weekend length of Dayselector.

        @type: int

        @see: L{weekend_length_get()}

        """
        def __get__(self):
            return elm_dayselector_weekend_length_get(self.obj)
        def __set__(self, length):
            elm_dayselector_weekend_length_set(self.obj, length)

    def callback_dayselector_changed_add(self, func, *args, **kwargs):
        """when the user changes the state of a day."""
        self._callback_add("dayselector,changed", func, *args, **kwargs)

    def callback_dayselector_changed_del(self, func):
        self._callback_del("dayselector,changed", func)

    def callback_language_changed_add(self, func, *args, **kwargs):
        """the program's language changed"""
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed", func)


_elm_widget_type_register("dayselector", Dayselector)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryDayselector_Type # hack to install metaclass
_install_metaclass(&PyElementaryDayselector_Type, ElementaryObjectMeta)
