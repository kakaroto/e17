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

import evas

from elementary import InnerWindow, Layout, Box, ScrolledEntry
from floater import WizardAction
import sysconfig

_instance = None


class LogWindow(InnerWindow):
    default_width = 500
    default_height = 500
    default_group = "editje/wizard"

    def __init__(self, parent, width=None, height=None, group=None):
        InnerWindow.__init__(self, parent)

        self._parent = parent
        #self.style_set("minimal")  # size fallbacks to __layout's min/max

        box = Box(self)
        box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        box.show()

        content = Box(self)
        content.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                     evas.EVAS_HINT_EXPAND)
        content.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        box.pack_end(content)
        content.show()

        actions = Box(self)
        actions.horizontal_set(True)
        box.pack_end(actions)
        actions.show()

        btn = WizardAction(self._parent, "Close", self.close, "Escape", None)
        btn.show()
        actions.pack_end(btn)

        layout = Layout(self)
        self.__edje = layout.edje_get()
        theme_file = sysconfig.theme_file_get("default")
        _width = width or self.default_width
        _height = height or self.default_height
        _group = group or self.default_group
        layout.file_set(theme_file, _group)
        layout.size_hint_min_set(_width, _height)
        layout.size_hint_max_set(_width, _height)

        InnerWindow.content_set(self, layout)
        layout.show()
        layout.content_set("content", box)

        self.__sc = ScrolledEntry(self)
        self.__sc.editable_set(False)
        self.__sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                evas.EVAS_HINT_EXPAND)
        self.__sc.size_hint_align_set(evas.EVAS_HINT_FILL,
                               evas.EVAS_HINT_FILL)
        self.__sc.bounce_set(False, False)
        self.__sc.show()
        content.pack_end(self.__sc)

        #self._action_add("Close", self.close, key="Escape")

    def _subtitle_text_set(self, value):
        if not value:
            self.__edje.signal_emit("wizard,subtitle,hide", "")
            self.__edje.part_text_set("subtitle.text", "")
        else:
            self.__edje.signal_emit("wizard,subtitle,show", "")
            self.__edje.part_text_set("subtitle.text", value)

    subtitle_text = property(fset=_subtitle_text_set)

    def _title_text_set(self, value):
        if not value:
            self.__edje.part_text_set("title.text", "")
        else:
            self.__edje.part_text_set("title.text", value)

    title_text = property(fset=_title_text_set)

    def message_set(self, message, title="", subtitle=""):
        self.__sc.entry_set(message)
        self.title_text = title
        self.subtitle_text = subtitle

    def show(self):
        global _instance
        if _instance:
            _instance.hide()
        _instance = self

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
