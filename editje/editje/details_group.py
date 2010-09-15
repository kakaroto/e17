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


class GroupDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(self, parent, operation_stack_cb)

        self.title = "group"
        self.open_disable = True

        prop = Property(parent, "min")
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Minimum group<br>width in pixels.")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Minimum group<br>height in pixels.")
        prop.widget_add("h", wid)
        self["main"].property_add(prop)
        self.e.callback_add("group.min.changed", self._min_update)

        prop = Property(parent, "max")
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Maximum group<br>width in pixels.<br>0 = disabled")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Maximum group<br>height in pixels.<br>0 = disabled")
        prop.widget_add("h", wid)
        self["main"].property_add(prop)
        self.e.callback_add("group.max.changed", self._max_update)

        prop = Property(parent, "current")
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Actual group<br>width in pixels.")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        wid.tooltip_set("Actual group<br>height in pixels.")
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

    def _context_recall(self, **kargs):
        pass

    def _prop_object_get(self):
        return self.e

    def prop_value_changed(self, prop, value, group):
        if not value:
            return

        if prop == "min":
            args = [["main"], [prop], [value], ["group_min"], [False], [None]]
            self._prop_change_do("group minimum size hints setting", *args)
        elif prop == "max":
            args = [["main"], [prop], [value], ["group_max"], [False], [None]]
            self._prop_change_do("group maximum size hints setting", *args)
        elif prop == "current":
            args = [["main"], [prop], [value], ["group_size"], [False], [None]]
            self._prop_change_do("group current size setting", *args)
