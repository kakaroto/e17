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

cdef class CtxpopupItem(ObjectItem):
    def __init__(self, evasObject ctxpopup, label = None, evasObject icon = None, callback = None, *args, **kargs):
        cdef Elm_Object_Item *item
        cdef Evas_Smart_Cb cb = NULL

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _object_item_callback

        self.params = (callback, args, kargs)
        item = elm_ctxpopup_item_append(ctxpopup.obj,
                                        _cfruni(label) if label is not None else NULL,
                                        icon.obj if icon is not None else NULL,
                                        cb,
                                        <void*>self)

        if item != NULL:
            self._set_obj(item)
        else:
            Py_DECREF(self)

cdef public class Ctxpopup(Object) [object PyElementaryCtxpopup, type PyElementaryCtxpopup_Type]:

    """Context popup widget.

    A ctxpopup is a widget that, when shown, pops up a list of items. It
    automatically chooses an area inside its parent object's view to
    optimally fit into it. In the default theme, it will also point an arrow
    to it's top left position at the time one shows it. Ctxpopup items have
    a label and/or an icon. It is intended for a small number of items
    (hence the use of list, not genlist).

    Signals that you can add callbacks for are:
        - "dismissed" - the ctxpopup was dismissed

    Default content parts of the ctxpopup widget that you can use for are:
        - "default" - A content of the ctxpopup

    Default content parts of the ctxpopup items that you can use for are:
        - "icon" - An icon in the title area

    Default text parts of the ctxpopup items that you can use for are:
        - "default" - Title label in the title area

    @note: Ctxpopup is a specialization of L{Hover}.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_ctxpopup_add(parent.obj))

    def hover_parent_set(self, evasObject parent):
        """Set the Ctxpopup's parent

        Set the parent object.

        @see: L{Hover.parent_set()}

        @param parent: The parent to use
        @type parent: L{Object}

        """
        elm_ctxpopup_hover_parent_set(self.obj, parent.obj)

    def hover_parent_get(self):
        """Get the Ctxpopup's parent

        @see: L{hover_parent_set()} for more information

        @return: The hover parent
        @rtype: L{Object}

        """
        return Object_from_instance(elm_ctxpopup_hover_parent_get(self.obj))

    property parent:
        """Ctxpopup's parent

        @type: L{Object}

        """
        def __get__(self):
            return Object_from_instance(elm_ctxpopup_hover_parent_get(self.obj))

        def __set__(self, evasObject parent):
            elm_ctxpopup_hover_parent_set(self.obj, parent.obj)

    def clear(self):
        """Clear all items in the given ctxpopup object."""
        elm_ctxpopup_clear(self.obj)

    def horizontal_set(self, horizontal):
        """Change the ctxpopup's orientation to horizontal or vertical.

        @param horizontal: C{True} for horizontal mode, C{False} for vertical
        @type horizontal: bool

        """
        elm_ctxpopup_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        """Get the value of current ctxpopup object's orientation.

        @see: L{horizontal_set()}

        @return: C{True} for horizontal mode, C{False} for vertical mode (or errors)
        @rtype: bool

        """
        return bool(elm_ctxpopup_horizontal_get(self.obj))

    property horizontal:
        """Ctxpopup objects orientation.

        @type: bool

        """
        def __get__(self):
            return bool(elm_ctxpopup_horizontal_get(self.obj))

        def __set__(self, horizontal):
            elm_ctxpopup_horizontal_set(self.obj, horizontal)

    def item_append(self, label, evasObject icon = None, func = None, *args, **kwargs):
        """Add a new item to a ctxpopup object.

        @warning: Ctxpopup can't hold both an item list and a content at the
        same time. When an item is added, any previous content will be
        removed.

        @see: L{Object.content_set()}

        @param icon: Icon to be set on new item
        @type icon: L{Object}
        @param label: The Label of the new item
        @type label: string
        @param func: Convenience function called when item selected
        @type func: function
        @return: The item added or C{None}, on errors
        @rtype: L{CtxpopupItem}

        """
        return CtxpopupItem(self, label, icon, func, *args, **kwargs)

    def direction_priority_set(self, first, second, third, fourth):
        """Set the direction priority order of a ctxpopup.

        This functions gives a chance to user to set the priority of ctxpopup
        showing direction. This doesn't guarantee the ctxpopup will appear
        in the requested direction.

        @param first: 1st priority of direction
        @type first: Elm_Ctxpopup_Direction
        @param second: 2nd priority of direction
        @type second: Elm_Ctxpopup_Direction
        @param third: 3th priority of direction
        @type third: Elm_Ctxpopup_Direction
        @param fourth: 4th priority of direction
        @type fourth: Elm_Ctxpopup_Direction

        """
        elm_ctxpopup_direction_priority_set(self.obj, first, second, third, fourth)

    def direction_priority_get(self):
        """Get the direction priority order of a ctxpopup.

        @see: L{direction_priority_set()} for more information.

        @return: The direction priority order
        @rtype: tuple of Elm_Ctxpopup_Direction

        """
        cdef Elm_Ctxpopup_Direction first, second, third, fourth
        elm_ctxpopup_direction_priority_get(self.obj, &first, &second, &third, &fourth)
        return (first, second, third, fourth)

    property direction_priority:
        """The direction priority order of a ctxpopup.

        This functions gives a chance to user to set the priority of ctxpopup
        showing direction. This doesn't guarantee the ctxpopup will appear
        in the requested direction.

        @type: tuple of Elm_Ctxpopup_Direction

        """
        def __get__(self):
            cdef Elm_Ctxpopup_Direction first, second, third, fourth
            elm_ctxpopup_direction_priority_get(self.obj, &first, &second, &third, &fourth)
            return (first, second, third, fourth)

        def __set__(self, priority):
            cdef Elm_Ctxpopup_Direction first, second, third, fourth
            first, second, third, fourth = priority
            elm_ctxpopup_direction_priority_set(self.obj, first, second, third, fourth)

    def direction_get(self):
        """Get the current direction of a ctxpopup.

        @warning: Only once the ctxpopup is shown can the direction be
            determined

        @return: current direction of a ctxpopup
        @rtype: Elm_Ctxpopup_Direction

        """
        return elm_ctxpopup_direction_get(self.obj)

    property direction:
        """Get the current direction of a ctxpopup.

        @warning: Only once the ctxpopup is shown can the direction be
            determined

        @type: Elm_Ctxpopup_Direction

        """
        def __get__(self):
            return elm_ctxpopup_direction_get(self.obj)

    def dismiss(self):
        """Dismiss a ctxpopup object

        Use this function to simulate clicking outside the ctxpopup to
        dismiss it. In this way, the ctxpopup will be hidden and the
        "clicked" signal will be emitted.

        """
        elm_ctxpopup_dismiss(self.obj)

    def callback_dismissed_add(self, func, *args, **kwargs):
        """the ctxpopup was dismissed"""
        self._callback_add("dismissed", func, *args, **kwargs)

    def callback_dismissed_del(self, func):
        self._callback_del("dismissed", func)

_elm_widget_type_register("ctxpopup", Ctxpopup)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryCtxpopup_Type # hack to install metaclass
_install_metaclass(&PyElementaryCtxpopup_Type, ElementaryObjectMeta)
