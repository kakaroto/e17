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

import edje

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_boolean import WidgetBoolean
from details_widget_combo import WidgetCombo
from details_widget_partlist import WidgetPartList
from prop import Property, PropertyTable

class PartDetails(EditjeDetails):
    def __init__(self, parent):
        EditjeDetails.__init__(self, parent,
                               group="editje/collapsable/part_properties")

        self.e.part.callback_add("part.changed", self._part_update)
        self.e.part.callback_add("name.changed", self._part_update)
        self.e.part.callback_add("part.unselected", self._part_removed)

        self.title_set("part properties")

        self._effects = ['NONE', 'PLAIN', 'OUTLINE', 'SOFT OUTLINE', 'SHADOW',
                         'SOFT SHADOW', 'OUTLINE SHADOW', 'OUTLINE SOFT SHADOW',
                         'FAR SHADOW', 'FAR SOFT SHADOW', 'GLOW']

        self._header_table = PropertyTable(parent)
        self._header_table._value_changed = self.header_prop_value_changed

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "type")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("t", wid)
        self._header_table.property_add(prop)

        self.content_set("part_name.swallow", self._header_table)

        prop = Property(parent, "clip_to")
        prop.widget_add("to", WidgetPartList(self, "Clipper selection"))
        self["main"].property_add(prop)
        prop = Property(parent, "mouse_events")
        prop.widget_add("me", WidgetBoolean(self))
        self["main"].property_add(prop)
        prop = Property(parent, "repeat_events")
        prop.widget_add("re", WidgetBoolean(self))
        self["main"].property_add(prop)
        # Missing properties: ignore_flags, scale, pointer_mode, precise_is_inside

        # textblock only (source is for group too, but later)
        self.group_add("textblock")
        prop = Property(parent, "effect")
        wid = WidgetCombo(self)
        for i in range(len(self._effects)):
            wid.item_add(self._effects[i])
        prop.widget_add("e", wid)
        self["textblock"].property_add(prop)
        # Missing properties: source*, entry_mode, select_mode, multiline

        self.main_hide()
        self.group_hide("textblock")

    def header_prop_value_changed(self, prop, value, group):
        if prop == "name":
            if not self.e.part.rename(value):
                self._header_table["name"].value = self.e.part.name

    def prop_value_changed(self, prop, value, group):
        if not group:
            self._prop_value_common_changed(prop, value)
        elif group == "textblock":
            self._prop_value_text_changed(prop, value)

    def _prop_value_common_changed(self, prop, value):
        if prop == "mouse_events":
            self.e.part._part.mouse_events = value
        elif prop == "repeat_events":
            self.e.part._part.repeat_events = value
        elif prop == "clip_to":
            self.e.part._part.clip_to = value
            clipper = self.e.part._part.clip_to
            if clipper:
                self["main"]["clip_to"].value = self.e.part._part.clip_to
            else:
                self["main"]["clip_to"].value = ""

    def _prop_value_text_changed(self, prop, value):
        if prop == "effect":
            self.e.part._part.effect = self._effects.index(value)

    def _part_update(self, emissor, data):
        if not self.e.part._part:
            return

        self._header_table["name"].value = self.e.part.name
        self._header_table["name"].show_value()
        type = self._part_type_to_text(self.e.part._part.type)
        self._header_table["type"].value = type
        self._header_table["type"].show_value()
        self._update_common_props()

        self.group_hide("textblock")
        if self.e.part._part.type == edje.EDJE_PART_TYPE_TEXT:
            self._update_text_props()

    def _part_removed(self, emissor, data):
        self._header_table["name"].value = None
        self._header_table["name"].hide_value()
        self._header_table["type"].value = None
        self._header_table["type"].hide_value()

        self.main_hide()
        self.group_hide("textblock")
        if not self.e.part._part:
            return

        self["main"]["clip_to"].hide_value()
        self["main"]["mouse_events"].hide_value()
        self["main"]["repeat_events"].hide_value()
        if self.e.part._part.type == edje.EDJE_PART_TYPE_TEXT:
            self["textblock"]["effect"].hide_value()

    def _update_common_props(self):
        self.main_hide()

        clipper = self.e.part._part.clip_to
        self["main"]["clip_to"].show_value()
        if clipper:
            self["main"]["clip_to"].value = clipper
        else:
            self["main"]["clip_to"].value = ""

        self["main"]["mouse_events"].show_value()
        self["main"]["mouse_events"].value = self.e.part._part.mouse_events

        self["main"]["repeat_events"].show_value()
        self["main"]["repeat_events"].value = self.e.part._part.repeat_events
        self.main_show()

    def _update_text_props(self):
        self["textblock"]["effect"].show_value()
        self["textblock"]["effect"].value = self._effects[self.e.part._part.effect]
        self.group_show("textblock")

    def _part_type_to_text(self, type):
        parttypes = ['NONE', 'RECTANGLE', 'TEXT', 'IMAGE', 'SWALLOW',
                     'TEXTBLOCK', 'GRADIENT', 'GROUP', 'BOX', 'TABLE',
                     'EXTERNAL']
        return parttypes[type]

