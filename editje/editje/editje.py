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

import os
import sys

import evas
import edje
import elementary

import sysconfig
from about import About
from editable import Editable
import objects_data

from desktop import Desktop
from collapsable import CollapsablesBox

from details_group import GroupDetails
from details_part import PartDetails
from details_state import PartStateDetails, PartAnimStateDetails

from parts import PartsList
from animations import AnimationDetails, AnimationsList
from signals import SignalsList, SignalDetails

from widgets_list import WidgetsList
from groupselector import GroupSelectionWizard

from misc import name_generate


def debug_cb(obj, emission, source):
    print "%s: %s %s" % (obj, emission, source)


class Editje(elementary.Window):
    def __init__(self, swapfile, theme="default"):
        self.theme = sysconfig.theme_file_get(theme)
        elementary.theme_extension_add(self.theme)

        # external modules
        for m in edje.available_modules_get():
            edje.module_load(m)

        elementary.Window.__init__(self, "editje", elementary.ELM_WIN_BASIC)
        self.title_set("Editje - Edje Editor")
        self.callback_destroy_add(self._destroy_cb)
        self.autodel_set(True)
        self.resize(800, 600)

        # Load Edje Theme File
        self._load_theme()

        self.e = Editable(self.evas, swapfile)

        # Setup Windows Parts
        self._toolbar_static_init()
        self._desktop_init()
        self._modes_init()

        self._clipboard = None

    def _destroy_cb(self, obj):
        self.e.close()

    def _cmd_write(self, cls, *args):
        cmd = "CMD,%s" % (cls,)
        arguments = ",".join(args)
        if arguments:
            cmd += "," + arguments
        cmd += "\n"
        sys.stdout.write(cmd)
        sys.stdout.flush()

    def _load_theme(self, group="main"):
        self.main_layout = elementary.Layout(self)
        self.main_layout.file_set(self.theme, group)
        self.main_layout.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(self.main_layout)
        self.main_edje = self.main_layout.edje_get()
        self.main_edje.signal_emit("details,enable", "") #TODO: remove this
        self.main_layout.show()

    def save(self):
        self.block(True)
        self.e.save()
        self.block(False)

    def _file_get(self):
        return self.e.filename

    file = property(_file_get)

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
        grp_wiz = GroupSelectionWizard(
            self, switch_only=True,
            selected_set_cb=self._group_wizard_selection_set_cb,
            selected_get_cb=self._group_wizard_selection_get_cb,
            new_grp_cb=self._group_wizard_new_group_cb,
            check_grp_cb=self._group_wizard_check_group_cb,
            del_grp_cb=self._group_wizard_del_group_cb)
        grp_wiz.file_set(self.e.workfile)
        grp_wiz.open()

    def show(self):
        if not self.e.group:
            self.select_group()
        elementary.Window.show(self)

    def _group_wizard_check_group_cb(self, grp_name):
        return self.e.group_exists(grp_name)

    def _group_wizard_new_group_cb(self, grp_name):
        return self.e.group_add(grp_name)

    def _group_wizard_del_group_cb(self, grp_name):
        return self.e.group_del(grp_name)

    def _group_wizard_selection_set_cb(self, selection):
        if not selection:
            return False
        self.group = selection
        return True

    def _group_wizard_selection_get_cb(self):
        return self.group

    def _image_wizard_new_image_cb(self, img):
         self.e.image_add(img)

    def _image_wizard_image_list_get_cb(self):
        return self.e.images_get()

    def _image_wizard_image_id_get_cb(self, name):
        return self.e.image_id_get(name)

    def _font_wizard_new_font_cb(self, fnt):
         self.e.font_add(fnt)

    def _font_wizard_font_list_get_cb(self):
#        font_list = self.e.fonts_get()
#        font_list += self.main_layout.edje_get().evas.font_available_list()
#        return font_list
        return self.main_layout.edje_get().evas.font_available_list()

    def _font_wizard_font_id_get_cb(self, name):
        return self.e.font_id_get(name)

    def _workfile_name_get_cb(self):
        return self.e.workfile

    def _part_object_get_cb(self, name):
        return self.e.part_object_get(name)

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
        self._toolbar_filename_cb(self, self.e.filename)
        self.e.callback_add("filename.changed", self._toolbar_filename_cb)

        self.main_edje.part_text_set("details_group_label", "Group:")
        self.main_edje.part_text_set("details_file_label", "File:")

        self._group_name_entry = elementary.Entry(self)
        self._group_name_entry.context_menu_disabled_set(True)
        self._group_name_entry.size_hint_weight_set(1.0, 1.0)
        self._group_name_entry.size_hint_align_set(-1.0, -1.0)
        self._group_name_entry.single_line_set(True)
        self._group_name_entry.style_set("editje")
        self._group_name_entry.callback_activated_add(self._group_name_changed)
        self.main_layout.content_set("details_group.swallow",
                                     self._group_name_entry)

        self._toolbar_group_cb(self, "< none >")
        self.e.callback_add("group.changed", self._toolbar_group_cb)

        self._toolbar_bt_init(self.main_edje, "open.bt", "Open", self._open_cb)
        self._toolbar_bt_init(self.main_edje, "save.bt", "Save", self._save_cb)
        self._toolbar_bt_init(self.main_edje, "group.bt", "Groups", self._group_cb)

        self._toolbar_bt_init(self.main_edje, "run.bt", "Run", self._run_cb)

        self._toolbar_bt_init(self.main_edje, "options.bt", "Options",
                              self._options_cb)

    def _group_name_changed(self, obj, *args, **kwargs):
        new_name = obj.entry_get()
        if not self.e.group_rename(new_name):
            obj.entry_set(self.e.group)

    def _toolbar_filename_cb(self, emissor, data):
        self.main_edje.part_text_set("details_filename", data)

    def _toolbar_group_cb(self, emissor, data):
        self._group_name_entry.entry_set(data)

    def _group_cb(self, obj, emission, source):
        grp_wiz = GroupSelectionWizard(
            self, switch_only=False,
            selected_set_cb=self._group_wizard_selection_set_cb,
            selected_get_cb=self._group_wizard_selection_get_cb,
            new_grp_cb=self._group_wizard_new_group_cb,
            check_grp_cb=self._group_wizard_check_group_cb,
            del_grp_cb=self._group_wizard_del_group_cb)

        grp_wiz.file_set(self.e.workfile)
        grp_wiz.open()

    def _toolbar_bt_init(self, edje, part, name, callback):
        edje.part_text_set(part + ".label", name)
        edje.signal_callback_add("mouse,clicked,1", part, callback)

    def _open_cb(self, obj, emission, source):
        from openfile import OpenFile
        open = OpenFile()
        open.path = os.path.dirname(self.e.filename)
        open.show()

    def _save_cb(self, obj, emission, source):
        self.save()

    def _run_cb(self, obj, emission, source):
        def _test_window_closed(obj):
            self.e.group_size = w.size
            #obj.delete()
            self.block(False)
        w = elementary.Window("edje-test", elementary.ELM_WIN_BASIC)
        w.callback_destroy_add(_test_window_closed)
        w.autodel_set(True)
        w.resize(*self.e.group_size)
        w.title_set("Edje Test")
        b = elementary.Background(w)
        b.size_hint_weight_set(1.0, 1.0)
        w.resize_object_add(b)
        b.show()
        l = elementary.Layout(w)
        l.file_set(self.e.workfile, self.group)
        l.size_hint_weight_set(1.0, 1.0)
        l.show()
        w.resize_object_add(l)
        w.show()
        self.block(True)

    def _options_cb(self, obj, emission, source):
        print "Options ...."

    def _image_list_cb(self, obj, emission, source):
        #FIXME: Not used
        #ImageSelectionWizard(self).open()
        return

    def _font_list_cb(self, obj, emission, source):
        #FIXME: Not used
        #FontSelectionWizard(self).open()
        return

    def _about_cb(self, obj, emission, source):
        About(self).open()
        return

    def _cut_cb(self, obj, emission, source):
        if not self.e.part.name:
            return

        self._clipboard = objects_data.Part(self.e.part._part)
        self.e.part_del(self.e.part.name)

    def _copy_cb(self, obj, emission, source):
        if not self.e.part.name:
            return

        self._clipboard = objects_data.Part(self.e.part._part)

    def _paste_cb(self, obj, emission, source):
        if not self._clipboard:
            return

        name = name_generate(self._clipboard.name, self.e.parts)

        type = self._clipboard.type

        source = self._clipboard["source"]
        if source is None:
            source = ''

        if self.e.part_add(name, type, source=source, signal=False):
            part = self.e._edje.part_get(name)
            self._clipboard.apply_to(part)
            self.e.event_emit("part.added", name)

    def _play_cb(self, obj, emission, source):
#        def play_end(emissor, data):
#               TODO: emit signals here to enable play, next, previous and disable stop button
#        self.e.animation.callback_add("animation.play.end", play_end)
        self.e.part.name = ""
        self.e.animation.play()
#       TODO: emit signals here to disable play, next, previous and enable stop button

    def _stop_cb(self, obj, emission, source):
        self.e.animation.stop()
#       TODO: emit signals here to enable play, next, previous and disable stop button

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
        self._toolbar_pager.style_set("editje.downwards")
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
        self._modes_selector.size_hint_align_set(
            evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        bx.pack_end(self._modes_selector)
        self._modes_selector.show()

        self._mainbar_pager = elementary.Pager(self)
        self._mainbar_pager.style_set("editje.rightwards")
        self._mainbar_pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                 evas.EVAS_HINT_EXPAND)
        self._mainbar_pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                                evas.EVAS_HINT_FILL)
        bx.pack_end(self._mainbar_pager)
        self._mainbar_pager.show()

    def _sidebar_init(self):
        self._sidebar_pager = elementary.Pager(self)
        self._sidebar_pager.style_set("editje.leftwards")
        self._sidebar_pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                                 evas.EVAS_HINT_EXPAND)
        self._sidebar_pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                                evas.EVAS_HINT_FILL)
        self.main_layout.content_set("details", self._sidebar_pager)
        self._sidebar_pager.show()

    def _mode_add(self, name, icon, toolbar, mainbar, sidebar):
        item = self._modes_selector.item_add(
            name, "", 0, self._mode_set_cb, name)
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

        self.mode = name

        if name == "Signals":
            self.desktop_block(True)
        elif name == "Parts":
            self.desktop_block(False)

        if name == "Animations":
            self.main_edje.signal_emit("mode,anim,on", "editje")
            if not hasattr(self, "_prevstates"):
                self._prevstates = []
                for p in self.e.parts:
                    part = self.e._edje.part_get(p)
                    self._prevstates.append((p, part.state_selected_get()))
            if hasattr(self, "_prevanim"):
	        self.e.animation.name = self._prevanim
        else:
            self.main_edje.signal_emit("mode,anim,off", "editje")
            self._prevanim = self.e.animation.name
            #This is a hack to force change the animation
            self.e.animation.name = None

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

    def _set_scrolled_contents(self, box):
        scr = elementary.Scroller(self)
        scr.style_set("editje.collapsable")
        scr.bounce_set(False, False)
        scr.content_set(box)
        scr.show()
        return scr

    # Parts

    def _parts_init(self):
        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.file_set(self.theme, "toolbar.edit")
        toolbar.show()

        edj = toolbar.edje_get()

        # FIXME: these 2 buttons are (#if 0)-ed at edc, decide if we're killing
        # it here soon or not
        self._toolbar_bt_init(
            edj, "image_list.bt", "Images", self._image_list_cb)
        self._toolbar_bt_init(edj, "font_list.bt", "Fonts", self._font_list_cb)

        self._toolbar_bt_init(edj, "cut.bt", "Cut", self._cut_cb)

        self._toolbar_bt_init(edj, "copy.bt", "Copy", self._copy_cb)

        self._toolbar_bt_init(edj, "paste.bt", "Paste", self._paste_cb)

        self._toolbar_bt_init(edj, "about.bt", "About", self._about_cb)

        # Mainbar
        mainbar = CollapsablesBox(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        list = PartsList(self, self.evas, self.e)
        list.title = "Parts"
        list.options = True
        list.open = True
        mainbar.pack_end(list)
        list.show()

        list = WidgetsList(self)
        list.title = "Widgets"
        list.open = True
        mainbar.pack_end(list)
        list.show()

        # Sidebar
        sidebar = self._parts_sidebar_create()

        self._mode_add("Parts", "editje/icon/part",
                       toolbar, mainbar, sidebar)

    def _parts_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.group_details = GroupDetails(self)
        box.pack_end(self.group_details)

        self.part_details = PartDetails(self)
        box.pack_end(self.part_details)

        self.part_state_details = PartStateDetails(self, \
                img_new_img_cb=self._image_wizard_new_image_cb, \
                img_list_get_cb=self._image_wizard_image_list_get_cb, \
                img_id_get_cb=self._image_wizard_image_id_get_cb, \
                fnt_new_fnt_cb=self._font_wizard_new_font_cb, \
                fnt_list_get_cb=self._font_wizard_font_list_get_cb, \
                fnt_id_get_cb=self._font_wizard_font_id_get_cb, \
                workfile_name_get_cb=self._workfile_name_get_cb, \
                part_object_get_cb=self._part_object_get_cb)
        box.pack_end(self.part_state_details)

        return self._set_scrolled_contents(box)

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

        def _animation_changed(it, ti):
            #TODO: emit signals to enable animations buttons here
            if self.mode == "Animations":
                self.desktop_block(False)

        def _animation_unselected(it, ti):
            #TODO: emit signals to disable animations buttons here
            if self.mode == "Animations":
                self.desktop_block(True)

        self.e.animation.callback_add("animation.changed", _animation_changed)
        self.e.animation.callback_add("animation.unselected", _animation_unselected)

        # Mainbar
        mainbar = CollapsablesBox(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        def new_anim_cb(name):
            return self.e.animation_add(name)

        def anims_list_cb():
            return self.e.animations

        list = AnimationsList(self, new_anim_cb, anims_list_cb)
        list.options = True
        list.title = "Animations"
        list.open = True
        mainbar.pack_end(list)
        list.show()

        list = PartsList(self, self.evas, self.e)
        list.title = "Parts"
        list.open = True
        mainbar.pack_end(list)
        list.show()

        # Sidebar
        sidebar = self._animations_sidebar_create()

        self._mode_add("Animations", "editje/icon/animation",
                       toolbar, mainbar, sidebar)

    def _animations_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.anim_details = AnimationDetails(self)
        box.pack_end(self.anim_details)
        self.anim_details.show()

        self.anim_state_details = PartAnimStateDetails(self, \
                img_new_img_cb=self._image_wizard_new_image_cb, \
                img_list_get_cb=self._image_wizard_image_list_get_cb, \
                img_id_get_cb=self._image_wizard_image_id_get_cb, \
                fnt_new_fnt_cb=self._font_wizard_new_font_cb, \
                fnt_list_get_cb=self._font_wizard_font_list_get_cb, \
                fnt_id_get_cb=self._font_wizard_font_id_get_cb, \
                workfile_name_get_cb=self._workfile_name_get_cb, \
                part_object_get_cb=self._part_object_get_cb) # fix
        self.anim_state_details.open = True
        self.anim_state_details.open_disable = True
        box.pack_end(self.anim_state_details)

        return self._set_scrolled_contents(box)

    # Signals

    def _signals_init(self):
        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.show()

        # Mainbar
        mainbar = CollapsablesBox(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        def new_sig_cb(name, type_):
            return self.e.signal_add(name, type_)

        def sigs_list_cb():
            return self.e.signals

        list = SignalsList(self, new_sig_cb, sigs_list_cb)
        list.title = "Signals"
        list.open = True
        list.options = True
        mainbar.pack_end(list)
        list.show()

        # Sidebar
        sidebar = self._signals_sidebar_create()

        self._mode_add("Signals", "editje/icon/signal",
                       toolbar, mainbar, sidebar)

    def _signals_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        signal_details = SignalDetails(self)
        signal_details.open = False
        signal_details.open_disable = True
        signal_details.show()

        box.pack_end(signal_details)

        return self._set_scrolled_contents(box)
