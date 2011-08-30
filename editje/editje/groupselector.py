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

import evas
import ecore
import edje
import elementary

from floater import Wizard


class GroupsList(elementary.List):
    def __init__(self, parent, select_cb=None, select_cb_data=None):
        elementary.List.__init__(self, parent)
        self._parent = parent
        self._select_cb = select_cb
        self._select_cb_data = select_cb_data

        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

        self._groups = []
        self._groups_items = {}
        self._selection = ""

    def _file_set(self, file_):
        self._file = file_
        self.update()

    def _file_get(self):
        return self._file

    file = property(fset=_file_set, fget=_file_get)

    def update(self):
        self._groups = edje.file_collection_list(self.file)
        if self._groups:
            self._groups.sort(key=str.lower)

        self.clear()
        if not self._groups:
            self._selection = ""
            return
        self._groups_items = {}

        for group in self._groups:
            item = self.item_append(group, None, None, self._select, group)
            self._groups_items[group] = item

        # TODO: make a way to let items highlighted but not actually
        # select()-ed, in the sense of selection callbacks

        # item = self._groups_items.get(self._selection)
        # if item:
        #     item.selected_set(True)
        # else:
        #     self._groups_items[self._groups[0]].selected_set(True)

        self.go()

    def _select(self, list_, item, group):
        self._selection = group
        if self._select_cb:
            if self._select_cb_data:
                self._select_cb(self._select_cb_data)
            else:
                self._select_cb()

    def _selection_set(self, value):
        item = self._groups_items.get(value)
        if item:
            item.selected_set(True)
            self._selection = value
        else:
            # TODO: make the list unselect everything, later
            self._selection = ""

    def _selection_get(self):
        return self._selection

    selection = property(fset=_selection_set, fget=_selection_get)

    def selection_clear(self):
        for i in self.selected_items_get():
            i.selected_set(False)


# TODO: maybe this widget is generic enough to be moved to a general
# widgets definition file
# FIXME: the status label will be visually refactored
class NameEntry(elementary.Box):
    default_weight_hints = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    default_align_hints = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    def __init__(self, parent, changed_cb=None,
                 weight_hints=(), align_hints=()):
        elementary.Box.__init__(self, parent)

        if weight_hints:
            self.size_hint_weight_set(*weight_hints)
        else:
            self.size_hint_weight_set(*self.default_align_hints)
        if align_hints:
            self.size_hint_align_set(*align_hints)
        else:
            self.size_hint_align_set(*self.default_align_hints)

        self._entry_bx = elementary.Box(parent)
        self._entry_bx.horizontal_set(True)
        self._entry_bx.size_hint_weight_set(
            evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self._entry_bx.size_hint_align_set(
            evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._entry_bx.show()

        entry_lb = elementary.Label(parent)
        entry_lb.text_set("Name: ")
        entry_lb.show()
        self._entry_bx.pack_end(entry_lb)

        self._status_lb = elementary.Label(parent)
        self._status_lb.text_set("")
        self._status_lb.show()

        scroller = elementary.Scroller(parent)
        scroller.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        scroller.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        scroller.content_min_limit(False, True)
        scroller.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                            elementary.ELM_SCROLLER_POLICY_OFF)
        scroller.bounce_set(False, False)
        self._entry_bx.pack_end(scroller)
        scroller.show()

        self._name = elementary.Entry(parent)
        self._name.single_line_set(True)
        self._name.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._name.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        self._name.context_menu_disabled_set(True)

        if changed_cb:
            self._name.callback_changed_add(changed_cb)

        scroller.content_set(self._name)
        self._name.show()

        self.pack_end(self._entry_bx)
        self.pack_end(self._status_lb)

    def focus(self):
        self._name.focus()

    def _entry_set(self, value):
        self._name.entry_set(value)

    def _entry_get(self):
        entry = self._name.entry_get()
        return entry

    entry = property(fset=_entry_set, fget=_entry_get)

    def _status_label_set(self, value):
        self._status_lb.text_set(value)

    def _status_label_get(self):
        status_label = self._status_lb.label_get()
        return status_label

    status_label = property(fset=_status_label_set, fget=_status_label_get)


class PreviewFrame(elementary.Scroller):
    def __init__(self, parent):
        elementary.Scroller.__init__(self, parent)

        self._parent = parent
        self._group = edje.Edje(self._parent.evas)
        self._group.show()

        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                        elementary.ELM_SCROLLER_POLICY_AUTO)
        self.bounce_set(False, False)
        self.content_set(self._group)
        self.show()

    def _dimensions_retrieve_no_zero(self, func):
        w, h = func()

        if w == 0:
            w = h
        if h == 0:
            h = w

        return w, h

    def group_set(self, file_, group_name):
        self._group.file_set(file_, group_name)

        key = "pref_size"
        data = self._group.data_get(key)
        if data:
            w, h = data.split("x")
            min_w, min_h = int(w), int(h)
        else:
            min_w, min_h = self._dimensions_retrieve_no_zero(
                self._group.size_min_get)

            if not min_w:
                min_w, min_h = self._dimensions_retrieve_no_zero(
                    self._group.size_min_calc)

            if not min_w:
                min_w, min_h = 300, 300

        self._group.size_hint_min_set(min_w, min_h)
        self._group.size_hint_max_set(min_w, min_h)
        self._group.size_hint_align_set(0.5, 0.5)

    def group_release(self):
        if self._group:
            self._group.hide()
            self._group.delete()

        self._group = edje.Edje(self._parent.evas)
        self._group.show()
        self.content_set(self._group)


class GroupSelectionWizard(Wizard):
    def __init__(self, parent, switch_only=False, selected_set_cb=None,
                 selected_get_cb=None, new_grp_cb=None, check_grp_cb=None,
                 del_grp_cb=None, error_get_cb=None):
        if not selected_set_cb or not selected_get_cb or not new_grp_cb or \
                not check_grp_cb or not del_grp_cb:
            raise TypeError("You must set callbacks for group" \
                            " {selection,addition,checking,deletion} on" \
                            " GroupSelectionWizard objects.")
        Wizard.__init__(self, parent)
        self._select_set_cb = selected_set_cb
        self._select_get_cb = selected_get_cb
        self._check_group_cb = check_grp_cb
        self._delete_cb = del_grp_cb
        self._error_get_cb = error_get_cb

        self.page_add("group_list", "Select a group",
                      "Select an existing group to edit, or create a new one.")
        self._groups_list = GroupsList(self, self._goto_preview)
        self.content_add("group_list", self._groups_list)
        self._groups_list.show()
        if not switch_only:
            self.action_add("group_list", "Cancel", self.close, key="Escape")
        self.action_add("group_list", "New", self._goto_new_group, key="n")

        def group_added(cb_func):
            name = self._grp_name_entry.entry

            success = cb_func(name)
            if not success:
                self.notify("Error creating new group.")
                return

            self._group_selected(name)

        self.page_add("new_group", "Create a new group",
                      "Enter a name for a new group in the file.",
                      separator=True)
        self._grp_name_entry = NameEntry(self,
                changed_cb=self._name_changed_cb)
        self.content_add("new_group", self._grp_name_entry)
        self._grp_name_entry.show()
        self.action_add("new_group", "Cancel", self.goto, "group_list",
                        key="Escape")
        self.action_add("new_group", "Create", group_added, new_grp_cb,
                        key="Return")

        self.page_add("group_preview", "Group preview",
                      "Delete or start editing this group.")
        self._preview = PreviewFrame(self)
        self.content_add("group_preview", self._preview)
        self._preview.show()
        self.action_add("group_preview", "Cancel", self._goto_group_list,
                        key="Escape")
        self.action_add("group_preview", "Delete", self._delete_group,
                        key="Delete")
        self.action_add("group_preview", "Open", self._group_selected,
                        key="Return")

        self.goto("group_list")

    def goto(self, page, alt_bg_style=None):
        Wizard.goto(self, page)
        if alt_bg_style is True:
            self.alternate_background_set(True)
        else:
            self.alternate_background_set(False)

    def _goto_new_group(self):
        self.goto("new_group", alt_bg_style=True)
        self._grp_name_entry.focus()

    def _goto_preview(self):
        self._preview.group_set(self._groups_list.file,
                                self._groups_list.selection)
        self.goto("group_preview")
        self.title_text = "Group preview - \"%s\"" % \
            self._groups_list.selection

    def _goto_group_list(self):
        self._groups_list.selection_clear()
        self.goto("group_list")

    def _group_selected(self, grp_name=None):
        if grp_name is None:
            grp_name = self._groups_list.selection
        r = self._select_set_cb(grp_name)
        if not r:
            self.notify("Error while selecting this group.")
        ecore.idler_add(self.close)

    def _delete_group(self):
        grp_name = self._groups_list.selection

        self._preview.group_release()

        # We have to call it *before* deletion, as it may change the active
        # group to None, in the case of current group deletion
        current = self._select_get_cb()
        r = self._delete_cb(grp_name)
        error_msg = self._error_get_cb()
        if not r:
            self._preview.group_set(self._groups_list.file,
                                    self._groups_list.selection)

            if error_msg:
                self.notify(error_msg)
            else:
                self.notify("Error while deleting this group")
            return

        if grp_name == current:
            self.action_disabled_set("group_list", "Cancel", True)

        self._groups_list.update()
        self.goto("group_list")

    def file_set(self, file):
        self._groups_list.file = file

    def _name_changed_cb(self, obj):
        self._check_name()

    def _check_name(self):
        error_msg = "This group name already exists in the file"

        def good():
            self._grp_name_entry.status_label = ""
            self.action_disabled_set("new_group", "Create", False)

        def bad():
            self._grp_name_entry.status_label = error_msg
            self.action_disabled_set("new_group", "Create", True)

        def ugly():
            self._grp_name_entry.status_label = ""
            self.action_disabled_set("new_group", "Create", True)

        name = self._grp_name_entry.entry
        if not name:
            ugly()
            return

        exists = self._check_group_cb(name)
        if exists:
            bad()
            return

        good()
