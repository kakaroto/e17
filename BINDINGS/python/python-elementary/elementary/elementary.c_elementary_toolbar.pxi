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

cdef object _toolbar_callback_mapping
_toolbar_callback_mapping = dict()

cdef void _toolbar_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _toolbar_callback_mapping.get(<long>event_info)
        if mapping is not None:
            callback = mapping["callback"] 
            if callback is not None and callable(callback):
                callback(mapping["class"], "clicked")
        else:
            print "ERROR: no callback available for the item"
    except Exception, e:
        traceback.print_exc()

cdef class ToolbarItem:
    """
    A item for the toolbar
    """
    cdef Elm_Toolbar_Item *item

    def __new__(self):
        self.item = NULL

    def __init__(self, c_evas.Object toolbar, c_evas.Object icon, label, callback):
        if icon is not None:
            self.item = elm_toolbar_item_add(toolbar.obj, icon.obj, label, _toolbar_callback, NULL)
        else:
            self.item = elm_toolbar_item_add(toolbar.obj, NULL, label, _toolbar_callback, NULL)
        
        # Add a new callback in our mapping dict
        mapping = dict()
        mapping["class"] = self
        mapping["callback"] = callback
        _toolbar_callback_mapping[<long>self.item] = mapping

    def delete(self):
        """Delete the item"""
        elm_toolbar_item_del(self.item)

    def select(self):
        """Select the item"""
        elm_toolbar_item_select(self.item)

cdef class Toolbar(Object):
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
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
       
    def item_add(self, c_evas.Object icon, label, callback):
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
        return ToolbarItem(self, icon, label, callback)

    property clicked:
        def __set__(self, value):
            """
            Set the callback function for the clicked-event

            @parm: L{value} callback function
            """
            self._callback_add("clicked", value)
       
