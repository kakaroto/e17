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
from editable_program import EditableProgram

class AnimationTimestop(object):

    def __init__(self, anim, timestamp, progname):
        self._anim = anim
        self._progname = progname
        self._ts = float(timestamp)
        self._targets = []

        prog = self._anim.e.program_get(progname)
        self._statename = prog.state_get()
        self._length = prog.transition_time

        #for p in prog.targets:
        for p in self._anim.e.parts:
            if p not in prog.targets:
                prog.target_add(p)
            self._targets.append(p)
            self._add_state_if_needed(p)

    def _timestamp_get(self):
        return self._ts

    timestamp = property(_timestamp_get)

    def _program_name_get(self):
        return self._progname

    def _program_name_set(self, prog):
        self._progname = prog

    program_name = property(_program_name_get, _program_name_set)

    def _state_name_get(self):
        return self._statename

    state_name = property(_state_name_get)

    def _length_get(self):
        return self._length

    def _length_set(self, length):
        if self._length == length or length <= 0:
            return
        return

        self._length = length
        self._statename = "@%s@%.1f" % (self._anim.name, self._ts)
        prog = self._anim.e.program_get(self._progname)
        prog.state_set(self._statename)
        prog.transition_time = self._length

        for p in prog.targets:
            self._add_state_if_needed(p)

    length = property(_length_get, _length_set)

    def after_set(self, ts=None):
        prog = self._anim.e.program_get(self._progname)
        prog.afters_clear()
        if ts:
            prog.after_add(ts.program_name)
        else:
            prog.after_add("@%s@end" % self._anim.name)

    def target_add(self, target):
        if target in self._targets:
            return

        prog = self._anim.e.program_get(self._progname)
        prog.target_add(target)
        self._targets.append(target)
        self._add_state_if_needed(target)

    def targets_clear(self):
        self._targets = []
        self._anim.e.program_get(self._progname).targets_clear()

    def _add_state_if_needed(self, p):
        part = self._anim.e._edje.part_get(p)
        if not part.state_exist(self._statename + " 0.0"):
            st = self._statename
            part.state_copy(part.state_selected_get(), st)

    def __call__(self):
        for p in self._targets:
            self._add_state_if_needed(p)
            part = self._anim.e._edje.part_get(p)
            part.state_selected_set(self._statename + " 0.0")


class EditableAnimation(Manager, object):

    def __init__(self, editable):
        Manager.__init__(self)

        self.e = editable
        self._name = None
        self.timestops = None

        self._states_init()

        self.program = EditableProgram(self.e)

        self._animation_unselect_cb(self, None)
        self.e.event_callback_add("group.changed", self._animation_unselect_cb)
        self.e.event_callback_add("animation.removed", self._animation_removed_cb)


    def _animation_unselect_cb(self, emissor, data):
        self.name = None

    def _animation_removed_cb(self, emissor, data):
        if self._name == data:
            self.name = None

    # Name
    def _name_set(self, value):
        if not self.e._edje:
            value = None

        if self._name != value:
            if value:
                if value in self.e.animations:
                    self._name = value
                    self.event_emit("animation.changed", self._name)
            else:
                self._name = None
                self.event_emit("animation.unselected")

    def _name_get(self):
        return self._name

    name = property(_name_get, _name_set)

    # Play
    def play(self):
        if self.name:
            self.e._edje.program_get(self.program.name).run()
            self.e._edje.signal_callback_add("animation,end", self._name, self._play_end)

    def _play_end(self, obj, emission, source):
        self.e._edje.signal_callback_del("animation,end", self._name, self._play_end)
        self._state_idx_set(0)
        self.event_emit("animation.play.end")

    def stop(self):
        self.e._edje.signal_emit("animation,stop", self._name)

    # States
    def _states_init(self):
        self.event_callback_add("animation.changed", self._states_reload_cb)
        self.event_callback_add("animation.unselected", self._states_reload_cb)
        #self.event_callback_add("state.added", self._states_reload_cb)
        #self.event_callback_add("state.removed", self._states_reload_cb)
        self.event_callback_add("animation.changed", self._state_reload_cb)
        self.event_callback_add("animation.unselected", self._state_reload_cb)

    def _states_reload_cb(self, emissor, data):
        self.timestops = []

        if not data:
            return

        p = self.e.program_get("@%s@start" % self._name)
        t = p.state_get().rsplit("@", 1)[1]
        while t != "end":
            self.timestops.append(AnimationTimestop(self, t, p.name))
            p = self.e.program_get(p.afters[0])
            if p.name == "@%s@end" % self._name:
                break
            t = p.state_get().rsplit("@", 1)[1]

        self.event_emit("states.changed", self.timestops)

    def state_add(self, time):
        if self.timestops is None:
            return # TODO: raise exception, catch it and show popup
        idx = 0
        for t in self.timestamps:
            if time == t:
                return
            if t > time:
                break
            idx += 1
        self._state_idx_set(idx-1)
        self._state_insert(time, idx)
        self.event_emit("state.added", time)

    def state_del(self, time):
        if time == 0.0 or time == "end":
            return
        idx = self._state_index_find(time, 0, len(self.timestops) - 1)
        if idx is None:
            return
        self._state_remove(idx)
        self.event_emit("state.removed", time)

    def _state_set(self, value):
        if self.timestops is None:
            return # TODO: raise exception, catch it and show popup
        idx = self._state_index_find(value, 0, len(self.timestops) - 1)
        if idx is None:
            return
        self._state_idx_set(idx)

    def _state_idx_set(self, idx):
        self._current = self.timestops[idx]
        self._idx = idx
        self.program.name = self._current.program_name
        self._current()
        if not self.e.part.name:
            self.e.part.name = self.e.parts[0]
        self.e.part.state.name = self._current.state_name + " 0.0"
        self.event_emit("state.changed", self._current.timestamp)

    def _state_get(self):
        return self._current

    state = property(_state_get, _state_set)

    def state_next(self):
        if self.name:
            if self._idx == len(self.timestops) - 1:
                return None
            return self.timestops[self._idx + 1]

    def state_next_goto(self):
        if self.name:
            if self._idx == len(self.timestops) - 1:
                return
            self._state_idx_set(self._idx + 1)

    def state_prev(self):
        if self.name:
            if self._idx == 0:
                return None
            return self.timestops[self._idx - 1]

    def state_prev_goto(self):
        if self.name:
            if self._idx == 0:
                return
            self._state_idx_set(self._idx - 1)

    def _state_reload_cb(self, emissor, data):
        if data and self.timestops:
            self._state_idx_set(0)
        else:
            self._current = None
            self._idx = 0
            self._prog = None

    def _state_index_find(self, time, start, end):
        if start > end:
            return None

        mid = (start + end) / 2
        if self.timestops[mid].timestamp > time:
            return self._state_index_find(time, start, mid - 1)
        elif self.timestops[mid].timestamp < time:
            return self._state_index_find(time, mid + 1, end)

        return mid

    def _state_insert_index_find(self, time, start, end):
        if start == end:
            if self.timestops[end].timestamp > time:
                return end
            else:
                return end + 1

        mid = (start + end) / 2
        if time > self.timestops[mid].timestamp:
            self._state_insert_index_find(time, mid + 1, end)
        elif time < self.timestops[mid].timestamp:
            self._state_insert_index_find(time, start, mid)

        return None

    def _state_insert(self, time, idx):
        if idx > len(self.timestops) - 1:
            self._state_append(time)
            return

        prev = self.timestops[idx - 1]
        next = self.timestops[idx]

        newstatename = "@%s@%.1f" % (self._name, time)
        newprogname = newstatename
        newlength = next.timestamp - time

        prevprog = self.e.program_get(next.program_name)
        prevprog.state_set(newstatename)
        prevprog.transition_time = time - prev.timestamp

        self.e.program_add(newprogname)
        prognew = self.e.program_get(newprogname)

        stopprog = self.e.program_get("@%s@stop" % self._name)
        stopprog.target_add(newprogname)

        prognew.afters_clear()
        #prognew.after_add(next.state_name)
        prognew.after_add(prevprog.afters[0])
        prognew.state_set(next.state_name)
        prognew.targets_clear()
        for p in self.e._edje.parts:
            prognew.target_add(p)
        prognew.transition = edje.EDJE_TWEEN_MODE_LINEAR
        prognew.transition_time = newlength

        newts = AnimationTimestop(self, time, next.program_name)
        self.timestops.insert(idx, newts)
        next.program_name = newprogname

        #newts.length = next.timestamp - time

        newts.after_set(next)

        # XXX: Add targets only as parts are modified
        for p in self.e._edje.parts:
            newts.target_add(p)

    def _state_append(self, time):
        last = self.timestops[-1]

        statename = "@%s@%.1f" % (self._name, time)
        progname = "@%s@%.1f" % (self._name, last.timestamp)
        proglength = time - last.timestamp

        self.e.program_add(progname)
        lastprog = self.e.program_get("@%s@%.1f" % (self._name, last.timestamp))

        stopprog = self.e.program_get("@%s@stop" % self._name)
        stopprog.target_add(progname)

        lastprog.afters_clear()
        lastprog.after_add(progname)

        newprog = self.e.program_get(progname)
        newprog.afters_clear()
        newprog.targets_clear()
        newprog.state_set(statename)
        newprog.transition = edje.EDJE_TWEEN_MODE_LINEAR
        newprog.transition_time = proglength

        newts = AnimationTimestop(self, time, progname)
        self.timestops.append(newts)

        last.after_set(newts)
        newts.after_set()

        # Add targets only as parts are modified
        for p in self.e._edje.parts:
            newts.target_add(p)

    def _state_remove(self, idx):
        goner = self.timestops.pop(idx)
        prev = self.timestops[idx - 1]

        prev.length += goner.length
        prev.after_set(self.timestops[idx])

        self.e.program_del(goner.program_name)
        # Clean up staled states

    # Info
    def _length_get(self):
        return self.timestops[-1].timestamp

    length = property(_length_get)

    def _timestamps_get(self):
        if self.timestops is None:
            return ()
        return (t.timestamp for t in self.timestops)

    timestamps = property(_timestamps_get)
