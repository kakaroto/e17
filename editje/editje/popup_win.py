# Copyright (C) 2010 Samsung Electronics.
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

from actions_box import ActionsBox


class PopupWindow(elementary.Window):
    def __init__(self, name="popup", type=elementary.ELM_WIN_BASIC):
        elementary.Window.__init__(self, name, type)
        self.autodel_set(True)

        bg = elementary.Background(self)
        self.resize_object_add(bg)
        bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bg.show()

        self._table = elementary.Box(self)
        self._table.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self._table.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self.resize_object_add(self._table)
        self._table.show()

        self._actions = ActionsBox(self._table)
        self._table.pack_end(self._actions)
        self._actions.show()

    def _title_set(self, title):
        self.title_set(title)

    def _title_get(self):
        return self.title_get()

    title = property(_title_get, _title_set)

    def action_add(self, label, func_cb, icon=None, data=None):
        self._actions.action_add(label, func_cb, icon, data)

    def action_disabled_set(self, label, disabled):
        self._actions.action_disabled_set(label, disabled)

    def pack_end(self, obj):
        self._table.pack_before(obj, self._actions)

if __name__ == "__main__":
    elementary.init()
    elementary.policy_set(elementary.ELM_POLICY_QUIT,
                          elementary.ELM_POLICY_QUIT_LAST_WINDOW_CLOSED)

    pop = PopupWindow()

    pop.action_add('XXX', None)
    pop.action_add('YYY', None)
    pop.action_add('ZZZ', None)
    pop.action_add('WWW', None)

    lb = elementary.Label(pop)
    lb.text_set("XXXXXXXXXXXXXX<br>XXXXXXXXXXX<br>XXXXXXXXXXXXXX<br>")
    lb.show()
    pop.pack_end(lb)

    pop.show()

    elementary.run()
    elementary.shutdown()
