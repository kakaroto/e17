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
from elementary import InnerWindow, Layout, Button, Box

import sysconfig


class PopUp(InnerWindow):
    def __init__(self, parent, group=None):
        InnerWindow.__init__(self, parent)
        self._parent = parent

        self._layout = Layout(self._parent)
        _theme_file = sysconfig.theme_file_get("default")
        _group = group or "editje/popup"
        self._layout.file_set(_theme_file, _group)
        self._layout_edje = self._layout.edje_get()
        self._layout.show()

        InnerWindow.content_set(self, self._layout)

    def __edje_get(self):
        return self._layout_edje

    layout_edje = property(__edje_get)

    def label_set(self, part, text):
        self._layout.edje_get().part_text_set(part, text)

    def content_set(self, part, content):
        self._layout.content_set(part, content)

    def open(self):
        self._parent.block(True)
        InnerWindow.show(self)

    def close(self):
        self.hide()
        self._parent.block(False)
        self.delete()


class ConfirmPopUp(PopUp):
    def __init__(self, parent, group=None, select_cb=None):
        PopUp.__init__(self, parent, group or "editje/confirm")
        self.style_set("minimal")

        self.text_set("header.title", "Overwrite existing file ?")

        self._set_controls()

    def set_message(self, message):
        self.text_set("filename", message)

    def action_add(self, label, func_cb, data=None):
        btn = Button(self)
        btn.text_set(label)
        btn.data["clicked"] = (func_cb, data)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_min_set(100, 30)
        btn.size_hint_max_set(100, 30)
        btn.show()
        self.box.pack_end(btn)

    def _action_btn_clicked(self, bt, *args, **kwargs):
        func, udata = bt.data["clicked"]
        func(udata)

    def _set_controls(self):
        self.box = Box(self)
        self.box.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                      evas.EVAS_HINT_EXPAND)
        self.box.size_hint_align_set(evas.EVAS_HINT_FILL,
                                     evas.EVAS_HINT_FILL)
        self.box.horizontal_set(True)
        self.box.show()
        self.content_set("actions", self.box)

    def _close_cb(self, data):
        self.close()

    def close(self):
        PopUp.close(self)
