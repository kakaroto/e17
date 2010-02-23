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
import elementary

from details_widget_entry_button import WidgetEntryButton
from floater_opener import FloaterListOpener


class WidgetEntryButtonList(FloaterListOpener, WidgetEntryButton):
    def __init__(self, parent):
        FloaterListOpener.__init__(self)
        WidgetEntryButton.__init__(self, parent)

    def value_set(self, value):
        self.value = value
        self._callback_call("changed")

    def _open(self, obj):
        self._floater_open(obj)

