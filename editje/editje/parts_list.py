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
import elementary

from clist import CList
from newpart import NewPart


class PartsList(CList):
    def __init__(self, parent):
        CList.__init__(self, parent)
        self.e = parent.e

        self._options_load()

        self.e.callback_add("parts.changed", self._parts_update)
        self.e.callback_add("part.added", self._part_added)
        self.e.callback_add("part.removed", self._part_removed)

        self.e.part.callback_add("part.changed", self._part_changed)
        self.e.part.callback_add("part.unselected", self._part_changed)

    def _parts_update(self, emissor, data):
        self.clear()
        for i in data[::-1]:
            self.add(i)
        self.go()

    def _part_added(self, emissor, data):
        self.add(data)
        self.go()
        self.open = True
        self.select(data)

    def _part_removed(self, emissor, data):
        self.remove(data)

    def _part_changed(self, emissor, data):
        self.selection_clear()
        self.select(data)

    # Selection
    def _selected_cb(self, li, it):
        CList._selected_cb(self, li, it)
        name = it.label_get()
        self.e.part.name = name
        self._options_edje.signal_emit("up,enable", "")
        self._options_edje.signal_emit("down,enable", "")
        self._options_edje.signal_emit("remove,enable", "")

    def _unselected_cb(self, li, it):
        CList._unselected_cb(self, li, it)
        if not self._selected:
            self._options_edje.signal_emit("up,disable", "")
            self._options_edje.signal_emit("down,disable", "")
            self._options_edje.signal_emit("remove,disable", "")

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(self.edje_get().evas,
                                file=self._theme_file,
                                group="editje/collapsable/list/options/parts")
        self._options_edje.signal_callback_add("new",
                                "editje/collapsable/list/options",
                                self._new_cb)
        self._options_edje.signal_callback_add("up",
                                "editje/collapsable/list/options",
                                self._up_cb)
        self._options_edje.signal_callback_add("down",
                                "editje/collapsable/list/options",
                                self._down_cb)
        self._options_edje.signal_callback_add("remove",
                                "editje/collapsable/list/options",
                                self._remove_cb)
        self._options_edje.signal_emit("up,disable", "")
        self._options_edje.signal_emit("down,disable", "")
        self._options_edje.signal_emit("remove,disable", "")
        self.content_set("options", self._options_edje)
        self._options = False

    def _new_cb(self, obj, emission, source):
        NewPart(self._parent).open()

    def _up_cb(self, obj, emission, source):
        if self.e.part._part:
            self.e.part._part.restack_above()
            self.e._parts_reload_cb(self, None)

    def _down_cb(self, obj, emission, source):
        if self.e.part._part:
            self.e.part._part.restack_below()
            self.e._parts_reload_cb(self, None)

    def _remove_cb(self, obj, emission, source):
        for i in self.selected:
            self.e.part_del(i[0])
