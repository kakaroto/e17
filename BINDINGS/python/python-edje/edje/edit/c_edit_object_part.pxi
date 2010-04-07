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

cdef class Part:
    cdef EdjeEdit edje
    cdef object _name

    property name:
        def __get__(self):
            return self._name
        def __set__(self, name):
            self.rename(name)

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self._name = name

    def above_get(self):
        cdef char *part
        part = edje_edit_part_above_get(self.edje.obj, self.name)
        if part == NULL: return None
        r = part
        edje_edit_string_free(part)
        return r

    def below_get(self):
        cdef char *part
        part = edje_edit_part_below_get(self.edje.obj, self.name)
        if part == NULL: return None
        r = part
        edje_edit_string_free(part)
        return r

    def restack_below(self):
        return bool(edje_edit_part_restack_below(self.edje.obj, self.name))

    def restack_above(self):
        return bool(edje_edit_part_restack_above(self.edje.obj, self.name))

    def rename(self, newname):
        cdef unsigned char r
        r = edje_edit_part_name_set(self.edje.obj, self.name, newname)
        if r == 0:
            return False
        self._name = newname
        return True

    property type:
        def __get__(self):
            return edje_edit_part_type_get(self.edje.obj, self.name)

    property states:
        def __get__(self):
            "@rtype: list of str"
            cdef evas.c_evas.Eina_List *lst, *itr
            ret = []
            lst = edje_edit_part_states_list_get(self.edje.obj, self.name)
            itr = lst
            while itr:
                ret.append(<char*>itr.data)
                itr = itr.next
            edje_edit_string_list_free(lst)
            return ret

    def state_get(self, char *sname, double value=0.0):
        if self.state_exist(sname, value):
            return State(self, sname, value)

    def state_add(self, char *sname, double value=0.0):
        edje_edit_state_add(self.edje.obj, self.name, sname, value)

    def state_del(self, char *sname, double value=0.0):
        edje_edit_state_del(self.edje.obj, self.name, sname, value)

    def state_exist(self, char *sname, double value=0.0):
        return bool(edje_edit_state_exist(self.edje.obj, self.name, sname,
                                          value))

    def state_copy(self, char *sfrom, double vfrom, char *sto, double vto):
        return bool(edje_edit_state_copy(self.edje.obj, self.name,
                    sfrom, vfrom, sto, vto))

    def state_selected_get(self):
        cdef char *sel
        cdef double val
        sel = edje_edit_part_selected_state_get(self.edje.obj, self.name, &val)
        if sel == NULL: return None
        r = sel
        v = val
        edje_edit_string_free(sel)
        return (r, v)

    def state_selected_set(self, char *state, double value=0.0):
        edje_edit_part_selected_state_set(self.edje.obj, self.name, state, value)

    property clip_to:
        def __get__(self):
            cdef char *clipper
            clipper = edje_edit_part_clip_to_get(self.edje.obj, self.name)
            if clipper == NULL: return None
            r = clipper
            edje_edit_string_free(clipper)
            return r

        def __set__(self, clipper):
            if not clipper:
                edje_edit_part_clip_to_set(self.edje.obj, self.name, NULL)
            else:
                edje_edit_part_clip_to_set(self.edje.obj, self.name, clipper)

        def __del__(self):
            edje_edit_part_clip_to_set(self.edje.obj, self.name, NULL)

    property source:
        def __get__(self):
            cdef char *source
            source = edje_edit_part_source_get(self.edje.obj, self.name)
            if source == NULL: return None
            r = source
            edje_edit_string_free(source)
            return r

        def __set__(self, source):
            if not source:
                edje_edit_part_source_set(self.edje.obj, self.name, NULL)
            else:
                edje_edit_part_source_set(self.edje.obj, self.name, source)

        def __del__(self):
            edje_edit_part_source_set(self.edje.obj, self.name, NULL)

    property mouse_events:
        def __get__(self):
            return bool(edje_edit_part_mouse_events_get(self.edje.obj,
                                                        self.name))

        def __set__(self, me):
            if me:
                edje_edit_part_mouse_events_set(self.edje.obj, self.name, 1)
            else:
                edje_edit_part_mouse_events_set(self.edje.obj, self.name, 0)

    property repeat_events:
        def __get__(self):
            return bool(edje_edit_part_repeat_events_get(self.edje.obj,
                                                         self.name))

        def __set__(self, re):
            if re:
                edje_edit_part_repeat_events_set(self.edje.obj, self.name, 1)
            else:
                edje_edit_part_repeat_events_set(self.edje.obj, self.name, 0)

    property effect:
        def __get__(self):
            return edje_edit_part_effect_get(self.edje.obj, self.name)

        def __set__(self, effect):
            edje_edit_part_effect_set(self.edje.obj, self.name, effect)

    property ignore_flags:
        def __get__(self):
            return edje_edit_part_ignore_flags_get(self.edje.obj, self.name)

        def __set__(self, flags):
            edje_edit_part_ignore_flags_set(self.edje.obj, self.name, flags)

    property drag:
        def __get__(self):
            cdef int x, y
            x = edje_edit_part_drag_x_get(self.edje.obj, self.name)
            y = edje_edit_part_drag_y_get(self.edje.obj, self.name)
            return (x, y)

        def __set__(self, val):
            x, y = val
            edje_edit_part_drag_x_set(self.edje.obj, self.name, x)
            edje_edit_part_drag_y_set(self.edje.obj, self.name, y)

    property drag_step:
        def __get__(self):
            cdef int x, y
            x = edje_edit_part_drag_step_x_get(self.edje.obj, self.name)
            y = edje_edit_part_drag_step_y_get(self.edje.obj, self.name)
            return (x, y)

        def __set__(self, val):
            x, y = val
            edje_edit_part_drag_step_x_set(self.edje.obj, self.name, x)
            edje_edit_part_drag_step_y_set(self.edje.obj, self.name, y)

    property drag_count:
        def __get__(self):
            cdef int x, y
            x = edje_edit_part_drag_count_x_get(self.edje.obj, self.name)
            y = edje_edit_part_drag_count_y_get(self.edje.obj, self.name)
            return (x, y)

        def __set__(self, val):
            x, y = val
            edje_edit_part_drag_count_x_set(self.edje.obj, self.name, x)
            edje_edit_part_drag_count_y_set(self.edje.obj, self.name, y)

    property drag_confine:
        def __get__(self):
            cdef char *confine
            confine = edje_edit_part_drag_confine_get(self.edje.obj, self.name)
            if confine == NULL: return None
            r = confine
            edje_edit_string_free(confine)
            return r

        def __set__(self, confine):
            edje_edit_part_drag_confine_set(self.edje.obj, self.name, confine)

    property drag_event:
        def __get__(self):
            cdef char *event
            event = edje_edit_part_drag_event_get(self.edje.obj, self.name)
            if event == NULL: return None
            r = event
            edje_edit_string_free(event)
            return r

        def __set__(self, event):
            edje_edit_part_drag_event_set(self.edje.obj, self.name, event)
