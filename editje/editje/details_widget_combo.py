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

from details_widget import Widget


class WidgetCombo(Widget):
    def __init__(self, parent):
        Widget.__init__(self)
        self.obj = elementary.Hoversel(parent)
        self.obj.hover_parent_set(parent)
        self.obj.style_set("editje")
        self.obj.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self.obj.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        self.obj.show()
        self.items = []

    def _internal_value_set(self, value):
        for it in self.items:
            if value == it.label_get():
                self.obj.text_set(value)
                break

    def _internal_value_get(self):
        return self.obj.label_get()

    def item_add(self, item):
        it = self.obj.item_add(item, "", 0, self._hover_item_selected_cb, item)
        self.items.append(it)

    def hover_end(self):
        self.obj.hover_end()

    def clear(self):
        self.items = []
        self.obj.clear()

    def _hover_item_selected_cb(self, obj, it, item, *args, **kwargs):
        if self.obj.label_get() == item:
            return
        self.obj.text_set(item)
        self._callback_call("changed")

    def tooltip_set(self, label):
        self.obj.tooltip_text_set(label)
