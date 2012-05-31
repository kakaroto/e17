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

cdef public class Button(Object) [object PyElementaryButton, type PyElementaryButton_Type]:
    """This is a push-button.

    The button has the following styles available:
    default: a normal button.
    anchor: Like default, but the button fades away when the mouse is not over
    it, leaving only the text or icon.
    hoversel_vertical: Internally used by Hoversel to give a continuous look
    across its options.
    hoversel_vertical_entry: Another internal for Hoversel.
    naviframe: Internally used by Naviframe for its back button.
    colorselector: Internally used by Colorselector for its left and right
    buttons.

    Default content parts of the button widget that you can use for are:
    "icon" - An icon of the button

    Default text parts of the button widget that you can use for are:
    "default" - Label of the button

    """

    def __init__(self,c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_button_add(parent.obj))

    def autorepeat_set(self, on):
        """Turn on/off the generated autorepeat event.

        When on, keeping a button pressed will continuously emit a @c repeated
        signal until the button is released. The time it takes until it starts
        emitting the signal is given by autorepeat_initial_timeout_set(),
        and the time between each new emission by autorepeat_gap_timeout_set().
        """
        elm_button_autorepeat_set(self.obj, on)

    def autorepeat_get(self):
        """Get whether the autorepeat feature is enabled."""
        return bool(elm_button_autorepeat_get(self.obj))

    property autorepeat:
        def __get__(self):
            return self.autorepeat_get()
        def __set__(self, on):
            self.autorepeat_set(on)

    def autorepeat_initial_timeout_set(self, t):
        """Set the initial timeout before the autorepeat event is generated."""
        elm_button_autorepeat_initial_timeout_set(self.obj, t)

    def autorepeat_initial_timeout_get(self):
        """Get the initial timeout before the autorepeat event is generated."""
        return elm_button_autorepeat_initial_timeout_get(self.obj)

    property autorepeat_initial_timeout:
        def __get__(self):
            return self.autorepeat_initial_timeout_get()
        def __set__(self, t):
            self.autorepeat_initial_timeout_set(t)

    def autorepeat_gap_timeout_set(self, t):
        """Set the interval between each generated autorepeat event."""
        elm_button_autorepeat_gap_timeout_set(self.obj, t)

    def autorepeat_gap_timeout_get(self):
        """Get the interval between each generated autorepeat event."""
        return elm_button_autorepeat_gap_timeout_get(self.obj)

    property autorepeat_gap_timeout:
        def __get__(self):
            return self.autorepeat_gap_timeout_get()
        def __set__(self, t):
            self.autorepeat_gap_timeout_set(t)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_repeated_add(self, func, *args, **kwargs):
        self._callback_add("repeated", func, *args, **kwargs)

    def callback_repeated_del(self, func):
        self._callback_del("repeated", func)

    def callback_pressed_add(self, func, *args, **kwargs):
        self._callback_add("pressed", func, *args, **kwargs)

    def callback_pressed_del(self, func):
        self._callback_del("pressed", func)

    def callback_unpressed_add(self, func, *args, **kwargs):
        self._callback_add("unpressed", func, *args, **kwargs)

    def callback_unpressed_del(self, func):
        self._callback_del("unpressed", func)

_elm_widget_type_register("button", Button)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryButton_Type # hack to install metaclass
_install_metaclass(&PyElementaryButton_Type, ElementaryObjectMeta)
