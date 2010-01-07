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
from edje.edit import EdjeEdit

from event_manager import Manager

class EditableState(Manager, object):

    def __init__(self, editable_part):
        Manager.__init__(self)

        self.e = editable_part.e
        self._name = None

        self.name = ""
        self.callback_add("state.unselected", self._rel1_reset)
        self.callback_add("state.unselected", self._rel2_reset)
        self.callback_add("state.changed", self._rel1_load)
        self.callback_add("state.changed", self._rel2_load)


        editable_part.callback_add("part.unselected", self._reset_cb)
        editable_part.callback_add("part.changed", self._load_cb)

    def _reset_cb(self, emissor, data):
        self.name = ""

    def _load_cb(self, emissor, data):
        if self.e.part._part:
            name = self.e.part._part.state_selected_get()
            if name == "(null) 0.00":
                name = "default 0.00"
            self._name = ""
            self.name = name
        else:
            self.name = ""

    # Name
    def _name_set(self, value):
        if not self.e._edje or not self.e.part._part:
            self._state = None
            self.event_emit("state.unselected")
        elif self._name != value:
            self._state = self.e.part._part.state_get(value)
            if self._state:
                self._name = value
                self.event_emit("state.changed", self._name)
            else:
                self.event_emit("state.unselected")

    def _name_get(self):
        return self._name

    name = property(_name_get, _name_set)

    def _rel1_reset(self, emissor, data):
        self._rel1x_to = ""
        self._rel1x_rel = 0.0
        self._rel1x_ofs = 0

        self._rel1y_to = ""
        self._rel1y_rel = 0.0
        self._rel1y_ofs = 0

    def _rel2_reset(self, emissor, data):
        self._rel2x_to = ""
        self._rel2x_rel = 1.0
        self._rel2x_ofs = -1

        self._rel2y_to = ""
        self._rel2y_rel = 1.0
        self._rel2y_ofs = -1

    def _rel1_load(self, emissor, data):
        self._rel1x_to, self._rel1y_to = self._state.rel1_to_get()
        if not self._rel1x_to:
            self._rel1x_to = ""
        if not self._rel1y_to:
            self._rel1y_to = ""
        self._rel1x_rel, self._rel1y_rel = self._state.rel1_relative_get()
        self._rel1x_ofs, self._rel1y_ofs = self._state.rel1_offset_get()
        self.event_emit("rel1x.changed",self.rel1x)
        self.event_emit("rel1y.changed",self.rel1y)
#        self._rel1_print()

    def _rel2_load(self, emissor, data):
        self._rel2x_to, self._rel2y_to = self._state.rel2_to_get()
        if not self._rel2x_to:
            self._rel2x_to = ""
        if not self._rel2y_to:
            self._rel2y_to = ""
        self._rel2x_rel, self._rel2y_rel = self._state.rel2_relative_get()
        self._rel2x_ofs, self._rel2y_ofs = self._state.rel2_offset_get()
        self.event_emit("rel2x.changed",self.rel2x)
        self.event_emit("rel2y.changed",self.rel2y)
#        self._rel2_print()

    def _rel1_print(self):
        print "REL1 to", self._rel1x_to, self._rel1y_to
        print "REL1 relative", self._rel1x_rel, self._rel1y_rel
        print "REL1 offset", self._rel1x_ofs, self._rel1y_ofs

    def _rel2_print(self):
        print "REL2 to", self._rel2x_to, self._rel2y_to
        print "REL2 relative", self._rel2x_rel, self._rel2y_rel
        print "REL2 offset", self._rel2x_ofs, self._rel2y_ofs

    def _rel1_to_write(self):
        self._state.rel1_to_set(self._rel1x_to, self._rel1y_to)

    def _rel1_rel_write(self):
        self._state.rel1_relative_set(self._rel1x_rel, self._rel1y_rel)

    def _rel1_ofs_write(self):
        self._state.rel1_offset_set(self._rel1x_ofs, self._rel1y_ofs)

    def _rel2_to_write(self):
        self._state.rel2_to_set(self._rel2x_to, self._rel2y_to)

    def _rel2_rel_write(self):
        self._state.rel2_relative_set(self._rel2x_rel, self._rel2y_rel)

    def _rel2_ofs_write(self):
        self._state.rel2_offset_set(self._rel2x_ofs, self._rel2y_ofs)

    def _rel1x_set(self, value):
#        print "->REL1X", value
        self._rel1x_to = value[0]
        self._rel1_to_write()
        self._rel1x_rel = value[1]
        self._rel1_rel_write()
        self._rel1x_ofs = value[2]
        self._rel1_ofs_write()
#        self._rel1_print()
        self.event_emit("rel1x.changed",self.rel1x)

    def _rel1y_set(self, value):
#        print "->REL1Y", value
        self._rel1y_to = value[0]
        self._rel1_to_write()
        self._rel1y_rel = value[1]
        self._rel1_rel_write()
        self._rel1y_ofs = value[2]
        self._rel1_ofs_write()
#        self._rel1_print()
        self.event_emit("rel1y.changed",self.rel1y)

    def _rel2x_set(self, value):
#        print "->REL2X", value
        self._rel2x_to = value[0]
        self._rel2_to_write()
        self._rel2x_rel = value[1]
        self._rel2_rel_write()
        self._rel2x_ofs = value[2]
        self._rel2_ofs_write()
#        self._rel2_print()
        self.event_emit("rel2x.changed",self.rel2x)

    def _rel2y_set(self, value):
#        print "->REL2Y", value
        self._rel2y_to = value[0]
        self._rel2_to_write()
        self._rel2y_rel = value[1]
        self._rel2_rel_write()
        self._rel2y_ofs = value[2]
        self._rel2_ofs_write()
#        self._rel2_print()
        self.event_emit("rel2y.changed",self.rel2y)

    def _rel1x_get(self):
        return (self._rel1x_to, self._rel1x_rel, self._rel1x_ofs)

    def _rel1y_get(self):
        return (self._rel1y_to, self._rel1y_rel, self._rel1y_ofs)

    def _rel2x_get(self):
        return (self._rel2x_to, self._rel2x_rel, self._rel2x_ofs)

    def _rel2y_get(self):
        return (self._rel2y_to, self._rel2y_rel, self._rel2y_ofs)

    rel1x = property(_rel1x_get, _rel1x_set)
    rel1y = property(_rel1y_get, _rel1y_set)
    rel2x = property(_rel2x_get, _rel2x_set)
    rel2y = property(_rel2y_get, _rel2y_set)
