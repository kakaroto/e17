#
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.
from os import system, popen, getcwd, remove, path, chdir
from shutil import move, copyfile
from tempfile import gettempdir

import edje
from edje.edit import EdjeEdit

from event_manager import Manager
from editable_part import EditablePart
from editable_program import EditableProgram
from editable_animation import EditableAnimation
from swapfile import SwapFile

class Editable(Manager, object):

    def __init__(self, canvas, swapfile):
        Manager.__init__(self)

        self._canvas = canvas

        self._swapfile = swapfile

        self._group = ""
        self._edje = None

        self._min_max_init()
        self._modification_init()
        self._parts_init()
        self._programs_init()
        self._animations_init()
        self._signals_init()

        self.animation = EditableAnimation(self)
        self.part = EditablePart(self)
        self.signal = EditableProgram(self)

    # Edje
    def _edje_get(self):
        return self._edje

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
        if value != self._group:
            self._group = value
            self._edje = EdjeEdit(self._canvas, file=self._swapfile.workfile,
                                  group=self._group)
            self._edje_group = self._edje.current_group
            self.event_emit("group.changed", value)

    group = property(_group_get, _group_set)

    def group_rename(self, name):
        if not self._group:
            return

        self._edje_group.rename(name)
        self._group = name

    # GROUP Min/Max

    def _min_max_init(self):
        self._min_max_update(self, None)
        self.callback_add("group.changed", self._min_max_update)

    def _min_max_update(self, emissor, data):
        if not data:
            self._max = None
            self._min = None
            self._size = None
            return

        self._max = (self._edje_group.w_max, self._edje_group.h_max)
        self.event_emit("group.max.changed", self._max)
        self._min = (self._edje_group.w_min, self._edje_group.h_min)
        self.event_emit("group.min.changed", self._min)
        
        key = self._group + "@pref_size"
        data = self._edje.data_get(key)
        if not data:
            self._edje.data_add(key, "300x300")
            w = 300
            h = 300
        else:
            w, h = data.split("x")
            w = int(w)
            h = int(h)
        self.group_size = (w, h)

    def _max_get(self):
        return self._max

    def _max_set(self, value):
        if self._max != value:
            w, h = value
            if w < 0:
                w = 0
            elif self._min[0] and w < self._min[0]:
                w = self._min[0]
            if h < 0:
                h = 0
            elif self._min[1] and h < self._min[1]:
                h = self._min[1]
            self._max = (w, h)
            self._edje_group.w_max = w
            self._edje_group.h_max = h
            self.event_emit("group.max.changed", self._max)

    group_max = property(_max_get, _max_set)

    def _min_get(self):
        return self._min

    def _min_set(self, value):
        if self._min != value:
            w, h = value
            if w < 0:
                w = 0
            elif self._max[0] and w > self._max[0]:
                w = self._max[0]
            if h < 0:
                h = 0
            elif self._max[1] and h > self._max[1]:
                h = self._max[1]
            self._min = (w, h)
            self._edje_group.w_min = w
            self._edje_group.h_min = h
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
        self._edje.size = (w, h)
        key = self._group + "@pref_size"
        self._edje.data_set(key, "%dx%d" % self._size)
        self.event_emit("group.size.changed", self._size)

    group_size = property(_size_get, _size_set)

    # Modifications
    def _modification_init(self):
        self._modification_clear_cb(self, None)
        self.callback_add("saved", self._modification_clear_cb)
        self.callback_add("group.changed", self._modification_clear_cb)

    def _modification_clear_cb(self, emissor, data):
        self._modificated = False

    def close(self):
        self._swapfile.close()

    def save(self):
#        if self._modificated:
        if self._edje.save():
            self._swapfile.save()
            self.event_emit("saved")
        else:
            self.event_emit("saved.error")
#        else:
#            print "No changes after last save"

    # Parts
    def _parts_init(self):
        self.parts = None
        self.callback_add("group.changed", self._parts_reload_cb)
        self.callback_add("part.added", self._parts_reload_cb)
        self.callback_add("part.removed", self._parts_reload_cb)

    def _parts_reload_cb(self, emissor, data):
        self.parts = self._edje.parts
        self.event_emit("parts.changed", self.parts)

    def part_add(self, name, type, source="", signal=True):
        if self._edje.part_add(name, type, source):
            self._modificated = True
            if signal:
                self.event_emit("part.added", name)
            return True

    def part_del(self, name):
        if self._edje.part_del(name):
            self._modificated = True
            self.event_emit("part.removed", name)
            return True

    # Programs
    def _programs_init(self):
        self.programs = None
        self.callback_add("group.changed", self._programs_reload_cb)

    def _programs_reload_cb(self, emissor, data):
        self.programs = self._edje.programs
        self.event_emit("programs.changed", self.programs)

    def program_add(self, name):
        if self._edje.program_add(name):
            self._modificated = True
            self.programs.append(name)
            self.event_emit("program.added", name)
            return True
        return False

    def program_del(self, name):
        if self._edje.program_del(name):
            self._modificated = True
            self.programs.remove(name)
            self.event_emit("program.removed", name)
            return True
        return False

    def program_get(self, program):
        if program in self.programs:
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
        self._animations = map(lambda x: x[1:x.rindex("@")],
                               filter(lambda x: x.startswith("@") and
                                                x.endswith("@end"),
                               self.programs))
        self.event_emit("animations.changed", self.animations)

    def animation_add(self, name):
        if name in self._animations:
            return False

        self._modificated = True

        # END
        endname = "@%s@end" % name
        self.program_add(endname)
        prog = self.program_get(endname)
        prog.signal_emit("animation,end", name)

        # START
        startname = "@%s@0.00" % name
        self.program_add(startname)
        prog = self.program_get(startname)
        prog.state_set(startname)
        prog.signal = "animation,play"
        prog.source = name
        prog.after_add("@%s@end" % name)

        prevstatename =  "default 0.00"
        statename = startname + " 0.00"
        for p in self.parts:
            prog.target_add(p)
            part = self._edje.part_get(p)
            part.state_add(startname)
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
            return
        for p in stopprog.targets:
            prog = self.program_get(p)
            for pp in prog.targets:
                part = self._edje.part_get(pp)
                if part:
                    part.state_del(prog.state_get() + " 0.00")
            self.program_del(p)
        self.program_del(stopname)
        self.event_emit("animation.removed", name)
        self._animations.pop(self._animations.index(name))
        self._modificated = True

    # Signals
    def _signal_get(self):
        return self._signals

    signals = property(_signal_get)

    def _signals_init(self):
        self._signals = None
        self.callback_add("programs.changed", self._signals_reload_cb)

    def _signals_reload_cb(self, emissor, data):
        self._signals = [e for e in self.programs if not e.startswith("@")]
        self.event_emit("signals.changed", self._signals)

    def signal_add(self, name, type):
        if name and not name.startswith("@"):
            if self.program_add(name):
                program = self.program_get(name)
                program._program.action_set(type)
                self._signals.append(name)
                self.event_emit("signal.added", name)
                self.event_emit("signals.changed", self._signals)
                return True
        return False

    def signal_del(self, name):
        if name in self._signals:
            if self.program_del(name):
                self._signals.remove(name)
                self.event_emit("signal.removed", name)
                self.event_emit("signals.changed", self._signals)
                return True
        return False
