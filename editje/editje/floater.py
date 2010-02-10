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
from elementary import Layout, Button, InnerWindow, Box, Pager, \
                       Icon, Label, Notify, ELM_NOTIFY_ORIENT_TOP, Separator
import sysconfig


class Floater(object):
    default_padding_x = 20
    default_padding_y = 20
    default_align_x = 0.5
    default_align_y = 0.5

    def __init__(self, parent, rel_to_obj=None):
        if not rel_to_obj:
            raise TypeError("You must pass an object whose geometry the Floater"
                            " will use to move itself in the canvas.")
        self._parent = parent
        self._rel_to_obj = rel_to_obj

        self._padding_x = self.default_padding_x
        self._padding_y = self.default_padding_y
        self._align_x = self.default_align_x
        self._align_y = self.default_align_y

        self._action_btns = []
        self._min_size = [0, 0]

        theme_file = sysconfig.theme_file_get("default")

        self._popup = Layout(parent)
        self._popup.on_changed_size_hints_add(self._move_and_resize)
        self._popup.file_set(theme_file, "editje/floater")

    def padding_set(self, pad_x, pad_y):
        self._padding_x = pad_x
        self._padding_y = pad_y

    def padding_get(self):
        return self._padding_x, self._padding_y

    # padding WRT to the *whole canvas* corners
    padding = property(fset=padding_set, fget=padding_get)

    def align_set(self, align_x, align_y):
        self._align_x = align_x
        self._align_y = align_y

    def align_get(self):
        return self._align_x, self._align_y

    # alignment WRT to *rel_to_obj*
    align = property(fset=align_set, fget=align_get)

    def _move_and_resize(self, obj, *args, **kargs):
        x, y, w, h = self._rel_to_obj.geometry
        cw, ch = self._rel_to_obj.evas.size

        ow, oh = self.size_hint_min_get()
        if ow < self._min_size[0]:
            ow = self._min_size[0]
        if oh < self._min_size[1]:
            oh = self._min_size[1]

        ox = x - int((ow - w) * self._align_x)
        oy = y - int((oh - h) * self._align_y)

        px, py = self.padding

        if ox - px < 0:
            ox = px
        elif ox + ow + px >= cw:
            ox = cw - ow - px

        if oy < py:
            oy = py
        elif oy + oh + py>= ch:
            oy = ch - oh - py

        self._popup.resize(ow, oh)
        self._popup.move(ox, oy)

    def title_set(self, title):
        self.title = title
        self._popup.edje_get().part_text_set("title.text", title)

    def size_hint_min_get(self):
        w, h = self._popup.size_hint_min_get()

        bw = 0
        for b in self._action_btns:
            b_min_w, unused = b.size_hint_min_get()
            bw += b_min_w + 5

        if bw > w:
            w = bw

        return (w, h)

    def size_min_set(self, w, h):
        self._min_size = [w, h]

    def content_set(self, content):
        content.size_hint_weight_set(1.0, 1.0)
        content.size_hint_align_set(-1.0, -1.0)
        self._popup.content_set("content", content)

    def action_add(self, label, func_cb, data = None):
        btn = Button(self._parent)
        self._action_btns.append(btn)
        btn.label_set(label)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_weight_set(1.0, 1.0)
        btn.size_hint_align_set(-1.0, -1.0)
        btn.data["clicked"] = (func_cb, data)
        btn.show()
        self._popup.edje_get().part_box_append("actions", btn)

    def show(self):
        self._move_and_resize(self._popup)
        self._popup.show()

    def hide(self):
        self._popup.hide()

    def open(self):
        self._parent.block(True)
        self.show()

    def close(self):
        self.hide()
        self._parent.block(False)
        self._popup.delete()

    def move(self, *args):
        self._popup.move(*args)

    def resize(self, *args):
        self._popup.resize(*args)

    def _action_btn_clicked(self, obj, *args, **kwargs):
        func, udata = obj.data["clicked"]
        func(self, udata)


# TODO: move this class elsewhere
class Wizard(InnerWindow):
    default_width = 500
    default_height = 500
    default_group = "editje/wizard"

    def __init__(self, parent, width=None, height=None, group=None):
        InnerWindow.__init__(self, parent)

        self._parent = parent
        self.style_set("minimal") # size fallbacks to __layout's min/max

        self.__layout = edje.Edje(self.evas)
        self.__theme_file = sysconfig.theme_file_get("default")
        self.__width = height or self.default_width
        self.__height = height or self.default_height
        _group = group or self.default_group
        self.__layout.file_set(self.__theme_file, _group)
        self.__layout.size_hint_min_set(self.__width, self.__height)
        self.__layout.size_hint_max_set(self.__width, self.__height)

        self.__layout.show()
        InnerWindow.content_set(self, self.__layout)

        self.__pager = Pager(self)
        self.__pager.style_set("editje.rightwards")
        self.__layout.part_swallow("content", self.__pager)
        self.__pager.show()

        self.__pages = {}
        self.__notification = None

    def _subtitle_text_set(self, value):
        if not value:
            self.__layout.signal_emit("wizard,subtitle,hide", "")
            self.__layout.part_text_set("subtitle.text", "")
        else:
            self.__layout.signal_emit("wizard,subtitle,show", "")
            self.__layout.part_text_set("subtitle.text", value)

    subtitle_text = property(fset=_subtitle_text_set)

    def alternate_background_set(self, value):
        if value:
            self.__layout.signal_emit("wizard,bg,alternate", "")
        else:
            self.__layout.signal_emit("wizard,bg,default", "")

    def _title_text_set(self, value):
        if not value:
            self.__layout.part_text_set("title.text", "")
        else:
            self.__layout.part_text_set("title.text", value)

    title_text = property(fset=_title_text_set)

    def page_add(self, name, title="", subtitle="", separator=False):
        box = Box(self)
        box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        box.show()

        content = Box(self)
        content.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        content.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        box.pack_end(content)
        content.show()

        if separator:
            sp = Separator(self)
            sp.horizontal_set(True)
            box.pack_end(sp)
            sp.show()

        actions = Box(self)
        actions.horizontal_set(True)
        box.pack_end(actions)
        actions.show()

        self.__pages[name] = (title, subtitle, box, content, actions, {})
        self.title_text = title
        self.subtitle_text = subtitle
        self.__pager.content_push(box)

    def content_add(self, pg_name, c):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page
            content.pack_end(c)

    # TODO: add support for equal-named actions on a page, if needed
    def action_add(self, pg_name, label, func_cb, data=None, icon=None):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page

            btn = Button(self._parent)
            btn.label_set(label)
            btn.callback_clicked_add(self.__action_btn_clicked)
            btn.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
            btn.size_hint_align_set(evas.EVAS_HINT_FILL,
                                    evas.EVAS_HINT_FILL)
            btn.data["clicked"] = (func_cb, data)

            if icon:
                ico = Icon(self._parent)
                ico.file_set(self.__theme_file, "editje/icon/" + icon)
                btn.icon_set(ico)
                ico.show()

            action_btns[label] = btn

            btn.show()
            actions.pack_end(btn)

    def action_disabled_set(self, pg_name, label, disabled):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page
            action_btns[label].disabled_set(disabled)

    def goto(self, page):
        page = self.__pages.get(page)
        if page:
            title, subtitle, box, content, actions, action_btns = page
            self.title_text = title
            self.subtitle_text = subtitle
            self.__pager.content_promote(box)

    def notify(self, message):
        if self.__notification:
            self.__notification.hide()
            self.__notification.delete()
            self.__notification = None
        self.__notification = Notify(self)
        self.__notification.timeout_set(2)
        self.__notification.orient_set(ELM_NOTIFY_ORIENT_TOP)

        bx = Box(self)
        bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bx.horizontal_set(True)
        self.__notification.content_set(bx)
        bx.show()

        lb = Label(self)
        lb.label_set(message)
        bx.pack_end(lb)
        lb.show()

        self.__notification.show()

    def open(self):
        self._parent.block(True)
        self.show()

    def close(self):
        self.hide()
        self._parent.block(False)
        self.delete()

    def __action_btn_clicked(self, obj, *args, **kwargs):
        func, data = obj.data["clicked"]
        if data:
            func(data)
        else:
            func()
