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

from details_widget_entry_button_list import WidgetEntryButtonList


class WidgetSignal(WidgetEntryButtonList):
    def __init__(
        self, parent, list_get_cb=None, popup_hide_object_signal_list=[]):
        WidgetEntryButtonList.__init__(
            self, parent, list_get_cb, popup_hide_object_signal_list)

    def _floater_list_items_update(self):
        items_list = []

        items_list.append(("On Load", "load"))
        items_list.append(("Mouse In", "mouse,in"))
        items_list.append(("Mouse Out", "mouse,out"))

        items_list.append(("Mouse Left Down", "mouse,down,1"))
        items_list.append(("Mouse Right Down", "mouse,down,3"))
        items_list.append(("Mouse Any Down", "mouse,down,*"))

        items_list.append(("Mouse Left Double Down", "mouse,down,1,double"))
        items_list.append(("Mouse Right Double Down", "mouse,down,3,double"))

        items_list.append(("Mouse Left Up", "mouse,up,1"))
        items_list.append(("Mouse Right Up", "mouse,up,3"))
        items_list.append(("Mouse Any Up", "mouse,up,*"))

        items_list.append(("Mouse Left Click", "mouse,clicked,1"))
        items_list.append(("Mouse Right Click", "mouse,clicked,3"))
        items_list.append(("Mouse Any Click", "mouse,clicked,*"))

        items_list.append(("Mouse Move", "mouse,move"))
        items_list.append(("Mouse Wheel Up", "mouse,wheel,0,-1"))
        items_list.append(("Mouse Wheel Down", "mouse,wheel,0,1"))

        items_list.append(("Animation End", "animation,end"))

        return items_list

    def _floater_title_init(self):
        self._floater.title_set("Common Signals")

    def _floater_actions_init(self):
        self._floater.action_add("None", self._none_selected)
        WidgetEntryButtonList._floater_actions_init(self)

    def _none_selected(self, *args):
        self.value_set("")
        self._floater_cancel()
