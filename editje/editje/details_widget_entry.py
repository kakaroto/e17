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


class WidgetEntry(Widget):
    def __init__(self, parent):
        Widget.__init__(self)

        self.entry = elementary.Entry(parent)
        self.entry.single_line_set(1)
        self.entry.style_set("editje.details")
        self.entry.size_hint_weight_set(1.0, 0.0)
        self.entry.context_menu_disabled_set(True)
        self.entry.callback_activated_add(self._entry_activate_cb)
        self.entry.callback_changed_add(self._entry_changed_cb)
        self.entry.callback_double_clicked_add(self._dblclick_cb)
        self.entry.callback_focused_add(self._focused_cb)
        self.entry.callback_unfocused_add(self._unfocused_cb)
        self.entry.show()

        self.obj = elementary.Scroller(parent)
        self.obj.style_set("editje.details")
        self.obj.size_hint_weight_set(1.0, 0.0)
        self.obj.size_hint_align_set(-1.0, -1.0)
        self.obj.content_min_limit(False, True)
        self.obj.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                            elementary.ELM_SCROLLER_POLICY_OFF)
        self.obj.bounce_set(False, False)
        self.obj.content_set(self.entry)
        self.obj.show()

    def disabled_set(self, val):
        self.entry.disabled_set(val)

    def validate(self, value):
        return True

    def _internal_value_set(self, val):
        if val is None:
            val = ""
        self.entry.entry_set(val)

    def _internal_value_get(self):
        text = self.entry.entry_get()
        return self.entry.markup_to_utf8(text)

    def _entry_activate_cb(self, obj, *args, **kwargs):
        self._callback_call("changed")

    def _entry_changed_cb(self, obj, *args, **kwargs):
        if self.validate(self.entry.entry_get()):
            # set entry in valid mode
            pass
        else:
            # set entry in invalid mode
            pass

    def _focused_cb(self, obj):
        self.entry.select_all()

    def _unfocused_cb(self, obj):
        self.entry.select_none()

    def _dblclick_cb(self, obj):
        self.entry.select_all()

    def type_float(self):
        self.parser_in = self._in_parser_number
        self.parser_out = self._out_parser_float

    def type_int(self):
        self.parser_in = self._in_parser_number
        self.parser_out = self._out_parser_int

    def _out_parser_float(self, val):
        try:
            return float(val)
        except Exception, e:
            return None

    def _out_parser_int(self, val):
        try:
            return int(val)
        except Exception, e:
            return None

    def _in_parser_number(self, val):
        if val is None:
            return ""
        return str(val)
