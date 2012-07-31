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

cdef class DiskselectorItem(ObjectItem):

    """An item for the Diskselector widget."""

    def __init__(self, evasObject diskselector, label, evasObject icon=None, callback=None, *args, **kargs):
        cdef Evas_Object* icon_obj = NULL
        cdef Evas_Smart_Cb cb = NULL

        if icon is not None:
            icon_obj = icon.obj

        if callback is not None:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _object_item_callback

        self.params = (callback, args, kargs)

        item = elm_diskselector_item_append(diskselector.obj, _cfruni(label), icon_obj, cb, <void*>self)

        if item != NULL:
            self._set_obj(item)
        else:
            Py_DECREF(self)

    property selected:
        """The selected state of an item.

        This sets the selected state of the given item.
        C{True} for selected, C{False} for not selected.

        If a new item is selected the previously selected will be unselected.
        Previously selected item can be fetched from the property
        L{Diskselector.selected_item}.

        If the item is unselected, the first item of diskselector will
        be selected.

        Selected items will be visible on center position of diskselector.
        So if it was on another position before selected, or was invisible,
        diskselector will animate items until the selected item reaches center
        position.

        @see: L{Diskselector.selected_item}

        @type: bool

        """
        def __set__(self, selected):
            elm_diskselector_item_selected_set(self.item, selected)
        def __get__(self):
            return bool(elm_diskselector_item_selected_get(self.item))

    property prev:
        """Get the item before C{item} in diskselector.

        The list of items follows append order. So it will return item appended
        just before C{item} and that wasn't deleted.

        If it is the first item, C{None} will be returned.
        First item can be get by L{Diskselector.first_item}.

        @see: L{Diskselector.item_append()}
        @see: L{Diskselector.items}

        @type: DiskselectorItem

        """
        def __get__(self):
            cdef Elm_Object_Item *it = elm_diskselector_item_prev_get(self.item)
            return _object_item_to_python(it)

    property next:
        """Get the item after C{item} in diskselector.

        The list of items follows append order. So it will return item appended
        just after C{item} and that wasn't deleted.

        If it is the last item, C{None} will be returned.
        Last item can be get by elm_diskselector_last_item_get().

        @see: L{Diskselector.item_append()}
        @see: L{Diskselector.items}

        @type: DiskselectorItem

        """
        def __get__(self):
            cdef Elm_Object_Item *it = elm_diskselector_item_next_get(self.item)
            return _object_item_to_python(it)

cdef public class Diskselector(Object) [object PyElementaryDiskselector, type PyElementaryDiskselector_Type]:

    """A diskselector is a kind of list widget. It scrolls horizontally,
    and can contain label and icon objects. Three items are displayed
    with the selected one in the middle.

    It can act like a circular list with round mode and labels can be
    reduced for a defined length for side items.

    Smart callbacks one can listen to:
        - "selected" - when item is selected, i.e. scroller stops.
        - "scroll,anim,start" - scrolling animation has started
        - "scroll,anim,stop" - scrolling animation has stopped
        - "scroll,drag,start" - dragging the diskselector has started
        - "scroll,drag,stop" - dragging the diskselector has stopped

    Available styles for it:
        - C{"default"}

    Default content parts of the diskselector items that you can use for are:
        - "icon" - An icon in the diskselector item

    Default text parts of the diskselector items that you can use for are:
        - "default" - Label of the diskselector item

    @note: The "scroll,anim,*" and "scroll,drag,*" signals are only emitted
        by user intervention.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_diskselector_add(parent.obj))

    property round_enabled:
        """Enable or disable round mode.

        Disabled by default. If round mode is enabled the items list will
        work like a circular list, so when the user reaches the last item,
        the first one will popup.

        @type: bool

        """
        def __set__(self, enabled):
            elm_diskselector_round_enabled_set(self.obj, enabled)
        def __get__(self):
            return bool(elm_diskselector_round_enabled_get(self.obj))

    property side_text_max_length:
        """The side labels max length.

        Length is the number of characters of items' label that will be
        visible when it's set on side positions. It will just crop
        the string after defined size. E.g.:

        An item with label "January" would be displayed on side position as
        "Jan" if max length is set to 3, or "Janu", if this property
        is set to 4.

        When it's selected, the entire label will be displayed, except for
        width restrictions. In this case label will be cropped and "..."
        will be concatenated.

        Default side label max length is 3.

        This property will be applied over all items, included before or
        later this function call.

        @type: int

        """
        def __get__(self):
            return elm_diskselector_side_text_max_length_get(self.obj)
        def __set__(self, length):
            elm_diskselector_side_text_max_length_set(self.obj, length)

    property display_item_num:
        """The number of items to be displayed.

        Default value is 3, and also it's the minimum. If C{num} is less
        than 3, it will be set to 3.

        Also, it can be set on theme, using data item C{display_item_num}
        on group "elm/diskselector/item/X", where X is style set.
        E.g.::
            group { name: "elm/diskselector/item/X";
            data {
                item: "display_item_num" "5";
                }

        @type: int

        """
        def __set__(self, num):
            elm_diskselector_display_item_num_set(self.obj, num)
        def __get__(self):
            return elm_diskselector_display_item_num_get(self.obj)

    property bounce:
        """Bouncing behaviour when the scrolled content reaches an edge.

        Tell the internal scroller object whether it should bounce or not
        when it reaches the respective edges for each axis.

        @see: L{Scroller.bounce}

        @type: (bool h_bounce, bool v_bounce)

        """
        def __set__(self, bounce):
            h_bounce, v_bounce = bounce
            elm_scroller_bounce_set(self.obj, h_bounce, v_bounce)
        def __get__(self):
            cdef Eina_Bool h_bounce, v_bounce
            elm_scroller_bounce_get(self.obj, &h_bounce, &v_bounce)
            return (h_bounce, v_bounce)

    property scroller_policy:
        """The scrollbar policy.

        This sets the scrollbar visibility policy for the given scroller.
        ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it
        is needed, and otherwise kept hidden. ELM_SCROLLER_POLICY_ON turns
        it on all the time, and ELM_SCROLLER_POLICY_OFF always keeps it off.
        This applies respectively for the horizontal and vertical scrollbars.

        Both are disabled by default, i.e., are set to ELM_SCROLLER_POLICY_OFF.

        @type: tuple of Elm_Scroller_Policy

        """
        def __get__(self):
            cdef Elm_Scroller_Policy h_policy, v_policy
            elm_scroller_policy_get(self.obj, &h_policy, &v_policy)
            return (h_policy, v_policy)
        def __set__(self, policy):
            h_policy, v_policy = policy
            elm_scroller_policy_set(self.obj, h_policy, v_policy)

    def clear(self):
        """Remove all diskselector's items.

        @see: L{ObjectItem.delete()}
        @see: L{item_append()}

        """
        elm_diskselector_clear(self.obj)

    property items:
        """Get a list of all the diskselector items.

        @see: L{item_append()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}

        @type: list of L{DiskselectorItem}s

        """
        def __get__(self):
            cdef Elm_Object_Item *it
            cdef const_Eina_List *lst

            lst = elm_diskselector_items_get(self.obj)
            ret = []
            ret_append = ret.append
            while lst:
                it = <Elm_Object_Item *>lst.data
                lst = lst.next
                o = _object_item_to_python(it)
                if o is not None:
                    ret_append(o)
            return ret

    def item_append(self, label, evasObject icon = None, callback = None, *args, **kwargs):
        """Appends a new item to the diskselector object.

        A new item will be created and appended to the diskselector, i.e.,
        will be set as last item. Also, if there is no selected item, it will
        be selected. This will always happens for the first appended item.

        If no icon is set, label will be centered on item position, otherwise
        the icon will be placed at left of the label, that will be shifted
        to the right.

        Items created with this method can be deleted with
        L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time
        this item is selected, i.e., the user stops the diskselector with
        this item on center position.

        Simple example (with no function callback or data associated)::
            disk = Diskselector(win)
            ic = Icon(win)
            ic.file_set("path/to/image")
            ic.resizable_set(EINA_TRUE, EINA_TRUE)
            disk.item_append("label", ic)

        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Image}

        @param label: The label of the diskselector item.
        @type label: string
        @param icon: The icon object to use at left side of the item. An
            icon can be any Evas object, but usually it is an L{Icon}.
        @type icon: L{evasObject}
        @param func: The function to call when the item is selected.
        @type func: function

        @return: The created item or C{None} upon failure.
        @rtype: L{DiskselectorItem}

        """
        return DiskselectorItem(self, label, icon, callback, *args, **kwargs)

    property selected_item:
        """Get the selected item.

        The selected item can be unselected with function
        L{DiskselectorItem.selected}, and the first item of diskselector
        will be selected.

        The selected item always will be centered on diskselector, with full
        label displayed, i.e., max length set to side labels won't apply on
        the selected item. More details on
        L{side_text_max_length}.

        @type: L{DiskselectorItem}

        """
        def __get__(self):
            cdef Elm_Object_Item *it = elm_diskselector_selected_item_get(self.obj)
            return _object_item_to_python(it)

    property first_item:
        """Get the first item of the diskselector.

        The list of items follows append order. So it will return the first
        item appended to the widget that wasn't deleted.

        @see: L{item_append()}
        @see: L{items}

        @type: L{DiskselectorItem}

        """
        def __get__(self):
            cdef Elm_Object_Item *it = elm_diskselector_first_item_get(self.obj)
            return _object_item_to_python(it)

    property last_item:
        """Get the last item of the diskselector.

        The list of items follows append order. So it will return last first
        item appended to the widget that wasn't deleted.

        @see: L{item_append()}
        @see: L{items}

        @type: L{DiskselectorItem}

        """
        def __get__(self):
            cdef Elm_Object_Item *it = elm_diskselector_last_item_get(self.obj)
            return _object_item_to_python(it)

    def callback_selected_add(self, func, *args, **kwargs):
        """When item is selected, i.e. scroller stops."""
        self._callback_add_full("selected", _cb_object_item_conv, func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _cb_object_item_conv, func)

    def callback_scroll_anim_start_add(self, func, *args, **kwargs):
        """Scrolling animation has started."""
        self._callback_add("scroll,anim,start", func, *args, **kwargs)

    def callback_scroll_anim_start_del(self, func):
        self._callback_del("scroll,anim,start", func)

    def callback_scroll_anim_stop_add(self, func, *args, **kwargs):
        """Scrolling animation has stopped."""
        self._callback_add("scroll,anim,stop", func, *args, **kwargs)

    def callback_scroll_anim_stop_del(self, func):
        self._callback_del("scroll,anim,stop", func)

    def callback_scroll_drag_start_add(self, func, *args, **kwargs):
        """Dragging the diskselector has started."""
        self._callback_add("scroll,drag,start", func, *args, **kwargs)

    def callback_scroll_drag_start_del(self, func):
        self._callback_del("scroll,drag,start", func)

    def callback_scroll_drag_stop_add(self, func, *args, **kwargs):
        """Dragging the diskselector has stopped."""
        self._callback_add("scroll,drag,stop", func, *args, **kwargs)

    def callback_scroll_drag_stop_del(self, func):
        self._callback_del("scroll,drag,stop", func)


_elm_widget_type_register("diskselector", Diskselector)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryDiskselector_Type # hack to install metaclass
_install_metaclass(&PyElementaryDiskselector_Type, ElementaryObjectMeta)
