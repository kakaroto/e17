#
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.
import elementary

from details_widget import Widget
from floater import Floater


class WidgetEntryButton(Widget):

    def __init__(self, parent):
        Widget.__init__(self)

        self.entry_value = ""
        self.selection_list = []

        self.parent = parent
        self.entry = elementary.Entry(parent)
        self.entry.single_line_set(1)
        self.entry.style_set("editje.details")
        self.entry.size_hint_weight_set(1.0, 0.0)
        self.entry.context_menu_disabled_set(True)
        self.entry.callback_activated_add(self._entry_activate_cb)
        self.entry.callback_changed_add(self._entry_changed_cb)
        self.entry.callback_double_clicked_add(self._dblclick_cb)
        self.entry.show()

        self.scr = elementary.Scroller(parent)
        self.scr.style_set("editje.details")
        self.scr.size_hint_weight_set(1.0, 0.0)
        self.scr.size_hint_align_set(-1.0, -1.0)
        self.scr.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                            elementary.ELM_SCROLLER_POLICY_OFF)
        self.scr.bounce_set(False, False)
        self.scr.content_set(self.entry)
        self.scr.content_min_limit(False, True)
        self.scr.show()

        self.rect = elementary.Button(parent)
        self.rect.label_set("...")
        self.rect.size_hint_align_set(-1.0, -1.0)
        self.rect.size_hint_min_set(30,16)
        self.rect.on_mouse_down_add(self._open)
        self.rect.style_set("editje.details")
        self.rect.show()

        self.obj = elementary.Box(parent)
        self.obj.horizontal_set(True)
        self.obj.size_hint_weight_set(1.0, 0.0)
        self.obj.size_hint_align_set(-1.0, -1.0)
        self.obj.pack_end(self.scr)
        self.obj.pack_end(self.rect)
        self.obj.show()

        self.delayed_callback = 0

    def _value_set(self, val):
        self.entry.entry_set(val)
        self.entry_value = val

    def _value_get(self):
        return self.entry_value

    value = property(_value_get, _value_set)

    def _entry_changed_cb(self, obj, *args, **kwargs):
        val = self.entry.entry_get().replace("<br>", "")
        if len(val) == 0:
            return
        self.entry_value = val
        if self.delayed_callback:
            self._callback_call("changed")
            self.delayed_callback = 0

    def _entry_activate_cb(self, obj, *args, **kwargs):
        self._callback_call("changed")

    def _dblclick_cb(self, obj):
        self.entry.select_all()

    def _items_load(self):
        list = []
        return list

    def _actions_init(self):
        self._pop.action_add("Cancel", self._cancel_clicked)

    def _open(self, bt, *args):
        self._pop = Floater(self.parent)
        list = elementary.List(self.parent)

        for item, returned_value in self._items_load():
            i = list.item_append(item, None, None, self._list_select_cb,
                                     returned_value)

            if returned_value == self.value:
                i.selected_set(True)

        list.scroller_policy_set(elementary.ELM_SCROLLER_POLICY_OFF, elementary.ELM_SCROLLER_POLICY_ON)
        list.go()
        list.show()

        self._pop.content_set(list)

        self._actions_init()

        x, y, w, h = self.obj.geometry

        cw, ch = self.obj.evas.size
        ow, oh = 200, 300
        ox = x - (ow - w) / 2
        oy = y - (oh - h) / 2

        if ox < 0:
            ox = 0
        elif ox + ow >= cw:
            ox = cw - ow

        if oy < 0:
            oy = 0
        elif oy + oh >= ch:
            oy = ch - oh

        self._pop.move(ox, oy)
        self._pop.resize(ow, oh)
        self._pop.show()

    def _cancel_clicked(self, popup, data):
        self._pop.hide()

    def _select_cb(self, obj, data):
        item = data
        self.value = item
        self._callback_call("changed")
        self._cancel_clicked(list, item)

    def _list_select_cb(self, list, it, entry_value):
        self._select_cb(list, entry_value)

    def _internal_value_get(self):
        return self.entry.entry_get().replace("<br>", "")
