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
import edje
from edje.edit import EdjeEdit

from event_manager import Manager
from editable_part import EditablePart
from editable_program import EditableProgram
from editable_animation import EditableAnimation
from os import system, popen, getcwd, remove, path, chdir
from shutil import move

class Editable(Manager, object):

    def __init__(self, canvas):
        Manager.__init__(self)

        self._canvas = canvas

        self._filename = ""
        self._group = ""
        self._edje = None
        self._edc = False
        self._edc_path = ""

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
    def realname_get(self):
        if self._edc:
            return self._edc_path
        else:
            return self._filename

    def _filename_get(self):
        return self._filename

    def _filename_set(self, value):
        if value == self._filename:
            return
        if not value.endswith(".edc"):
            self._filename = value
            self._edje = None
            self._edje_group = None
            self._edc = False
        else:
            self._edc_path = value
            tmp_file = "/tmp/" + path.basename(value)[:-3] + "edj";
            old_dir = getcwd()
            chdir(path.dirname(value))
            system('edje_cc ' + value + ' ' + tmp_file);
            chdir(old_dir)
            self._filename = tmp_file
            self._edc = True
        self.event_emit("filename.changed", value)

    filename = property(_filename_get, _filename_set)

    # Group Name
    def _group_get(self):
        return self._group

    def _group_set(self, value):
        if value != self._group:
            self._group = value
            self._edje = EdjeEdit(self._canvas, file=self._filename,
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
        self.event_callback_add("group.changed", self._min_max_update)

    def _min_max_update(self, emissor, data):
        if data:
            self._max = (self._edje_group.w_max, self._edje_group.h_max)
            self.event_emit("group.max.changed", self._max)
            self._min = (self._edje_group.w_min, self._edje_group.h_min)
            self.event_emit("group.min.changed", self._min)
        else:
            self._max = None
            self._min = None

    def _max_get(self):
        return self._max

    def _max_set(self, value):
        if self._max != value:
            self._max = value
            self._edje_group.w_max = value[0]
            self._edje_group.h_max = value[1]
            self.event_emit("group.max.changed", value)

    group_max = property(_max_get, _max_set)

    def _min_get(self):
        return self._min

    def _min_set(self, value):
        if self._min != value:
            self._min = value
            self._edje_group.w_min = value[0]
            self._edje_group.h_min = value[1]
            self.event_emit("group.min.changed", value)

    group_min = property(_min_get, _min_set)

    # Modifications
    def _modification_init(self):
        self._modification_clear_cb(self, None)
        self.event_callback_add("saved", self._modification_clear_cb)
        self.event_callback_add("group.changed", self._modification_clear_cb)

    def _modification_clear_cb(self, emissor, data):
        self._modificated = False

    def save(self):
#        if self._modificated:
        if self._edje.save():
            if self._edc == True:
                edje_decc_options = " -no-build-sh -current-dir"
                popen('edje_decc ' + self._filename + edje_decc_options)
                move(getcwd() + "/generated_source.edc", self._edc_path)
                remove("/tmp/" + path.basename(self._filename))
            self.event_emit("saved")
        else:
            self.event_emit("saved.error")
#        else:
#            print "No changes after last save"

    # Parts
    def _parts_init(self):
        self.parts = None
        self.event_callback_add("group.changed", self._parts_reload_cb)
        self.event_callback_add("part.added", self._parts_reload_cb)
        self.event_callback_add("part.removed", self._parts_reload_cb)

    def _parts_reload_cb(self, emissor, data):
        self.parts = self._edje.parts
        self.event_emit("parts.changed", self.parts)

    def part_add(self, name, type, source=""):
        if self._edje.part_add(name, type, source):
            self._modificated = True
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
        self.event_callback_add("group.changed", self._programs_reload_cb)

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
        self.event_callback_add("programs.changed", self._animations_reload_cb)

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
        if name in self._animations:
            self._modificated = True
            anim = EditableAnimation(self)
            anim.name = name
            for time in anim.timestops:
                self.program_del("@%s@%.1f" % (name, time))
                for p in self.parts:
                    part = self._edje.part_get(p)
                    part.state_del("@%s@%.1f" % (name, time))
            self.program_del("@%s@start" % name)
            self.program_del("@%s@end" % name)
        self._animations.pop(self._animations.index(name))
        self.event_emit("animation.removed", name)

    # Signals
    def _signal_get(self):
        return self._signals

    signals = property(_signal_get)

    def _signals_init(self):
        self._signals = None
        self.event_callback_add("programs.changed", self._signals_reload_cb)

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
