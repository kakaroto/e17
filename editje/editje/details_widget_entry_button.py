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

import elementary

import sysconfig
from details_widget_entry import WidgetEntry


class WidgetEntryButton(WidgetEntry):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent, bt_icon=None):
        WidgetEntry.__init__(self, parent)

        self.selection_list = []

        self.button = elementary.Button(parent)
        self.button.size_hint_align_set(-1.0, -1.0)
        self.button.callback_clicked_add(self._open)
        self.button.style_set("editje.details")
        self.button.show()
        self.theme_file = sysconfig.theme_file_get("default")
        ico = elementary.Icon(self.button)
        if bt_icon:
            ico.file_set(self.theme_file, bt_icon)
        else:
            ico.file_set(self.theme_file, "editje/icon/entry_button")
        ico.size_hint_min_set(18, 18)
        ico.show()
        self.button.icon_set(ico)
        self.button.size_hint_min_set(20, 20)

        self.box = elementary.Box(parent)
        self.box.horizontal_set(True)
        self.box.size_hint_weight_set(1.0, 0.0)
        self.box.size_hint_align_set(-1.0, -1.0)
        self.box.pack_end(self.scr)
        self.box.pack_end(self.button)
        self.box.show()

        self.obj = self.box

    def _value_set(self, val):
        self._internal_value_set(val)

    def _value_get(self):
        return self._value

    value = property(_value_get, _value_set)

    def _entry_changed_cb(self, obj, *args, **kwargs):
        WidgetEntry._entry_changed_cb(self, obj, *args, **kwargs)

    def _internal_value_set(self, val):
        WidgetEntry._internal_value_set(self, val)
        self.entry.select_all()

    def tooltip_set(self, entry, button=None):
        self.entry.tooltip_text_set(entry)
        if not button:
            button = entry
        self.button.tooltip_text_set(button)
