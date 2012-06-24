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

cdef class PopupItem(ObjectItem):

    """An item for L{Popup}.

    Default contents parts of the popup items that you can use for are:
        - "default" -Item's icon

    Default text parts of the popup items that you can use for are:
        - "default" - Item's label

    """

    def __init__(self, evasObject popup, label = None, evasObject icon = None, func = None, *args, **kwargs):
        cdef Elm_Object_Item *item
        cdef Evas_Smart_Cb cb

        if func is None:
            cb = NULL
        elif callable(func):
            cb = _object_item_callback
        else:
            raise TypeError("func is not None or callable")

        self.params = (func, args, kwargs)
        item = elm_popup_item_append(popup.obj, _cfruni(label),
                                     icon.obj if icon else NULL,
                                     cb, <void *>self)

        if item != NULL:
            self._set_obj(item)
        else:
            Py_DECREF(self)

    def __str__(self):
        return "%s(func=%s, item_data=%s)" % \
               (self.__class__.__name__,
                self.params[0],
                self.params[1])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Object_Item=%#x, "
                "item_class=%s, func=%s, item_data=%r)") % \
               (self.__class__.__name__,
                <unsigned long><void*>self,
                PY_REFCOUNT(self),
                <unsigned long>self.item,
                self.params[0],
                self.params[1])

cdef public class Popup(Object) [object PyElementaryPopup, type PyElementaryPopup_Type]:

    """This widget is an enhancement of L{Notify}.
    In addition to Content area, there are two optional sections namely Title
    area and Action area.

    Popup Widget displays its content with a particular orientation in the
    parent area. This orientation can be one among top, center, bottom,
    left, top-left, top-right, bottom-left and bottom-right. Content part of
    Popup can be an Evas Object set by application or it can be Text set by
    application or set of items containing an icon and/or text. The
    content/item-list can be removed using elm_object_content_set with second
    parameter passed as None.

    Following figures shows the textual layouts of popup in which Title Area
    and Action area area are optional ones. Action area can have up to 3
    buttons handled using elm_object common APIs mentioned below. If user
    wants to have more than 3 buttons then these buttons can be put inside
    the items of a list as content. User needs to handle the clicked signal
    of these action buttons if required. No event is processed by the widget
    automatically when clicked on these action buttons.

    Figure::
        |---------------------|    |---------------------|    |---------------------|
        |     Title Area      |    |     Title Area      |    |     Title Area      |
        |Icon|    Text        |    |Icon|    Text        |    |Icon|    Text        |
        |---------------------|    |---------------------|    |---------------------|
        |       Item 1        |    |                     |    |                     |
        |---------------------|    |                     |    |                     |
        |       Item 2        |    |                     |    |    Description      |
        |---------------------|    |       Content       |    |                     |
        |       Item 3        |    |                     |    |                     |
        |---------------------|    |                     |    |                     |
        |         .           |    |---------------------|    |---------------------|
        |         .           |    |     Action Area     |    |     Action Area     |
        |         .           |    | Btn1  |Btn2|. |Btn3 |    | Btn1  |Btn2|  |Btn3 |
        |---------------------|    |---------------------|    |---------------------|
        |       Item N        |     Content Based Layout     Description based Layout
        |---------------------|
        |     Action Area     |
        | Btn1  |Btn2|. |Btn3 |
        |---------------------|
           Item Based Layout

    Timeout can be set on expiry of which popup instance hides and sends a
    smart signal "timeout" to the user. The visible region of popup is
    surrounded by a translucent region called Blocked Event area. By
    clicking on Blocked Event area, the signal "block,clicked" is sent to
    the application. This block event area can be avoided by using API
    elm_popup_allow_events_set. When gets hidden, popup does not get
    destroyed automatically, application should destroy the popup instance
    after use. To control the maximum height of the internal scroller for
    item, we use the height of the action area which is passed by theme
    based on the number of buttons currently set to popup.

    Signals that you can add callbacks for are:
        - "timeout" - when ever popup is closed as a result of timeout.
        - "block,clicked" - when ever user taps on Blocked Event area.

    Styles available for Popup
        - "default"

    Default contents parts of the popup widget that you can use for are:
        - "default" - The content of the popup
        - "title,icon" - Title area's icon
        - "button1" - 1st button of the action area
        - "button2" - 2nd button of the action area
        - "button3" - 3rd button of the action area

    Default text parts of the popup widget that you can use for are:
        - "title,text" - This operates on Title area's label
        - "default" - content-text set in the content area of the widget

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_popup_add(parent.obj))

    def item_append(self, label = None, evasObject icon = None, func = None, *args, **kwargs):
        """item_append(label, icon, func, *args, **kwargs)

        Add a new item to a Popup object

        Both an item list and a content cannot be set at the same time!
        Once you add an item, the previous content will be removed.

        @param label: The Label of the new item
        @type label: string
        @param icon: Icon to be set on new item
        @type icon: L{Object}
        @param func: Convenience function called when item selected
        @type func: function

        @return: A handle to the item added or C{None} on errors.
        @rtype: L{PopupItem}

        @warning: When the first item is appended to popup object, any
            previous content of the content area is deleted. At a time, only
            one of content, content-text and item(s) can be there in a popup
            content area.

        """
        return PopupItem(self, label, icon, func, *args, **kwargs)

    property content_text_wrap_type:
        """Sets the wrapping type of content text packed in content
        area of popup object.

        @type: Elm_Wrap_Type

        """
        def __set__(self, wrap):
            elm_popup_content_text_wrap_type_set(self.obj, wrap)
        def __get__(self):
            return elm_popup_content_text_wrap_type_get(self.obj)

    property orient:
        """Sets the orientation of the popup in the parent region

        Sets the position in which popup will appear in its parent

        @type: Elm_Popup_Orient

        """
        def __set__(self, orient):
            elm_popup_orient_set(self.obj, orient)
        def __get__(self):
            return elm_popup_orient_get(self.obj)

    property timeout:
        """A timeout to hide popup automatically

        Setting this starts the timer controlling when the popup is hidden.
        Since calling evas_object_show() on a popup restarts the timer
        controlling when it is hidden, setting this before the popup is
        shown will in effect mean starting the timer when the popup is
        shown. Smart signal "timeout" is called afterwards which can be
        handled if needed.

        @note: Set a value <= 0.0 to disable a running timer.

        @note: If the value > 0.0 and the popup is previously visible, the
        timer will be started with this value, canceling any running timer.

        @type: float

        """
        def __set__(self, timeout):
            elm_popup_timeout_set(self.obj, timeout)
        def __get__(self):
            return elm_popup_timeout_get(self.obj)

    property allow_events:
        """Whether events should be passed to by a click outside.

        Enabling allow event will remove the Blocked event area and events will
        pass to the lower layer objects otherwise they are blocked.

        @note: The default value is False.

        @type: bool

        """
        def __set__(self, allow):
            elm_popup_allow_events_set(self.obj, allow)
        def __get__(self):
            return bool(elm_popup_allow_events_get(self.obj))

    def callback_timeout_add(self, func, *args, **kwargs):
        """When popup is closed as a result of timeout."""
        self._callback_add("timeout", func, *args, **kwargs)

    def callback_timeout_del(self, func):
        self._callback_del("timeout", func)

    def callback_block_clicked_add(self, func, *args, **kwargs):
        """When the user taps on Blocked Event area."""
        self._callback_add("block,clicked", func, *args, **kwargs)

    def callback_block_clicked_del(self, func):
        self._callback_del("block,clicked", func)

_elm_widget_type_register("popup", Popup)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPopup_Type # hack to install metaclass
_install_metaclass(&PyElementaryPopup_Type, ElementaryObjectMeta)
