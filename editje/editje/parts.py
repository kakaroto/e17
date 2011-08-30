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

import evas
import edje
import elementary

import re

from clist import CList
from floater import Wizard
from groupselector import NameEntry
from operation import Operation
from filewizard import ImageSelectionWizard
import objects_data
import sysconfig


class PartsList(CList):
    def __init__(self, parent, editable_grp, operation_stack_cb,
            img_new_img_cb=None, img_list_get_cb=None,
            img_id_get_cb=None, workfile_name_get_cb=None):

        CList.__init__(self, parent)

        self._edit_grp = editable_grp
        self._operation_stack_cb = operation_stack_cb
        self._new_img_cb = img_new_img_cb
        self._img_list_get_cb = img_list_get_cb
        self._img_id_get_cb = img_id_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb

        self._options_load()

        self._edit_grp.callback_add("parts.changed", self._parts_update_cb)
        self._edit_grp.callback_add("part.added", self._part_added_cb)
        self._edit_grp.callback_add("part.removed", self._part_removed_cb)

        self._edit_grp.part.callback_add("part.changed", self._part_changed_cb)
        self._edit_grp.part.callback_add(
            "part.unselected", self._part_changed_cb)
        self._edit_grp.part.callback_add("name.changed", self._name_changed_cb)

    def _parts_update_cb(self, emissor, data):
        parts_list = data

        self.clear()
        for i in parts_list[::-1]:
            self.add(i)
        self.go()

    def _part_added_cb(self, emissor, data):
        self.add(data)
        self.go()
        self.open = True
        self.select(data)

    def _part_removed_cb(self, emissor, data):
        self.remove(data)

    def _part_changed_cb(self, emissor, data):
        self.selection_clear()
        self.select(data)

    def _name_changed_cb(self, emissor, data):
        for s in self._selected.iterkeys():
            item = self._items[s]
            if item.label_get() == data[0]:
                item.text_set(data[1])
                self._selected[data[1]] = self._selected[data[0]]
                self._items[data[1]] = self._items[data[0]]
                del self._selected[data[0]]
                del self._items[data[0]]
                return

    # Selection
    def _selected_cb(self, list_, list_item):
        CList._selected_cb(self, list_, list_item)
        name = list_item.label_get()

        self._edit_grp.part.name = name
        if (len(self._items) > 1):
            self._options_edje.signal_emit("up,enable", "")
            self._options_edje.signal_emit("down,enable", "")
        self._options_edje.signal_emit("remove,enable", "")

    def _unselected_cb(self, li, it):
        CList._unselected_cb(self, li, it)
        if not self._selected:
            if (len(self._items) > 1):
                self._options_edje.signal_emit("up,disable", "")
                self._options_edje.signal_emit("down,disable", "")
            self._options_edje.signal_emit("remove,disable", "")

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(
            self.evas, file=self._theme_file,
            group="editje/collapsable/list/options/parts")
        self._options_edje.signal_callback_add(
            "new", "editje/collapsable/list/options", self._new_cb)
        self._options_edje.signal_callback_add(
            "up", "editje/collapsable/list/options", self._up_cb)
        self._options_edje.signal_callback_add(
            "down", "editje/collapsable/list/options", self._down_cb)
        self._options_edje.signal_callback_add(
            "remove", "editje/collapsable/list/options", self._remove_cb)
        self._options_edje.signal_emit("up,disable", "")
        self._options_edje.signal_emit("down,disable", "")
        self._options_edje.signal_emit("remove,disable", "")
        self.content_set("options", self._options_edje)
        self._options = False

    def _new_cb(self, obj, emission, source):
        new_part_wiz = NewPartWizard(
            self._parent, self._edit_grp, self._operation_stack_cb,
            img_new_img_cb=self._new_img_cb,
            img_list_get_cb=self._img_list_get_cb,
            img_id_get_cb=self._img_id_get_cb,
            workfile_name_get_cb=self._workfile_name_get_cb)
        new_part_wiz.open()

    def _restack_above(self, part_name):
        self._context_recall(part=part_name)
        return self._edit_grp.part.restack_above()

    def _restack_below(self, part_name):
        self._context_recall(part=part_name)
        return self._edit_grp.part.restack_below()

    def _up_cb(self, obj, emission, source):
        r = self._restack_above(self._edit_grp.part.name)
        if not r:
            return

        op = Operation("part re-stacking (above)")
        op.redo_callback_add(self._restack_above, self._edit_grp.part.name)
        op.undo_callback_add(self._restack_above, self._edit_grp.part.name)
        self._operation_stack_cb(op)

    def _down_cb(self, obj, emission, source):
        r = self._restack_below(self._edit_grp.part.name)
        if not r:
            return

        op = Operation("part re-stacking (below)")
        op.redo_callback_add(self._restack_below, self._edit_grp.part.name)
        op.undo_callback_add(self._restack_above, self._edit_grp.part.name)
        self._operation_stack_cb(op)

    def _remove_cb(self, obj, emission, source):

        def anims_restore(part_name, anims_save):
            curr_anim = self._edit_grp.animation.name
            self._edit_grp.animation.name = None

            part_obj = self._edit_grp.part_get(part_name)

            for name, contents in anims_save.iteritems():
                for time, st_save in contents:
                    p_name = "@%s@%.2f" % (name, time)

                    # state should exist by objects_data.Part.apply_to
                    state = part_obj.state_get(p_name, 0.0)
                    st_save.apply_to(state)

                    prog = self._edit_grp.program_get(p_name)
                    prog.target_add(part_name)

            self._edit_grp.animation.name = curr_anim
            if self._edit_grp.mode == "Parts":
                self._edit_grp.part.state.name = "default 0.0"

        def sigs_restore(part_name, sigs_save):
            curr_sig = self._edit_grp.signal.name
            self._edit_grp.signal.name = None

            for sig in sigs_save:
                prog = self._edit_grp.program_get(sig)
                prog.source = part_name

            self._edit_grp.signal.name = curr_sig

        def relative_animations_part_clear(part_name):
            anims = {}

            curr_anim = self._edit_grp.animation.name
            part_obj = self._edit_grp.part_get(part_name)

            for anim in self._edit_grp.animations:
                prog = self._edit_grp.program_get("@%s@0.00" % anim)
                if not part_name in prog.targets:
                    continue

                anims[anim] = []

                time = prog.name[-4:]
                while time != "@end":
                    st_cls = objects_data.state_class_from_part_type_get(
                        part_obj)
                    st_save = st_cls(part_obj.state_get(prog.name, 0.0))

                    # we gotta clean editable_animation's data for curr_anim
                    if anim != curr_anim:
                        prog.target_del(part_name)

                    anims[anim].append((float(time), st_save))
                    prog = self._edit_grp.program_get(prog.afters[0])
                    time = prog.name[-4:]

            if curr_anim:
                self._edit_grp.animation.part_remove(part_name)

            return anims

        # FIXME/TODO: when the "source" property's floater box of general
        # purpose signals get properly populated with the group's parts,
        # also save the participation of the part being deleted on that signal
        def relative_signals_part_clear(part_name):
            sigs = []

            curr_sig = self._edit_grp.signal.name
            self._edit_grp.signal.name = None

            for sig in self._edit_grp.signals:
                prog = self._edit_grp.program_get(sig)
                if part_name != prog.source:
                    continue

                prog.source = ""
                sigs.append(sig)

            self._edit_grp.signal.name = curr_sig

            return sigs

        for to_del in self.selected:
            part_name = to_del[0]
            part_save = objects_data.Part(self._edit_grp.part_get(part_name))
            relatives = self._edit_grp.relative_parts_get(part_name)

            anims_save = relative_animations_part_clear(part_name)
            sigs_save = relative_signals_part_clear(part_name)

            r = self._edit_grp.part_del(part_name)
            if not r:
                del part_save
                del anims_save
                del sigs_save
                continue

            op = Operation("part deletion")
            op.redo_callback_add(relative_animations_part_clear, part_name)
            op.redo_callback_add(relative_signals_part_clear, part_name)
            op.redo_callback_add(self._edit_grp.part_del, part_name)
            op.undo_callback_add(
                self._edit_grp.part_add_bydata, part_save, relatives)
            op.undo_callback_add(anims_restore, part_name, anims_save)
            op.undo_callback_add(sigs_restore, part_name, sigs_save)

            self._operation_stack_cb(op)

    def _context_recall(self, **kargs):
        if "part" in kargs:
            self._edit_grp.part.name = kargs["part"]
            if "state" in kargs:
                self._edit_grp.part.state.name = kargs["state"]


class ExternalSelector(elementary.Box):
    def __init__(self, parent, type_cb):
        elementary.Box.__init__(self, parent)

        self.horizontal_set(True)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

        self._module = ""
        self._type = ""
        self._type_selected_cb = type_cb

        self._types_load()
        self._modules_init()
        self._types_init()
        self._modules_load()

    def _type_get(self):
        return self._type

    def _type_set(self, external_type):
        self._type = external_type

    type = property(fget=_type_get, fset=_type_set)

    def _module_get(self):
        return self._module

    module = property(_module_get)

    def _types_load(self):
        self._loaded_types = {}
        for external_type in edje.ExternalIterator():
            module = external_type.module_name
            name = external_type.name
            label = external_type.label_get()

            types_list = self._loaded_types.get(module)
            if not types_list:
                types_list = []
                self._loaded_types[module] = types_list
            elif (name, label, external_type) in types_list:
                continue
            types_list.append((name, label, external_type))

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
        modules_list = self._loaded_types.keys()

        modules_list.sort(key=str.lower)

        if modules_list:
            self._modules_first = self._modules.item_append(
                    modules_list[0], None, None,
                    self._module_select, modules_list[0])
            self._modules_first.selected_set(True)
        for item in modules_list[1:]:
            self._modules.item_append(item, None, None,
                                      self._module_select, item)
        self._modules.go()

    def _module_select(self, li, it, module):
        if self._module:
            self._clear_types()
        self._module = module
        self._types.clear()
        types_list = self._loaded_types.get(module)

        types_list.sort(key=lambda x: (str.lower(x[0])))

        if types_list:
            name, label, external_type = types_list[0]

            ico = external_type.icon_add(self.evas)

            self._types.item_append(label, ico, None, self._type_select,
                                    name).selected_set(False)
        for (name, label, external_type) in types_list[1:]:
            ico = external_type.icon_add(self.evas)
            self._types.item_append(label, ico, None, self._type_select, name)

        self._types.go()

    def _type_select(self, li, it, external_type):
        self._type = external_type
        if self._type_selected_cb:
            name = it.label_get().replace(" ", "")
            self._type_selected_cb(name)

    def _clear_types(self):
        self._type = ""
        external_type = self._types.selected_item_get()
        if external_type:
            external_type.selected_set(False)
        if self._type_selected_cb:
            self._type_selected_cb("")


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
    def __init__(self, parent, editable_grp, operation_stack_cb,
                 img_new_img_cb=None, img_list_get_cb=None,
                 img_id_get_cb=None, workfile_name_get_cb=None):

        Wizard.__init__(self, parent)
        self._parent = parent
        self._edit_grp = editable_grp
        self._operation_stack_cb = operation_stack_cb
        self._new_img_cb = img_new_img_cb
        self._img_list_get_cb = img_list_get_cb
        self._img_id_get_cb = img_id_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb
        self._part_name = None
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

        self.action_add("default", "Cancel", self._cancel, key="Escape")
        self.action_add("default", "Add", self._sizing_go, key="Return")
        self.action_disabled_set("default", "Add", True)

        edje.message_signal_process()
        self._name_changed = False

        self.page_add("sizing", "New Part", "Position and size of new part.")
        self._partsetup = PartSetup(self)
        self._partsetup.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                evas.EVAS_HINT_EXPAND)
        self._partsetup.size_hint_align_set(evas.EVAS_HINT_FILL,
                evas.EVAS_HINT_FILL)
        self._partsetup.show()
        self.action_add("sizing", "Cancel", self._default_go, key="Escape")
        self.action_add("sizing", "Add", self._add, key="Return")
        self.content_add("sizing", self._partsetup)

        self.goto("default")

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
        error_msg = "This part name is already used in this group"

        def good():
            self._part_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", False)

        def bad():
            self._part_name_entry.status_label = error_msg
            self.action_disabled_set("default", "Add", True)

        def incomplete():
            self._part_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", True)

        name = self._part_name_entry.entry
        if name in self._edit_grp.parts:
            bad()
            return

        if not name or not self._type:
            incomplete()
            return

        external_type = self._ext_list.type
        if self._type == edje.EDJE_PART_TYPE_EXTERNAL and not external_type:
            incomplete()
            return

        good()

    def _default_name_set(self, label):
        if self._name_changed and self._part_name_entry.entry or not label:
            self._check_name_and_type()
            return

        max_num = 0
        for p in self._edit_grp.parts:
            name = re.match("^%s(\d{2,})$" % label, p)
            if name:
                num = int(name.group(1))
                if num > max_num:
                    max_num = num
        self._part_name_entry.entry = label + "%.2d" % (max_num + 1)
        edje.message_signal_process()
        self._name_changed = False

    def _add(self):
        def add_internal(name, edje_type, ext_name=""):
            if not self._edit_grp.part_add(name, edje_type, ext_name,
                                           init=self._partsetup.apply_to):
                self.notify("Error adding new part.")
                return False
            return True

        self._part_name = self._part_name_entry.entry
        if self._type == edje.EDJE_PART_TYPE_EXTERNAL:
            ext_name = self._ext_list.type
        else:
            ext_name = ""

        if self._type == edje.EDJE_PART_TYPE_IMAGE:
            ImageSelectionWizard(self._parent, self._image_set,
                    self._new_img_cb, self._img_list_get_cb,
                    self._img_id_get_cb, self._workfile_name_get_cb).show()
        else:
            if add_internal(self._part_name, self._type, ext_name):
                op = Operation("part addition")
                op.redo_callback_add(
                    add_internal, self._part_name, self._type, ext_name)
                op.undo_callback_add(self._edit_grp.part_del, self._part_name)
                self._operation_stack_cb(op)

            self.close()

    def _image_set(self, selection):
        def part_add(name, edje_type, selection):
            if not self._edit_grp.part_add(name, edje_type, ""):
                return False
            self._edit_grp.part.state.image = selection
            return True

        if part_add(self._part_name, self._type, selection):
            op = Operation("part addition")
            op.redo_callback_add(part_add, self._part_name,
                    self._type, selection)
            op.undo_callback_add(self._edit_grp.part_del, self._part_name)
            self._operation_stack_cb(op)

    def _external_selector_toggle(self, show):
        if show:
            self._ext_list.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                evas.EVAS_HINT_EXPAND)
            self._ext_list._modules_first.selected_set(True)
        else:
            self._ext_list.size_hint_weight_set(0, 0)
            external_type = self._ext_list._types.selected_item_get()
            if external_type:
                external_type.selected_set(False)
                self._ext_list.type = ""

    def _cancel(self):
        self.close()

    def _sizing_go(self):
        self.goto("sizing")

    def _default_go(self):
        self.goto("default")


class PartSetup(elementary.Box):
    def __init__(self, parent):
        elementary.Box.__init__(self, parent)
        self.__reference_init()
        self.__position_init()
        self.__size_init()

    def __reference_init(self):
        fr = elementary.Frame(self)
        fr.text_set("Reference")
        fr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        fr.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self.pack_end(fr)
        fr.show()

        fbx = elementary.Box(self)
        fbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        fbx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        fr.content_set(fbx)
        fbx.show()

        rdg = elementary.Radio(fbx)
        rdg.state_value_set(0)
        rdg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        rdg.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        rdg.text_set("Group")
        fbx.pack_end(rdg)
        rdg.show()
        self.__reference = rdg

        fbx2 = elementary.Box(fbx)
        fbx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        fbx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        fbx.pack_end(fbx2)
        fbx2.horizontal_set(True)
        fbx2.show()

        rd = elementary.Radio(fbx2)
        rd.state_value_set(1)
        rd.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        rd.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        rd.text_set("Part")
        rd.group_add(rdg)
        rd.disabled_set(True)
        fbx2.pack_end(rd)
        rd.show()

    def __position_init(self):
        fr = elementary.Frame(self)
        fr.text_set("Position")
        fr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        fr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.pack_end(fr)
        fr.show()

        ly = elementary.Layout(fr)
        ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        ly.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        ly.file_set(sysconfig.theme_file_get("default"), "editje/position")
        fr.content_set(ly)
        ly.show()

        rdg = elementary.Radio(ly)
        rdg.state_value_set(0)
        ly.content_set("internal.middle", rdg)
        rdg.show()
        self.__position = rdg

        rd = elementary.Radio(ly)
        rd.state_value_set(1)
        rd.group_add(rdg)
        ly.content_set("external.top_left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(2)
        rd.group_add(rdg)
        ly.content_set("external.top", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(3)
        rd.group_add(rdg)
        ly.content_set("external.top_right", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(4)
        rd.group_add(rdg)
        ly.content_set("external.left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(5)
        rd.group_add(rdg)
        ly.content_set("external.right", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(6)
        rd.group_add(rdg)
        ly.content_set("external.bottom_left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(7)
        rd.group_add(rdg)
        ly.content_set("external.bottom", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(8)
        rd.group_add(rdg)
        ly.content_set("external.bottom_right", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(9)
        rd.group_add(rdg)
        ly.content_set("internal.top_left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(10)
        rd.group_add(rdg)
        ly.content_set("internal.top", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(11)
        rd.group_add(rdg)
        ly.content_set("internal.top_right", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(12)
        rd.group_add(rdg)
        ly.content_set("internal.left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(13)
        rd.group_add(rdg)
        ly.content_set("internal.right", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(14)
        rd.group_add(rdg)
        ly.content_set("internal.bottom_left", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(15)
        rd.group_add(rdg)
        ly.content_set("internal.bottom", rd)
        rd.show()

        rd = elementary.Radio(ly)
        rd.state_value_set(16)
        rd.group_add(rdg)
        ly.content_set("internal.bottom_right", rd)
        rd.show()

    def __size_init(self):
        fr = elementary.Frame(self)
        fr.text_set("Size")
        fr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        fr.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self.pack_end(fr)
        fr.show()

        fbx = elementary.Box(self)
        fbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        fbx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        fr.content_set(fbx)
        fbx.show()

        rdg = elementary.Radio(fbx)
        rdg.state_value_set(0)
        rdg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        rdg.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        rdg.text_set("Fill reference")
        fbx.pack_end(rdg)
        rdg.show()
        self.__size = rdg

        fbx2 = elementary.Box(fbx)
        fbx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        fbx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        fbx.pack_end(fbx2)
        fbx2.horizontal_set(True)
        fbx2.show()

        rd = elementary.Radio(fbx2)
        rd.state_value_set(1)
        rd.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        rd.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        rd.text_set("Fixed size:")
        rd.group_add(rdg)
        rd.disabled_set(True)
        fbx2.pack_end(rd)
        rd.show()

    def apply_to(self, part):
        state = part.state_get(*part.state_selected_get())

        if self.__reference.value_get() == 0:
            to = None
        else:
            to = None # FIXME
        state.rel1_to = (to, to)
        state.rel2_to = (to, to)

        align_x = 0.5
        align_y = 0.5
        rel1_x = 0.0
        rel1_y = 0.0
        ofs1_x = 0
        ofs1_y = 0
        rel2_x = 1.0
        rel2_y = 1.0
        ofs2_x = -1
        ofs2_y = -1

        position = self.__position.value_get()
        if position in [1, 4, 6]:
            align_x = 1.0
            rel1_x = -1.0
            rel2_x = 0.0
        elif position in [11, 13, 16]:
            align_x = 1.0
        elif position in [3, 5, 8]:
            align_x = 0.0
            rel1_x = 1.0
            rel2_x = 2.0
        elif position in [9, 12, 14]:
            align_x = 0.0

        if position in [1, 2, 3]:
            align_y = 1.0
            rel1_y = -1.0
            rel2_y = 0.0
        elif position in [14, 15, 16]:
            align_y = 1.0
        elif position in [6, 7, 8]:
            align_y = 0.0
            rel1_y = 1.0
            rel2_y = 2.0
        elif position in [9, 10, 11]:
            align_y = 0.0

        state.align = (align_x, align_y)
        state.rel1_relative = (rel1_x, rel1_y)
        state.rel1_offset = (ofs1_x, ofs1_y)
        state.rel2_relative = (rel2_x, rel2_y)
        state.rel2_offset = (ofs2_x, ofs2_y)
