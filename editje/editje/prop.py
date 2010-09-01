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

import elementary


class ValueBox(elementary.Box):
    def __init__(self, parent):
        elementary.Box.__init__(self, parent)
        self.horizontal_set(True)

        self._values_list = []
        self._values_dict = dict()

    def add(self, name, widget):
        if name in self._values_dict:
            raise KeyError(name)
        self._values_list.append(widget)
        self._values_dict[name] = widget
        self.pack_end(widget.obj)

    def _value_get(self):
        if len(self._values_list) == 1:
            return self._values_list[0].value
        ret = []
        for w in self._values_list:
            ret.append(w.value)
        return ret

    def _value_set(self, value):
        if len(self._values_list) == 1:
            self._values_list[0].value = value
            return
        elif type(value) != tuple and type(value) != list:
            raise TypeError("Type of value data to be set (%s) mismatch the"
                            "expected ones, which are tuples or lists of "
                            "cardinality (%d)" % \
                                (type(value), len(self._values_list)))
        if len(value) != len(self._values_list):
            raise IndexError("Cardinality of values to be set (%d) mismatch"
                             " the cardinality of widgets in the box (%d)" % \
                                 (len(value), len(self._values_list)))
        for i, w in enumerate(self._values_list):
            w.value = value[i]

    value = property(_value_get, _value_set)

    def __len__(self):
        return len(self._values_list)

    def __getitem__(self, key):
        wid = None
        if type(key) is int:
            if key < 0 or key >= len(self._values_list):
                raise IndexError(key)
            wid = self._values_list[key]
        elif type(key) is str:
            if key not in self._values_dict:
                raise KeyError(key)
            wid = self._values_dict[key]
        else:
            raise TypeError(key)

        return wid.value

    def __setitem__(self, key, value):
        wid = None
        if type(key) is int:
            if key < 0 or key >= len(self._values_list):
                raise IndexError(key)
            wid = self._values_list[key]
        elif type(key) is str:
            if key not in self._values_dict:
                raise KeyError(key)
            wid = self._values_dict[key]
        else:
            raise TypeError(key)

        wid.value = value

    def __contains__(self, key):
        return key in self._values_dict


class Property(object):
    def __init__(self, parent, name):
        self._name = name
        self._parent = parent

        lbl = elementary.Layout(self._parent)
        lbl.file_set(self._parent.theme, "prop_label")
        ed = lbl.edje_get()

        ed.part_text_set("label", self._name.replace("_", " ") + ":")
        self._label_obj = lbl
        self._label_obj.size_hint_weight_set(0.0, 0.0)
        self._label_obj.size_hint_align_set(1.0, 0.5)
        self._label_obj.size_hint_min_set(*ed.size_min_calc())

        self._value_obj = ValueBox(self._parent)
        self._value_obj.size_hint_weight_set(1.0, 0.0)
        self._value_obj.size_hint_align_set(-1.0, -1.0)

        self._change_notifier_cb = None
        self._change_notifier_data = None

    def _name_get(self):
        return self._name

    name = property(_name_get)

    def show_label(self):
        self._label_obj.show()

    def show_value(self):
        self._value_obj.show()

    def show(self):
        self.show_label()
        self.show_value()

    def hide_label(self):
        self._label_obj.hide()

    def hide_value(self):
        self._value_obj.hide()

    def hide(self):
        self.hide_label()
        self.hide_value()

    def _label_obj_get(self):
        return self._label_obj

    label_obj = property(_label_obj_get)

    def _value_obj_get(self):
        return self._value_obj

    value_obj = property(_value_obj_get)

    def _value_get(self):
        return self._value_obj.value

    def _value_set(self, val):
        self._value_obj.value = val

    value = property(_value_get, _value_set)

    def _changed_cb(self, obj, *args, **kwargs):
        if self._change_notifier_cb:
            self._change_notifier_cb(self, self._change_notifier_data)

    def widget_add(self, name, wid):
        self._value_obj.add(name, wid)
        wid.changed = (self._changed_cb, self)

    def destroy(self):
        self._label_obj.delete()
        self._value_obj.delete()

    def __getitem__(self, key):
        if key in self._value_obj:
            return self._value_obj[key]
        raise KeyError(key)


class PropertyTable(elementary.Table):
    def __init__(self, parent, group_name="", value_changed_cb=None):
        elementary.Table.__init__(self, parent)

        self._value_changed_cb = value_changed_cb
        self._group_name = group_name

        #self.homogenous_set(True)
        self.size_hint_weight_set(1.0, 0.0)
        self.size_hint_align_set(-1.0, 0.0)

        self._props = dict()
        self._prop_list = []

    def property_add(self, prop, row=None):
        if prop.name in self._props:
            raise KeyError(prop.name)

        if row:
            return self._property_add_row(prop, row)

        row = len(self._props)
        self.pack(prop.label_obj, 0, row, 1, 1)
        self.pack(prop.value_obj, 1, row, 1, 1)
        self._props[prop.name] = prop
        prop.show()
        prop._change_notifier_cb = self._prop_changed_cb
        self._prop_list.append(prop)

    def _property_add_row(self, prop, row):
        proplist = self._prop_list[row:]

        for it in proplist:
            self.property_del(it.name)

        self.property_add(prop)
        for it in proplist:
            self.property_add(it)

    def property_del(self, key):
        if not key in self._props:
            raise KeyError(key)

        prop = self._props[key]
        self.unpack(prop.label_obj)
        self.unpack(prop.value_obj)
        del self._props[prop.name]
        prop.hide()
        prop._change_notifier_cb = None
        self._prop_list.remove(prop)
        return prop

    def clear(self):
        for o in self._props.itervalues():
            o.destroy()
        self._props = dict()

    def get(self, key):
        return self._props.get(key)

    def __contains__(self, key):
        return key in self._props

    def __getitem__(self, key):
        if key in self._props:
            return self._props[key]
        raise KeyError(key)

    def _prop_changed_cb(self, prop, group_name):
        if self._value_changed_cb:
            self._value_changed_cb(prop.name, prop.value, self._group_name)
