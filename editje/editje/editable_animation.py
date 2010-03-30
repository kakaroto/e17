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
import re

import edje

from event_manager import Manager
from editable_program import EditableProgram

re_anim_program_stop = re.compile("@(.*)@stop$")
re_anim_program_end = re.compile("@(.*)@end$")
re_anim_program_time = re.compile("@(.*)@(\d+\.\d+)$")
re_anim_program = re.compile("@(.*)@((?:\d+\.\d+)|end|stop)$")

class EditableAnimation(Manager, object):
    def __init__(self, editable):
        Manager.__init__(self)

        self.e = editable
        self._name = ""
        self.timestops = None

        self._parts_init()
        self._states_init()

        self.program = EditableProgram(self.e)

        self._name = None
        self.e.callback_add("group.changed", self._group_changed_cb)
        self.e.callback_add("animation.removed", self._animation_removed_cb)

    def _group_changed_cb(self, emissor, data):
        self.name = None

    def _animation_removed_cb(self, emissor, data):
        if self._name == data:
            self.name = None
        for p in self.e.parts:
            part = self.e.part_get(p)
            part.state_selected_set("default 0.00")

    # Name
    def _name_set(self, value):
        if not self.e._edje:
            return

        if not value:
            self._name = ""
            self.event_emit("animation.unselected")
        elif self._name != value:
            if value in self.e.animations:
                self._name = value
                self.event_emit("animation.changed", self._name)
            else:
                self._name = ""
                for p in self.e.parts:
                    part = self.e._edje.part_get(p)
                    part.state_selected_set("default 0.00")
                self.event_emit("animation.unselected")

    def _name_get(self):
        return self._name

    name = property(_name_get, _name_set)

    def name_set(self, name):
        if not name:
            return

        # stop program
        stopname = "@%s@stop" % self._name
        stopprog = self.e._edje.program_get(stopname)
        if not stopprog or not stopprog.rename("@%s@stop" % name):
            return
        stopprog.source_set(name)

        # others programs
        for p in stopprog.targets_get():
            prog = self.e._edje.program_get(p)
            time = re_program.match(p).group(2)
            p2 = "@%s@%s" % (name, time)
            if time == "end":
                prog.state2_set(name)
            else:
                for pp in prog.targets_get():
                    part = self.e._edje.part_get(pp)
                    if not part:
                        #prog.target_del(pp) TODO: binding
                        continue
                    state = part.state_get(p + " 0.00")
                    if not state:
                        continue
                    state.name_set(p2 + " 0.00")

                if time == "0.00":
                    prog.source_set(name)

                prog.state_set(p2)

            prog.rename(p2)
        # Hack to force reload animation list
        # self._name = name
        state = self.state
        self.e._programs_reload_cb(self, True)
        self.name = name
        self.state = state
        return

    # Play
    def play(self):
        if self.name:
            self.e._edje.signal_callback_add("animation,end", self._name, self._play_end)
            self.e._edje.program_get(self.program._program.afters_get()[0]).run()

    def _play_end(self, obj, emission, source):
        self.e._edje.signal_callback_del("animation,end", self._name, self._play_end)
        self.state = self.state
        self.event_emit("animation.play.end")

    def stop(self):
        self.e._edje.signal_emit("animation,stop", self._name)

    # Parts
    def _parts_init(self):
        self.parts = {}
        self.e.part.callback_add("name.changed", self._part_rename_cb)
        self.callback_add("animation.changed", self._parts_reload_cb)
        self.callback_add("animation.unselected", self._parts_reload_cb)

    def _parts_reload_cb(self, emissor, data):
        self.parts = {}
        if not data:
            return

        prog = self.e.program_get("@%s@0.00" % self._name)
        for t in prog.targets:
            self.parts[t] = True

    def _part_rename_cb(self, emissor, data):
        old_name, new_name = data
        p = self.parts.get(old_name)
        if not p:
            return
        self.parts[new_name] = True
        del self.parts[old_name]

    def part_add(self, part):
        if part in self.parts:
            return

        progname = "@%s@0.00" % self._name
        prog = self.e.program_get(progname)
        prog.target_add(part)
        p = self.e._edje.part_get(part)
        p.state_copy("default 0.00", progname)
        self.parts[part] = True

        # Re-set current state to make sure everything is consistent
        self._state_set(self._current)

        self.event_emit("part.added", part)

    def part_remove(self, part):
        if part not in self.parts:
            return

        p = self.e._edje.part_get(part)
        p.state_selected_set("default 0.00")
        if p.name == self.e.part.name:
            self.e.part.state.name = ""
        for t in self.timestops:
            progname = "@%s@%.2f" % (self._name, t)
            st = progname + " 0.00"
            prog = self.e.program_get(progname)
            prog.target_del(part)
            p.state_del(st)

        del self.parts[part]
        self.event_emit("part.removed", part)

    def part_belongs(self, part):
        return part in self.parts

    # States
    def _states_init(self):
        self.timestops = []
        self.callback_add("animation.changed", self._states_reload_cb)
        self.callback_add("animation.unselected", self._states_reload_cb)
        self.callback_add("animation.changed", self._state_reload_cb)
        self.callback_add("animation.unselected", self._state_reload_cb)

    def _states_reload_cb(self, emissor, data):
        self.timestops = []

        if not data:
            return

        p = self.e.program_get("@%s@0.00" % self._name)
        t = p.name[-4:]
        while t != "@end":
            self.timestops.append(float(t))
            p = self.e.program_get(p.afters[0])
            t = p.name[-4:]

        self.event_emit("states.changed", self.timestops)

    def state_add(self, time):
        if not self._name:
            return
        if time < 0.0:
            return

        # Search
        idx = 0
        for t in self.timestops:
            if time == t:
                return idx
            if t > time:
                break
            idx += 1

        # Defines
        prev = self.timestops[idx - 1]
        prevname = "@%s@%.2f" % (self._name, prev)
        name = "@%s@%.2f" % (self._name, time)

        # States
        prevstatename = prevname + " 0.00"
        statename = name + " 0.00"

        # Create
        self.e.program_add(name)
        prog = self.e.program_get(name)
        prog.state_set(name)
        prog.transition = edje.EDJE_TWEEN_MODE_LINEAR
        prog.transition_time = time - prev
        for p in self.parts.iterkeys():
            prog.target_add(p)
            part = self.e._edje.part_get(p)
            part.state_add(name)
            state = part.state_get(statename)
            state.copy_from(prevstatename)

        # Link Prev
        prevprog = self.e.program_get(prevname)
        nextname = prevprog.afters[0]
        prog.after_add(nextname)
        prevprog.afters_clear()
        prevprog.after_add(name)

        # Link Next
        next = nextname[-4:]
        if not next == "@end":
            next = float(next)
            nextprog = self.e.program_get(nextname)
            nextprog.transition_time = next - time

        # Stop
        stopname = "@%s@stop" % self._name
        self.e.program_add(stopname)
        prog = self.e.program_get(stopname)
        prog.action = edje.EDJE_ACTION_TYPE_ACTION_STOP
        prog.signal = "animation,stop"
        prog.target_add(name)

        self.timestops.insert(idx, time)
        self.event_emit("state.added", time)

        return idx

    def state_del(self, time):
        if time == 0.0 or time == "end":
            return

        # Search
        idx = self.timestops.index(time)

        progname = "@%s@%.2f" % (self._name, time)
        prog = self.e.program_get(progname)

        # Unlink
        prev = self.timestops[idx - 1]
        prevname = "@%s@%.2f" % (self._name, prev)
        prevprog = self.e.program_get(prevname)
        nextname = prog.afters[0]
        prevprog.afters_clear()
        prevprog.after_add(nextname)

        # Fix Next
        next = nextname[-4:]
        if not next == "@end":
            next = float(next)
            nextprog = self.e.program_get(nextname)
            nextprog.transition_time = next - prev

        # Delete states from parts
        statename = progname + " 0.00"
        for p in self.parts.iterkeys():
            part = self.e._edje.part_get(p)
            part.state_del(statename)

        self.timestops.pop(idx)
        self.event_emit("state.removed", time)

    def _part_state_create(self, part):
        statename = self.program.name
        orig_state = "default 0.00"
        time_idx = 0
        while time_idx <= self._current_idx:
            time = self.timestops[time_idx]
            name = "@%s@%.2f 0.00" % (self._name, time)
            if part.state_exist(name):
                orig_state = name
            else:
                part.state_copy(orig_state, statename)
            time_idx += 1

        self.program.target_add(part.name)

    def _state_set(self, time):
        if not self._name:
            return
        self._current_idx = self.timestops.index(time)
        self._current = time
        self.program.name = "@%s@%.2f" % (self._name, time)
        statename = self.program.name + " 0.00"
        for p in self.parts.iterkeys():
            part = self.e._edje.part_get(p)
            if part.state_exist(statename):
                part.state_selected_set(statename)
            else:
                self._part_state_create(part)
                part.state_selected_set(statename)
                #self.e.part.state.name = statename # Why is this here?
        if self.e.part.name in self.parts:
            self.e.part.state.name = statename
        self.event_emit("state.changed", self.e.part.state.name)

    def _state_get(self):
        return self._current

    state = property(_state_get, _state_set)

    def state_next(self):
        if not self._name:
            return None
        if self._current_idx == len(self.timestops) - 1:
            return None
        return self.timestops[self._current_idx + 1]

    def state_next_goto(self):
        state = self.state_next()
        if state is not None:
            self.state = state

    def state_prev(self):
        if not self._name:
            return None
        if self._current_idx == 0:
            return None
        return self.timestops[self._current_idx - 1]

    def state_prev_goto(self):
        state = self.state_prev()
        if state is not None:
            self.state = state

    def _state_reload_cb(self, emissor, data):
        if data and self.timestops:
            self.state = 0.0
        else:
            self._current = None
            self._idx = 0
            self._prog = None

    # Info
    def _length_get(self):
        if self.timestops:
            return self.timestops[-1]
        return 0.0

    length = property(_length_get)
