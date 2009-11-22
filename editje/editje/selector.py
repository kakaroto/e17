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
import elementary

class Selector(elementary.Window):

    def __init__(self):
        elementary.Window.__init__(self, "editje.open", elementary.ELM_WIN_BASIC)
        self.title_set("Editje - Open")
        self.destroy = self._destroy_cb
        self.resize(400, 400)

        self.file = ""
        self.group = ""
        self.done = self._done_default

        self.bg = elementary.Background(self)
        self.bg.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(self.bg)
        self.bg.show()

        self.pager = elementary.Pager(self)
        self.pager.size_hint_weight_set(1.0, 1.0)
        self.resize_object_add(self.pager)
        self.pager.show()

        self._fileselector_init()
        self._groupselector_init()

        self.pager.content_promote(self.fs)

    def _fileselector_init(self):
        self.fs = elementary.Fileselector(self)
        self.fs.done = self._fileselector_done
        self.fs.is_save_set(False)
        self.fs.expandable_set(False)
        self.fs.path_set(os.getenv("PWD"))
        self.fs.size_hint_weight_set(1.0, 1.0)
        self.fs.size_hint_align_set(-1.0, -1.0)
        self.pager.content_push(self.fs)
        self.fs.show()

    def _fileselector_done(self, fs, selected, *args, **kwargs):
        if selected:
            self.file = fs.selected_get()
            if self.file:
                self._groupselector_update()
                self.pager.content_promote(self.gs)
        else:
            self._destroy_cb(fs)

    def _groupselector_update(self):
        self.groups.clear()
        self.groups.item_append(self.groups_list[0], None, None,
                                self._groupselector_selected_cb,
                                self.groups_list[0]).selected_set(True)
        for group in self.groups_list[1:]:
            self.groups.item_append(group, None, None,
                                    self._groupselector_selected_cb,
                                    group)
        self.groups.go()

    def _groupselector_init(self):
        self.gs = elementary.Box(self)
        self.gs.size_hint_weight_set(1.0, 1.0)
        self.gs.size_hint_align_set(-1.0, -1.0)

        self.preview = edje.Edje(self.evas, file=self.file, group=self.group)

        self.preview_scr = elementary.Scroller(self)
        self.preview_scr.size_hint_weight_set(1.0, 1.0)
        self.preview_scr.size_hint_align_set(-1.0, -1.0)
        self.preview_scr.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                                    elementary.ELM_SCROLLER_POLICY_AUTO)
        self.preview_scr.content_set(self.preview)

        self.gs.pack_end(self.preview_scr)
        self.preview_scr.bounce_set(False, False)
        self.preview_scr.show()

        self.groups = elementary.List(self)
        self.groups.size_hint_weight_set(1.0, 1.0)
        self.groups.size_hint_align_set(-1.0, -1.0)
        self.gs.pack_end(self.groups)
        self.groups.show()

        hbox = elementary.Box(self)
        hbox.horizontal_set(True)
        self.gs.pack_end(hbox)
        hbox.show()

        cancel = elementary.Button(self)
        cancel.label_set("Cancel")
        cancel.clicked = self._groupselector_cancel
        hbox.pack_end(cancel)
        cancel.show()

        done = elementary.Button(self)
        done.label_set("Done")
        done.clicked = self._groupselector_done
        hbox.pack_end(done)
        done.show()

        self.pager.content_push(self.gs)
        self.gs.show()

    def _groupselector_selected_cb(self, li, it, group, *args, **kwargs):
        self.group = group
        if self.group:
            self.preview.file_set(self.file, self.group)

    def _groupselector_cancel(self, bt, *args, **kwargs):
        self.group = ""
        self.pager.content_promote(self.fs)

    def _groupselector_done(self, bt, *args, **kwargs):
        self.hide()
        self._done(self.file, self.group)
        self.delete()

    def _file_set(self, value):
        self.groups_list = edje.file_collection_list(value)
        if self.groups_list:
            self._file = value
        else:
            self._file = ""

    def _file_get(self):
        return self._file

    file = property(_file_get, _file_set)

    def _group_set(self, value):
        if value in self.groups_list:
            self._group = value
        else:
            self._group = ""

    def _group_get(self):
        return self._group

    group = property(_group_get, _group_set)

    def _destroy_cb(self, obj, *args, **kwargs):
        self.hide()
        self.delete()
        elementary.exit()

    def show(self):
        if self.group:
            self.hide()
            self._done(self.file, self.group)
            self.delete()
            return
        elif self.file:
            self._groupselector_update()
            self.pager.content_promote(self.gs)
        elementary.Window.show(self)

    def _done_default(self, file, group):
        print file, group

    def _done_set(self, value):
        self._done = value

    done = property(fset=_done_set)


if __name__ == "__main__":
    elementary.init()

    sel = Selector()
    sel.show()

    elementary.run()
    elementary.shutdown()
