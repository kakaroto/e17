# Copyright (c) 2010 Boris Faure
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

cdef void _menu_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (menu, callback, it, a, ka) = <object>cbt
        callback(menu, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _menu_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

cdef class MenuItem(ObjectItem):
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent, label, icon,
                 callback, *args, **kargs):
        cdef Elm_Object_Item *parent_obj = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if parent:
            parent_obj = parent.obj

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _menu_callback

        self.cbt = (menu, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_menu_item_add(menu.obj, parent_obj, icon, label,
                                          cb, cbdata)

        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _menu_item_del_cb)

    def object_get(self):
        """Get the Evas_Object of an Elm_Object_Item

        @warning: Don't manipulate this object!

        @return: The edje object containing the swallowed content

        """
        return <Object>elm_menu_item_object_get(self.obj)

    def icon_name_set(self, icon):
        """Set the icon of a menu item to the standard icon with name C{icon}

        Once this icon is set, any previously set icon will be deleted.

        @param icon: The name of icon object to set for the content of item
        @type icon: string

        """
        elm_menu_item_icon_name_set(self.obj, icon)

    def icon_name_get(self):
        """Get the string representation from the icon of a menu item

        @see: L{icon_name_set()}

        @return: The string representation of item's icon or None
        @rtype: string

        """
        return elm_menu_item_icon_name_get(self.obj)

    property icon_name:
        """The standard icon name of a menu item

        Once this icon is set, any previously set icon will be deleted.

        @type: string

        """
        def __get__(self):
            return self.icon_name_get()
        def __set__(self, icon):
            self.icon_name_set(icon)

    def selected_set(self, selected):
        """Set the selected state of the item.

        @param selected: The selected/unselected state of the item
        @type selected: bool

        """
        elm_menu_item_selected_set(self.obj, selected)

    def selected_get(self):
        """Get the selected state of the item.

        @see: L{selected_set()}

        @return: The selected/unselected state of the item
        @rtype: bool

        """
        return elm_menu_item_selected_get(self.obj)

    property selected:
        """The selected state of the item.

        @type: bool

        """
        def __get__(self):
            return self.selected_get()
        def __set__(self, selected):
            self.selected_set(selected)

    def is_separator(self):
        """Returns whether the item is a separator.

        @see: L{Menu.item_separator_add()}

        @return: If True, the item is a separator
        @rtype: bool

        """
        return False

    def subitems_get(self):
        """Returns a list of item's subitems.

        @return: A tuple of item's subitems
        @rtype: tuple of L{MenuItem}s

        """
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_menu_item_subitems_get(self.obj)
        itr = lst
        while itr:
            data = elm_object_item_data_get(<Elm_Object_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    property subitems:
        """A list of item's subitems.

        @type: tuple of L{MenuItem}s

        """
        def __get__(self):
            return self.subitems_get()

    def index_get(self):
        """Get the position of a menu item

        This function returns the index position of a menu item in a menu.
        For a sub-menu, this number is relative to the first item in the sub-menu.

        @note: Index values begin with 0

        @return: The item's index
        @rtype: int

        """
        return elm_menu_item_index_get(self.obj)

    def next_get(self):
        """Get the next item in the menu.

        @return: The item after it, or None
        @rtype: L{MenuItem}

        """
        cdef Elm_Object_Item *it
        it = elm_menu_item_next_get(self.obj)
        return _elm_menu_item_to_python(it)

    def prev_get(self):
        """Get the previous item in the menu.

        @return: The item before it, or None
        @rtype: L{MenuItem}

        """
        cdef Elm_Object_Item *it
        it = elm_menu_item_prev_get(self.obj)
        return _elm_menu_item_to_python(it)

cdef _elm_menu_item_to_python(Elm_Object_Item *it):
    cdef void *data
    cdef object prm
    if it == NULL:
        return None
    data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    prm = <object>data
    return prm[2]

cdef void _menu_item_separator_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    it = <object>data
    it.__del_cb()

cdef class MenuItemSeparator(ObjectItem):
    def __del_cb(self):
        self.obj = NULL
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent):
        cdef Elm_Object_Item *parent_obj = NULL

        if parent:
            parent_obj = parent.obj
        self.obj = elm_menu_item_separator_add(menu.obj, parent_obj)
        if not self.obj:
            raise RuntimeError("Error creating separator")

        elm_object_item_data_set(self.obj, <void*>self)
        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _menu_item_separator_del_cb)

    def is_separator(self):
        """Returns whether the item is a separator.

        @see: L{Menu.item_separator_add()}

        @return: If True, the item is a separator
        @rtype: bool

        """
        return True

cdef public class Menu(Object) [object PyElementaryMenu, type PyElementaryMenu_Type]:

    """A menu is a list of items displayed above its parent.

    When the menu is showing its parent is darkened. Each item can have a
    sub-menu. The menu object can be used to display a menu on a right click
    event, in a toolbar, anywhere.

    Signals that you can add callbacks for are:
        - "clicked" - the user clicked the empty space in the menu to dismiss.

    Default content parts of the menu items that you can use for are:
        - "default" - A main content of the menu item

    Default text parts of the menu items that you can use for are:
        - "default" - label in the menu item

    """

    def __init__(self, c_evas.Object parent, obj = None):
        if obj is None:
            Object.__init__(self, parent.evas)
            self._set_obj(elm_menu_add(parent.obj))
        else:
            self._set_obj(<c_evas.Evas_Object*>obj)

    def parent_set(self, c_evas.Object parent):
        """Set the parent for the given menu widget

        @param parent: The new parent.
        @type parent: L{Object}

        """
        elm_menu_parent_set(self.obj, parent.obj)

    def parent_get(self):
        """Get the parent for the given menu widget

        @see: L{parent_set()}

        @return: The parent.
        @rtype: L{Object}

        """
        cdef c_evas.Evas_Object *o
        o = elm_menu_parent_get(self.obj)
        return evas.c_evas._Object_from_instance(<long>o)

    property parent:
        """The parent for the given menu widget.

        @type: L{Object}

        """
        def __get__(self):
            return self.parent_get()
        def __set__(self, parent):
            self.parent_set(parent)

    def move(self, x, y):
        """Move the menu to a new position

        Sets the top-left position of the menu to (C{x},C{y}).

        @note: C{x} and C{y} coordinates are relative to parent.

        @param x: The new position.
        @type x: Evas_Coord (int)
        @param y: The new position.
        @type y: Evas_Coord (int)

        """
        elm_menu_move(self.obj, x, y)

    def close(self):
        """Close a opened menu

        Hides the menu and all it's sub-menus.

        """
        elm_menu_close(self.obj)

    def items_get(self):
        """Returns a list of C{item}'s items.

        @return: A tuple of C{item}'s items
        @rtype: tuple of L{Object}s

        """
        cdef Elm_Object_Item *it
        cdef c_evas.const_Eina_List *lst
        cdef void *data
        cdef object prm

        lst = elm_menu_items_get(self.obj)
        ret = []
        ret_append = ret.append
        while lst:
            it = <Elm_Object_Item *>lst.data
            lst = lst.next
            o = _elm_menu_item_to_python(it)
            if o is not None:
                ret_append(o)
        return ret

    def item_add(self, parent = None, label = None, icon = None, callback = None, *args, **kwargs):
        """Add an item at the end of the given menu widget

        @param parent: The parent menu item (optional)
        @type parent: L{Object}
        @param icon: An icon display on the item. The icon will be destroyed by the menu.
        @type icon: string
        @param label: The label of the item.
        @type label: string
        @param func: Function called when the user select the item.
        @type func: function
        @return: Returns the new item.
        @rtype: L{MenuItem}

        """
        return MenuItem(self, parent, label, icon, callback, *args, **kwargs)

    def item_separator_add(self, item = None):
        """Add a separator item to menu under C{parent}.

        This is item is a @ref Separator.

        @param parent: The item to add the separator under
        @type parent: L{Object}
        @return: The created item or None on failure
        @rtype: L{MenuItemSeparator}

        """
        return MenuItemSeparator(self, item)

    def selected_item_get(self):
        """Get the selected item in the menu

        @see: L{MenuItem.selected_get()}
        @see: L{MenuItem.selected_set()}

        @return: The selected item, or None
        @rtype: L{MenuItem}

        """
        cdef Elm_Object_Item *it
        it = elm_menu_selected_item_get(self.obj)
        return _elm_menu_item_to_python(it)

    property selected_item:
        """The selected item in the menu

        @see: L{MenuItem.selected}

        @type: L{MenuItem}

        """
        def __get__(self):
            return self.selected_item_get()

    def last_item_get(self):
        """Get the last item in the menu

        @return: The last item, or None
        @rtype: L{MenuItem}

        """
        cdef Elm_Object_Item *it
        it = elm_menu_last_item_get(self.obj)
        return _elm_menu_item_to_python(it)

    property last_item:
        """The last item in the menu

        @type: L{MenuItem}

        """
        def __get__(self):
            return self.last_item_get()

    def first_item_get(self):
        """Get the first item in the menu

        @return: The first item, or None
        @rtype: MenuItem

        """
        cdef Elm_Object_Item *it
        it = elm_menu_first_item_get(self.obj)
        return _elm_menu_item_to_python(it)

    property first_item:
        """The first item in the menu

        @type: MenuItem

        """
        def __get__(self):
            return self.first_item_get()

    def callback_clicked_add(self, func, *args, **kwargs):
        """The user clicked the empty space in the menu to dismiss."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

_elm_widget_type_register("menu", Menu)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryMenu_Type # hack to install metaclass
_install_metaclass(&PyElementaryMenu_Type, ElementaryObjectMeta)
