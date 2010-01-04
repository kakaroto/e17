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

from desktop_handler import Handler
from desktop_part_listener import PartListener

class PartHandler(Handler, PartListener):
    def __init__(self, parent):
        Handler.__init__(self, parent, "editje/desktop/part/resize_handler")
        PartListener.__init__(self)

    def down(self, x, y):
        if self._part:
            self._geometry = self._part.geometry

class PartHandler_Move(PartHandler):
    def __init__(self, parent):
        Handler.__init__(self, parent, "editje/desktop/part/move_handler")
        self.size = (10,10)
        PartListener.__init__(self)

    def part_move(self, obj):
        self.center = obj.center
        self.show()

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.pos = (x + dw, y + dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(dw, dh)
            self._parent.part_move2(dw, dh)

class PartHandler_T(PartHandler):
    def part_move(self, obj):
        self.bottom_center = obj.top_center
        if obj.size[0] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x, y + dh, w, h - dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(0, dh)


class PartHandler_TL(PartHandler):
    def part_move(self, obj):
        self.show()
        self.bottom_right = obj.top_left

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x + dw, y + dh, w - dw, h - dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(dw, dh)


class PartHandler_TR(PartHandler):
    def part_move(self, obj):
        self.show()
        self.bottom_left = obj.top_right

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x, y + dh, w + dw, h - dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(0, dh)
            self._parent.part_move2(dw, 0)


class PartHandler_B(PartHandler):
    def part_move(self, obj):
        self.top_center = obj.bottom_center
        if obj.size[0] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x, y, w, h + dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move2(0, dh)


class PartHandler_BR(PartHandler):
    def part_move(self, obj):
        self.show()
        self.top_left = obj.bottom_right

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x, y, w + dw, h + dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move2(dw, dh)


class PartHandler_BL(PartHandler):
    def part_move(self, obj):
        self.show()
        self.top_right = obj.bottom_left

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x + dw, y, w - dw, h + dh)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(dw, 0)
            self._parent.part_move2(0, dh)


class PartHandler_L(PartHandler):
    def part_move(self, obj):
        self.right_center = obj.left_center
        if obj.size[1] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x + dw, y, w - dw, h)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move1(dw, 0)


class PartHandler_R(PartHandler):
    def part_move(self, obj):
        self.left_center = obj.right_center
        if obj.size[1] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh):
        if self._part:
            x, y, w, h = self._geometry
            self._part.geometry = (x, y, w + dw, h)

    def up(self, dw, dh):
        if self._part:
            self._parent.part_move2(dw, 0)
