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
import evas.decorators
import edje
import elementary
from elementary import cursors

from controller import Controller, View
from desktop_handler import Handler
from desktop_part_listener import PartListener
import desktop_part_handlers as part_handlers
from desktop_parts_manager import PartsManager
from operation import Operation


class Desktop(Controller):
    def __init__(self, parent, op_stack_cb):
        self._operation_stack_cb = op_stack_cb
        self.e = parent.e
        Controller.__init__(self, parent)

        self.e.callback_add("group.changed", self._group_load)
        self.e.callback_add("group.min.changed", self._group_min_load)
        self.e.callback_add("group.max.changed", self._group_max_load)
        self.e.callback_add("group.size.changed", self._group_size_load)
        self.e.callback_add("part.added", self._part_added)
        self.e.part.callback_add("part.changed", self._part_load)
        self.e.part.callback_add("part.unselected", self._part_load)
        self.e.part.callback_add("name.changed", self._part_rename)
        self.e.part.state.callback_add("state.rel1.changed", self._rel1_load)
        self.e.part.state.callback_add("state.rel2.changed", self._rel2_load)

    def _view_load(self):
        self._view = DesktopView(
            self, self.parent.view, self._operation_stack_cb)

    def _group_load(self, emissor, data):
        if data:
            self._view.group = self.e.edje
        else:
            self._view.group = None

    def _group_min_load(self, emissor, data):
        self._view.manager.group_min_set(*data)

    def _group_max_load(self, emissor, data):
        self._view.manager.group_max_set(*data)

    def _group_size_load(self, emissor, data):
        self._view.manager.group_resize(*data)

    def _part_load(self, emissor, data):
        if data:
            self._view.part = self.e.part
        else:
            self._view.part = None

    def _part_added(self, emissor, data):
        self._view.manager.parts_manager.part_load(data)

    def _part_rename(self, emissor, data):
        self._view.manager.parts_manager.part_rename(*data)

    def _rel1_load(self, emissor, data):
        self._view.part_rel1_set(data)

    def _rel2_load(self, emissor, data):
        self._view.part_rel2_set(data)

    def parts_block(self, parts_list=[]):
        self._view.manager.parts_manager.parts_unblock()
        self._view.manager.parts_manager.parts_block(parts_list)


class DesktopView(View, elementary.Scroller):
    def __init__(self, controller, parent_view, op_stack_cb):
        View.__init__(self, controller, parent_view)
        self._layout_load(op_stack_cb)

    def _layout_load(self, op_stack_cb):
        elementary.Scroller.__init__(self, self.parent_view)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                        elementary.ELM_SCROLLER_POLICY_AUTO)
        self.bounce_set(False, False)

        self.manager = EditManager(self.controller, self, op_stack_cb)
        self.content_set(self.manager)
        self.manager.show()

    # Group
    def _group_set(self, group):
        self._group = group
        self.manager.group = group

    group = property(fset=_group_set)

    # Part
    def _part_set(self, part):
        self._part = part
        if part:
            self.manager.part = self._group.part_object_get(self._part.name)
        else:
            self.manager.part = None

    part = property(fset=_part_set)

    def _get_relative_part(self, part_x, part_y):
        px = self._group.part_object_get(part_x or "")
        py = self._group.part_object_get(part_y or "")

        return (px or self._group, py or self._group)

    def part_rel1_set(self, data):
        self.rel1_to, rel, off = data
        to = self._get_relative_part(*self.rel1_to)

        self.manager.rel1_to = to
        self.manager.rel1_rel = rel
        self.manager.rel1_off = off

    def part_rel2_set(self, data):
        self.rel2_to, rel, off = data
        to = self._get_relative_part(*self.rel2_to)

        self.manager.rel2_to = to
        self.manager.rel2_rel = rel
        self.manager.rel2_off = off

    def part_clicked(self, part):
        self.controller.e.part.name = part

    def part_rel1_change(self, rel, ofs):
        self.controller.e.part.state.rel1_to = self.rel1_to
        self.controller.e.part.state.rel1_relative = rel
        self.controller.e.part.state.rel1_offset = ofs

    def part_rel2_change(self, rel, ofs):
        self.controller.e.part.state.rel2_to = self.rel2_to
        self.controller.e.part.state.rel2_relative = rel
        self.controller.e.part.state.rel2_offset = ofs


class EditManager(View, evas.ClippedSmartObject):
    def __init__(self, controller, parent_view, op_stack_cb):
        View.__init__(self, controller, parent_view)
        evas.ClippedSmartObject.__init__(self, parent_view.evas)

        self._operation_stack_cb = op_stack_cb
        self._group = None
        self._group_min = (0, 0)
        self._group_max = (0, 0)
        self._part = None
        self._objs = []
        self._group_listeners = []
        self._part_listeners = []

        self.outside_area = self.evas.Rectangle(color=(0, 0, 0, 0))
        self.member_push(self.outside_area)
        self.outside_area.on_mouse_down_add(self._outside_area_clicked)
        self.outside_area.show()

        # Border
        self.border = GroupBorder(self)
        self.listener_member_push(self.border)

        #Group Handler
        self.group_handler = GroupResizeHandler(
            self.controller.e, self.parent_view, self.theme,
            self.group_resize, self.padding_update, self._operation_stack_cb)
        self.listener_member_push(self.group_handler)

        # Highlight
        # self.highlight1x = PartHighlight(
        #     self, group = "editje/desktop/rel1/highlight")
        # self.member_push(self.highlight1x)
        # self.highlight1y = PartHighlight(
        #     self, group = "editje/desktop/rel1/highlight")
        # self.member_push(self.highlight1y)
        # self.highlight2x = PartHighlight(
        #     self, group = "editje/desktop/rel2/highlight")
        # self.member_push(self.highlight2x)
        # self.highlight2y = PartHighlight(
        #     self, group = "editje/desktop/rel2/highlight")
        # self.member_push(self.highlight2y)

        self.parts_manager = PartsManager(self.evas)
        self.parts_manager.select = self.parent_view.part_clicked
        self.member_push(self.parts_manager)

        self._handlers_init()

        self.parent_view.on_resize_add(self._padding_init)

        #self.on_key_down_add(self._key_down_cb)

    def _key_down_cb(self, obj, event):
        print event.keyname

    def _handlers_init(self):
        #Hilight
        self.highlight = PartHighlight(self)
        self.listener_member_push(self.highlight)

        def handler_args_list():
            return (self.controller.e, self.parent_view, self.theme,
                    self._inform_rel1_changed, self._inform_rel2_changed,
                    self._operation_stack_cb)

        #Move
        self.handler_move = \
            part_handlers.PartHandler_Move(*handler_args_list())
        self.listener_member_push(self.handler_move)
        #Top Right
        self.handler_tr = part_handlers.PartHandler_TR(*handler_args_list())
        self.listener_member_push(self.handler_tr)
        #Bottom Left
        self.handler_bl = part_handlers.PartHandler_BL(*handler_args_list())
        self.listener_member_push(self.handler_bl)
        # Top
        self.handler_t = part_handlers.PartHandler_T(*handler_args_list())
        self.listener_member_push(self.handler_t)
        # Left
        self.handler_l = part_handlers.PartHandler_L(*handler_args_list())
        self.listener_member_push(self.handler_l)
        # Bottom
        self.handler_b = part_handlers.PartHandler_B(*handler_args_list())
        self.listener_member_push(self.handler_b)
        # Right
        self.handler_r = part_handlers.PartHandler_R(*handler_args_list())
        self.listener_member_push(self.handler_r)
        #Top Left
        self.handler_tl = part_handlers.PartHandler_TL(*handler_args_list())
        self.listener_member_push(self.handler_tl)
        #Bottom Right
        self.handler_br = part_handlers.PartHandler_BR(*handler_args_list())
        self.listener_member_push(self.handler_br)

    def member_push(self, obj):
        self._objs.append(obj)
        self.member_add(obj)

    def listener_member_push(self, obj):
        if isinstance(obj, GroupListener):
            self._group_listeners.append(obj)
        if isinstance(obj, PartListener):
            self._part_listeners.append(obj)
        self.member_push(obj)

    def resize(self, w, h):
        self.outside_area.resize(w, h)

    def move(self, x, y):
        self.outside_area.move(x, y)

        if self._group is None:
            return
        ox, oy = self.pos
        dx = x - ox
        dy = y - oy
        self._group.move_relative(dx, dy)

    def delete(self):
        evas.ClippedSmartObject.delete(self)

    def _outside_area_clicked(self, o, ev):
        self.controller.e.part.name = ""

    #  GROUP CHANGED
    def group_min_set(self, w, h):
        self._group_min = (w, h)
        self.group_resize(*self._group.size)

    def group_max_set(self, w, h):
        self._group_max = (w, h)
        self.group_resize(*self._group.size)

    def group_resize(self, w, h, grow=False):
        self.parent_view.controller.e.group_size = (w, h)
        self.padding_update(grow)

    # Group
    def _group_member_add(self):
        for obj in self._objs:
            self.member_del(obj)
        self.member_add(self._group)
        for obj in self._objs:
            self.member_add(obj)

    def _group_set(self, group):
        if self._group:
            self._group.hide()
            self.member_del(self._group)

        if not group:
            self._group = None
        else:
            self._group = group
            self._padding_init(self)
            self._group_member_add()
            self.outside_area.lower()
            group.show()
            for obj in self._group_listeners:
                obj.group = group
        self.parts_manager.edje = self._group

    def _group_get(self):
        return self._group

    group = property(_group_get, _group_set)

    # Part
    def _part_set(self, part):
        self._part = part

        self.rel1_to = (self._group, self._group)
        self.rel1_rel = (0.0, 0.0)
        self.rel1_off = (0, 0)

        self.rel2_to = (self._group, self._group)
        self.rel2_rel = (1.0, 1.0)
        self.rel2_off = (-1, -1)

        for obj in self._part_listeners:
            obj.part = part

    def _part_get(self):
        return self._part

    part = property(_part_get, _part_set)

    def _rel1_to_get(self):
        return self._rel1_to

    def _rel1_to_set(self, value):
        self._rel1_to = value

    rel1_to = property(fget=_rel1_to_get, fset=_rel1_to_set)

    def _rel1_rel_get(self):
        return self._rel1_rel

    def _rel1_rel_set(self, value):
        self._rel1_rel = value

    rel1_rel = property(fget=_rel1_rel_get, fset=_rel1_rel_set)

    def _rel1_off_get(self):
        return self._rel1_off

    def _rel1_off_set(self, value):
        self._rel1_off = value

    rel1_off = property(fget=_rel1_off_get, fset=_rel1_off_set)

    def _rel2_to_get(self):
        return self._rel2_to

    def _rel2_to_set(self, value):
        self._rel2_to = value

    rel2_to = property(fget=_rel2_to_get, fset=_rel2_to_set)

    def _rel2_rel_get(self):
        return self._rel2_rel

    def _rel2_rel_set(self, value):
        self._rel2_rel = value

    rel2_rel = property(fget=_rel2_rel_get, fset=_rel2_rel_set)

    def _rel2_off_get(self):
        return self._rel2_off

    def _rel2_off_set(self, value):
        self._rel2_off = value

    rel2_off = property(fget=_rel2_off_get, fset=_rel2_off_set)

    # Padding
    def _padding_init(self, obj):
        if not self._group:
            return

        w, h = self._group.size
        w += 600
        h += 600
        scr_x, scr_y, scr_w, scr_h = self.parent_view.region_get()

        if w < scr_w:
            w = scr_w
        if h < scr_h:
            h = scr_h

        self.size_hint_min_set(w, h)
        self._group.center = self.center

        self.parent_view.region_show((w - scr_w) / 2, (h - scr_h) / 2,
                                     scr_w, scr_h)

    def padding_update(self, grow=False):
        w, h = self._group.size
        w += 600
        h += 600
        scr_x, scr_y, scr_w, scr_h = self.parent_view.region_get()

        if w < scr_w:
            w = scr_w
        if h < scr_h:
            h = scr_h

        if grow:
            w0, h0 = self.size_hint_min_get()
            if w0 > w:
                w = w0
            if h0 > h:
                h = h0

        self.size_hint_min_set(w, h)

    # Actions
    def _inform_rel1_changed(self, dw, dh):
        dw += self.rel1_off[0]
        dh += self.rel1_off[1]
        self.parent_view.part_rel1_change(self.rel1_rel, (dw, dh))

    def _inform_rel2_changed(self, dw, dh):
        dw += self.rel2_off[0]
        dh += self.rel2_off[1]
        self.parent_view.part_rel2_change(self.rel2_rel, (dw, dh))


class GroupListener(object):
    def __init__(self):
        self._group = None

    # group is an Edje_Edit obj!!
    def _group_set(self, group):
        if self._group:
            self._group.on_move_del(self.group_move)
            self._group.on_resize_del(self.group_move)

        if group:
            self._group = group
            self.top_left = group.bottom_right
            group.on_move_add(self.group_move)
            group.on_resize_add(self.group_move)
            group.on_del_add(self._group_del_cb)
            self.group_move(group)
            self.show()
        else:
            self._group = None
            self.hide()

    def _group_del_cb(self, obj):
        self._group = None

    group = property(fset=_group_set)

    def group_move(self, obj):
        print "MOVE", obj


class GroupBorder(GroupListener, edje.Edje):
    def __init__(self, parent):
        self._parent = parent
        edje.Edje.__init__(self, parent.evas, file=parent.theme,
                           group="editje/desktop/frame")
        GroupListener.__init__(self)

    def group_move(self, obj):
        if self._group:
            self.geometry = obj.geometry
            self.show()


class GroupResizeHandler(Handler, GroupListener):
    cursor = cursors.ELM_CURSOR_BOTTOM_RIGHT_CORNER

    def __init__(self, editable_grp, desktop_scroller, theme_file,
                 group_resize_cb, padding_update_cb,
                 op_stack_cb=None, group="editje/desktop/handler"):
        Handler.__init__(self, editable_grp, desktop_scroller,
                         theme_file, group, op_stack_cb)
        GroupListener.__init__(self)
        self._group_resize_cb, self._padding_update_cb = \
            group_resize_cb, padding_update_cb

    def group_move(self, obj):
        self.show()
        self.top_left = obj.bottom_right

    def down(self, x, y):
        if self._group:
            self._size = self._group.size

    def move(self, dw, dh, size=None):
        if not self._group:
            return

        sz = size or self._size
        self._group_resize_cb(sz[0] + dw, sz[1] + dh, grow=True)

    def up(self, dw, dh):
        if (dw, dh) == (0, 0) or not self._group:
            return

        curr_sz = self._size[:]

        op = Operation("group resizing")

        op.redo_callback_add(self.move, dw, dh, curr_sz)
        op.redo_callback_add(self._padding_update_cb)

        op.undo_callback_add(self.move, -dw, -dh, self._group.size)
        op.undo_callback_add(self._padding_update_cb)
        self._operation_stack_cb(op)

        self._padding_update_cb()
        del self._size


class PartHighlight(PartListener, edje.Edje):
    def __init__(self, parent):
        edje.Edje.__init__(self, parent.evas, file=parent.theme,
                           group="editje/desktop/highlight")
        PartListener.__init__(self)

    def _part_set(self, part):
        PartListener._part_set(self, part)
        if self._part:
            self.signal_emit("animate", "")

    part = property(fset=_part_set)

    def part_move(self, obj):
        self.geometry = obj.geometry
        self.show()
