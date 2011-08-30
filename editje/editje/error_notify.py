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

from actions_box import ActionsBox


class ErrorNotify(elementary.Notify):
    def __init__(self, parent, orient=elementary.ELM_NOTIFY_ORIENT_BOTTOM):
        elementary.Notify.__init__(self, parent)
        self.timeout_set(-1)
        self.orient_set(orient)
        self.repeat_events_set(False)

        self._table = elementary.Box(parent)
        self._table.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self.content_set(self._table)
        self._table.show()

        self._title = elementary.Label(self._table)
        self._table.pack_start(self._title)
        self.title = "TITLE"
        self._title.show()

        self._actions = ActionsBox(self._table)
        self._table.pack_end(self._actions)
        self._actions.show()

    def delete(self):
        self._table.delete()
        elementary.Notify.delete(self)

    def _title_set(self, title):
        self._title_value = title
        self._title.text_set("<b>" + title + "</b>")

    def _title_get(self):
        return self._title_value

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
    win = elementary.Window("Test", elementary.ELM_WIN_BASIC)
    win.title_set("Test")
    win.autodel_set(True)
    win.resize(800, 600)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    win.show()

    nt = ErrorNotify(win)
    nt.title = "Testing popup"

    nt.action_add('XXX', None)
    nt.action_add('YYY', None)
    nt.action_add('ZZZ', None)
    nt.action_add('WWW', None)

    lb = elementary.Label(nt)
    lb.text_set("XXXXXXXXXXXXXX<br>XXXXXXXXXXX<br>XXXXXXXXXXXXXX<br>")
    lb.show()
    nt.pack_end(lb)

    nt.show()

    elementary.run()
    elementary.shutdown()
