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

cdef class Program:
    cdef EdjeEdit edje
    cdef object _name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self._name = name

    property name:
        def __get__(self):
            return self._name

        def __set__(self, char *newname):
            self.rename(newname)

    def rename(self, char *newname):
        cdef unsigned char r
        r = edje_edit_program_name_set(self.edje.obj, self.name, newname)
        if r == 0:
            return False
        self._name = newname
        return True

    def edje_get(self):
        return self.edje

    def run(self):
        return bool(edje_edit_program_run(self.edje.obj, self.name))

    def source_get(self):
        cdef char *s
        s = edje_edit_program_source_get(self.edje.obj, self.name)
        if s == NULL:
            return None
        ret = s
        edje_edit_string_free(s)
        return ret

    def source_set(self, char *s):
        return bool(edje_edit_program_source_set(self.edje.obj, self.name, s))

    def signal_get(self):
        cdef char *s
        s = edje_edit_program_signal_get(self.edje.obj, self.name)
        if s == NULL:
            return None
        ret = s
        edje_edit_string_free(s)
        return ret

    def signal_set(self, char *s):
        return bool(edje_edit_program_signal_set(self.edje.obj, self.name, s))

    def in_from_get(self):
        return edje_edit_program_in_from_get(self.edje.obj, self.name)

    def in_from_set(self, double f):
        return bool(edje_edit_program_in_from_set(self.edje.obj, self.name, f))

    def in_range_get(self):
        return edje_edit_program_in_range_get(self.edje.obj, self.name)

    def in_range_set(self, double r):
        return bool(edje_edit_program_in_range_set(self.edje.obj, self.name, r))

    def action_get(self):
        return edje_edit_program_action_get(self.edje.obj, self.name)

    def action_set(self, action):
        return bool(edje_edit_program_action_set(self.edje.obj, self.name,
                                                 action))

    def targets_get(self):
        cdef evas.c_evas.Eina_List *lst, *itr
        ret = []
        lst = edje_edit_program_targets_get(self.edje.obj, self.name)
        itr = lst
        while itr:
            ret.append(<char*>itr.data)
            itr = itr.next
        edje_edit_string_list_free(lst)
        return ret

    def target_add(self, char *t):
        return bool(edje_edit_program_target_add(self.edje.obj, self.name, t))

    def target_del(self, char *t):
        return bool(edje_edit_program_target_del(self.edje.obj, self.name, t))

    def targets_clear(self):
        return bool(edje_edit_program_targets_clear(self.edje.obj, self.name))

    def afters_get(self):
        cdef evas.c_evas.Eina_List *lst, *itr
        ret = []
        lst = edje_edit_program_afters_get(self.edje.obj, self.name)
        itr = lst
        while itr:
            ret.append(<char*>itr.data)
            itr = itr.next
        edje_edit_string_list_free(lst)
        return ret

    def after_add(self, char *a):
        return bool(edje_edit_program_after_add(self.edje.obj, self.name, a))

    def after_del(self, char *a):
        return bool(edje_edit_program_after_del(self.edje.obj, self.name, a))

    def afters_clear(self):
        return bool(edje_edit_program_afters_clear(self.edje.obj, self.name))

    def state_get(self):
        cdef char *s
        s = edje_edit_program_state_get(self.edje.obj, self.name)
        if s == NULL:
            return None
        ret = s
        edje_edit_string_free(s)
        return ret

    def state_set(self, char *s):
        return bool(edje_edit_program_state_set(self.edje.obj, self.name, s))

    def value_get(self):
        return edje_edit_program_value_get(self.edje.obj, self.name)

    def value_set(self, double v):
        return bool(edje_edit_program_value_set(self.edje.obj, self.name, v))

    def state2_get(self):
        cdef char *s
        s = edje_edit_program_state2_get(self.edje.obj, self.name)
        if s == NULL:
            return None
        ret = s
        edje_edit_string_free(s)
        return ret

    def state2_set(self, char *s):
        return bool(edje_edit_program_state2_set(self.edje.obj, self.name, s))

    def value2_get(self):
        return edje_edit_program_value2_get(self.edje.obj, self.name)

    def value2_set(self, double v):
        return bool(edje_edit_program_value2_set(self.edje.obj, self.name, v))

    def transition_get(self):
        return edje_edit_program_transition_get(self.edje.obj, self.name)

    def transition_set(self, t):
        return bool(edje_edit_program_transition_set(self.edje.obj, self.name,
                                                     t))

    def transition_time_get(self):
        return edje_edit_program_transition_time_get(self.edje.obj, self.name)

    def transition_time_set(self, double t):
        return bool(edje_edit_program_transition_time_set(self.edje.obj,
                                                          self.name, t))
