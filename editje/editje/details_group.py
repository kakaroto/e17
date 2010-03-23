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

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from prop import Property
from operation import Operation


class GroupDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(self, parent, operation_stack_cb)

        self.title = "group"
        self.open_disable = True

        prop = Property(parent, "min")
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)
        self.e.callback_add("group.min.changed", self._min_update)

        prop = Property(parent, "max")
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)
        self.e.callback_add("group.max.changed", self._max_update)

        prop = Property(parent, "current")
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.parser_in = lambda x: str(x)
        wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)
        self.e.callback_add("group.size.changed", self._group_update)

    def _group_update(self, emissor, data):
        self["main"]["current"].value = self.e.group_size
        self.open_disable = False
        self.open = True
        self.show()

    def _min_update(self, emissor, data):
        self["main"]["min"].value = data

    def _max_update(self, emissor, data):
        self["main"]["max"].value = data

    def prop_value_changed(self, prop_name, prop_value, group_name):
        if not prop_value:
            return

        if prop_name == "min":

            def min_set(values):
                self.e.group_min = values

            old_values = self.e.group_min
            min_set(prop_value)

            op = Operation("group minimum size hints setting")
            op.redo_callback_add(min_set, prop_value)
            op.undo_callback_add(min_set, old_values)
            self._operation_stack_cb(op)

        elif prop_name == "max":

            def max_set(values):
                self.e.group_max = values

            old_values = self.e.group_max
            max_set(prop_value)

            op = Operation("group maximum size hints setting")
            op.redo_callback_add(max_set, prop_value)
            op.undo_callback_add(max_set, old_values)
            self._operation_stack_cb(op)

        elif prop_name == "current":

            def current_set(values):
                self.e.group_size = values
                self["main"]["current"].value = values

            old_values = self.e.group_size
            current_set(prop_value)

            op = Operation("group current size setting")
            op.redo_callback_add(current_set, prop_value)
            op.undo_callback_add(current_set, old_values)
            self._operation_stack_cb(op)
