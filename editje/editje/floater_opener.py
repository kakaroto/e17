# Copyright (C) 2010 Samsung Electronics.
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

import elementary

from floater import Floater


class FloaterOpener(object):
    _floater_min_w = 200
    _floater_min_h = 300

    def __init__(self):
        self._floater = None
        #self._floater_parent = parent

    def _floater_title_init(self):
        self._floater.title_set("")

    def _floater_actions_init(self):
        self._floater.action_add("Cancel", self._floater_cancel)

    def _floater_content_init(self):
        return

    def _floater_open(self, parent, *args):
        if not self._floater:
            self._floater = Floater(parent, parent)
            self._floater.size_min_set(self._floater_min_w, self._floater_min_h)

            self._floater_title_init()
            self._floater_actions_init()
            self._floater_content_init()

        self._floater_update()

        self._floater.show()

    def _floater_update(self):
        return

    def _floater_cancel(self, *args):
        self._floater.hide()


class FloaterListOpener(FloaterOpener):
    def _floater_content_init(self):
        self._floater_list = elementary.List(self._floater)

        self._floater_list.scroller_policy_set(
            elementary.ELM_SCROLLER_POLICY_OFF,
            elementary.ELM_SCROLLER_POLICY_ON)
        self._floater_list.callback_selected_add(self._floater_list_select_cb)
        self._floater.content_set(self._floater_list)
        self._floater_list.show()

    def _floater_update(self):
        self._floater_list.clear()
        for label, value in self._floater_list_items_update():
            it = self._floater_list.item_append(label, None, None, None, value)
        self._floater_list.go()

    def _floater_list_items_update(self):
        return []

    def _floater_list_select_cb(self, li, id):
        self.value_set(li.selected_item_get().data_get()[0][0])
        self._floater_cancel()

    def value_set(self, value):
        return
