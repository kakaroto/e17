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

import re

import evas
import edje
import elementary

from collapsable import Collapsable
from operation import Operation
from filewizard import ImageSelectionWizard


class WidgetsList(Collapsable):
    def __init__(self, parent, editable_grp, operation_stack_cb,
            img_new_img_cb=None, img_list_get_cb=None,
            img_id_get_cb=None, workfile_name_get_cb=None):
        Collapsable.__init__(self, parent)

        self._parent = parent
        self._edit_grp = editable_grp
        self._operation_stack_cb = operation_stack_cb
        self._new_img_cb = img_new_img_cb
        self._img_list_get_cb = img_list_get_cb
        self._img_id_get_cb = img_id_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb

        self._part_name = None
        self._part_type = None

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
        self._pager.style_set("editje.rightwards")
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

        types = self._loaded_types[group]
        types.sort(key=lambda x: (str.lower(x[0])))

        for widget in types:
            ico = None
            if widget[1] == edje.EDJE_PART_TYPE_EXTERNAL:
                ico = widget[2].icon_add(self.evas)
            list.item_append(widget[0], ico, None, None, widget)
        list.go()
        list.show()

        it.selected = False

        self._pager.content_push(list)

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(
            self.evas, file=self._theme_file,
            group="editje/collapsable/list/options/widgets")
        self._options_edje.signal_callback_add(
            "back", "editje/collapsable/list/options", self._back_cb)
        self.content_set("options", self._options_edje)
        self._options = False

    def _back_cb(self, obj, emission, source):
        self._pager.content_pop()
        self._group = None
        self.options = False

    # Widgets
    def _widget_selected_cb(self, li, it):
        label, self._part_type, external_type = it.data_get()[0][0]

        max = 0
        for p in self._edit_grp.parts:
            if re.match("%s\d{2,}" % label, p):
                num = int(p[len(label):])
                if num > max:
                    max = num
        self._part_name = label + "%.2d" % (max + 1)

        if self._part_type == edje.EDJE_PART_TYPE_EXTERNAL:
            external_name = external_type.name
            external_module = external_type.module
        else:
            external_name = ""
            external_module = None

        def part_add(name, edje_type, external_name="", external_module=None):
            return self._edit_grp.part_add(name, edje_type, external_name)

        if self._part_type == edje.EDJE_PART_TYPE_IMAGE:
            ImageSelectionWizard(
                self._parent, self._image_set, self._new_img_cb,
                self._img_list_get_cb, self._img_id_get_cb,
                self._workfile_name_get_cb).show()
        else:
            if part_add(self._part_name, self._part_type, external_name,
                        external_module):
                op = Operation("part addition")
                op.redo_callback_add(part_add, self._part_name,
                        self._part_type, external_name, external_module)
                op.undo_callback_add(self._edit_grp.part_del, self._part_name)
                self._operation_stack_cb(op)

        it.selected = False

    def _image_set(self, selection):
        def part_add(name, edje_type, selection):
            if not self._edit_grp.part_add(name, edje_type, ""):
                return False
            self._edit_grp.part.state.image = selection
            return True

        if part_add(self._part_name, self._part_type, selection):
            op = Operation("part addition")
            op.redo_callback_add(part_add, self._part_name, self._part_type,
                    selection)
            op.undo_callback_add(self._edit_grp.part_del, self._part_name)
            self._operation_stack_cb(op)
