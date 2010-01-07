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
from details_widget_boolean import WidgetBoolean
from details_widget_color import WidgetColor
from details_widget_button import WidgetButton
from details_widget_combo import WidgetCombo
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
        self.e.callback_add("group.changed", self._group_update)

    def _group_update(self, emissor, data):
        self["main"]["current"].value = self.e._edje.size
        self.e._edje.on_resize_add(self._group_resized)

    def _group_resized(self, obj):
        self["main"]["current"].value = obj.size

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
                self["main"]["current"].value = self.e._edje.size
            else:
                w, h = value
                w_min, h_min = self.e.group_min
                w_max, h_max = self.e.group_max

                if w < w_min:
                    w = w_min
                elif w_max > 0 and w > w_max:
                    w = w_max

                if h < h_min:
                    h = h_min
                elif h_max > 0 and h > h_max:
                    h = h_max

                self.e._edje.resize(w, h)
                self.prop_set("current", (w, h))
