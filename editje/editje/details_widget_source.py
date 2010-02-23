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
from details_widget_entry_button_list import WidgetEntryButtonList


class WidgetSource(WidgetEntryButtonList):

    def _floater_list_items_update(self):
        list = []

        for item in self.parent.e.parts:
            list.append((item, item))
        return list

    def _floater_title_init(self):
        self._floater.title_set("Parts list")

    def _floater_actions_init(self):
        self._floater.action_add("None", self._none_selected)
        WidgetEntryButtonList._floater_actions_init(self)

    def _none_selected(self, *args):
        self.value_set("")
        self._floater_cancel()
