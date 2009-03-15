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

cdef class Entry(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_entry_add(parent.obj))

    property changed:
        def __set__(self, value):
            self._callback_add("changed",value)
            
    property selection_start:
        def __set__(self, value):
            self._callback_add("selection,start",value)
        
    property selection_changed:
        def __set__(self, value):
            self._callback_add("selection,changed",value)
    
    property selection_cleared:
        def __set__(self, value):
            self._callback_add("selection,cleared",value)
    
    property selection_paste:
        def __set__(self, value):
            self._callback_add("selection,paste",value)
            
    property selection_copy:
        def __set__(self, value):
            self._callback_add("selection,copy",value)
            
    property selection_cut:
        def __set__(self, value):
            self._callback_add("selection,cut",value)
            
    property cursor_changed:
        def __set__(self, value):
            self._callback_add("cursor,changed",value)
    
    property anchor_clicked:
        def __set__(self, value):
            self._callback_add("anchor,clicked",value)
            
    property activated:
        def __set__(self, value):
            self._callback_add("activated",value)
        
    def single_line_set(self, single_line):
        elm_entry_single_line_set(self.obj, single_line)
        
    def password_set(self, password):
        elm_entry_password_set(self.obj, password)
        
    def entry_set(self, entry):
        elm_entry_entry_set(self.obj, entry)
        
    def entry_get(self):
        return elm_entry_entry_get(self.obj)
        
    def selection_get(self):
        cdef char* str
        str = elm_entry_selection_get(self.obj)
        if str == NULL:
            return ""
        return str
        
    def entry_insert(self, entry):
        elm_entry_entry_insert(self.obj, entry)
        
    def line_wrap_set(self, wrap):
        elm_entry_line_wrap_set(self.obj, wrap)
        
    def editable_set(self, editable):
        elm_entry_editable_set(self.obj, editable)

    def select_none(self):
        elm_entry_select_none(self.obj)

    def select_all(self):
        elm_entry_select_all(self.obj)
    
    @staticmethod
    def markup_to_utf8(str):
        cdef char* string
        string = elm_entry_markup_to_utf8(str)
        return string
    
    @staticmethod
    def markup_to_utf8(str):
        cdef char* string
        string = elm_entry_utf8_to_markup(str)
        return string
    
 
