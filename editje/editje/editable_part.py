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

from event_manager import Manager
from editable_state import EditableState


class EditablePart(Manager):
    def __init__(self, editable):
        Manager.__init__(self)

        self.e = editable
        self._name = None

        self._part = None

        self._api_export = False

        self.state = EditableState(self)

        self._states_init()

        self._part_unselect_cb(self, None)
        self.e.callback_add("group.changed", self._part_unselect_cb)
        self.e.callback_add("part.removed", self._part_removed_cb)

    def _part_unselect_cb(self, emissor, data):
        self.name = None

    def _part_removed_cb(self, emissor, data):
        if self._name == data:
            self.name = None

    # Name
    def _name_set(self, value):
        def null():
            self._part = None
            self._name = None

        if not self.e.edje:
            null()
            return

        if not value:
            null()
            self.event_emit("part.unselected")

        if self._name == value:
            return

        self._part = self.e.part_get(value)
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
        if not self.name or not name:
            return False
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
        if not self.name:
            return None

        return self._part.type

    type = property(_type_get)

    def _source_get(self):
        if not self.name:
            return None

        return self._part.source

    source = property(fget=_source_get)

    # States
    def _states_init(self):
        self.states = None
        self.callback_add("part.changed", self._states_reload_cb)
        self.callback_add("part.unselected", self._states_reload_cb)
        self.callback_add("state.added", self._states_reload_cb)
        self.state.callback_add("state.renamed", self._states_reload_cb)
        self.callback_add("state.removed", self._states_reload_cb)

    def _states_reload_cb(self, emissor, data):
        if data and self._part:
            self.states = []
            for s in filter(lambda x: not x.startswith("@"),
                            self._part.states):
                n, v = s.split(None)
                self.states.append((n, float(v)))
        else:
            self.states = []
        self.event_emit("states.changed")

    def state_add(self, name, value=0.0):
        if not self.name:
            return False

        self._part.state_add(name, value)
        self.event_emit("state.added", (name, value))
        return True

    def state_del(self, name, value=0.0):
        if not self.name:
            return False

        self._part.state_del(name, value)
        self.event_emit("state.removed", (name, value))
        return True

    def _mouse_events_set(self, value):
        if not self.name:
            return

        self._part.mouse_events = value
        self.event_emit("part.mouse_events.changed")

    def _mouse_events_get(self):
        if not self.name:
            return None

        return self._part.mouse_events

    mouse_events = property(fget=_mouse_events_get, fset=_mouse_events_set)

    def _api_set(self, value):
        if not self.name:
            return

        old_val = self._part.api
        self._part.api = value
        self.event_emit("part.api.changed", (old_val, value))

    def _api_get(self):
        if not self.name:
            return None

        return self._part.api

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

    def restack_above(self):
        part = self.e.part_get(self.name)
        if not part:
            return False

        r = part.restack_above()
        if r is True:
            self.e.event_emit("parts.changed", self.e.parts)
            self.event_emit("part.changed", self.name)

        return r

    def restack_below(self):
        part = self.e.part_get(self.name)
        if not part:
            return False

        r = part.restack_below()
        if r is True:
            self.e.event_emit("parts.changed", self.e.parts)
            self.event_emit("part.changed", self.name)

        return r
