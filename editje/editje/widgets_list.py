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

from collapsable import Collapsable


class WidgetsList(Collapsable):

    def __init__(self, parent):
        Collapsable.__init__(self, parent)
        self.e = parent.e

        self._types_load()
        self._content_load()
        self._options_load()

    def _types_load(self):
        self._loaded_types = {}
        list = []
        none = ""
        self._loaded_types["Edje"] = list
        list.append(("Rectangle", edje.EDJE_PART_TYPE_RECTANGLE, none))
        list.append(("Text", edje.EDJE_PART_TYPE_TEXT, none))
        list.append(("Image", edje.EDJE_PART_TYPE_IMAGE, none))
        list.append(("Swallow", edje.EDJE_PART_TYPE_SWALLOW, none))
        list.append(("TextBlock", edje.EDJE_PART_TYPE_TEXTBLOCK, none))
        list.append(("Gradient", edje.EDJE_PART_TYPE_GRADIENT, none))
        list.append(("Group", edje.EDJE_PART_TYPE_GROUP, none))
        #list.append(("Box", edje.EDJE_PART_TYPE_BOX, none))
        #list.append(("Table", edje.EDJE_PART_TYPE_TABLE, none))

        for type in edje.ExternalIterator():
            module_name = type.module_name
            label = type.label_get()

            list = self._loaded_types.get(module_name)
            if not list:
                list = []
                self._loaded_types[module_name] = list
            elif (type.name, label, type) in list:
                continue
            list.append((label, edje.EDJE_PART_TYPE_EXTERNAL, type))

    # Groups
    def _content_load(self):
        self._pager = elementary.Pager(self)
        self._pager.style_set("editje.mainbar")
        self._pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self._pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self.content_set("content", self._pager)

        self._groups_list = elementary.List(self._pager)
        self._groups_list.style_set("editje.collapsable")
        self._groups_list.bounce_set(False, False)
        self._groups_list.callback_selected_add(self._group_selected_cb)
        for group in self._loaded_types.keys():
            self._groups_list.item_append(group)
        self._groups_list.go()
        self._groups_list.show()

        self._pager.content_push(self._groups_list)

    def _group_selected_cb(self, li, it):
        group = it.label_get()
        self._group = group

        self._options_edje.part_text_set("group_name", group)

        self.options = True

        list = elementary.List(self._pager)
        list.bounce_set(False, False)
        list.callback_selected_add(self._widget_selected_cb)
        for widget in self._loaded_types[group]:
            ico = None
            if widget[1] == edje.EDJE_PART_TYPE_EXTERNAL:
                ico = widget[2].icon_add(self.e._canvas)
            list.item_append(widget[0], ico, None, None, widget)
        list.go()
        list.show()

        it.selected = False

        self._pager.content_push(list)

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(self.edje_get().evas,
                                file=self._theme_file,
                                group="editje/collapsable/list/options/widgets")
        self._options_edje.signal_callback_add("back",
                                "editje/collapsable/list/options",
                                self._back_cb)
        self.content_set("options", self._options_edje)
        self._options = False

    def _back_cb(self, obj, emission, source):
        self._pager.content_pop()
        self._group = None
        self.options = False

    # Widgets
    def _widget_selected_cb(self, li, it):
        label, edje_type, type = it.data_get()[0][0]

        type_name = ""

        max = 0
        for p in self._parent.e.parts:
            if p.startswith(label):
                num = int(p[len(label):])
                if num > max:
                    max = num
        name = label + "%.2d" % (max + 1)

        if (edje_type == edje.EDJE_PART_TYPE_EXTERNAL):
            type_name = type.name

        success = self._parent.e.part_add(name, edje_type,
                                          type_name, signal=False)
        if success:
            self._part_init(name, edje_type)
            self._parent.e.event_emit("part.added", name)
            if edje_type == edje.EDJE_PART_TYPE_EXTERNAL:
                self._parent.e._edje.external_add(type.module)
        it.selected = False

    def _part_init(self, name, type):
        part = self._parent.e._edje.part_get(name)
        statename = part.state_selected_get()
        state = part.state_get(statename)

        w, h = self._parent.e._edje.size
        state.rel1_relative_set(0.0, 0.0)
        state.rel1_offset_set(w / 4, h / 4)
        state.rel2_relative_set(0.0, 0.0)
        state.rel2_offset_set(w * 3 / 4, h * 3 / 4)

        if type == edje.EDJE_PART_TYPE_RECTANGLE:
            self._part_init_rectangle(part, state)
        elif type == edje.EDJE_PART_TYPE_TEXT:
            self._part_init_text(part, state)

    def _part_init_rectangle(self, part, state):
        part.mouse_events = False

        state.color_set(0, 255, 0, 128)

    def _part_init_text(self, part, state):
        part.mouse_events = False

        state.color_set(0, 0, 0, 255)
        state.text_set("YOUR TEXT HERE")
        state.font_set("Sans")
        state.text_size_set(16)
