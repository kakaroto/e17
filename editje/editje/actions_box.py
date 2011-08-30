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


class ActionsBox(elementary.Box):
    def __init__(self, parent):
        elementary.Box.__init__(self, parent)
        self.horizontal_set(True)
        self.homogenous_set(False)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

        self._actions = {}

    def action_add(self, label, func_cb, icon=None, data=None):
        btn = elementary.Button(self)
        #btn.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        #btn.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        btn.text_set(label)

        if func_cb:
            btn.callback_clicked_add(func_cb, data)

        if icon:
            ico = elementary.Icon(self._parent)
            ico.file_set(self.__theme_file, "editje/icon/" + icon)
            btn.icon_set(ico)
            ico.show()

        self._actions[label] = btn

        btn.show()
        self.pack_end(btn)

    def action_disabled_set(self, label, value):
        bt = self._actions.get(label)
        if bt:
            bt.disabled_set(value)
