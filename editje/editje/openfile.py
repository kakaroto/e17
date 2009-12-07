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
import shutil
import evas
import edje
import elementary

from editje import Editje
from fileselector import FileSelector
from popups import NewFilePopUp
import sysconfig

class OpenFile(elementary.Window):
    def __init__(self, theme="default"):

        self.theme = sysconfig.theme_file_get(theme)

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
        self._fs.action_add("New", self._new)
        self._fs.action_add("Cancel", self._cancel)
        self._fs.action_add("Ok", self._open)
        self.resize_object_add(self._fs)
        self._fs.show()

        self._load_shade()

    def block(self, bool):
        if bool:
            self.shade.show()
        else:
            self.shade.hide()

    def _load_shade(self):
        self.shade = evas.Rectangle(self.evas, color=(0, 0, 0, 200))
        self.shade.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(self.shade)

    def _filter(self, file):
        return file.endswith(".edc") or file.endswith(".edj")

    def _path_set(self, value):
        self._fs.path = value

    path = property(fset=_path_set)

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

    def _state_popup_place(self, popup):
        x, y, w, h = self.geometry
        popup.move(x + 150, y + 140)
        popup.resize(300, 200)

    def _open_template(self, data):
        dest_name = self._newfile_pop.entry
        if dest_name == "":
            self._notify("Please enter a name for the new file")
            return

        if not dest_name.endswith(".edj"):
            dest_name += ".edj"
        # TODO: multiple template types to choose from in the future
        t = sysconfig.template_file_get("default")
        dest = os.path.join(self._fs.path, dest_name)
        shutil.copyfile(t, dest)

        editje = Editje()
        editje.file = dest
        editje.group = "default"

        self._pop_cancel(None)
        editje.show()
        self._cancel(None)

    def _new(self, bt):
        self._newfile_pop = NewFilePopUp(self)
        self._newfile_pop.action_add("Ok", self._open_template)
        self._newfile_pop.action_add("Cancel", self._pop_cancel)
        self._newfile_pop.open()

    def _pop_cancel(self, data):
        self._newfile_pop.close()
        self._newfile_pop = None

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

