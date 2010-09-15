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
import edje
import elementary

import colorpicker
from details_widget_entry import WidgetEntry
from floater_opener import FloaterOpener
from misc import validator_rgba


class WidgetColor(FloaterOpener, WidgetEntry):
    padding_x = 20
    padding_y = 20

    def __init__(self, parent, editable, popup_hide_object_signal_list=[]):
        FloaterOpener.__init__(self, popup_hide_object_signal_list)
        WidgetEntry.__init__(self, parent)
        self.validator_set(validator_rgba)
        self._edit_grp = editable
        self.color = (255, 255, 255, 255)
        self.parent = parent
        self.entry.callback_activated_add(self._entry_activate_cb)
        self.entry.callback_changed_add(self._entry_changed_cb)
        self.entry.callback_double_clicked_add(self._dblclick_cb)

        self.scr.content_min_limit(False, True)

        ed = parent.edje_get()
        file = ed.file_get()[0]
        self.rect = edje.Edje(ed.evas, file=file, group="colorpreviewer")
        self.rect.size_hint_align_set(-1.0, -1.0)
        self.rect.size_hint_min_set(*self.rect.size_min_get())
        self.rect.on_mouse_down_add(self._sample_clicked_cb)
        self.rect.show()

        self.box = elementary.Box(parent)
        self.box.horizontal_set(True)
        self.box.size_hint_weight_set(1.0, 0.0)
        self.box.size_hint_align_set(-1.0, -1.0)
        self.box.pack_end(self.scr)
        self.box.pack_end(self.rect)
        self.box.show()

        self.obj = self.box

        self._delayed_callback = False

    def _value_set(self, val):
        self._internal_value_set("%d %d %d %d" % val)
        self.color = val

    def _value_get(self):
        return self.color

    value = property(_value_get, _value_set)

    def show(self):
        self.scr.show()
        self.rect.show()

    def hide(self):
        self.scr.hide()
        self.rect.hide()

    def _entry_changed_cb(self, obj, *args, **kwargs):
        entry = self.entry.entry_get()
        val = self.entry.markup_to_utf8(entry)
        if not self._validator_call(self.obj, val):
            self.rect.color_class_set("colorpicker.sample", 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0)
            return

        if val[0] == "#":
            try:
                r, g, b, a = evas.color_parse(val)
            except Exception, e:
                return
        else:
            try:
                t = []
                t = val.split(None)
                if len(t) < 3:
                    return None
                elif len(t) == 4:
                    r, g, b, a = t
                else:
                    r, g, b = t
                    a = 255
            except Exception, e:
                return
        try:
            r = int(r)
            g = int(g)
            b = int(b)
            a = int(a)

        except Exception, e:
            print "Error parsing integer values for color: ", str(e)
            return

        self.color = (r, g, b, a)
        self.rect.color_class_set("colorpicker.sample", r, g, b, a,
                                  0, 0, 0, 0, 0, 0, 0, 0)

        if self._delayed_callback:
            self._delayed_callback = False
            self._update_value()

    def _update_value(self):
        if self._validated:
            if self._value == self._validated_value:
                return
            self._value = self._validated_value
            self._callback_call("changed")
        else:
            self.entry.entry_set("%d %d %d %d" % self.color)

    def _dblclick_cb(self, obj):
        self.entry.select_all()

    def _sample_clicked_cb(self, obj, event):
        self._floater_open(self.parent)
        self.picker.current_color_set(*self.color)

    def _set_clicked(self, popup, data):
        val = self.picker.current_color_get()
        self._delayed_callback = True
        self.entry.entry_set("%d %d %d %d" % val)
        self._floater.hide()

    def _floater_content_init(self):
        self.picker = colorpicker.Colorpicker(self.parent)
        self.picker.show()
        self._edit_grp.part.callback_add(
            "part.unselected", self._floater_cancel)
        self._edit_grp.part.callback_add(
            "part.changed", self._floater_cancel)
        self._floater.content_set(self.picker)

    def _floater_actions_init(self):
        self._floater.action_add("Set", self._set_clicked)
        FloaterOpener._floater_actions_init(self)

    def _floater_title_init(self):
        self._floater.title_set("Color")

    def tooltip_set(self, entry, button=None):
        self.entry.tooltip_text_set(entry)
