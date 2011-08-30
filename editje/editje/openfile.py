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

import evas
import elementary

from fileselector import FileSelector
import swapfile
from error_notify import ErrorNotify


class OpenFileManager(object):
    __openfile = None

    def _openfile(self, open_cb, dir=None):
        if self.__class__.__openfile:
            self.__class__.__openfile.show()
            return
        self.__class__.__openfile = OpenFile(open_cb)
        self.__class__.__openfile.on_del_add(self.__openfile_on_del_cb)
        if dir:
            self.__class__.__openfile.path = dir
        self.__class__.__openfile.show()

    def __openfile_on_del_cb(self, obj):
        if obj == self.__class__.__openfile:
            self.__class__.__openfile.on_del_del(self.__openfile_on_del_cb)
            self.__class__.__openfile = None

    def _openfile_close(self):
        self.__class__.__openfile.delete()


class OpenFile(elementary.Window):
    _notification = None

    def __init__(self, open_cb, *args, **kargs):
        elementary.Window.__init__(self, "openfile",
                                   elementary.ELM_WIN_BASIC)
        self.title_set("Open file")
        self.autodel_set(True)

        self._open_cb = (open_cb, args, kargs)

        bg = elementary.Background(self)
        self.resize_object_add(bg)
        bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bg.show()

        self._fs = FileSelector(self)
        self._fs.filter = self._filter
        self._fs.action_add("Cancel", self._cancel)
        self._fs.action_add("Ok", self._open)
        self._fs.action_disabled_set("Ok", True)
        self._fs.callback_add("file.clicked", self._file_clicked)
        self._fs.callback_add("file.selected", self._file_selected)
        self._fs.callback_add("file.selection_clear", self._file_unselected)
        self.resize_object_add(self._fs)
        self._fs.show()

        self.resize(600, 480)

    def show(self):
        self.activate()
        elementary.Window.show(self)

    def _file_clicked(self, obj, data):
        self._open(None)

    def _file_selected(self, obj, data):
        self._fs.action_disabled_set("Ok", False)

    def _file_unselected(self, obj, data):
        self._fs.action_disabled_set("Ok", True)

    def _filter(self, file):
        return file.endswith(".edc") or file.endswith(".edj")

    def _path_set(self, value):
        self._fs.path = value

    path = property(fset=_path_set)

    def _open_ok(self, sf, **kargs):
        self.hide()
        open_cb, args, kargs = self._open_cb
        open_cb(sf, *args, **kargs)
        self.delete()

    def _open(self, bt, mode=None):
        swapfile.open(self._fs.file, self._open_ok, self._notify_err, mode)

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

    def _cancel(self, bt):
        self.delete()

    def _notify_del(self):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None

    def _notify(self, message):
        self._notify_del()
        self._notification = elementary.Notify(self)
        self._notification.timeout_set(2)
        self._notification.orient_set(
            elementary.ELM_NOTIFY_ORIENT_BOTTOM_RIGHT)

        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self._notification.content_set(bx)
        bx.show()

        lb = elementary.Label(self)
        lb.text_set(message)
        bx.pack_end(lb)
        lb.show()

        self._notification.show()

    def _notify_err(self, err, **kargs):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None

        self._notification = ErrorNotify(self)

        if isinstance(err, IOError) and err.errno == errno.EEXIST:
            self._notification.title = "Swap file already exists"
            lb = elementary.Label(self._notification)
            lb.text_set(
                "Another program may be editing the same file<br>" +
                "or a previous edition session for this file crashed.")
            self._notification.pack_end(lb)
            lb.show()
            self._notification.action_add("Ignore Swap", self._open_forced)
            self._notification.action_add("Recover", self._open_recovery)
            self._notification.action_add("Abort", self._notify_abort)
        elif isinstance(err, swapfile.CompileError):
            self._notification.title = "Compiler Error"
            lb = elementary.Label(self._notification)
            lb.text_set(str(err.message).replace('\n', '<br>'))
            self._notification.pack_end(lb)
            lb.show()
            self._notification.action_add("Ok", self._notify_abort)
        else:
            self._notification.title = str(err).replace(':', '<br>')
            self._notification.action_add("Ok", self._notify_abort)

        self._notification.show()

    def _notify_abort(self, bt, data):
        self._notify_del()
