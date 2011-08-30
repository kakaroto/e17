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
from evas import EVAS_HINT_EXPAND
from elementary import ELM_NOTIFY_ORIENT_BOTTOM, Notify, Box, Label

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_boolean import WidgetBoolean
from details_widget_combo import WidgetCombo
from details_widget_button_list import WidgetButtonList
from prop import Property, PropertyTable
from operation import Operation
from misc import part_type_to_text


class PartDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(
            self, parent, operation_stack_cb,
            group="editje/collapsable/part_properties")

        self.mainwidow = parent
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

        self.content_set("part_name.swallow", self._header_table)
        self._header_table.show()

        self.focus_custom_chain_set([self._header_table, self._box])

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.tooltip_set("Unique name of part.")
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "type")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("Static type of part.")
        prop.widget_add("t", wid)
        self._header_table.property_add(prop)

        self._source_prop = Property(parent, "source widget")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("External Widget Type.")
        self._source_prop.widget_add("s", wid)
        self._source_prop.hide()
        self._header_table.property_add(self._source_prop)

        self._module_prop = Property(parent, "module")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("External Widget Module.")
        self._module_prop.widget_add("m", wid)
        self._module_prop.hide()
        self._header_table.property_add(self._module_prop)

        self.__notification = None

        def parts_get():
            return self.e.parts

        # Assuming that we are not opening files that already
        # has cycle this might finish
        def has_cycle(source, destination):
            next = self.e.part_get(destination)
            while next:
                if not next.clip_to:
                    return False
                elif next.clip_to == source:
                    return True
                next = self.e.part_get(next.clip_to)
            return False

        def clippers_get():
            clippers = []
            for p in self.e.parts:
                if not has_cycle(self.e.part.name, p):
                    clippers.append(p)
            return clippers

        def sel_part_get():
            return self.e.part.name

        popup_hide_cb_list = [(self.e.part, "part.unselected"),
                              (self.e.part, "part.changed")]

        prop = Property(parent, "clip_to")
        wid = WidgetButtonList(self, "Clipper selection", clippers_get,
                               sel_part_get, popup_hide_cb_list)
        wid.tooltip_set("Reference clipper part.")
        prop.widget_add("to", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "mouse_events")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Enable mouse events<br>in this part.")
        prop.widget_add("me", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "repeat_events")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Enable repeat mouse events<br>to the parts below.")
        prop.widget_add("re", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "scale")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Scalable part.")
        prop.widget_add("s", wid)
        self["main"].property_add(prop)
        # Missing properties: ignore_flags, pointer_mode,
        # precise_is_inside

        # textblock only (source is for group too, but later)
        self.group_add("textblock")
        prop = Property(parent, "effect")
        wid = WidgetCombo(parent)
        for i in range(len(self._effects)):
            wid.item_add(self._effects[i])
        wid.tooltip_set("Select text effect.<br>"
                        "Shadow and Outline options.")
        prop.widget_add("e", wid)
        self["textblock"].property_add(prop)
        # Missing properties: sources of text block, entry_mode,
        # select_mode, multiline
        self.group_add("group")

        def groups_get():
            return self.e.groups

        def sel_group_get():
            return self.e.group

        prop = Property(parent, "source")
        prop.widget_add("src", WidgetButtonList(
                self, "Source group selection", groups_get, sel_group_get,
                popup_hide_cb_list))
        self["group"].property_add(prop)

        self.group_add("api")
        prop = Property(parent, "export")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Add this part to exported parts.")
        prop.widget_add("export", wid)
        self["api"].property_add(prop)
        self._prop_api_name = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.tooltip_set("Export name of part.")
        self._prop_api_name.widget_add("name", wid)
        self["api"].property_add(self._prop_api_name)
        self._prop_api_description = Property(parent, "description")
        wid = WidgetEntry(self)
        wid.tooltip_set("Description of exported part.")
        self._prop_api_description.widget_add("description", wid)
        self["api"].property_add(self._prop_api_description)
        self.group_hide("api")

        self.main_hide()
        self.group_hide("textblock")
        self.group_hide("group")

        self.open_disable = False
        self.open = True

    # no sense in switching states to part properties
    def _context_recall(self, **kargs):
        self.e.part.name = kargs["part"]

    def _prop_object_get(self):
        return self.e.part_get(self.e.part.name)

    def _closed_cb(self, obj, emission, source):
        if not self.e.part.name:
            return

        expanded = self.e.part.type == edje.EDJE_PART_TYPE_EXTERNAL
        self.min_size_expanded_toggle(expanded)
        EditjeDetails._closed_cb(self, obj, emission, source)

    def header_prop_value_changed(self, prop_name, prop_value, group_name):

        def part_rename(old_name, new_name):
            # select 1st
            self._context_recall(part=old_name)

            # rename later
            try:
                self.e.part.rename(new_name)
            except Exception, e:
                notification = self.notify(str(e))
                return False
            return True

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
        elif group_name == "group":
            self._prop_value_group_changed(prop_name, prop_value)
        elif group_name == "api":
            self._prop_value_api_changed(prop_name, prop_value)

    def _prop_value_common_changed(self, prop, value):
        args = [["main"], [prop], [value], [None], [False], [None]]
        if prop == "mouse_events":
            self._prop_change_do(
                "part reaction to mouse events setting", *args)
        elif prop == "repeat_events":
            self._prop_change_do(
                "part events repeating property setting", *args)
        elif prop == "clip_to":
            self._prop_change_do("part clipper setting", *args)
        elif prop == "scale":
            self._prop_change_do("part scale setting", *args)

    def _prop_value_text_changed(self, prop, value):
        if prop != "effect":
            return

        args = [["textblock"], [prop], [self._effects.index(value)],
                [None], [False], [lambda x: self._effects[x]]]
        self._prop_change_do("text part effects setting", *args)

    def _prop_value_group_changed(self, prop, value):
        if prop != "source":
            return

        args = [["group"], [prop], [value], [None], [False], [None]]
        self._prop_change_do("part group source setting", *args)

    def _prop_value_api_changed(self, prop, value):
        if prop == "name":

            def api_name_change(value, part_name):
                self._context_recall(part=part_name)
                self.e.part.api = value
                self["api"]["name"].value = value[0]

            val = [value, self.e.part.api[1]]
            old_val = [self.e.part.api[0], val[1]]

            op = Operation("part's api name change")
            op.redo_callback_add(api_name_change, val, self.e.part.name)
            op.undo_callback_add(api_name_change, old_val, self.e.part.name)
            self._operation_stack_cb(op)
            op.redo()

        if prop == "description":

            def api_description_change(value, part_name):
                self._context_recall(part=part_name)
                self.e.part.api = value
                self["api"]["description"].value = value[1]

            val = [self.e.part.api[0], value]
            old_val = [val[0], self.e.part.api[1]]

            op = Operation("part's api description change")
            op.redo_callback_add(api_description_change, val, self.e.part.name)
            op.undo_callback_add(api_description_change,
                                 old_val, self.e.part.name)
            self._operation_stack_cb(op)
            op.redo()

        if prop == "export":

            def api_export_change(value, part_name, export):
                self._context_recall(part=part_name)
                self.e.part.api_export = export
                self.e.part.api = value
                self._update_api_props(export, value)

            export = value
            old_export = self.e.part.api_export
            old_val = [self.e.part.api[0], self.e.part.api[1]]
            val = [None, None]

            op = Operation("part's api description change")
            op.redo_callback_add(api_export_change, val, self.e.part.name,
                                 export)
            op.undo_callback_add(api_export_change, old_val, self.e.part.name,
                                 old_export)
            self._operation_stack_cb(op)
            op.redo()

    def _part_update(self, emissor, data):
        if not self.e.part.name:
            return

        self._header_table["name"].value = self.e.part.name
        self._header_table["name"].show_value()
        type_ = part_type_to_text(self.e.part.type)
        self._header_table["type"].value = type_
        self._header_table["type"].show_value()
        self._update_common_props()

        if self["api"]["export"].value:
            self["api"]["name"].value = self.e.part.api[0]
            self["api"]["description"].value = self.e.part.api[1]

        self.group_hide("textblock")
        self.group_hide("group")

        if self.e.part.type == edje.EDJE_PART_TYPE_EXTERNAL:
            source = self.e.part.source
            if source.startswith("elm/"):
                source = source[4:]
                module = "Elementary"
            else:
                module = "Emotion"

            self.edje_get().signal_emit("cl,extra,activate", "")
            if "source widget" not in self._header_table:
                self._header_table.property_add(self._source_prop)
            if "module" not in self._header_table:
                self._header_table.property_add(self._module_prop)

            self._header_table["source widget"].value = source
            self._header_table["source widget"].show_value()
            self._header_table["module"].value = module
            self._header_table["module"].show_value()
        else:
            self._header_extra_hide()

        self._update_api_props(self.e.part.api_export, self.e.part.api)
        self.group_show("api")

        if self.e.part.type == edje.EDJE_PART_TYPE_TEXT:
            self._update_text_props()

        if self.e.part.type == edje.EDJE_PART_TYPE_GROUP:
            self._update_group_props()

        self.show()

    def _header_extra_hide(self):
        self.edje_get().signal_emit("cl,extra,deactivate", "")
        for p in ["source widget", "module"]:
            if p in self._header_table:
                self._header_table.property_del(p)

    def _part_removed(self, emissor, data):
        self._header_table["name"].value = None
        self._header_table["name"].hide_value()
        self._header_table["type"].value = None
        self._header_table["type"].hide_value()

        self._header_extra_hide()
        self.main_hide()
        self.group_hide("textblock")
        self.group_hide("group")

        self.group_hide("api")

        self["main"]["clip_to"].hide_value()
        self["main"]["mouse_events"].hide_value()
        self["main"]["repeat_events"].hide_value()
        if self.e.part.type == edje.EDJE_PART_TYPE_TEXT:
            self["textblock"]["effect"].hide_value()
            self["textblock"]["effect"].value_obj._values_dict["e"].hover_end()
        elif self.e.part.type == edje.EDJE_PART_TYPE_GROUP:
            self["group"]["source"].hide_value()

        self.hide()

    def _part_common_props_changed_cb(self, emissor, data):
        self._update_common_props()

    def _update_common_props(self):
        self.main_hide()
        self.group_hide("api")

        clipper = self.e.part._part.clip_to
        self["main"]["clip_to"].show_value()
        if clipper:
            self["main"]["clip_to"].value = clipper
        else:
            self["main"]["clip_to"].value = None

        self["main"]["mouse_events"].value = self.e.part._part.mouse_events
        self["main"]["mouse_events"].show_value()

        self["main"]["repeat_events"].value = self.e.part._part.repeat_events
        self["main"]["repeat_events"].show_value()

        self["main"]["scale"].value = self.e.part._part.scale
        self["main"]["scale"].show_value()

        self.main_show()

    def _update_api_props(self, export, api):
        self["api"]["export"].value = export
        if self["api"]["export"].value:
            if "name" not in self["api"]:
                self["api"].property_add(self._prop_api_name)
                self["api"].property_add(self._prop_api_description)
            self["api"]["name"].value = api[0]
            self["api"]["description"].value = api[1]
        elif "name" in self["api"]:
            self["api"].property_del("name")
            self["api"].property_del("description")

    def _update_text_props(self):
        self["textblock"]["effect"].value = \
            self._effects[self.e.part._part.effect]
        self["textblock"]["effect"].show_value()
        self.group_show("textblock")

    def _update_group_props(self):
        source = self.e.part._part.source
        if source:
            self["group"]["source"].value = source
        else:
            self["group"]["source"].value = None
            self["group"]["source"].show_value()
        self.group_show("group")

    def notify(self, message):
        if self.__notification:
            self.__notification.hide()
            self.__notification.delete()
            self.__notification = None

        self.__notification = Notify(self.mainwidow)
        self.__notification.timeout_set(1)
        self.__notification.orient_set(ELM_NOTIFY_ORIENT_BOTTOM)

        bx = Box(self)
        bx.size_hint_weight_set(EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self.__notification.content_set(bx)
        bx.show()

        lb = Label(self)
        lb.text_set(message)
        bx.pack_end(lb)
        lb.show()

        self.__notification.show()
