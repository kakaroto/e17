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
import edje
from elementary import Layout, Button, InnerWindow, Box, Pager, \
                       Icon, Label, Notify, ELM_NOTIFY_ORIENT_TOP, Separator
import sysconfig

_instance = None


class Floater(Layout):
    default_padding_x = 20
    default_padding_y = 20
    default_align_x = 0.5
    default_align_y = 0.5

    def __init__(self, parent, rel_to_obj=None):
        if not rel_to_obj:
            raise TypeError("You must pass an object whose geometry the"
                            " Floater will use to move itself in the canvas.")
        Layout.__init__(self, parent)

        theme_file = sysconfig.theme_file_get("default")
        self.file_set(theme_file, "editje/floater")

        self._parent = parent
        self._rel_to_obj = rel_to_obj
        self._rel_to_obj.on_move_add(self._move_and_resize)
        self.on_changed_size_hints_add(self._move_and_resize)

        self._padding_x = self.default_padding_x
        self._padding_y = self.default_padding_y
        self._align_x = self.default_align_x
        self._align_y = self.default_align_y

        self._action_btns = []
        self._min_size = [0, 0]

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

        if oy - py < 0:
            oy = py
        elif oy + oh + py >= ch:
            oy = ch - oh - py

        self.resize(ow, oh)
        self.move(ox, oy)

    def title_set(self, title):
        self.title = title
        self.edje_get().part_text_set("title.text", title)

    def size_hint_min_get(self):
        w, h = Layout.size_hint_min_get(self)

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
        Layout.content_set(self, "content", content)

    def action_add(self, label, func_cb, data=None):
        btn = Button(self._parent)
        self._action_btns.append(btn)
        btn.text_set(label)
        btn.callback_clicked_add(self._action_btn_clicked)
        btn.size_hint_weight_set(1.0, 1.0)
        btn.size_hint_align_set(-1.0, -1.0)
        btn.data["clicked"] = (func_cb, data)
        btn.show()
        self.edje_get().part_box_append("actions", btn)

    def action_remove(self, label):
        for action in self._action_btns:
            if action.label_get() == label:
                self.edje_get().part_box_remove("actions", action)
                self._action_btns.remove(action)
                action.delete()

    def actions_clear(self):
        for action in self._action_btns:
            self.edje_get().part_box_remove("actions", action)
            action.delete()
        del self._action_btns[:]

    def show(self):
        self._move_and_resize(self)
        global _instance
        if _instance:
            _instance.hide()
        _instance = self
        Layout.show(self)

    def hide(self):
        global _instance
        if _instance == self:
            _instance = None
        Layout.hide(self)

    def move(self, *args):
        Layout.move(self, *args)

    def resize(self, *args):
        Layout.resize(self, *args)

    def _action_btn_clicked(self, obj, *args, **kwargs):
        func, udata = obj.data["clicked"]
        func(self, udata)


class WizardAction(Button):
    def __init__(self, parent, label, func, key=None, data=None):
        Button.__init__(self, parent)
        self.text_set(label)
        self.callback_clicked_add(self.__action_btn_clicked)
        self.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                  evas.EVAS_HINT_EXPAND)
        self.size_hint_align_set(evas.EVAS_HINT_FILL,
                                 evas.EVAS_HINT_FILL)

        self._func = func
        self._data = data
        self._key = key

    def _key_get(self):
        return self._key

    key = property(_key_get)

    def __action_btn_clicked(self, obj):
        self()

    def __call__(self):
        if self.disabled_get():
            return
        if self._data:
            self._func(self._data)
        else:
            self._func()


# TODO: move this class elsewhere
class Wizard(InnerWindow):
    default_width = 500
    default_height = 500
    default_group = "editje/wizard"

    def __init__(self, parent, width=None, height=None, group=None):
        InnerWindow.__init__(self, parent)

        self._parent = parent
        #self.style_set("minimal")  # size fallbacks to __layout's min/max

        self.__layout = Layout(self)
        self.__edje = self.__layout.edje_get()
        self.__theme_file = sysconfig.theme_file_get("default")
        self.__width = width or self.default_width
        self.__height = height or self.default_height
        _group = group or self.default_group
        self.__layout.file_set(self.__theme_file, _group)
        self.__layout.size_hint_min_set(self.__width, self.__height)
        self.__layout.size_hint_max_set(self.__width, self.__height)

        self.on_key_down_add(self.__key_down_cb)

        InnerWindow.content_set(self, self.__layout)
        self.__layout.show()

        self.__pager = Pager(self)
        self.__pager.style_set("editje.rightwards")
        self.__pager.show()
        self.__layout.content_set("content", self.__pager)

        self.__pages = {}
        self.__current_page = None
        self.__notification = None

    def _subtitle_text_set(self, value):
        if not value:
            self.__edje.signal_emit("wizard,subtitle,hide", "")
            self.__edje.part_text_set("subtitle.text", "")
        else:
            self.__edje.signal_emit("wizard,subtitle,show", "")
            self.__edje.part_text_set("subtitle.text", value)

    subtitle_text = property(fset=_subtitle_text_set)

    def alternate_background_set(self, value):
        if value:
            self.__edje.signal_emit("wizard,bg,alternate", "")
        else:
            self.__edje.signal_emit("wizard,bg,default", "")

    def _title_text_set(self, value):
        if not value:
            self.__edje.part_text_set("title.text", "")
        else:
            self.__edje.part_text_set("title.text", value)

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
        self.__current_page = name

    def content_add(self, pg_name, c):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page
            content.pack_end(c)

    # TODO: add support for equal-named actions on a page, if needed
    def action_add(self, pg_name, label, func_cb, data=None, icon=None,
                   key=None):
        page = self.__pages.get(pg_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page

            btn = WizardAction(self._parent, label, func_cb, key, data)

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

    def goto(self, page_name):
        page = self.__pages.get(page_name)
        if page:
            title, subtitle, box, content, actions, action_btns = page
            self.title_text = title
            self.subtitle_text = subtitle
            self.__pager.content_promote(box)
            self.__current_page = page_name

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
        lb.text_set(message)
        bx.pack_end(lb)
        lb.show()

        self.__notification.show()

    def show(self):
        global _instance
        if _instance:
            _instance.hide()
        _instance = self

        # only Wizard InnerWindows are to be on top of the window blocker
        self._parent.block(True, self)
        InnerWindow.show(self)
        self.focus_set(True)

    def hide(self):
        global _instance
        if _instance == self:
            _instance = None
        InnerWindow.hide(self)
        self._parent.block(False)

    def open(self):
        self.show()

    def close(self):
        self._parent.focus_set(True)
        self.hide()
        self.delete()

    def __key_down_cb(self, obj, event):
        page = self.__pages.get(self.__current_page)
        if not page:
            return
        acts = page[-1]
        for a in acts.itervalues():
            if a.key == event.keyname:
                a()
                return
