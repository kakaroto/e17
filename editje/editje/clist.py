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

import evas
import elementary

from collapsable import Collapsable


class CList(Collapsable):
    def __init__(self, parent):
        Collapsable.__init__(self, parent)
        self._content_load()

        self._first = None
        self._items = {}
        self._selected = {}
        self.multi = False

    def _content_load(self):
        self._list = elementary.List(self._parent)
        self._list.style_set("editje.collapsable")
        self._list.bounce_set(False, False)
        self._list.callback_selected_add(self._selected_cb)
        self._list.callback_unselected_add(self._unselected_cb)
        self.content_set("content", self._list)
        self._list.show()

    def _opened_cb(self, obj, emission, source):
        Collapsable._opened_cb(self, obj, emission, source)
        self._list.disabled_set(False)

    def _closed_cb(self, obj, emission, source):
        Collapsable._closed_cb(self, obj, emission, source)
        self._list.disabled_set(True)

    # List
    def clear(self):
        self._first = None
        self._items.clear()
        self._list.clear()
        self._selected.clear()

    def add(self, item, data=None):
        self.add_full(item, data=data)

    def add_full(self, item, icon=None, end=None, data=None):
        if self._items.get(item):
            return
        i = self._list.item_append(item, icon, end, None, data)
        self._items[item] = i
        self.event_emit("item.added", item)
        if self._selected.get(item):
            i.selected = True
        if not self._first:
            self._first = i

    def remove(self, item):
        i = self._items.get(item)
        if i:
            next_item = i.next
            prev_item = i.prev
            if not prev_item:
                self._first = next
            i.delete()
            self._list.go()
            del self._items[item]
            self.event_emit("item.removed", item)
            if next_item:
                next_item.selected_set(True)
            elif prev_item:
                prev_item.selected_set(True)

    def go(self):
        self._selected.clear()
        for i in self._list.selected_items_get():
            self._selected[i.label_get()] = True
        self._list.go()

    # Selection
    def select(self, item):
        i = self._items.get(item)
        if i and not i.selected:
            i.selected_set(True)

    def select_first(self):
        if self._first and not self._first.selected:
            self._first.selected_set(True)

    def _selected_cb(self, li, it):
        data = (it.label_get(),) + it.data_get()[0]
        if not data[0] in self._selected:
            self._selected[data[0]] = True
            self.event_emit("item.selected", data)

    def unselect(self, item):
        i = self._items.get(item)
        if i and i.selected:
            i.selected_set(False)

    def _unselected_cb(self, li, it):
        data = (it.label_get(),) + it.data_get()[0]
        if data[0] in self._selected:
            self._selected.pop(data[0], None)
            self.event_emit("item.unselected", data)

    def selection_clear(self):
        for i in self._list.selected_items_get():
            i.selected_set(False)

    def _selected_get(self):
        ret = []
        for s in self._selected.iterkeys():
            ret.append((s,) + self._items[s].data_get()[0])
        return ret

    selected = property(_selected_get)

    # Multi
    def _multi_set(self, value):
        self._list.multi_select = value

    def _multi_get(self):
        return self._list.multi_select

    multi = property(_multi_get, _multi_set)

# FIXME: this test is unmantained and currently is barfing
if __name__ == "__main__":
    from collapsable import CollapsablesBox

    elementary.init()
    elementary.policy_set(elementary.ELM_POLICY_QUIT,
                          elementary.ELM_POLICY_QUIT_LAST_WINDOW_CLOSED)
    win = elementary.Window("Test", elementary.ELM_WIN_BASIC)
    win.title_set("Test")
    win.autodel_set(True)
    win.resize(300, 600)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = CollapsablesBox(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                            evas.EVAS_HINT_EXPAND)
    bx.size_hint_align_set(evas.EVAS_HINT_FILL,
                           evas.EVAS_HINT_FILL)
    win.resize_object_add(bx)
    bx.show()

    i = CList(bx)
    i.title = "Item1"
    i.open = True
    bx.pack_end(i)
    i.show()

    i.add("One", None)
    i.add("Two", (1, 1))
    i.add("Three", (1, 1))
    i.add("Four", (1, 1))
    i.add("XXX", (1, 1))
    i.add("YYY", (1, 1))
    i.add("KKK", (1, 1))
    i.add("ZZZ", (1, 1))
    i.go()

    i = CList(bx)
    i.title = "Item2"
    i.open = True
    i.multi = True
    bx.pack_end(i)
    i.show()

    i.add("One", None)
    i.add("Two", (1, 1))
    i.add("Three", (1, 1))
    i.add("Four", (1, 1))
    i.add("XXX", (1, 1))
    i.add("YYY", )
    i.add("KKK", (1, 1))
    i.add("ZZZ", (1, 1))
    i.go()

    win.show()

    elementary.run()
    elementary.shutdown()
