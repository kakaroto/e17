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

from collapsable import Collapsable, CollapsableView


class CList(Collapsable):
    def _view_load(self):
        self._selected = None
        self._view = CListView(self, self.parent.view)

    def populate(self, list):
        self._view.populate(list)
        if self._selected:
            self._view.select(self._selected)

    def add(self, item):
        self._view.add(item)
        self.event_emit("item.added", item)

    def remove(self, item):
        self._view.remove(item)
        self.event_emit("item.removed", item)
        if self._selected == item:
            self._view.select(None)

    def select(self, item):
        if self._selected != item:
            self._selected = item
            self._view.select(item)
            self.event_emit("item.selected", self._selected)

    def unselect(self):
        self._view.unselect(self._selected)
        self._selected = None
        self.event_emit("item.unselected", self._selected)

    def _item_select_cb(self, list, it, item, *args, **kwargs):
        if self._selected != item:
            self.select(item)


class CListView(CollapsableView):
    def _content_load(self):
        self.list = elementary.List(self.parent_view)
        self.list.style_set("editje.collapsable")
        self.content_set("content", self.list)
        self.list.show()

        self._first = None
        self.items = {}

    def populate(self, list):
        self._first = None
        self.items.clear()
        self.list.clear()
        if list:
            self._first = self._add(list[0])
        for item in list[1:]:
            self._add(item)
        self.list.go()

    def remove(self, item):
        self.items[item].delete()
        self.list.go()
        del self.items[item]

    def _add(self, item):
        if not item in self.items:
            i = self.list.item_append(item, None, None,
                                      self.controller._item_select_cb, item)
            self.items[item] = i
            return i

    def add(self, item):
        self._add(item)
        self.list.go()

    def select(self, name):
        item = self.items.get(name)
        if name and item:
            item.selected_set(True)
        elif self._first:
            self._first.selected_set(True)

    def unselect(self, name):
        item = self.items.get(name)
        if name and item:
            item.selected_set(False)

