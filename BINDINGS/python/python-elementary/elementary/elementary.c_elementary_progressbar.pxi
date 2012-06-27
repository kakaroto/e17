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

cdef public class Progressbar(LayoutClass) [object PyElementaryProgressbar, type PyElementaryProgressbar_Type]:

    """The progress bar is a widget for visually representing the progress
    status of a given job/task.

    A progress bar may be horizontal or vertical. It may display an icon
    besides it, as well as primary and B{units} labels. The former is meant
    to label the widget as a whole, while the latter, which is formatted
    with floating point values (and thus accepts a C{printf}-style format
    string, like C{"%1.2f units"}), is meant to label the widget's B{progress
    value}. Label, icon and unit strings/objects are B{optional} for
    progress bars.

    A progress bar may be B{inverted}, in which case it gets its values
    inverted, i.e., high values being on the left or top and low values on
    the right or bottom, for horizontal and vertical modes respectively.

    The B{span} of the progress, as set by L{span_size_set()}, is its length
    (horizontally or vertically), unless one puts size hints on the widget
    to expand on desired directions, by any container. That length will be
    scaled by the object or applications scaling factor. Applications can
    query the progress bar for its value with L{value_get()}.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"changed"} - when the value is changed

    This widget has the following styles:
        - C{"default"}
        - C{"wheel"} (simple style, no text, no progression, only "pulse"
            effect is available)

    Default text parts of the progressbar widget that you can use for are:
        - "default" - Label of the progressbar

    Default content parts of the progressbar widget that you can use for are:
        - "icon" - An icon of the progressbar

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_progressbar_add(parent.obj))

    def pulse_set(self, pulse):
        """Set whether a given progress bar widget is at "pulsing mode" or not.

        By default, progress bars will display values from the low to high
        value boundaries. There are, though, contexts in which the progress
        of a given task is B{unknown}.  For such cases, one can set a
        progress bar widget to a "pulsing state", to give the user an idea
        that some computation is being held, but without exact progress
        values. In the default theme, it will animate its bar with the
        contents filling in constantly and back to non-filled, in a loop. To
        start and stop this pulsing animation, one has to explicitly call
        L{pulse()}.

        @see: L{pulse_get()}
        @see: L{pulse()}

        @param pulse: C{True} to put the object in pulsing mode,
            C{False} to put it back to its default one
        @type pulse: bool

        """
        elm_progressbar_pulse_set(self.obj, pulse)

    def pulse_get(self):
        """Get whether a given progress bar widget is at "pulsing mode" or not.

        @return: C{True}, if the object is in pulsing mode, C{False}
            if it's in the default one (and on errors)
        @rtype: bool

        """
        return elm_progressbar_pulse_get(self.obj)

    def pulse(self, state):
        """Start/stop a given progress bar "pulsing" animation, if its
        under that mode

        @note: This call won't do anything if the object is not under "pulsing mode".

        @see: L{pulse_set()} for more details.

        @param state: C{True}, to B{start} the pulsing animation, C{False} to
            B{stop} it
        @type state: bool

        """
        elm_progressbar_pulse(self.obj, state)

    def value_set(self, value):
        """Set the progress value (in percentage) on a given progress bar
        widget.

        Use this call to set progress bar levels.

        @note: If you pass a value out of the specified range for C{val},
            it will be interpreted as the B{closest} of the B{boundary}
            values in the range.

        @param val: The progress value (B{must} be between C{0.0} and C{1.0})
        @type val: float

        """
        elm_progressbar_value_set(self.obj, value)

    def value_get(self):
        """Get the progress value (in percentage) on a given progress bar
        widget.

        @see: L{value_set()} for more details

        @return: The value of the progressbar
        @rtype: float

        """
        return elm_progressbar_value_get(self.obj)

    property value:
        """The progress value (in percentage) on a given progress bar widget.

        The progress value (B{must} be between C{0.0} and C{1.0})

        @note: If you set a value out of the specified range for C{val},
            it will be interpreted as the B{closest} of the B{boundary}
            values in the range.

        @type: float

        """
        def __get__(self):
            return elm_progressbar_value_get(self.obj)

        def __set__(self, value):
            elm_progressbar_value_set(self.obj, value)

    def span_size_set(self, size):
        """Set the (exact) length of the bar region of a given progress bar
        widget.

        This sets the minimum width (when in horizontal mode) or height
        (when in vertical mode) of the actual bar area of the progress bar.
        This in turn affects the object's minimum size. Use this when you're
        not setting other size hints expanding on the given direction (like
        weight and alignment hints) and you would like it to have a specific
        size.

        @note: Icon, label and unit text around the object will require their
            own space, which will make the object to actually require more size.

        @see: L{span_size_get()}

        @param size: The length of the progress bar's bar region
        @type size: Evas_Coord (int)

        """
        elm_progressbar_span_size_set(self.obj, size)

    def span_size_get(self):
        """Get the length set for the bar region of a given progress bar
        widget.

        If that size was not set previously, with L{span_size_set()}, this
        call will return C{0}.

        @return: The length of the progress bar's bar region
        @rtype: Evas_Coord (int)

        """
        return elm_progressbar_span_size_get(self.obj)

    property span_size:
        """The (exact) length of the bar region of a given progress bar widget.

        This property is the minimum width (when in horizontal mode) or height
        (when in vertical mode) of the actual bar area of the progress bar.
        This in turn affects the object's minimum size. Use this when you're
        not setting other size hints expanding on the given direction (like
        weight and alignment hints) and you would like it to have a specific
        size.

        @note: Icon, label and unit text around the object will require their
            own space, which will make the object to actually require more size.

        @type: Evas_Coord (int)

        """
        def __get__(self):
            return elm_progressbar_span_size_get(self.obj)

        def __set__(self, size):
            elm_progressbar_span_size_set(self.obj, size)

    def unit_format_set(self, format):
        """Set the format string for a given progress bar widget's units
        label.

        If C{None} is passed on C{format}, it will make the objects units
        area to be hidden completely. If not, it'll set the B{format string}
        for the units label's B{text}. The units label is provided a
        floating point value, so the units text is up display at most one
        floating point value. Note that the units label is optional. Use a
        format string such as "%1.2f meters" for example.

        @note: The default format string for a progress bar is an integer
            percentage, as in C{"%.0f %%"}.

        @see: L{unit_format_get()}

        @param format: The format string for objects units label
        @type format: string

        """
        if format is None:
            elm_progressbar_unit_format_set(self.obj, NULL)
        else:
            elm_progressbar_unit_format_set(self.obj, _cfruni(format))

    def unit_format_get(self):
        """Retrieve the format string set for a given progress bar widget's
        units label.

        @see: L{unit_format_set()} for more details

        @return: The format set string for the objects units label or
            C{None}, if none was set (and on errors)
        @rtype: string

        """
        return _ctouni(elm_progressbar_unit_format_get(self.obj))

    property unit_format:
        """The format string for a given progress bar widget's units label.

        If this is set to C{None}, it will make the objects units area to be
        hidden completely. If not, it'll set the B{format string} for the
        units label's B{text}. The units label is provided a floating point
        value, so the units text is up display at most one floating point
        value. Note that the units label is optional. Use a format string
        such as "%1.2f meters" for example.

        @note: The default format string for a progress bar is an integer
            percentage, as in C{"%.0f %%"}.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_progressbar_unit_format_get(self.obj))

        def __set__(self, format):
            if format is None:
                elm_progressbar_unit_format_set(self.obj, NULL)
            else:
                elm_progressbar_unit_format_set(self.obj, _cfruni(format))

    property unit_format_function:
        """Set the callback function to format the unit string.

        @see: L{unit_format} for more info on how this works.

        @type: function

        """
        def __set__(self, func not None):
            pass
            #if not callable(func):
                #raise TypeError("func is not callable")
            #TODO: char * func(double value)
            #elm_progressbar_unit_format_function_set(self.obj, func, NULL)

    def horizontal_set(self, horizontal):
        """Set the orientation of a given progress bar widget.

        Use this function to change how your progress bar is to be
        disposed: vertically or horizontally.

        @see: L{horizontal_get()}

        @param horizontal: Use C{True} to make the object to be
            B{horizontal}, C{False} to make it B{vertical}.
        @type horizontal: bool

        """
        elm_progressbar_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        """Retrieve the orientation of a given progress bar widget.

        @see: L{horizontal_set()} for more details

        @return: C{True}, if the object is set to be B{horizontal},
            C{False} if it's B{vertical} (and on errors)
        @rtype: bool

        """
        return bool(elm_progressbar_horizontal_get(self.obj))

    property horizontal:
        """The orientation of a given progress bar widget.

        This property reflects how your progress bar is to be disposed:
        vertically or horizontally.

        @type: bool

        """
        def __get__(self):
            return bool(elm_progressbar_horizontal_get(self.obj))

        def __set__(self, horizontal):
            elm_progressbar_horizontal_set(self.obj, horizontal)

    def inverted_set(self, inverted):
        """Invert a given progress bar widget's displaying values order.

        A progress bar may be B{inverted}, in which state it gets its values
        inverted, with high values being on the left or top and low values
        on the right or bottom, as opposed to normally have the low values
        on the former and high values on the latter, respectively, for
        horizontal and vertical modes.

        @see: L{inverted_get()}

        @param inverted: Use C{True} to make the object inverted,
            C{False} to bring it back to default, non-inverted values.
        @type inverted: bool

        """
        elm_progressbar_inverted_set(self.obj, inverted)

    def inverted_get(self):
        """Get whether a given progress bar widget's displaying values are
        inverted or not.

        @see: L{inverted_set()} for more details

        @return: C{True}, if the object has inverted values,
            C{False} otherwise (and on errors)
        @rtype: bool

        """
        return bool(elm_progressbar_inverted_get(self.obj))

    property inverted:
        """Whether a given progress bar widget's displaying values are
        inverted or not.

        A progress bar may be B{inverted}, in which state it gets its values
        inverted, with high values being on the left or top and low values
        on the right or bottom, as opposed to normally have the low values
        on the former and high values on the latter, respectively, for
        horizontal and vertical modes.

        @type: bool

        """
        def __get__(self):
            return bool(elm_progressbar_inverted_get(self.obj))

        def __set__(self, inverted):
            elm_progressbar_inverted_set(self.obj, inverted)

    def callback_changed_add(self, func, *args, **kwargs):
        """When the value is changed."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

_elm_widget_type_register("progressbar", Progressbar)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryProgressbar_Type # hack to install metaclass
_install_metaclass(&PyElementaryProgressbar_Type, ElementaryObjectMeta)
