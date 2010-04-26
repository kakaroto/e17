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
cimport evas.c_evas as c_evas

cdef class EdjeEdit(edje.c_edje.Edje): # [object PyEdjeEdit, type PyEdjeEdit_Type]:
    def __init__(self, c_evas.Canvas canvas not None, **kargs):
        evas.c_evas.Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(edje_edit_object_add(self.evas.obj))
        self._set_common_params(**kargs)

    def compiler_get(self):
        """@rtype: str"""
        cdef char *s
        s = edje_edit_compiler_get(self.obj)
        if s != NULL:
            r = s
            edje_edit_string_free(s)
            return r
        return None

    def save(self):
        """@rtype: bool"""
        return bool(edje_edit_save(self.obj))

    def save_all(self):
        """@rtype: bool"""
        return bool(edje_edit_save_all(self.obj))

    def print_internal_status(self):
        edje_edit_print_internal_status(self.obj)

    # Group

    property current_group:
        """@rtype: bool"""
        def __get__(self):
            return Group(self)

    def group_add(self, char *name):
        """@rtype: bool"""
        return bool(edje_edit_group_add(self.obj, name))

    def group_del(self, char *name):
        """@rtype: bool"""
        return bool(edje_edit_group_del(self.obj, name))

    def group_exist(self, char *name):
        """@rtype: bool"""
        return bool(edje_edit_group_exist(self.obj, name))

    # Data

    property data:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_data_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def data_get(self, char *name):
        cdef char *val
        val = edje_edit_data_value_get(self.obj, name)
        if val == NULL:
            return None
        r = val
        edje_edit_string_free(val)
        return r

    def data_set(self, char *name, char *value):
        return bool(edje_edit_data_value_set(self.obj, name, value))

    def data_add(self, char *name, char *value):
        return bool(edje_edit_data_add(self.obj, name, value))

    def data_rename(self, char *old, char *new):
        return bool(edje_edit_data_name_set(self.obj, old, new))

    def data_del(self, char *name):
        return bool(edje_edit_data_del(self.obj, name))

    def group_data_get(self, char *name):
        cdef char *val
        val = edje_edit_group_data_value_get(self.obj, name)
        if val == NULL:
            return None
        r = val
        edje_edit_string_free(val)
        return r

    def group_data_set(self, char *name, char *value):
        return bool(edje_edit_group_data_value_set(self.obj, name, value))

    def group_data_add(self, char *name, char *value):
        return bool(edje_edit_group_data_add(self.obj, name, value))

    def group_data_rename(self, char *old, char *new):
        return bool(edje_edit_group_data_name_set(self.obj, old, new))

    def group_data_del(self, char *name):
        return bool(edje_edit_group_data_del(self.obj, name))

    # Text Style

    property text_styles:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_styles_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def text_style_get(self, char *name):
        return Text_Style(self, name)

    def text_style_add(self, char *name):
        return bool(edje_edit_style_add(self.obj, name))

    def text_style_del(self, char *name):
        edje_edit_style_del(self.obj, name)
        return True

    # Color Class

    property color_classes:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_color_classes_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def color_class_get(self, char *name):
        return Color_Class(self, name)

    def color_class_add(self, char *name):
        return bool(edje_edit_color_class_add(self.obj, name))

    def color_class_del(self, char *name):
        return bool(edje_edit_color_class_del(self.obj, name))

    # External
    property externals:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_externals_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def external_add(self, char *name):
        return bool(edje_edit_external_add(self.obj, name))

    def external_del(self, char *name):
        return bool(edje_edit_external_del(self.obj, name))

    # Part

    property parts:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_parts_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def part_get(self, char *name):
        if self.part_exist(name):
            return Part(self, name)
        return None

    def part_add(self, char *name, int type, char *source=""):
        if type != edje.EDJE_PART_TYPE_EXTERNAL:
            return bool(edje_edit_part_add(
                self.obj, name, <edje.c_edje.Edje_Part_Type>type))
        else:
            return bool(edje_edit_part_external_add(self.obj, name, source))

    def part_del(self, char *name):
        return bool(edje_edit_part_del(self.obj, name))

    def part_exist(self, char *name):
        return bool(edje_edit_part_exist(self.obj, name))

    property fonts:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_fonts_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def font_add(self, char *font, char *alias=NULL):
        return bool(edje_edit_font_add(self.obj, font, alias))

    def font_del(self, char *alias):
        return bool(edje_edit_font_del(self.obj, alias))

    # Image

    property images:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_images_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def image_id_get(self, char *image):
        return edje_edit_image_id_get(self.obj, image)

    def image_add(self, char *image):
        return bool(edje_edit_image_add(self.obj, image))

    def image_del(self, char *name):
        return bool(edje_edit_image_del(self.obj, name))

    # Spectrum

    property spectrum:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_spectrum_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def spectra_get(self, char *name):
        return Spectra(self, name)

    def spectra_add(self, char *name):
        return bool(edje_edit_spectra_add(self.obj, name))

    def spectra_del(self, char *name):
        return bool(edje_edit_spectra_del(self.obj, name))

    # Programs

    property programs:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_programs_list_get(self.obj)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def program_get(self, char *name):
        if self.program_exist(name):
            return Program(self, name)
        return None

    def program_add(self, char *name):
        return bool(edje_edit_program_add(self.obj, name))

    def program_del(self, char *name):
        return bool(edje_edit_program_del(self.obj, name))

    def program_exist(self, char *name):
        return bool(edje_edit_program_exist(self.obj, name))
