# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

import os

import edje
from edje.edit import EdjeEdit

from editable_animation import EditableAnimation
from editable_part import EditablePart
from editable_program import EditableProgram
from event_manager import Manager


class Editable(Manager):
    default_display_size = (500, 500)
    pref_size_key = "pref_size"

    def __init__(self, canvas, swapfile, mode_get_cb=None):
        Manager.__init__(self)

        self._canvas = canvas
        self._swapfile = swapfile
        # FIXME: this is a temporary hack to let deeper objects know the
        # state we're at. Find a better place for it
        self._mode_get_cb = mode_get_cb

        self._group = ""
        self.__edje = None

        self.part = EditablePart(self)
        self.animation = EditableAnimation(self)
        self.signal = EditableProgram(self)

        self._size = None
        self._size_init()
        self._modification_init()
        self._parts_init()
        self._programs_init()
        self._animations_init()
        self._signals_init()

    def _mode_get(self):
        return self._mode_get_cb()

    mode = property(fget=_mode_get)

    # Edje
    def _edje_get(self):
        return self.__edje

    edje = property(_edje_get)

    # Filename
    def _filename_get(self):
        return self._swapfile.file

    filename = property(_filename_get)

    def _workfile_get(self):
        return self._swapfile.workfile

    workfile = property(_workfile_get)

    # Group Name
    def _group_get(self):
        return self._group

    def _group_set(self, value):
        if not value:
            value = ""
            self.event_emit("group.changed", value)
            self.__edje and self.__edje.delete()
            self.__edje = None
            self._edje_group = None
        else:
            old_group = self.__edje
            self.__edje = EdjeEdit(
                self._canvas, file=self._swapfile.workfile, group=value)
            self._edje_group = self.__edje.current_group
            self.event_emit("group.changed", value)
            old_group and old_group.delete()

        self._group = value

    group = property(_group_get, _group_set)

    def group_add(self, grp_name):
        if not self.__edje:
            self.__edje = EdjeEdit(
                self._canvas, file=self._swapfile.workfile,
                group=edje.file_collection_list(self._swapfile.workfile)[0])
            self.event_emit("group.changed", self.group)

        return self.__edje.group_add(grp_name)

    def group_exists(self, grp_name):
        if not self.__edje:
            self.__edje = EdjeEdit(
                self._canvas, file=self._swapfile.workfile,
                group=edje.file_collection_list(self._swapfile.workfile)[0])
            self.event_emit("group.changed", self.group)

        return self.__edje.group_exist(grp_name)

    def group_del(self, grp_name):
        dummy_grp = None
        all_grps = edje.file_collection_list(self._swapfile.workfile)
        for g in all_grps:
            if g != grp_name:
                dummy_grp = g
                break

        if not dummy_grp:
            return False

        if not self.__edje or self._group == grp_name:
            self.group = ""

            dummy_edje = EdjeEdit(
                self._canvas, file=self._swapfile.workfile, group=dummy_grp)
            r = dummy_edje.group_del(grp_name)
            dummy_edje.delete()
            return r

        return self.__edje.group_del(grp_name)

    def group_rename(self, name):
        if not self._group:
            return False
        if not name:
            return False
        if self.__edje.group_exist(name):
            return False

        self._edje_group.rename(name)
        self._group = name
        return True

    # GROUP Min/Max

    def _size_init(self):
        self._group_size_update(self, None)
        self.callback_add("group.changed", self._group_size_update)

    def _group_size_update(self, emissor, grp_name):
        self._size = None

        if not grp_name:
            self._max = None
            self._min = None
            return

        self._max = (self._edje_group.w_max, self._edje_group.h_max)
        self.event_emit("group.max.changed", self._max)
        self._min = (self._edje_group.w_min, self._edje_group.h_min)
        self.event_emit("group.min.changed", self._min)

        data = self.__edje.group_data_get(self.pref_size_key)

        if not data:
            w, h = self.default_display_size
        else:
            w, h = data.split("x")
            w = int(w)
            h = int(h)

        self.group_size = (w, h)

    def _verify_max_w(self, w, group_w, group_h, min_w, group):
        if w <= 0:
            return 0
        if w and w < min_w:
            w = min_w
        if w < group_w:
            group.resize(w, group_h)

        return w

    def _verify_max_h(self, h, group_w, group_h, min_h, group):
        if h <= 0:
            return 0
        if h and h < min_h:
            h = min_h
        if h < group_h:
            group.resize(group_w, h)

        return h

    def _max_get(self):
        return self._max

    def _max_set(self, value):
        if self._max == value:
            return

        w, h = value
        group_w, group_h = self.group_size
        min_w, min_h = self._min

        w = self._verify_max_w(w, group_w, group_h, min_w, self.__edje)
        h = self._verify_max_h(h, group_w, group_h, min_h, self.__edje)
        self._edje_group.w_max = w
        self._edje_group.h_max = h
        self._max = (w, h)
        self.event_emit("group.max.changed", self._max)

    group_max = property(_max_get, _max_set)

    def _verify_min_w(self, w, group_w, group_h, max_w, group):
        if w < 0:
            w = 0
        if max_w and w > max_w:
            w = max_w
        if w > group_w:
            group.resize(w, group_h)

        return w

    def _verify_min_h(self, h, group_w, group_h, max_h, group):
        if h < 0:
            h = 0
        if max_h and h > max_h:
            h = max_h
        if h > group_h:
            group.resize(group_w, h)

        return h

    def _min_get(self):
        return self._min

    def _min_set(self, value):
        if self._min == value:
            return

        w, h = value
        group_w, group_h = self.group_size
        max_w, max_h = self._max

        w = self._verify_min_w(w, group_w, group_h, max_w, self.__edje)
        h = self._verify_min_h(h, group_w, group_h, max_h, self.__edje)
        self._edje_group.w_min = w
        self._edje_group.h_min = h
        self._min = (w, h)
        self.event_emit("group.min.changed", self._min)

    group_min = property(_min_get, _min_set)

    def _size_get(self):
        return self._size

    def _size_set(self, value):
        if self._size == value:
            return

        w, h = value
        max_w, max_h = self._max
        min_w, min_h = self._min

        if min_w and w < min_w:
            w = min_w
        elif max_w and w > max_w:
            w = max_w

        if min_h and h < min_h:
            h = min_h
        elif max_h and h > max_h:
            h = max_h

        self._size = (w, h)
        self.__edje.size = (w, h)

        value = self.__edje.group_data_get(self.pref_size_key)
        if not value:
            self.__edje.group_data_add(self.pref_size_key, "0x0")
        self.__edje.group_data_set(self.pref_size_key, "%dx%d" % self._size)

        self.event_emit("group.size.changed", self._size)

    group_size = property(_size_get, _size_set)

    #Images
    def images_get(self):
        return self.__edje.images

    def image_id_get(self, name):
        return self.__edje.image_id_get(name)

    def image_add(self, img):
        if os.path.basename(img) not in self.images_get():
            return self.__edje.image_add(img)

    #Fonts
    def fonts_get(self):
        return self.__edje.fonts

    def font_add(self, fnt):
        if os.path.basename(fnt) not in self.fonts_get():
            self.__edje.font_add(fnt)

    # Modifications
    def _modification_init(self):
        self._modification_clear_cb(self, None)
        self.callback_add("saved", self._modification_clear_cb)
        self.callback_add("group.changed", self._modification_clear_cb)

    def _modification_clear_cb(self, emissor, data):
        self._modified = False

    def close(self):
        self._swapfile.close()

    def _empty_animations_clear(self):
        anim_del_lst = []
        for anim in self.animations:
            prog = self.program_get("@%s@0.00" % anim)
            if not prog.targets:
                anim_del_lst.append(anim)

        for a in anim_del_lst:
            self.animation_del(a)

    def save(self):
        self._empty_animations_clear()

        if self.__edje.save_all():
            self._swapfile.save()
            self.event_emit("saved")
        else:
            self.event_emit("saved.error")

    def save_as(self, path, mode=None):
        self._empty_animations_clear()

        if self.__edje.save_all():
            self._swapfile.save(path, mode)
            self.event_emit("filename.changed", path)
            self.event_emit("saved")
            return True
        else:
            self.event_emit("saved.error")
            return False

    # Parts
    def _parts_get(self):
        return self.__edje.parts

    parts = property(_parts_get)

    def _parts_init(self):
        self.callback_add("group.changed", self._parts_reload_cb)
        self.callback_add("part.added", self._parts_reload_cb)
        self.callback_add("part.removed", self._parts_reload_cb)
        self.part.callback_add("name.changed", self._parts_reload_cb)

    def _parts_reload_cb(self, emissor, data):
        if data:
            self.event_emit("parts.changed", self.parts)
        else:
            self.event_emit("parts.changed", [])

    # TODO: externals API may change in near future
    # besides being totally annoying this use (part_add + external_add, when
    # when type is external), there is no external_del (not even indirectly
    # called
    def external_add(self, module):
        return self.__edje.external_add(module)

    def _part_add(self, name, edje_type, source):
        if edje_type == edje.EDJE_PART_TYPE_EXTERNAL:
            external = edje.external_type_get(source)
            if external:
                self.__edje.external_add(external.module)
        return self.__edje.part_add(name, edje_type, source)

    def part_add(self, name, edje_type, source="", init=None):
        if not self._part_add(name, edje_type, source):
            return False

        part = self._part_init(name)

        if init:
            init(part)

        self._modified = True
        self.event_emit("part.added", name)
        return True

    def part_add_bydata(self, part_data, relatives=None, name=None):
        source = part_data["source"]
        if source is None:
            source = ''

        if not name:
            name = part_data.name

        if not self._part_add(name, part_data.type, source):
            return False

        part = self.__edje.part_get(name)
        part_data.apply_to(part)

        if relatives:
            for p, st in relatives.iteritems():
                part = self.part_get(p)
                for (st_name, st_value), rels in st.iteritems():
                    state = part.state_get(st_name, st_value)
                    if rels[0]:
                        state.rel1_to_x_set(name)
                    if rels[1]:
                        state.rel1_to_y_set(name)
                    if rels[2]:
                        state.rel2_to_x_set(name)
                    if rels[3]:
                        state.rel2_to_y_set(name)

        self.event_emit("part.added", name)

        return True

    def _part_init(self, name):
        part = self.__edje.part_get(name)
        edje_type = part.type
        state = part.state_get(*part.state_selected_get())

        w, h = self.__edje.size

        state.rel1_to = (None, None)
        state.rel1_relative = (0.0, 0.0)
        state.rel1_offset = (w / 4, h / 4)

        state.rel2_to = (None, None)
        state.rel2_relative = (0.0, 0.0)
        state.rel2_offset = (w * 3 / 4, h * 3 / 4)

        if edje_type == edje.EDJE_PART_TYPE_RECTANGLE:
            part.mouse_events = False

        elif edje_type == edje.EDJE_PART_TYPE_TEXT:
            part.mouse_events = False
            state.color = (0, 0, 0, 255)
            state.text = "YOUR TEXT HERE"
            state.font = "Sans"
            state.text_size = 16

        return part

    def part_get(self, part_name):
        return self.__edje.part_get(part_name)

    def part_object_get(self, part_name):
        return self.__edje.part_object_get(part_name)

    def part_del(self, name):
        if self.__edje.part_del(name):
            self._modified = True
            self.event_emit("part.removed", name)
            return True
        return False

    # Programs
    def _programs_get(self):
        if not self.__edje:
            return []
        return self.__edje.programs

    programs = property(_programs_get)

    def _programs_init(self):
        self.callback_add("group.changed", self._programs_reload_cb)
        self.signal.callback_add(
            "program.name.changed", self._programs_reload_cb)

    def _programs_reload_cb(self, emissor, data):
        if data:
            self.event_emit("programs.changed", self.programs)
        else:
            self.event_emit("programs.changed", [])

    def program_add(self, name):
        if not self.__edje.program_add(name):
            return False

        self._modified = True
        self.event_emit("program.added", name)

        return True

    def program_del(self, name):
        if not self.__edje.program_del(name):
            return False

        self._modified = True
        self.event_emit("program.removed", name)

        return True

    def program_get(self, program):
        if not program in self.programs:
            return None

        prg = EditableProgram(self)
        prg.name = program

        return prg

    # Animations
    def _animation_get(self):
        return self._animations

    animations = property(_animation_get)

    def _animations_init(self):
        self._animations = None
        self.callback_add("programs.changed", self._animations_reload_cb)

    def _animations_reload_cb(self, emissor, data):
        self._animations = \
            map(lambda x: x[1:x.rindex("@")],
                filter(lambda x: x.startswith("@") and x.endswith("@end"),
                       self.programs))
        self.event_emit("animations.changed", self.animations)

    def animation_add(self, name, parts=None):
        if name in self._animations:
            return False

        self._modified = True

        # END
        endname = "@%s@end" % name
        self.program_add(endname)
        prog = self.program_get(endname)
        prog.signal_emit_action_set(("animation,end", name))

        # START
        startname = "@%s@0.00" % name
        self.program_add(startname)
        prog = self.program_get(startname)
        prog.state_set_action_set(startname)
        prog.signal = "animation,play"
        prog.source = name
        prog.after_add("@%s@end" % name)

        prevstatename = "default"
        statename = startname
        if not parts:
            parts = self.parts
        for p in parts:
            prog.target_add(p)
            part = self.__edje.part_get(p)
            if not part.state_add(startname):
                return False
            state = part.state_get(statename)
            state.copy_from(prevstatename)

        # STOP
        stopname = "@%s@stop" % name
        self.program_add(stopname)
        prog = self.program_get(stopname)
        prog.action = edje.EDJE_ACTION_TYPE_ACTION_STOP
        prog.signal = "animation,stop"
        prog.source = name
        prog.target_add(startname)
        prog.target_add(endname)

        self._animations.append(name)
        self.event_emit("animation.added", name)
        return True

    def animation_del(self, name):
        stopname = "@%s@stop" % name
        stopprog = self.program_get(stopname)

        if not stopprog:
            return False
        for p in stopprog.targets:
            prog = self.program_get(p)
            for pp in prog.targets:
                part = self.__edje.part_get(pp)
                if part:
                    part.state_selected_set("default")
                    if not part.state_del(p):
                        return False
            self.program_del(p)
        self.program_del(stopname)
        self.event_emit("animation.removed", name)
        self._animations.pop(self._animations.index(name))
        self._modified = True

        return True

    # Signals
    def _signal_get(self):
        return self._signals

    signals = property(_signal_get)

    def _signals_init(self):
        self._signals = None
        self.callback_add("programs.changed", self._signals_reload_cb)
        self.callback_add("signal.added", self._signals_reload_cb)
        self.callback_add("signal.removed", self._signals_reload_cb)

    def _signals_reload_cb(self, emissor, data):
        self._signals = [e for e in self.programs if not e.startswith("@")]
        self.event_emit("signals.changed", self._signals)

    def _signal_add(self, name):
        if not name or name.startswith("@"):
            return False

        if not self.program_add(name):
            return False

        return True

    def signal_add(self, name, action_type):
        if not self._signal_add(name):
            return False

        program = self.__edje.program_get(name)
        program.action_set(action_type)

        self.event_emit("signal.added", name)

        return True

    def signal_add_bydata(self, sig_data):
        name = sig_data.name
        if not self._signal_add(name):
            return False

        program = self.__edje.program_get(name)
        sig_data.apply_to(program)

        self.event_emit("signal.added", name)

        return True

    def signal_del(self, name):
        if not name in self._signals:
            return False

        if not self.program_del(name):
            return False

        self.event_emit("signal.removed", name)

        return True

    def relative_parts_get(self, part_name):
        relatives = dict()
        for p in self.parts:
            saved_states = dict()
            part = self.__edje.part_get(p)
            for st in part.states:
                st_name, value = st.split()
                st_value = float(value)
                state = part.state_get(st_name, st_value)
                if part_name in (state.rel1_to_get()[0],
                                 state.rel1_to_get()[1],
                                 state.rel2_to_get()[0],
                                 state.rel2_to_get()[1]):
                    saved_states[st_name, st_value] = (
                        part_name == state.rel1_to_get()[0],
                        part_name == state.rel1_to_get()[1],
                        part_name == state.rel2_to_get()[0],
                        part_name == state.rel2_to_get()[1])

            if saved_states:
                relatives[p] = saved_states
        return relatives

    def _groups_get(self):
        return edje.file_collection_list(self._swapfile.workfile)

    groups = property(_groups_get)
