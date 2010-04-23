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

import sysconfig
from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_signals import WidgetSignal
from details_widget_source import WidgetSource
from details_widget_actionslist import WidgetActionsList
from floater import Wizard
from clist import CList
from prop import Property, PropertyTable
from groupselector import NameEntry
from operation import Operation


class SignalsList(CList):
    def __init__(self, parent, new_sig_cb, sigs_list_cb):
        CList.__init__(self, parent)
        self.e = parent.e

        self._new_sig_cb = new_sig_cb
        self._sigs_list_cb = sigs_list_cb
        self._options_load()
        self.options = True

        self.e.callback_add("signals.changed", self._signals_update)
        self.e.callback_add("signal.added", self._signal_added)
        self.e.callback_add("signal.removed", self._signal_removed)

        self.e.signal.callback_add("program.changed", self._signal_changed)
        self.e.signal.callback_add("program.unselected", self._signal_changed)

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
        self.e.signal.name = name
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

    def _new_cb(self, obj, emission, source):
        sig_wiz = NewSignalWizard(
            self._parent, new_sig_cb=self._new_sig_cb,
            sigs_list_cb=self._sigs_list_cb)
        sig_wiz.open()

    def _remove_cb(self, obj, emission, source):
        for i in self.selected:
            self.e.signal_del(i[0])


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
        self.action_add("default", "Create", self._add, key="Return")
        self.action_disabled_set("default", "Create", True)

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
            self.action_disabled_set("default", "Create", False)

        def bad():
            self._sig_name_entry.status_label = error_msg
            self.action_disabled_set("default", "Create", True)

        def incomplete():
            self._sig_name_entry.status_label = ""
            self.action_disabled_set("default", "Create", True)

        name = self._sig_name_entry.entry
        if not name or self._type is None:
            incomplete()
            return

        if name in self._sigs_list_cb():
            bad()
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

        # FIXME: no signal renaming yet (thus, no prop_value_changed_cb and
        # wid.disable_set(True), for now
        self._header_table = PropertyTable(parent, "signal name")

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        self.content_set("part_state.swallow", self._header_table)

        prop = Property(parent, "signal")
        prop.widget_add("s", WidgetSignal(self))
        self["main"].property_add(prop)

        def parts_get():
            return self.e.parts

        prop = Property(parent, "source")
        prop.widget_add("s", WidgetSource(self, parts_get))
        self["main"].property_add(prop)

        prop = Property(parent, "delay")
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: float(x)
        prop.widget_add("delay", wid)
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: float(x)
        prop.widget_add("range", wid)
        self["main"].property_add(prop)

        def programs_get():
            return self.e.programs

        prop = Property(parent, "action")
        prop.widget_add(
            "a", WidgetActionsList(self, "Animations", programs_get))
        self["main"].property_add(prop)

        self.group_add("out")

        prop = Property(parent, "signal")
        prop.widget_add("s", WidgetSignal(self))
        self["out"].property_add(prop)

        prop = Property(parent, "source")
        prop.widget_add("s", WidgetSource(self))
        self["out"].property_add(prop)

        self.e.callback_add("signal.added", self._update)
        self.e.callback_add("signal.removed", self._removed)
        self.e.callback_add("group.changed", self._removed)
        self.e.signal.callback_add("program.changed", self._update)
        self.e.signal.callback_add("program.unselected", self._removed)

        self.open = True
        self.open_disable = True
        self.show()

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

        self["main"]["action"].value = ""
        self["main"]["action"].hide_value()

        self.group_hide("out")
        self.group_show("out")

        self["out"]["signal"].value = ""
        self["out"]["signal"].hide_value()

        self["out"]["source"].value = ""
        self["out"]["source"].hide_value()
        self.open_set(False)

    def _update(self, emissor, data):
        self._header_table["name"].value = data
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

        self["main"]["action"].show_value()
        if action == edje.EDJE_ACTION_TYPE_NONE:
            self["main"]["action"].show()
            self.group_hide("out")

            afters = self.e.signal.afters
            if afters:
                fixedname = afters[0][1:afters[0].rindex("@")]
                self["main"]["action"].value = fixedname
            else:
                self["main"]["action"].value = ""
        elif action == edje.EDJE_ACTION_TYPE_SIGNAL_EMIT:
            self["main"]["action"].hide()
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

    def prop_value_changed(self, prop, value, group):
        if group == "main":
            tbl = self["main"]

            if prop == "signal":
                args = [["main"], [prop], [value], [None], [False], [None]]
                self._prop_change_do(
                    "signal's triggering action change", *args)

            elif prop == "source":
                args = [["main"], [prop], [value], [None], [False], [None]]
                self._prop_change_do(
                    "signal's source part change", *args)

            elif prop == "action":

                def afters_change(value):
                    self.e.signal.afters_clear()
                    self.e.signal.after_add(value or "")
                    if value:
                        value = value[1:value.rindex("@")]
                    self["main"]["action"].value = value

                afters = self.e.signal.afters
                old_val = None
                if afters:
                    old_val = afters[0]

                op = Operation("signal's \"after\" action change")
                op.redo_callback_add(afters_change, value)
                op.undo_callback_add(afters_change, old_val)
                self._operation_stack_cb(op)
                op.redo()

            elif prop == "delay":
                args = [["main"], [prop], [value], ["in_time"], [False], [None]]
                self._prop_change_do(
                    "signal's source delay time change", *args)

        elif group == "out":
            if prop == "signal":
                def signal_change(value):
                    self.e.signal.signal_emit_action_set(val)
                    self["out"]["signal"].value = value[0]

                val = [value, self["out"]["source"].value]
                old_val = [self["out"]["signal"].value, val[1]]

                op = Operation("signal's \"out\" emission change")
                op.redo_callback_add(signal_change, val)
                op.undo_callback_add(signal_change, old_val)
                self._operation_stack_cb(op)
                op.redo()

            elif prop == "source":
                def source_change(value):
                    self.e.signal.signal_emit_action_set(val)
                    self["out"]["source"].value = value[1]

                val = [self["out"]["signal"].value, value]
                old_val = [val[0], self["out"]["signal"].value]

                op = Operation("signal's \"out\" emission change")
                op.redo_callback_add(source_change, val)
                op.undo_callback_add(source_change, old_val)
                self._operation_stack_cb(op)
                op.redo()

