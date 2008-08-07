# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

import evas
import evas.utils
import logging

log = logging.getLogger("evas.dbg")

class DebugObserver(object):
    """Debug object events using an observer that prints debug messages.

    Debug messages are printed using logging.debug() with "evas.dbg" logger.

    One can control which messages to use by providing on_* parameter, later
    used as attributes.
    """
    def __init__(self, obj,
                 on_resize=True,
                 on_move=True,
                 on_hide=True,
                 on_show=True,
                 on_restack=False,
                 on_free=False,
                 on_mouse_in=False,
                 on_mouse_out=False,
                 on_mouse_down=False,
                 on_mouse_up=False,
                 on_mouse_move=False,
                 on_mouse_wheel=False,
                 on_key_down=False,
                 on_key_up=False,
                 on_focus_in=False,
                 on_focus_out=False):
        self.on_resize = on_resize
        self.on_move = on_move
        self.on_hide = on_hide
        self.on_show = on_show
        self.on_restack = on_restack
        self.on_free = on_free
        self.on_mouse_in = on_mouse_in
        self.on_mouse_out = on_mouse_out
        self.on_mouse_down = on_mouse_down
        self.on_mouse_up = on_mouse_up
        self.on_mouse_move = on_mouse_move
        self.on_mouse_wheel = on_mouse_wheel
        self.on_key_down = on_key_down
        self.on_key_up = on_key_up
        self.on_focus_in = on_focus_in
        self.on_focus_out = on_focus_out
        evas.utils.connect_observer(obj, self)
        self.obj = obj

    def unregister(self):
        if self.obj:
            evas.utils.disconnect_observer(self.obj, self)
            self.obj = None

    def cb_on_resize(self, obj):
        if self.on_resize:
            w, h = obj.size_get()
            log.debug("resize to %dx%d: %.20r...", w, h, obj)

    def cb_on_move(self, obj):
        if self.on_move:
            x, y = obj.pos_get()
            log.debug("move to %d,%d: %.20r...", x, y, obj)

    def cb_on_hide(self, obj):
        if self.on_hide:
            log.debug("hide %.20r...", obj)

    def cb_on_show(self, obj):
        if self.on_show:
            log.debug("show %.20r...", obj)

    def cb_on_restack(self, obj):
        if self.on_restack:
            log.debug("restack %.20r...", obj)

    def cb_on_free(self, obj):
        if self.on_free:
            log.debug("clip unsset: %.20r...", obj)

    def cb_on_mouse_in(self, obj, e):
        if self.on_mouse_in:
            log.debug("mouse in %s: %.20r...", e, obj)

    def cb_on_mouse_out(self, obj, e):
        if self.on_mouse_out:
            log.debug("mouse out %s: %.20r...", e, obj)

    def cb_on_mouse_down(self, obj, e):
        if self.on_mouse_down:
            log.debug("mouse down %s: %.20r...", e, obj)

    def cb_on_mouse_up(self, obj, e):
        if self.on_mouse_up:
            log.debug("mouse up %s: %.20r...", e, obj)

    def cb_on_mouse_move(self, obj, e):
        if self.on_mouse_move:
            log.debug("mouse move %s: %.20r...", e, obj)

    def cb_on_mouse_wheel(self, obj, e):
        if self.on_mouse_wheel:
            log.debug("mouse wheel %s: %.20r...", e, obj)

    def cb_on_key_down(self, obj, e):
        if self.on_key_down:
            log.debug("key down %s: %.20r...", e, obj)

    def cb_on_key_up(self, obj, e):
        if self.on_key_up:
            log.debug("key up %s: %.20r...", e, obj)

    def cb_on_focus_in(self, obj, e):
        if self.on_focus_in:
            log.debug("focus in %s: %.20r...", e, obj)

    def cb_on_focus_out(self, obj, e):
        if self.on_focus_out:
            log.debug("focus out %s: %.20r...", e, obj)


class VisualDebug(evas.ClippedSmartObject):
    """Debug object events using an observer that lays on top of it.

    This visual debug attaches an smart object that will follow the given
    object and draw red lines on top and left and blue lines on bottom
    and right with squares on these corners.

    Text is used to display information:
     - class, id, name
     - geometry
     - clip id, parent id
    """
    def __init__(self, obj):
        evas.ClippedSmartObject.__init__(self, obj.evas)
        evas.utils.connect_observer(obj, self)
        self.obj = obj

        self.layer_set(self.obj.layer_get())
        self.stack_above(self.obj)
        self.top = self.Rectangle(color=(255, 0, 0, 255))
        self.top.show()
        self.left = self.Rectangle(color=(255, 0, 0, 255))
        self.left.show()
        self.rel1 = self.Rectangle(color=(255, 0, 0, 255), size=(8, 8))
        self.rel1.show()

        self.bottom = self.Rectangle(color=(0, 0, 255, 255))
        self.bottom.show()
        self.right = self.Rectangle(color=(0, 0, 255, 255))
        self.right.show()
        self.rel2 = self.Rectangle(color=(0, 0, 255, 255), size=(8, 8))
        self.rel2.show()

        self.title = self.Text(color=(255, 255, 255, 255),
                              font=("sans", 10),
                              outline_color=(0, 0, 0, 255))
        self.title.style_set(evas.EVAS_TEXT_STYLE_SOFT_OUTLINE)
        self.title.text_set("%s %#x %s" %
                           (self.obj.__class__.__name__, id(self.obj),
                            self.name_get()))
        self.title.show()

        self.desc = []
        for i in xrange(2):
            t = self.Text(color=(255, 255, 255, 255),
                          font=("sans", 10),
                          outline_color=(0, 0, 0, 255))
            t.style_set(evas.EVAS_TEXT_STYLE_SOFT_OUTLINE)
            t.show()
            self.desc.append(t)
        self.geometry_set(*self.obj.geometry_get())
        self._update_desc()

    def unregister(self):
        if self.obj:
            evas.utils.disconnect_observer(self.obj, self)
            self.obj = None
        self.hide()

    def show_desc(self):
        self.title.show()
        for d in self.desc:
            d.show()

    def hide_desc(self):
        self.title.hide()
        for d in self.desc:
            d.hide()

    def _update_desc(self):
        clip = self.obj.clip_get()
        if clip is None:
            clip = 0
        else:
            clip = id(clip)

        parent = self.obj.parent_get()
        if parent is None:
            parent = 0
        else:
            parent = id(parent)
        self.desc[0].text_set("g=%s" % (self.obj.geometry_get(),))
        self.desc[1].text_set("c=%#x, p=%#x" % (clip, parent))

    def resize(self, w, h):
        x, y = self.pos_get()
        self.top.geometry_set(x - 1, y - 1, w + 2, 1)
        self.bottom.geometry_set(x - 1, y + h + 1, w + 2, 1)

        self.left.geometry_set(x - 1, y - 1, 1, h + 2)
        self.right.geometry_set(x + w + 1, y - 1, 1, h + 2)

        self.rel1.center_set(x, y)
        self.rel2.center_set(x + w, y + h)
        self.title.geometry_set(x, y, w, 20)
        for i, d in enumerate(self.desc):
            i += 1
            d.geometry_set(x, y + 20 * i, w, 20)

    def cb_on_move(self, obj):
        self.move(*obj.pos_get())
        self._update_desc()

    def cb_on_resize(self, obj):
        self.resize(*obj.size_get())
        self._update_desc()

    def cb_on_restack(self, obj):
        self.layer_set(self.obj.layer_get())
        self.stack_above(self.obj)

    def cb_on_hide(self, obj):
        self.hide()

    def cb_on_show(self, obj):
        self.show()

    def cb_on_free(self, obj):
        self.delete()
