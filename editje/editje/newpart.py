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
from groupselector import NameEntry


class ExternalSelector(elementary.Box):
    def __init__(self, parent, type_cb):
        elementary.Box.__init__(self, parent)
        self.horizontal_set(True)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                  evas.EVAS_HINT_EXPAND)
        self.size_hint_align_set(evas.EVAS_HINT_FILL,
                                 evas.EVAS_HINT_FILL)

        self._module = ""
        self._type = ""
        self._parent = parent

        self._type_selected_cb = type_cb

        self._types_load()
        self._modules_init()
        self._types_init()
        self._modules_load()

    def _type_get(self):
        return self._type

    type = property(_type_get)

    def _module_get(self):
        return self._module

    module = property(_module_get)

    def _types_load(self):
        self._loaded_types = {}
        for type in edje.ExternalIterator():
            module = type.module
            name = type.name
            label = type.label_get()

            list = self._loaded_types.get(module)
            if not list:
                list = []
                self._loaded_types[module] = list
            elif (name, label, type) in list:
                continue
            list.append((name, label, type))

    def _modules_init(self):
        self._modules = elementary.List(self)
        self._modules.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                              evas.EVAS_HINT_EXPAND)
        self._modules.size_hint_align_set(evas.EVAS_HINT_FILL,
                                             evas.EVAS_HINT_FILL)
        self.pack_end(self._modules)
        self._modules.show()

    def _types_init(self):
        self._types = elementary.List(self)
        self._types.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self._types.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self.pack_end(self._types)
        self._types.show()

    def _modules_load(self):
        self._module = ""
        self._modules.clear()
        list = self._loaded_types.keys()

        list.sort(key=str.lower)

        if list:
            self._modules.item_append(list[0], None, None, self._module_select,
                                      list[0]).selected_set(True)
        for item in list[1:]:
            self._modules.item_append(item, None, None,
                                         self._module_select, item)
        self._modules.go()

    def _module_select(self, li, it, module):
        self._module = module
        self._types.clear()
        list = self._loaded_types.get(module)

        list.sort(key=lambda x:(str.lower(x[0])))

        if list:
            name, label, type = list[0]

            ico = type.icon_add(self._parent._parent.e._canvas)

            self._types.item_append(label, ico, None, self._type_select,
                                    name).selected_set(False)
        for (name, label, type) in list[1:]:
            ico = type.icon_add(self._parent._parent.e._canvas)
            self._types.item_append(label, ico, None, self._type_select, name)

        self._types.go()

    def _type_select(self, li, it, type):
        self._type = type
        if self._type_selected_cb:
            name = it.label_get().replace(" ", "")
            self._type_selected_cb(name)


class TypesList(elementary.List):
    def __init__(self, parent, type_select_cb=None):
        elementary.List.__init__(self, parent)
        self._parent = parent
        self._type_select_cb = type_select_cb

        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

        self.item_append("Rectangle", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_RECTANGLE)
        self.item_append("Text", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_TEXT)
        self.item_append("Image", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_IMAGE)
        self.item_append("Swallow", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_SWALLOW)
        self.item_append("Textblock", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_TEXTBLOCK)
        self.item_append("Gradient", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_GRADIENT)
        self.item_append("Group", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_GROUP)
        # self.item_append("Box", None, None, self._type_select_cb,
        #                  edje.EDJE_PART_TYPE_BOX)
        # self.item_append("Table", None, None, self._type_select_cb,
        #                  edje.EDJE_PART_TYPE_TABLE)
        self.item_append("External widget", None, None, self._type_select_cb,
                         edje.EDJE_PART_TYPE_EXTERNAL)
        self.go()


class NewPartWizard(Wizard):
    def __init__(self, parent, parts_list_cb=None, new_part_cb=None):
        # TODO: to be used on the future (see next FIXME tag)
        # if not new_part_cb:
        #     raise TypeError("You must set a callback for part addition on" \
        #                     " NewPartWizard objects.")
        # self._new_part_cb = new_part_cb

        Wizard.__init__(self, parent)
        self._parts_list_cb = parts_list_cb
        self._type = None

        self.page_add("default", "New Part",
                      "Name the new part to be inserted and choose its type.")

        self._part_name_entry = NameEntry(
            self, changed_cb=self._name_changed_cb,
            weight_hints=(evas.EVAS_HINT_EXPAND, 0.0),
            align_hints=(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL))
        self.content_add("default", self._part_name_entry)
        self._part_name_entry.show()

        self._types_list = TypesList(self, self._type_select)
        self.content_add("default", self._types_list)
        self._types_list.show()

        self._ext_list = ExternalSelector(self, self._default_name_set)
        self._ext_list.size_hint_weight_set(0.0, 0.0)
        self.content_add("default", self._ext_list)
        self._ext_list.show()

        self.action_add("default", "Cancel", self._cancel)
        self.action_add("default", "Add", self._add)
        self.action_disabled_set("Add", True)

        edje.message_signal_process()
        self._name_changed = False

    def _name_changed_cb(self, obj):
        self._name_changed = True
        self._check_name_and_type()

    def _type_select(self, list_, item, label, *args, **kwargs):
        self._type = label
        if self._type == edje.EDJE_PART_TYPE_EXTERNAL:
            self._external_selector_toggle(True)
        else:
            self._external_selector_toggle(False)
            self._default_name_set(item.label_get())
        self._check_name_and_type()

    def _check_name_and_type(self):
        name = self._part_name_entry.entry
        if self._type is not None and name != "":
            self.action_disabled_set("Add", False)
        else:
            self.action_disabled_set("Add", True)

    def _default_name_set(self, name):
        if self._name_changed:
            return
        count = 0
        for p in self._parent.e.parts:
            if p.startswith(name):
                count += 1
        self._part_name_entry.entry = name + "%.2d" % count
        edje.message_signal_process()
        self._name_changed = False

    # FIXME: horrible design here, won't fix it now
    def _add(self):
        name = self._part_name_entry.entry

        success = self._parent.e.part_add(name, self._type,
                                          self._ext_list.type, signal=False)

        if success:
            self._part_init(name, self._type)
            self._parent.e.event_emit("part.added", name)
            if self._type == edje.EDJE_PART_TYPE_EXTERNAL:
                self._parent.e._edje.external_add(self._ext_list.module)
        else:
            self.notify("Choose another name")

    def _part_init(self, name, type_):
        part = self._parent.e._edje.part_get(name)
        statename = part.state_selected_get()
        state = part.state_get(statename)

        w, h = self._parent.e._edje.size
        state.rel1_relative_set(0.0, 0.0)
        state.rel1_offset_set(w / 4, h / 4)
        state.rel2_relative_set(0.0, 0.0)
        state.rel2_offset_set(w * 3 / 4, h * 3 / 4)

        if type_ == edje.EDJE_PART_TYPE_RECTANGLE:
            self._part_init_rectangle(part, state)
        elif type_ == edje.EDJE_PART_TYPE_TEXT:
            self._part_init_text(part, state)
        elif type_ == edje.EDJE_PART_TYPE_EXTERNAL:
            self._part_init_external(part, state)
        self.close()

    def _part_init_rectangle(self, part, state):
        part.mouse_events = False
        state.color_set(0, 255, 0, 128)

    def _part_init_text(self, part, state):
        part.mouse_events = False
        state.color_set(0, 0, 0, 255)
        state.text_set("YOUR TEXT HERE")
        state.font_set("Sans")
        state.text_size_set(16)

    def _part_init_external(self, part, state):
        pass

    def _external_selector_toggle(self, show):
        if show:
            self._ext_list.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                evas.EVAS_HINT_EXPAND)
        else:
            self._ext_list.size_hint_weight_set(0, 0)

    def _cancel(self):
        self.close()
