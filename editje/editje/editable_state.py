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

from event_manager import Manager


class EditableState(Manager):
    def __init__(self, editable_part):
        Manager.__init__(self)

        self._edit_grp = editable_part.e
        self.name = None

        self.callback_add("state.changed", self._rel1_inform)
        self.callback_add("state.changed", self._rel2_inform)

        editable_part.callback_add("part.unselected", self._reset_cb)
        editable_part.callback_add("part.changed", self._reload_cb)

        # FIXME: temporary hack to fix the state references at details_state.py
        # refactor that baby ASAP
        editable_part.callback_add("name.changed", self._reload_cb)

    def _reset_cb(self, emissor, data):
        self.name = None

    def _reload_cb(self, emissor, data):
        part_name = self._edit_grp.part.name

        if not part_name:
            self.name = None
            return

        part = self._edit_grp.part_get(part_name)
        st_name, st_val = part.state_selected_get()
        if st_name == "(null)":
            st_name = "default"

        # forcing last condition at name setter
        self._name = None
        self.name = st_name, st_val

    # Name
    def _name_set(self, st):

        def null():
            self._state = None
            self._name = None
            self._value = 0.0

        name = None
        value = None
        if type(st) is list or type(st) is tuple:
            if len(st) == 2:
                name, value = st
            else:
                name = st[0]
                value = 0.0
        elif type(st) is str:
            tmp = st.split(None, 1)
            name = tmp[0]
            if len(tmp) == 2:
                value = float(tmp[1])
            else:
                value = 0.0

        if not self._edit_grp.edje:
            null()
            return

        part_name = self._edit_grp.part.name
        if not part_name:
            null()
            return

        if not name:
            null()
            # not sure if useful, but leaving for now
            self.event_emit("state.unselected")
            return

        if self._name == name and self._value == value:
            return

        part = self._edit_grp.part_get(part_name)
        self._state = part.state_get(name, value)
        if self._state:
            self._name = name
            self._value = value

            self.event_emit("state.changed", (self._name, self._value))

    def _name_get(self):
        return self._name, self._value

    name = property(_name_get, _name_set)

    def rename(self, new_name, new_value=None):
        if not self.name or not new_name:
            return False

        if new_value is None:
            new_value = self._value

        r = self._state.name_set(new_name, new_value)
        if r:
            self.event_emit("state.renamed", (self._name, self._value))
            self._name_set((new_name, new_value))

        return r

    def copy_from(self, state, value=0.0):
        if not self.name:
            return False

        r = self._state.copy_from(state, value)
        if not r:
            return False

        self.event_emit("state.changed", (self._name, self._value))
        return True

    def _rel1_inform(self, emissor, data):
        rel1_to = self._state.rel1_to_get()
        rel1_rel = self._state.rel1_relative_get()
        rel1_ofs = self._state.rel1_offset_get()

        self.event_emit("state.rel1.changed", (rel1_to, rel1_rel, rel1_ofs))

    def _rel2_inform(self, emissor, data):
        rel2_to = self._state.rel2_to_get()
        rel2_rel = self._state.rel2_relative_get()
        rel2_ofs = self._state.rel2_offset_get()

        self.event_emit("state.rel2.changed", (rel2_to, rel2_rel, rel2_ofs))

    def _rel1_to_get(self):
        if not self.name:
            return None

        return self._state.rel1_to

    def _rel1_to_set(self, value):
        if not self.name:
            return

        self._state.rel1_to = value
        self._rel1_inform(None, None)

    rel1_to = property(fget=_rel1_to_get, fset=_rel1_to_set)

    def _rel1_relative_get(self):
        if not self.name:
            return None

        return self._state.rel1_relative

    def _rel1_relative_set(self, value):
        if not self.name:
            return

        self._state.rel1_relative = value
        self._rel1_inform(None, None)

    rel1_relative = property(fget=_rel1_relative_get, fset=_rel1_relative_set)

    def _rel1_offset_get(self):
        if not self.name:
            return None

        return self._state.rel1_offset

    def _rel1_offset_set(self, value):
        if not self.name:
            return

        self._state.rel1_offset = value
        self._rel1_inform(None, None)

    rel1_offset = property(fget=_rel1_offset_get, fset=_rel1_offset_set)

    def _rel2_to_get(self):
        if not self.name:
            return None

        return self._state.rel2_to

    def _rel2_to_set(self, value):
        if not self.name:
            return

        self._state.rel2_to = value
        self._rel2_inform(None, None)

    rel2_to = property(fget=_rel2_to_get, fset=_rel2_to_set)

    def _rel2_relative_get(self):
        if not self.name:
            return None

        return self._state.rel2_relative

    def _rel2_relative_set(self, value):
        if not self.name:
            return

        self._state.rel2_relative = value
        self._rel2_inform(None, None)

    rel2_relative = property(fget=_rel2_relative_get, fset=_rel2_relative_set)

    def _rel2_offset_get(self):
        if not self.name:
            return None

        return self._state.rel2_offset

    def _rel2_offset_set(self, value):
        if not self.name:
            return

        self._state.rel2_offset = value
        self._rel2_inform(None, None)

    rel2_offset = property(fget=_rel2_offset_get, fset=_rel2_offset_set)

    def _max_get(self):
        if not self.name:
            return None

        return self._state.max

    def _max_set(self, value):
        if not self.name:
            return

        if self._state.max == value:
            return

        min_ = self.min
        w, h = value
        if w < 0:
            w = -1
        elif min_[0] and w < min_[0]:
            w = min_[0]
        if h < 0:
            h = -1
        elif min_[1] and h < min_[1]:
            h = min_[1]
        self._state.max = (w, h)
        self.event_emit("part.state.max.changed", (w, h))

    max = property(_max_get, _max_set)

    def _min_get(self):
        if not self.name:
            return None

        return self._state.min

    def _min_set(self, value):
        if not self.name:
            return

        if self._state.min == value:
            return

        max_ = self._state.max
        w, h = value
        if w < 0:
            w = 0
        elif max_[0] >= 0 and w > max_[0]:
            w = max_[0]
        if h < 0:
            h = 0
        elif max_[1] >= 0 and h > max_[1]:
            h = max_[1]
        self._state.min = (w, h)
        self.event_emit("part.state.min.changed", (w, h))

    min = property(_min_get, _min_set)

    def _color_get(self):
        if not self.name:
            return None

        return self._state.color

    def _color_set(self, value):
        if not self.name:
            return

        self._state.color = value
        self.event_emit("state.color.changed")

    color = property(fget=_color_get, fset=_color_set)

    def _color2_get(self):
        if not self.name:
            return None

        return self._state.color2

    def _color2_set(self, value):
        if not self.name:
            return

        self._state.color2 = value
        self.event_emit("state.color2.changed")

    color2 = property(fget=_color2_get, fset=_color2_set)

    def _color3_get(self):
        if not self.name:
            return None

        return self._state.color3

    def _color3_set(self, value):
        if not self.name:
            return

        self._state.color3 = value
        self.event_emit("state.color3.changed")

    color3 = property(fget=_color3_get, fset=_color3_set)

    def _visible_get(self):
        if not self.name:
            return None

        return self._state.visible

    def _visible_set(self, value):
        if not self.name:
            return

        self._state.visible = value
        self.event_emit("state.visible.changed")

    visible = property(fget=_visible_get, fset=_visible_set)

    def _align_get(self):
        if not self.name:
            return None

        return self._state.align

    def _align_set(self, value):
        if not self.name:
            return

        self._state.align = value
        self.event_emit("state.align.changed")

    align = property(fget=_align_get, fset=_align_set)

    def _text_get(self):
        if not self.name:
            return None

        return self._state.text

    def _text_set(self, value):
        if not self.name:
            return

        self._state.text = value
        self.event_emit("state.text.changed")

    text = property(fget=_text_get, fset=_text_set)

    def _font_get(self):
        if not self.name:
            return None

        return self._state.font

    def _font_set(self, value):
        if not self.name:
            return

        self._state.font = value
        self.event_emit("state.font.changed")

    font = property(fget=_font_get, fset=_font_set)

    def _text_size_get(self):
        if not self.name:
            return None

        return self._state.text_size

    def _text_size_set(self, value):
        if not self.name:
            return

        self._state.text_size = value
        self.event_emit("state.text_size.changed")

    text_size = property(fget=_text_size_get, fset=_text_size_set)

    def _text_align_get(self):
        if not self.name:
            return None

        return self._state.text_align

    def _text_align_set(self, value):
        if not self.name:
            return

        self._state.text_align = value
        self.event_emit("state.text_align.changed")

    text_align = property(fget=_text_align_get, fset=_text_align_set)

    def _text_elipsis_get(self):
        if not self.name:
            return None

        return self._state.text_elipsis

    def _text_elipsis_set(self, value):
        if not self.name:
            return

        self._state.text_elipsis = value
        self.event_emit("state.text_elipsis.changed")

    text_elipsis = property(fget=_text_elipsis_get, fset=_text_elipsis_set)

    def _text_fit_get(self):
        if not self.name:
            return None

        return self._state.text_fit

    def _text_fit_set(self, value):
        if not self.name:
            return

        self._state.text_fit = value
        self.event_emit("state.text_fit.changed")

    text_fit = property(fget=_text_fit_get, fset=_text_fit_set)

    def _image_get(self):
        if not self.name:
            return None

        return self._state.image

    def _image_set(self, value):
        if not self.name:
            return

        self._state.image = value
        self.event_emit("state.image.changed")

    image = property(fget=_image_get, fset=_image_set)

    def _image_border_get(self):
        if not self.name:
            return None

        return self._state.image_border

    def _image_border_set(self, value):
        if not self.name:
            return

        self._state.image_border = value
        self.event_emit("state.image_border.changed")

    image_border = property(fget=_image_border_get, fset=_image_border_set)

    def _image_border_fill_get(self):
        if not self.name:
            return None

        return self._state.image_border_fill

    def _image_border_fill_set(self, value):
        if not self.name:
            return

        self._state.image_border_fill = value
        self.event_emit("state.image_border_fill.changed")

    image_border_fill = property(fget=_image_border_fill_get,
                                 fset=_image_border_fill_set)

    def external_param_set(self, pname, value):
        if not self.name:
            return False

        return self._state.external_param_set(pname, value)

    def external_param_get(self, pname):
        if not self.name:
            return None

        return self._state.external_param_get(pname)
