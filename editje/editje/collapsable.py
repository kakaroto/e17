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
import evas
import elementary

import sysconfig
from event_manager import Manager

class Collapsable(Manager, elementary.Layout):

    def __init__(self, parent):
        Manager.__init__(self)
        self._parent = parent
        self._theme_file = sysconfig.theme_file_get("default")

        elementary.Layout.__init__(self, self._parent)
        self.file_set(self._theme_file, "editje/collapsable/list/default")
        self._edje = self.edje_get()

        self._open_load()
        self._options_load()

    # Title
    def _title_set(self, value):
        self._edje.part_text_set("header.title", value)

    def _title_get(self):
        self._edje.part_text_get("header.title")

    title = property(_title_get, _title_set)

    #  Open / Close
    def _open_load(self):
        edje = self.edje_get()
        edje.signal_callback_add("opened", "editje/collapsable/list",
                                 self._opened_cb)
        edje.signal_callback_add("closed", "editje/collapsable/list",
                                 self._closed_cb)

        self._open = False
        self.open = False

    def _open_set(self, value):
        if value:
            self.edje_get().signal_emit("open", "")
        else:
            self.edje_get().signal_emit("close", "")

    def _opened_cb(self, obj, emission, source):
        self._open = True
        self.size_hint_weight_set(1.0, 1.0)
        self.event_emit("opened")

    def _closed_cb(self, obj, emission, source):
        self._open = False
        self.size_hint_weight_set(1.0, 0.0)
        self.event_emit("closed")

    def _open_get(self):
        return self._open

    open = property(_open_get, _open_set)

    #  Options
    def _options_load(self):
        self._options = False
        self.options = False

    def _options_set(self, value):
        self._options = value
        if value:
            self.edje_get().signal_emit("options.enable", "")
        else:
            self.edje_get().signal_emit("options.disable", "")

    def _options_get(self):
        return self._options

    options = property(_options_get, _options_set)


class CollapsablesBox(elementary.Box):

    def __init__(self, parent):
        elementary.Box.__init__(self, parent)
        self.horizontal_set(False)
        self.homogenous_set(False)

        self._spacer = self.evas.Rectangle(color=(0, 0, 0, 0))
        self._spacer.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                          evas.EVAS_HINT_EXPAND)
        self._spacer.size_hint_align_set(evas.EVAS_HINT_FILL,
                                         evas.EVAS_HINT_FILL)
        elementary.Box.pack_end(self, self._spacer)
        self._spacer.show()

        self._items = []

    def pack_end(self, obj):
        obj.on_changed_size_hints_add(self._children_changed_cb)
        obj.size_hint_align_set(evas.EVAS_HINT_FILL,
                                evas.EVAS_HINT_FILL)
        self.pack_before(obj, self._spacer)
        self._items.append(obj)

    # Hack, remove when spaced is not needed
    def _children_changed_cb(self, obj, *args, **kwargs):
        for item in self._items:
            wx, wy = item.size_hint_weight
            if wy > 0.0:
                break
        else:
            self._spacer.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                              evas.EVAS_HINT_EXPAND)
            return
        self._spacer.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)


if __name__ == "__main__":
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

    i = Collapsable(bx)
    i.title = "Item1"
    bx.pack_end(i)
    i.show()

    i = Collapsable(bx)
    i.title = "Item2"
    bx.pack_end(i)
    i.show()

    i = Collapsable(bx)
    i.title = "Item2"
    bx.pack_end(i)
    i.show()
    
    win.show()

    elementary.run()
    elementary.shutdown()
