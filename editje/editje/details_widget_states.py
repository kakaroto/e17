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

import re

import evas
from elementary import Label, Box, Pager, Button, Icon, List

import sysconfig
from details_widget_entry_button import WidgetEntryButton
from floater import Floater
from groupselector import NameEntry


class WidgetStates(WidgetEntryButton):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent, editable):
        WidgetEntryButton.__init__(self, parent)
        self.editable = editable
        self._selstate = None
        self.theme_file = sysconfig.theme_file_get("default")

        ico = Icon(self.rect)
        ico.file_set(self.theme_file, "editje/icon/options")
        ico.show()
        self.rect.label_set("")
        self.rect.icon_set(ico)
        self.rect.size_hint_min_set(24, 24)

        self._pop = None

    def _open(self, bt, *args):
        self.open()

    def _list_populate(self, *args):
        self.states.clear()
        old = self.parent.state.name
        for s in self.editable.part.states:
            ico = None
            if s == self.editable.part.state.name:
                ico = Icon(self.states)
                ico.file_set(self.theme_file, "editje/icon/confirm")
                ico.scale_set(0, 0)
            it = self.states.item_append(s, None, ico, self._show_actions, s)
        self.states.go()

    def _state_add_new_cb(self, popup, data):
        self.newstate_entry = NameEntry(self._pop)
        self.newstate_entry.entry = self._state_newname()
        self.newstate_entry.show()
        self.pager.content_push(self.newstate_entry)
        self._pop.actions_clear()
        self._pop.action_add("Add", self._states_added_cb)
        self._pop.action_add("Close", self._cancel_clicked)

    def _state_newname(self):
        max = 0
        cur_state = self.editable.part.state.name.split(None, 1)
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
        nst = cur + "%.2d" % (max + 1) + " 0.00"
        return nst

    def _states_added_cb(self, it, ti):
        st = self.newstate_entry._entry_get()
        nst = st.split(None, 1)[0]
        if not self.parent.part.state_exist(st):
            self.parent.part.state_copy(self.parent.state.name, nst)
            self.editable.part.event_emit("state.added", st)
            self.editable.part.state.name = nst + " 0.00"
            self.close()

    def _is_default(self, state):
        return state == "default 0.00"

    def _remove_state_cb(self, it, state):
        if not self._is_default(state):
            if state == self.editable.part.state.name:
                self.editable.part.state.name = "default 0.00"
            self.editable.part.state_del(state)

        self._pop.actions_clear()
        self._pop.action_add("New", self._state_add_new_cb)
        self._pop.action_add("Close", self._cancel_clicked)
        self.pager.content_pop()

    def _reset_state_to_cb(self, it, state):
        self.parent.state.copy_from(state)
        self.editable.part.state.event_emit(
            "state.changed", self.parent.state.name)
        self.close()

    def _cancel_clicked(self, popup, data):
        self.close()

    def _states_select_cb(self, it, state):
        self._selstate = state
        self.editable.part.state.name = state
        self.close()

    def _action_button_add(self, label, callback, state):
        btn = Button(self._pop)
        btn.label_set(label)
        btn.callback_clicked_add(callback, state)
        btn.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        btn.size_hint_weight_set(0.0, 0.0)
        self.actions_box.pack_end(btn)
        btn.show()
        return btn

    def _show_actions(self, it , ti, state):
        self.actions_box = Box(self._pop)
        name_box = Box(self._pop)
        lb_state = Label(self._pop)
        lb_state.label_set("<b>State: ")
        lb_name = Label(self._pop)
        lb_name.label_set(state)
        name_box.horizontal_set(True)
        name_box.pack_end(lb_state)
        lb_state.show()
        name_box.pack_end(lb_name)
        lb_name.show()
        self.actions_box.pack_end(name_box)
        name_box.show()
        space = Label(self._pop)
        space.label_set(" ")
        self.actions_box.pack_end(space)
        space.show()
        btn_changeto = self._action_button_add(
            "Change to", self._states_select_cb, state)
        btn_resetto = self._action_button_add(
            "Reset to", self._reset_state_to_cb, state)
        btn_delete = self._action_button_add(
            "Delete", self._remove_state_cb, state)

        if self._is_default(state):
            btn_delete.disabled_set(True)
            lb_name.style_set("editje.statedefault")
        else:
            lb_name.style_set("editje.statenormal")

        self.pager.content_push(self.actions_box)
        self._pop.actions_clear()
        self._pop.action_add("Back", self._back_to_list_cb)
        self._pop.action_add("Close", self._cancel_clicked)

    def _back_to_list_cb(self, it, ti):
        self.pager.content_pop()
        self._pop.actions_clear()
        self._pop.action_add("New", self._state_add_new_cb)
        self._pop.action_add("Close", self._cancel_clicked)
        self.states.selected_item_get().selected_set(False)

    def open(self):
        if not self._pop:
            self._pop = Floater(self.rect, self.obj)
            self.pager = Pager(self._pop)
            self.pager.style_set("editje.rightwards")
            self.states = List(self._pop)
            self.states.size_hint_weight_set(
                evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
            self.states.size_hint_align_set(-1.0, -1.0)
            self.states.show()
            self.pager.content_push(self.states)
            self._pop.content_set(self.pager)
            self._pop.size_min_set(self.pop_min_w, self.pop_min_h)
            self._pop.title_set("States selection")
            self._pop.action_add("New", self._state_add_new_cb)
            self._pop.action_add("Close", self._cancel_clicked)
            self.editable.part.callback_add(
                "states.changed", self._list_populate)

        self._list_populate()
        self._pop.show()

    def close(self):
        if not self._pop:
            return
        self.editable.part.callback_del("states.changed",  self._list_populate)
        self._pop.hide()
        self._pop = None

    def _entry_changed_cb(self, obj, *args, **kwargs):
        WidgetEntryButton._entry_changed_cb(self, obj, *args, **kwargs)
        if self._is_default(self.entry.entry_get()):
            self.entry.disabled_set(True)
        else:
            self.entry.disabled_set(False)
