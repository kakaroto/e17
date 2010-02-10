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
import ecore
import edje
import elementary
import re

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_boolean import WidgetBoolean
from details_widget_color import WidgetColor
from details_widget_button import WidgetButton
from details_widget_partlist import WidgetPartList
from details_widget_font import WidgetFont
from floater import Floater
from prop import Property, PropertyTable
import popups


class StatesPopUp(Floater):
    min_w = 200
    min_h = 300

    def __init__(self, parent, rel_to_obj=None):
        Floater.__init__(self, parent, rel_to_obj)
        self.size_min_set(self.min_w, self.min_h)

        self.title_set("States selection")

        self.states = elementary.List(parent)
        self.states.size_hint_weight_set(1.0, 1.0)
        self.states.size_hint_align_set(-1.0, -1.0)
        self.states.show()
        self._selstate = None

        self.content_set(self.states)

        self._parent.e.part.callback_add("states.changed", self._list_populate)
        self._list_populate()

    def _list_populate(self, *args):
        self.states.clear()
        for s in self._parent.e.part.states:
            self.states.item_append(s, None, None, self._state_selected_cb, s)

        self.states.go()

    def _state_selected_cb(self, obj, event, state):
        if self._selstate != state:
            self._selstate = state
            self._parent.e.part.state.name = state
        self.hide()

    def selected_get(self):
        return self._selstate

    def close(self):
        self._parent.e.part.callback_del("states.changed",
                                         self._list_populate)
        Floater.close(self)


class PartStateDetails(EditjeDetails):
    state_pop_min_w = 200
    state_pop_min_h = 300

    def __init__(self, parent, anim=False):
        if anim:
            self._anim_init(parent)
        else:
            self._edit_init(parent)

        self._animmode = anim
        self._update_schedule = None
        self.on_del_add(self._del_handler)

        self._external_type = None

        self._common_props_create()
        self._rel_props_create()
        self._image_props_create()
        self._text_props_create()
        self._gradient_props_create()
        self._external_props_create()

        self.edje_get().signal_emit("cl,option,enable", "editje")

        self.e.callback_add("group.changed", self._edje_load)
        self.e.part.callback_add("part.changed", self._part_update)
        self.e.part.callback_add("part.renamed", self._part_update)
        self.e.part.callback_add("part.unselected", self._part_removed)
        self.e.part.state.callback_add("state.changed", self._state_changed_cb)
        self.e.part.state.callback_add(
            "rel1x.changed", self._state_rels_changed_cb)
        self.e.part.state.callback_add(
            "rel1y.changed", self._state_rels_changed_cb)
        self.e.part.state.callback_add(
            "rel2x.changed", self._state_rels_changed_cb)
        self.e.part.state.callback_add(
            "rel2y.changed", self._state_rels_changed_cb)
        self.e.part.state.callback_add(
            "part.state.min.changed", self._update_min)
        self.e.part.state.callback_add(
            "part.state.max.changed", self._update_max)
        self._hide_all()

    def _del_handler(self, o):
        if self._update_schedule is not None:
            self._update_schedule.delete()
            self._update_schedule = None

    def _edit_init(self, parent):
        EditjeDetails.__init__(self, parent,
                               group="editje/collapsable/part_state")

        self.title_set("part state")

        self._header_table = PropertyTable(parent)

        prop = Property(parent, "state")
        wid = WidgetEntry(self)
        prop.widget_add("s", wid)
        self._header_table.property_add(prop)
        wid.changed = self._state_entry_changed_cb

        self.content_set("part_state.swallow", self._header_table)

        self.edje_get().signal_callback_add("cl,option,clicked",
                                            "editje/collapsable",
                                            self._edit_opt_clicked_cb)

    def _anim_init(self, parent):
        EditjeDetails.__init__(self, parent,
                               group="editje/collapsable/part_properties")

        self.title_set("part properties")

        self._header_table = PropertyTable(parent)

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

        self.content_set("part_name.swallow", self._header_table)

        self.edje_get().signal_callback_add("cl,option,clicked",
                                            "editje/collapsable",
                                            self._anim_opt_clicked_cb)

    def _edit_opt_clicked_cb(self, obj, emission, source):
        icon = self.edje_get().part_object_get("cl.options")
        popup = StatesPopUp(self._parent, icon)
        popup.action_add("New", self._state_add_new_cb)
        popup.action_add("Reset to", self._reset_to_state_cb)
        popup.action_add("Delete", self._state_delete_cb)
        popup.action_add("Cancel", self._popup_cancel_cb)
        popup.show()

    def _anim_opt_clicked_cb(self, obj, emission, source):
        icon = self.edje_get().part_object_get("cl.options")
        popup = StatesPopUp(self._parent, icon)
        popup.action_add("Reset to", self._reset_to_state_cb)
        popup.action_add("Cancel", self._popup_cancel_cb)
        popup.show()

    def _state_selected_cb(self, popup, data):
        self.e.part.state.name = popup.selected_get()
        popup.close()

    def _state_add_new_cb(self, popup, data):
        max = 0
        cur_state = self.e.part.state.name.split(None,1)
        if re.match("[a-zA-Z]*\d{2,}", cur_state[0]):
            cur = cur_state[0][:-2]
        else:
            cur = cur_state[0]

        for p in self.e.part.states:
             state = p.split(None, 1)
             if re.match("%s\d{2,}" % cur, state[0]):
                 num = int(state[0][len(cur):])
                 if num > max:
                       max = num
        nst = cur + "%.2d" % (max + 1)
        st = nst + " 0.00"
        if not self.part.state_exist(st):
            self.part.state_copy(self.state.name, nst)
            self.e.part.event_emit("state.added", st)
            self.e.part.state.name = st


    def _state_delete_cb(self, popup, data):
        st = popup.selected_get()
        if st == "default 0.00":
            return
        print "DELETE STATE", st
        self.e.part.state_del(st)

    def _reset_to_state_cb(self, popup, data):
        self.state.copy_from(popup.selected_get())
        print "COPIED",popup.selected_get(),"TO",self.state.name
        self.e.part.state.event_emit("state.changed", self.state.name)
        popup.close()

    def _popup_cancel_cb(self, popup, data):
        popup.close()

    def _edje_load(self, emissor, data):
        self.editable = self.e.edje

    def _part_update(self, emissor, data):
        self.part = self.e.part._part
        state = self.part.state_selected_get()
        if self._animmode:
            self._header_table["name"].value = self.part.name
            self._header_table["type"].value = \
                self._part_type_to_text(self.part.type)
        else:
            if state == "(null) 0.00":
                state = "default 0.00"
            #self.state_prop[1].clear()
            #for st in part.states:
            #    print st
            #    self.state_prop[1].item_add(st)
            self._header_table["state"].value = state
        self.state = self.part.state_get(state)
        self._update()
        self.open()

    def _part_removed(self, emissor, data):
        if not self.e.part:
            return

        if self._animmode:
            self._header_table["name"].value = None
            self._header_table["type"].value = None
        else:
            self._header_table["state"].value = None
        self._hide_all()

    def _part_type_to_text(self, type):
        parttypes = ['NONE', 'RECTANGLE', 'TEXT', 'IMAGE', 'SWALLOW',
                     'TEXTBLOCK', 'GRADIENT', 'GROUP', 'BOX', 'TABLE',
                     'EXTERNAL']
        return parttypes[type]

    def _common_props_create(self):
        prop = Property(self._parent, "min")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        prop = Property(self._parent, "max")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        # 'step' not implemented in edje_edit

        prop = Property(self._parent, "current")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("w", wid)
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("h", wid)
        self["main"].property_add(prop)

        prop = Property(self._parent, "visible")
        prop.widget_add("v", WidgetBoolean(self))
        self["main"].property_add(prop)

        prop = Property(self._parent, "align")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["main"].property_add(prop)

        # 'fixed' not implemented in edje_edit
        # 'aspect' and 'aspect_preference' missing

        prop = Property(self._parent, "color")
        prop.widget_add("c", WidgetColor(self))
        self["main"].property_add(prop)

    def _text_props_create(self):
        self.group_add("text")
        # missing: text_class
        # not implemented in edje_edit: min, max

        prop = Property(self._parent, "text")
        prop.widget_add("t", WidgetEntry(self))
        self["text"].property_add(prop)

        prop = Property(self._parent, "font")
        prop.widget_add("f", WidgetFont(self))
        self["text"].property_add(prop)

        prop = Property(self._parent, "size")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
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
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "elipsis")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("e", wid)
        self["text"].property_add(prop)

        prop = Property(self._parent, "color2")
        prop.widget_add("c", WidgetColor(self))
        self["text"].property_add(prop)

        prop = Property(self._parent, "color3")
        prop.widget_add("c", WidgetColor(self))
        self["text"].property_add(prop)

    def _gradient_props_create(self):
        self.group_add("gradient")

        prop = Property(self._parent, "type")
        prop.widget_add("t", WidgetEntry(self))
        self["gradient"].property_add(prop)

        prop = Property(self._parent, "spectrum")
        prop.widget_add("s", WidgetEntry(self))
        self["gradient"].property_add(prop)

        prop = Property(self._parent, "angle")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("a", wid)
        self["gradient"].property_add(prop)

        self.group_add("g_rel1")
        self.group_title_set("g_rel1", "gradient top-left")

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["g_rel1"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("y", wid)
        self["g_rel1"].property_add(prop)

        self.group_add("g_rel2")
        self.group_title_set("g_rel2", "gradient bottom-right")

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["g_rel2"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("y", wid)
        self["g_rel2"].property_add(prop)

    def _image_props_create(self):
        self.group_add("image")

        prop = Property(self._parent, "normal")
        wid = WidgetButton(self)
        wid.clicked = self._image_btn_clicked
        prop.widget_add("n", wid)
        self["image"].property_add(prop)

        prop = Property(self._parent, "border")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("l", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("r", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("t", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("b", wid)
        self["image"].property_add(prop)

        prop = Property(self._parent, "middle")
        wid = WidgetBoolean(self)
        wid.states_set("Solid", "None")
        prop.widget_add("m", wid)
        self["image"].property_add(prop)
#        wid = WidgetBoolean(self)
#        wid.states_set("Dynamic", "Static")
#        self.prop_add("scale_hint", wid)

    def _rel_props_create(self):
        self.group_add("rel1")
        self.group_title_set("rel1", "top-left")

        prop = Property(self._parent, "to")
        prop.widget_add("x", WidgetPartList(self))
        prop.widget_add("y", WidgetPartList(self))
        self["rel1"].property_add(prop)

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["rel1"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("y", wid)
        self["rel1"].property_add(prop)

        self.group_add("rel2")
        self.group_title_set("rel2", "bottom-right")

        prop = Property(self._parent, "to")
        prop.widget_add("x", WidgetPartList(self))
        prop.widget_add("y", WidgetPartList(self))
        self["rel2"].property_add(prop)

        prop = Property(self._parent, "relative")
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("y", wid)
        self["rel2"].property_add(prop)

        prop = Property(self._parent, "offset")
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("x", wid)
        wid = WidgetEntry(self)
        wid.type_int()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: int(x)
        prop.widget_add("y", wid)
        self["rel2"].property_add(prop)

    def _external_props_create(self):
        self.group_add("external")

    def _image_btn_clicked(self, *args):
        popups.ImagePopUp(self._parent,
                          select_cb=self._image_selected_cb).show()

    def _image_selected_cb(self, image):
        self["image"]["normal"].value = image
        # HACK until the image properly emits a change
        self.state.image_set(image)

    def _state_changed_cb(self, emissor, data):
        if not data:
            return
        self.part.state_selected_set(data)
        self.state = self.e.part.state._state
        if not self._animmode:
            self._header_table["state"].value = data
        self._update()

    def _state_rels_changed_cb(self, emissor, data):
        self._update_common()

    def _state_entry_changed_cb(self, obj, *args, **kwargs):
        state = obj.value.split(None, 1)
        old = self.e.part.state.name

        if len(state) == 1:
            st = state[0] + " 0.00"
        else:
            st = " ".join(state)

        if (not self.part.state_exist(st)) and old.split(None, 1)[0] != "default":
            self.e.part.state.name_set(st)
        else:
            obj.value = old


    def _hide_all(self):
        self.main_hide()
        self.group_hide("rel1")
        self.group_hide("rel2")
        self.group_hide("text")
        self.group_hide("image")
        self.group_hide("gradient")
        self.group_hide("g_rel1")
        self.group_hide("g_rel2")
        self.group_hide("external")

    def _update(self):
        if self._update_schedule is not None:
            self._update_schedule.delete()
        self._update_schedule = ecore.idler_add(self._update_do)

    def _update_do(self):
        self._update_schedule = None
        self._hide_all()
        self._update_common()
        self.main_show()
        self.group_show("rel1")
        self.group_show("rel2")
        if self.part.type == edje.EDJE_PART_TYPE_TEXT:
            self._update_text()
            self.group_show("text")
        elif self.part.type == edje.EDJE_PART_TYPE_IMAGE:
            self._update_image()
            self.group_show("image")
        elif self.part.type == edje.EDJE_PART_TYPE_GRADIENT:
            self._update_gradient()
            self.group_show("gradient")
            self.group_show("g_rel1")
            self.group_show("g_rel2")
        elif self.part.type == edje.EDJE_PART_TYPE_EXTERNAL:
            self._update_external()
            self.group_show("external")
        return False

    def _update_min(self, emissor, data):
        self["main"]["min"].value = data

    def _update_max(self, emissor, data):
        self["main"]["max"].value = data

    def _update_common(self):
        self._update_min(self, self.e.part.state.min)
        self._update_max(self, self.e.part.state.max)

        (x, y, w, h) = self.editable.part_geometry_get(self.part.name)
        self["main"]["current"].value = w, h

        self["main"]["align"].value = self.state.align_get()
        self["main"]["color"].value = self.state.color_get()
        self["main"]["visible"].value = self.state.visible

        x, y = self.state.rel1_to_get()
        if x is None:
            x = ""
        if y is None:
            y = ""
        self["rel1"]["to"].value = x, y
        self["rel1"]["relative"].value = self.state.rel1_relative_get()
        self["rel1"]["offset"].value = self.state.rel1_offset_get()

        x, y = self.state.rel2_to_get()
        if x is None:
            x = ""
        if y is None:
            y = ""
        self["rel2"]["to"].value = x, y
        self["rel2"]["relative"].value = self.state.rel2_relative_get()
        self["rel2"]["offset"].value = self.state.rel2_offset_get()

    def _update_text(self):
        t = self.state.text_get()
        if t is None:
            t = ""
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
        if img is None:
            img = ""
        self["image"]["normal"].value = img
        self["image"]["border"].value = self.state.image_border_get()
        self["image"]["middle"].value = self.state.image_border_fill_get()

    def _update_gradient(self):
        t = self.state.gradient_type_get()
        if t is None:
            t = ""
        self["gradient"]["type"].value = t
        s = self.state.gradient_spectra_get()
        if s is None:
            s = ""
        self["gradient"]["spectrum"].value = s
        self["gradient"]["angle"].value = str(self.state.gradient_angle_get())

        self["g_rel1"]["relative"].value = self.state.gradient_rel1_relative_get()
        self["g_rel1"]["offset"].value = self.state.gradient_rel1_offset_get()

        self["g_rel2"]["relative"].value = self.state.gradient_rel2_relative_get()
        self["g_rel2"]["offset"].value = self.state.gradient_rel2_offset_get()

    def _create_props_by_type(self, type):
        self._params_info = edje.external_param_info_get(type)
        for p in self._params_info:
            prop = Property(self._parent, p.name)
            if p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_BOOL:
                wid = WidgetBoolean(self)
                wid.states_set(p.true_string, p.false_string)
            else:
                wid = WidgetEntry(self)
                if p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_INT:
                    wid.type_int()
                elif p.type == edje.EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
                    wid.type_float()
            prop.widget_add("v", wid)
            self["external"].property_add(prop)

    def _update_external(self):
        t = self.part.source
        if t != self._external_type:
            self._external_type = t
            self["external"].clear()
            self._create_props_by_type(t)
        for p in self.state.external_params_get():
            self["external"][p.name].value = p.value

    def prop_value_changed(self, prop, value, group):
        if not group:
            self._prop_common_value_changed(prop, value)
        elif group == "rel1":
            self._prop_rel1_value_changed(prop, value)
        elif group == "rel2":
            self._prop_rel2_value_changed(prop, value)
        elif group == "text":
            self._prop_text_value_changed(prop, value)
        elif group == "image":
            self._prop_image_value_changed(prop, value)
        elif group == "gradient" or \
             group == "g_rel1" or \
             group == "g_rel2":
            self._prop_gradient_value_changed(prop, value, group)
        elif group == "external":
            self._prop_external_value_changed(prop, value)

        self.editable.calc_force()

    def _prop_common_value_changed(self, prop, value):
        tbl = self["main"]
        if prop == "min":
            if value is not None:
                self.e.part.state.min = value
        elif prop == "max":
            if value is not None:
                self.e.part.state.max = value
        elif prop == "color":
            self.state.color_set(*value)
        elif prop == "visible":
            self.state.visible = value
        elif prop == "align":
            if value is not None:
                self.state.align_set(*value)
            tbl["align"].value = self.state.align_get()

    def _prop_rel1_value_changed(self, prop, value):
        tbl = self["rel1"]
        if prop == "to":
            self.state.rel1_to_set(*value)
            (x, y) = self.state.rel1_to_get()
            if x is None:
                x = ""
            if y is None:
                y = ""
            tbl["to"].value = x, y
        elif prop == "offset":
            if value is not None:
                self.state.rel1_offset_set(*value)
            tbl["offset"].value = self.state.rel1_offset_get()
        elif prop == "relative":
            if value is not None:
                self.state.rel1_relative_set(*value)
            tbl["relative"].value = self.state.rel1_relative_get()

    def _prop_rel2_value_changed(self, prop, value):
        tbl = self["rel2"]
        if prop == "to":
            self.state.rel2_to_set(*value)
            (x, y) = self.state.rel2_to_get()
            if x is None:
                x = ""
            if y is None:
                y = ""
            tbl["to"].value = x, y
        elif prop == "offset":
            if value is not None:
                self.state.rel2_offset_set(*value)
            tbl["offset"].value = self.state.rel2_offset_get()
        elif prop == "relative":
            if value is not None:
                self.state.rel2_relative_set(*value)
            tbl["relative"].value = self.state.rel2_relative_get()

    def _prop_text_value_changed(self, prop, value):
        tbl = self["text"]
        if prop == "text":
            self.state.text_set(value)
        elif prop == "font":
            self.state.font_set(value)
        elif prop == "size":
            if value is not None:
                self.state.text_size_set(value)
            tbl["size"].value = str(self.state.text_size_get())
        elif prop == "fit":
            self.state.text_fit_set(*value)
        elif prop == "align":
            if value is not None:
                self.state.text_align_set(*value)
            tbl["align"].value = self.state.text_align_get()
        elif prop == "color2":
            self.state.color2_set(*value)
        elif prop == "color3":
            self.state.color3_set(*value)
        elif prop == "elipsis":
            if value is not None:
                self.state.text_elipsis_set(value)
            tbl["elipsis"].value = str(self.state.text_elipsis_get())

    def _prop_image_value_changed(self, prop, value):
        if prop == "normal":
            self.state.image_set(value)
        elif prop == "border":
            self.state.image_border_set(*value)
        elif prop == "middle":
            self.state.image_border_fill_set(value)

    def _prop_gradient_value_changed(self, prop, value, group):
        if prop == "type":
            self.state.gradient_type_set(value)
        elif prop == "spectrum":
            self.state.gradient_spectra_set(value)
        elif prop == "angle":
            self.state.gradient_angle_set(value)
        elif prop == "relative":
            if group == "g_rel1":
                self.state.gradient_rel1_relative_set(*value)
            else:
                self.state.gradient_rel2_relative_set(*value)
        elif prop == "offset":
            if group == "g_rel1":
                self.state.gradient_rel1_offset_set(*value)
            else:
                self.state.gradient_rel2_offset_set(*value)

    def _prop_external_value_changed(self, prop, value):
        for p in self._params_info:
            if p.name == prop:
                if not p.validate(value):
                    nil, value = self.state.external_param_get(prop)
                    self["external"][prop].value = value
                    return
        self.state.external_param_set(prop, value)
