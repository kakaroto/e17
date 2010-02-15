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

import elementary

from prop import PropertyTable


class EditjeDetails(elementary.Layout):
    def __init__(self, parent, group="editje/collapsable/default"):
        elementary.Layout.__init__(self, parent)

        self.file_set(parent.theme, group)
        self._parent = parent
        self._proptable = PropertyTable(parent)
        self._proptable.show()
        self._proptable._value_changed = self.prop_value_changed
        self.e = parent.e

        self._box = elementary.Box(parent)
        self._box.pack_end(self._proptable)
        self._box.size_hint_weight_set(1.0, 0.0)
        self._box.size_hint_align_set(-1.0, 0.0)
        self._box.show()
        scr = elementary.Scroller(parent)
        scr.style_set("editje.collapsable")
        scr.bounce_set(0, 0)
        scr.content_set(self._box)
        scr.show()
        self.content_set("cl.content", scr)
        self.size_hint_weight_set(1.0, 0.0)
        self.size_hint_align_set(-1.0, -1.0)
        self.edje_get().signal_callback_add("cl,*", "editje/collapsable",
                                            self._header_toggle_cb)

        self._subgroups = dict()

    def open(self):
        self.edje_get().signal_emit("mouse,clicked,1", "cl.header.open")

    def close(self):
        self.edje_get().signal_emit("mouse,clicked,1", "cl.header.close")

    def title_set(self, title):
        self.edje_get().part_text_set("cl.header.title", title)

    def group_add(self, name):
        if name in self._subgroups:
            raise KeyError(name)

        tbl = PropertyTable(self._parent)
        tbl.show()
        tbl._value_changed = self.prop_value_changed
        tbl._value_data = name
        frm = elementary.Layout(self._parent)

        frm.file_set(self._parent.theme, "subgroup")
        frm.edje_get().part_text_set("title.label", name)
        frm.size_hint_weight_set(1.0, 0.0)
        frm.size_hint_align_set(-1.0, 0.0)
        frm.content_set("content", tbl)
        frm.show()
        self._box.pack_end(frm)

        grp = dict()
        grp["table"] = tbl
        grp["frame"] = frm
        self._subgroups[name] = grp

    def group_title_set(self, name, title):
        if not name in self._subgroups:
            raise KeyError(name)

        self._subgroups[name]["frame"].edje_get().part_text_set("title.label",
                                                                title)

    def group_show(self, name):
        if not name in self._subgroups:
            raise KeyError(name)
        grp = self._subgroups[name]["frame"]
        self._box.pack_end(grp)
        grp.show()

    def group_hide(self, name):
        if not name in self._subgroups:
            raise KeyError(name)
        grp = self._subgroups[name]["frame"]
        self._box.unpack(grp)
        grp.hide()

    def main_hide(self):
        self._box.unpack(self._proptable)
        self._proptable.hide()

    def main_show(self):
        self._box.pack_end(self._proptable)
        self._proptable.show()

    def prop_value_changed(self, prop, value, group):
        pass

    def _header_toggle_cb(self, obj, emission, source):
        if emission == "cl,opened":
            self.size_hint_weight_set(1.0, 1.0)
        elif emission == "cl,closed":
            self.size_hint_weight_set(1.0, 0.0)

    def __getitem__(self, key):
        if key == "main":
            return self._proptable
        elif key in self._subgroups:
            return self._subgroups[key]["table"]
        raise KeyError(key)
