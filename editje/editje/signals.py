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
import ecore
import edje
import re

import sysconfig
from editable_animation import re_anim_program
from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_signals import WidgetSignal
from details_widget_source import WidgetSource
from details_widget_actionslist import WidgetActionsList
from details_widget_boolean import WidgetBoolean
from floater import Wizard
from clist import CList
from prop import Property, PropertyTable
from groupselector import NameEntry
from operation import Operation
from elementary import Notify, ELM_NOTIFY_ORIENT_BOTTOM, Box, Label
import objects_data


class SignalsList(CList):
    def __init__(self, parent, editable_grp, new_sig_cb, sigs_list_cb,
                 operation_stack_cb):
        CList.__init__(self, parent)
        self._edit_grp = editable_grp

        self._new_sig_cb = new_sig_cb
        self._sigs_list_cb = sigs_list_cb
        self._operation_stack_cb = operation_stack_cb

        self._options_load()
        self.options = True

        self._edit_grp.callback_add("signals.changed", self._signals_update)
        self._edit_grp.callback_add("signal.added", self._signal_added)
        self._edit_grp.callback_add("signal.removed", self._signal_removed)

        self._edit_grp.signal.callback_add(
            "program.changed", self._signal_changed)
        self._edit_grp.signal.callback_add(
            "program.unselected", self._signal_changed)
        self._edit_grp.signal.callback_add(
            "program.name.changed", self._name_changed_cb)

    def _signals_update(self, emissor, data):
        self.clear()
        for i in data:
            self.add(i)
        self.go()

    def _signal_added(self, emissor, data):
        self.add(data)
        self.go()
        self.open = True
        self.select(data)

    def _signal_removed(self, emissor, data):
        self.remove(data)

    def _signal_changed(self, emissor, data):
        self.selection_clear()
        self.select(data)

    # Selection
    def _selected_cb(self, li, it):
        CList._selected_cb(self, li, it)
        name = it.label_get()
        self._edit_grp.signal.name = name
        self._options_edje.signal_emit("remove,enable", "")

    def _unselected_cb(self, li, it):
        CList._unselected_cb(self, li, it)
        if not self._selected:
            self._options_edje.signal_emit("remove,disable", "")

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(self.edje_get().evas,
                               file=self._theme_file,
                               group="editje/collapsable/list/options/signals")
        self._options_edje.signal_callback_add(
            "new", "editje/collapsable/list/options", self._new_cb)
        self._options_edje.signal_callback_add(
            "remove", "editje/collapsable/list/options", self._remove_cb)
        self._options_edje.signal_emit("remove,disable", "")
        self.content_set("options", self._options_edje)
        self._options = False

    def _name_changed_cb(self, emissor, data):
        old_name, new_name = data

        for s in self._selected.iterkeys():
            item = self._items[s]
            if item.label_get() == old_name:
                item.text_set(new_name)
                self._selected[new_name] = self._selected[old_name]
                self._items[new_name] = self._items[old_name]
                del self._selected[old_name]
                del self._items[old_name]
                return

    def _new_cb(self, obj, emission, source):
        sig_wiz = NewSignalWizard(
            self._parent, new_sig_cb=self._new_sig_cb,
            sigs_list_cb=self._sigs_list_cb)
        sig_wiz.open()

    def _remove_cb(self, obj, emission, source):
        for to_del in self.selected:
            sig_name = to_del[0]
            sig_save = objects_data.Program(
                self._edit_grp.edje.program_get(sig_name))

            r = self._edit_grp.signal_del(sig_name)
            if not r:
                del sig_save
                continue

            op = Operation("signal deletion")
            op.redo_callback_add(self._edit_grp.signal_del, sig_name)
            op.undo_callback_add(self._edit_grp.signal_add_bydata, sig_save)
            self._operation_stack_cb(op)


class SignalTypesButtons(edje.Edje):
    def __init__(self, parent, type_select_cb=None):
        edje.Edje.__init__(self, parent.evas)
        self._type_select_cb = type_select_cb

        theme_file = sysconfig.theme_file_get("default")

        self._file_set(theme_file, "editje/list/signal_type_buttons")
        self._labels_set()

        self.on_mouse_down_add(self._mouse_down_cb)

    def _file_set(self, file_, group):
        edje.Edje.file_set(self, file_, group)

        self.signal_callback_add(
            "animation_sig,selected", "editje/signal_type_buttons",
            self._sig_cb)
        self.signal_callback_add(
            "general_sig,selected", "editje/signal_type_buttons", self._sig_cb)

    def _labels_set(self):
        self.part_text_set(
            "button_01.label", "Animation triggering signal")
        self.part_text_set(
            "button_01.sublabel", "Choose it if you want a signal"
            " coming from an UI element interaction to trigger an animation.")

        self.part_text_set(
            "button_02.label", "General purpose signal")
        self.part_text_set(
            "button_02.sublabel", "Choose it if you "
            "want a signal coming from an UI element interaction"
            " to yield another custom signal.")

    def _sig_cb(self, obj, emission, source):
        if emission == "animation_sig,selected":
            self._type_select_cb(edje.EDJE_ACTION_TYPE_NONE)
        elif emission == "general_sig,selected":
            self._type_select_cb(edje.EDJE_ACTION_TYPE_SIGNAL_EMIT)

    def _mouse_down_cb(self, obj, event):
        self.focus_set(True)


class NewSignalWizard(Wizard):
    def __init__(self, parent, new_sig_cb=None, sigs_list_cb=None):
        if not new_sig_cb or not sigs_list_cb:
            raise TypeError("You must set callbacks for signals retrieval and"
                            " creation on NewSignalWizard objects.")

        Wizard.__init__(self, parent)
        self._type = None

        self.page_add("default", "New Signal",
                      "Name the new signal to be created and choose its type.")

        self._sig_name_entry = NameEntry(
            self, changed_cb=self._name_changed_cb,
            weight_hints=(evas.EVAS_HINT_EXPAND, 0.0),
            align_hints=(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL))
        self.content_add("default", self._sig_name_entry)
        self._sig_name_entry.show()

        self._types_btns = SignalTypesButtons(self, self._type_select)
        self._types_btns.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                              evas.EVAS_HINT_EXPAND)
        self._types_btns.size_hint_align_set(evas.EVAS_HINT_FILL,
                                             evas.EVAS_HINT_FILL)
        self.content_add("default", self._types_btns)
        self._types_btns.show()

        self.action_add("default", "Cancel", self._cancel, key="Escape")
        self.action_add("default", "Add", self._add, key="Return")
        self.action_disabled_set("default", "Add", True)

        self._new_sig_cb = new_sig_cb
        self._sigs_list_cb = sigs_list_cb

    def _name_changed_cb(self, obj):
        self._check_name_and_type()

    def _type_select(self, type_):
        self._type = type_
        self._check_name_and_type()

    def _check_name_and_type(self):
        error_msg = "This signal name is already used in this group"

        def good():
            self._sig_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", False)

        def bad():
            self._sig_name_entry.status_label = error_msg
            self.action_disabled_set("default", "Add", True)

        def incomplete():
            self._sig_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", True)

        name = self._sig_name_entry.entry
        if name in self._sigs_list_cb():
            bad()
            return

        if not name or self._type is None:
            incomplete()
            return

        good()

    def _add(self):
        name = self._sig_name_entry.entry

        success = self._new_sig_cb(name, self._type)
        if success:
            ecore.idler_add(self.close)
        else:
            self.notify("Error creating new signal.")

    def _cancel(self):
        self.close()


class SignalDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(
            self, parent, operation_stack_cb,
            group="editje/collapsable/part_state")

        self.title = "signal"

        self._parent = parent
        self._actions_added = 0

        self._header_table = PropertyTable(
            parent, "signal name", self._header_prop_value_changed)
        self.content_set("part_state.swallow", self._header_table)
        self.focus_custom_chain_set([self._header_table, self._box])

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.tooltip_set("Unique signal name.")
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        popup_hide_cb_list = [(self.e.signal, "program.unselected"),
                              (self.e.signal, "program.changed")]

        prop = Property(parent, "signal")
        wid = WidgetSignal(self,
                           popup_hide_object_signal_list=popup_hide_cb_list)
        wid.tooltip_set("Signal to be received.", "Click to select one common"
                        "<br>signal to be received.")
        prop.widget_add("s", wid)
        self["main"].property_add(prop)

        def parts_get():
            return self.e.parts

        def animations_get():
            return self.e.animations

        self._prop_source_animations = Property(parent, "source")
        wid = WidgetSource(self, "Animation list",
                           animations_get, popup_hide_cb_list)
        wid.tooltip_set("Accepted signal source.", "Click to select one "
                        "existent<br>animation as source.")
        self._prop_source_animations.widget_add("s", wid)
        self["main"].property_add(self._prop_source_animations)
        self["main"].property_del("source")

        self._prop_source_parts = Property(parent, "source")
        wid = WidgetSource(self, "Parts list", parts_get, popup_hide_cb_list)
        wid.tooltip_set("Accepted signal source.", "Click to select one "
                        "existent<br>part as source.")
        self._prop_source_parts.widget_add("s", wid)
        self["main"].property_add(self._prop_source_parts)

        prop = Property(parent, "delay")
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Fixed delay to do the action in seconds. The<br>total "
                        "delay is sum off fixed and variable delays.")
        prop.widget_add("delay", wid)
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Variable delay to do the action in seconds. Will<br>be"
                        " selected one random value between 0 and this.")
        prop.widget_add("range", wid)
        self["main"].property_add(prop)

        self.group_add("api")
        prop = Property(parent, "export")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Add this part to exported signals.")
        prop.widget_add("export", wid)
        self["api"].property_add(prop)
        self._prop_api_name = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.tooltip_set("Export name of signal.")
        self._prop_api_name.widget_add("name", wid)
        self["api"].property_add(self._prop_api_name)
        self._prop_api_description = Property(parent, "description")
        wid = WidgetEntry(self)
        wid.tooltip_set("Description of exported signal.")
        self._prop_api_description.widget_add("description", wid)
        self["api"].property_add(self._prop_api_description)

        self.group_add("actions")

        self.group_add("out")

        prop = Property(parent, "signal")
        wid = WidgetSignal(self,
                           popup_hide_object_signal_list=popup_hide_cb_list)
        wid.tooltip_set("Signal to be emmited.", "Click to select one common "
                        "signal to be emmited.")
        prop.widget_add("s", wid)
        self["out"].property_add(prop)

        prop = Property(parent, "source")
        wid = WidgetSource(self, "Parts list", parts_get, popup_hide_cb_list)
        wid.tooltip_set("Name registered as signal source.", "Click to select "
                        "one existent<br>part as source.")
        prop.widget_add("s", wid)
        self["out"].property_add(prop)

        self.e.callback_add("signal.added", self._update)
        self.e.callback_add("signal.removed", self._removed)
        self.e.callback_add("group.changed", self._removed)
        self.e.signal.callback_add("program.changed", self._update)
        self.e.signal.callback_add("program.name.changed", self._update)
        self.e.signal.callback_add("program.unselected", self._removed)

        self.open = True
        self.open_disable = True
        self.show()
        self.__notification = None

    def _header_prop_value_changed(self, prop_name, prop_value, group_name):

        def signal_rename(old_name, new_name):
            # select 1st
            self._context_recall(signal=old_name)

            # rename later
            return self.e.signal.rename(new_name)

        if prop_name != "name":
            return

        old_name = self.e.signal.name
        if signal_rename(old_name, prop_value):
            op = Operation("signal renaming")

            op.redo_callback_add(signal_rename, old_name, prop_value)
            op.undo_callback_add(signal_rename, prop_value, old_name)
            self._operation_stack_cb(op)
        else:
            # TODO: notify the user of renaming failure
            self._header_table["name"].value = old_name

    def _new_action(self, number):

        def programs_get():
            return self.e.programs

        popup_hide_cb_list = [(self.e.signal, "program.unselected"),
                              (self.e.signal, "program.changed")]

        prop = Property(self._parent, "action " + str(number))
        wid = WidgetActionsList(self, "Animations", programs_get, None,
                                popup_hide_cb_list)
        wid.tooltip_set("Animation to be started.<br>Click to select it.")
        prop.widget_add("a", wid)
        self["actions"].property_add(prop)

    def _removed(self, emissor, data):
        self._header_table["name"].value = "Unselected"
        self._header_table["name"].value_obj._values_dict["n"].disabled_set(
            True)

        self["main"]["signal"].value = ""
        self["main"]["signal"].hide_value()

        self["main"]["source"].value = ""
        self["main"]["source"].hide_value()

        self["main"]["delay"].value = (None, None)
        self["main"]["delay"].hide_value()

        self.group_hide("api")
        self.group_show("api")

        if "name" in self["api"]:
            self["api"]["name"].value = None
            self["api"]["description"].value = None

        self.group_hide("actions")
        self.group_show("actions")

        for i in range(self._actions_added):
            self["actions"]["action " + str(i + 1)].value = ""
            self["actions"]["action " + str(i + 1)].hide_value()

        self.group_hide("out")
        self.group_show("out")

        self["out"]["signal"].value = ""
        self["out"]["signal"].hide_value()

        self["out"]["source"].value = ""
        self["out"]["source"].hide_value()

        self.open_set(False)

    def _update(self, emissor, data):
        self._emissor = emissor
        self._data = data

        self._header_table["name"].value = self.e.signal.name
        self._header_table["name"].show_value()
        self._header_table["name"].value_obj._values_dict["n"].disabled_set(
            False)

        signal = self.e.signal.signal
        self["main"]["signal"].show_value()
        if signal:
            self["main"]["signal"].value = signal
        else:
            self.e.signal.signal = ""
            self["main"]["signal"].value = ""

        source = self.e.signal.source
        self["main"]["source"].show_value()
        if source:
            self["main"]["source"].value = source
        else:
            self.e.signal.source = ""
            self["main"]["source"].value = ""

        self["main"]["delay"].show_value()
        self["main"]["delay"].value = self.e.signal.in_time

        action = self.e.signal.action

        self._update_api_props(self.e.signal.api_export, self.e.signal.api)
        self.group_hide("api")
        self.group_show("api")

        if action == edje.EDJE_ACTION_TYPE_NONE:
            afters = self.e.signal.afters

            while (self._actions_added < len(afters) + 1):
                self._actions_added = self._actions_added + 1
                self._new_action(self._actions_added)

            for i in range(self._actions_added):
                self["actions"]["action " + str(i + 1)].hide()

            for i in range(len(afters)):
                self["actions"]["action " + str(i + 1)].show_value()
                self["actions"]["action " + str(i + 1)].show()
                fixedname = re_anim_program.match(afters[i]).groups()[0]
                self["actions"]["action " + str(i + 1)].value = fixedname
            # The last action with none value
            self["actions"]["action " + str(len(afters) + 1)].show_value()
            self["actions"]["action " + str(len(afters) + 1)].show()
            self["actions"]["action " + str(len(afters) + 1)].value = ""

            self.group_hide("out")
            self.group_hide("actions")
            self.group_show("actions")

        elif action == edje.EDJE_ACTION_TYPE_SIGNAL_EMIT:
            self.group_hide("actions")
            self.group_hide("out")
            self.group_show("out")

            state = self.e.signal.state_get()
            self["out"]["signal"].show_value()
            if state:
                self["out"]["signal"].value = state
            else:
                self["out"]["signal"].value = ""

            state = self.e.signal._program.state2_get()
            self["out"]["source"].show_value()
            if state:
                self["out"]["source"].value = state
            else:
                self["out"]["source"].value = ""

        self.open_set(True)
        self.show()

    def _context_recall(self, **kargs):
        self.e.signal.name = kargs["signal"]

    def _prop_object_get(self):
        return self.e.signal

    def _notify(self, message):
        if self.__notification:
            self.__notification.hide()
            self.__notification.delete()
            self.__notification = None
        self.__notification = Notify(self._parent)
        self.__notification.timeout_set(4)
        self.__notification.orient_set(ELM_NOTIFY_ORIENT_BOTTOM)

        bx = Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self.__notification.content_set(bx)
        bx.show()

        lb = Label(self)
        lb.text_set(message)
        bx.pack_end(lb)
        lb.show()

        self.__notification.show()

    def _conflicting_animation_get(self, afters_list, anim):
        if not afters_list or not anim:
            return None
        anim_targets_list = self.e.edje.program_get(anim).targets_get()
        for after in afters_list:
            after_targets_list = self.e.edje.program_get(after).targets_get()
            for target in anim_targets_list:
                if target in after_targets_list:
                    return after
        return None

    def prop_value_changed(self, prop, value, group):
        if group == "main":
            if prop == "signal":
                args = [["main"], [prop], [value], [None], [False], [None]]
                self._prop_change_do(
                    "signal's triggering action change", *args)
                self["main"].property_del("source")
                if value == "animation,end":
                    self["main"].property_add(self._prop_source_animations, 1)
                else:
                    self["main"].property_add(self._prop_source_parts, 1)

            elif prop == "source":
                args = [["main"], [prop], [value], [None], [False], [None]]
                self._prop_change_do(
                    "signal's source part change", *args)

            elif prop == "delay":
                args = [["main"], [prop], [value], ["in_time"], [False],
                        [None]]
                self._prop_change_do(
                    "signal's source delay time change", *args)

        elif group == "actions":

            def afters_change(afters, sig_name):
                self._context_recall(signal=sig_name)

                self.e.signal.afters_clear()
                for i in range(len(afters)):
                    self.e.signal.after_add(afters[i])
                self._update(self._emissor, self._data)

            old_afters = self.e.signal._afters_get()
            new_afters = self.e.signal._afters_get()

            if value in old_afters:
                value = re_anim_program.match(value).groups()[0]
                self._notify("Error: " + value + " is already in the list")
                self._update(self._emissor, self._data)
                return

            action_number = int(re.match("action (\d+)", prop).group(1))

            if value:
                if action_number <= len(new_afters):
                    new_afters[action_number - 1] = value
                else:
                    new_afters.append(value)
            elif action_number <= len(new_afters):
                new_afters.pop(action_number - 1)

            conflicting_after = self._conflicting_animation_get(new_afters,
                                                                value)
            if conflicting_after and conflicting_after != value:
                conflicting_after = re_anim_program.match(
                                        conflicting_after).groups()[0]
                self._notify("Error: this animation conflicts with "
                             + conflicting_after + ".<br>"
                             "There are parts in common being animated.")
                self._update(self._emissor, self._data)
                return

            op = Operation("signal's \"after\" action change")
            op.redo_callback_add(afters_change, new_afters, self.e.signal.name)
            op.undo_callback_add(afters_change, old_afters, self.e.signal.name)
            self._operation_stack_cb(op)
            op.redo()

        elif group == "out":
            if prop == "signal":

                def signal_change(value, sig_name):
                    self._context_recall(signal=sig_name)

                    self.e.signal.signal_emit_action_set(value)
                    self["out"]["signal"].value = value[0]

                val = [value, self["out"]["source"].value]
                old_val = [self.e.signal.state_get() or "", val[1]]

                op = Operation("signal's \"out\" emission change")
                op.redo_callback_add(signal_change, val, self.e.signal.name)
                op.undo_callback_add(signal_change,
                                     old_val, self.e.signal.name)
                self._operation_stack_cb(op)
                op.redo()

            elif prop == "source":

                def source_change(value, sig_name):
                    self._context_recall(signal=sig_name)

                    self.e.signal.signal_emit_action_set(value)
                    self["out"]["source"].value = value[1]

                val = [self["out"]["signal"].value, value]
                old_val = [val[0], self.e.signal.state2_get() or ""]

                op = Operation("signal's \"out\" source change")
                op.redo_callback_add(source_change, val, self.e.signal.name)
                op.undo_callback_add(source_change,
                                     old_val, self.e.signal.name)
                self._operation_stack_cb(op)
                op.redo()

        elif group == "api":
            if prop == "name":

                def api_name_change(value, sig_name):
                    self._context_recall(signal=sig_name)
                    self.e.signal.api = value
                    self["api"]["name"].value = value[0]

                val = [value, self.e.signal.api[1]]
                old_val = [self.e.signal.api[0], val[1]]

                op = Operation("program's api name change")
                op.redo_callback_add(api_name_change, val, self.e.signal.name)
                op.undo_callback_add(api_name_change, old_val,
                                     self.e.signal.name)
                self._operation_stack_cb(op)
                op.redo()

            if prop == "description":

                def api_description_change(value, sig_name):
                    self._context_recall(signal=sig_name)
                    self.e.signal.api = value
                    self["api"]["description"].value = value[1]

                val = [self.e.signal.api[0], value]
                old_val = [val[0], self.e.signal.api[1]]

                op = Operation("program's api description change")
                op.redo_callback_add(api_description_change, val,
                                     self.e.signal.name)
                op.undo_callback_add(api_description_change,
                                         old_val, self.e.signal.name)
                self._operation_stack_cb(op)
                op.redo()

            if prop == "export":

                def api_export_change(value, sig_name, export):
                    self._context_recall(signal=sig_name)
                    self.e.signal.api_export = export
                    self.e.signal.api = value
                    self._update_api_props(export, value)

                export = value
                old_export = self.e.signal.api_export
                old_val = [self.e.signal.api[0], self.e.signal.api[1]]
                val = [None, None]

                op = Operation("program's api description change")
                op.redo_callback_add(api_export_change, val,
                                     self.e.signal.name, export)
                op.undo_callback_add(api_export_change, old_val,
                                     self.e.signal.name, old_export)
                self._operation_stack_cb(op)
                op.redo()

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
