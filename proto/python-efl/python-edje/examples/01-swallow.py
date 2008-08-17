#!/usr/bin/env python

import evas
import ecore
import ecore.evas
import edje
import edje.decorators
import sys
import os

w, h = 800, 480
ee = ecore.evas.SoftwareX11(w=w, h=h)

class Container(evas.ClippedSmartObject):
    padding = 2

    def __init__(self, *args, **kargs):
        evas.ClippedSmartObject.__init__(self, *args, **kargs)
        self.bg = self.Rectangle(color=(200, 200, 200, 255))
        self.bg.show()
        self.children = []

    def resize(self, w, h):
        x, y = self.pos
        self.bg.resize(w, h)
        self.reconfigure(x, y, w, h)

    def reconfigure(self, x, y, w, h):
        n = len(self.children)
        if n == 0:
            return
        elif n == 1:
            self.children[0].geometry_set(x, y, w, h)
            return

        cw = (w - self.padding * (n - 1)) / n
        for c in self.children:
            c.geometry_set(x, y, cw, h)
            x += cw + self.padding

    def add(self, child):
        self.children.append(child)
        self.member_add(child)
        self.reconfigure(*self.geometry)


class Screen(edje.Edje):
    def __init__(self, canvas, file):
        edje.Edje.__init__(self, canvas, file=file, group="layout")
        self.container = Container(self.evas)
        self.part_swallow("contents", self.container)

    @edje.decorators.signal_callback("mouse,clicked,1", "*")
    def cb_on_btn_clicked(self, emission, source):
        group = "%s-group" % source
        c = edje.Edje(self.evas, file=self.file_get()[0], group=group)
        c.show()
        self.container.add(c)


# Load and setup UI
ee.title = "Edje Swallow Demo"
canvas = ee.evas
edje_file = os.path.join(os.path.dirname(sys.argv[0]), "01-swallow.edj")

screen = Screen(canvas, edje_file)
screen.size = canvas.size
screen.show()
ee.data["screen"] = screen


def resize_cb(ee):
    r = ee.evas.rect
    ee.data["screen"].size = r.size
ee.callback_resize = resize_cb

ee.show()
ecore.main_loop_begin()
