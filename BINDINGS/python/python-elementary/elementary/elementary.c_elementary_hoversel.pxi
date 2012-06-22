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

cdef class HoverselItem(ObjectItem):

    """An item for the L{Hoversel} widget."""

    def __init__(self, evasObject hoversel, label, icon_file, icon_type,
                 callback, *args, **kargs):
        cdef Evas_Smart_Cb cb = NULL

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _object_item_callback

        self.params = (callback, args, kargs)
        item = elm_hoversel_item_add(   hoversel.obj,
                                        _cfruni(label) if label is not None else NULL,
                                        _cfruni(icon_file) if icon_file is not None else NULL,
                                        icon_type,
                                        cb,
                                        <void*>self)

        if item != NULL:
            self._set_obj(item)
        else:
            Py_DECREF(self)

    def icon_set(self, icon_file, icon_group, icon_type):
        """This sets the icon for the given hoversel item.

        The icon can be loaded from the standard set, from an image file, or
        from an edje file.

        @see: L{Hoversel.item_add()}

        @param icon_file: An image file path on disk to use for the icon or
            standard icon name
        @type icon_file: string
        @param icon_group: The edje group to use if C{icon_file} is an edje
            file. Set this to None if the icon is not an edje file
        @param icon_type: The icon type

        """
        elm_hoversel_item_icon_set(self.item, _cfruni(icon_file), _cfruni(icon_group), icon_type)

    def icon_get(self):
        """Get the icon object of the hoversel item

        @see: L{icon_set()}
        @see: L{Hoversel.item_add()}

        @return: The image file path / Standard name, Icon group, Icon type
        @rtype: (string, string, Elm_Icon_Type)

        """
        cdef const_char_ptr cicon_file, cicon_group
        cdef Elm_Icon_Type cicon_type
        elm_hoversel_item_icon_get(self.item, &cicon_file, &cicon_group, &cicon_type)
        return (_ctouni(cicon_file), _ctouni(cicon_group), cicon_type)

    property icon:
        """This sets the icon for the given hoversel item.

        The icon can be loaded from the standard set, from an image file, or
        from an edje file.

        @see: L{Hoversel.item_add()}

        @type: tuple(string file, string group, Elm_Icon_Type type)

        """
        def __set__(self, value):
            icon_file, icon_group, icon_type = value
            elm_hoversel_item_icon_set(self.item, _cfruni(icon_file), _cfruni(icon_group), icon_type)

        def __get__(self):
            cdef const_char_ptr cicon_file, cicon_group
            cdef Elm_Icon_Type cicon_type
            elm_hoversel_item_icon_get(self.item, &cicon_file, &cicon_group, &cicon_type)
            return (_ctouni(cicon_file), _ctouni(cicon_group), cicon_type)

cdef public class Hoversel(Button) [object PyElementaryHoversel, type PyElementaryHoversel_Type]:

    """A hoversel is a button that pops up a list of items (automatically
    choosing the direction to display) that have a label and, optionally, an
    icon to select from.

    It is a convenience widget to avoid the need to do all the piecing
    together yourself. It is intended for a small number of items in the
    hoversel menu (no more than 8), though is capable of many more.

    This widget emits the following signals, besides the ones sent from
    L{Button}:
        - C{"clicked"} - the user clicked the hoversel button and popped up
            the sel
        - C{"selected"} - an item in the hoversel list is selected. event_info
            is the item
        - C{"dismissed"} - the hover is dismissed

    Default content parts of the hoversel widget that you can use for are:
        - "icon" - An icon of the hoversel

    Default text parts of the hoversel widget that you can use for are:
        - "default" - Label of the hoversel

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_hoversel_add(parent.obj))

    def horizontal_set(self, horizontal):
        """This sets the hoversel to expand horizontally.

        @note: The initial button will display horizontally regardless of this
            setting.

        @param horizontal: If True, the hover will expand horizontally to the
            right.
        @type horizontal: bool

        """
        elm_hoversel_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        """This returns whether the hoversel is set to expand horizontally.

        @see: L{horizontal_set()}

        @return: If True, the hover will expand horizontally to the right.
        @rtype: bool

        """
        return bool(elm_hoversel_horizontal_get(self.obj))

    property horizontal:
        """Whether the hoversel is set to expand horizontally.

        @note: The initial button will display horizontally regardless of this
            setting.

        @type: bool

        """
        def __set__(self, horizontal):
            elm_hoversel_horizontal_set(self.obj, horizontal)

        def __get__(self):
            return bool(elm_hoversel_horizontal_get(self.obj))

    def hover_parent_set(self, evasObject parent):
        """Set the Hover parent

        Sets the hover parent object, the area that will be darkened when the
        hoversel is clicked. Should probably be the window that the hoversel is
        in. See L{Hover} objects for more information.

        @param parent: The parent to use
        @type parent: L{Object}

        """
        elm_hoversel_hover_parent_set(self.obj, parent.obj)

    def hover_parent_get(self):
        """Get the Hover parent

        Gets the hover parent object.

        @see: elm_hoversel_hover_parent_set()

        @return: The used parent
        @rtype: L{Object}

        """
        return Object_from_instance(elm_hoversel_hover_parent_get(self.obj))

    property hover_parent:
        """The Hover parent.

        The hover parent object, the area that will be darkened when the
        hoversel is clicked. Should probably be the window that the hoversel
        is in. See L{Hover} objects for more information.

        @type: L{Object}

        """
        def __set__(self, evasObject parent):
            elm_hoversel_hover_parent_set(self.obj, parent.obj)

        def __get__(self):
            return Object_from_instance(elm_hoversel_hover_parent_get(self.obj))

    def hover_begin(self):
        """hover_begin()

        This triggers the hoversel popup from code, the same as if the user
        had clicked the button.

        """
        elm_hoversel_hover_begin(self.obj)

    def hover_end(self):
        """hover_end()

        This dismisses the hoversel popup as if the user had clicked outside
        the hover.

        """
        elm_hoversel_hover_end(self.obj)

    def expanded_get(self):
        """Returns whether the hoversel is expanded.

        @return: This will return True if the hoversel is expanded or
            False if it is not expanded.
        @rtype: bool

        """
        return bool(elm_hoversel_expanded_get(self.obj))

    property expanded:
        """Returns whether the hoversel is expanded.

        @type: bool

        """
        def __get__(self):
            return bool(elm_hoversel_expanded_get(self.obj))

    def clear(self):
        """clear()

        This will remove all the children items from the hoversel.

        @warning: Should B{not} be called while the hoversel is active; use
            L{expanded_get()} to check first.

        @see: L{ObjectItem.delete()}

        """
        elm_hoversel_clear(self.obj)

    def items_get(self):
        """Get the list of items within the given hoversel.

        @see: L{item_add()}

        @return: Returns a list of Elm_Object_Item*
        @rtype: tuple of Elm_Object_Items

        """
        return _object_item_list_to_python(elm_hoversel_items_get(self.obj))

    property items:
        """Get the list of items within the given hoversel.

        @see: L{item_add()}

        @type: tuple of Elm_Object_Items

        """
        def __get__(self):
            return _object_item_list_to_python(elm_hoversel_items_get(self.obj))

    def item_add(self, label = None, icon_file = None, icon_type = ELM_ICON_NONE, callback = None, *args, **kwargs):
        """item_add(label, icon_file=None, icon_type=ELM_ICON_NONE, callback=None, *args, **kwargs)

        Add an item to the hoversel button

        This adds an item to the hoversel to show when it is clicked. Note:
        if you need to use an icon from an edje file then use
        L{HoverselItem.icon_set()} right after this function, and set
        icon_file to None here.

        For more information on what C{icon_file} and C{icon_type} are, see the
        L{Icon} "icon documentation".

        @param label: The text label to use for the item (None if not desired)
        @type label: string
        @param icon_file: An image file path on disk to use for the icon or
            standard icon name (None if not desired)
        @type icon_file: string
        @param icon_type: The icon type if relevant
        @type icon_type: string
        @param callback: Convenience function to call when this item is selected
        @type callback: function

        @return: The item added.
        @rtype: Elm_Object_Item

        """
        return HoverselItem(self, label, icon_file, icon_type, callback, *args, **kwargs)

    def callback_clicked_add(self, func, *args, **kwargs):
        """The user clicked the hoversel button and popped up the sel."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_selected_add(self, func, *args, **kwargs):
        """An item in the hoversel list is selected. event_info is the item."""
        self._callback_add_full("selected", _cb_object_item_conv, func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _cb_object_item_conv, func)

    def callback_dismissed_add(self, func, *args, **kwargs):
        """The hover is dismissed."""
        self._callback_add("dismissed", func, *args, **kwargs)

    def callback_dismissed_del(self, func):
        self._callback_del("dismissed", func)

_elm_widget_type_register("hoversel", Hoversel)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryHoversel_Type # hack to install metaclass
_install_metaclass(&PyElementaryHoversel_Type, ElementaryObjectMeta)
