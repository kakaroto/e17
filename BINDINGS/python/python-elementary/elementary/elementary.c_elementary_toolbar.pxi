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

cdef void _toolbar_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (toolbar, callback, it, a, ka) = <object>cbt
        callback(toolbar, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _toolbar_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

cdef class ToolbarItem(WidgetItem):
    """
    A item for the toolbar
    """
    cdef Elm_Toolbar_Item *obj
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)


    def __init__(self, c_evas.Object toolbar, icon, label,
                 callback, *args, **kargs):
        cdef c_evas.Evas_Object *ic = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _hoversel_callback

        self.cbt = (toolbar, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_toolbar_item_append(toolbar.obj, icon, label, cb, cbdata)

        Py_INCREF(self)
        elm_toolbar_item_del_cb_set(self.obj, _toolbar_item_del_cb)

    def next_get(self):
        cdef Elm_Toolbar_Item *it
        it = elm_toolbar_item_next_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property next:
        def __get__(self):
            return self.next_get()

    def prev_get(self):
        cdef Elm_Toolbar_Item *it
        it = elm_toolbar_item_prev_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property prev:
        def __get__(self):
            return self.prev_get()

    def delete(self):
        """Delete the item"""
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        elm_toolbar_item_del(self.obj)

    def icon_get(self):
        cdef const_char_ptr i
        i = elm_toolbar_item_icon_get(self.obj)
        if i == NULL:
            return None
        return i

    property icon:
        def __get__(self):
            return self.icon_get()

    def label_set(self, label):
        elm_toolbar_item_label_set(self.obj, label)

    def label_get(self):
        cdef const_char_ptr l
        l = elm_toolbar_item_label_get(self.obj)
        if l == NULL:
            return None
        return l

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, value):
            self.label_set(value)

    def data_get(self):
        cdef void* data
        data = elm_toolbar_item_data_get(self.obj)
        if data == NULL:
            return None
        else:
            (tb, func, it, a, ka) = <object>data
            return (a, ka)

    property data:
        def __get__(self):
            return self.data_get()

    def selected_set(self, selected):
        """Select the item"""
        elm_toolbar_item_selected_set(self.obj, selected)

    def selected_get(self):
        return elm_toolbar_item_selected_get(self.obj)

    property selected:
        def __set__(self, selected):
            elm_toolbar_item_selected_set(self.obj, selected)

        def __get__(self):
            return elm_toolbar_item_selected_get(self.obj)

    def disabled_set(self, disabled):
        elm_toolbar_item_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return elm_toolbar_item_disabled_get(self.obj)

    property disabled:
        def __set__(self, disabled):
            elm_toolbar_item_disabled_set(self.obj, disabled)

        def __get__(self):
            return elm_toolbar_item_disabled_get(self.obj)

    def separator_set(self, separator):
        elm_toolbar_item_separator_set(self.obj, separator)

    def separator_get(self):
        return elm_toolbar_item_separator_get(self.obj)

    property separator:
        def __set__(self, separator):
            elm_toolbar_item_separator_set(self.obj, separator)

        def __get__(self):
            return elm_toolbar_item_separator_get(self.obj)

    def menu_set(self, menu):
        elm_toolbar_item_menu_set(self.obj, menu)

    def menu_get(self):
        cdef c_evas.Evas_Object *menu
        menu = elm_toolbar_item_menu_get(self.obj)
        if menu == NULL:
            return None
        else:
            return Menu(None, <object>menu)

    def tooltip_text_set(self, char *text):
        """ Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed.
        Internaly, this method call @tooltip_content_cb_set
        """
        elm_toolbar_item_tooltip_text_set(self.obj, text)

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """ Set the content to be shown in the tooltip object

        @param: B{func} Function to be create tooltip content, called when
                need show tooltip.

        Setup the tooltip to object. The object can have only one tooltip,
        so any previews tooltip data is removed. @func(with @{args,kargs}) will
        be called every time that need show the tooltip and it should return a
        valid Evas_Object. This object is then managed fully by tooltip system
        and is deleted when the tooltip is gone.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, self, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_toolbar_item_tooltip_content_cb_set(self.obj,
                                                _tooltip_item_content_create,
                                                cbdata,
                                                _tooltip_item_data_del_cb)

    def item_tooltip_unset(self):
        """ Unset tooltip from object

        Remove tooltip from object. If used the @tool_text_set the internal
        copy of label will be removed correctly. If used
        @tooltip_content_cb_set, the data will be unreferred but no freed.
        """
        elm_toolbar_item_tooltip_unset(self.obj)

    def tooltip_style_set(self, style=None):
        """ Sets a different style for this object tooltip.

        @note before you set a style you should define a tooltip with
        elm_toolbar_item_tooltip_content_cb_set() or
        elm_toolbar_item_tooltip_text_set()
        """
        if style:
            elm_toolbar_item_tooltip_style_set(self.obj, style)
        else:
            elm_toolbar_item_tooltip_style_set(self.obj, NULL)

    def tooltip_style_get(self):
        """ Get the style for this object tooltip.
        """
        cdef const_char_ptr style
        style = elm_toolbar_item_tooltip_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_set(self, char *cursor):
        """ Set the cursor to be shown when mouse is over the toolbar item

        Set the cursor that will be displayed when mouse is over the
        item. The item can have only one cursor set to it, so if
        this function is called twice for an item, the previous set
        will be unset.
        """
        elm_toolbar_item_cursor_set(self.obj, cursor)

    def cursor_unset(self):
        """  Unset the cursor to be shown when mouse is over the toolbar item
        """
        elm_toolbar_item_cursor_unset(self.obj)

    def cursor_style_set(self, style=None):
        """ Sets a different style for this object cursor.

        @note before you set a style you should define a cursor with
        elm_toolbar_item_cursor_set()
        """
        if style:
            elm_toolbar_item_cursor_style_set(self.obj, style)
        else:
            elm_toolbar_item_cursor_style_set(self.obj, NULL)

    def cursor_style_get(self):
        """ Get the style for this object cursor.
        """
        cdef const_char_ptr style
        style = elm_toolbar_item_cursor_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_engine_only_set(self, engine_only):
        """ Sets cursor engine only usage for this object.

        @note before you set engine only usage you should define a cursor with
        elm_toolbar_item_cursor_set()
        """
        elm_toolbar_item_cursor_engine_only_set(self.obj, bool(engine_only))

    def cursor_engine_only_get(self):
        """ Get the engine only usage for this object.
        """
        return elm_toolbar_item_cursor_engine_only_get(self.obj)

cdef _elm_toolbar_item_to_python(Elm_Toolbar_Item *it):
    cdef void *data
    cdef object prm
    if it == NULL:
        return None
    data = elm_toolbar_item_data_get(it)
    if data == NULL:
        return None
    prm = <object>data
    return prm[2]


cdef class Toolbar(Object):
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_toolbar_add(parent.obj))

    def menu_parent_set(self, c_evas.Object parent):
        elm_toolbar_menu_parent_set(self.obj, parent.obj)

    property menu_parent:
        def __set__(self, c_evas.Object parent):
            elm_toolbar_menu_parent_set(self.obj, parent.obj)


    def homogenous_set(self, homogenous):
        elm_toolbar_homogenous_set(self.obj, homogenous)

    property homogenous:
        def __set__(self, homogenous):
            elm_toolbar_homogenous_set(self.obj, homogenous)


    def align_set(self, align):
        elm_toolbar_align_set(self.obj, align)

    property align:
        def __set__(self, align):
            elm_toolbar_align_set(self.obj, align)


    def icon_size_set(self, icon_size):
        elm_toolbar_icon_size_set(self.obj, icon_size)

    def icon_size_get(self, icon_size):
        return elm_toolbar_icon_size_get(self.obj)

    property icon_size:
        def __set__(self, icon_size):
            elm_toolbar_icon_size_set(self.obj, icon_size)

        def __get__(self):
            return elm_toolbar_icon_size_get(self.obj)

    def selected_item_get(self):
        cdef Elm_Toolbar_Item *it
        it = elm_toolbar_selected_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property selected_item:
        def __get__(self):
            return self.selected_item_get()

    def first_item_get(self):
        cdef Elm_Toolbar_Item *it
        it = elm_toolbar_first_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def last_item_get(self):
        cdef Elm_Toolbar_Item *it
        it = elm_toolbar_last_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def item_append(self, icon, label, callback = None, *args, **kargs):
        """
        Appends a new item to the toolbar

        @parm: L{icon} icon for the item
        @parm: L{label} label for the item
        @parm: L{callback} function to click if the user clicked on the item
        """
        # Everything is done in the ToolbarItem class, because of wrapping the
        # C structures in python classes
        return ToolbarItem(self, icon, label, callback, *args, **kargs)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)


_elm_widget_type_register("toolbar", Toolbar)
