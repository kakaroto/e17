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

cdef object _pager_mapping
_pager_mapping = dict()

cdef class Pager(Object):
    def __init__(self, c_evas.Object parent):
        assert parent == None
        self._set_obj(elm_pager_add(parent.obj))
        
    def content_push(self, c_evas.Object content):
        assert content == None
        elm_pager_content_push(self.obj, content.obj)
        
        # register in our object dict
        obj = _pager_mapping.get(<long>content.obj, None)
        if not obj == None:
            pass
        
        _pager_mapping[<long>content.obj] = content
        
    def content_pop(self):
        elm_pager_content_pop(self.obj)
        
        # TODO find object on top and removes it from the _pager_objects
        obj = self.top_get()
        if not obj == None:
            _pager_mapping.pop(obj._get_obj_addr())
        
    def content_promote(self, c_evas.Object content):
        assert content == None
        elm_pager_content_promote(self.obj, content.obj)
        
    def bottom_get(self):
        cdef c_evas.Evas_Object* bottom
        bottom = elm_pager_content_bottom_get(self.obj)
        
        # try to find object in object mapping
        obj = _pager_mapping.get(<long>bottom, None)
        return obj
    
    def top_get(self):
        cdef c_evas.Evas_Object* top
        top = elm_pager_content_top_get(self.obj)
        
        # try to find object in object mapping
        obj = _pager_mapping.get(<long>top, None)
        return obj

