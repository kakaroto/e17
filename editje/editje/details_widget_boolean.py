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

from details_widget_button import WidgetButton


class WidgetBoolean(WidgetButton):
    def __init__(self, parent):
        WidgetButton.__init__(self, parent)
        self._label_true = "True"
        self._label_false = "False"
        self.value = True
        self.clicked = self._change

    def _internal_value_set(self, value):
        self._value = value
        self._update()

    def _internal_value_get(self):
        return self._value

    def states_set(self, true, false):
        self._label_true = true
        self._label_false = false
        self._update()

    def _change(self, *args):
        self.value = not self._value
        self._callback_call("changed")

    def _update(self):
        if self.value:
            self.obj.text_set(self._label_true)
        else:
            self.obj.text_set(self._label_false)
