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
        self._groups = edje.file_collection_list(file_)
        if self._groups:
            self._file = file_
            self._groups.sort(key=str.lower)
            self._update()

    def _file_get(self):
        return self._file

    file = property(fset=_file_set, fget=_file_get)

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

    def _update(self):
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

    def selection_clear(self):
        for i in self.selected_items_get():
            i.selected_set(False)


# TODO: maybe this widget is generic enough to be moved to general
# widgets definition file
class NameEntry(elementary.Box):
    default_weight_hints = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    default_align_hints = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    def __init__(self, parent, changed_cb=None,
                 weight_hints=(), align_hints=()):
        elementary.Box.__init__(self, parent)
        self.horizontal_set(True)

        if weight_hints:
            self.size_hint_weight_set(*weight_hints)
        else:
            self.size_hint_weight_set(*self.default_align_hints)
        if align_hints:
            self.size_hint_align_set(*align_hints)
        else:
            self.size_hint_align_set(*self.default_align_hints)

        label = elementary.Label(parent)
        label.label_set("Name: ")
        self.pack_end(label)
        label.show()

        scroller = elementary.Scroller(parent)
        scroller.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        scroller.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        scroller.content_min_limit(False, True)
        scroller.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                            elementary.ELM_SCROLLER_POLICY_OFF)
        scroller.bounce_set(False, False)
        self.pack_end(scroller)
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
        self._name.focus()

    def _entry_set(self, value):
        self._name.entry_set(value)

    def _entry_get(self):
        entry = self._name.entry_get().replace("<br>", "")
        return entry

    entry = property(fset=_entry_set, fget=_entry_get)


class PreviewFrame(elementary.Scroller):
    def __init__(self, parent):
        elementary.Scroller.__init__(self, parent)
        self._group = edje.Edje(parent.evas)
        self._group.show()

        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                        elementary.ELM_SCROLLER_POLICY_AUTO)
        self.bounce_set(False, False)
        self.content_set(self._group)
        self.show()

    def group_set(self, file_, group):
        self._group.file_set(file_, group)


class GroupSelectionWizard(Wizard):
    def __init__(self, parent, switch_only=False, selected_cb=None,
                 new_grp_cb=None):
        if not selected_cb:
            raise TypeError("You must set a callback for group selection on" \
                            " GroupSelectionWizard objects.")
        Wizard.__init__(self, parent)
        self._select_cb = selected_cb

        self.page_add("group_list", "Select a group",
                      "Select an existing group to edit, or create a new one.")
        self._groups_list = GroupsList(self, self._goto_preview)
        self.content_add("group_list", self._groups_list)
        self._groups_list.show()
        if not switch_only:
            self.action_add("group_list", "Cancel", self.close)

        if new_grp_cb:
            def group_added(cb_func):
                name = self._grp_name_entry.entry

                success = cb_func(name)
                if not success:
                    self.notify("There is a group with this name in the "
                                "file, already. Please choose another name.")
                    return

                self._group_selected()

            self.page_add("new_group", "Create a new group",
                          "Enter a name for a new group in the file.",
                          separator=True)
            self._grp_name_entry = NameEntry(self,
                    changed_cb=self._name_changed_cb)
            self.content_add("new_group", self._grp_name_entry)
            self._grp_name_entry.show()
            self.action_add("new_group", "Cancel", self.goto, "group_list")
            self.action_add("new_group", "Create", group_added, new_grp_cb)

            self.action_add("group_list", "New", self._goto_new_group)

        self.page_add("group_preview", "Group preview",
                      "Delete or start editing this group.")
        self._preview = PreviewFrame(self)
        self.content_add("group_preview", self._preview)
        self._preview.show()
        self.action_add("group_preview", "Cancel", self._goto_group_list)
        self.action_add("group_preview", "Delete", self._delete_group)
        self.action_add("group_preview", "Open", self._group_selected)

        self.goto("group_list")

    def goto(self, page, alt_bg_style=None):
        Wizard.goto(self, page)
        if alt_bg_style is True:
            self.alternate_background_set(True)
        else:
            self.alternate_background_set(False)

    def _goto_new_group(self):
        self.goto("new_group", alt_bg_style=True)

    def _goto_preview(self):
        self._preview.group_set(self._groups_list.file,
                                self._groups_list.selection)
        self.goto("group_preview")

    def _goto_group_list(self):
        self._groups_list.selection_clear()
        self.goto("group_list")

    def _group_selected(self):
        self._select_cb(self._groups_list.selection)
        ecore.idler_add(self.close)

    def _delete_group(self):
        print "FIXME: group deletion to be added soon."
        self.notify("Group deletion to be added soon.")
        self.goto("group_list")

    def file_set(self, file_, group=None):
        self._groups_list.file = file_
        # if group:
        #     self._groups_list.selection = group

    def _name_changed_cb(self, obj):
        self._name_chaged = True
        self._check_name()

    def _check_name(self):
        name = self._grp_name_entry.entry
        if name:
            self.action_disabled_set("Create", False)
        else:
            self.action_disabled_set("Create", True)
