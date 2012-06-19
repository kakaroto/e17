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

cdef public class InnerWindow(LayoutClass) [object PyElementaryInnerWindow, type PyElementaryInnerWindow_Type]:

    """An inwin is a window inside a window that is useful for a quick popup.
    It does not hover.

    It works by creating an object that will occupy the entire window, so it
    must be created using an L{Window} as parent only. The inwin
    object can be hidden or restacked below every other object if it's
    needed to show what's behind it without destroying it. If this is done,
    the L{activate()} function can be used to bring it back to
    full visibility again.

    There are three styles available in the default theme. These are:
        - default: The inwin is sized to take over most of the window it's
            placed in.
        - minimal: The size of the inwin will be the minimum necessary to show
            its contents.
        - minimal_vertical: Horizontally, the inwin takes as much space as
            possible, but it's sized vertically the most it needs to fit its
            contents.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_win_inwin_add(parent.obj))

    def activate(self):
        """Activates an inwin object, ensuring its visibility

        This function will make sure that the inwin is completely visible
        by calling L{show()} and L{_raise()} on it, to bring it
        to the front. It also sets the keyboard focus to it, which will be passed
        onto its content.

        The object's theme will also receive the signal "elm,action,show" with
        source "elm".

        """
        elm_win_inwin_activate(self.obj)

    def content_set(self, evasObject content):
        """Set the content of an inwin object.

        Once the content object is set, a previously set one will be deleted.
        If you want to keep that old content object, use the
        L{content_unset()} function.

        @param content: The object to set as content
        @type content: L{Object}

        """
        cdef Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_win_inwin_content_set(self.obj, o)

    def content_get(self):
        """Get the content of an inwin object.

        Return the content object for this widget.

        The returned object is valid as long as the inwin is still alive and no
        other content is set on it. Deleting the object will notify the inwin
        about it and this one will be left empty.

        If you need to remove an inwin's content to be reused somewhere else,
        see L{content_unset()}.

        @return: The content that is being used
        @rtype: L{Object}

        """
        return Object_from_instance(elm_win_inwin_content_get(self.obj))

    def content_unset(self):
        """Unset the content of an inwin object.

        Unparent and return the content object which was set for this widget.

        @return: The content that was being used
        @rtype: L{Object}

        """
        return Object_from_instance(elm_win_inwin_content_unset(self.obj))

    property content:
        """The content of an inwin object.

        Once the content object is set, a previously set one will be deleted.

        @type: L{Object}

        """
        def __get__(self):
            return Object_from_instance(elm_win_inwin_content_get(self.obj))

        def __set__(self, evasObject content):
            cdef Evas_Object *o
            if content is not None:
                o = content.obj
            else:
                o = NULL
            elm_win_inwin_content_set(self.obj, o)

        def __del__(self):
            elm_win_inwin_content_unset(self.obj)

_elm_widget_type_register("inwin", InnerWindow)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryInnerWindow_Type # hack to install metaclass
_install_metaclass(&PyElementaryInnerWindow_Type, ElementaryObjectMeta)
