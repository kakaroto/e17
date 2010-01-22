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
import os

import edje
import evas
import ecore
from elementary import InnerWindow, Layout, List, Entry, Scroller, Label, \
                       Fileselector, Button, Box, ELM_SCROLLER_POLICY_OFF

import sysconfig
from controller import Controller, View
from floater import Wizard

class PopUp(InnerWindow):
    def __init__(self, parent, group=None):
        InnerWindow.__init__(self, parent)
        self._parent = parent

        self._layout = Layout(self._parent)
        _theme_file = sysconfig.theme_file_get("default")
        _group = group or "editje/popup"
        self._layout.file_set(_theme_file, _group)
        self._layout_edje = self._layout.edje_get()
        self._layout.show()

        InnerWindow.content_set(self, self._layout)

    def __edje_get(self):
        return self._layout_edje

    layout_edje = property(__edje_get)

    def label_set(self, part, text):
        self._layout.edje_get().part_text_set(part, text)

    def content_set(self, part, content):
        self._layout.content_set(part, content)

    def open(self):
        self._parent.block(True)
        InnerWindow.show(self)

    def close(self):
        self.hide()
        self._parent.block(False)
        self.delete()

class NewFilePopUp(PopUp):
    def __init__(self, parent, group=None, select_cb=None):
        PopUp.__init__(self, parent, group or "editje/new_file")
        self.style_set("minimal")

        scr = Scroller(self)

        scr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        scr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        scr.policy_set(ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF)
        scr.bounce_set(False, False)

        self._newfile_name = Entry(self)
        self._newfile_name.single_line_set(True)
        self._newfile_name.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._newfile_name.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._newfile_name.entry_set("")
        self._newfile_name.context_menu_disabled_set(True)
        self._newfile_name.show()

        scr.content_set(self._newfile_name)
        scr.show()
        self.content_set("content", scr)

        self.label_set("header.title", "New File")
        self.label_set("filename.label", "File name: ")
        self._set_controls()

    def __entry_value_get(self):
        return self._newfile_name.entry_get()

    entry = property(__entry_value_get)

    def action_add(self, label, func_cb, data=None):
        btn = Button(self)
        btn.label_set(label)
        btn.data["clicked"] = (func_cb, data)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_min_set(100, 30)
        btn.size_hint_max_set(100, 30)
        btn.show()

        self.box.pack_end(btn)

    def _action_btn_clicked(self, bt, *args, **kwargs):
        func, udata = bt.data["clicked"]
        func(udata)

    def _set_controls(self):
        self.box = Box(self)
        self.box.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                      evas.EVAS_HINT_EXPAND)
        self.box.size_hint_align_set(evas.EVAS_HINT_FILL,
                                     evas.EVAS_HINT_FILL)
        self.box.horizontal_set(True)
        self.box.show()
        self.content_set("actions", self.box)

    def _close_cb(self, data):
        self.close()

    def close(self):
        PopUp.close(self)


class ImagePopUp(PopUp):
    def __init__(self, parent, group=None, select_cb=None):
        PopUp.__init__(self, parent, group or "editje/image_select")

        self.select_cb = select_cb
        self.fs = None
        self._preview_image_resize_timer = None
        self._actions_list = {}

        self.preview_image = self.layout_edje.evas.FilledImage()
        self.content_set("image.preview", self.preview_image)
        self.layout_edje.signal_emit("editje,preview,hide", "editje")

        area = self.layout_edje.part_object_get("image.preview.area")
        area.on_resize_add(self._resize_preview_image)

        self.label_set("header.title", "Select an image")

        self._set_controls()
        self._set_image_list()

    def _calculate_aspect_from_ratio(self, old_a):
        w, h = self.layout_edje.part_geometry_get("image.preview.area")[2:]

        if old_a == 0.0:
            return (w, h)

        a = w / float(h)

        if (old_a > a and w > 0) or h <= 0:
           h = w / old_a
        else:
            w = h * old_a

        return (w, h)

    def _calculate_aspect(self, old_w, old_h):
       ia = old_w / float(old_h)

       return self._calculate_aspect_from_ratio(ia)

    def _action_add(self, label, func_cb, data=None):
        btn = Button(self)
        btn.label_set(label)
        btn.data["clicked"] = (func_cb, data)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_min_set(100, 30)
        btn.size_hint_max_set(100, 30)
        self._actions_list[label] = btn
        btn.show()

        self.box.pack_end(btn)

    def _actions_hide(self):
        self._actions_list["Select"].hide()
        self._actions_list["Add new"].hide()
        self._actions_list["Close"].hide()

    def _actions_show(self):
        self._actions_list["Select"].show()
        self._actions_list["Add new"].show()
        self._actions_list["Close"].show()

    def _action_btn_clicked(self, bt, *args, **kwargs):
        func, udata = bt.data["clicked"]
        func(udata)

    def _set_controls(self):
        self.box = Box(self)
        self.box.size_hint_weight_set(1.0, 1.0)
        self.box.size_hint_align_set(-1.0, -1.0)
        self.box.horizontal_set(False)
        self.box.show()
        self.content_set("actions", self.box)

        if self.select_cb is not None:
            self._action_add("Select", self._select_cb)

        self._action_add("Add new", self._add_cb)
        self._action_add("Close", self._close_cb)

    def _set_image_list(self):
        self.list = List(self)
        self.list.size_hint_weight_set(1.0, 1.0)
        self.list.size_hint_align_set(-1.0, -1.0)
        self.list.show()
        self.content_set("images.list", self.list)
        self._image_list_load()

    def _image_list_load(self):
        for i in self._parent.editable.images:
            self.list.item_append(i, None, None, self._select_image_cb, i)
        self.list.go()

    def _fix_image_size(self, w, h):
        self.preview_image.load_size_set(w, h)
        self.preview_image.size_hint_max_set(w, h)

    def _preview_image_resize_apply(self):
        self._preview_image_resize_timer = None
        old_w, old_h = self.preview_image.image_size_get()
        if old_w == 0 or old_h == 0:
            return False
        w, h = self._calculate_aspect(old_w, old_h)
        self._fix_image_size(w, h)
        return False

    def _resize_preview_image(self, area):
        if self._preview_image_resize_timer:
            self._preview_image_resize_timer.delete()
        t = ecore.animator_add(self._preview_image_resize_apply)
        self._preview_image_resize_timer = t

    def _set_new_image(self, filename, key=None):
        try:
            self.preview_image.file_set(filename, key)
            old_w, old_h = self.preview_image.image_size_get()
            w, h = self._calculate_aspect(old_w, old_h)
            self._fix_image_size(w, h)
            self.layout_edje.signal_emit("editje,preview,show", "editje")
        except Exception, e:
            self.layout_edje.signal_emit("editje,preview,hide", "editje")
            print "Error setting preview image: ", e

    def _select_image_cb(self, obj, it, i, *args, **kwargs):
        self.image = i
        id = self._parent.editable.image_id_get(i)
        filename = self._parent.editable.file_get()[0]
        key = "images/" + str(id)

        self._set_new_image(filename, key)

    def _select_cb(self, data):
        if self.select_cb is None:
            return
        self.select_cb(self.image)
        self.close()

    def _add_cb(self, data):
        self._actions_hide()
        if self.fs is None:
            self.fs = Fileselector(self)
            self.fs.size_hint_weight_set(1.0, 1.0)
            self.fs.size_hint_align_set(-1.0, -1.0)
            self.fs.callback_selected_add(self._fs_selected_cb)
            self.fs.callback_done_add(self._fs_done_cb)
            self.fs.path_set(os.getenv("HOME"))
        self.list.hide()
        self.fs.show()
        self.content_set("images.list", self.fs)

    def _close_cb(self, data):
        self.close()

    def _fs_selected_cb(self, obj, selected, *args, **kwargs):
        self._set_new_image(selected)

    def _fs_done_cb(self, obj, selected):
        self._actions_show()
        if selected is not None:
            self._parent.editable.image_add(selected)
            self.list.clear()
            self._image_list_load()
        self.fs.hide()
        self.list.show()
        self.content_set("images.list", self.list)

    def close(self):
        if self._preview_image_resize_timer:
            self._preview_image_resize_timer.delete()
            self._preview_image_resize_timer = None
        PopUp.close(self)


class FontPopUp(PopUp):
    def __init__(self, parent, group=None, select_cb=None):
        PopUp.__init__(self, parent, group or "editje/font_select")

        self.select_cb = select_cb
        self.fs = None

        scr = Scroller(self)
        scr.show()

        frame = Layout(self)
        frame.file_set(self._parent.theme, "subgroup")
        frame.show()
        self.content_set("font.preview", frame)
        frame.content_set("content", scr)
        frame.edje_get().part_text_set("title.label", "Font preview")

        self.preview_font = self.layout_edje.evas.Text()
        self.preview_font.text_set("AaBbCc 12345")
        self.preview_font.color = (0, 0, 0, 255)
        self.preview_font.font_set("Sans", 12)
        self.preview_font.size_hint_weight_set(1.0, 1.0)
        self.preview_font.size_hint_align_set(-1.0, -1.0)
        self.preview_font.show()
        x, y, w, h = frame.edje_get().part_geometry_get("content")
        scr.content_set(self.preview_font)

        self.label_set("header.title", "Select a font")

        self._set_controls()
        self._set_font_list()

    def _action_add(self, label, func_cb, data=None):
        btn = Button(self)
        btn.label_set(label)
        btn.data["clicked"] = (func_cb, data)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_min_set(100, 30)
        btn.size_hint_max_set(100, 30)
        btn.show()

        self.box.pack_end(btn)

    def _action_btn_clicked(self, bt, *args, **kwargs):
        func, udata = bt.data["clicked"]
        func(udata)

    def _set_controls(self):
        self.box = Box(self)
        self.box.size_hint_weight_set(1.0, 1.0)
        self.box.size_hint_align_set(-1.0, -1.0)
        self.box.horizontal_set(False)
        self.box.show()
        self.content_set("actions", self.box)

        if self.select_cb is not None:
            self._action_add("Select", self._select_cb)

        self._action_add("Add new", self._add_cb)
        self._action_add("Close", self._close_cb)

    def _set_font_list(self):
        self.list = List(self)
        self.list.size_hint_weight_set(1.0, 1.0)
        self.list.size_hint_align_set(-1.0, -1.0)
        self.list.show()
        self.content_set("fonts.list", self.list)
        self._font_list_load()

    def _font_list_load(self):
        for i in self._parent.editable.fonts:
            self.list.item_append(i, None, None, self._select_font_cb, i)
        self.list.go()

    def _select_font_cb(self, obj, it, i, *args, **kwargs):
        self.font = i
        self.preview_font.font_set(self.font, 12)

    def _select_cb(self, data):
        if self.func is None:
            return
        self.func(self.font)
        self.close()

    def _add_cb(self, data):
        if self.fs is None:
            self.fs = Fileselector(self)
            self.fs.size_hint_weight_set(1.0, 1.0)
            self.fs.size_hint_align_set(-1.0, -1.0)
            self.fs.callback_selected_add(self._fs_selected_cb)
            self.fs.callback_done_add(self._fs_done_cb)
            self.fs.path_set(os.getenv("HOME"))
        self.list.hide()
        self.fs.show()
        self.content_set("fonts.list", self.fs)

    def _close_cb(self, data):
        self.close()

    def _fs_selected_cb(self, fs, selected, data):
        try:
            self.preview_font.font_set(selected, 12)
        except Exception, e:
            pass

    def _fs_done_cb(self, fs, selected, *args, **kwargs):
        if selected is not None:
            self._parent.editable.font_add(selected)
            self.list.clear()
            self._font_list_load()
        self.fs.hide()
        self.list.show()
        self.content_set("fonts.list", self.list)


class ConfirmPopUp(PopUp):
    def __init__(self, parent, group=None, select_cb=None):
        PopUp.__init__(self, parent, group or "editje/confirm")
        self.style_set("minimal")

        self.label_set("header.title", "Overwrite existing file ?")

        self._set_controls()

    def set_message(self,message):
        self.label_set("filename",message)

    def action_add(self, label, func_cb, data=None):
        btn = Button(self)
        btn.label_set(label)
        btn.data["clicked"] = (func_cb, data)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_min_set(100, 30)
        btn.size_hint_max_set(100, 30)
        btn.show()
        self.box.pack_end(btn)

    def _action_btn_clicked(self, bt, *args, **kwargs):
        func, udata = bt.data["clicked"]
        func(udata)

    def _set_controls(self):
        self.box = Box(self)
        self.box.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                      evas.EVAS_HINT_EXPAND)
        self.box.size_hint_align_set(evas.EVAS_HINT_FILL,
                                     evas.EVAS_HINT_FILL)
        self.box.horizontal_set(True)
        self.box.show()
        self.content_set("actions", self.box)

    def _close_cb(self, data):
        self.close()

    def close(self):
        PopUp.close(self)

class NewNamePopUp(Wizard):

    def __init__(self, parent, label):
        Wizard.__init__(self, parent, label)
        self.page_add("default")
        self.style_set("minimal")

        self._name_init()

        self.action_add("default", "Cancel", self._cancel, icon="cancel")
        self.action_add("default", "Add", self._internal_add, icon="confirm")
        self.action_disabled_set("Add", True)
        self.goto("default")

        self._name.focus()

    def _name_init(self):
        bx2 = Box(self)
        bx2.horizontal_set(True)
        bx2.size_hint_weight_set(1.0, 0.0)
        bx2.size_hint_align_set(-1.0, 0.0)
        bx2.size_hint_min_set(160, 160)
        self.content_append("default", bx2)
        bx2.show()

        lb = Label(self)
        lb.label_set("Name:")
        bx2.pack_end(lb)
        lb.show()

        scr = Scroller(self)
        scr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        scr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        scr.content_min_limit(False, True)
        scr.policy_set(ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF)
        scr.bounce_set(False, False)
        bx2.pack_end(scr)

        self._name = Entry(self)
        self._name.single_line_set(True)
        self._name.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._name.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._name.callback_activated_add(self._name_activated_cb)
        self._name.callback_changed_add(self._name_changed_cb)
        self._name.entry_set("")
        self._name.context_menu_disabled_set(True)
        self._name.show()

        scr.content_set(self._name)
        scr.show()

    def _name_activated_cb(self, obj):
        self._internal_add(None, None)

    def _name_changed_cb(self, obj):
        name = self._name.entry_get()
        if name != "" and name != "<br>":
            self.action_disabled_set("Add", False)
        else:
            self.action_disabled_set("Add", True)

    def _internal_add(self, popup, data):
        name = self._name.entry_get().replace("<br>", "")
        
        self._add(name)
        return

    def _cancel(self, popup, data):
        self.close()

class NewAnimationNamePopUp(NewNamePopUp):
    def __init__(self, parent):
        NewNamePopUp.__init__(self, parent, "New Animation")

    def _add(self, name):
        success = self._parent.e.animation_add(name)
        if success:
            self.close()
        else:
            self._notify("Choice another name")
