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

import edje

from event_manager import Manager


class EditableProgram(Manager, object):
    def __init__(self, editable):
        Manager.__init__(self)

        self._edit_grp = editable
        self._null()
        self._edit_grp.callback_add("group.changed", self._group_changed_cb)

        self._api_export = False

    def _group_changed_cb(self, emissor, data):
        self.name = None

    def _null(self):
        self._name = None
        self._program = None

    # Name
    def _name_set(self, value):
        if not self._edit_grp.edje:
            self._null()
            return

        if not value:
            self._null()
            self.event_emit("program.unselected")

        if self._name == value:
            return

        if value in self._edit_grp.programs:
            self._name = value
            self._program_fetch()
            self.event_emit("program.changed", self.name)
        else:
            self._null()
            self.event_emit("program.unselected")

    def _name_get(self):
        return self._name

    name = property(_name_get, _name_set)

    def rename(self, name):
        if not self.name or not name:
            return False

        if self._name == name:
            return False

        old_name = self._program.name
        self._program.name = name
        if self._program.name == name:
            self._name = self._program.name
            self.event_emit("program.name.changed", (old_name, self._name))
            return True
        return False

    def _program_fetch(self):
        self._program = self._edit_grp.edje.program_get(self._name)

    def _signal_get(self):
        return self._program.signal_get()

    def _signal_set(self, value):
        self._program.signal_set(value)

    signal = property(_signal_get, _signal_set)

    def _source_get(self):
        return self._program.source_get()

    def _source_set(self, value):
        self._program.source_set(value)

    source = property(_source_get, _source_set)

    def _action_get(self):
        return self._program.action_get()

    def _action_set(self, act):
        self._program.action_set(act)

    action = property(_action_get, _action_set)

    def _afters_get(self):
        return self._program.afters_get()

    afters = property(_afters_get)

    def after_add(self, after):
        self._program.after_add(after)

    def afters_clear(self):
        self._program.afters_clear()

    def _targets_get(self):
        return self._program.targets_get()

    targets = property(_targets_get)

    def target_add(self, target):
        return self._program.target_add(target)

    def target_del(self, target):
        return self._program.target_del(target)

    def targets_clear(self):
        self._program.targets_clear()

    def state_get(self):
        """
        Retrieve the program's target part, if it's action is STATE_SET,
        or the program's emission string, if it is of SIGNAL_EMIT type. The
        method's name confusion is an inheritance of the EdjeEdit API itself,
        what should be fixed in near future.

        """
        return self._program.state_get()

    def state2_get(self):
        return self._program.state2_get()

    def state_set_action_set(self, target):
        """ Set the program's action to "STATE_SET".

        "target" argument is the target part. This method will
        override what whas set by signal_emit_action_self().

        """
        self._program.action_set(edje.EDJE_ACTION_TYPE_STATE_SET)
        self._program.state_set(target)

    def signal_emit_action_set(self, signal):
        """ Set the program's action to "SIGNAL_EMIT".

        "signal" argument is the emission string (in case source will
        fallback to the empty string) or a 2-tuple containing emission
        and source strings. This method will override what whas set by
        state_set_action_set().

        """
        t = type(signal)
        if t is list or t is tuple:
            if len(signal) != 2:
                raise TypeError("signal must either be an emission string"
                                " or a (emission, source) tuple/list")
            emission, source = signal
        else:
            emission, source = signal, ""

        self._program.action_set(edje.EDJE_ACTION_TYPE_SIGNAL_EMIT)
        self._program.state_set(emission)
        self._program.state2_set(source)

    def _transition_get(self):
        return self._program.transition_get()

    def _transition_set(self, transition):
        self._program.transition_set(transition)

    transition = property(_transition_get, _transition_set)

    def _transition_time_get(self):
        return self._program.transition_time_get()

    def _transition_time_set(self, ttime):
        self._program.transition_time_set(ttime)

    transition_time = property(_transition_time_get, _transition_time_set)

    def _in_get(self):
        return (self._program.in_from_get(), self._program.in_range_get())

    def _in_set(self, value):
        self._program.in_from_set(value[0])
        self._program.in_range_set(value[1])

    in_time = property(_in_get, _in_set)

    def _api_set(self, value):
        if not self.name:
            return

        old_val = self._program.api
        self._program.api = value
        self.event_emit("program.api.changed", (old_val, value))

    def _api_get(self):
        if not self.name:
            return None

        return self._program.api

    api = property(_api_get, _api_set)

    def _api_export_set(self, value):
        if not self.name:
            return

        self._api_export = value

    def _api_export_get(self):
        if not self.name:
            return None

        return (self._api_get() != (None, None))

    api_export = property(_api_export_get, _api_export_set)
