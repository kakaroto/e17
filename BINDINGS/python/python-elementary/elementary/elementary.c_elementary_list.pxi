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

cdef object _list_callback_mapping
_list_callback_mapping = dict()

cdef void _list_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _list_callback_mapping.get(<long>event_info)
        if mapping is not None:
            callback = mapping["callback"] 
            if callback is not None and callable(callback):
                callback(mapping["class"], "clicked", mapping["data"])
        else:
            print "ERROR: no callback available for the item"
    except Exception, e:
        traceback.print_exc()
            
cdef class ListItem:
    """ 
    An item for the list widget
    """
    cdef Elm_List_Item *item
    cdef c_evas.Evas_Object *list

    def __new__(self):
        self.item = NULL

    def __init__(self):
        pass

    def _create_mapping(self, callback, data):
        mapping = dict()
        mapping["class"] = self
        mapping["callback"] = callback
        mapping["data"] = data
        _list_callback_mapping[<long>self.itme] = mapping

    def append(self, c_evas.Object list, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if not icon == None:
            icon_obj = icon.obj
        if not end == None:
            end_obj = end.obj
        
        self.item = elm_list_item_append(list.obj, label, icon_obj, end_obj, _list_callback, NULL)
        self._create_mapping(callback, data)

    def prepend(self, c_evas.Object list, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if not icon == None:
            icon_obj = icon.obj
        if not end == None:
            end_obj = end.obj
        
        self.item = elm_list_item_prepend(list.obj, label, icon_obj, end_obj, _list_callback, NULL)
        self._create_mapping(callback, data)

    def insert_before(self, c_evas.Object list, ListItem before, label, c_evas.Object icon,
            c_evas.Object end, callback, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        if before == None:
            raise ValueError("need a valid before object to add an item before another item")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if not icon == None:
            icon_obj = icon.obj
        if not end ==  None:
            end_obj = end.obj

        self.item = elm_list_item_insert_before(list.obj, before.item, label, icon_obj, end_obj, _list_callback, NULL)
        self._create_mapping(callback, data)

    def insert_after(self, c_evas.Object list, ListItem after, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")
        
        if after == None:
            raise ValueError("need a valid after object to add an item after another item")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if not icon == None:
            icon_obj = icon.obj
        if not end ==  None:
            end_obj = end.obj

        self.item = elm_list_item_insert_after(list.obj, after.item, label, icon_obj, end_obj,
            _list_callback, NULL)
        self._create_mapping(callback, data)
   
    def selected_set(self, selected):
        if selected:
            elm_list_item_selected_set(self.item, 1)
        else:
            elm_list_item_selected_set(self.item, 0)
    
    def show(self):
        elm_list_item_show(self.item)
        
    def delete(self):
        elm_list_item_del(self.item)
        
    def data_get(self):
        cdef void* data
        data = elm_list_item_data_get(self.item)
        return None
        
    def icon_get(self):
        return None
    
    def end_get(self):
        return None

cdef class List(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_list_add(parent.obj))
    
    def item_append(self, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        item = ListItem()
        item.append(self, label, icon, end, callback, data)
        return item
    
    def item_prepend(self, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        item = ListItem()
        item.prepend(self, label, icon, end, data)
        return item
    
    def item_insert_before(self, ListItem before, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        item = ListItem()
        item.insert_before(self, before, label, icon, end, callback, data)
        return item
    
    def item_insert_after(self, ListItem after, label, c_evas.Object icon, c_evas.Object end, callback, data = None):
        item = ListItem()
        item.insert_after(self, after, label, icon, end, callback, data)
        return item
