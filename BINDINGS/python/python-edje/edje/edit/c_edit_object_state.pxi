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


cdef class State:
    cdef EdjeEdit edje
    cdef object part
    cdef object _name
    cdef object _part_obj
    cdef object _value

    def __init__(self, Part part, char *name, double value=0.0):
        self._part_obj = part
        self.edje = part.edje
        self.part = part.name
        self._name = name
        self._value = value

    property name:
        def __get__(self):
            return self._name

    property value:
        def __get__(self):
            return self._value

    def part_get(self):
        return self._part_obj

    def name_set(self, new_name, new_value=None):
        if new_value == None:
            new_value = self.value
        return bool(edje_edit_state_name_set(self.edje.obj, self.part,
                                             self._name, self._value, new_name,
                                             new_value))

    def copy_from(self, from_state, from_value=0.0):
        return bool(edje_edit_state_copy(self.edje.obj, self.part,
                                         from_state, from_value, self.name,
                                         self.value))

    def rel1_to_get(self):
        cdef char *tx, *ty
        tx = edje_edit_state_rel1_to_x_get(self.edje.obj, self.part, self.name,
                                           self.value)
        ty = edje_edit_state_rel1_to_y_get(self.edje.obj, self.part, self.name,
                                           self.value)
        if tx != NULL:
            x = tx
            edje_edit_string_free(tx)
        else:
            x = None

        if ty != NULL:
            y = ty
            edje_edit_string_free(ty)
        else:
            y = None

        return (x, y)

    def rel1_to_set(self, x, y):
        if x:
            edje_edit_state_rel1_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        else:
            edje_edit_state_rel1_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        if y:
            edje_edit_state_rel1_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)
        else:
            edje_edit_state_rel1_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)

    property rel1_to:
        def __get__(self):
            return self.rel1_to_get()
        def __set__(self, rel1_to):
            self.rel1_to_set(*rel1_to)

    def rel1_to_x_set(self, x):
        if x:
            edje_edit_state_rel1_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        else:
            edje_edit_state_rel1_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)

    def rel1_to_y_set(self, y):
        if y:
            edje_edit_state_rel1_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)
        else:
            edje_edit_state_rel1_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)

    def rel1_relative_get(self):
        cdef double x, y
        x = edje_edit_state_rel1_relative_x_get(self.edje.obj, self.part,
                                                self.name, self.value)
        y = edje_edit_state_rel1_relative_y_get(self.edje.obj, self.part,
                                                self.name, self.value)
        return (x, y)

    def rel1_relative_set(self, double x, double y):
        edje_edit_state_rel1_relative_x_set(self.edje.obj, self.part, self.name,
                                            self.value, x)
        edje_edit_state_rel1_relative_y_set(self.edje.obj, self.part, self.name,
                                            self.value, y)

    property rel1_relative:
        def __get__(self):
            return self.rel1_relative_get()
        def __set__(self, value):
            self.rel1_relative_set(*value)

    def rel1_relative_x_set(self, double x):
        edje_edit_state_rel1_relative_x_set(self.edje.obj, self.part, self.name,
                                            self.value, x)

    def rel1_relative_y_set(self, double y):
        edje_edit_state_rel1_relative_y_set(self.edje.obj, self.part, self.name,
                                            self.value, y)

    def rel1_offset_get(self):
        cdef int x, y
        x = edje_edit_state_rel1_offset_x_get(self.edje.obj, self.part,
                                              self.name, self.value)
        y = edje_edit_state_rel1_offset_y_get(self.edje.obj, self.part,
                                              self.name, self.value)
        return (x, y)

    def rel1_offset_set(self, int x, int y):
        edje_edit_state_rel1_offset_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        edje_edit_state_rel1_offset_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)

    property rel1_offset:
        def __get__(self):
            return self.rel1_offset_get()
        def __set__(self, value):
            self.rel1_offset_set(*value)

    def rel1_offset_x_set(self, int x):
        edje_edit_state_rel1_offset_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)

    def rel1_offset_y_set(self, int y):
        edje_edit_state_rel1_offset_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)

    def rel2_to_get(self):
        cdef char *tx, *ty
        tx = edje_edit_state_rel2_to_x_get(self.edje.obj, self.part, self.name,
                                           self.value)
        ty = edje_edit_state_rel2_to_y_get(self.edje.obj, self.part, self.name,
                                           self.value)
        if tx != NULL:
            x = tx
            edje_edit_string_free(tx)
        else:
            x = None

        if ty != NULL:
            y = ty
            edje_edit_string_free(ty)
        else:
            y = None

        return (x, y)

    def rel2_to_set(self, x, y):
        if x:
            edje_edit_state_rel2_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        else:
            edje_edit_state_rel2_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        if y:
            edje_edit_state_rel2_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)
        else:
            edje_edit_state_rel2_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)

    property rel2_to:
        def __get__(self):
            return self.rel2_to_get()
        def __set__(self, rel2_to):
            self.rel2_to_set(*rel2_to)

    def rel2_to_x_set(self, x):
        if x:
            edje_edit_state_rel2_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        else:
            edje_edit_state_rel2_to_x_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)
    def rel2_to_y_set(self, y):
        if y:
            edje_edit_state_rel2_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)
        else:
            edje_edit_state_rel2_to_y_set(self.edje.obj, self.part, self.name,
                                          self.value, NULL)
        # remove when fixed in edje_edit
        edje_edit_part_selected_state_set(self.edje.obj, self.part, self.name,
                                          self.value)

    def rel2_relative_get(self):
        cdef double x, y
        x = edje_edit_state_rel2_relative_x_get(self.edje.obj, self.part,
                                                self.name, self.value)
        y = edje_edit_state_rel2_relative_y_get(self.edje.obj, self.part,
                                                self.name, self.value)
        return (x, y)

    def rel2_relative_set(self, double x, double y):
        edje_edit_state_rel2_relative_x_set(self.edje.obj, self.part, self.name,
                                            self.value, x)
        edje_edit_state_rel2_relative_y_set(self.edje.obj, self.part, self.name,
                                            self.value, y)

    property rel2_relative:
        def __get__(self):
            return self.rel2_relative_get()
        def __set__(self, value):
            self.rel2_relative_set(*value)

    def rel2_relative_x_set(self, double x):
        edje_edit_state_rel2_relative_x_set(self.edje.obj, self.part, self.name,
                                            self.value, x)
    def rel2_relative_y_set(self, double y):
        edje_edit_state_rel2_relative_y_set(self.edje.obj, self.part, self.name,
                                            self.value, y)

    def rel2_offset_get(self):
        cdef int x, y
        x = edje_edit_state_rel2_offset_x_get(self.edje.obj, self.part,
                                              self.name, self.value)
        y = edje_edit_state_rel2_offset_y_get(self.edje.obj, self.part,
                                              self.name, self.value)
        return (x, y)

    def rel2_offset_set(self, int x, int y):
        edje_edit_state_rel2_offset_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
        edje_edit_state_rel2_offset_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)

    property rel2_offset:
        def __get__(self):
            return self.rel2_offset_get()
        def __set__(self, value):
            self.rel2_offset_set(*value)

    def rel2_offset_x_set(self, int x):
        edje_edit_state_rel2_offset_x_set(self.edje.obj, self.part, self.name,
                                          self.value, x)
    def rel2_offset_y_set(self, int y):
        edje_edit_state_rel2_offset_y_set(self.edje.obj, self.part, self.name,
                                          self.value, y)

    def color_get(self):
        cdef int r, g, b, a
        edje_edit_state_color_get(self.edje.obj, self.part, self.name,
                                  self.value, &r, &g, &b, &a)
        return (r, g, b, a)

    def color_set(self, int r, int g, int b, int a):
        edje_edit_state_color_set(self.edje.obj, self.part, self.name,
                                  self.value, r, g, b, a)

    property color:
        def __get__(self):
            return self.color_get()
        def __set__(self, color):
            self.color_set(*color)

    def color2_get(self):
        cdef int r, g, b, a
        edje_edit_state_color2_get(self.edje.obj, self.part, self.name,
                                   self.value, &r, &g, &b, &a)
        return (r, g, b, a)

    def color2_set(self, int r, int g, int b, int a):
        edje_edit_state_color2_set(self.edje.obj, self.part, self.name,
                                   self.value, r, g, b, a)

    property color2:
        def __get__(self):
            return self.color2_get()
        def __set__(self, color):
            self.color2_set(*color)

    def color3_get(self):
        cdef int r, g, b, a
        edje_edit_state_color3_get(self.edje.obj, self.part, self.name,
                                   self.value, &r, &g, &b, &a)
        return (r, g, b, a)

    def color3_set(self, int r, int g, int b, int a):
        edje_edit_state_color3_set(self.edje.obj, self.part, self.name,
                                   self.value, r, g, b, a)

    property color3:
        def __get__(self):
            return self.color3_get()
        def __set__(self, color):
            self.color3_set(*color)

    def align_get(self):
        cdef double x, y
        x = edje_edit_state_align_x_get(self.edje.obj, self.part, self.name,
                                        self.value)
        y = edje_edit_state_align_y_get(self.edje.obj, self.part, self.name,
                                        self.value)
        return (x, y)

    def align_set(self, double x, double y):
        edje_edit_state_align_x_set(self.edje.obj, self.part, self.name,
                                    self.value, x)
        edje_edit_state_align_y_set(self.edje.obj, self.part, self.name,
                                    self.value, y)

    property align:
        def __get__(self):
            return self.align_get()
        def __set__(self, align):
            self.align_set(*align)

    def min_get(self):
        cdef int w, h
        w = edje_edit_state_min_w_get(self.edje.obj, self.part, self.name,
                                      self.value)
        h = edje_edit_state_min_h_get(self.edje.obj, self.part, self.name,
                                      self.value)
        return (w, h)

    def min_set(self, int w, int h):
        edje_edit_state_min_w_set(self.edje.obj, self.part, self.name,
                                  self.value, w)
        edje_edit_state_min_h_set(self.edje.obj, self.part, self.name,
                                  self.value, h)

    property min:
        def __get__(self):
            return self.min_get()
        def __set__(self, min):
            self.min_set(*min)

    def max_get(self):
        cdef int w, h
        w = edje_edit_state_max_w_get(self.edje.obj, self.part, self.name,
                                      self.value)
        h = edje_edit_state_max_h_get(self.edje.obj, self.part, self.name,
                                      self.value)
        return (w, h)

    def max_set(self, int w, int h):
        edje_edit_state_max_w_set(self.edje.obj, self.part, self.name,
                                  self.value, w)
        edje_edit_state_max_h_set(self.edje.obj, self.part, self.name,
                                  self.value, h)

    property max:
        def __get__(self):
            return self.max_get()
        def __set__(self, max):
            self.max_set(*max)

    def aspect_min_get(self):
        return edje_edit_state_aspect_min_get(self.edje.obj, self.part,
                                              self.name, self.value)

    def aspect_min_set(self, double a):
        edje_edit_state_aspect_min_set(self.edje.obj, self.part, self.name,
                                       self.value, a)

    def aspect_max_get(self):
        return edje_edit_state_aspect_max_get(self.edje.obj, self.part,
                                              self.name, self.value)

    def aspect_max_set(self, double a):
        edje_edit_state_aspect_max_set(self.edje.obj, self.part, self.name,
                                       self.value, a)

    def aspect_pref_get(self):
        return edje_edit_state_aspect_pref_get(self.edje.obj, self.part,
                                               self.name, self.value)

    def aspect_pref_set(self, char a):
        edje_edit_state_aspect_pref_set(self.edje.obj, self.part, self.name,
                                        self.value, a)

    def fill_origin_relative_get(self):
        cdef double x, y
        x = edje_edit_state_fill_origin_relative_x_get(self.edje.obj, self.part,
                                                       self.name, self.value)
        y = edje_edit_state_fill_origin_relative_y_get(self.edje.obj, self.part,
                                                       self.name, self.value)
        return (x, y)

    def fill_origin_relative_set(self, double x, double y):
        edje_edit_state_fill_origin_relative_x_set(self.edje.obj, self.part,
                                                   self.name, self.value, x)
        edje_edit_state_fill_origin_relative_y_set(self.edje.obj, self.part,
                                                   self.name, self.value, y)

    def fill_origin_offset_get(self):
        cdef int x, y
        x = edje_edit_state_fill_origin_offset_x_get(self.edje.obj, self.part,
                                                     self.name, self.value)
        y = edje_edit_state_fill_origin_offset_y_get(self.edje.obj, self.part,
                                                     self.name, self.value)
        return (x, y)

    def fill_origin_offset_set(self, x, y):
        edje_edit_state_fill_origin_offset_x_set(self.edje.obj, self.part,
                                                 self.name, self.value, x)
        edje_edit_state_fill_origin_offset_y_set(self.edje.obj, self.part,
                                                 self.name, self.value, y)

    def fill_size_relative_get(self):
        cdef double x, y
        x = edje_edit_state_fill_size_relative_x_get(self.edje.obj, self.part,
                                                     self.name, self.value)
        y = edje_edit_state_fill_size_relative_y_get(self.edje.obj, self.part,
                                                     self.name, self.value)
        return (x, y)

    def fill_size_relative_set(self, double x, double y):
        edje_edit_state_fill_size_relative_x_set(self.edje.obj, self.part,
                                                 self.name, self.value, x)
        edje_edit_state_fill_size_relative_y_set(self.edje.obj, self.part,
                                                 self.name, self.value, y)

    def fill_size_offset_get(self):
        cdef int x, y
        x = edje_edit_state_fill_size_offset_x_get(self.edje.obj, self.part,
                                                   self.name, self.value)
        y = edje_edit_state_fill_size_offset_y_get(self.edje.obj, self.part,
                                                   self.name, self.value)
        return (x, y)

    def fill_size_offset_set(self, x, y):
        edje_edit_state_fill_size_offset_x_set(self.edje.obj, self.part,
                                               self.name, self.value, x)
        edje_edit_state_fill_size_offset_y_set(self.edje.obj, self.part,
                                               self.name, self.value, y)

    property visible:
        def __get__(self):
            return bool(edje_edit_state_visible_get(self.edje.obj, self.part,
                                                    self.name, self.value))
        def __set__(self, v):
            if v:
                edje_edit_state_visible_set(self.edje.obj, self.part, self.name,
                                            self.value, 1)
            else:
                edje_edit_state_visible_set(self.edje.obj, self.part, self.name,                                            self.value, 0)

    def color_class_get(self):
        cdef char *cc
        cc = edje_edit_state_color_class_get(self.edje.obj, self.part,
                                             self.name, self.value)
        if cc == NULL:
            return None
        rcc = cc
        edje_edit_string_free(cc)
        return rcc

    def color_class_set(self, cc):
        if not cc:
            edje_edit_state_color_class_set(self.edje.obj, self.part,
                                            self.name, self.value, NULL)
        else:
            edje_edit_state_color_class_set(self.edje.obj, self.part,
                                            self.name, self.value, cc)

    def external_params_get(self):
        cdef evas.c_evas.Eina_List *lst
        ret = []
        lst = edje_edit_state_external_params_list_get(self.edje.obj, self.part,
                                                       self.name, self.value)
        while lst:
            p = edje.c_edje._ExternalParam_from_ptr(<long>lst.data)
            if p is not None:
                ret.append(p)
            lst = lst.next
        return ret

    def external_param_get(self, param):
        cdef edje.c_edje.Edje_External_Param_Type type
        cdef void *value
        cdef char *s

        if not edje_edit_state_external_param_get(self.edje.obj, self.part,
                                                  self.name, self.value, param,
                                                  &type, &value):
            return None
        if type == edje.EDJE_EXTERNAL_PARAM_TYPE_BOOL:
            b = (<evas.c_evas.Eina_Bool *>value)[0]
            return (type, bool(b))
        elif type == edje.EDJE_EXTERNAL_PARAM_TYPE_INT:
            i = (<int *>value)[0]
            return (type, i)
        elif type == edje.EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
            d = (<double *>value)[0]
            return (type, d)
        elif type == edje.EDJE_EXTERNAL_PARAM_TYPE_STRING:
            s = <char *>value
            if s == NULL:
                return (type, None)
            else:
                return (type, s)
        elif type == edje.EDJE_EXTERNAL_PARAM_TYPE_CHOICE:
            s = <char *>value
            if s == NULL:
                return (type, None)
            else:
                return (type, s)
        return None

    def external_param_set(self, param, value):
        if isinstance(value, bool):
            return self.external_param_bool_set(param, value)
        elif isinstance(value, (long, int)):
            return self.external_param_int_set(param, value)
        elif isinstance(value, float):
            return self.external_param_double_set(param, value)
        elif isinstance(value, basestring):
            t = edje_object_part_external_param_type_get(
                self.edje.obj, self.part, param)
            if t == edje.EDJE_EXTERNAL_PARAM_TYPE_STRING:
                return self.external_param_string_set(param, value)
            elif t == edje.EDJE_EXTERNAL_PARAM_TYPE_CHOICE:
                return self.external_param_choice_set(param, value)

        t = edje_object_part_external_param_type_get(
            self.edje.obj, self.part, param)
        if t >= edje.EDJE_EXTERNAL_PARAM_TYPE_MAX:
            raise TypeError("no external parameter %s" % (param,))
        else:
            expected = edje_external_param_type_str(t)
            raise TypeError(
                "invalid external parameter %s of (%s), expected %s" %
                (param, type(value).__name__, expected))

    def external_param_int_get(self, param):
        cdef int value

        if not edje_edit_state_external_param_int_get(self.edje.obj, self.part,
                                                      self.name, self.value,
                                                      param, &value):
            return None
        return value

    def external_param_bool_get(self, param):
        cdef evas.c_evas.Eina_Bool value

        if not edje_edit_state_external_param_bool_get(
            self.edje.obj, self.part, self.name, self.value, param, &value):
            return None
        return bool(value)

    def external_param_double_get(self, param):
        cdef double value

        if not edje_edit_state_external_param_double_get(self.edje.obj, self.part,
                                                      self.name, self.value,
                                                      param, &value):
            return None
        return value

    def external_param_string_get(self, param):
        cdef char *value

        if not edje_edit_state_external_param_string_get(self.edje.obj, self.part,
                                                      self.name, self.value,
                                                      param, &value):
            return None
        if value != NULL:
            return value

    def external_param_choice_get(self, param):
        cdef char *value

        if not edje_edit_state_external_param_choice_get(
            self.edje.obj, self.part, self.name, self.value, param, &value):
            return None
        if value != NULL:
            return value

    def external_param_int_set(self, param, value):
        return bool(edje_edit_state_external_param_int_set(self.edje.obj,
                                                           self.part, self.name,
                                                           self.value, param,
                                                           value))

    def external_param_bool_set(self, param, value):
        return bool(edje_edit_state_external_param_bool_set(
                self.edje.obj, self.part, self.name, self.value, param, value))

    def external_param_double_set(self, param, value):
        return bool(edje_edit_state_external_param_double_set(self.edje.obj,
                                                           self.part, self.name,
                                                           self.value,param,
                                                           value))

    def external_param_string_set(self, param, value):
        return bool(edje_edit_state_external_param_string_set(self.edje.obj,
                                                           self.part, self.name,
                                                           self.value, param,
                                                           value))

    def external_param_choice_set(self, param, value):
        return bool(edje_edit_state_external_param_choice_set(
                self.edje.obj, self.part, self.name, self.value, param, value))

    def text_get(self):
        cdef char *t
        t = edje_edit_state_text_get(self.edje.obj, self.part, self.name,
                                     self.value)
        if t == NULL:
            return None
        r = t
        edje_edit_string_free(t)
        return r

    def text_set(self, t):
        edje_edit_state_text_set(self.edje.obj, self.part, self.name,
                                 self.value, t)

    property text:
        def __get__(self):
            return self.text_get()
        def __set__(self, text):
            self.text_set(text)

    def font_get(self):
        cdef char *f
        f = edje_edit_state_font_get(self.edje.obj, self.part, self.name,
                                     self.value)
        if f == NULL:
            return None
        r = f
        edje_edit_string_free(f)
        return r

    def font_set(self, char *f):
        edje_edit_state_font_set(self.edje.obj, self.part, self.name,
                                 self.value, f)

    property font:
        def __get__(self):
            return self.font_get()
        def __set__(self, font):
            self.font_set(font)

    def text_size_get(self):
        return edje_edit_state_text_size_get(self.edje.obj, self.part,
                                             self.name, self.value)

    def text_size_set(self, int s):
        edje_edit_state_text_size_set(self.edje.obj, self.part, self.name,
                                      self.value, s)

    property text_size:
        def __get__(self):
            return self.text_size_get()
        def __set__(self, value):
            self.text_size_set(value)

    def text_align_get(self):
        cdef double x, y
        x = edje_edit_state_text_align_x_get(self.edje.obj, self.part,
                                             self.name, self.value)
        y = edje_edit_state_text_align_y_get(self.edje.obj, self.part,
                                             self.name, self.value)
        return (x, y)

    def text_align_set(self, double x, double y):
        edje_edit_state_text_align_x_set(self.edje.obj, self.part, self.name,
                                         self.value, x)
        edje_edit_state_text_align_y_set(self.edje.obj, self.part, self.name,
                                         self.value, y)

    property text_align:
        def __get__(self):
            return self.text_align_get()
        def __set__(self, align):
            self.text_align_set(*align)

    def text_elipsis_get(self):
        return edje_edit_state_text_elipsis_get(self.edje.obj, self.part,
                                                self.name, self.value)

    def text_elipsis_set(self, double e):
        edje_edit_state_text_elipsis_set(self.edje.obj, self.part, self.name,
                                         self.value, e)

    property text_elipsis:
        def __get__(self):
            return self.text_elipsis_get()
        def __set__(self, value):
            self.text_elipsis_set(value)

    def text_fit_get(self):
        x = bool(edje_edit_state_text_fit_x_get(self.edje.obj, self.part,
                                                self.name, self.value))
        y = bool(edje_edit_state_text_fit_y_get(self.edje.obj, self.part,
                                                self.name, self.value))
        return (x, y)

    def text_fit_set(self, x, y):
        if x:
            edje_edit_state_text_fit_x_set(self.edje.obj, self.part, self.name,
                                           self.value, 1)
        else:
            edje_edit_state_text_fit_x_set(self.edje.obj, self.part, self.name,
                                           self.value, 0)
        if y:
            edje_edit_state_text_fit_y_set(self.edje.obj, self.part, self.name,
                                           self.value, 1)
        else:
            edje_edit_state_text_fit_y_set(self.edje.obj, self.part, self.name,
                                           self.value, 0)

    property text_fit:
        def __get__(self):
            return self.text_fit_get()
        def __set__(self, value):
            self.text_fit_set(*value)

    def image_get(self):
        cdef char *img
        img = edje_edit_state_image_get(self.edje.obj, self.part, self.name,
                                        self.value)
        if img == NULL:
            return None
        r = img
        edje_edit_string_free(img)
        return r

    def image_set(self, char *image):
        if not image:
            return

        edje_edit_state_image_set(self.edje.obj, self.part, self.name,
                                  self.value, image)

    property image:
        def __get__(self):
            return self.image_get()
        def __set__(self, image):
            self.image_set(image)

    def image_border_get(self):
        cdef int l, r, t, b
        edje_edit_state_image_border_get(self.edje.obj, self.part, self.name,
                                         self.value, &l, &r, &t, &b)
        return (l, r, t, b)

    def image_border_set(self, int l, int r, int t, int b):
        edje_edit_state_image_border_set(self.edje.obj, self.part, self.name,
                                         self.value, l, r, t, b)

    property image_border:
        def __get__(self):
            return self.image_border_get()
        def __set__(self, value):
            self.image_border_set(*value)

    def image_border_fill_get(self):
        cdef unsigned char r
        r = edje_edit_state_image_border_fill_get(self.edje.obj, self.part,
                                                  self.name, self.value)
        if r == 0:
            return False
        return True

    def image_border_fill_set(self, fill):
        if fill:
            edje_edit_state_image_border_fill_set(self.edje.obj, self.part,
                                                  self.name, self.value, 1)
        else:
            edje_edit_state_image_border_fill_set(self.edje.obj, self.part,
                                                  self.name, self.value, 0)

    property image_border_fill:
        def __get__(self):
            return self.image_border_fill_get()
        def __set__(self, value):
            self.image_border_fill_set(value)

    property tweens:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_state_tweens_list_get(self.edje.obj, self.part,
                                                  self.name, self.value)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def tween_add(self, char *img):
        return bool(edje_edit_state_tween_add(self.edje.obj, self.part,
                                              self.name, self.value, img))

    def tween_del(self, char *img):
        return bool(edje_edit_state_tween_del(self.edje.obj, self.part,
                                              self.name, self.value, img))

    def gradient_type_get(self):
        cdef char *t
        t = edje_edit_state_gradient_type_get(self.edje.obj, self.part,
                                              self.name, self.value)
        if t == NULL:
            return None
        ret = t
        edje_edit_string_free(t)
        return ret

    def gradient_type_set(self, char *type_):
        return bool(edje_edit_state_gradient_type_set(self.edje.obj, self.part,
                                                      self.name, self.value,
                                                      type_))

    property gradient_type:
        def __get__(self):
            return self.gradient_type_get()
        def __set__(self, type_):
            self.gradient_type_set(type_)

    def gradient_spectra_get(self):
        cdef char *s
        s = edje_edit_state_gradient_spectra_get(self.edje.obj, self.part,
                                                 self.name, self.value)
        if s == NULL:
            return None
        ret = s
        edje_edit_string_free(s)
        return ret

    def gradient_spectra_set(self, char *s):
        edje_edit_state_gradient_spectra_set(self.edje.obj, self.part,
                                             self.name, self.value, s)

    property gradient_spectra:
        def __get__(self):
            return self.gradient_spectra_get()
        def __set__(self, value):
            self.gradient_spectra_set(value)

    def gradient_angle_get(self):
        return edje_edit_state_gradient_angle_get(self.edje.obj, self.part,
                                                  self.name, self.value)

    def gradient_angle_set(self, int angle):
        edje_edit_state_gradient_angle_set(self.edje.obj, self.part, self.name,
                                           self.value, angle)

    property gradient_angle:
        def __get__(self):
            return self.gradient_angle_get()
        def __set__(self, value):
            self.gradient_angle_set(value)

    def gradient_use_fill_get(self):
        return bool(edje_edit_state_gradient_use_fill_get(self.edje.obj,
                                                          self.part, self.name,
                                                          self.value))

    def gradient_rel1_relative_get(self):
        cdef double x, y
        x = edje_edit_state_gradient_rel1_relative_x_get(self.edje.obj,
                                                         self.part, self.name,
                                                         self.value)
        y = edje_edit_state_gradient_rel1_relative_y_get(self.edje.obj,
                                                         self.part, self.name,
                                                         self.value)
        return (x, y)

    def gradient_rel1_relative_set(self, double x, double y):
        edje_edit_state_gradient_rel1_relative_x_set(self.edje.obj, self.part,
                                                     self.name, self.value, x)
        edje_edit_state_gradient_rel1_relative_y_set(self.edje.obj, self.part,
                                                     self.name, self.value, y)

    property gradient_rel1_relative:
        def __get__(self):
            return self.gradient_rel1_relative_get()
        def __set__(self, value):
            self.gradient_rel1_relative_set(*value)

    def gradient_rel1_offset_get(self):
        cdef int x, y
        x = edje_edit_state_gradient_rel1_offset_x_get(self.edje.obj,
                                                       self.part, self.name,
                                                       self.value)
        y = edje_edit_state_gradient_rel1_offset_y_get(self.edje.obj,
                                                       self.part, self.name,
                                                       self.value)
        return (x, y)

    def gradient_rel1_offset_set(self, int x, int y):
        edje_edit_state_gradient_rel1_offset_x_set(self.edje.obj, self.part,
                                                   self.name, self.value, x)
        edje_edit_state_gradient_rel1_offset_y_set(self.edje.obj, self.part,
                                                   self.name, self.value, y)

    property gradient_rel1_offset:
        def __get__(self):
            return self.gradient_rel1_offset_get()
        def __set__(self, value):
            self.gradient_rel1_offset_set(*value)

    def gradient_rel2_relative_get(self):
        cdef double x, y
        x = edje_edit_state_gradient_rel2_relative_x_get(self.edje.obj,
                                                         self.part, self.name,
                                                         self.value)
        y = edje_edit_state_gradient_rel2_relative_y_get(self.edje.obj,
                                                         self.part, self.name,
                                                         self.value)
        return (x, y)

    def gradient_rel2_relative_set(self, double x, double y):
        edje_edit_state_gradient_rel2_relative_x_set(self.edje.obj, self.part,
                                                     self.name, self.value, x)
        edje_edit_state_gradient_rel2_relative_y_set(self.edje.obj, self.part,
                                                     self.name, self.value, y)

    property gradient_rel2_relative:
        def __get__(self):
            return self.gradient_rel2_relative_get()
        def __set__(self, value):
            self.gradient_rel2_relative_set(*value)

    def gradient_rel2_offset_get(self):
        cdef int x, y
        x = edje_edit_state_gradient_rel2_offset_x_get(self.edje.obj,
                                                       self.part, self.name,
                                                       self.value)
        y = edje_edit_state_gradient_rel2_offset_y_get(self.edje.obj,
                                                       self.part, self.name,
                                                       self.value)
        return (x, y)

    def gradient_rel2_offset_set(self, int x, int y):
        edje_edit_state_gradient_rel2_offset_x_set(self.edje.obj, self.part,
                                                   self.name, self.value, x)
        edje_edit_state_gradient_rel2_offset_y_set(self.edje.obj, self.part,
                                                   self.name, self.value, y)

    property gradient_rel2_offset:
        def __get__(self):
            return self.gradient_rel2_offset_get()
        def __set__(self, value):
            self.gradient_rel2_offset_set(*value)
