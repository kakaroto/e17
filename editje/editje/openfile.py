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
import shutil
import string

import evas
import elementary

from editje import Editje
from fileselector import FileSelector
import swapfile
import sysconfig
from error_notify import ErrorNotify

class OpenFile(elementary.Window):
    def __init__(self, theme="default"):

        self.theme = sysconfig.theme_file_get(theme)

        elementary.Window.__init__(self, "openfile",
                                   elementary.ELM_WIN_BASIC)
        self.title_set("Open Edje")
        self.autodel_set(True)

        self._notification = None
        self._swapfile = swapfile.SwapFile()

        bg = elementary.Background(self)
        self.resize_object_add(bg)
        bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bg.show()

        self._pager = elementary.Pager(self)
        self._pager.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self._pager.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self.resize_object_add(self._pager)
        self._pager.show()

        self._fs = FileSelector(self)
        self._fs.filter = self._filter
        self._fs.action_add("New", self._new)
        self._fs.action_add("Cancel", self._cancel)
        self._fs.action_add("Ok", self._open)
        self._fs.action_disabled_set("Ok", True)
        self._fs.callback_add("file.selected", self._file_selected)
        self._fs.callback_add("file.selection_clear", self._file_unselected)
        self._fs.show()
        self._pager.content_push(self._fs)

        self.resize(600, 480)

    def _file_selected(self, obj, data):
        self._fs.action_disabled_set("Ok", False)

    def _file_unselected(self, obj, data):
        self._fs.action_disabled_set("Ok", True)

    def _filter(self, file):
        return file.endswith(".edc") or file.endswith(".edj")

    def _path_set(self, value):
        self._fs.path = value

    path = property(fset=_path_set)

    def _open(self, bt, mode=None):
        try:
            self._swapfile.file = self._fs.file
            self._swapfile.open(mode)
        except Exception, e:
            self._notify_err(e)
            return

        editje = Editje(self._swapfile)
        editje.show()
        self._cancel(bt)

    def _open_forced(self, bt, data):
        self._open(bt, swapfile.REPLACE)

    def _open_recovery(self, bt, data):
        self._open(bt, swapfile.RESTORE)

    def list_files_on_diretory(self):
        path = os.getenv("PWD")
        list = os.listdir(path)
        list.sort(key=str.lower)
        files = []
        for file in list:
            if not file.startswith("."):
                full = os.path.join(path, file)
                if os.path.isfile(full):
                    if file.endswith(".edj"):
                        files.append(file)
        return files


    def _new(self, bt):
        self._new_popup()
        return

        self._swapfile.file = ""
        self._swapfile.new = True
        self._swapfile.open()

        editje = Editje(self._swapfile)
        editje.show()
        self._cancel(bt)
#        self._templates_load()

    def _cancel(self, bt):
        self.hide()
        self.delete()

    def _notify(self, message):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None
        self._notification = elementary.Notify(self)
        self._notification.timeout_set(2)
        self._notification.orient_set(elementary.ELM_NOTIFY_ORIENT_BOTTOM_RIGHT)

        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self._notification.content_set(bx)
        bx.show()

        lb = elementary.Label(self)
        lb.label_set(message)
        bx.pack_end(lb)
        lb.show()

        self._notification.show()

    def _notify_err(self, err):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None

        self._notification = ErrorNotify(self)

        if isinstance(err, swapfile.CacheAlreadyExists):
            self._notification.title = "Swap file already exists"
            lb = elementary.Label(self._notification)
            lb.label_set("Another program may be editing the same file.<br>" +
                         "Or an edit session for this file crashed.")
            self._notification.pack_end(lb)
            lb.show()
            self._notification.action_add("Ignore Swap", self._open_forced)
            self._notification.action_add("Recovery", self._open_recovery)
            self._notification.action_add("Abort", self._notify_abort)
        elif isinstance(err, swapfile.CompileError):
            self._notification.title = "Compiler Error"
            lb = elementary.Label(self._notification)
            lb.label_set(string.replace(str(err.message), '\n', '<br>'))
            self._notification.pack_end(lb)
            lb.show()
            self._notification.action_add("Ok", self._notify_abort)
        else:
            self._notification.title = string.replace(str(err), ':', '<br>')
            self._notification.action_add("Ok", self._notify_abort)

        self._notification.show()

    def _notify_abort(self, bt, data):
        self._notification.hide()
        self._notification.delete()
        self._notification = None

    def _templates_load(self):
        tb = elementary.Table(self)
        tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        tb.size_hint_align_set(evas.EVAS_HINT_FILL,
                               evas.EVAS_HINT_FILL)
        tb.show()
        self._pager.content_push(tb)


    def _templates_cancel(self):
        self._pager.content_pop()

    def _templates_ok(self):
        pass



    # HACK
    def _new_popup(self):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None

        self._notification = ErrorNotify(self)

        self._notification.title = "        New Edje File       "
        
        bx = elementary.Box(self._notification)
        bx.horizontal_set(True)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._notification.pack_end(bx)
        bx.show()

        lb = elementary.Label(bx)
        lb.label_set("Filename:")
        bx.pack_start(lb)
        lb.show()

        sc = elementary.Scroller(bx)
        sc.content_min_limit(0, 1)
        sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                      elementary.ELM_SCROLLER_POLICY_OFF)
        sc.bounce_set(False, False)
        sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        bx.pack_end(sc)
        sc.show()

        self._new_entry = elementary.Entry(bx)
        self._new_entry.entry_set("filename")
        self._new_entry.single_line_set(True)
        self._new_entry.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                            evas.EVAS_HINT_EXPAND)
        self._new_entry.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        sc.content_set(self._new_entry)
        self._new_entry.callback_changed_add(self._name_changed_cb)
        self._new_entry.show()

        self._notification.action_add("Cancel", self._notify_abort)
        self._notification.action_add("Create", self._new_popup_create)

        self._notification.show()

    def _new_popup_create(self, bt, mode=None):
        name = self._new_entry.entry_get()

        if not name.endswith(".edj"):
            name += ".edj"
        file = os.path.join(self._fs.path, name)
        self._notify_abort(bt, None)

        if os.path.isfile(file):
            if self._notification:
                self._notification.hide()
                self._notification.delete()
                self._notification = None
            self._notification = ErrorNotify(self)
            self._notification.title = "File already exists"
            self._notification.action_add("Rename", self._new_rename)
            self._notification.action_add("Overwrite", self._new_forced, None, file)
            self._notification.action_add("Abort", self._notify_abort)
            self._notification.show()
            return

        shutil.copyfile(sysconfig.template_file_get("default"), file)
        self._new_open(file)
        
    def _new_forced(self, bt, file):
        self._notify_abort(bt, None)
        shutil.copyfile(sysconfig.template_file_get("default"), file)
        self._new_open(file)
        
    def _new_rename(self, bt, data):
        self._notify_abort(bt, None)
        self._new(bt, None)

    def _new_open(self, file):
        try:
            self._swapfile.file = file
            self._swapfile.open(swapfile.REPLACE)
        except Exception, e:
            self._notify_err(e)
            return

        editje = Editje(self._swapfile)
        editje.show()
        self._cancel(None)

    def _name_changed_cb(self, obj):
        self._name_chaged = True
        self._check_name()

    def _check_name(self):
        name = self._new_entry.entry_get()
        if name:
            self._notification.action_disabled_set("Create", False)
        else:
            self._notification.action_disabled_set("Create", True)
