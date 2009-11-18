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
import evas
import edje
import elementary

from floater import Wizard

class NewPart(Wizard):

    def __init__(self, parent):
        Wizard.__init__(self, parent, "New Part")
        self.page_add("default")
        self.style_set("minimal")

        self._name_init()
        self._types_init()

        self.action_add("default", "Cancel", self._cancel, icon="cancel")
        self.action_add("default", "Add", self._add, icon="confirm")
        self.goto("default")

    def _name_init(self):
        bx2 = elementary.Box(self)
        bx2.horizontal_set(True)
        bx2.size_hint_weight_set(1.0, 0.0)
        bx2.size_hint_align_set(-1.0, 0.0)
        bx2.size_hint_min_set(160, 160)
        self.content_append("default", bx2)
        bx2.show()

        lb = elementary.Label(self)
        lb.label_set("Name:")
        bx2.pack_end(lb)
        lb.show()

        scr = elementary.Scroller(self)
        scr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        scr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        scr.content_min_limit(False, True)
        scr.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                       elementary.ELM_SCROLLER_POLICY_OFF)
        scr.bounce_set(False, False)
        bx2.pack_end(scr)

        self._name = elementary.Entry(self)
        self._name.single_line_set(True)
        self._name.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._name.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._name.entry_set("")
        self._name.show()

        scr.content_set(self._name)
        scr.show()

    def _types_init(self):
        list = elementary.List(self)
        list.size_hint_weight_set(1.0, 1.0)
        list.size_hint_align_set(-1.0, -1.0)

        list.item_append("Rectangle", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_RECTANGLE).selected_set(True)
        list.item_append("Text", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_TEXT)
        list.item_append("Image", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_IMAGE)
        list.item_append("Swallow", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_SWALLOW)
        list.item_append("TextBlock", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_TEXTBLOCK)
        list.item_append("Gradient", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_GRADIENT)
        list.item_append("Group", None, None, self._type_select,
                         edje.EDJE_PART_TYPE_GROUP)
#        list.item_append("Box", None, None, self._type_select,
#                         edje.EDJE_PART_TYPE_BOX)
#        list.item_append("Table", None, None, self._type_select,
#                         edje.EDJE_PART_TYPE_TABLE)
        list.go()

        self.content_append("default", list)
        list.show()

    def _type_select(self, obj, event, data):
        self._type = data

    def _add(self, popup, data):
        name = self._name.entry_get().replace("<br>", "")
        if name == "":
            self._notify("Please set part name")
            return

        success = self._parent.e.part_add(name, self._type)
        if success:
            self.close()
        else:
            self._notify("Choice another name")

    def _cancel(self, popup, data):
        self.close()

