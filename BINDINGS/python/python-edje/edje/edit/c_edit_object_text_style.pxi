# Copyright (C) 2007-2008 ProFUSION embedded systems
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje. If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.edit.pyx

cdef class Text_Style:
    cdef EdjeEdit edje
    cdef object name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self.name = name

    property tags:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_style_tags_list_get(self.edje.obj, self.name)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def tag_get(self, char *name):
        return Text_Style_Tag(self, name)

    def tag_add(self, char *name):
        return bool(edje_edit_style_tag_add(self.edje.obj, self.name, name))

    def tag_del(self, char *name):
        edje_edit_style_tag_del(self.edje.obj, self.name, name)
        return True

cdef class Text_Style_Tag:
    cdef Text_Style text_style
    cdef object name

    def __init__(self, Text_Style text_style, char *name):
        self.text_style = text_style
        self.name = name

    property value:
        def __get__(self):
            cdef char *val
            val =  edje_edit_style_tag_value_get(self.text_style.edje.obj, self.text_style.name, self.name)
            if val == NULL: return None
            ret = val
            edje_edit_string_free(val)
            return ret
        def __set__(self, value):
            edje_edit_style_tag_value_set(self.text_style.edje.obj, self.text_style.name, self.name, value)

    def rename(self, char *newname):
        edje_edit_style_tag_name_set(self.text_style.edje.obj, self.text_style.name, self.name, newname)
        self.name = newname
        return True

