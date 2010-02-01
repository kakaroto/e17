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
from event_manager import Manager
from editable_state import EditableState

class EditablePart(Manager, object):

    def __init__(self, editable):
        Manager.__init__(self)

        self.e = editable
        self._name = None

        self._part = None

        self._states_init()

        self.state = EditableState(self)

        self._part_unselect_cb(self, None)
        self.e.callback_add("group.changed", self._part_unselect_cb)
        self.e.callback_add("part.removed", self._part_removed_cb)

    def _part_unselect_cb(self, emissor, data):
        self.name = ""

    def _part_removed_cb(self, emissor, data):
        if self._name == data:
            self.name = None

    # Name
    def _name_set(self, value):
        if not self.e._edje or not value:
            self._part = None
            self._name = ""
            self.event_emit("part.unselected")
        elif self._name != value:
            self._part = self.e._edje.part_get(value)
            if self._part:
                self._name = self._part.name
                self.event_emit("part.changed", self._name)
            else:
                self._name = None
                self.event_emit("part.unselected")

    def _name_get(self):
        return self._name

    name = property(_name_get, _name_set)

    def rename(self, name):
        if self._name != name:
            old_name = self._part.name
            self._part.name = name
            if self._part.name == name:
                self._name = self._part.name
                self.event_emit("name.changed", (old_name, self._name))
                return True
            return False

    # Type
    def _type_get(self):
        return self._part.type

    type = property(_type_get)

    # States
    def _states_init(self):
        self.states = None
        self.callback_add("part.changed", self._states_reload_cb)
        self.callback_add("part.unselected", self._states_reload_cb)
        self.callback_add("state.added", self._states_reload_cb)
        self.callback_add("state.removed", self._states_reload_cb)

    def _states_reload_cb(self, emissor, data):
        if data and self._part:
            self.states = filter(lambda x: not x.startswith("@"),
                                 self._part.states)
        else:
            self.states = []
        self.event_emit("states.changed")

    def state_add(self, name, type):
        self._part.state_add(name)
        self.event_emit("state.added", name)

    def state_del(self, name):
        self._part.state_del(name)
        self.event_emit("state.removed", name)

