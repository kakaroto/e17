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
from misc import validator_int, validator_float


class WidgetEntryValidator(object):
    def __init__(self):
        self._validated = True
        self._validator_rect = None
        self.validator_set(self._validator_default)

    # Validator

    def _validator_default(self, value):
        return True

    def validator_set(self, validator, *args, **kargs):
        if not callable(validator):
            raise TypeError("validator are not callable")
        self._validator = (validator, args, kargs)

    def _validator_call(self, obj, value):
        validator, args, kargs = self._validator
        self._validated = validator(value, *args, **kargs)
        if self._validated:
            self._validated_value = value
        else:
            self._validated_value = None
        return self._validated


class WidgetEntry(Widget, WidgetEntryValidator):
    def __init__(self, parent):
        Widget.__init__(self)
        WidgetEntryValidator.__init__(self)
        self._value = ""

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

        self.scr = elementary.Scroller(parent)
        self.scr.style_set("editje.details")
        self.scr.size_hint_weight_set(1.0, 0.0)
        self.scr.size_hint_align_set(-1.0, -1.0)
        self.scr.content_min_limit(False, True)
        self.scr.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                            elementary.ELM_SCROLLER_POLICY_OFF)
        self.scr.bounce_set(False, False)
        self.scr.content_set(self.entry)
        self.scr.show()

        self.obj = self.scr

    def disabled_set(self, val):
        self.entry.disabled_set(val)

    def _internal_value_set(self, val):
        if val is None:
            val = ""
        self._value = val
        self.entry.entry_set(val)

    def _internal_value_get(self):
        return self._value

    def _update_value(self):
        if self._validated:
            if self._value == self._validated_value:
                return
            self._value = self._validated_value
            self._callback_call("changed")
        else:
            self.entry.entry_set(self._value)

    def _entry_activate_cb(self, obj, *args, **kwargs):
        self._update_value()

    def _entry_changed_cb(self, obj, *args, **kwargs):
        entry = self.entry.entry_get()
        text = self.entry.markup_to_utf8(entry)
        self._validator_call(obj, text)

    def _focused_cb(self, obj):
        self.entry.select_all()

    def _unfocused_cb(self, obj):
        self._update_value()
        self.entry.select_none()

    def _dblclick_cb(self, obj):
        self.entry.select_all()

    def type_float(self):
        self.validator_set(validator_float)
        self.parser_in = self._in_parser_number
        self.parser_out = self._out_parser_float

    def type_int(self):
        self.validator_set(validator_int)
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
