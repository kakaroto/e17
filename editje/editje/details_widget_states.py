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

from details_widget_entry_button import WidgetEntryButton
from floater import Floater
from groupselector import NameEntry
from operation import Operation
import objects_data


class WidgetStates(WidgetEntryButton):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent, editable, operation_stack_cb):
        WidgetEntryButton.__init__(self, parent, "editje/icon/options")

        self._operation_stack_cb = operation_stack_cb
        self._edit_grp = editable
        self._selstate = None
        self._pop = None
        self._edit_grp.part.callback_add("part.unselected", self._close_cb)
        self._edit_grp.part.callback_add("part.changed", self._close_cb)

    def _open(self, bt, *args):
        self.open()

    def _list_populate(self, *args):
        self.states.clear()
        for s in self._edit_grp.part.states:
            ico = None
            if s == self._edit_grp.part.state.name:
                ico = Icon(self.states)
                ico.file_set(self.theme_file, "editje/icon/confirm")
                ico.scale_set(0, 0)
            lbl = "%s %.2f" % s
            self.states.item_append(lbl, None, ico, self._show_actions, s)
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
        cur_state = self._edit_grp.part.state.name
        if re.match("[a-zA-Z]*\d{2,}", cur_state[0]):
            cur = cur_state[0][:-2]
        else:
            cur = cur_state[0]

        for state in self._edit_grp.part.states:
            if re.match("%s\d{2,}" % cur, state[0]):
                num = int(state[0][len(cur):])
                if num > max:
                    max = num
        nst = cur + "%.2d" % (max + 1)
        return nst

    # if st_to state does not exist for the part, it is created
    def _part_state_copy_from(self, part_name, st_from, st_to):
        self._edit_grp.part.name = part_name

        pt = self._edit_grp.part_get(part_name)
        existed = pt.state_exist(*st_to)

        # FIXME: totally don't know why state_copy was not working for the
        # latter case, return here when things change underneath. also fix
        # the ugly event emitions
        if not existed:
            pt.state_copy(st_from[0], st_from[1], st_to[0], st_to[1])
            self._edit_grp.part.event_emit("state.added", st_to)
            self._edit_grp.part.state.name = st_to
        else:
            st = pt.state_get(*st_to)
            st.copy_from(*st_from)
            self._edit_grp.part.state.event_emit("state.changed", st_to)

    def _remove_state_internal(self, state):
        if self._is_default(state):
            # FIXME: notify the user of it somehow
            return False

        if state == self._edit_grp.part.state.name:
            self._edit_grp.part.state.name = ("default", 0.00)
        return self._edit_grp.part.state_del(*state)

    def _states_added_cb(self, popup, data):
        new_state = self.newstate_entry.entry

        tmp = new_state.split(None, 1)
        if len(tmp) == 1:
            new_state = (tmp[0], 0.0)
        else:
            new_state = (tmp[0], float(tmp[1]))

        part_name = self._edit_grp.part.name
        part = self._edit_grp.part_get(part_name)
        curr_state = self._edit_grp.part.state.name

        if part.state_exist(*new_state):
            # FIXME: notify the user of it somehow
            return

        self._part_state_copy_from(part_name, curr_state, new_state)

        op = Operation("state addition")
        op.redo_callback_add(
            self._part_state_copy_from, part_name, curr_state, new_state)
        op.undo_callback_add(self._remove_state_internal, new_state)
        self._operation_stack_cb(op)

        self.close()

    def _is_default(self, state):
        if type(state) == str:
            return state == "default 0.00"
        return state == ("default", 0.00)

    def _state_restore(self, part_name, state_save, readd=False):
        self._edit_grp.part.name = part_name
        st_name = (state_save.name, state_save.value)

        if readd:
            if not self._edit_grp.part.state_add(*st_name):
                return

        state = self._edit_grp.part_get(part_name).state_get(*st_name)
        state_save.apply_to(state)

        # FIXME: ugly hacks
        if readd:
            # 2nd time for this sig
            self._edit_grp.part.event_emit("state.added", st_name)
        else:
            self._edit_grp.part.state.event_emit("state.changed", st_name)

        self._edit_grp.part.state.name = st_name

    def _remove_state_cb(self, btn, state_name):
        part_name = self._edit_grp.part.name
        part = self._edit_grp.part_get(part_name)
        st_obj = part.state_get(*state_name)
        st_class = objects_data.state_class_from_part_type_get(part)
        state_save = st_class(st_obj)

        if self._remove_state_internal(state_name):
            op = Operation("state deletion")
            op.redo_callback_add(self._remove_state_internal, state_name)
            op.undo_callback_add(
                self._state_restore, part_name, state_save, readd=True)
            self._operation_stack_cb(op)
        else:
            del state_save

        self._pop.actions_clear()
        self._pop.action_add("New", self._state_add_new_cb)
        self._pop.action_add("Close", self._cancel_clicked)
        self.pager.content_pop()

    def _reset_state_to_cb(self, it, st_from):
        part_name = self._edit_grp.part.name
        curr_state = self._edit_grp.part.state.name
        part = self._edit_grp.part_get(part_name)
        st_obj = part.state_get(*curr_state)
        st_class = objects_data.state_class_from_part_type_get(part)
        state_save = st_class(st_obj)

        self._part_state_copy_from(part_name, st_from, curr_state)

        op = Operation(
            "state copying (%s into %s)" % (st_from[0], curr_state[0]))
        op.redo_callback_add(
            self._part_state_copy_from, part_name, st_from, curr_state)
        op.undo_callback_add(self._state_restore, part_name, state_save)
        self._operation_stack_cb(op)

        self.close()

    def _cancel_clicked(self, popup, data):
        self.close()

    def _states_select_cb(self, it, state):
        self._selstate = state
        self._edit_grp.part.state.name = state
        self.close()

    def _action_button_add(self, label, callback, state):
        btn = Button(self._pop)
        btn.text_set(label)
        btn.callback_clicked_add(callback, state)
        btn.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        btn.size_hint_weight_set(0.0, 0.0)
        self.actions_box.pack_end(btn)
        btn.show()
        return btn

    def _show_actions(self, it, ti, state):
        strstate = "%s %.2f" % state
        self.actions_box = Box(self._pop)
        name_box = Box(self._pop)
        lb_state = Label(self._pop)
        lb_state.text_set("<b>State: ")
        lb_name = Label(self._pop)
        lb_name.text_set(strstate)
        name_box.horizontal_set(True)
        name_box.pack_end(lb_state)
        lb_state.show()
        name_box.pack_end(lb_name)
        lb_name.show()
        self.actions_box.pack_end(name_box)
        name_box.show()
        space = Label(self._pop)
        space.text_set(" ")
        self.actions_box.pack_end(space)
        space.show()
        self._action_button_add("Change to", self._states_select_cb, state)
        self._action_button_add("Reset to", self._reset_state_to_cb, state)
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
            self._pop = Floater(self.button, self.obj)
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

            self._edit_grp.part.callback_add(
                "states.changed", self._list_populate)
            self._edit_grp.part.state.callback_add(
                "state.changed", self._list_populate)

        self._list_populate()
        self._pop.show()

    def _close_cb(self, *args):
        self.close()

    def close(self):
        if not self._pop:
            return
        self._edit_grp.part.callback_del("states.changed", self._list_populate)
        self._edit_grp.part.state.callback_del(
            "state.changed", self._list_populate)
        self._pop.hide()
        self._pop = None

    def _internal_value_set(self, val):
        WidgetEntryButton._internal_value_set(self, val)
        if self._is_default(val):
            self.entry.disabled_set(True)
        else:
            self.entry.disabled_set(False)
