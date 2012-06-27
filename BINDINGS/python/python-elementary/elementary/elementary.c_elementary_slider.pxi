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

cdef public class Slider(LayoutClass) [object PyElementarySlider, type PyElementarySlider_Type]:

    """The slider adds a draggable "slider" widget for selecting the value of
    something within a range.

    A slider can be horizontal or vertical. It can contain an Icon and has a
    primary label as well as a units label (that is formatted with floating
    point values and thus accepts a printf-style format string, like
    "%1.2f units". There is also an indicator string that may be somewhere
    else (like on the slider itself) that also accepts a format string like
    units. Label, Icon Unit and Indicator strings/objects are optional.

    A slider may be inverted which means values invert, with high vales being
    on the left or top and low values on the right or bottom (as opposed to
    normally being low on the left or top and high on the bottom and right).

    The slider should have its minimum and maximum values set by the
    application with  L{min_max_set()} and value should also be set by
    the application before use with  L{value_set()}. The span of the
    slider is its length (horizontally or vertically). This will be scaled by
    the object or applications scaling factor. At any point code can query the
    slider for its value with L{value_get()}.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"changed"} - Whenever the slider value is changed by the user.
        - C{"slider,drag,start"} - dragging the slider indicator around has
            started.
        - C{"slider,drag,stop"} - dragging the slider indicator around has
            stopped.
        - C{"delay,changed"} - A short time after the value is changed by
            the user. This will be called only when the user stops dragging
            for a very short period or when they release their finger/mouse,
            so it avoids possibly expensive reactions to the value change.

    Available styles for it:
        - C{"default"}

    Default content parts of the slider widget that you can use for are:
        - "icon" - An icon of the slider
        - "end" - A end part content of the slider

    Default text parts of the slider widget that you can use for are:
        - "default" - Label of the slider

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_slider_add(parent.obj))

    def span_size_set(self, size):
        """Set the (exact) length of the bar region of a given slider widget.

        This sets the minimum width (when in horizontal mode) or height
        (when in vertical mode) of the actual bar area of the slider. This
        in turn affects the object's minimum size. Use this when you're not
        setting other size hints expanding on the given direction (like
        weight and alignment hints) and you would like it to have a specific
        size.

        @note: Icon, end, label, indicator and unit text around the object
            will require their own space, which will make the object to
            require more the C{size}, actually.

        @see: L{span_size_get()}

        @param size: The length of the slider's bar region.
        @type size: Evas_Coord (int)

        """
        elm_slider_span_size_set(self.obj, size)

    def span_size_get(self):
        """Get the length set for the bar region of a given slider widget

        If that size was not set previously, with L{span_size_set()}, this
        call will return C{0}.

        @return: The length of the slider's bar region.
        @rtype: Evas_Coord (int)

        """
        return elm_slider_span_size_get(self.obj)

    property span_size:
        """The (exact) length of the bar region of a given slider widget.

        This property reflects the minimum width (when in horizontal mode)
        or height (when in vertical mode) of the actual bar area of the
        slider. This in turn affects the object's minimum size. Use this
        when you're not setting other size hints expanding on the given
        direction (like weight and alignment hints) and you would like it to
        have a specific size.

        @note: Icon, end, label, indicator and unit text around the object
            will require their own space, which will make the object to
            require more the C{size}, actually.

        @type: Evas_Coord (int)

        """
        def __get__(self):
            return elm_slider_span_size_get(self.obj)

        def __set__(self, size):
            elm_slider_span_size_set(self.obj, size)

    def unit_format_set(self, format):
        """Set the format string for the unit label.

        Unit label is displayed all the time, if set, after slider's bar.
        In horizontal mode, at right and in vertical mode, at bottom.

        If C{None}, unit label won't be visible. If not it sets the format
        string for the label text. To the label text is provided a floating point
        value, so the label text can display up to 1 floating point value.
        Note that this is optional.

        Use a format string such as "%1.2f meters" for example, and it will
        display values like: "3.14 meters" for a value equal to 3.14159.

        Default is unit label disabled.

        @see: L{indicator_format_get()}

        @param format: The format string for the unit display.
        @type format: string

        """
        elm_slider_unit_format_set(self.obj, _cfruni(format))

    def unit_format_get(self):
        """Get the unit label format of the slider.

        Unit label is displayed all the time, if set, after slider's bar.
        In horizontal mode, at right and in vertical mode, at bottom.

        @see: L{unit_format_set()} for more information on how this works.

        @return: The unit label format string in UTF-8.
        @rtype: string

        """
        return _ctouni(elm_slider_unit_format_get(self.obj))

    property unit_format:
        """The format string for the unit label.

        Unit label is displayed all the time, if set, after slider's bar.
        In horizontal mode, at right and in vertical mode, at bottom.

        If C{None}, unit label won't be visible. If not it sets the format
        string for the label text. To the label text is provided a floating point
        value, so the label text can display up to 1 floating point value.
        Note that this is optional.

        Use a format string such as "%1.2f meters" for example, and it will
        display values like: "3.14 meters" for a value equal to 3.14159.

        Default is unit label disabled.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_slider_unit_format_get(self.obj))

        def __set__(self, format):
            elm_slider_unit_format_set(self.obj, _cfruni(format))

    def indicator_format_set(self, format):
        """Set the format string for the indicator label.

        The slider may display its value somewhere else then unit label, for
        example, above the slider knob that is dragged around. This function
        sets the format string used for this.

        If C{None}, indicator label won't be visible. If not it sets the
        format string for the label text. To the label text is provided a
        floating point value, so the label text can display up to 1 floating
        point value. Note that this is optional.

        Use a format string such as "%1.2f meters" for example, and it will
        display values like: "3.14 meters" for a value equal to 3.14159.

        Default is indicator label disabled.

        @see: L{indicator_format_get()}

        @param indicator: The format string for the indicator display.
        @type indicator: string

        """
        elm_slider_indicator_format_set(self.obj, _cfruni(format))

    def indicator_format_get(self):
        """Get the indicator label format of the slider.

        The slider may display its value somewhere else then unit label, for
        example, above the slider knob that is dragged around. This function
        gets the format string used for this.

        @see: L{indicator_format_set()} for more information on how this works.

        @return: The indicator label format string in UTF-8.
        @rtype: string

        """
        return _ctouni(elm_slider_indicator_format_get(self.obj))

    property indicator_format:
        """The format string for the indicator label.

        The slider may display its value somewhere else then unit label, for
        example, above the slider knob that is dragged around. This function
        sets the format string used for this.

        If C{None}, indicator label won't be visible. If not it sets the
        format string for the label text. To the label text is provided a
        floating point value, so the label text can display up to 1 floating
        point value. Note that this is optional.

        Use a format string such as "%1.2f meters" for example, and it will
        display values like: "3.14 meters" for a value equal to 3.14159.

        Default is indicator label disabled.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_slider_indicator_format_get(self.obj))

        def __set__(self, format):
            elm_slider_indicator_format_set(self.obj, _cfruni(format))

    #TODO: def indicator_format_function_set(self, func, free_func)
#~         """Set the format function pointer for the indicator label
#~
#~         Set the callback function to format the indicator string.
#~
#~         @see: L{indicator_format_set()} for more info on how this works.
#~
#~         @param func: The indicator format function.
#~         @type func: function
#~         @param free_func: The freeing function for the format string.
#~         @type free_func: function
#~
#~         """
        #elm_slider_indicator_format_function_set(self.obj, char(*func)(double val), void (*free_func)(charstr))

    #TODO: def units_format_function_set(self, func, free_func)
#~         """Set the format function pointer for the units label
#~
#~         Set the callback function to format the units string.
#~
#~         @see: L{units_format_set() for more info on how this works.
#~
#~         @param func: The units format function.
#~         @type func: function
#~         @param free_func: The freeing function for the format string.
#~         @type free_func: function
#~
#~         """
        #elm_slider_units_format_function_set(self.obj, char(*func)(double val), void (*free_func)(charstr))

    def horizontal_set(self, horizontal):
        """Set the orientation of a given slider widget.

        Use this function to change how your slider is to be
        disposed: vertically or horizontally.

        By default it's displayed horizontally.

        @see: L{horizontal_get()}

        @param horizontal: Use C{True} to make the object be B{horizontal},
            C{False} to make it B{vertical}.
        @type horizontal: bool

        """
        elm_slider_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        """Retrieve the orientation of a given slider widget

        @see: L{horizontal_set()} for more details.

        @return: C{True}, if the object is set to be B{horizontal}, C{False}
            if it's B{vertical} (and on errors).
        @rtype: bool

        """
        return bool(elm_slider_horizontal_get(self.obj))

    property horizontal:
        """The orientation of a given slider widget.

        This property reflects how your slider is to be disposed: vertically
        or horizontally.

        By default it's displayed horizontally.

        @type: bool

        """
        def __get__(self):
            return bool(elm_slider_horizontal_get(self.obj))
        def __set__(self, horizontal):
            elm_slider_horizontal_set(self.obj, horizontal)

    def min_max_set(self, min, max):
        """Set the minimum and maximum values for the slider.

        Define the allowed range of values to be selected by the user.

        If actual value is less than C{min}, it will be updated to C{min}. If it
        is bigger then C{max}, will be updated to C{max}. Actual value can be
        get with L{value_get()}.

        By default, min is equal to 0.0, and max is equal to 1.0.

        @warning: Maximum must be greater than minimum, otherwise behavior
            is undefined.

        @see: L{min_max_get()}

        @param min: The minimum value.
        @type min: float
        @param max: The maximum value.
        @type max: float

        """
        elm_slider_min_max_set(self.obj, min, max)

    def min_max_get(self):
        """Get the minimum and maximum values of the slider.

        @note: If only one value is needed, the other pointer can be passed
        as C{None}.

        @see: L{min_max_set()} for details.

        @return: The minimum and maximum values
        @rtype: tuple of floats

        """
        cdef double min, max
        elm_slider_min_max_get(self.obj, &min, &max)
        return (min, max)

    property min_max:
        """The minimum and maximum values for the slider.

        If actual value is less than C{min}, it will be updated to C{min}. If it
        is bigger then C{max}, will be updated to C{max}. Actual value can be
        get with L{value_get()}.

        By default, min is equal to 0.0, and max is equal to 1.0.

        @warning: Maximum must be greater than minimum, otherwise behavior
            is undefined.

        @type: (float, float)

        """
        def __get__(self):
            cdef double min, max
            elm_slider_min_max_get(self.obj, &min, &max)
            return (min, max)

        def __set__(self, value):
            min, max = value
            elm_slider_min_max_set(self.obj, min, max)

    def value_set(self, value):
        """Set the value the slider displays.

        Value will be presented on the unit label following format specified
        with L{unit_format_set()} and on indicator with
        L{indicator_format_set()}.

        @warning: The value must to be between min and max values. These
            values are set by L{min_max_set()}.

        @see: L{value_get()}
        @see: L{unit_format_set()}
        @see: L{indicator_format_set()}
        @see: L{min_max_set()}

        @param val: The value to be displayed.
        @type val: float

        """
        elm_slider_value_set(self.obj, value)

    def value_get(self):
        """Get the value displayed by the spinner.

        @see: L{value_set()} for details.

        @return: The value displayed.
        @rtype: float

        """
        return elm_slider_value_get(self.obj)

    property value:
        """The value displayed in the slider.

        Value will be presented on the unit label following format specified
        with L{unit_format_set()} and on indicator with
        L{indicator_format_set()}.

        @warning: The value must to be between min and max values. These
            values are set by L{min_max}.

        @see: L{unit_format}
        @see: L{indicator_format}
        @see: L{min_max}

        @type: float

        """
        def __get__(self):
            return elm_slider_value_get(self.obj)
        def __set__(self, value):
            elm_slider_value_set(self.obj, value)

    def inverted_set(self, inverted):
        """Invert a given slider widget's displaying values order

        A slider may be B{inverted}, in which state it gets its
        values inverted, with high vales being on the left or top and
        low values on the right or bottom, as opposed to normally have
        the low values on the former and high values on the latter,
        respectively, for horizontal and vertical modes.

        @see: L{inverted_get()}

        @param inverted: Use C{True} to make the object inverted, C{False} to
            bring it back to default, non-inverted values.
        @type inverted: bool

        """
        elm_slider_inverted_set(self.obj, inverted)

    def inverted_get(self):
        """Get whether a given slider widget's displaying values are
        inverted or not.

        @see: L{inverted_set()} for more details.

        @return: C{True}, if the object has inverted values, C{False}
            otherwise (and on errors).
        @rtype: bool

        """
        return bool(elm_slider_inverted_get(self.obj))

    property inverted:
        """Invert a given slider widget's displaying values order

        A slider may be B{inverted}, in which state it gets its
        values inverted, with high vales being on the left or top and
        low values on the right or bottom, as opposed to normally have
        the low values on the former and high values on the latter,
        respectively, for horizontal and vertical modes.

        @type: bool

        """
        def __get__(self):
            return bool(elm_slider_inverted_get(self.obj))

        def __set__(self, inverted):
            elm_slider_inverted_set(self.obj, inverted)

    def indicator_show_set(self, show):
        """Set whether to enlarge slider indicator (augmented knob) or not.

        By default, indicator will be bigger while dragged by the user.

        @warning: It won't display values set with
            L{indicator_format_set()} if you disable indicator.

        @param show: C{True} will make it enlarge, C{False} will let the
            knob always at default size.
        @type show: bool

        """
        elm_slider_indicator_show_set(self.obj, show)

    def indicator_show_get(self):
        """Get whether a given slider widget's enlarging indicator or not.

        @see: L{indicator_show_set()} for details.

        @return: C{True}, if the object is enlarging indicator, or C{False}
            otherwise (and on errors).
        @rtype: bool

        """
        return bool(elm_slider_indicator_show_get(self.obj))

    property indicator_show:
        """Whether to enlarge slider indicator (augmented knob) or not.

        By default, indicator will be bigger while dragged by the user.

        @warning: It won't display values set with
            L{indicator_format} if you disable indicator.

        @type: bool

        """
        def __get__(self):
            return bool(elm_slider_indicator_show_get(self.obj))

        def __set__(self, show):
            elm_slider_indicator_show_set(self.obj, show)

    def callback_changed_add(self, func, *args, **kwargs):
        """Whenever the slider value is changed by the user."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_slider_drag_start_add(self, func, *args, **kwargs):
        """Dragging the slider indicator around has started."""
        self._callback_add("slider,drag,start", func, *args, **kwargs)

    def callback_slider_drag_start_del(self, func):
        self._callback_del("slider,drag,start", func)

    def callback_slider_drag_stop_add(self, func, *args, **kwargs):
        """Dragging the slider indicator around has stopped."""
        self._callback_add("slider,drag,stop", func, *args, **kwargs)

    def callback_slider_drag_stop_del(self, func):
        self._callback_del("slider,drag,stop", func)

    def callback_delay_changed_add(self, func, *args, **kwargs):
        """A short time after the value is changed by the user. This will be
        called only when the user stops dragging for a very short period or
        when they release their finger/mouse, so it avoids possibly
        expensive reactions to the value change.

        """
        self._callback_add("delay,changed", func, *args, **kwargs)

    def callback_delay_changed_del(self, func):
        self._callback_del("delay,changed", func)

_elm_widget_type_register("slider", Slider)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementarySlider_Type # hack to install metaclass
_install_metaclass(&PyElementarySlider_Type, ElementaryObjectMeta)
