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
        self._pop = None
        self._pop_list = None

    def _items_load(self):
        list = []
        return list

    def _actions_init(self):
        self._pop.action_add("Cancel", self._cancel_clicked)

    def _open(self, bt, *args):
        if not self._pop:
            self._pop = Floater(bt, self.obj)
            self._pop.size_min_set(self.pop_min_w, self.pop_min_h)
            self._actions_init()

            self._pop_list = elementary.List(self._pop)

            self._pop_list.scroller_policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                                               elementary.ELM_SCROLLER_POLICY_ON)
            self._pop_list.callback_selected_add(self._select_cb)
            self._pop.content_set(self._pop_list)
            self._pop_list.show()

        self._list_update()

        self._pop.show()

    def _list_update(self):
        self._pop_list.clear()
        for label, value in self._items_load():
            it = self._pop_list.item_append(label, None, None, None, value)

            if value == self.value:
                it.selected_set(True)
        self._pop_list.go()

    def _cancel_clicked(self, popup, data):
        self._pop.hide()

    def _select_cb(self, li, id):
        self.value = li.selected_item_get().data_get()[0][0]
        self._callback_call("changed")
        self._cancel_clicked(list, li)

    def _list_select_cb(self, list, it, entry_value):
        self._select_cb(list, entry_value)
