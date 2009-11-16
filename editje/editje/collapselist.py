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
import sys

import edje
from edje.edit import EditEdje
import elementary


class CollapseList(elementary.List):

    def __init__(self, parent, callback):
        elementary.List.__init__(self, parent)
        self.callback = callback

    def populate(self, list):
        self.clear()
        for item in list:
            self.item_append(item, None, None, self.callback, item)
        self.go()

    def selected_item_get(self):
        return self.selected_item
