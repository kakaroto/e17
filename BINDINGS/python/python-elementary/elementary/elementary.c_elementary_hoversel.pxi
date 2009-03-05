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

cdef object _hoversel_callback_mapping
_hoversel_callback_mapping = dict()

cdef void _hoversel_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _hoversel_callback_mapping.get(<long>event_info)
        if mapping is not None:
            callback = mapping["callback"] 
            if callback is not None and callable(callback):
                callback(mapping["class"], "clicked")
        else:
            print "ERROR: no callback available for the hoversel-item"
    except Exception, e:
        traceback.print_exc()

cdef class HoverselItem:
    """A item for the hoversel widget"""
    cdef Elm_Hoversel_Item *item

    def __init__(self, c_evas.Object hoversel, label, icon_file, icon_type, callback, data = None):
        self.item = elm_hoversel_item_add(hoversel.obj, label, icon_file, icon_type,_hoversel_callback, NULL)
       
        # Create the mapping
        mapping = dict()
        mapping["class"] = hoversel
        mapping["callback"] = callback
        _hoversel_callback_mapping[<long>self.item] = mapping

    def delete(self):
        """Delete the hoversel item"""
        elm_hoversel_item_del(self.item)

cdef class Hoversel(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_hoversel_add(parent.obj))

    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)
    
    property selected:
        def __set__(self, value):
            self._callback_add("selected", value)
            
    property dismissed:
        def __set__(self, value):
            self._callback_add("dismissed", value)
                
    def hover_parent_set(self, c_evas.Object parent):
        elm_hoversel_hover_parent_set(self.obj, parent.obj)
    
    def label_set(self, label):
        elm_hoversel_label_set(self.obj, label)
        
    def icon_set(self, c_evas.Object icon):
        elm_hoversel_icon_set(self.obj, icon.obj)
    
    def hover_end(self):
        elm_hoversel_hover_end(self.obj)
    
    def item_add(self, label, icon_file, icon_type, callback):
        return HoverselItem(self, label, icon_file, icon_type, callback)


