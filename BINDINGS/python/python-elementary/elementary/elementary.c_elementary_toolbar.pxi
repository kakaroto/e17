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

cdef class ToolbarItem:
    """
    A item for the toolbar
    """
    cdef Elm_Toolbar_Item *item
    cdef object cbt

    def __del_cb(self):
        self.item = NULL
        self.cbt = None
        Py_DECREF(self)


    def __init__(self, c_evas.Object toolbar, c_evas.Object icon, label,
                 callback, *args, **kargs):
        cdef c_evas.Evas_Object *ic = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if icon is not None:
           ic = icon.obj

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _hoversel_callback

        self.cbt = (toolbar, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.item = elm_toolbar_item_add(toolbar.obj, ic, label, cb, cbdata)

        Py_INCREF(self)
        elm_toolbar_item_del_cb_set(self.item, _toolbar_item_del_cb)

    def delete(self):
        """Delete the item"""
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_toolbar_item_del(self.item)

    def select(self):
        """Select the item"""
        elm_toolbar_item_select(self.item)

cdef class Toolbar(Object):
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_toolbar_add(parent.obj))

    def scrollable_set(self, scrollable):
        """
        Set the scrollable property

        @parm: L{scrollable}
        """
        if scrollable:
            elm_toolbar_scrollable_set(self.obj, 1)
        else:
            elm_toolbar_scrollable_set(self.obj, 0)

    def item_add(self, c_evas.Object icon, label, callback, *args, **kargs):
        """
        Adds a new item to the toolbar

        @note: Never pass the the same icon object to more than one item. For
               a every item you must create a new icon!

        @parm: L{icon} icon for the item
        @parm: L{label} label for the item
        @parm: L{callback} function to click if the user clicked on the item
        """
        # Everything is done in the ToolbarItem class, because of wrapping the
        # C structures in python classes
        return ToolbarItem(self, icon, label, callback, *args, **kargs)

    property clicked:
        def __set__(self, value):
            """
            Set the callback function for the clicked-event

            @parm: L{value} callback function
            """
            self._callback_add("clicked", value)

