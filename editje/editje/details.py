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

import edje
import evas
import elementary
from itertools import izip

from prop import PropertyTable
from operation import Operation


class EditjeDetails(edje.Edje):
    def __init__(self, parent, operation_stack_cb,
                 group="editje/collapsable/default"):
        if not operation_stack_cb:
            raise TypeError("You must set a callback for operation stacking on"
                            " EditjeDetails objects.")

        edje.Edje.__init__(self, parent.evas_get())
        self._parent = parent
        self._operation_stack_cb = operation_stack_cb

        self.file_set(parent.theme, group)

        self._proptable = PropertyTable(
            parent, "main", self.prop_value_changed)
        self._proptable.show()
        self.e = parent.e

        self._min_sizes_init(group)

        self._box = elementary.Box(parent)
        self._box.pack_end(self._proptable)
        self._box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._box.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        self._box.show()
        self.content_set("cl.content", self._box)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_min_set(*self._min_size_collapsed)

        self._subgroups = dict()

        self._open_load()

    def _min_sizes_init(self, group):
        self._m_save = self.size_min_calc()
        self._min_size_collapsed = self._m_save
        self.edje_get().signal_emit("cl,extra,activate", "")
        edje.message_signal_process()
        self._m_save_extra = self.size_min_calc()
        self.edje_get().signal_emit("cl,extra,deactivate", "")
        self._min_size = self._min_size_collapsed

    def min_size_expanded_toggle(self, value):
        if value:
            self._min_size_collapsed = self._m_save_extra
        else:
            self._min_size_collapsed = self._m_save

    def _size_hint_changed_cb(self, obj):
        self._min_size = self.size_min_calc()
        if self._open:
            self.size_hint_min_set(*self._min_size)

    def content_set(self, part, obj):
        obj.on_changed_size_hints_add(self._size_hint_changed_cb)
        self.part_swallow(part, obj)
        self._min_size = self.size_min_calc()

    def edje_get(self):
        return self

    def group_add(self, name):
        if name in self._subgroups:
            raise KeyError(name)

        tbl = PropertyTable(self._parent, name, self.prop_value_changed)
        tbl.show()
        frm = elementary.Layout(self._parent)

        frm.file_set(self._parent.theme, "subgroup")
        frm.edje_get().part_text_set("title.label", name)
        frm.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        frm.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        frm.content_set("content", tbl)
        frm.show()
        self._box.pack_end(frm)

        grp = dict()
        grp["table"] = tbl
        grp["frame"] = frm
        self._subgroups[name] = grp

    def group_title_set(self, name, title):
        if not name in self._subgroups:
            raise KeyError(name)

        self._subgroups[name]["frame"].edje_get().part_text_set("title.label",
                                                                title)

    def group_show(self, name):
        if not name in self._subgroups:
            raise KeyError(name)
        grp = self._subgroups[name]["frame"]
        self._box.pack_end(grp)
        grp.show()

    def group_hide(self, name):
        if not name in self._subgroups:
            raise KeyError(name)
        grp = self._subgroups[name]["frame"]
        self._box.unpack(grp)
        grp.hide()

    def main_hide(self):
        self._box.unpack(self._proptable)
        self._proptable.hide()

    def main_show(self):
        self._box.pack_end(self._proptable)
        self._proptable.show()

    def prop_value_changed(self, prop, value, group):
        pass

    def __getitem__(self, key):
        if key == "main":
            return self._proptable
        elif key in self._subgroups:
            return self._subgroups[key]["table"]
        raise KeyError(key)

    # Title
    def _title_set(self, value):
        self.part_text_set("cl.header.title", value)

    def _title_get(self):
        return self.part_text_get("cl.header.title")

    title = property(_title_get, _title_set)

    #  Open / Close
    def _open_load(self):
        self.signal_callback_add("cl,opened", "editje/collapsable",
                                 self._opened_cb)
        self.signal_callback_add("cl,closed", "editje/collapsable",
                                 self._closed_cb)

        self._open = False
        self._open_disable = True
        self.open = False

    def _open_set(self, value):
        if self._open_disable:
            return
        self.open_set(value)

    def open_set(self, value):
        if value:
            self.signal_emit("cl,open", "")
        else:
            self.signal_emit("cl,close", "")

    def _opened_cb(self, obj, emission, source):
        self._open = True
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.size_hint_min_set(*self._min_size)
        self.calc_force()

    def _closed_cb(self, obj, emission, source):
        self._open = False
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self.size_hint_min_set(*self._min_size_collapsed)
        self.calc_force()

    def _open_get(self):
        return self._open

    open = property(_open_get, _open_set)

    def _open_disable_set(self, value):
        if value:
            self.edje_get().signal_emit("cl,disable", "")
        else:
            self.edje_get().signal_emit("cl,enable", "")
        self._open_disable = value

    def _open_disable_get(self, value):
        return self._open_disable

    open_disable = property(_open_disable_get, _open_disable_set)

    # most general form, specialise if needed
    def _context_recall(self, **kargs):
        self.e.part.name = kargs["part"]
        self.e.part.state.name = kargs["state"]

    # most general form, specialise if needed
    def _prop_old_values_get(self, prop_attrs, is_external):
        old_values = []
        obj = self._prop_object_get()

        for i, p in enumerate(prop_attrs):
            old_values.append(getattr(obj, p))

        return old_values

    def _prop_change_do(self, op_name, prop_groups, prop_names, prop_values,
                        prop_attrs, is_external, filters):

        def set_property(part_name, state_name, anim_name, frame, sig_name,
                         prop_attrs, prop_names, prop_values, is_external,
                         filter_, reverse=False):

            if reverse:
                efunc = lambda l: izip(xrange(len(l) - 1, -1, -1), reversed(l))
            else:
                efunc = enumerate

            self._context_recall(
                part=part_name, state=state_name, animation=anim_name,
                time=frame, signal=sig_name)

            for i, p in efunc(prop_attrs):
                if is_external[i]:
                    if not self.e.part.state.external_param_set(prop_attrs[i],
                            prop_values[i]):
                        return i
                else:
                    obj = self._prop_object_get()
                    setattr(obj, prop_attrs[i], prop_values[i])

                if filter_[i]:
                    label_value = filter_[i](prop_values[i])
                else:
                    label_value = prop_values[i]
                if self[prop_groups[i]][prop_names[i]].value != label_value:
                    self[prop_groups[i]][prop_names[i]].value = label_value
            return True

        if not self._prop_object_get:
            raise NotImplementedError(
                "One must implement self._prop_object_get for"
                " EditjeDetails children classes.")

        l = len(prop_groups)
        for arg in (prop_names, prop_values, prop_attrs,
                    is_external, filters):
            if len(arg) != l:
                raise TypeError("Cardinality of property fields differ.")

        part_name = self.e.part.name
        state_name = self.e.part.state.name

        # animations' only
        anim_name = self.e.animation.name
        frame = self.e.animation.state

        # signals' only
        sig_name = self.e.signal.name

        old_values = []
        for i, p in enumerate(prop_attrs):
            if not p:
                prop_attrs[i] = prop_names[i]

        old_values = self._prop_old_values_get(prop_attrs, is_external)

        is_valid = set_property(part_name, state_name, anim_name, frame,
                                sig_name, prop_attrs, prop_names, prop_values,
                                is_external, filters)

        if is_valid is not True:
            self["external"][prop_attrs[is_valid]].value = old_values[is_valid]
            return

        op = Operation(op_name)
        op.redo_callback_add(
            set_property, part_name, state_name, anim_name, frame, sig_name,
            prop_attrs, prop_names, prop_values, is_external, filters)
        op.undo_callback_add(
            set_property, part_name, state_name, anim_name, frame, sig_name,
            prop_attrs, prop_names, old_values, is_external, filters, True)
        self._operation_stack_cb(op)

        return op
