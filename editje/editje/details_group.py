#
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
from details import EditjeDetails
from details_widget_entry import WidgetEntry
from prop import Property


class GroupDetails(EditjeDetails):

    def __init__(self, parent):
        EditjeDetails.__init__(self, parent)

        self.title_set("group")

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

    def _min_update(self, emissor, data):
        self["main"]["min"].value = data

    def _max_update(self, emissor, data):
        self["main"]["max"].value = data

    def prop_value_changed(self, prop, value, group):
        if prop == "min":
            if value:
                self.e.group_min = value
        elif prop == "max":
            if value:
                self.e.group_max = value
        elif prop == "current":
            if value:
                self.e.group_size = value
                self["main"]["current"].value = self.e.group_size
