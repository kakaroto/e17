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

cdef enum Elm_List_Item_Insert_Kind:
    ELM_LIST_ITEM_INSERT_APPEND
    ELM_LIST_ITEM_INSERT_PREPEND
    ELM_LIST_ITEM_INSERT_BEFORE
    ELM_LIST_ITEM_INSERT_AFTER
    ELM_LIST_ITEM_INSERT_SORTED

cdef class ListItem(ObjectItem):

    """An item for the list widget."""

    def __init__(self, kind, evasObject list, label, evasObject icon = None,
                 evasObject end = None, ListItem before_after = None,
                 callback = None, *args, **kargs):

        cdef Evas_Object* icon_obj = NULL
        cdef Evas_Object* end_obj = NULL
        cdef Evas_Smart_Cb cb = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _object_item_callback

        self.params = (callback, args, kargs)

        if kind == ELM_LIST_ITEM_INSERT_APPEND:
            item = elm_list_item_append(   list.obj,
                                            _cfruni(label),
                                            icon_obj,
                                            end_obj,
                                            cb,
                                            <void*>self)

        elif kind == ELM_LIST_ITEM_INSERT_PREPEND:
            item = elm_list_item_prepend(  list.obj,
                                            _cfruni(label),
                                            icon_obj,
                                            end_obj,
                                            cb,
                                            <void*>self)

        #elif kind == ELM_LIST_ITEM_INSERT_SORTED:
            #item = elm_list_item_sorted_insert(   list.obj,
                                                    #_cfruni(label),
                                                    #icon_obj,
                                                    #end_obj,
                                                    #cb,
                                                    #<void*>self,
                                                    #cmp_f)

        else:
            if before_after == None:
                raise ValueError("need a valid after object to add an item before/after another item")

            if kind == ELM_LIST_ITEM_INSERT_BEFORE:
                item = elm_list_item_insert_before(list.obj,
                                                    before_after.item,
                                                    _cfruni(label),
                                                    icon_obj,
                                                    end_obj,
                                                    cb,
                                                    <void*>self)

            else:
                item = elm_list_item_insert_after( list.obj,
                                                    before_after.item,
                                                    _cfruni(label),
                                                    icon_obj,
                                                    end_obj,
                                                    cb,
                                                    <void*>self)
        if item != NULL:
            self._set_obj(item)
        else:
            Py_DECREF(self)

    def __str__(self):
        return ("%s(label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, self.text_get(), bool(self.part_content_get("icon")),
             bool(self.part_content_get("end")), self.params[0], self.params[1], self.params[2])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Object_Item=%#x, "
                "label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, <unsigned long><void *>self,
             PY_REFCOUNT(self), <unsigned long><void *>self.item,
             self.text_get(), bool(self.part_content_get("icon")),
             bool(self.part_content_get("end")), self.params[0], self.params[1], self.params[2])

    def selected_set(self, selected):
        """Set the selected state of an item.

        This sets the selected state of the given item.
        C{True} for selected, C{False} for not selected.

        If a new item is selected the previously selected will be unselected,
        unless multiple selection is enabled with L{List.multi_select_set()}.
        Previously selected item can be get with function
        L{List.selected_item_get()}.

        Selected items will be highlighted.

        @see: L{selected_get()}
        @see: L{List.selected_item_get()}
        @see: L{List.multi_select_set()}

        @param selected: The selected state
        @type selected: bool

        """
        elm_list_item_selected_set(self.item, selected)

    def selected_get(self):
        """Get whether the item is selected or not.

        @see: L{List.selected_item_get()}
        @see: L{selected_set()} for details.

        @return: C{True} means item is selected. C{False} indicates it's not.
        @rtype: bool

        """
        return bool(elm_list_item_selected_get(self.item))

    property selected:
        """The selected state of an item.

        This property is the selected state of the given item.
        C{True} for selected, C{False} for not selected.

        If a new item is selected the previously selected will be unselected,
        unless multiple selection is enabled with L{List.multi_select}.
        Previously selected item can be get with
        L{List.selected_item}.

        Selected items will be highlighted.

        @see: L{List.selected_item}
        @see: L{List.multi_select}

        @type: bool

        """
        def __get__(self):
            return bool(elm_list_item_selected_get(self.item))

        def __set__(self, selected):
            elm_list_item_selected_set(self.item, selected)

    def separator_set(self, separator):
        """Set or unset item as a separator.

        Items aren't set as separator by default.

        If set as separator it will display separator theme, so won't display
        icons or label.

        @see: L{separator_get()}

        @param setting: C{True} to set item as separator or C{False} to
            unset, i.e., item will be used as a regular item.
        @type setting: bool

        """
        elm_list_item_separator_set(self.item, separator)

    def separator_get(self):
        """Get a value whether item is a separator or not.

        @see: L{separator_set()} for details.

        @return: C{True} means item is a separator. C{False} indicates it's not.
        @rtype: bool

        """
        return bool(elm_list_item_separator_get(self.item))

    property separator:
        """Set or unset item as a separator.

        Items aren't set as separator by default.

        If set as separator it will display separator theme, so won't display
        icons or label.

        @type: bool

        """
        def __get__(self):
            return bool(elm_list_item_separator_get(self.item))

        def __set__(self, separator):
            elm_list_item_separator_set(self.item, separator)

    def show(self):
        """show()

        Show the item in the list view.

        It won't animate list until item is visible. If such behavior is wanted,
        use L{bring_in()} instead.

        """
        elm_list_item_show(self.item)

    def bring_in(self):
        """bring_in()

        Bring in the given item to list view.

        This causes list to jump to the given item and show it
        (by scrolling), if it is not fully visible.

        This may use animation to do so and take a period of time.

        If animation isn't wanted, L{show()} can be used.

        """
        elm_list_item_bring_in(self.item)

    def object_get(self):
        """Returns the base object set for this list item.

        Base object is the one that visually represents the list item
        row. It must not be changed in a way that breaks the list
        behavior (like deleting the base!), but it might be used to
        feed Edje signals to add more features to row representation.

        @rtype: edje.Edje

        """
        return Object_from_instance(elm_list_item_object_get(self.item))

    property object:
        """Returns the base object set for this list item.

        Base object is the one that visually represents the list item
        row. It must not be changed in a way that breaks the list
        behavior (like deleting the base!), but it might be used to
        feed Edje signals to add more features to row representation.

        @type: edje.Edje

        """
        def __get__(self):
            return Object_from_instance(elm_list_item_object_get(self.item))

    def prev_get(self):
        """Get the item before this item in the list.

        @note: If it is the first item, C{None} will be returned.

        @see: L{List.item_append()}
        @see: L{List.items_get()}

        @return: The item before this item, or C{None} if none or on failure.
        @rtype: L{ListItem}

        """
        return _object_item_to_python(elm_list_item_prev(self.item))

    property prev:
        """The item before this item in the list.

        @note: If the item is the first item, C{None} will be returned.

        @see: L{List.item_append()}
        @see: L{List.items_get()}

        @type: L{ListItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_list_item_prev(self.item))

    def next_get(self):
        """Get the item after this item in the list.

        @note: If it is the last item, C{None} will be returned.

        @see: L{List.item_append()}
        @see: L{List.items_get()}

        @return: The item after this item, or C{None} if none or on failure.
        @rtype: L{ListItem}

        """
        return _object_item_to_python(elm_list_item_next(self.item))

    property next:
        """The item after this item in the list.

        @note: If the item is the last item, C{None} will be returned.

        @see: L{List.item_append()}
        @see: L{List.items_get()}

        @type: L{ListItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_list_item_next(self.item))

cdef public class List(Object) [object PyElementaryList, type PyElementaryList_Type]:

    """A list widget is a container whose children are displayed vertically or
    horizontally, in order, and can be selected.
    The list can accept only one or multiple items selection. Also has many
    modes of items displaying.

    A list is a very simple type of list widget.  For more robust
    lists, L{Genlist} should probably be used.

    Smart callbacks one can listen to:
        - C{"activated"} - The user has double-clicked or pressed
            (enter|return|spacebar) on an item. The C{event_info} parameter
            is the item that was activated.
        - C{"clicked,double"} - The user has double-clicked an item.
            The C{event_info} parameter is the item that was double-clicked.
        - "selected" - when the user selected an item
        - "unselected" - when the user unselected an item
        - "longpressed" - an item in the list is long-pressed
        - "edge,top" - the list is scrolled until the top edge
        - "edge,bottom" - the list is scrolled until the bottom edge
        - "edge,left" - the list is scrolled until the left edge
        - "edge,right" - the list is scrolled until the right edge
        - "language,changed" - the program's language changed

    Available styles for it:
        - C{"default"}

    Default content parts of the list items that you can use for are:
        - "start" - A start position object in the list item
        - "end" - A end position object in the list item

    Default text parts of the list items that you can use for are:
        - "default" - label in the list item

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_list_add(parent.obj))

    def go(self):
        """go()

        Starts the list.

        Example::
            li = List(win)
            li.item_append("First", None, None, None)
            li.item_append("Second", None, None, None)
            li.go()
            li.show()

        @note: Call before running show() on the list object.
        @warning: If not called, it won't display the list properly.

        """
        elm_list_go(self.obj)

    def multi_select_set(self, multi):
        """Enable or disable multiple items selection on the list object.

        Disabled by default. If disabled, the user can select a single item of
        the list each time. Selected items are highlighted on list.
        If enabled, many items can be selected.

        If a selected item is selected again, it will be unselected.

        @see: L{multi_select_get()}

        @param multi: C{True} to enable multi selection or C{False} to
            disable it.
        @type multi: bool

        """
        elm_list_multi_select_set(self.obj, multi)

    def multi_select_get(self):
        """Get a value whether multiple items selection is enabled or not.

        @see: L{multi_select_set()} for details.

        @return: C{True} means multiple items selection is enabled.
            C{False} indicates it's disabled.
        @rtype: bool

        """
        return bool(elm_list_multi_select_get(self.obj))

    property multi_select:
        """Enable or disable multiple items selection on the list object.

        Disabled by default. If disabled, the user can select a single item of
        the list each time. Selected items are highlighted on list.
        If enabled, many items can be selected.

        If a selected item is selected again, it will be unselected.

        @type: bool

        """
        def __get__(self):
            return elm_list_multi_select_get(self.obj)

        def __set__(self, multi):
            elm_list_multi_select_set(self.obj, multi)

    def mode_set(self, Elm_List_Mode mode):
        """Set which mode to use for the list object.

        Set list's resize behavior, transverse axis scroll and
        items cropping. See each mode's description for more details.

        Only one can be set, if a previous one was set, it will be changed
        by the new mode set. Bitmask won't work as well.

        @see: L{mode_get()}

        @note: Default value is ELM_LIST_SCROLL.

        @param mode: One of: ELM_LIST_COMPRESS, ELM_LIST_SCROLL, ELM_LIST_LIMIT or ELM_LIST_EXPAND.
        @type mode: Elm_List_Mode

        """
        elm_list_mode_set(self.obj, mode)

    def mode_get(self):
        """Get the mode the list is at.

        @see: L{mode_set()} for more information.

        @return: One of: ELM_LIST_COMPRESS, ELM_LIST_SCROLL, ELM_LIST_LIMIT, ELM_LIST_EXPAND or ELM_LIST_LAST on errors.
        @rtype: Elm_List_Mode

        """
        return elm_list_mode_get(self.obj)

    property mode:
        """Which mode to use for the list object.

        The list's resize behavior, transverse axis scroll and
        items cropping. See each mode's description for more details.

        Only one can be set, if a previous one was set, it will be changed
        by the new mode set. Bitmask won't work as well.

        @note: Default value is ELM_LIST_SCROLL.

        @type: Elm_List_Mode

        """
        def __get__(self):
            return elm_list_mode_get(self.obj)

        def __set__(self, Elm_List_Mode mode):
            elm_list_mode_set(self.obj, mode)

    property horizontal:
        """Enable or disable horizontal mode on the list object.

        @note: Vertical mode is set by default.

        On horizontal mode items are displayed on list from left to right,
        instead of from top to bottom. Also, the list will scroll horizontally.
        Each item will presents left icon on top and right icon, or end, at
        the bottom.

        @type: bool

        """
        def __get__(self):
            return elm_list_horizontal_get(self.obj)

        def __set__(self, horizontal):
            elm_list_horizontal_set(self.obj, horizontal)

    def select_mode_set(self, mode):
        """Set the list select mode.

        select_mode_set() changes item select mode in the list widget.
            - ELM_OBJECT_SELECT_MODE_DEFAULT : Items will only call their
                selection func and callback when first becoming selected. Any
                further clicks will do nothing, unless you set always
                select mode.
            - ELM_OBJECT_SELECT_MODE_ALWAYS :  This means that, even if
                selected, every click will make the selected callbacks be called.
            - ELM_OBJECT_SELECT_MODE_NONE : This will turn off the ability
                to select items entirely and they will neither appear
                selected nor call selected callback functions.

        @see: L{select_mode_get()}

        @param mode: The select mode
        @type mode: Elm_Object_Select_Mode

        """
        elm_list_select_mode_set(self.obj, mode)

    def select_mode_get(self):
        """Get the list select mode.

        @see: L{select_mode_set()}

        @return: The select mode (If getting mode is failed,
            it returns ELM_OBJECT_SELECT_MODE_MAX)
        @rtype: Elm_Object_Select_Mode

        """
        return elm_list_select_mode_get(self.obj)

    property select_mode:
        """The list select mode.

        Possible modes are:
            - ELM_OBJECT_SELECT_MODE_DEFAULT : Items will only call their
                selection func and callback when first becoming selected. Any
                further clicks will do nothing, unless you set always
                select mode.
            - ELM_OBJECT_SELECT_MODE_ALWAYS :  This means that, even if
                selected, every click will make the selected callbacks be
                called.
            - ELM_OBJECT_SELECT_MODE_NONE : This will turn off the ability
                to select items entirely and they will neither appear
                selected nor call selected callback functions.

        @type: Elm_Object_Select_Mode

        """
        def __set__(self, mode):
            elm_list_select_mode_set(self.obj, mode)

        def __get__(self):
            return elm_list_select_mode_get(self.obj)

    def bounce_set(self, h, v):
        """Set bouncing behaviour when the scrolled content reaches an edge.

        Tell the internal scroller object whether it should bounce or not
        when it reaches the respective edges for each axis.

        @see: L{Scroller.bounce_set()}

        @param h_bounce: Whether to bounce or not in the horizontal axis.
        @type h_bounce: bool
        @param v_bounce: Whether to bounce or not in the vertical axis.
        @type v_bounce: bool

        """
        elm_scroller_bounce_set(self.obj, h, v)

    def bounce_get(self):
        """Get the bouncing behaviour of the internal scroller.

        Get whether the internal scroller should bounce when the edge of each
        axis is reached scrolling.

        @see: L{Scroller.bounce_get()}
        @see: L{bounce_set()}

        @return: The bounce states of horizontal axis and vertical axis.
        @rtype: tuple of bools

        """
        cdef Eina_Bool h, v
        elm_scroller_bounce_get(self.obj, &h, &v)
        return (h, v)

    property bounce:
        """The bouncing behaviour when the scrolled content reaches an edge.

        Whether the internal scroller object should bounce or not when it
        reaches the respective edges for each axis.

        @type: tuple of bools

        """
        def __set__(self, value):
            h, v = value
            elm_scroller_bounce_set(self.obj, h, v)

    def scroller_policy_set(self, policy_h, policy_v):
        """Set the scrollbar policy.

        This sets the scrollbar visibility policy for the given scroller.
        ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it
        is needed, and otherwise kept hidden. ELM_SCROLLER_POLICY_ON turns
        it on all the time, and ELM_SCROLLER_POLICY_OFF always keeps it off.
        This applies respectively for the horizontal and vertical scrollbars.

        The both are disabled by default, i.e., are set to
        ELM_SCROLLER_POLICY_OFF.

        @param policy_h: Horizontal scrollbar policy.
        @type policy_h: Elm_Scroller_Policy
        @param policy_v: Vertical scrollbar policy.
        @type policy_v: Elm_Scroller_Policy

        """
        elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def scroller_policy_get(self):
        """Get the scrollbar visibility policy.

        @see: L{scroller_policy_get()} for details.

        @return: Policy for horizontal axis and vertical axis.
        @rtype: Elm_Scroller_Policy

        """
        cdef Elm_Scroller_Policy policy_h, policy_v
        elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
        return (policy_h, policy_v)

    property scroller_policy:
        """The scrollbar policy.

        This sets the scrollbar visibility policy for the given scroller.
        ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it
        is needed, and otherwise kept hidden. ELM_SCROLLER_POLICY_ON turns
        it on all the time, and ELM_SCROLLER_POLICY_OFF always keeps it off.
        This applies respectively for the horizontal and vertical scrollbars.

        The both are disabled by default, i.e., are set to
        ELM_SCROLLER_POLICY_OFF.

        @type: Elm_Scroller_Policy

        """
        def __set__(self, value):
            policy_h, policy_v = value
            elm_scroller_policy_set(self.obj, policy_h, policy_v)

        def __get__(self):
            cdef Elm_Scroller_Policy policy_h, policy_v
            elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
            return (policy_h, policy_v)

    def item_append(self, label, evasObject icon = None,
                    evasObject end = None, callback = None, *args, **kargs):
        """item_append(label, icon=None, end=None, callback=None, *args, **kargs)

        Append a new item to the list object.

        A new item will be created and appended to the list, i.e., will
        be set as B{last} item.

        Items created with this method can be deleted with L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time this item
        is selected, i.e., the user clicks over an unselected item.
        If always select is enabled it will call this function every time
        user clicks over an item (already selected or not).
        If such function isn't needed, just passing
        C{None} as C{func} is enough.

        Simple example (with no function callback or data associated)::
            li = List(win)
            ic = Icon(win)
            ic.file_set("path/to/image")
            ic.resizable_set(True, True)
            li.item_append("label", ic, None, None)
            li.go()
            li.show()

        @see: L{select_mode_set()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Icon}

        @param label: The label of the list item.
        @type  label: string
        @param  icon: The icon object to use for the left side of the item. An
                      icon can be any Evas object, but usually it is an L{Icon}.
        @type   icon: L{Object}
        @param   end: The icon object to use for the right side of the item. An
                      icon can be any Evas object.
        @type    end: L{Object}
        @param  callback: The function to call when the item is clicked.
        @type   callback: function

        @return:      The created item or C{None} upon failure.
        @rtype:       L{ListItem}

        """
        return ListItem(ELM_LIST_ITEM_INSERT_APPEND, self, label, icon, end,
                        None, callback, *args, **kargs)

    def item_prepend(self, label, evasObject icon = None,
                    evasObject end = None, callback = None, *args, **kargs):
        """item_prepend(label, icon=None, end=None, callback=None, *args, **kargs)

        Prepend a new item to the list object.

        A new item will be created and prepended to the list, i.e., will
        be set as B{first} item.

        Items created with this method can be deleted with
        L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time
        this item is selected, i.e., the user clicks over an unselected item.
        If always select is enabled it will call this function every time
        user clicks over an item (already selected or not). If such function
        isn't needed, just passing C{None} as C{func} is enough.

        @see: L{item_append()} for a simple code example.
        @see: L{select_mode_set()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Icon}

        @param label: The label of the list item.
        @type  label: string
        @param  icon: The icon object to use for the left side of the item. An
                      icon can be any Evas object, but usually it is an L{Icon}.
        @type   icon: L{Object}
        @param   end: The icon object to use for the right side of the item. An
                      icon can be any Evas object.
        @type    end: L{Object}
        @param  callback: The function to call when the item is clicked.
        @type   callback: function

        @return:      The created item or C{None} upon failure.
        @rtype:       L{ListItem}

        """
        return ListItem(ELM_LIST_ITEM_INSERT_PREPEND, self, label, icon, end,
                        None, callback, *args, **kargs)

    def item_insert_before(self, ListItem before, label, evasObject icon = None,
                    evasObject end = None, callback = None, *args, **kargs):
        """item_insert_before(before, label, icon=None, end=None, callback=None, *args, **kargs)

        Insert a new item into the list object before item C{before}.

        A new item will be created and added to the list. Its position in
        this list will be just before item C{before}.

        Items created with this method can be deleted with
        L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time
        this item is selected, i.e., the user clicks over an unselected item.
        If always select is enabled it will call this function every time
        user clicks over an item (already selected or not). If such function
        isn't needed, just passing C{None} as C{func} is enough.

        @see: L{item_append()} for a simple code example.
        @see: L{select_mode_set()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Icon}

        @param before: The list item to insert before.
        @type  before: L{ListItem}
        @param  label: The label of the list item.
        @type   label: string
        @param   icon: The icon object to use for the left side of the item. An
                       icon can be any Evas object, but usually it is an L{Icon}.
        @type    icon: L{Object}
        @param    end: The icon object to use for the right side of the item. An
                       icon can be any Evas object.
        @type     end: L{Object}
        @param   callback: The function to call when the item is clicked.
        @type    callback: function

        @return:       The created item or C{None} upon failure.
        @rtype:        L{ListItem}

        """
        return ListItem(ELM_LIST_ITEM_INSERT_BEFORE, self, label, icon, end,
                        before, callback, *args, **kargs)

    def item_insert_after(self, ListItem after, label, evasObject icon = None,
                    evasObject end = None, callback = None, *args, **kargs):
        """item_insert_after(after, label, icon=None, end=None, callback=None, *args, **kargs)

        Insert a new item into the list object after item C{after}.

        A new item will be created and added to the list. Its position in
        this list will be just after item C{after}.

        Items created with this method can be deleted with
        L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time
        this item is selected, i.e., the user clicks over an unselected item.
        If always select is enabled it will call this function every time
        user clicks over an item (already selected or not). If such function
        isn't needed, just passing C{None} as C{func} is enough.

        @see: L{item_append()} for a simple code example.
        @see: L{select_mode_set()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Icon}

        @param after: The list item to insert after.
        @type  after: L{ListItem}
        @param label: The label of the list item.
        @type  label: string
        @param  icon: The icon object to use for the left side of the item. An
                      icon can be any Evas object, but usually it is an L{Icon}.
        @type   icon: L{Object}
        @param   end: The icon object to use for the right side of the item. An
                      icon can be any Evas object.
        @type    end: L{Object}
        @param  callback: The function to call when the item is clicked.
        @type   callback: function

        @return:      The created item or C{None} upon failure.
        @rtype:       L{ListItem}

        """
        return ListItem(ELM_LIST_ITEM_INSERT_AFTER, self, label, icon, end,
                        after, callback, *args, **kargs)

    #def item_sorted_insert(self, label, evasObject icon = None,
                    #evasObject end = None, callback = None, cmp_func=None, *args, **kargs):
        """item_sorted_insert(label, icon=None, end=None, callback=None, cmp_func=None, *args, **kargs)

        Insert a new item into the sorted list object.

        A new item will be created and added to the list. Its position in
        this list will be found comparing the new item with previously inserted
        items using function C{cmp_func.

        Items created with this method can be deleted with L{ObjectItem.delete()}.

        If a function is passed as argument, it will be called every time this item
        is selected, i.e., the user clicks over an unselected item.
        If always select is enabled it will call this function every time
        user clicks over an item (already selected or not).
        If such function isn't needed, just passing
        C{None} as C{func} is enough.

        @see: L{item_append()} for a simple code example.
        @see: L{select_mode_set()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}
        @see: L{Icon}

        @note: This function inserts values into a list object assuming it was
        sorted and the result will be sorted.

        @param    label: The label of the list item.
        @type     label: string
        @param     icon: The icon object to use for the left side of the item.
                         An icon can be any Evas object, but usually it is
                         an L{Icon}.
        @type      icon: L{Object}
        @param      end: The icon object to use for the right side of the item.
                         An icon can be any Evas object.
        @type       end: L{Object}
        @param     func: The function to call when the item is clicked.
        @type      func: function
        @param cmp_func: The comparing function to be used to sort list
                         items B{by L{ObjectItem} item handles}. This
                         function will receive two items and compare them,
                         returning a non-negative integer if the second item
                         should be place after the first, or negative value
                         if should be placed before.
        @type  cmp_func: function

        @return:         The created item or C{None} upon failure.
        @rtype:          L{ListItem}

        """
        #return ListItem(ELM_LIST_ITEM_INSERT_SORTED, self, label, icon, end,
                        #None, callback, *args, **kargs)

    def clear(self):
        """clear()

        Remove all list's items.

        @see: L{ObjectItem.delete()}
        @see: L{item_append()}

        """
        elm_list_clear(self.obj)

    def items_get(self):
        """Get a list of all the list items.

        @see: L{item_append()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}

        @return: A tuple of list items, or C{None} on failure.
        @rtype: tuple of L{ListItem}s

        """
        return _object_item_list_to_python(elm_list_items_get(self.obj))

    property items:
        """Get a list of all the list items.

        @see: L{item_append()}
        @see: L{ObjectItem.delete()}
        @see: L{clear()}

        @type: tuple of L{ListItem}s

        """
        def __get__(self):
            return _object_item_list_to_python(elm_list_items_get(self.obj))

    def selected_item_get(self):
        """Get the selected item.

        The selected item can be unselected with function
        L{ListItem.selected_set()}.

        The selected item always will be highlighted on list.

        @see: L{selected_items_get()}

        @return: The selected list item.
        @rtype: L{ListItem}

        """
        return _object_item_to_python(elm_list_selected_item_get(self.obj))

    property selected_item:
        """Get the selected item.

        The selected item can be unselected with function
        L{ListItem.selected_set()}.

        The selected item always will be highlighted on list.

        @see: L{selected_items}

        @type: L{ListItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_list_selected_item_get(self.obj))

    def selected_items_get(self):
        """Return a list of the currently selected list items.

        Multiple items can be selected if multi select is enabled. It can be
        done with L{multi_select_set()}.

        @see: L{selected_item_get()}
        @see: L{multi_select_set()}

        @return: A tuple of list items, or C{None} on failure.
        @rtype: tuple of L{ListItem}s

        """
        return _object_item_list_to_python(elm_list_selected_items_get(self.obj))

    property selected_items:
        """Return a list of the currently selected list items.

        Multiple items can be selected if multi select is enabled. It can be
        done with L{multi_select_set()}.

        @see: L{selected_item}
        @see: L{multi_select}

        @type: tuple of L{ListItem}s

        """
        def __get__(self):
            return _object_item_list_to_python(elm_list_selected_items_get(self.obj))

    def first_item_get(self):
        """Get the first item in the list

        This returns the first item in the list.

        @return: The first item, or None
        @rtype: L{ListItem}

        """
        return _object_item_to_python(elm_list_first_item_get(self.obj))

    property first_item:
        """The first item in the list

        @type: L{ListItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_list_first_item_get(self.obj))

    def last_item_get(self):
        """Get the last item in the list

        This returns the last item in the list.

        @return: The last item, or None
        @rtype: L{ListItem}

        """
        return _object_item_to_python(elm_list_last_item_get(self.obj))

    property last_item:
        """The last item in the list

        @type: L{ListItem}

        """
        def __get__(self):
            return _object_item_to_python(elm_list_last_item_get(self.obj))

    def callback_activated_add(self, func, *args, **kwargs):
        """The user has double-clicked or pressed (enter|return|spacebar) on
        an item. The C{event_info} parameter is the item that was activated."""
        self._callback_add_full("activated", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_activated_del(self, func):
        self._callback_del_full("activated",  _cb_object_item_conv, func)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        """The user has double-clicked an item. The C{event_info} parameter
        is the item that was double-clicked."""
        self._callback_add_full("clicked,double", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del_full("clicked,double",  _cb_object_item_conv, func)

    def callback_selected_add(self, func, *args, **kwargs):
        """When the user selected an item."""
        self._callback_add_full("selected", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _cb_object_item_conv, func)

    def callback_unselected_add(self, func, *args, **kwargs):
        """When the user unselected an item."""
        self._callback_add_full("unselected", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_unselected_del(self, func):
        self._callback_del_full("unselected", _cb_object_item_conv, func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        """An item in the list is long-pressed."""
        self._callback_add_full("longpressed", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del_full("longpressed", _cb_object_item_conv, func)

    def callback_edge_top_add(self, func, *args, **kwargs):
        """The list is scrolled until the top edge."""
        self._callback_add("edge,top", func, *args, **kwargs)

    def callback_edge_top_del(self, func):
        self._callback_del("edge,top",  func)

    def callback_edge_bottom_add(self, func, *args, **kwargs):
        """The list is scrolled until the bottom edge."""
        self._callback_add("edge,bottom", func, *args, **kwargs)

    def callback_edge_bottom_del(self, func):
        self._callback_del("edge,bottom",  func)

    def callback_edge_left_add(self, func, *args, **kwargs):
        """The list is scrolled until the left edge."""
        self._callback_add("edge,left", func, *args, **kwargs)

    def callback_edge_left_del(self, func):
        self._callback_del("edge,left",  func)

    def callback_edge_right_add(self, func, *args, **kwargs):
        """The list is scrolled until the right edge."""
        self._callback_add("edge,right", func, *args, **kwargs)

    def callback_edge_right_del(self, func):
        self._callback_del("edge,right",  func)

    def callback_language_changed_add(self, func, *args, **kwargs):
        """The program's language changed."""
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed",  func)

_elm_widget_type_register("list", List)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryList_Type # hack to install metaclass
_install_metaclass(&PyElementaryList_Type, ElementaryObjectMeta)
