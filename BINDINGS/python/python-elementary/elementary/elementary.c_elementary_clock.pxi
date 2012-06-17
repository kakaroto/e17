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

cdef public class Clock(LayoutClass) [object PyElementaryClock, type PyElementaryClock_Type]:

    """This is a digital clock widget.

    In its default theme, it has a vintage "flipping numbers clock" appearance,
    which will animate sheets of individual algarisms individually as time goes
    by.

    A newly created clock will fetch system's time (already considering
    local time adjustments) to start with, and will tick accordingly. It may
    or may not show seconds.

    Clocks have an B{edition} mode. When in it, the sheets will display
    extra arrow indications on the top and bottom and the user may click on
    them to raise or lower the time values. After it's told to exit edition
    mode, it will keep ticking with that new time set (it keeps the
    difference from local time).

    Also, when under edition mode, user clicks on the cited arrows which are
    B{held} for some time will make the clock to flip the sheet, thus
    editing the time, continuously and automatically for the user. The
    interval between sheet flips will keep growing in time, so that it helps
    the user to reach a time which is distant from the one set.

    The time display is, by default, in military mode (24h), but an am/pm
    indicator may be optionally shown, too, when it will switch to 12h.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{changed} - the clock's user changed the time

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_clock_add(parent.obj))

    def time_set(self, hours, minutes, seconds):
        """Set a clock widget's time, programmatically

        This function updates the time that is showed by the clock
        widget.

        Values B{must} be set within the following ranges:
            - 0 - 23, for hours
            - 0 - 59, for minutes
            - 0 - 59, for seconds,
        even if the clock is not in "military" mode.

        @warning: The behavior for values set out of those ranges is
            B{undefined}.

        @param hrs: The hours to set
        @type hrs: int
        @param min: The minutes to set
        @type min: int
        @param sec: The seconds to set
        @type sec: int

        """
        elm_clock_time_set(self.obj, hours, minutes, seconds)

    def time_get(self):
        """Get a clock widget's time values

        This function gets the time set, returning it as a tuple.

        @return: Hours, minutes and seconds
        @rtype: tuple of ints

        """
        cdef int hrs, min, sec

        elm_clock_time_get(self.obj, &hrs, &min, &sec)
        return (hrs, min, sec)

    property time:
        """The clock widget's time

        This property reflects the time that is showed by the clock widget.

        Values B{must} be set within the following ranges:
            - 0 - 23, for hours
            - 0 - 59, for minutes
            - 0 - 59, for seconds,
        even if the clock is not in "military" mode.

        @warning: The behavior for values set out of those ranges is
            B{undefined}.

        @type: (int h, int m, int s)

        """
        def __get__(self):
            cdef int hrs, min, sec
            elm_clock_time_get(self.obj, &hrs, &min, &sec)
            return (hrs, min, sec)

        def __set__(self, value):
            cdef int hrs, min, sec
            hrs, min, sec = value
            elm_clock_time_set(self.obj, hrs, min, sec)

    def edit_set(self, edit):
        """Set whether a given clock widget is under B{edition mode} or
        under (default) displaying-only mode.

        This function makes a clock's time to be editable or not B{by user
        interaction}. When in edition mode, clocks B{stop} ticking, until
        one brings them back to canonical mode. The L{edit_mode_set()}
        function will influence which digits of the clock will be editable.

        @note: am/pm sheets, if being shown, will B{always} be editable
            under edition mode.

        @see: L{edit_get()}

        @param edit: C{True} to put it in edition, C{False} to put it back to
            "displaying only" mode
        @type edit: bool

        """
        elm_clock_edit_set(self.obj, edit)

    def edit_get(self, edit):
        """Retrieve whether a given clock widget is under editing mode or
        under (default) displaying-only mode.

        This function retrieves whether the clock's time can be edited or
        not by user interaction.

        @see: L{edit_set()} for more details

        @return: C{True}, if it's in edition mode, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_clock_edit_get(self.obj))

    property edit:
        """Whether a given clock widget is under B{edition mode} or under
        (default) displaying-only mode.

        This property reflects whether the clock editable or not B{by user
        interaction}. When in edition mode, clocks B{stop} ticking, until
        one brings them back to canonical mode. The L{edit_mode}
        property will influence which digits of the clock will be editable.

        @note: am/pm sheets, if being shown, will B{always} be editable
            under edition mode.

        @type: bool

        """
        def __get__(self):
            return bool(elm_clock_edit_get(self.obj))

        def __set__(self, edit):
            elm_clock_edit_set(self.obj, edit)

    def edit_mode_set(self, mode):
        """Set what digits of the given clock widget should be editable
        when in edition mode.

        @see: L{edit_mode_get()}

        @param digedit: Bit mask indicating the digits to be editable
        @type digedit: Elm_Clock_Edit_Mode

        """
        elm_clock_edit_mode_set(self.obj, mode)

    def edit_mode_get(self):
        """Retrieve what digits of the given clock widget should be
        editable when in edition mode.

        @see: L{edit_mode_set()} for more details

        @return: Bit mask indicating the digits to be editable
        @rtype: Elm_Clock_Edit_Mode

        """
        return elm_clock_edit_mode_get(self.obj)

    property edit_mode:
        """Which digits of the given clock widget should be editable when in
        edition mode.

        @type: Elm_Clock_Edit_Mode

        """
        def __get__(self):
            return elm_clock_edit_mode_get(self.obj)

        def __set__(self, mode):
            elm_clock_edit_mode_set(self.obj, mode)

    def show_am_pm_set(self, am_pm):
        """Set if the given clock widget must show hours in military or
        am/pm mode

        This function sets if the clock must show hours in military or
        am/pm mode. In some countries like Brazil the military mode
        (00-24h-format) is used, in opposition to the USA, where the
        am/pm mode is more commonly used.

        @see: L{show_am_pm_get()}

        @param am_pm: C{True} to put it in am/pm mode, C{False} to military mode
        @type am_pm: bool

        """
        elm_clock_show_am_pm_set(self.obj, am_pm)

    def show_am_pm_get(self):
        """Get if the given clock widget shows hours in military or am/pm
        mode

        @see: L{show_am_pm_set()} for more details

        @return: C{True}, if in am/pm mode, C{False} if in military
        @rtype: bool

        """
        return elm_clock_show_am_pm_get(self.obj)

    property show_am_pm:
        """Whether the given clock widget must show hours in military or
        am/pm mode

        This property reflects if the clock must show hours in military or
        am/pm mode. In some countries like Brazil the military mode
        (00-24h-format) is used, in opposition to the USA, where the
        am/pm mode is more commonly used.

        C{True}, if in am/pm mode, C{False} if in military

        @type: bool

        """
        def __get__(self):
            return elm_clock_show_am_pm_get(self.obj)

        def __set__(self, am_pm):
            elm_clock_show_am_pm_set(self.obj, am_pm)

    def show_seconds_set(self, seconds):
        """Set if the given clock widget must show time with seconds or not

        This function sets if the given clock must show or not elapsed
        seconds. By default, they are B{not} shown.

        @see: L{show_seconds_get()}

        @param seconds: C{True} to show seconds, C{False} otherwise
        @type seconds: bool

        """
        elm_clock_show_seconds_set(self.obj, seconds)

    def show_seconds_get(self):
        """Get whether the given clock widget is showing time with seconds
        or not

        @see: elm_clock_show_seconds_set()

        @return: C{True} if it's showing seconds, C{False} otherwise
        @rtype: bool

        """
        return elm_clock_show_seconds_get(self.obj)

    property show_seconds:
        """Whether the given clock widget must show time with seconds or not

        By default, they are B{not} shown.

        @type: bool

        """
        def __get__(self):
            return elm_clock_show_seconds_get(self.obj)

        def __set__(self, seconds):
            elm_clock_show_seconds_set(self.obj, seconds)

    def first_interval_set(self, interval):
        """Set the first interval on time updates for a user mouse button hold
        on clock widgets' time edition.

        This interval value is B{decreased} while the user holds the
        mouse pointer either incrementing or decrementing a given the
        clock digit's value.

        This helps the user to get to a given time distant from the
        current one easier/faster, as it will start to flip quicker and
        quicker on mouse button holds.

        The calculation for the next flip interval value, starting from
        the one set with this call, is the previous interval divided by
        1.05, so it decreases a little bit.

        The default starting interval value for automatic flips is
        B{0.85} seconds.

        @see: L{first_interval_get()}

        @param interval: The first interval value in seconds
        @type interval: float

        """
        elm_clock_first_interval_set(self.obj, interval)

    def first_interval_get(self):
        """Get the first interval on time updates for a user mouse button hold
        on clock widgets' time edition.

        @see: L{first_interval_set()} for more details

        @return: The first interval value, in seconds, set on it
        @rtype: float

        """
        return elm_clock_first_interval_get(self.obj)

    property first_interval:
        """The first interval on time updates for a user mouse button hold
        on clock widgets' time edition.

        This interval value is B{decreased} while the user holds the
        mouse pointer either incrementing or decrementing a given the
        clock digit's value.

        This helps the user to get to a given time distant from the
        current one easier/faster, as it will start to flip quicker and
        quicker on mouse button holds.

        The calculation for the next flip interval value, starting from
        the one set with this call, is the previous interval divided by
        1.05, so it decreases a little bit.

        The default starting interval value for automatic flips is
        B{0.85} seconds.

        @type: float

        """
        def __get__(self):
            return elm_clock_first_interval_get(self.obj)

        def __set__(self, interval):
            elm_clock_first_interval_set(self.obj, interval)

    def callback_changed_add(self, func, *args, **kwargs):
        """The clock's user changed the time"""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

_elm_widget_type_register("clock", Clock)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryClock_Type # hack to install metaclass
_install_metaclass(&PyElementaryClock_Type, ElementaryObjectMeta)
