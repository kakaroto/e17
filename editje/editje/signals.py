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

import evas
import ecore
import edje
import elementary

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


class SignalsList(CList):
    def __init__(self, parent, new_sig_cb):
        CList.__init__(self, parent)
        self.e = parent.e

        self._new_sig_cb = new_sig_cb
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
        self._options_edje.signal_callback_add("new",
                                "editje/collapsable/list/options",
                                self._new_cb)
        self._options_edje.signal_callback_add("remove",
                                "editje/collapsable/list/options",
                                self._remove_cb)
        self._options_edje.signal_emit("remove,disable", "")
        self.content_set("options", self._options_edje)
        self._options = False

    def _new_cb(self, obj, emission, source):
        sig_wiz = NewSignalWizard(
            self._parent, new_sig_cb=self._new_sig_cb)
        sig_wiz.open()

    def _remove_cb(self, obj, emission, source):
        for i in self.selected:
            self.e.signal_del(i[0])


class TypesList(elementary.List):
    def __init__(self, parent, type_select_cb=None):
        elementary.List.__init__(self, parent)
        self._parent = parent
        self._type_select_cb = type_select_cb

        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

        theme_file = sysconfig.theme_file_get("default")

        ico = elementary.Icon(parent)
        ico.file_set(theme_file, "editje/icon/animation")
        ico.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
        ico.show()
        self.item_append("Animation triggering signal", ico, None,
                         self._type_select_cb, edje.EDJE_ACTION_TYPE_NONE)
        ico = elementary.Icon(parent)
        ico.file_set(theme_file, "editje/icon/signal")
        ico.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
        ico.show()
        self.item_append("General purpose signal", ico, None,
                         self._type_select_cb,
                         edje.EDJE_ACTION_TYPE_SIGNAL_EMIT)
        # self.item_append("Script triggering signal", None, None,
        #                  self._type_select_cb, edje.EDJE_ACTION_TYPE_SCRIPT)

        self.go()


class NewSignalWizard(Wizard):
    def __init__(self, parent, new_sig_cb=None):
        if not new_sig_cb:
            raise TypeError("You must set a callback for new signals on" \
                            " NewSignalWizard objects.")

        Wizard.__init__(self, parent)

        self.page_add("default", "New Signal",
                      "Name the new signal to be created and choose its type.")

        self._sig_name_entry = NameEntry(
            self, changed_cb=self._name_changed_cb,
            weight_hints=(evas.EVAS_HINT_EXPAND, 0.0),
            align_hints=(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL))
        self.content_add("default", self._sig_name_entry)
        self._sig_name_entry.show()

        self._types_list = TypesList(self, self._type_select)
        self.content_add("default", self._types_list)
        self._types_list.show()

        self.action_add("default", "Cancel", self._cancel)
        self.action_add("default", "Create", self._add)
        self.action_disabled_set("Create", True)

        self._new_sig_cb = new_sig_cb
        self._type = None

    def _name_changed_cb(self, obj):
        self._check_name_and_type()

    def _type_select(self, list_, item, label, *args, **kwargs):
        self._type = label
        self._check_name_and_type()

    def _check_name_and_type(self):
        name = self._sig_name_entry.entry
        if self._type is not None and name != "":
            self.action_disabled_set("Create", False)
        else:
            self.action_disabled_set("Create", True)

    def _add(self):
        name = self._sig_name_entry.entry.replace("<br>", "")
        if name == "":
            self.notify("Please give a name to the new signal.")
            return

        success = self._new_sig_cb(name, self._type)
        if success:
            ecore.idler_add(self.close)
        else:
            self.notify("There is a signal with this name in the "
                        "group, already. Please choose another name.")

    def _cancel(self):
        self.close()


class SignalDetails(EditjeDetails):
    def __init__(self, parent):
        EditjeDetails.__init__(self, parent,
                               group="editje/collapsable/part_state")

        self.title_set("signal")

        self._header_table = PropertyTable(parent)

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        self.content_set("part_state.swallow", self._header_table)

        prop = Property(parent, "signal")
        prop.widget_add("s", WidgetSignal(self))
        self["main"].property_add(prop)

        prop = Property(parent, "source")
        prop.widget_add("s", WidgetSource(self))
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

        prop = Property(parent, "action")
        prop.widget_add("a", WidgetActionsList(self))
        self["main"].property_add(prop)

        self.group_add("out")

        prop = Property(parent, "signal")
        prop.widget_add("s", WidgetSignal(self))
        self["out"].property_add(prop)

        prop = Property(parent, "source")
        prop.widget_add("s", WidgetSource(self))
        self["out"].property_add(prop)

        self.e.signal.callback_add("program.changed",self._update)

    def _update(self, emissor, data):
        self._header_table["name"].value = data

        signal = self.e.signal.signal
        if signal:
            self["main"]["signal"].value = signal
        else:
            self.e.signal.signal = ""
            self["main"]["signal"].value = ""

        source = self.e.signal.source
        if source:
            self["main"]["source"].value = source
        else:
            self.e.signal.source = ""
            self["main"]["source"].value = ""

        self["main"]["delay"].value = self.e.signal.in_time

        action = self.e.signal._program.action_get()

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

            state = self.e.signal._program.state_get()
            if state:
                self["out"]["signal"].value = state
            else:
                self["out"]["signal"].value = ""

            state = self.e.signal._program.state2_get()
            if state:
                self["out"]["source"].value = state
            else:
                self["out"]["source"].value = ""

    def prop_value_changed(self, prop, value, group):
        if not group:
            tbl = self["main"]
            if prop == "signal":
                self.e.signal.signal = value
                tbl["signal"].value = value
            elif prop == "source":
                self.e.signal.source = value
                tbl["source"].value = value
            elif prop == "action":
                self.e.signal.afters_clear()
                self.e.signal.after_add(value)
                fixedname = value[1:value.rindex("@")]
                tbl["action"].value = fixedname
            elif prop == "delay":
                self.e.signal.in_time = value
                tbl["delay"].value = value
        elif group == "out":
            tbl = self["out"]
            if prop == "signal":
                self.e.signal._program.state_set(value)
                tbl["signal"].value = value
            elif prop == "source":
                self.e.signal._program.state2_set(value)
                tbl["source"].value = value

