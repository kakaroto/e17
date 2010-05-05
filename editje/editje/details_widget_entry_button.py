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
from details_widget_entry import WidgetEntryValidator


class WidgetEntryButton(Widget, WidgetEntryValidator):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent):
        Widget.__init__(self)
        WidgetEntryValidator.__init__(self)

        self._value = ""
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
        self.rect.size_hint_min_set(30, 16)
        self.rect.callback_clicked_add(self._open)
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
        self._internal_value_set(val)

    def _value_get(self):
        return self._value

    value = property(_value_get, _value_set)

    def _entry_changed_cb(self, obj, *args, **kwargs):
        entry = self.entry.entry_get()
        text = self.entry.markup_to_utf8(entry)
        self._validator_call(self.obj, text)
        if self.delayed_callback:
            self._callback_call("changed")
            self.delayed_callback = 0

    def _entry_activate_cb(self, obj, *args, **kwargs):
        if self._validated:
            self._value = self._validated_value
            self._callback_call("changed")
        else:
            self.entry.entry_set(self._value)

    def _dblclick_cb(self, obj):
        self.entry.select_all()

    def _internal_value_get(self):
        return self.entry.entry_get()

    def _internal_value_set(self, val):
        if val is None:
            val = ""
        self._value = val
        self.entry.entry_set(val)
        self.entry.select_all()
