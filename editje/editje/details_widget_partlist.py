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

from details_widget_button import WidgetButton
from floater import Floater


class WidgetPartList(WidgetButton):

    def __init__(self, parent):
        WidgetButton.__init__(self, parent)
        self._value = None
        self.clicked = self._open

    def show(self):
        for o in self.objs:
            o.show()

    def hide(self):
        for o in self.objs:
            o.hide()

    def _internal_value_set(self, val):
        self._value = val
        self._update()

    def _internal_value_get(self):
        return self._value

    def _update(self):
        if self._value:
            self.obj.label_set(self._value)
        else:
            self.obj.label_set("< None >")

    def _items_load(self):
        list = []
        for item in self.parent.e.parts:
            list.append((item, item))
        return list

    def _actions_init(self):
        self._pop.title_set("List")
        self._pop.action_add("None", self._select_cb, "")
        self._pop.action_add("Cancel", self._cancel_clicked)

    def _open(self, obj, *args):
        self._pop = Floater(self.parent)
        list = elementary.List(self.parent)

        for item, action in self._items_load():
            if item != self.parent.e.part.name:
                i = list.item_append(item, None, None, self._list_select_cb,
                                     action)
                if item == self.value:
                    i.selected_set(True)

        list.go()
        list.show()
        self._pop.content_set(list)

        self._actions_init()

        x, y, w, h = self.obj.geometry

        cw, ch = self.obj.evas.size
        ow, oh = 200, 300
        ox = x - (ow - w) / 2
        oy = y - (oh - h) / 2

        if ox < 0:
            ox = 0
        elif ox + ow >= cw:
            ox = cw - ow

        if oy < 0:
            oy = 0
        elif oy + oh >= ch:
            oy = ch - oh

        self._pop.move(ox, oy)
        self._pop.resize(ow, oh)
        self._pop.show()

    def _cancel_clicked(self, popup, data):
        self._pop.hide()

    def _select_cb(self, obj, data):
        item = data
        self._value = item
        self._callback_call("changed")
        self._cancel_clicked(list, item)

    def _list_select_cb(self, list, event, data):
        self._select_cb(list, data)

