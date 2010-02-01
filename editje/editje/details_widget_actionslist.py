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
from details_widget_partlist import WidgetPartList


class WidgetActionsList(WidgetPartList):

    def _items_load(self):
        list = []
        for item in self.parent.e.programs:
            if item.startswith("@") and item.endswith("@0.00"):
                fixedname = item[1:item.rindex("@")]
                list.append((fixedname, item))
        return list

    def _actions_init(self):
        self._pop.title_set("List")
        self._pop.action_add("Cancel", self._cancel_clicked)
