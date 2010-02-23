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


class WidgetSignal(WidgetEntryButtonList):

    def _floater_list_items_update(self):
        list = []

        list.append(("On Load", "load"))
        list.append(("Mouse In", "mouse,in"))
        list.append(("Mouse Out", "mouse,out"))

        list.append(("Mouse Left Down", "mouse,down,1"))
        list.append(("Mouse Right Down", "mouse,down,3"))
        list.append(("Mouse Any Down", "mouse,down,*"))

        list.append(("Mouse Left Double Down", "mouse,down,1,double"))
        list.append(("Mouse Right Double Down", "mouse,down,3,double"))

        list.append(("Mouse Left Up", "mouse,up,1"))
        list.append(("Mouse Right Up", "mouse,up,3"))
        list.append(("Mouse Any Up", "mouse,up,*"))

        list.append(("Mouse Left Click", "mouse,clicked,1"))
        list.append(("Mouse Right Click", "mouse,clicked,3"))
        list.append(("Mouse Any Click", "mouse,clicked,*"))

        list.append(("Mouse Move", "mouse,move"))
        list.append(("Mouse Wheel Up", "mouse,wheel,0,-1"))
        list.append(("Mouse Wheel Down", "mouse,wheel,0,1"))

        return list

    def _floater_title_init(self):
        self._floater.title_set("Common Signals")

    def _floater_actions_init(self):
        self._floater.action_add("None", self._none_selected)
        WidgetEntryButtonList._floater_actions_init(self)

    def _none_selected(self, *args):
        self.value_set("")
        self._floater_cancel()
