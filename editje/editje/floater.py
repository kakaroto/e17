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
                       Icon, Label, Notify, ELM_NOTIFY_ORIENT_TOP

import sysconfig

from elementary import Separator

class Floater(object):
    def __init__(self, parent):
        self._parent = parent
        self.popup = Layout(parent)
        self.size_hint_min_set(0, 0)
        self._size_hint_min_cbs = []
        self._action_btns = []
        self._min_size_hints = [0, 0]

        theme_file = sysconfig.theme_file_get("default")
        self.popup.file_set(theme_file, "editje/floater")

    def title_set(self, title):
        self.title = title
        self.popup.edje_get().part_text_set("title.text", title)

    def size_hint_min_get(self):
        return self._min_size_hints

    def size_hint_min_set(self, w, h):
        self._min_size_hints = [w, h]

    def on_changed_size_hints_add(self, func, *args, **kargs):
        if not callable(func):
            raise TypeError("func must be callable")

        r = (func, args, kargs)
        self._size_hint_min_cbs.append(r)

    def on_changed_size_hints_del(self, func):
        i = None
        for i, r in enumerate(self._size_hint_min_cbs):
            if func == r[0]:
                break
        else:
            raise ValueError("Callback %s was not registered before" % func)

        self._size_hint_min_cbs.pop(i)

    def content_set(self, content):
        content.size_hint_weight_set(1.0, 1.0)
        content.size_hint_align_set(-1.0, -1.0)
        self.popup.content_set("content", content)

    def action_add(self, label, func_cb, data = None):
        btn = Button(self._parent)
        self._action_btns.append(btn)
        btn.on_changed_size_hints_add(self._size_hint_min_recalc)
        btn.label_set(label)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_weight_set(1.0, 1.0)
        btn.size_hint_align_set(-1.0, -1.0)
        btn.data["clicked"] = (func_cb, data)
        btn.show()
        self.popup.edje_get().part_box_append("actions", btn)

    def _children_changed_cb(self):
        for cb in self._size_hint_min_cbs:
            func, args, kargs = cb
            func(self, *args, **kargs)

    def _size_hint_min_recalc(self, obj, *args, **kwargs):
        o_min_w, o_min_h = self.size_hint_min_get()
        self.size_hint_min_set(0, 0)

        for b in self._action_btns:
            b_min_w, b_min_h = b.size_hint_min_get()
            self._min_size_hints[0] += b_min_w + 10
            self._min_size_hints[1] += b_min_h + 10

        self._children_changed_cb()

    def show(self):
        self.popup.show()

    def hide(self):
        self.popup.hide()

    def open(self):
        self._parent.block(True)
        self.show()

    def close(self):
        self.hide()
        self._parent.block(False)
        self.popup.delete()

    def move(self, *args):
        self.popup.move(*args)

    def resize(self, *args):
        self.popup.resize(*args)

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
        self.__actions_list = {}
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

        self.__pages[name] = (title, subtitle, box, content, actions)
        self.title_text = title
        self.subtitle_text = subtitle
        self.__pager.content_push(box)

    def content_add(self, pg_name, c):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions = page
            content.pack_end(c)

    def action_add(self, pg_name, label, func_cb, data=None, icon=None):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions = page

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

            self.__actions_list[label] = btn

            btn.show()
            actions.pack_end(btn)

    def action_disabled_set(self, label, disabled):
        self.__actions_list[label].disabled_set(disabled)

    def goto(self, page):
        page = self.__pages.get(page)
        if page:
            title, subtitle, box, content, actions = page
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
