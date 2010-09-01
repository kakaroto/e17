# Copyright (C) 2010 Samsung Electronics.
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

from details_widget_entry_button import WidgetEntryButton
from filewizard import FontSelectionWizard


class WidgetFont(WidgetEntryButton):
    def __init__(self, parent, fnt_new_fnt_cb=None, fnt_list_get_cb=None,
                 workfile_name_get_cb=None):
        WidgetEntryButton.__init__(self, parent, "editje/icon/font_button")

        self._parent = parent
        self._fnt_new_fnt_cb = fnt_new_fnt_cb
        self._fnt_list_get_cb = fnt_list_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb

        self.entry_value = "Sans"

    def _actions_init(self):
        self._pop.title_set("Fonts")
        self._pop.action_add("None", self._select_cb, "")

    def _open(self, bt, *args):
        FontSelectionWizard(self._parent,
                selected_cb=self._font_selected_cb,
                file_add_cb=self._fnt_new_fnt_cb,
                file_list_cb=self._fnt_list_get_cb,
                workfile_name_get_cb=self._workfile_name_get_cb).show()

    def _font_selected_cb(self, font):
        self.value = font
        self._callback_call("changed")
