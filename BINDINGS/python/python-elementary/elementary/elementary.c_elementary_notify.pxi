# Copyright (c) 2008-2009 ProFUSION embedded systems
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary. If not, see <http://www.gnu.org/licenses/>.

cdef public class Notify(Object) [object PyElementaryNotify, type PyElementaryNotify_Type]:
    """Display a container in a particular region of the parent.

    A timeout can be set to automatically hide the notify. This is so that,
    after an evas_object_show() on a notify object, if a timeout was set on it,
    it will automatically get hidden after that time.

    @group Callbacks: callback_*

    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_notify_add(parent.obj))

    def parent_set(self, c_evas.Object parent):
        """Set the notify parent.

        @param content: The new parent
        """
        cdef c_evas.Evas_Object *o
        if parent is not None:
            o = parent.obj
        else:
            o = NULL
        elm_notify_parent_set(self.obj, o)

    def parent_get(self):
        """Get the notify parent."""
        cdef c_evas.Evas_Object *o
        o = elm_notify_parent_get(self.obj)
        return evas.c_evas._Object_from_instance(<long>o)

    def orient_set(self, int orient):
        """Set the orientation.

        @param orient: The new orientation
        """
        elm_notify_orient_set(self.obj, orient)

    def orient_get(self):
        """Return the orientation."""
        return elm_notify_orient_get(self.obj)

    def timeout_set(self, double timeout):
        """Set the time before the notify window is hidden.

        Set a value < 0 to disable a running timer.

        @param time: The new timeout

        @note: If the value > 0 and the notify is visible, the timer will be started
        with this value, canceling any before started timer to this notify.
        """
        elm_notify_timeout_set(self.obj, timeout)

    def timeout_get(self):
        """Return the timeout value (in seconds)."""
        return elm_notify_timeout_get(self.obj)

    def allow_events_set(self, repeat):
        """
        When True if the user clicks outside the window the events will be
        catch by the others widgets, else the events are block and the signal
        dismiss will be sent when the user click outside the window.

        @note: The default value is True.

        @param repeat: repeats If True events are repeats, else no
        """
        elm_notify_allow_events_set(self.obj, repeat)

    def allow_events_get(self):
        """Return True if events are repeat below the notify object."""
        return bool(elm_notify_allow_events_get(self.obj))

    def callback_timeout_add(self, func, *args, **kwargs):
        """When timeout happens on notify and it's hidden."""
        self._callback_add("timeout", func, *args, **kwargs)

    def callback_timeout_del(self, func):
        self._callback_del("timeout", func)

    def callback_block_clicked_add(self, func, *args, **kwargs):
        """When a click outside of the notify happens."""
        self._callback_add("block,clicked", func, *args, **kwargs)

    def callback_block_clicked_del(self, func):
        self._callback_del("block,clicked", func)

_elm_widget_type_register("notify", Notify)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryNotify_Type # hack to install metaclass
_install_metaclass(&PyElementaryNotify_Type, ElementaryObjectMeta)
