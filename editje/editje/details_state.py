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

import ecore
import edje

from details import EditjeDetails
from details_widget_boolean import WidgetBoolean
from details_widget_button import WidgetButton
from details_widget_color import WidgetColor
from details_widget_combo import WidgetCombo
from details_widget_entry import WidgetEntry
from details_widget_font import WidgetFont
from details_widget_button_list import WidgetButtonList
from details_widget_states import WidgetStates
from prop import Property, PropertyTable
from filewizard import ImageSelectionWizard
from floater_opener import FloaterListOpener
from operation import Operation
from misc import part_type_to_text


class StateCopyButton(FloaterListOpener):
    def __init__(self, editable):
        FloaterListOpener.__init__(self)
        self.e = editable

    def _floater_list_items_update(self):
        items_list = []
        for s in self.e.part.states:
            lbl = "%s %.2f" % s
            items_list.append((lbl, s))
        return items_list

    def _floater_title_init(self):
        self._floater.title_set("Copy from state")

    def value_set(self, value):
        self.e.part.state.copy_from(*value)


class PartStateDetails(EditjeDetails):
    state_pop_min_w = 200
    state_pop_min_h = 300

    def __init__(self, parent, operation_stack_cb, img_new_img_cb=None,
                 img_list_get_cb=None, img_id_get_cb=None,
                 fnt_new_fnt_cb=None, fnt_list_get_cb=None,
                 workfile_name_get_cb=None, part_object_get_cb=None,
                 group="editje/collapsable/part_state"):
        EditjeDetails.__init__(self, parent, operation_stack_cb, group)

        self._header_init(parent)

        self._img_new_img_cb = img_new_img_cb
        self._img_list_get_cb = img_list_get_cb
        self._img_id_get_cb = img_id_get_cb
        self._fnt_new_fnt_cb = fnt_new_fnt_cb
        self._fnt_list_get_cb = fnt_list_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb
        self._part_object_get_cb = part_object_get_cb

        self._update_schedule = None
        self.on_del_add(self._del_handler)

        self._external_type = None

        self._common_props_create()
        self._rel_props_create()
        self._image_props_create()
        self._text_props_create()
        self._external_props_create()

        self.e.callback_add("group.changed", self._edje_load)
        self.e.callback_add("part.removed", self._part_removed)
        self.e.part.callback_add("part.changed", self._part_update)
        # self.e.part.callback_add("name.changed", self._part_update)
        self.e.part.callback_add("part.unselected", self._part_unselected)
        self.e.part.state.callback_add("state.changed", self._state_changed_cb)

        for event in ["state.rel1.changed", "state.rel2.changed",
                      "state.color.changed", "state.text.changed",
                      "state.font.changed", "state.text_size.changed"]:
            self.e.part.state.callback_add(
                event, self._state_common_props_changed_cb)

        self.e.part.state.callback_add(
            "part.state.min.changed", self._update_min)
        self.e.part.state.callback_add(
            "part.state.max.changed", self._update_max)
        self._hide_all()

        self.open_disable = False
        self.open = True
        self.part_edje = None
        self.part_evas = None

    def _prop_object_get(self):
        return self.e.part.state

    def _prop_old_values_get(self, prop_attrs, is_external):
        old_values = []
        state = self._prop_object_get()

        for i, p in enumerate(prop_attrs):
            if is_external[i]:
                type_, old_value = state.external_param_get(p)
                if (type_ == edje.EDJE_EXTERNAL_PARAM_TYPE_STRING or type_ == \
                        edje.EDJE_EXTERNAL_PARAM_TYPE_CHOICE) and \
                        old_value is None:
                    old_values.append("")
                else:
                    old_values.append(old_value)
            else:
                old_values.append(getattr(state, p))

        return old_values

    def _header_init(self, parent):
        self.title = "part state"

        self._header_table = PropertyTable(parent, "state name")

        prop = Property(parent, "state")
        wid = WidgetStates(self, self.e, self._operation_stack_cb)
        wid.tooltip_set("Unique state name for part.<br>"
                        "'default 0.0' is immutable.")
        prop.widget_add("s", wid)
        self._header_table.property_add(prop)
        # FIXME: ugly and dangerous: overriding property_add's behavior on
        # widget's changed callback
        wid.changed = self._state_entry_changed_cb

        self.content_set("part_state.swallow", self._header_table)

    def _del_handler(self, o):
        if self._update_schedule is not None:
            self._update_schedule.delete()
            self._update_schedule = None

    def _edje_load(self, emissor, data):
        self.editable = self.e.edje

    def _part_update(self, emissor, data):
        if self.part_evas:
            self.part_evas.on_resize_del(self._size_changed)
        self.part_edje = self.e.part._part
        self.part_evas = self._part_object_get_cb(self.part_edje.name)
        self.part_evas.on_resize_add(self._size_changed)
        self._size_changed(self.part_evas)
        state, val = self.part_edje.state_selected_get()
        if state == "(null)":
            state = "default"
        self._header_table["state"].value = "%s %.2f" % (state, val)
        self._header_table["state"].show_value()
        self.state = self.part_edje.state_get(state, val)
        self._update()
        self.show()

    def _part_unselected(self, emissor, data):
        if not self.e.part:
            return

        if self.part_evas:
            self.part_evas.on_resize_del(self._size_changed)
            self.part_evas = None

        self._header_table["state"].value = None
        self._header_table["state"].hide_value()
        self._hide_all()
        self.hide()

    def _part_removed(self, emissor, data):
        self.part_edje = None
        self.part_evas = None

    def _common_props_create(self):
        prop = Property(self._parent, "min")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Minimum part<br>width in pixels.")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Minimum part<br>height in pixels.")
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        prop = Property(self._parent, "max")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Maximum part<br>width in pixels.<br>0 = disabled")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Maximum part<br>height in pixels.<br>0 = disabled")
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        # 'step' not implemented in edje_edit

        prop = Property(self._parent, "current")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Current part<br>height in pixels.")
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Current part<br>width in pixels.")
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        prop = Property(self._parent, "visible")
        wid = WidgetBoolean(self)
        wid.tooltip_set("Change part's visibility.")
        prop.widget_add("v", wid)
        self["main"].property_add(prop)

        prop = Property(self._parent, "align")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Part horizontal align.<br>0.0 = left  1.0 = right")
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Part vertical align.<br>0.0 = top  1.0 = bottom")
        prop.widget_add("y", wid)
        self["main"].property_add(prop)

        # 'fixed' not implemented in edje_edit
        # 'aspect' and 'aspect_preference' missing

        prop = Property(self._parent, "color")
        wid = WidgetColor(self, self.e)
        wid.tooltip_set("Part main color.<br>"
                        "Enter color in integers or<br>"
                        "in Hexa notation.",
                        "Preview of main color.<br>"
                        "Click to edit it.")
        prop.widget_add("c", wid)
        self["main"].property_add(prop)

    def _text_props_create(self):
        self.group_add("text")
        # missing: text_class
        # not implemented in edje_edit: min, max

        prop = Property(self._parent, "text")
        wid = WidgetEntry(self)
        wid.tooltip_set("Set the text of part.")
        prop.widget_add("t", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "font")
        wid = WidgetFont(self._parent, \
                         self._fnt_new_fnt_cb, self._fnt_list_get_cb, \
                         self._workfile_name_get_cb)
        wid.tooltip_set("Change the text's font.")
        prop.widget_add("f", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "size")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Change text font's size.")
        prop.widget_add("s", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "fit")
        prop.widget_add("x", WidgetBoolean(self))
        prop.widget_add("y", WidgetBoolean(self))
        self["text"].property_add(prop)

        prop = Property(self._parent, "align")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Text horizontal align.<br>0.0 = left  1.0 = right")
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Text vertical align.<br>0.0 = top  1.0 = bottom")
        prop.widget_add("y", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "elipsis")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Cut text if biggest then part's area.<br>"
                        "0.0 = fix the left side  1.0 = right side")
        prop.widget_add("e", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "color2")
        wid = WidgetColor(self, self.e)
        wid.tooltip_set("Text shadow color.<br>"
                        "Enter color in integers or<br>"
                        "in Hexa notation.",
                        "Preview of text shadow color.<br>"
                        "Click to edit it.")
        prop.widget_add("c", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "color3")
        wid = WidgetColor(self, self.e)
        wid.tooltip_set("Text outline color.<br>"
                        "Enter color in integers or<br>"
                        "in Hexa notation.",
                        "Preview of text outline color.<br>"
                        "Click to edit it.")
        prop.widget_add("c", wid)
        self["text"].property_add(prop)

    def _image_props_create(self):
        self.group_add("image")

        prop = Property(self._parent, "normal")
        wid = WidgetButton(self)
        wid.clicked = self._image_btn_clicked
        wid.tooltip_set("Actual image.<br>Click to change it.")
        prop.widget_add("n", wid)
        self["image"].property_add(prop)

        prop = Property(self._parent, "border")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Lenght of left border in pixels.<br>"
                        "Left border isn't horizontal scalable.")
        prop.widget_add("l", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Lenght of right border in pixels.<br>"
                        "Right border isn't horizontal scalable.")
        prop.widget_add("r", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Lenght of top border in pixels.<br>"
                        "Top border isn't vertical scalable.")
        prop.widget_add("t", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Lenght of bottom border in pixels.<br>"
                        "Bottom border isn't vertical scalable.")
        prop.widget_add("b", wid)
        self["image"].property_add(prop)

        prop = Property(self._parent, "middle")
        wid = WidgetBoolean(self)
        wid.states_set("Solid", "None")
        wid.tooltip_set("Enables painting of image middle.<br>"
                        "If 'None', the image middle is hidden.")
        prop.widget_add("m", wid)
        self["image"].property_add(prop)
#        wid = WidgetBoolean(self)
#        wid.states_set("Dynamic", "Static")
#        self.prop_add("scale_hint", wid)

    def _rel_props_create(self):
        rel_to_box_title = "Placement reference"

        self.group_add("rel1")
        self.group_title_set("rel1", "top-left")

        def parts_get():
            return self.e.parts

        def sel_part_get():
            return self.e.part.name

        popup_hide_cb_list = [(self.e.part, "part.unselected"),
                              (self.e.part, "part.changed")]

        prop = Property(self._parent, "to")
        wid = WidgetButtonList(self, rel_to_box_title, parts_get, sel_part_get,
                               popup_hide_cb_list)
        wid.tooltip_set("Left reference part.")
        prop.widget_add("x", wid)
        wid = WidgetButtonList(
                self, rel_to_box_title, parts_get, sel_part_get,
                popup_hide_cb_list)
        wid.tooltip_set("Top reference part.")
        prop.widget_add("y", wid)
        self["rel1"].property_add(prop)

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Left relative position to<br>"
                        "reference part.<br>"
                        "0.0 = 0%/begining  1.0 = 100%/end")
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Top relative position to<br>"
                        "reference part.<br>"
                        "0.0 = 0%/begining  1.0 = 100%/end")
        prop.widget_add("y", wid)
        self["rel1"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Left offset from relative<br>"
                        "position in pixels.")
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Top offset from relative<br>"
                        "position in pixels.")
        prop.widget_add("y", wid)
        self["rel1"].property_add(prop)

        self.group_add("rel2")
        self.group_title_set("rel2", "bottom-right")

        prop = Property(self._parent, "to")
        wid = WidgetButtonList(
                self, rel_to_box_title, parts_get, sel_part_get,
                popup_hide_cb_list)
        wid.tooltip_set("Right reference part.")
        prop.widget_add("x", wid)
        wid = WidgetButtonList(
                self, rel_to_box_title, parts_get, sel_part_get,
                popup_hide_cb_list)
        wid.tooltip_set("Bottom reference part.")
        prop.widget_add("y", wid)
        self["rel2"].property_add(prop)

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Right relative position to<br>"
                        "reference part.<br>"
                        "0.0 = 0%/begining  1.0 = 100%/end")
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Bottom relative position to<br>"
                        "reference part.<br>"
                        "0.0 = 0%/begining  1.0 = 100%/end")
        prop.widget_add("y", wid)
        self["rel2"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("x", wid)
        wid.tooltip_set("Right offset from relative<br>"
                        "position in pixels.<br>"
                        "default = -1 (see docs)")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        wid.tooltip_set("Bottom offset from relative<br>"
                        "position in pixels.<br>"
                        "default = -1 (see docs)")
        prop.widget_add("y", wid)
        self["rel2"].property_add(prop)

    def _external_props_create(self):
        self.group_add("external")

    def _image_btn_clicked(self, *args):
        ImageSelectionWizard(self._parent, self._image_selected_cb,
               self._img_new_img_cb, self._img_list_get_cb,
               self._img_id_get_cb, self._workfile_name_get_cb,).show()

    def _image_selected_cb(self, image):
        self.prop_value_changed("normal", image, "image")

    def _state_changed_cb(self, emissor, data):
        if not data:
            return
        self.part_edje.state_selected_set(*data)
        self.state = self.e.part.state._state
        prop = self._header_table.get("state")
        if prop:
            prop.value = "%s %.2f" % data
        self._update()

    def _state_common_props_changed_cb(self, emissor, data):
        self._update()

    def _state_entry_changed_cb(self, st_widget, *args, **kwargs):

        def state_rename(part_name, old_name, new_name):
            # select 1st
            if self.e.part.name != part_name:
                self.e.part.name = part_name
            self.e.part.state.name = old_name[0]

            part = self.e.part_get(part_name)

            if (part.state_exist(*new_name)) or old_name == ["default", 0.0]:
                return False

            # rename later
            return self.e.part.state.rename(*new_name)

        part = self.e.part.name
        old_name = self.e.part.state.name

        new_name = st_widget.value.split(None, 1)
        if len(new_name) == 1:
            new_name[1] = 0.0
        else:
            new_name[1] = float(new_name[1])

        if state_rename(part, old_name, new_name):
            op = Operation("state renaming")

            op.redo_callback_add(state_rename, part, old_name, new_name)
            op.undo_callback_add(state_rename, part, new_name, old_name)
            self._operation_stack_cb(op)
        else:
            # TODO: notify the user of renaming failure
            st_widget.value = "%s %.2f" % old_name

    def _hide_all(self):
        self.main_hide()
        self.group_hide("rel1")
        self.group_hide("rel2")
        self.group_hide("text")
        self.group_hide("image")
        self.group_hide("external")
        self.edje_get().signal_emit("cl,option,disable", "editje")

    def _update(self):
        if self._update_schedule is not None:
            self._update_schedule.delete()
        self._update_schedule = ecore.idler_add(self._update_do)

    def _update_do(self):
        self._update_schedule = None
        self._hide_all()

        if not self.e.part.state.name[0]:
            return

        self._update_common()

        self.main_show()
        self.group_show("rel1")
        self.group_show("rel2")
        if self.part_edje.type == edje.EDJE_PART_TYPE_TEXT:
            self._update_text()
            self.group_show("text")
        elif self.part_edje.type == edje.EDJE_PART_TYPE_IMAGE:
            self._update_image()
            self.group_show("image")
        elif self.part_edje.type == edje.EDJE_PART_TYPE_EXTERNAL:
            self._update_external()
            self.group_show("external")
        self.edje_get().signal_emit("cl,option,enable", "editje")
        return False

    def _update_min(self, emissor, data):
        self["main"]["min"].value = data

    def _update_max(self, emissor, data):
        self["main"]["max"].value = data

    def _update_common(self):
        self._update_min(self, self.e.part.state.min)
        self._update_max(self, self.e.part.state.max)

        self["main"]["align"].value = self.state.align_get()
        self["main"]["color"].value = self.state.color_get()
        self["main"]["visible"].value = self.state.visible

        x, y = self.state.rel1_to_get()
        self["rel1"]["to"].value = x, y
        self["rel1"]["relative"].value = self.state.rel1_relative_get()
        self["rel1"]["offset"].value = self.state.rel1_offset_get()

        x, y = self.state.rel2_to_get()
        self["rel2"]["to"].value = x, y
        self["rel2"]["relative"].value = self.state.rel2_relative_get()
        self["rel2"]["offset"].value = self.state.rel2_offset_get()

    def _update_text(self):
        t = self.state.text_get()
        self["text"]["text"].value = t
        f = self.state.font_get()
        if f is None:
            f = ""
        self["text"]["font"].value = f
        self["text"]["size"].value = self.state.text_size_get()
        self["text"]["fit"].value = self.state.text_fit_get()
        self["text"]["align"].value = self.state.text_align_get()
        self["text"]["color2"].value = self.state.color2_get()
        self["text"]["color3"].value = self.state.color3_get()
        self["text"]["elipsis"].value = str(self.state.text_elipsis_get())

    def _update_image(self):
        img = self.state.image_get()
        self["image"]["normal"].value = img
        self["image"]["border"].value = self.state.image_border_get()
        self["image"]["middle"].value = self.state.image_border_fill_get()

    def _create_props_by_type(self, edje_type):
        edje_type = edje.external_type_get(edje_type)
        self._params_info = edje_type.parameters_info_get()
        for p in self._params_info:
            prop = Property(self._parent, p.name)
            if p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_BOOL:
                wid = WidgetBoolean(self)
                wid.states_set(p.true_string, p.false_string)
            elif p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_CHOICE:
                wid = WidgetCombo(self._parent)
                for choice in p.choices:
                    wid.item_add(choice)
            else:
                wid = WidgetEntry(self)
                if p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_INT:
                    wid.type_int()
                elif p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
                    wid.type_float()
            prop.widget_add("v", wid)
            self["external"].property_add(prop)

    def _update_external(self):
        t = self.part_edje.source
        if t != self._external_type:
            self._external_type = t
            self["external"].clear()
            self._create_props_by_type(t)
        for p in self.state.external_params_get():
            self["external"][p.name].value = p.value

    def prop_value_changed(self, prop_name, prop_value, group_name):
        if group_name == "main":
            self._prop_common_value_changed(prop_name, prop_value)
        elif group_name == "rel1":
            self._prop_rel1_value_changed(prop_name, prop_value)
        elif group_name == "rel2":
            self._prop_rel2_value_changed(prop_name, prop_value)
        elif group_name == "text":
            self._prop_text_value_changed(prop_name, prop_value)
        elif group_name == "image":
            self._prop_image_value_changed(prop_name, prop_value)
        elif group_name == "external":
            self._prop_external_value_changed(prop_name, prop_value)

        self.editable.calc_force()

    def _prop_common_value_changed(self, prop, value):
        args = [["main"], [prop], [value], [None], [False], [None]]
        if prop == "min":
            self._prop_change_do(
                "part state mininum size setting", *args)
        elif prop == "max":
            self._prop_change_do(
                "part state maximum size setting", *args)
        elif prop == "color":
            self._prop_change_do(
                "part state color setting", *args)
        elif prop == "visible":
            self._prop_change_do(
                "part state visibility setting", *args)
        elif prop == "align":
            self._prop_change_do(
                "part state alignment setting", *args)

    def _check_state_relto_void(self):
        state = self.e.part.state
        rel1_to, rel2_to = state.rel1_to, state.rel2_to
        if (rel1_to == (None, None) and rel2_to == (None, None)):
            return True
        return False

    def _relto_fill_args(self, relto):
        return [["rel1", "rel2", "rel1", "rel2", "rel1", "rel2"],
                ["to", "to", "relative", "relative", "offset", "offset"],
                [relto, relto, [0.0, 0.0], [1.0, 1.0], [0, 0], [-1, -1]],
                ["rel1_to", "rel2_to", "rel1_relative", "rel2_relative",
                 "rel1_offset", "rel2_offset"],
                [False, False, False, False, False, False],
                [None, None, None, None, None, None]]

    def _prop_rel1_value_changed(self, prop, value):
        if prop == "to":
            if self._check_state_relto_void():
                if value[0]:
                    value = [value[0], value[0]]
                else:
                    value = [value[1], value[1]]
                args = self._relto_fill_args(value)
            else:
                args = [["rel1"], [prop], [value], ["rel1_to"], [False],
                        [None]]
            self._prop_change_do(
                "part state relative positioning setting (top-left corner's"
                " origin part)", *args)
        elif prop == "relative":
            args = [["rel1"], [prop], [value], ["rel1_relative"], [False],
                    [None]]
            self._prop_change_do(
                "part state relative positioning setting (top-left corner's"
                " relative position WRT origin's dimensions)", *args)
        elif prop == "offset":
            args = [["rel1"], [prop], [value], ["rel1_offset"], [False],
                    [None]]
            self._prop_change_do(
                "part state relative positioning setting (top-left corner's"
                " additional offset)", *args)

    def _prop_rel2_value_changed(self, prop, value):
        if prop == "to":
            if self._check_state_relto_void():
                if value[0]:
                    value = [value[0], value[0]]
                else:
                    value = [value[1], value[1]]
                args = self._relto_fill_args(value)
            else:
                args = [["rel2"], [prop], [value], ["rel2_to"], [False],
                        [None]]
            self._prop_change_do(
                "part state relative positioning setting (bottom-right"
                " corner's origin part)", *args)
        elif prop == "relative":
            args = [["rel2"], [prop], [value], ["rel2_relative"], [False],
                    [None]]
            self._prop_change_do(
                "part state relative positioning setting (bottom-right"
                " corner's relative position WRT origin's dimensions)", *args)
        elif prop == "offset":
            args = [["rel2"], [prop], [value], ["rel2_offset"], [False],
                    [None]]
            self._prop_change_do(
                "part state relative positioning setting (bottom-right"
                " corner's additional offset)", *args)

    def _prop_text_value_changed(self, prop, value):
        if prop == "text":
            args = [["text"], [prop], [value], [None], [False], [None]]
            self._prop_change_do(
                "part state text string setting", *args)
        elif prop == "font":
            args = [["text"], [prop], [value], [None], [False], [None]]
            self._prop_change_do(
                "part state text font setting", *args)
        elif prop == "size":
            args = [["text"], [prop], [value], ["text_size"], [False], [None]]
            self._prop_change_do("part state text size setting", *args)
        elif prop == "fit":
            args = [["text"], [prop], [value], ["text_fit"], [False], [None]]
            self._prop_change_do("part state text fit to given axis setting",
                                 *args)
        elif prop == "align":
            args = [["text"], [prop], [value], ["text_align"], [False], [None]]
            self._prop_change_do("part state text alignment setting", *args)
        elif prop == "color2":
            args = [["text"], [prop], [value], [None], [False], [None]]
            self._prop_change_do("part state text shadow color setting", *args)
        elif prop == "color3":
            args = [["text"], [prop], [value], [None], [False], [None]]
            self._prop_change_do("part state text outline color setting",
                                 *args)
        elif prop == "elipsis":
            args = [["text"], [prop], [value], ["text_elipsis"], [False],
                    [None]]
            self._prop_change_do("part state text elipsis (balancing) setting",
                                 *args)

    def _prop_image_value_changed(self, prop, value):
        if prop == "normal":
            # FIXME: note that undoing an image set from <nothing> to some img
            # won't reset the img part at all: edje was not really meant to
            # have "void" img parts. img part addition should raise the img
            # wizard dialog to choose an initial img from.
            args = [["image"], [prop], [value], ["image"], [False],
                    [None]]
            self._prop_change_do("part state image setting", *args)
        elif prop == "border":
            args = [["image"], [prop], [value], ["image_border"], [False],
                    [None]]
            self._prop_change_do("part state image border setting", *args)
        elif prop == "middle":
            args = [["image"], [prop], [value], ["image_border_fill"], [False],
                    [None]]
            self._prop_change_do("part state \"middle\" setting", *args)

    def _prop_external_value_changed(self, prop, value):
        for p in self._params_info:
            if p.name == prop:
                if not p.validate(value):
                    nil, old_value = self.state.external_param_get(prop)
                    self["external"][prop].value = old_value
                    return
        args = [["external"], [prop], [value], [None], [True], [None]]
        self._prop_change_do(
            "(external) part state \"%s\" setting" % prop, *args)

    def _size_changed(self, obj):
        self["main"]["current"].value = obj.size
        if self.part_edje.type == edje.EDJE_PART_TYPE_IMAGE:
            obj.fill_set(0, 0, *obj.size)


class PartAnimStateDetails(PartStateDetails):
    def __init__(
        self, parent, operation_stack_cb, img_new_img_cb=None,
        img_list_get_cb=None, img_id_get_cb=None, fnt_new_fnt_cb=None,
        fnt_list_get_cb=None, workfile_name_get_cb=None,
        part_object_get_cb=None, group="editje/collapsable/part_properties"):
        PartStateDetails.__init__(
            self, parent, operation_stack_cb, img_new_img_cb, img_list_get_cb,
            img_id_get_cb, fnt_new_fnt_cb, fnt_list_get_cb,
            workfile_name_get_cb, part_object_get_cb, group)

    def _context_recall(self, **kargs):
        self.e.animation.name = kargs["animation"]
        self.e.animation.state = kargs["time"]
        self.e.part.name = kargs["part"]
        self.e.part.state.name = kargs["state"]

    def _header_init(self, parent):
        self.title = "part frame properties"

        self._header_table = PropertyTable(parent, "part name/type")

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "type")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("t", wid)
        self._header_table.property_add(prop)

        self._source_prop = Property(parent, "source widget")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        self._source_prop.widget_add("s", wid)
        self._source_prop.hide()

        self._module_prop = Property(parent, "module")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        self._module_prop.widget_add("m", wid)
        self._module_prop.hide()

        self.content_set("part_name.swallow", self._header_table)
        self.e.animation.callback_add("animation.changed", self._anim_selected)
        self.e.animation.callback_add("animation.unselected",
                                      self._anim_unselected)
        self.e.animation.callback_add("part.added", self._anim_parts_changed)
        self.e.animation.callback_add("part.removed", self._anim_parts_changed)

        self._state_copy_button = StateCopyButton(self.e)

        self.edje_get().signal_callback_add(
            "cl,option,clicked", "editje/collapsable",
            self._state_copy_button._floater_open)

    def _show(self):
        state = self.part_edje.state_selected_get()
        self._header_table["name"].value = self.part_edje.name
        self._header_table["name"].show_value()
        self._header_table["type"].value = \
            part_type_to_text(self.part_edje.type)
        self._header_table["type"].show_value()

        if self.e.part.type == edje.EDJE_PART_TYPE_EXTERNAL:
            source = self.e.part.source
            if source.startswith("elm/"):
                source = source[4:]
                module = "Elementary"
            else:
                module = "Emotion"

            self.edje_get().signal_emit("cl,extra,activate", "")
            if "source widget" not in self._header_table:
                self._header_table.property_add(self._source_prop)
            if "module" not in self._header_table:
                self._header_table.property_add(self._module_prop)

            self._header_table["source widget"].value = source
            self._header_table["source widget"].show_value()
            self._header_table["module"].value = module
            self._header_table["module"].show_value()
        else:
            self._header_extra_hide()

        self.edje_get().signal_emit("cl,option,enable", "editje")
        self.state = self.part_edje.state_get(*state)
        self._update()
        self.show()

    def _header_extra_hide(self):
        self.edje_get().signal_emit("cl,extra,deactivate", "")
        for p in ["source widget", "module"]:
            if p in self._header_table:
                self._header_table.property_del(p)

    def _hide(self):
        self._header_table["name"].value = None
        self._header_table["name"].hide_value()
        self._header_table["type"].value = None
        self._header_table["type"].hide_value()
        self._header_extra_hide()

        self.edje_get().signal_emit("cl,option,disable", "editje")
        self._hide_all()
        self.hide()

    def _check_and_show(self):
        anim = self.e.animation
        part = self.e.part
        ret = anim.name and anim.part_belongs(part.name)
        if ret:
            self._show()
        else:
            self._hide()
        return ret

    def _part_update(self, emissor, data):
        if self.part_evas:
            self.part_evas.on_resize_del(self._size_changed)
        self.part_edje = self.e.part._part
        self.part_evas = self._part_object_get_cb(self.part_edje.name)
        self.part_evas.on_resize_add(self._size_changed)
        self._size_changed(self.part_evas)

        self._check_and_show()

    def _part_unselected(self, emissor, data):
        if self.part_evas:
            self.part_evas.on_resize_del(self._size_changed)

        self.part_evas = None
        self.part_edje = None
        self._hide()

    def _anim_selected(self, emissor, data):
        self._check_and_show()

    def _anim_unselected(self, emissor, data):
        self._hide()

    def _anim_parts_changed(self, emissor, data):
        self._check_and_show()

    def _state_changed_cb(self, emissor, data):
        if not data:
            return
        self.part_edje.state_selected_set(*data)
        self.state = self.e.part.state._state
        if self._check_and_show():
            self._update()
