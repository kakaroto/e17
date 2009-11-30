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
import sys

import evas
import edje
from edje.edit import EdjeEdit
import elementary

import sysconfig
from editable import Editable
from details_group import GroupDetails
from details_part import PartDetails
from details_state import PartStateDetails
from popups import ImagePopUp, FontPopUp
from desktop import Desktop
from parts_list import PartsList
from animations_list import AnimationsList
from animations import AnimationDetails
from signals import SignalsList, SignalDetails
from groupselector import GroupChange

class Editje(elementary.Window):

    def __init__(self, theme="default"):
        self.theme = sysconfig.theme_file_get(theme)

        self.group_details = None
        self.editable = None
        self._mode = None

        elementary.theme_extension_add(self.theme)

        for m in edje.available_modules_get():
            edje.module_load(m)

        elementary.Window.__init__(self, "editje", elementary.ELM_WIN_BASIC)
        self.title_set("Editje - Edje Editor")
        self.callback_destroy_add(self._destroy_cb)
        self.resize(800, 600)

        # Load Edje Theme File
        self._load_theme()

        self.e = Editable(self.main_layout.edje_get().evas)

        # Setup Windows Parts
        self._toolbar_static_init()
        self._desktop_init()
        self._modes_init()
        self._hacks_init()

    def _cmd_write(self, cls, *args):
        cmd = "CMD,%s" % (cls,)
        arguments = ",".join(args)
        if arguments:
            cmd += "," + arguments
        cmd += "\n"
        sys.stdout.write(cmd)
        sys.stdout.flush()

    def _destroy_cb(self, obj, *args, **kwargs):
        elementary.exit()

    def _load_theme(self, group="main"):
        self.main_layout = elementary.Layout(self)
        self.main_layout.file_set(self.theme, group)
        self.main_layout.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(self.main_layout)
        self.main_edje = self.main_layout.edje_get()
        self.main_layout.show()

    # Hack, remove when spaced is not needed
    def _collapsable_size_hint_changed(self, obj, spacer, objs, *args, **kwargs):
        for o in objs:
            wx, wy = o.size_hint_weight
            if wy > 0.0:
                break
        else:
            spacer.size_hint_weight_set(1.0, 1.0)
            return
        spacer.size_hint_weight_set(1.0, 0.0)

    def save(self):
        self.block(True)
        self.e.save()
        self.block(False)

    def _file_set(self, file):
        self.e.filename = file

    def _file_get(self):
        return self.e.filename

    file = property(_file_get, _file_set)

    def _group_set(self, group):
        self.e.group = group

    def _group_get(self):
        return self.e.group

    group = property(_group_get, _group_set)

    def block(self, bool):
        if bool:
            self.main_edje.signal_emit("blocker,enable", "")
        else:
            self.main_edje.signal_emit("blocker,disable", "")

    def desktop_block(self, bool):
        if bool:
            self.main_edje.signal_emit("desktop,blocker,enable", "")
        else:
            self.main_edje.signal_emit("desktop,blocker,disable", "")

    def select_group(self):
        gc = GroupChange(self)
        gc.file = self.e.filename
        gc.open()

    ###########
    # DESKTOP
    ###########
    def _get_view(self):   #HACK
        return self

    view = property(_get_view)

    def _desktop_init(self):
        self.desktop = Desktop(self)
        self.main_layout.content_set("desktop", self.desktop.view)
        self.desktop.view.show()

    ###########
    # TOOLBAR
    ###########
    def _toolbar_static_init(self):
        self._toolbar_filename_cb(self, "< none >")
        self.e.event_callback_add("filename.changed",
                                  self._toolbar_filename_cb)

        self.main_edje.part_text_set("details_group_label", "Group:")

        self._group_name_entry = elementary.Entry(self)
        self._group_name_entry.size_hint_weight_set(1.0, 1.0)
        self._group_name_entry.size_hint_align_set(-1.0, -1.0)
        self._group_name_entry.single_line_set(True)
        self._group_name_entry.style_set("editje")
        self._group_name_entry.callback_activated_add(self._group_name_changed)
        self.main_layout.content_set("details_group.swallow",
                                     self._group_name_entry)

        self._toolbar_group_cb(self, "< none >")
        self.e.event_callback_add("group.changed",
                                  self._toolbar_group_cb)

        self._toolbar_bt_init(self.main_edje, "save.bt", "Save", self._save_cb)

        self._toolbar_bt_init(self.main_edje, "options.bt", "Options",
                              self._options_cb)

    def _group_name_changed(self, obj, *args, **kwargs):
        new_name = obj.entry_get().replace("<br>", "")
        self.e.group_rename(new_name)

    def _toolbar_filename_cb(self, emissor, data):
        print data
        self.main_edje.part_text_set("details_filename", data)

    def _toolbar_group_cb(self, emissor, data):
        self._group_name_entry.entry_set(data)

    def _toolbar_bt_init(self, edje, part, name, callback):
        edje.part_text_set(part + ".label", name)
        edje.signal_callback_add("mouse,clicked,1", part, callback)

    def _save_cb(self, obj, emission, source):
        self.save()

    def _options_cb(self, obj, emission, source):
        print "Options ...."

    def _image_list_cb(self, obj, emission, source):
        ImagePopUp(self).open()

    def _font_list_cb(self, obj, emission, source):
        FontPopUp(self).show()

    def _play_cb(self, obj, emission, source):
#        part = self.e.part.name
#        def play_end(emissor, data):
#            self.e.part.name = part
#        self.e.animation.event_callback_add("animation.play.end", play_end)
        self.e.part.name = ""
        self.e.animation.play()

    def _stop_cb(self, obj, emission, source):
        self.e.animation.stop()

    def _previous_cb(self, obj, emission, source):
        self.e.animation.state_prev_goto()

    def _next_cb(self, obj, emission, source):
        self.e.animation.state_next_goto()

    ###########
    # Modes
    ###########
    def _modes_init(self):
        self._modes = {}
        self._toolbar_init()
        self._mainbar_init()
        self._sidebar_init()

        self._parts_init()
        self._animations_init()
        self._signals_init()

        self._mode_set_cb(self._modes_selector, None, "Parts")

    def _toolbar_init(self):
        self._toolbar_pager = elementary.Pager(self)
        self._toolbar_pager.style_set("editje.toolbar")
        self._toolbar_pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                 evas.EVAS_HINT_EXPAND)
        self._toolbar_pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                                evas.EVAS_HINT_FILL)
        self.main_layout.content_set("toolbar.contents.swallow",
                                     self._toolbar_pager)
        self._toolbar_pager.show()


    def _mainbar_init(self):
        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bx.size_hint_align_set(evas.EVAS_HINT_FILL,evas.EVAS_HINT_FILL)
        self.main_layout.content_set("objects", bx)
        bx.show()

        self._modes_selector = elementary.Hoversel(self)
        self._modes_selector.hover_parent_set(self)
        self._modes_selector.label_set("Mode")
        self._modes_selector.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._modes_selector.size_hint_align_set(evas.EVAS_HINT_FILL,evas.EVAS_HINT_FILL)
        bx.pack_end(self._modes_selector)
        self._modes_selector.show()

        self._mainbar_pager = elementary.Pager(self)
        self._mainbar_pager.style_set("editje.mainbar")
        self._mainbar_pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                 evas.EVAS_HINT_EXPAND)
        self._mainbar_pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                                evas.EVAS_HINT_FILL)
        bx.pack_end(self._mainbar_pager)
        self._mainbar_pager.show()

    def _sidebar_init(self):
        self._sidebar_pager = elementary.Pager(self)
        self._sidebar_pager.style_set("editje.sidebar")
        self._sidebar_pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                 evas.EVAS_HINT_EXPAND)
        self._sidebar_pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                                evas.EVAS_HINT_FILL)
        self.main_layout.content_set("details", self._sidebar_pager)
        self._sidebar_pager.show()

    def _mode_add(self, name, icon, toolbar, mainbar, sidebar):
        item = self._modes_selector.item_add(name, "", 0, self._mode_set_cb, name)
        self._modes[name] = (item, toolbar, mainbar, sidebar)
        item.icon_set(self.theme, icon, elementary.ELM_ICON_FILE)
        self._toolbar_pager.content_push(toolbar)
        self._mainbar_pager.content_push(mainbar)
        self._sidebar_pager.content_push(sidebar)

    def _mode_set_cb(self, obj, it, name, *args, **kwargs):
        item, toolbar, mainbar, sidebar = self._modes[name]
        self._toolbar_pager.content_promote(toolbar)
        self._mainbar_pager.content_promote(mainbar)
        self._sidebar_pager.content_promote(sidebar)

        if name == "Signals":
            self.desktop_block(True)
        else:
            self.desktop_block(False)

        if name == "Animations":
            self.main_edje.signal_emit("mode,anim,on", "editje")
            if not hasattr(self, "_prevstates"):
                self._prevstates = []
                for p in self.e.parts:
                    part = self.e._edje.part_get(p)
                    self._prevstates.append((p, part.state_selected_get()))
            if self.e.animation.name:
                self.e.animation._current()
                if self.e.part.name:
                    self.e.part.state.name = self.e.part._part.state_selected_get()
            else:
                self.anim_state_details._hide_all()
        else:
            self.e.animation.name = None
            self.main_edje.signal_emit("mode,anim,off", "editje")

        if name == "Parts":
            if hasattr(self, "_prevstates"):
                for p, s in self._prevstates:
                    part = self.e._edje.part_get(p)
                    part.state_selected_set(s)
                    if p == self.e.part.name:
                        self.e.part.state.name = s
                del self._prevstates

        self._modes_selector.label_set("Mode: " + name)

        file, group, type = item.icon_get()
        ico = elementary.Icon(self)
        ico.file_set(file, group)
        ico.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
        ico.show()
        self._modes_selector.icon = ico

    # Parts

    def _parts_init(self):
        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.file_set(self.theme, "toolbar.edit")
        toolbar.show()

        edj = toolbar.edje_get()
        self._toolbar_bt_init(edj, "image_list.bt", "Images",
                              self._image_list_cb)
        self._toolbar_bt_init(edj, "font_list.bt", "Fonts", self._font_list_cb)

        # Mainbar
        mainbar = PartsList(self)
        mainbar.title = "Parts"
        mainbar.options = True
        mainbar.open = True
        mainbar.view.show()

        # Sidebar
        sidebar = self._parts_sidebar_create()

        self._mode_add("Parts", "editje/icon/part",
                       toolbar, mainbar.view, sidebar)

    def _parts_sidebar_create(self):
        box = elementary.Box(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.group_details = GroupDetails(self)
        self.group_details.open()
        self.group_details.size_hint_weight_set(0, 0)
        self.group_details.size_hint_align_set(-1, -1)
        box.pack_end(self.group_details)
        self.group_details.show()

        self.part_details = PartDetails(self)
        box.pack_end(self.part_details)
        self.part_details.show()

        self.part_state_details = PartStateDetails(self)
        box.pack_end(self.part_state_details)
        self.part_state_details.show()

        spacer = self.evas.Rectangle(color=(0, 0, 0, 0))
        spacer.size_hint_weight_set(1.0, 1.0)
        spacer.size_hint_align_set(-1.0, -1.0)
        data = (spacer, (self.group_details, self.part_details, self.part_state_details))
        self.group_details.event_callback_add(
            evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS,
            self._collapsable_size_hint_changed, *data)
        self.part_details.event_callback_add(
            evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS,
            self._collapsable_size_hint_changed, *data)
        self.part_state_details.event_callback_add(
            evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS,
            self._collapsable_size_hint_changed, *data)
        box.pack_end(spacer)
        spacer.show()

        return box

    # Animations

    def _animations_init(self):
        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.file_set(self.theme, "toolbar.anim")
        toolbar.show()

        edj = toolbar.edje_get()
        self._toolbar_bt_init(edj, "previous.bt", "Previous", self._previous_cb)
        self._toolbar_bt_init(edj, "play.bt", "Play", self._play_cb)
        self._toolbar_bt_init(edj, "next.bt", "Next", self._next_cb)
        self._toolbar_bt_init(edj, "stop.bt", "Stop", self._stop_cb)

        # Mainbar
        mainbar = elementary.Box(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        list = AnimationsList(self)
        list.options = True
        list.title = "Animations"
        list.open = True
        mainbar.pack_end(list.view)
        list.view.show()

        list = PartsList(self)
        list.title = "Parts"
        list.open = True
        mainbar.pack_end(list.view)
        list.view.show()

        # Sidebar
        sidebar = self._animations_sidebar_create()

        self._mode_add("Animations", "editje/icon/animation",
                       toolbar, mainbar, sidebar)

    def _animations_sidebar_create(self):
        box = elementary.Box(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.anim_details = AnimationDetails(self)
        self.anim_details.open()
        box.pack_end(self.anim_details)
        self.anim_details.show()

        self.anim_state_details = PartStateDetails(self, True) # fix
        self.anim_state_details.open()
        box.pack_end(self.anim_state_details)
        self.anim_state_details.show()

        spacer = self.evas.Rectangle(color=(0, 0, 0, 0))
        spacer.size_hint_weight_set(1.0, 1.0)
        spacer.size_hint_align_set(-1.0, -1.0)
        data = (spacer, (self.anim_details, self.anim_state_details))
        self.anim_details.event_callback_add(
                evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                self._collapsable_size_hint_changed, *data)
        self.anim_state_details.event_callback_add(
                evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS,
                self._collapsable_size_hint_changed, *data)
        box.pack_end(spacer)
        spacer.show()

        return box

    # Signals

    def _signals_init(self):
        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.show()

        # Mainbar
        mainbar = SignalsList(self)
        mainbar.title = "Signals"
        mainbar.open = True
        mainbar.options = True

        # Sidebar
        sidebar = SignalDetails(self)
        sidebar.open()
        sidebar.show()

        self._mode_add("Signals", "editje/icon/signal",
                       toolbar, mainbar.view, sidebar)

    # HACKS
    def _hacks_init(self):
        self.e.event_callback_add("group.changed",
                                       self._group_changed_cb)
        self.e.part.event_callback_add("part.changed",
                                       self._part_selected_cb)

    def _group_changed_cb(self, emissor, data):
        self.editable = self.e.edje

        self.part_state_details.editable_set(self.editable)
        self.anim_state_details.editable_set(self.editable)

    def _part_selected_cb(self, emissor, name):
        part = self.editable.part_get(name)
        self.part_state_details.active_part_set(part)
        self.anim_state_details.active_part_set(part)
