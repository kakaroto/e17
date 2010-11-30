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

import math

from desktop_handler import Handler
from desktop_part_listener import PartListener
from operation import Operation
from elementary import cursors

class PartHandler(Handler, PartListener):
    def __init__(self, editable_grp, desktop_scroller, theme_file,
                 rel1_move_offset_inform_cb=None,
                 rel2_move_offset_inform_cb=None, op_stack_cb=None,
                 group="editje/desktop/part/resize_handler"):
        Handler.__init__(
            self, editable_grp, desktop_scroller, theme_file, group,
            op_stack_cb)
        PartListener.__init__(self)
        self._rel1_move_offset_inform_cb = rel1_move_offset_inform_cb
        self._rel2_move_offset_inform_cb = rel2_move_offset_inform_cb

    def down(self, x, y):
        if self._part:
            self._geometry = self._part.geometry

    def _operation_args_get(self):
        args = [self._edit_grp.part.name, self._edit_grp.part.state.name]
        if self._edit_grp.mode == "Animations":
            args.append(self._edit_grp.animation.name)
            args.append(self._edit_grp.animation.state)

        return args

    # one time only calls to move() (undo/redo) will call this
    def _context_recall(self, part, state, animation=None, time=None):
        if self._edit_grp.mode == "Animations":
            if animation:
                self._edit_grp.animation.name = animation
            if time:
                self._edit_grp.animation.state = time

        self._edit_grp.part.name = part
        self._edit_grp.part.state.name = state

    def _modifier_ctrl_adjust(self, dw, dh):
        if self._modifier_control:
            dw = (dw / 10) * 10
            dh = (dh / 10) * 10
        return (dw, dh)


class PartHandler_Move(PartHandler):
    cursor = cursors.ELM_CURSOR_FLEUR

    def __init__(self, editable_grp, desktop_scroller, theme_file,
                 rel1_move_offset_inform_cb=None,
                 rel2_move_offset_inform_cb=None, op_stack_cb=None,
                 group="editje/desktop/part/move_handler"):
        PartHandler.__init__(
            self, editable_grp, desktop_scroller, theme_file,
            rel1_move_offset_inform_cb, rel2_move_offset_inform_cb,
            op_stack_cb, group)
        self.size = (10, 10)

    def _modifier_shift_adjust(self, dw, dh):
        if self._modifier_shift:
            angle = math.atan2(dh, dw)
            sin = abs(math.sin(angle))
            sin45 = math.sin(math.radians(45))
            adjust = int(round(math.degrees(angle) / 45)) % 2

            if sin > sin45:
                dw = math.copysign(dh * adjust, dw)
            else:
                dh = math.copysign(dw * adjust, dh)

        return (dw, dh)

    def part_move(self, obj):
        self.center = obj.center
        self.show()

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):

        if part_name and state_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            dw, dh = self._modifier_shift_adjust(dw, dh)
            self._part.pos = (x + dw, y + dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        dw, dh = self._modifier_shift_adjust(dw, dh)
        if (dw, dh) != (0, 0):
            op = Operation("part moving")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, dw, dh)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, dw, dh)

            op.undo_callback_add(self.move, -dw, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, -dw, -dh)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, -dw, -dh)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(dw, dh)
        self._rel2_move_offset_inform_cb(dw, dh)


class PartHandler_T(PartHandler):
    cursor = cursors.ELM_CURSOR_TOP_SIDE

    def part_move(self, obj):
        self.bottom_center = obj.top_center
        if obj.size[0] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            self._part.geometry = (x, y + dh, w, h - dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        if dh != 0:
            op = Operation("part resizing (from top)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, 0, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, 0, dh)

            op.undo_callback_add(self.move, 0, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, 0, -dh)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(0, dh)


class PartHandler_TL(PartHandler):
    cursor = cursors.ELM_CURSOR_TOP_LEFT_CORNER

    def _modifier_shift_adjust(self, dw, dh):
        if self._modifier_shift:
            x, y, w, h = self._geometry
            aspect = float(w)/h
            temp_dw = int(dh * aspect)
            if temp_dw < dw:
                dh = int(dw / aspect)
                dw = int(dh * aspect)
            else:
                dw = temp_dw
        return (dw, dh)

    def part_move(self, obj):
        self.show()
        self.bottom_right = obj.top_left

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            dw, dh = self._modifier_shift_adjust(dw, dh)
            self._part.geometry = (x + dw, y + dh, w - dw, h - dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        dw, dh = self._modifier_shift_adjust(dw, dh)
        if (dw, dh) != (0, 0):
            op = Operation("part resizing (from top-left)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, dw, dh)

            op.undo_callback_add(self.move, -dw, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, -dw, -dh)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(dw, dh)


class PartHandler_TR(PartHandler):
    cursor = cursors.ELM_CURSOR_TOP_RIGHT_CORNER

    def _modifier_shift_adjust(self, dw, dh):
        if self._modifier_shift:
            dh = - dh
            x, y, w, h = self._geometry
            aspect = float(w)/h
            temp_dw = int(dh * aspect)
            if temp_dw > dw:
                dh = int(dw / aspect)
                dw = int(dh * aspect)
            else:
                dw = temp_dw
            dh = - dh
        return (dw, dh)

    def part_move(self, obj):
        self.show()
        self.bottom_left = obj.top_right

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            dw, dh = self._modifier_shift_adjust(dw, dh)
            self._part.geometry = (x, y + dh, w + dw, h - dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        dw, dh = self._modifier_shift_adjust(dw, dh)
        if (dw, dh) != (0, 0):
            op = Operation("part resizing (from top-left)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, 0, dh)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, dw, 0)

            op.undo_callback_add(self.move, -dw, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, 0, -dh)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, -dw, 0)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(0, dh)
        self._rel2_move_offset_inform_cb(dw, 0)


class PartHandler_B(PartHandler):
    cursor = cursors.ELM_CURSOR_BOTTOM_SIDE

    def part_move(self, obj):
        self.top_center = obj.bottom_center
        if obj.size[0] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            self._part.geometry = (x, y, w, h + dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        if dh != 0:
            op = Operation("part resizing (from bottom)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, 0, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, 0, dh)

            op.undo_callback_add(self.move, 0, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, 0, -dh)
            self._operation_stack_cb(op)

        self._rel2_move_offset_inform_cb(0, dh)


class PartHandler_BR(PartHandler):
    cursor = cursors.ELM_CURSOR_BOTTOM_RIGHT_CORNER

    def _modifier_shift_adjust(self, dw, dh):
        if self._modifier_shift:
            x, y, w, h = self._geometry
            aspect = float(w)/h
            temp_dw = int(dh * aspect)
            if temp_dw > dw:
                dh = int(dw / aspect)
                dw = int(dh * aspect)
            else:
                dw = temp_dw
        return (dw, dh)

    def part_move(self, obj):
        self.show()
        self.top_left = obj.bottom_right

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            dw, dh = self._modifier_shift_adjust(dw, dh)
            self._part.geometry = (x, y, w + dw, h + dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        dw, dh = self._modifier_shift_adjust(dw, dh)
        if (dw, dh) != (0, 0):
            op = Operation("part resizing (from bottom-right)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, dw, dh)

            op.undo_callback_add(self.move, -dw, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, -dw, -dh)
            self._operation_stack_cb(op)

        self._rel2_move_offset_inform_cb(dw, dh)


class PartHandler_BL(PartHandler):
    cursor = cursors.ELM_CURSOR_BOTTOM_LEFT_CORNER

    def _modifier_shift_adjust(self, dw, dh):
        if self._modifier_shift:
            dw = - dw
            x, y, w, h = self._geometry
            aspect = float(w)/h
            temp_dw = int(dh * aspect)
            if temp_dw > dw:
                dh = int(dw / aspect)
                dw = int(dh * aspect)
            else:
                dw = temp_dw
            dw = - dw
        return (dw, dh)

    def part_move(self, obj):
        self.show()
        self.top_right = obj.bottom_left

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            dw, dh = self._modifier_shift_adjust(dw, dh)
            self._part.geometry = (x + dw, y, w - dw, h + dh)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        dw, dh = self._modifier_shift_adjust(dw, dh)
        if (dw, dh) != (0, 0):
            op = Operation("part resizing (from bottom-left)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, dh, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, dw, 0)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, 0, dh)

            op.undo_callback_add(self.move, -dw, -dh, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, -dw, 0)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, 0, -dh)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(dw, 0)
        self._rel2_move_offset_inform_cb(0, dh)


class PartHandler_L(PartHandler):
    cursor = cursors.ELM_CURSOR_LEFT_SIDE

    def part_move(self, obj):
        self.right_center = obj.left_center
        if obj.size[1] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            self._part.geometry = (x + dw, y, w - dw, h)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        if dw != 0:
            op = Operation("part resizing (from left)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, 0, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel1_move_offset_inform_cb, dw, 0)

            op.undo_callback_add(self.move, -dw, 0, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel1_move_offset_inform_cb, -dw, 0)
            self._operation_stack_cb(op)

        self._rel1_move_offset_inform_cb(dw, 0)


class PartHandler_R(PartHandler):
    cursor = cursors.ELM_CURSOR_RIGHT_SIDE

    def part_move(self, obj):
        self.left_center = obj.right_center
        if obj.size[1] < 10:
            self.hide()
        else:
            self.show()

    def move(self, dw, dh, part_name=None, state_name=None,
             anim=None, time=None):
        if part_name:
            self._context_recall(part_name, state_name, anim, time)

        if self._part:
            x, y, w, h = self._geometry
            dw, dh = self._modifier_ctrl_adjust(dw, dh)
            self._part.geometry = (x, y, w + dw, h)

    def up(self, dw, dh):
        if not self._part:
            return

        dw, dh = self._modifier_ctrl_adjust(dw, dh)
        if dw != 0:
            op = Operation("part resizing (from right)")

            args = self._operation_args_get()

            op.redo_callback_add(self.move, dw, 0, *args)
            op.redo_callback_add(self.part_move, self._part)
            op.redo_callback_add(self._rel2_move_offset_inform_cb, dw, 0)

            op.undo_callback_add(self.move, -dw, 0, *args)
            op.undo_callback_add(self.part_move, self._part)
            op.undo_callback_add(self._rel2_move_offset_inform_cb, -dw, 0)
            self._operation_stack_cb(op)

        self._rel2_move_offset_inform_cb(dw, 0)
