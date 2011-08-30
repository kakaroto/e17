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
import errno

import evas
import ecore
import edje
import elementary
import ecore

import sysconfig
from about import About
from editable import Editable
import objects_data
from openfile import OpenFileManager

from desktop import Desktop
from collapsable import CollapsablesBox
from fileselector import FileSelector

from details_group import GroupDetails
from details_part import PartDetails
from details_state import PartStateDetails, PartAnimStateDetails

from parts import PartsList
from animations import AnimationDetails, AnimationsList, AnimationsPartsList
from signals import SignalsList, SignalDetails
from error_notify import ErrorNotify
import swapfile

from widgets_list import WidgetsList
from groupselector import GroupSelectionWizard
from operation import Operation
from rpc_handlers import QueriesHandler, ReportsHandler

from misc import name_generate, accepted_filetype

from log_window import LogWindow


def debug_cb(obj, emission, source):
    print "%s: %s %s" % (obj, emission, source)


class Editje(elementary.Window, OpenFileManager):
    def __init__(self, swapfile, theme="default", slave_mode=False,
                 in_port=None):
        self.theme = sysconfig.theme_file_get(theme)
        elementary.theme_extension_add(self.theme)

        # external modules
        for m in edje.available_modules_get():
            edje.module_load(m)

        elementary.Window.__init__(self, "editje", elementary.ELM_WIN_BASIC)
        self.title_set("Editje - Edje Editor")
        self.callback_destroy_add(self._destroy_cb)
        self.resize(800, 600)

        # Load Edje Theme File
        self._load_theme()

        # using None as sentinel
        self._op_stack = [None]
        self._op_stack_ptr = 0

        self.e = Editable(self.evas, swapfile, self._mode_get)

        self._child_to_close = []

        self._slave_mode = slave_mode
        if slave_mode:
            try:
                self._slave_mode_init(in_port)
            except Exception, e:
                self._close()
                raise

        # Setup Windows Parts
        self._toolbar_static_init()
        self._desktop_init()

        self._mode = None
        self._modes_init()

        self._clipboard = None

        self.elm_event_callback_add(self._event_cb)

        self.__notification = None

    def open(self, sf):
        editje = Editje(sf)
        editje.show()

    def open_new(self):
        def ok(sf):
            editje = Editje(sf)
            editje.group = "main"
            editje.show()
        swapfile.open_new(ok, None)

    def _event_cb(self, obj, src, t, event):
        if t != evas.EVAS_CALLBACK_KEY_DOWN:
            return False

        if self.mode == "Animations":
            return False

        key = event.keyname
        alt_key = event.modifier_is_set("Alt")
        control_key = event.modifier_is_set("Control")
        shift_key = event.modifier_is_set("Shift")
        super_key = event.modifier_is_set("Super")

        if not control_key:
            if key == "Delete":
                name = self.e.part.name
                if not name:
                    return False

                relatives = self.e.relative_parts_get(name)
                op = Operation("part deletion: " + name)
                op.undo_callback_add(
                    self.e.part_add_bydata,
                    objects_data.Part(self.e.part_get(name)), relatives)
                op.redo_callback_add(self.e.part_del, name)
                self._operation_stack(op)
                op.redo()
                return True

        else:
            if key == "x":
                self._cut_cb(self, None, None)
                return True
            elif key == "c":
                self._copy_cb(self, None, None)
                return True
            elif key == "v":
                self._paste_cb(self, None, None)
                return True
            elif not shift_key and key == "z":
                self._undo_cb(self, None, None)
                return True
            elif shift_key and key == "z":
                self._redo_cb(self, None, None)
                return True
            elif key == "n":
                self._new_cb(self, None, None)
            elif key == "o":
                self._open_cb(self, None, None)
            elif key == "s":
                self._save_cb(self, None, None)
            elif key == "g":
                self._group_cb(self, None, None)
            elif key == "r":
                self._run_cb(self, None, None)

    def _destroy_cb(self, obj):
        if self.e.filename:
            self._close()
            return

        def save_as(bt, notification):
            notification.hide()
            notification.delete()
            self.save_as()

        def dismiss(bt, notification):
            notification.hide()
            notification.delete()

        notification = ErrorNotify(self,
                                   orient=elementary.ELM_NOTIFY_ORIENT_CENTER)
        notification.title = "Not saved"
        notification.action_add("Close", self._close)
        notification.action_add("Save", save_as, None, notification)
        notification.action_add("Cancel", dismiss, None, notification)
        notification.show()

    def _slave_mode_init(self, in_port):
        self._rpc_client = ReportsHandler(self.e)
        self._rpc_server = QueriesHandler(self.e, in_port,
                self._rpc_client.agent_register,
                self._rpc_client.agent_unregister)

        return True

    def _close(self, *args):
        for i in self._child_to_close:
            i.delete()
        self.e.close()

        if self._slave_mode:
            if hasattr(self, "_rpc_server"):
                self._rpc_server.delete()

        self.hide()
        self.delete()

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
        self.main_layout.size_hint_weight_set(
            evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.resize_object_add(self.main_layout)
        self.main_edje = self.main_layout.edje_get()
        self.main_edje.signal_emit("details,enable", "")  # TODO: remove this
        self.main_layout.show()

        self._window_blocker = None

    # Save
    def save(self):

        # Just call the compile, if it's not required it will return True
        if not self.e.edje.script_compile():
            # Oops, build failed and we don't yet have errors, so just
            # say the build failed.
            log_win = LogWindow(self)
            message = "There was an error rebuilding the Embryo script.<br>" \
                       "This could be caused by referenced parts or programs" \
                       "that have been removed or renamed.<br>" \
                       "The object will be saved and the generated source will" \
                       "contain the script. For the time being, it requires" \
                       "the user to manually edit it. Use edje_decc to extract" \
                       "the edc source from a binary file, fix it by hand and" \
                       "build again with edje_cc.<br>"
            for p, e in self.e.edje.script_errors:
                if not p:
                    p = "shared group script"
                message += "in '%s'<br>   %s<br>" % (p, e)
            log_win.message_set(message, title = "Script Error",
                                subtitle = "Could not build Embryo script.")
            log_win.open()
        if not self.e.filename:
            return self.save_as()

        def save_ok(sf):
            self.block(False)

        def save_error(err):
            nt = elementary.Notify(self)
            nt.timeout_set(2)
            nt.orient_set(elementary.ELM_NOTIFY_ORIENT_BOTTOM)
            nt.repeat_events_set(True)

            lb = elementary.Label(nt)
            lb.text_set("Save Error " + str(err))
            nt.content_set(lb)
            lb.show()

            nt.show()
            self.block(False)

        self.block(True)
        self.e.save(save_ok, save_error)

    def save_as(self):
        self.callback_destroy_del(self._destroy_cb)

        def popup_cancel(bt):
            win.hide()
            win.delete()
            self.block(False)
            self.callback_destroy_add(self._destroy_cb)

        def save(bt, mode=None):
            new_file = fs.file

            if not new_file:
                notification = ErrorNotify(win)
                notification.title = "Please set the filename"
                notification.action_add("Ok", notify_close, None, notification)
                notification.show()
                return

            if not accepted_filetype(new_file):
                new_file += ".edj"

            self.e.save_as(new_file, popup_cancel, save_error, mode)

        def save_error(err):
            if isinstance(err, IOError) and err.errno == errno.EEXIST:
                notification = ErrorNotify(win)
                notification.title = str(err).replace(':', '<br>')
                notification.action_add("Abort", notify_close,
                                        None, notification)
                notification.action_add("Overwrite", notify_overwrite,
                                        None, notification)
                notification.show()
            else:
                notification = ErrorNotify(win)
                notification.title = str(err).replace(':', '<br>')
                notification.action_add("Ok", notify_close, None, notification)
                notification.show()

        def notify_close(bt, notification):
            notification.hide()
            notification.delete()
            notification = None

        def notify_overwrite(bt, notification):
            notify_close(bt, notification)
            save(bt, swapfile.REPLACE)

        self.block(True)

        win = elementary.Window("fileselector", elementary.ELM_WIN_BASIC)
        win.title_set("Save as")
        win.callback_destroy_add(popup_cancel)
        win.resize(600, 480)
        win.maximized_set(True)

        bg = elementary.Background(win)
        win.resize_object_add(bg)
        bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bg.show()

        fs = FileSelector(win)
        fs.filter = accepted_filetype
        fs.save = True
        fs.action_add("Cancel", popup_cancel)
        fs.action_add("Save", save)
        win.resize_object_add(fs)
        fs.show()

        win.show()

    def _file_get(self):
        return self.e.filename

    file = property(_file_get)

    def _group_set(self, group):
        self.e.group = group

    def _group_get(self):
        return self.e.group

    group = property(_group_get, _group_set)

    def block(self, value=True, object_over=None):

        def create_window_blocker():
            if self._window_blocker:
                return
            self._window_blocker = elementary.Layout(self)
            self._window_blocker.file_set(self.theme, "blocker")
            self._window_blocker.size_hint_weight_set(
                evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
            self.resize_object_add(self._window_blocker)
            self._window_blocker.disabled_set(True)
            if object_over:
                self._window_blocker.stack_below(object_over)
            self.main_layout.disabled_set(True)
            self._window_blocker.show()

        def delete_window_blocker():
            if not self._window_blocker:
                return

            self.main_layout.disabled_set(False)
            self._window_blocker.delete()
            self._window_blocker = None

        if value:
            create_window_blocker()
        else:
            delete_window_blocker()

    def desktop_block(self, bool):
        if bool:
            self.main_edje.signal_emit("desktop,blocker,enable", "")
            self.desktop.view.disabled_set(True)
        else:
            self.main_edje.signal_emit("desktop,blocker,disable", "")
            self.desktop.view.disabled_set(False)

    def select_group(self):
        grp_wiz = GroupSelectionWizard(
            self, switch_only=True,
            selected_set_cb=self._group_wizard_selection_set_cb,
            selected_get_cb=self._group_wizard_selection_get_cb,
            new_grp_cb=self._group_wizard_new_group_cb,
            check_grp_cb=self._group_wizard_check_group_cb,
            del_grp_cb=self._group_wizard_del_group_cb,
            error_get_cb=self._error_get_cb)
        grp_wiz.file_set(self.e.workfile)
        grp_wiz.open()

    def show(self):
        if not self.e.group:
            self.select_group()
        elementary.Window.show(self)
        self.focus_set(True)

    def _group_wizard_check_group_cb(self, grp_name):
        return self.e.group_exists(grp_name)

    def _group_wizard_new_group_cb(self, grp_name):
        r = self.e.group_add(grp_name)
        if r:
            self._operation_stack_clean()

        return r

    def _group_wizard_del_group_cb(self, grp_name):
        return self.e.group_del(grp_name)

    def _error_get_cb(self):
        return self.e.error

    def _group_wizard_selection_set_cb(self, selection):
        if not selection:
            return False

        if self.group != selection:
            self._operation_stack_clean()
            self._context_clean()
            self._mode_set_cb(self._modes_selector, None, "Parts")
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

    # if local == True, we'll get fonts from *edje*
    # if local == False, we'll get fonts from *system*
    def _font_wizard_font_list_get_cb(self, local):
        if local:
            return self.e.fonts_get()
        else:
            return self.main_edje.evas.font_available_list()

    def _workfile_name_get_cb(self):
        return self.e.workfile

    def _part_object_get_cb(self, name):
        return self.e.part_object_get(name)

    ###########
    # DESKTOP
    ###########
    def _get_view(self):  # HACK
        return self

    view = property(_get_view)

    def _desktop_init(self):
        self.desktop = Desktop(self, self._operation_stack)
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

        self._group_name_entry = elementary.ScrolledEntry(self)
        self._group_name_entry.context_menu_disabled_set(True)
        self._group_name_entry.size_hint_weight_set(1.0, 1.0)
        self._group_name_entry.size_hint_align_set(-1.0, -1.0)
        self._group_name_entry.single_line_set(True)
        self._group_name_entry.style_set("editje")
        self._group_name_entry.tooltip_text_set("Name of current visible group.<br>Click to rename it.")
        self._group_name_entry.callback_activated_add(self._group_name_changed)
        self._group_name_entry.callback_unfocused_add(self._group_name_changed)
        self.main_layout.content_set("details_group.swallow",
                                     self._group_name_entry)

        self._toolbar_group_cb(self, "< none >")
        self.e.callback_add("group.changed", self._toolbar_group_cb)

        self._toolbar_bt_init(self.main_edje, "new.bt", "New", self._new_cb,
                              "Create new file in<br>another window.")
        self._toolbar_bt_init(self.main_edje, "open.bt", "Open", self._open_cb,
                              "Open one file in<br>another window.")
        self._toolbar_bt_init(self.main_edje, "save.bt", "Save", self._save_cb,
                              "Save current file.")
        self._toolbar_bt_init(self.main_edje, "undo.bt", "Undo", self._undo_cb,
                              "Undo last action.")
        self._toolbar_bt_init(self.main_edje, "redo.bt", "Redo", self._redo_cb,
                              "Redo last undone action.")
        self._toolbar_bt_init(self.main_edje, "group.bt", "Groups",
                              self._group_cb, "Manage the file groups.")
        if self._slave_mode:
            self.main_edje.signal_emit("group.bt,disable", "")

        self.main_edje.signal_emit("undo.bt,disable", "")
        self.main_edje.signal_emit("redo.bt,disable", "")

        self._toolbar_bt_init(self.main_edje, "run.bt", "Run", self._run_cb,
                              "Execute actual group<br>in new window.")

        self._toolbar_bt_init(self.main_edje, "options.bt", "Options",
                              self._options_cb, "Change Editje options.")

    def _group_name_changed(self, obj, *args, **kwargs):

        def group_rename(new_name):
            try:
                self.e.group_rename(new_name)
            except Exception, e:
                notification = self.notify(str(e))
                return False
                return False
            return True

        old_name = self.e.group
        new_name = obj.entry_get().replace("<br>", "")
        if old_name == new_name:
            return
        if group_rename(new_name):
            op = Operation("group renaming")

            op.redo_callback_add(group_rename, new_name)
            op.redo_callback_add(obj.entry_set, new_name)

            op.undo_callback_add(group_rename, old_name)
            op.undo_callback_add(obj.entry_set, old_name)

            # no need to switch back to working mode
            self._operation_stack(op, False)
        else:
            # TODO: notify the user of renaming failure
            obj.entry_set(old_name)

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
            del_grp_cb=self._group_wizard_del_group_cb,
            error_get_cb=self._error_get_cb)

        grp_wiz.file_set(self.e.workfile)
        grp_wiz.open()

    def _toolbar_bt_init(self, edje, part, name, callback, tooltip=None):
        edje.part_text_set(part + ".label", name)
        edje.signal_callback_add("%s,selected" % part, part, callback)

    def _new_cb(self, obj, emission, source):
        self.open_new()

    def _open_cb(self, obj, emission, source):
        self._openfile(self.open, os.path.dirname(self.e.filename))

    def _save_cb(self, obj, emission, source):
        self.save()

    def _run_cb(self, obj, emission, source):

        def test_window_closed(obj):
            #obj.delete()
            self.block(False)
            self._child_to_close.remove(w)
        w = elementary.Window("edje-test", elementary.ELM_WIN_BASIC)
        w.callback_destroy_add(test_window_closed)
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
        w.size_hint_min_set(*self.e.group_min)
        w.size_hint_max_set(*self.e.group_max)
        w.show()
        self._child_to_close.append(w)
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

    def _operation_stack_clean(self):
        self._op_stack_ptr = 0
        del self._op_stack[1:]
        self.main_edje.signal_emit("undo.bt,disable", "")
        self.main_edje.signal_emit("redo.bt,disable", "")

    def _operation_stack(self, op, keep_mode=True):
        # just for safeness
        if not isinstance(op, Operation):
            raise TypeError("Only Operation objects may enter the undo list.")

        mode = None
        if keep_mode:
            mode = self.mode

        sz = len(self._op_stack) - 1
        if self._op_stack_ptr != sz:
            del self._op_stack[self._op_stack_ptr + 1:]
            self.main_edje.signal_emit("redo.bt,disable", "")

        self._op_stack.append([op, mode])

        self._op_stack_ptr += 1
        self.main_edje.signal_emit("undo.bt,enable", "")

    def _unstack_pre_hooks_do(self):
        op, mode = self._op_stack[self._op_stack_ptr]
        if mode:
            self._mode_set_cb(self._modes_selector, None, mode)

        return op

    def _undo_cb(self, obj, emission, source):
        if self._op_stack_ptr == 0:  # can't go back
            return

        op = self._unstack_pre_hooks_do()
        op.undo()
        self._op_stack_ptr -= 1

        self.main_edje.signal_emit("redo.bt,enable", "")
        if self._op_stack_ptr == 0:
            self.main_edje.signal_emit("undo.bt,disable", "")
        else:
            self.main_edje.signal_emit("undo.bt,enable", "")

    def _redo_cb(self, obj, emission, source):
        sz = len(self._op_stack) - 1
        if self._op_stack_ptr == sz:  # can't advance
            return

        self._op_stack_ptr += 1
        op = self._unstack_pre_hooks_do()
        op.redo()

        self.main_edje.signal_emit("undo.bt,enable", "")
        if self._op_stack_ptr == sz:
            self.main_edje.signal_emit("redo.bt,disable", "")
        else:
            self.main_edje.signal_emit("redo.bt,enable", "")

    def _cut_cb(self, obj, emission, source):
        if not self._copy_cb(obj, emission, source):
            return

        name = self._clipboard.name

        op = Operation("cut part: " + name)
        op.undo_callback_add(self.e.part_add_bydata, self._clipboard)
        op.redo_callback_add(self.e.part_del, name)
        self._operation_stack(op)
        op.redo()

    def _copy_cb(self, obj, emission, source):
        if not self.e.part.name:
            return False

        self._clipboard = objects_data.Part(self.e.part_get(self.e.part.name))
        return True

    def _paste_cb(self, obj, emission, source):
        if not self._clipboard:
            return

        new_part_name = \
            name_generate(self._clipboard.name, self.e.parts)

        op = Operation("paste part: " + new_part_name)
        op.undo_callback_add(self.e.part_del, new_part_name)
        op.redo_callback_add(self.e.part_add_bydata, self._clipboard, \
                                 name=new_part_name)
        self._operation_stack(op)
        op.redo()

    def _animation_toolbar_set(self, state="disabled"):
        stop_sig = "disable"
        other_sig = "disable"

        if state == "playing":
            stop_sig = "enable"
        elif state == "stopped":
            other_sig = "enable"

        self._anim_toolbar_edje.signal_emit("stop.bt,%s" % stop_sig, "")

        self._anim_toolbar_edje.signal_emit("play.bt,%s" % other_sig, "")
        self._anim_toolbar_edje.signal_emit("previous.bt,%s" % other_sig, "")
        self._anim_toolbar_edje.signal_emit("next.bt,%s" % other_sig, "")

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

        def states_drop(emissor, data):
            part_name = data

            if not hasattr(self, "_prevstates"):
                return

            l = len(self._prevstates)
            while l != 0:
                l = l - 1
                p_name, s_name = self._prevstates[l]
                if p_name == part_name:
                    self._prevstates.pop(l)

        self.e.callback_add("part.removed", states_drop)
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
        bx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.main_layout.content_set("objects", bx)
        bx.show()

        self._modes_selector = elementary.Hoversel(self)
        self._modes_selector.hover_parent_set(self)
        self._modes_selector.tooltip_text_set("Change between the edition"
                                              " modes:<br>"
                                              "Parts, Animations and Signals")
        self._modes_selector.text_set("Mode")
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

    def _mode_get(self):
        return self._mode

    mode = property(fget=_mode_get)

    def _context_clean(self):
        if hasattr(self, "_prevparts"):
            del self._prevparts

    def _context_save(self, mode):

        def parts_save():
            if hasattr(self, "_prevparts"):
                return

            self._prevparts = []
            for p_name in self.e.parts:
                real_part = self.e.part_get(p_name)
                s_name = real_part.state_selected_get()
                self._prevparts.append((p_name, s_name))

        def animations_save():
            pass

        def signals_save():
            pass

        save_contexts = \
            {"Parts": parts_save, "Animations": animations_save,
             "Signals": signals_save}

        save_contexts[mode]()

    def _context_restore(self, mode):
        def parts_restore():
            if not hasattr(self, "_prevparts"):
                return

            for p_name, s_name in self._prevparts:
                real_part = self.e.part_get(p_name)
                real_part.state_selected_set(*s_name)
                if p_name == self.e.part.name:
                    self.e.part.state.name = s_name[0]
            del self._prevparts

            self.e.animation.name = None
            self.e.signal.name = None

        def animations_restore():
            self.e.part.name = None
            self.e.signal.name = None
            # Animation is being deselected here too to trigger
            # desktop_block when this change is coming from Parts Mode
            self.e.animation.name = None

        def signals_restore():
            self.e.part.name = None
            self.e.animation.name = None

        restore_contexts = \
            {"Parts": parts_restore, "Animations": animations_restore,
             "Signals": signals_restore}

        restore_contexts[mode]()

    def _mode_set_cb(self, obj, it, name, *args, **kwargs):
        if self.mode == name:
            return

        item, toolbar, mainbar, sidebar = self._modes[name]
        self._toolbar_pager.content_promote(toolbar)
        self._mainbar_pager.content_promote(mainbar)
        self._sidebar_pager.content_promote(sidebar)

        if self.mode:
            self._context_save(self.mode)
        self._mode = name

        if name == "Signals":
            self.main_edje.signal_emit("mode,anim,off", "editje")
            self.desktop_block(True)
        elif name == "Animations":
            self.main_edje.signal_emit("mode,anim,on", "editje")
        else:
            self.main_edje.signal_emit("mode,anim,off", "editje")
            self.desktop_block(False)

        self._context_restore(self.mode)

        self._modes_selector.text_set("Mode: " + self.mode)

        theme_file, group, edje_type = item.icon_get()
        ico = elementary.Icon(self)
        ico.file_set(theme_file, group)
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
        self._toolbar_bt_init(edj, "image_list.bt", "Images",
                              self._image_list_cb, "Manage loaded images.")
        self._toolbar_bt_init(edj, "font_list.bt", "Fonts", self._font_list_cb,
                              "Manage fonts.")
        self._toolbar_bt_init(edj, "cut.bt", "Cut", self._cut_cb,
                              "Cut current selection.")
        self._toolbar_bt_init(edj, "copy.bt", "Copy", self._copy_cb,
                              "Copy current selection.")
        self._toolbar_bt_init(edj, "paste.bt", "Paste", self._paste_cb,
                              "Paste current selection,<br>with valid name.")
        self._toolbar_bt_init(edj, "about.bt", "About", self._about_cb,
                              "Show about Editje window.")

        # Mainbar
        mainbar = CollapsablesBox(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        parts_list = PartsList(self, self.e, self._operation_stack,
                img_new_img_cb=self._image_wizard_new_image_cb,
                img_list_get_cb=self._image_wizard_image_list_get_cb,
                img_id_get_cb=self._image_wizard_image_id_get_cb,
                workfile_name_get_cb=self._workfile_name_get_cb)
        parts_list.title = "Parts"
        parts_list.options = True
        parts_list.open = True
        mainbar.pack_end(parts_list)
        parts_list.show()

        widgets_list = WidgetsList(self, self.e, self._operation_stack,
                img_new_img_cb=self._image_wizard_new_image_cb,
                img_list_get_cb=self._image_wizard_image_list_get_cb,
                img_id_get_cb=self._image_wizard_image_id_get_cb,
                workfile_name_get_cb=self._workfile_name_get_cb)
        widgets_list.title = "Widgets"
        widgets_list.open = True
        mainbar.pack_end(widgets_list)
        widgets_list.show()

        # Sidebar
        sidebar = self._parts_sidebar_create()

        self._mode_add("Parts", "editje/icon/part",
                       toolbar, mainbar, sidebar)

    def _parts_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.group_details = GroupDetails(self, self._operation_stack)
        box.pack_end(self.group_details)

        self.part_details = PartDetails(self, self._operation_stack)
        box.pack_end(self.part_details)

        self.part_state_details = PartStateDetails(
            self, self._operation_stack,
            img_new_img_cb=self._image_wizard_new_image_cb,
            img_list_get_cb=self._image_wizard_image_list_get_cb,
            img_id_get_cb=self._image_wizard_image_id_get_cb,
            fnt_new_fnt_cb=self._font_wizard_new_font_cb,
            fnt_list_get_cb=self._font_wizard_font_list_get_cb,
            workfile_name_get_cb=self._workfile_name_get_cb,
            part_object_get_cb=self._part_object_get_cb)

        box.pack_end(self.part_state_details)

        return self._set_scrolled_contents(box)

    # Animations

    def _animations_init(self):
        self.e.animation.callback_add("parts.blocked.changed",
                                      self._parts_block_cb)

        # Toolbar
        toolbar = elementary.Layout(self)
        toolbar.file_set(self.theme, "toolbar.anim")
        toolbar.show()
        self._delay = None

        def play_end():
            self.e.animation.state = 0.0
            self._animation_toolbar_set("stopped")
            self._delay = None
            return False

        def play_end_cb(emissor, data):
            self._delay = ecore.timer_add(0.5, play_end)

        self.e.animation.callback_add("animation.play.end", play_end_cb)

        def play_cb(obj, emission, source):
            self.e.part.name = ""
            self.e.animation.play()
            self._animation_toolbar_set("playing")

        def stop_cb(obj, emission, source):
            self.e.animation.stop()
            self._animation_toolbar_set("stopped")

        def previous_cb(obj, emission, source):
            self.e.animation.state_prev_goto()

        def next_cb(obj, emission, source):
            self.e.animation.state_next_goto()

        self._anim_toolbar_edje = toolbar.edje_get()
        self._toolbar_bt_init(self._anim_toolbar_edje, "previous.bt",
                              "Previous", previous_cb, "Go to previous keyframe.")
        self._toolbar_bt_init(self._anim_toolbar_edje, "play.bt",
                              "Play", play_cb, "Play animation<br>from current keyframe.")
        self._toolbar_bt_init(self._anim_toolbar_edje, "next.bt",
                              "Next", next_cb, "Go to next keyframe.")
        self._toolbar_bt_init(self._anim_toolbar_edje, "stop.bt",
                              "Stop", stop_cb, "Stop animation")

        def _animation_changed(it, ti):
            self._animation_toolbar_set("stopped")
            if self.mode == "Animations":
                self.desktop_block(False)

        def _animation_unselected(it, ti):
            self._animation_toolbar_set("disabled")
            if self.mode == "Animations":
                self.desktop_block(True)

        self.e.animation.callback_add("animation.changed", _animation_changed)
        self.e.animation.callback_add(
            "animation.unselected", _animation_unselected)

        # Mainbar
        mainbar = CollapsablesBox(self)
        mainbar.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        mainbar.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
        mainbar.show()

        def new_anim_cb(name, parts):
            r = self.e.animation_add(name, parts)
            if not r:
                return r

            op = Operation("new animation: %s " % name)

            op.undo_callback_add(self.e.animation_del, name)
            op.redo_callback_add(self.e.animation_add, name, parts)
            self._operation_stack(op)

            return r

        def anims_list_cb():
            return self.e.animations

        def parts_list_cb():
            return self.e.parts

        animations_list = AnimationsList(self, new_anim_cb, anims_list_cb,
                              parts_list_cb, self._operation_stack)
        animations_list.options = True
        animations_list.title = "Animations"
        animations_list.open = True
        mainbar.pack_end(animations_list)
        animations_list.show()

        animations_list = AnimationsPartsList(
            self, self.e, self._operation_stack)
        animations_list.title = "Participating Parts"
        animations_list.open = True
        mainbar.pack_end(animations_list)
        animations_list.show()

        # Sidebar
        sidebar = self._animations_sidebar_create()

        self._mode_add("Animations", "editje/icon/animation",
                       toolbar, mainbar, sidebar)

    def _animations_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        self.anim_details = AnimationDetails(self, self._operation_stack)
        box.pack_end(self.anim_details)
        self.anim_details.show()

        self.anim_state_details = PartAnimStateDetails(
            self, self._operation_stack,
            img_new_img_cb=self._image_wizard_new_image_cb,
            img_list_get_cb=self._image_wizard_image_list_get_cb,
            img_id_get_cb=self._image_wizard_image_id_get_cb,
            fnt_new_fnt_cb=self._font_wizard_new_font_cb,
            fnt_list_get_cb=self._font_wizard_font_list_get_cb,
            workfile_name_get_cb=self._workfile_name_get_cb,
            part_object_get_cb=self._part_object_get_cb)  # fix

        self.anim_state_details.open = True
        self.anim_state_details.open_disable = True
        box.pack_end(self.anim_state_details)

        return self._set_scrolled_contents(box)

    def _parts_block_cb(self, emissor, data):
        self.desktop.parts_block(data)

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
            r = self.e.signal_add(name, type_)
            if not r:
                return r

            op = Operation("new signal: %s (type %s)" % (name, type_))

            op.undo_callback_add(self.e.signal_del, name)
            op.redo_callback_add(self.e.signal_add, name, type_)
            self._operation_stack(op)

            return r

        def sigs_list_cb():
            return self.e.signals

        signals_list = SignalsList(
            self, self.e, new_sig_cb, sigs_list_cb, self._operation_stack)
        signals_list.title = "Signals"
        signals_list.open = True
        signals_list.options = True
        mainbar.pack_end(signals_list)
        signals_list.show()

        # Sidebar
        sidebar = self._signals_sidebar_create()

        self._mode_add("Signals", "editje/icon/signal",
                       toolbar, mainbar, sidebar)

    def _signals_sidebar_create(self):
        box = CollapsablesBox(self)
        box.size_hint_weight_set(1.0, 0.0)
        box.size_hint_align_set(-1.0, 0.0)
        box.show()

        signal_details = SignalDetails(self, self._operation_stack)
        signal_details.open = False
        signal_details.open_disable = True
        signal_details.show()

        box.pack_end(signal_details)

        return self._set_scrolled_contents(box)

    def notify(self, message):
        if self.__notification:
            self.__notification.hide()
            self.__notification.delete()
            self.__notification = None
        self.__notification = elementary.Notify(self)
        self.__notification.timeout_set(1)
        self.__notification.orient_set(elementary.ELM_NOTIFY_ORIENT_BOTTOM)

        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self.__notification.content_set(bx)
        bx.show()

        lb = elementary.Label(self)
        lb.text_set(message)
        bx.pack_end(lb)
        lb.show()

        self.__notification.show()
