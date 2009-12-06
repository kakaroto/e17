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
import edje
import elementary

from editje import Editje
from fileselector import FileSelector

class OpenFile(elementary.Window):
    def __init__(self, new=False):
        elementary.Window.__init__(self, "openfile",
                                   elementary.ELM_WIN_BASIC)
        self.title_set("Open Edje")
        self.autodel_set(True)
        self.resize(600, 480)

        self._notification = None

        bg = elementary.Background(self)
        self.resize_object_add(bg)
        bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bg.show()

        self._fs = FileSelector(self)
        self._fs.filter = self._filter
        if new:
            self._fs.action_add("New", self._new)
        self._fs.action_add("Cancel", self._cancel)
        self._fs.action_add("Ok", self._open)
        self.resize_object_add(self._fs)
        self._fs.show()

    def _filter(self, file):
        return file.endswith(".edc") or file.endswith(".edj")


    def _open(self, bt):
        if not self._fs.file:
            self._notify("Select file")
            return

        list = edje.file_collection_list(self._fs.file)
        if not list and not self._fs.file.endswith(".edc"):
            self._notify("Invalid file")
            return

        editje = Editje()
        editje.file = self._fs.file
        if len(list) == 1:
            editje.group = list[0]
        else:
            editje.select_group()
        editje.show()
        self._cancel(bt)

    def _new(self, bt):
        return

    def _cancel(self, bt):
        self.hide()
        self.delete()

    def _notify(self, message):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None
        self._notification = elementary.Notify(self)
        self._notification.timeout_set(2)
        self._notification.orient_set(elementary.ELM_NOTIFY_ORIENT_BOTTOM_RIGHT)

        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self._notification.content_set(bx)
        bx.show()

        lb = elementary.Label(self)
        lb.label_set(message)
        bx.pack_end(lb)
        lb.show()

        self._notification.show()

