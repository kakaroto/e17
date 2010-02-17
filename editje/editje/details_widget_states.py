#
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.
import elementary
import evas

from details_widget_entry_button import WidgetEntryButton
from floater import Floater
from elementary import Notify, Label, Box, ELM_NOTIFY_ORIENT_BOTTOM

import sysconfig
import re


class WidgetStates(WidgetEntryButton):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent, mainwindow, editable):
        WidgetEntryButton.__init__(self, parent)
        self.mainwindow = mainwindow
        self.editable = editable
        self._selstate = None
        self.theme_file = sysconfig.theme_file_get("default")
        ico = elementary.Icon(self.rect)
        ico.file_set(self.theme_file, "editje/icon/options")
        ico.show()
        self.rect.label_set("")
        self.rect.icon_set(ico)
        self._pop = Floater(self.rect, self.obj)
        self._pop.size_min_set(self.pop_min_w, self.pop_min_h)
        self._pop.title_set("States selection")
        self.states = elementary.List(self._pop._popup)
        self.states.size_hint_weight_set(1.0, 1.0)
        self.states.size_hint_align_set(-1.0, -1.0)
        self._pop.content_set(self.states)
	self.states.show()

        self.editable.part.callback_add("states.changed", self._list_populate)
        self._actions_init()


    def _actions_init(self):
        self._pop.action_add("New", self._state_add_new_cb)
        self._pop.action_add("Cancel", self._cancel_clicked)

    def _open(self, bt, *args):
        self._list_populate()
        self.show()

    def _list_populate(self, *args):
        self.states.clear()
        for s in self.editable.part.states:

            old = self.parent.state.name

            box = elementary.Box(self._pop)
            box.horizontal_set(True)
            box.show()

            it = self.states.item_append(s, None, box, self._states_select_cb, s)

            ico1 = elementary.Icon(self._pop)
            ico2 = elementary.Icon(self._pop)

            if s == self.editable.part.state.name:
                ico1.file_set(self.theme_file, "editje/icon/part_disabled")
                ico2.file_set(self.theme_file, "editje/icon/cancel_disabled")
            else:
                if s == "default 0.00":
                    ico1.file_set(self.theme_file, "editje/icon/part")
                    ico2.file_set(self.theme_file, "editje/icon/cancel_disabled")
                else:
                    ico1.file_set(self.theme_file, "editje/icon/part")
                    ico2.file_set(self.theme_file, "editje/icon/cancel")

            ico1.show()
            ico2.show()

            btn1 = elementary.Button(self._pop)
            btn1.icon_set(ico1)
            btn1.style_set("editje.details")
            btn1.label_set("")
            btn1.callback_clicked_add(self._reset_state_to_cb, None, (old, s))
            btn1.show()

            btn2 = elementary.Button(self._pop)
            btn2.icon_set(ico2)
            btn2.style_set("editje.details")
            btn2.label_set("")
            btn2.callback_clicked_add(self._remove_state_cb, None, (old, s))
            btn2.show()

            if s == self.editable.part.state.name:
                btn1.disabled_set(True)
                btn2.disabled_set(True)
            if s == "default 0.00":
                btn2.disabled_set(True)

            box.pack_end(btn1)
            box.pack_end(btn2)
        self.states.go()

    def _state_add_new_cb(self, popup, data):
        max = 0

        if self.editable.part.state.name == None:
            return

        cur_state = self.editable.part.state.name.split(None,1)
        if re.match("[a-zA-Z]*\d{2,}", cur_state[0]):
            cur = cur_state[0][:-2]
        else:
            cur = cur_state[0]

        for p in self.editable.part.states:
             state = p.split(None, 1)
             if re.match("%s\d{2,}" % cur, state[0]):
                 num = int(state[0][len(cur):])
                 if num > max:
                       max = num
        nst = cur + "%.2d" % (max + 1)
        st = nst + " 0.00"
        if not self.parent.part.state_exist(st):
            self.parent.part.state_copy(self.parent.state.name, nst)
            self.editable.part.event_emit("state.added", st)
            self.editable.part.state.name = st


    def _remove_state_cb(self, obj, event, st):
        self.editable.part.state_del(st[1])
        self.editable.part.state.name = st[0]
        self.show()

    def _reset_state_to_cb(self, obj, event, st):
        self.editable.part.state.name = st[0]
        self.parent.state.copy_from(st[1])
        self.editable.part.state.event_emit("state.changed", self.parent.state.name)

    def _cancel_clicked(self, popup, data):
        self.hide()

    def _states_select_cb(self, states, it, state):
        if self._selstate != state:
            self._selstate = state
            self.editable.part.state.name = state
        self.hide()

    def hide(self):
        self._pop.hide()

    def show(self):
        self._pop.show()

    def selected_get(self):
        return self._selstate

    def close(self):
        self.editable.part.callback_del("states.changed",
                                         self._list_populate)
        self._pop.close()

    def _entry_changed_cb(self, obj, *args, **kwargs):
        WidgetEntryButton._entry_changed_cb(self, obj, *args, **kwargs)
        if self.entry.entry_get() == "default 0.00":
            self.entry.disabled_set(True)
        else:
            self.entry.disabled_set(False)
