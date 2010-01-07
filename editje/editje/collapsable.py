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
import os

from elementary import Layout, Box, Button, Icon

import sysconfig
from controller import Controller, View
from event_manager import Manager


class Collapsable(Manager, Controller):

    def __init__(self, parent):
        Manager.__init__(self)
        Controller.__init__(self, parent)

    # VIEW
    def _view_load(self):
        self._view = CollapsableView(self, self.parent.view)

    # OPEN
    def _open_set(self, value):
        self._view.open = value
        if self._view.open:
            self.event_emit("opened")
        else:
            self.event_emit("closed")

    def _open_get(self):
        return self._view.open

    open = property(_open_get, _open_set)

    # OPTIONS
    def _options_set(self, value):
        self._view.options = value

    def _options_get(self):
        return self._view.options

    options = property(_options_get, _options_set)

class CollapsableView(View, Layout):

    def __init__(self, controller, parent_view):
        View.__init__(self, controller, parent_view)
        self._theme_file = sysconfig.theme_file_get("default")
        self._layout_load()
        self._open_load()
        self._options_load()

    def _layout_load(self):
        Layout.__init__(self, self.parent_view)
        self.file_set(self._theme_file, "editje/collapsable/list/default")
        self.size_hint_weight_set(1.0, 1.0)
        self.size_hint_align_set(-1.0, -1.0)

        self._content_load()

    def _content_load(self):
        pass

    # Title
    def _title_set(self, value):
        self.edje_get().part_text_set("header.title", value)

    def _title_get(self):
        self.edje_get().part_text_get("header.title")

    title = property(_title_get, _title_set)

    #  OPEN / CLOSE
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

    def _closed_cb(self, obj, emission, source):
        self._open = False
        self.size_hint_weight_set(1.0, 0.0)

    def _open_get(self):
        return self._open

    open = property(_open_get, _open_set)

    #  Options
    def _options_load(self):
        self._options = False

    def _options_set(self, value):
        self._options = value
        if value:
            self.edje_get().signal_emit("options.enable", "")
        else:
            self.edje_get().signal_emit("options.disable", "")

    def _options_get(self):
        return self._options

    options = property(_options_get, _options_set)
