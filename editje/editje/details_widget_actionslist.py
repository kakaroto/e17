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

from details_widget_button_list import WidgetButtonList
from floater_opener import FloaterListOpener


class WidgetActionsList(WidgetButtonList):
    def _floater_list_items_update(self):
        list = []
        for item in self._list_get_cb():
            if item.startswith("@") and item.endswith("@0.00"):
                fixedname = item[1:item.rindex("@")]
                list.append((fixedname, item))
        return list

    def _floater_actions_init(self):
        self._floater.action_add("None", self._none_selected)
        FloaterListOpener._floater_actions_init(self)

    def _none_selected(self, *args):
        self.value_set("")
        self._floater_cancel()
