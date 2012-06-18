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

cdef public class Radio(LayoutClass) [object PyElementaryRadio, type PyElementaryRadio_Type]:

    """Radio is a widget that allows for one or more options to be displayed
    and have the user choose only one of them.

    A radio object contains an indicator, an optional Label and an optional
    icon object. While it's possible to have a group of only one radio they,
    are normally used in groups of two or more.

    Radio objects are grouped in a slightly different, compared to other UI
    toolkits. There is no seperate group name/id to remember or manage. The
    members represent the group, there are the group. To make a group, use
    L{group_add()} and pass existing radio object and the new radio object.

    The radio object(s) will select from one of a set of integer values, so
    any value they are configuring needs to be mapped to a set of integers.
    To configure what value that radio object represents, use
    elm_radio_state_value_set() to set the integer it represents. To set the
    value the whole group(which one is currently selected) is to indicate
    use L{value_set()} on any group member, and to get the groups value use
    L{value_get()}. For convenience the radio objects are also able to
    directly set an integer(int) to the value that is selected. To specify
    the pointer to this integer to modify, use L{value_pointer_set()}. The
    radio objects will modify this directly. That implies the pointer must
    point to valid memory for as long as the radio objects exist.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - changed - This is called whenever the user changes the state of
            one of the radio objects within the group of radio objects that
            work together.

    Default text parts of the radio widget that you can use for are:
        - "default" - Label of the radio

    Default content parts of the radio widget that you can use for are:
        - "icon" - An icon of the radio

    """

    def __init__(self, evasObject parent, obj=None):
        if obj is None:
            Object.__init__(self, parent.evas)
            self._set_obj(elm_radio_add(parent.obj))
        else:
            self._set_obj(<Evas_Object*>obj)

    def group_add(self, evasObject group):
        """group_add(group)

        Add this radio to a group of other radio objects

        Radio objects work in groups. Each member should have a different integer
        value assigned. In order to have them work as a group, they need to know
        about each other. This adds the given radio object to the group of which
        the group object indicated is a member.

        @param group: Any object whose group the object is to join.
        @type group: L{Radio}

        """
        elm_radio_group_add(self.obj, group.obj)

    def state_value_set(self, value):
        """Set the integer value that this radio object represents

        This sets the value of the radio.

        @param value: The value to use if this radio object is selected
        @type value: int

        """
        elm_radio_state_value_set(self.obj, value)

    def state_value_get(self):
        """Get the integer value that this radio object represents

        This gets the value of the radio.
        @see: L{value_set()}

        @return: The value used if this radio object is selected
        @rtype: int

        """
        return elm_radio_state_value_get(self.obj)

    property state_value:
        """The integer value that this radio object represents

        @type: int

        """
        def __get__(self):
            return elm_radio_state_value_get(self.obj)
        def __set__(self, value):
            elm_radio_state_value_set(self.obj, value)

    def value_set(self, value):
        """Set the value of the radio group.

        This sets the value of the radio group and will also set the value if
        pointed to, to the value supplied, but will not call any callbacks.

        @param value: The value to use for the group
        @type value: int

        """
        elm_radio_value_set(self.obj, value)

    def value_get(self):
        """Get the value of the radio group

        @return: The integer state
        @rtype: int

        """
        return elm_radio_value_get(self.obj)

    property value:
        """The value of the radio group.

        This reflects the value of the radio group and will also set the
        value if pointed to, to the value supplied, but will not call any
        callbacks.

        @type: int

        """
        def __get__(self):
            return elm_radio_value_get(self.obj)
        def __set__(self, value):
            elm_radio_value_set(self.obj, value)

    #TODO: Check whether this actually works
    def value_pointer_set(self, value):
        """Set a convenience pointer to a integer to change when radio group
        value changes.

        This sets a pointer to a integer, that, in addition to the radio objects
        state will also be modified directly. To stop setting the object pointed
        to simply use None as the C{valuep} argument. If valuep is not None, then
        when this is called, the radio objects state will also be modified to
        reflect the value of the integer valuep points to, just like calling
        L{value_set()}.

        @param valuep: Pointer to the integer to modify
        @type valuep: int

        """
        cdef int valuep = value
        elm_radio_value_pointer_set(self.obj, &valuep)

    def selected_object_get(self):
        """Get the selected radio object.

        @return: The selected radio object
        @rtype: L{Radio}

        """
        cdef Radio r = Radio()
        cdef Evas_Object *selected = elm_radio_selected_object_get(self.obj)
        if selected == NULL:
            return None
        else:
            r.obj = selected
            return r

    property selected_object:
        """Get the selected radio object.

        @type: L{Radio}

        """
        def __get__(self):
            cdef Radio r = Radio()
            cdef Evas_Object *selected = elm_radio_selected_object_get(self.obj)
            if selected == NULL:
                return None
            else:
                r.obj = selected
                return r

    def callback_changed_add(self, func, *args, **kwargs):
        """This is called whenever the user changes the state of one of the
        radio objects within the group of radio objects that work together.

        """
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

_elm_widget_type_register("radio", Radio)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryRadio_Type # hack to install metaclass
_install_metaclass(&PyElementaryRadio_Type, ElementaryObjectMeta)
