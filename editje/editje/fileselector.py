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
import errno

import evas
import elementary

from event_manager import Manager


class FileSelector(Manager, elementary.Table):
    def __init__(self, parent):
        Manager.__init__(self)
        self._parent = parent
        elementary.Table.__init__(self, parent)
        self.size_hint_align_set(evas.EVAS_HINT_FILL,
                                 evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                  evas.EVAS_HINT_EXPAND)
        self.homogenous_set(False)

        self._filter_call = None
        self._home = os.getenv("HOME")
        self._ls_dir = dict()

        self._navigator_init()
        self._files_init()
        self._actions_init()
        self._filter_init()

        self._path = ""
        self._file = ""
        self.save = False
        self.multi = False
        self.path = os.getenv("PWD")
        self.__actions_list = {}

        self._notification = None

    def _navigator_init(self):
        bx = elementary.Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        bx.size_hint_align_set(evas.EVAS_HINT_FILL,
                               evas.EVAS_HINT_FILL)
        self.pack(bx, 0, 0, 1, 4)
        bx.show()

        self._nav_home = elementary.Button(self)
        self._nav_home.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._nav_home.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        self._nav_home.text_set("Home")
        ic = elementary.Icon(self)
        ic.standard_set("home")
        ic.scale_set(0, 0)
        self._nav_home.icon_set(ic)
        self._nav_home.callback_clicked_add(self._home_load)
        bx.pack_end(self._nav_home)
        self._nav_home.show()

        sp = elementary.Separator(self)
        sp.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        sp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        sp.horizontal_set(True)
        bx.pack_end(sp)
        sp.show()

        self._nav_up = elementary.Button(self)
        self._nav_up.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._nav_up.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        self._nav_up.text_set("Up")
        ic = elementary.Icon(self)
        ic.standard_set("arrow_up")
        ic.scale_set(0, 0)
        self._nav_up.icon_set(ic)
        self._nav_up.callback_clicked_add(self._parent_load)
        bx.pack_end(self._nav_up)
        self._nav_up.show()

        self._directories = elementary.List(self)
        self._directories.size_hint_align_set(evas.EVAS_HINT_FILL,
                                            evas.EVAS_HINT_FILL)
        self._directories.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                               evas.EVAS_HINT_EXPAND)
        self._directories.callback_selected_add(self._folder_change)
        bx.pack_end(self._directories)
        self._directories.show()

    def _files_init(self):
        self._right_bx = elementary.Box(self)
        self._right_bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                            evas.EVAS_HINT_EXPAND)
        self._right_bx.size_hint_align_set(evas.EVAS_HINT_FILL,
                                           evas.EVAS_HINT_FILL)
        self.pack(self._right_bx, 1, 0, 3, 4)
        self._right_bx.show()


        self._nav_path = elementary.ScrolledEntry(self)
        self._nav_path.single_line_set(True)
        self._nav_path.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._nav_path.size_hint_align_set(evas.EVAS_HINT_FILL,
                                           evas.EVAS_HINT_FILL)
        self._nav_path.editable_set(False)
        self._nav_path.entry_set("PATH")
        self._nav_path.callback_anchor_clicked_add(self._path_go)
        self._nav_path.callback_changed_add(self._path_change)
        self._right_bx.pack_end(self._nav_path)
        self._nav_path.show()

        self._files = elementary.List(self)
        self._files.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self._files.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self._files.callback_selected_add(self._file_selected)
        self._files.callback_unselected_add(self._file_unselected)
        self._files.callback_clicked_add(self._file_clicked)
        self._right_bx.pack_end(self._files)
        self._files.show()

        self._file_entry = elementary.ScrolledEntry(self)
        self._file_entry.single_line_set(True)
        self._file_entry.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._file_entry.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._file_entry.editable_set(True)
        self._file_entry.entry_set("")
        self._file_entry.callback_changed_add(self._file_entry_change)
        self._right_bx.pack_end(self._file_entry)

    def _multi_set(self, value):
        if self.save and value:
            value = False
        self._files.multi_select_set(value)
        self._multi = value

        if not value:
            selected = self._files.selected_items_get()
            for i in selected[:-1]:
                i.selected_set(False)

    def _multi_get(self):
        return self._multi

    multi = property(_multi_get, _multi_set)

    def _save_set(self, value):
        self._save = value
        if self._save:
            self._right_bx.pack_end(self._file_entry)
            self._file_entry.show()
            self.multi = False
        else:
            self._file_entry.hide()
            self._right_bx.unpack(self._file_entry)

    def _save_get(self):
        return self._save

    save = property(_save_get, _save_set)

    def _actions_init(self):
        sp = elementary.Separator(self)
        sp.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        sp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        sp.size_hint_min_set(600, 1)
        sp.horizontal_set(True)
        self.pack(sp, 0, 4, 4, 1)
        sp.show()

        self._actions = elementary.Box(self)
        self._actions.horizontal_set(True)
        self._actions.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._actions.size_hint_align_set(1.0, evas.EVAS_HINT_FILL)
        self.pack(self._actions, 3, 5, 1, 1)
        self._actions.show()

    def _filter_init(self):
        bx = elementary.Box(self)
        bx.horizontal_set(True)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        bx.size_hint_align_set(evas.EVAS_HINT_FILL,
                               evas.EVAS_HINT_FILL)
        self.pack(bx, 0, 5, 2, 1)
        bx.show()

        self._hidden = elementary.Check(self)
        self._hidden.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._hidden.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._hidden.state_set(False)
        self._hidden.text_set("Show hidden files")
        self._hidden.callback_changed_add(self._update)
        bx.pack_end(self._hidden)
        self._hidden.show()

        self._filter = elementary.Check(self)
        self._filter.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._filter.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._filter.text_set("Filter extensions")
        self._filter.state_set(False)
        self._filter.callback_changed_add(self._update)
        bx.pack_end(self._filter)

    def _home_load(self, bt):
        self.path = self._home

    def _parent_load(self, bt):
        head, tail = os.path.split(self._path)
        self.path = head

    def _path_go(self, obj, en):
        print en.entry_get()
        return

    def _update(self, obj):
        self._ls_dir.clear()
        self._files.clear()
        self.event_emit("file.selection_clear", None)
        self._directories.clear()

        hidden = self._hidden.state_get()
        filter = self._filter.state_get()

        try:
            list = os.listdir(self.path)
        except OSError, e:
            if e.errno == errno.EACCES:
                self._notify("Permission denied: \'%s\'" % self.path)
                self.path = self.path.rsplit("/", 1)[0]
                return
        list.sort(key=str.lower)
        for file in list:
            if hidden or not file.startswith("."):
                full = os.path.join(self.path, file)
                if os.path.isdir(full):
                    ic = elementary.Icon(self)
                    ic.standard_set("folder")
                    ic.scale_set(0, 0)
                    it = self._directories.item_append(
                        file, ic, None, None, full)
                    self._ls_dir[file] = it
                elif os.path.isfile(full):
                    if not filter or self._filter_call(full):
                        ic = elementary.Icon(self)
                        ic.standard_set("file")
                        ic.scale_set(0, 0)
                        it = self._files.item_append(file, ic, None, None,
                                                       full)
                        self._ls_dir[file] = it

        self._files.go()
        self._directories.go()

    def _folder_change(self, li, id):
        self.path = li.selected_item_get().data_get()[0][0]

    def _path_change(self, en):
        self.path = self._nav_path.entry_get()

    def _file_selected(self, li, it):
        self._file_entry.entry_set(it.label_get())
        self.event_emit("file.selected", it.data_get()[0][0])

    def _file_unselected(self, li, it):
        self.event_emit("file.unselected", it.data_get()[0][0])
        if not self._files.selected_items_get():
            self.event_emit("file.selection_clear", None)

    def _file_clicked(self, li, it):
	    self.event_emit("file.clicked", it.data_get()[0][0])

    def _file_entry_change(self, en):
        if not self.save:
            return

        path = en.entry_get()
        if not path.endswith("/"):
            return

        it = self._ls_dir.get(path[:-1])
        if it:
            it.selected_set(True)
            en.entry_set("")
        else:
            it = self._files.selected_item_get()
            if it:
                it.selected_set(False)

    # PATH
    def _path_set(self, path):
        if path == self._path:
            return

        if os.path.isdir(path):
            self._path = path
            self.file = ""
            self._nav_path.entry_set(self._path)
            self._update(self)

    def _path_get(self):
        return self._path

    path = property(_path_get, _path_set)

    def _file_set(self, file):
        if file == self._file:
            return

        if os.path.isfile(file):
            self._file = file
            self.path = os.path.dirname(file)
            self._file_entry.entry_set(os.path.basename(file))
        else:
            self._file = ""

    def _file_get(self):
        if self.save:
            file = self._file_entry.entry_get()
            if not file:
                return file
            return os.path.join(self.path, self._file_entry.entry_get())

        item = self._files.selected_item_get()
        if item:
            return item.data_get()[0][0]
        return ""

    file = property(_file_get, _file_set)

    def _files_get(self):
        if self.save:
            return [self.file]

        ret = []
        for i in self._files.selected_items_get():
            ret.append(i.data_get()[0][0])
        return ret

    files = property(_files_get)

    def _filter_set(self, filter):
        if filter and self._filter_call != filter:
            self._filter.state_set(True)
            self._filter_call = filter
            self._filter.show()
            self._update(self)
        elif not filter and self._filter_call:
            self._filter.state_set(False)
            self._filter_call = None
            self._filter.hide()
            self._update(self)

    filter = property(fset=_filter_set)

    def selection_clear(self):
        for i in self._files.selected_items_get():
            i.selected_set(False)

    def action_add(self, label, func_cb, data=None, icon=None):
        btn = elementary.Button(self)
        btn.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        btn.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        btn.text_set(label)

        if func_cb:
            btn.callback_clicked_add(func_cb)
            btn.data["clicked"] = data

        if icon:
            ico = elementary.Icon(self)
            ico.file_set(self.__theme_file, "editje/icon/" + icon)
            btn.icon_set(ico)
            ico.show()

        self.__actions_list[label] = btn

        btn.show()
        self._actions.pack_end(btn)

    def action_disabled_set(self, label, disabled):
        self.__actions_list[label].disabled_set(disabled)

    def _notify_del(self):
        if self._notification:
            self._notification.hide()
            self._notification.delete()
            self._notification = None

    def _notify(self, message):
        self._notify_del()
        self._notification = elementary.Notify(self)
        self._notification.timeout_set(1)
        self._notification.orient_set(elementary.ELM_NOTIFY_ORIENT_BOTTOM)

        lb = elementary.Label(self)
        lb.text_set(message)
        self._notification.content_set(lb)
        lb.show()

        self._notification.show()

if __name__ == "__main__":
    elementary.init()
    elementary.policy_set(elementary.ELM_POLICY_QUIT,
                          elementary.ELM_POLICY_QUIT_LAST_WINDOW_CLOSED)
    win = elementary.Window("fileselector", elementary.ELM_WIN_BASIC)
    win.title_set("FileSelector")
    win.autodel_set(True)
    win.resize(600, 480)
    win.maximized_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    fs = FileSelector(win)
    win.resize_object_add(fs)

    def filter(file):
        return file.endswith(".edj")

    def ok(*args):
        print "Save:", fs.save
        print "File:", fs.file
        print "Files:", fs.files

    def save(*args):
        fs.save = not fs.save

    def multi(*args):
        fs.multi = not fs.multi

    fs.filter = filter
    fs.action_add("Ok", ok)
    fs.action_add("Save", save)
    fs.action_add("Multi", multi)
    fs.show()

    win.show()

    elementary.run()
    elementary.shutdown()
