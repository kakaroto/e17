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

import evas
import edje
import elementary

class GroupChange(elementary.InnerWindow):
    def __init__(self, parent, cancel=True):
        self._parent = parent
        elementary.InnerWindow.__init__(self, parent)
        self.title_set("Open Edje")
        self.autodel_set(True)
        self.resize(600, 480)

        self._notification = None

        self._gs = GroupSelector(self)
        if cancel:
            self._gs.action_add("Cancel", self._cancel)
        self._gs.action_add("Ok", self._ok)
        self.content_set(self._gs)
        self._gs.show()

    def _file_set(self, file):
        self._gs.file = file

    file = property(fset=_file_set)

    def open(self):
        self._parent.block(True)
        self.show()

    def close(self):
        self.hide()
        self._parent.block(False)
        self.delete()

    def _ok(self, bt):
        if self._gs.group:
            self._parent.group = self._gs.group
            self.close()

    def _cancel(self, bt):
        self.close()


class GroupSelector(elementary.Table):

    def __init__(self, parent):
        self._parent = parent
        elementary.Table.__init__(self, parent)
        self.size_hint_align_set(evas.EVAS_HINT_FILL,
                                 evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                  evas.EVAS_HINT_EXPAND)
        self.homogenous_set(False)

        self._groups_init()
        self._preview_init()
        self._actions_init()

    def _groups_init(self):
        self._groups = elementary.List(self._parent)
        self._groups.size_hint_align_set(evas.EVAS_HINT_FILL,
                                        evas.EVAS_HINT_FILL)
        self._groups.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                         evas.EVAS_HINT_EXPAND)
        self.pack(self._groups, 0, 0, 1, 1)
        self._groups.show()

        self._groups_items = {}
        self._group = None

    def _preview_init(self):
        self._preview = edje.Edje(self.evas)
        self._preview_scr = elementary.Scroller(self._parent)
        self._preview_scr.size_hint_align_set(evas.EVAS_HINT_FILL,
                                              evas.EVAS_HINT_FILL)
        self._preview_scr.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                               evas.EVAS_HINT_EXPAND)
        self._preview_scr.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                                     elementary.ELM_SCROLLER_POLICY_AUTO)
        self._preview_scr.content_set(self._preview)
        self.pack(self._preview_scr, 1, 0, 2, 1)
        self._preview_scr.bounce_set(False, False)
        self._preview_scr.show()

    def _actions_init(self):
        self._actions = elementary.Box(self._parent)
        self._actions.horizontal_set(True)
        self._actions.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._actions.size_hint_align_set(1.0, evas.EVAS_HINT_FILL)
        self.pack(self._actions, 0, 1, 3, 1)
        self._actions.show()

    def _update(self, opt):
        self._groups.clear()
        if not self._groups_list:
            self._group = ""
            return
        self._groups_items = {}

        for group in self._groups_list:
            item = self._groups.item_append(group, None, None,
                                            self._group_select, group)
            self._groups_items[group] = item

        item = self._groups_items.get(self._group)
        if item:
            item.selected_set(True)
        else:
            self._groups_items[self._groups_list[0]].selected_set(True)

        self._groups.go()

    def _group_select(self, li, it, group):
        self._group = group
        self._preview.file_set(self.file, self._group)

    def _file_set(self, file):
        self._groups_list = edje.file_collection_list(file)
        if self._groups_list:
            self._groups_list.sort(key=str.lower)
            self._file = file
            self._update(self)

    def _file_get(self):
        return self._file

    file = property(_file_get, _file_set)

    def _group_set(self, value):
        if value in self._groups_list:
            self._groups_items[value].selected_set(True)

    def _group_get(self):
        return self._group

    group = property(_group_get, _group_set)

    def action_add(self, label, func_cb, data=None, icon=None):
        btn = elementary.Button(self._parent)
        btn.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        btn.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        btn.label_set(label)

        if func_cb:
            btn.callback_clicked_add(func_cb)
            btn.data["clicked"] = data

        if icon:
            ico = elementary.Icon(self._parent)
            ico.file_set(self.__theme_file, "editje/icon/" + icon)
            btn.icon_set(ico)
            ico.show()

        btn.show()
        self._actions.pack_end(btn)


if __name__ == "__main__":
    elementary.init()
    win = elementary.Window("groupselector", elementary.ELM_WIN_BASIC)
    win.title_set("GroupSelector")
    win.autodel_set(True)
    win.resize(480, 300)
    win.maximized_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    fs = GroupSelector(win)
    win.resize_object_add(fs)
    fs.file = "./test/sample2.edj"
    fs.action_add("Ok", None)
    fs.action_add("Cancel", None)
    fs.show()

    win.show()

    elementary.run()
    elementary.shutdown()
