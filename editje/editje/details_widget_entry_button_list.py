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
import elementary

from details_widget_entry_button import WidgetEntryButton
from floater import Floater


class WidgetEntryButtonList(WidgetEntryButton):
    pop_min_w = 200
    pop_min_h = 300

    def __init__(self, parent):
        WidgetEntryButton.__init__(self,parent)

    def _items_load(self):
        list = []
        return list

    def _actions_init(self):
        self._pop.action_add("Cancel", self._cancel_clicked)

    def _open(self, bt, *args):
        self._pop = Floater(self.parent, self.obj)
        self._pop.size_min_set(self.pop_min_w, self.pop_min_h)

        list = elementary.List(self.parent)

        for item, returned_value in self._items_load():
            i = list.item_append(item, None, None, self._list_select_cb,
                                 returned_value)

            if returned_value == self.value:
                i.selected_set(True)

        list.scroller_policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                                 elementary.ELM_SCROLLER_POLICY_ON)
        list.go()
        list.show()

        self._pop.content_set(list)
        self._actions_init()
        self._pop.show()

    def _cancel_clicked(self, popup, data):
        self._pop.hide()

    def _select_cb(self, obj, data):
        item = data
        self.value = item
        self._callback_call("changed")
        self._cancel_clicked(list, item)

    def _list_select_cb(self, list, it, entry_value):
        self._select_cb(list, entry_value)
