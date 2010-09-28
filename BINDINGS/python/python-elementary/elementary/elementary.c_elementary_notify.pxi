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

cdef class Notify(Object):
    """Display a window in a particular region of the application (top,
    bottom, etc.  A timeout can be set to automatically close the
    window. This is so that, after an evas_object_show() on a notify
    object, if a timeout was set on it, it will <b>automatically</b>
    get hidden after that time.
    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_notify_add(parent.obj))

    def content_set(self, c_evas.Object content):
        """ Set the content of the notify widget

        Once the content object is set, a previously set one will be deleted.
        If you want to keep that old content object, use the
        elm_notify_content_unset() function.

        @parm: B{obj} The notify object
        @parm: B{content} The content will be filled in this notify object
        """
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_notify_content_set(self.obj, o)

    def content_unset(self):
        """ Unset the content of the notify widget

        Unparent and return the content object which was set for this widget

        @parm: obj The notify object
        @return: The content that was being used
        """
        cdef c_evas.Evas_Object *o
        o = elm_notify_content_unset(self.obj)
        if o == NULL:
            return None

        cdef Object obj
        obj = <Object>c_evas.evas_object_data_get(o, "python-evas")

        return obj;

    def content_get(self):
        """ Return the content of the notify widget

        @parm: obj The notify object
        @return: The content that is being used
        """
        cdef c_evas.Evas_Object *o
        o = elm_notify_content_get(self.obj)
        if o == NULL:
            return None

        cdef Object obj
        obj = <Object>c_evas.evas_object_data_get(o, "python-evas")

        return obj;

    def parent_set(self, c_evas.Object parent):
        """ Set the notify parent

        @parm: obj The notify object
        @parm: content The new parent
        """
        cdef c_evas.Evas_Object *o
        if parent is not None:
            o = parent.obj
        else:
            o = NULL
        elm_notify_parent_set(self.obj, o)

    def orient_set(self, int orient):
        """ Set the orientation

        @parm: obj The notify object
        @parm: orient The new orientation
        """
        elm_notify_orient_set(self.obj, orient)

    def orient_get(self):
        """ Return the orientation
        @parm: obj the notify objects
        """
        return elm_notify_orient_get(self.obj)

    def timeout_set(self, double timeout):
        """ Set the time before the notify window is hidden.

        Set a value < 0 to disable a running timer.

        @parm: obj The notify object
        @parm: time The new timeout

        @note: If the value > 0 and the notify is visible, the timer will be started
        with this value, canceling any before started timer to this notify.
        """
        elm_notify_timeout_set(self.obj, timeout)

    def timeout_get(self):
        """ Return the timeout value (in seconds)
        @parm: obj the notify object
        """
        return elm_notify_timeout_get(self.obj)

    def repeat_events_set(self, repeat):
        """
        When true if the user clicks outside the window the events will be
        catch by the others widgets, else the events are block and the signal
        dismiss will be sent when the user click outside the window.

        @note: The default value is EINA_TRUE.

        @parm: obj The notify object
        @parm: repeats EINA_TRUE Events are repeats, else no
        """
        elm_notify_repeat_events_set(self.obj, repeat)

    def repeat_events_get(self):
        """
        Return true if events are repeat below the notify object

        @parm: B{obj} the notify object
        """
        return bool(elm_notify_repeat_events_get(self.obj))


_elm_widget_type_register("notify", Notify)
