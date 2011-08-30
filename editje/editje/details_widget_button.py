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

from details_widget import Widget


class WidgetButton(Widget):
    def __init__(self, parent):
        Widget.__init__(self)
        self.parent = parent

        self.obj = elementary.Button(parent)
        self.obj.size_hint_weight_set(1.0, 0.0)
        self.obj.size_hint_align_set(-1.0, 0.0)
        self.obj.style_set("editje.details")
        self.obj.callback_clicked_add(self._clicked)
        self.obj.show()
        self._value = ""

    def _internal_value_set(self, value):
        self._value = value or ""
        self.obj.text_set(self._value)

    def _internal_value_get(self):
        return self._value

    def _clicked(self, obj, *args, **kwargs):
        self._callback_call("clicked", obj)

    def _clicked_cb_set(self, cb):
        self.callback_add("clicked", cb)

    clicked = property(fset=_clicked_cb_set)

    def tooltip_set(self, label):
        self.obj.tooltip_text_set(label)
