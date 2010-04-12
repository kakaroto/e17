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
from operation import Operation


class PartDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(
            self, parent, operation_stack_cb,
            group="editje/collapsable/part_properties")

        self.e.part.callback_add("part.changed", self._part_update)
        self.e.part.callback_add("name.changed", self._part_update)
        self.e.part.callback_add("part.unselected", self._part_removed)
        self.e.part.callback_add(
            "part.mouse_events.changed", self._part_common_props_changed_cb)

        self.title = "part properties"

        self._effects = ['NONE', 'PLAIN', 'OUTLINE', 'SOFT OUTLINE', 'SHADOW',
                         'SOFT SHADOW', 'OUTLINE SHADOW',
                         'OUTLINE SOFT SHADOW', 'FAR SHADOW',
                         'FAR SOFT SHADOW', 'GLOW']

        self._header_table = PropertyTable(
            parent, "part name/type", self.header_prop_value_changed)

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "type")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("t", wid)
        self._header_table.property_add(prop)

        self._source_prop = Property(parent, "source widget")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        self._source_prop.widget_add("s", wid)
        self._source_prop.hide()

        self._module_prop = Property(parent, "module")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        self._module_prop.widget_add("m", wid)
        self._module_prop.hide()

        self.content_set("part_name.swallow", self._header_table)

        def parts_get():
            return self.e.parts

        def sel_part_get():
            return self.e.part.name

        prop = Property(parent, "clip_to")
        prop.widget_add("to", WidgetPartList(
                self, "Clipper selection", parts_get, sel_part_get))
        self["main"].property_add(prop)
        prop = Property(parent, "mouse_events")
        prop.widget_add("me", WidgetBoolean(self))
        self["main"].property_add(prop)
        prop = Property(parent, "repeat_events")
        prop.widget_add("re", WidgetBoolean(self))
        self["main"].property_add(prop)
        # Missing properties: ignore_flags, scale, pointer_mode,
        # precise_is_inside

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

        self.open_disable = False
        self.open = True

    def _closed_cb(self, obj, emission, source):
        if not self.e.part.name:
            return

        expanded = self.e.part.type == edje.EDJE_PART_TYPE_EXTERNAL
        self.min_size_expanded_toggle(expanded)
        EditjeDetails._closed_cb(self, obj, emission, source)

    def _part_select(self, part_name):
        if self.e.part.name != part_name:
            self.e.part.name = part_name

    def header_prop_value_changed(self, prop_name, prop_value, group_name):

        def part_rename(old_name, new_name):
            # select 1st
            self._part_select(old_name)

            # rename later
            return self.e.part.rename(new_name)

        if prop_name != "name":
            return

        old_name = self.e.part.name
        if part_rename(old_name, prop_value):
            op = Operation("part renaming")

            op.redo_callback_add(part_rename, old_name, prop_value)
            op.undo_callback_add(part_rename, prop_value, old_name)
            self._operation_stack_cb(op)
        else:
            # TODO: notify the user of renaming failure
            self._header_table["name"].value = old_name

    def prop_value_changed(self, prop_name, prop_value, group_name):
        if group_name == "main":
            self._prop_value_common_changed(prop_name, prop_value)
        elif group_name == "textblock":
            self._prop_value_text_changed(prop_name, prop_value)

    def _prop_change_do(self, op_name, prop_group, prop_name, prop_value,
                        pval_filter=None):

        def set_property(part_name, prop_name, prop_value, filter_):
            self._part_select(part_name)
            part = self.e.part_get(part_name)
            setattr(part, prop_name, prop_value)

            if filter_:
                label_value = filter_(prop_value)
            else:
                label_value = prop_value
            if self[prop_group][prop_name].value != label_value:
                self[prop_group][prop_name].value = label_value

        part_name = self.e.part.name
        part = self.e.part_get(part_name)
        old_value = getattr(part, prop_name)

        set_property(part_name, prop_name, prop_value, pval_filter)

        op = Operation(op_name)
        op.redo_callback_add(
            set_property, part_name, prop_name, prop_value, pval_filter)
        op.undo_callback_add(
            set_property, part_name, prop_name, old_value, pval_filter)

        self._operation_stack_cb(op)

    def _prop_value_common_changed(self, prop, value):
        if prop == "mouse_events":
            self._prop_change_do(
                "part reaction to mouse events setting", "main", prop, value)
        elif prop == "repeat_events":
            self._prop_change_do(
                "part events repeating property setting", "main", prop, value)
        elif prop == "clip_to":
            self._prop_change_do(
                "part clipper setting", "main", prop, value)

    def _prop_value_text_changed(self, prop, value):
        if prop != "effect":
            return

        self._prop_change_do(
            "text part effects setting", "textblock", prop,
            self._effects.index(value), lambda x: self._effects[x])

    def _part_update(self, emissor, data):
        if not self.e.part.name:
            return

        self._header_table["name"].value = self.e.part.name
        self._header_table["name"].show_value()
        type = self._part_type_to_text(self.e.part.type)
        self._header_table["type"].value = type
        self._header_table["type"].show_value()
        self._update_common_props()

        self.group_hide("textblock")

        if self.e.part.type == edje.EDJE_PART_TYPE_EXTERNAL:
            source = self.e.part.source
            if source.startswith("elm/"):
                source = source[4:]
                module = "Elementary"
            else:
                module = "Emotion"

            self.edje_get().signal_emit("cl,extra,activate", "")
            if not self._header_table.has_key("source widget"):
                self._header_table.property_add(self._source_prop)
            if not self._header_table.has_key("module"):
                self._header_table.property_add(self._module_prop)

            self._header_table["source widget"].value = source
            self._header_table["source widget"].show_value()
            self._header_table["module"].value = module
            self._header_table["module"].show_value()
        else:
            self._header_extra_hide()

        if self.e.part.type == edje.EDJE_PART_TYPE_TEXT:
            self._update_text_props()

        self.show()

    def _header_extra_hide(self):
        self.edje_get().signal_emit("cl,extra,deactivate", "")
        for p in ["source widget", "module"]:
            if self._header_table.has_key(p):
                self._header_table.property_del(p)

    def _part_removed(self, emissor, data):
        self._header_table["name"].value = None
        self._header_table["name"].hide_value()
        self._header_table["type"].value = None
        self._header_table["type"].hide_value()

        self._header_extra_hide()
        self.main_hide()
        self.group_hide("textblock")

        self["main"]["clip_to"].hide_value()
        self["main"]["mouse_events"].hide_value()
        self["main"]["repeat_events"].hide_value()
        if self.e.part.type == edje.EDJE_PART_TYPE_TEXT:
            self["textblock"]["effect"].hide_value()

        self.hide()

    def _part_common_props_changed_cb(self, emissor, data):
        self._update_common_props()

    def _update_common_props(self):
        self.main_hide()

        clipper = self.e.part._part.clip_to
        self["main"]["clip_to"].show_value()
        if clipper:
            self["main"]["clip_to"].value = clipper
        else:
            self["main"]["clip_to"].value = ""

        self["main"]["mouse_events"].value = self.e.part._part.mouse_events
        self["main"]["mouse_events"].show_value()

        self["main"]["repeat_events"].value = self.e.part._part.repeat_events
        self["main"]["repeat_events"].show_value()

        self.main_show()

    def _update_text_props(self):
        self["textblock"]["effect"].value = \
            self._effects[self.e.part._part.effect]
        self["textblock"]["effect"].show_value()
        self.group_show("textblock")

    def _part_type_to_text(self, type):
        parttypes = ['NONE', 'RECTANGLE', 'TEXT', 'IMAGE', 'SWALLOW',
                     'TEXTBLOCK', 'GRADIENT', 'GROUP', 'BOX', 'TABLE',
                     'EXTERNAL']
        return parttypes[type]
