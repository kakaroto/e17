#!/usr/bin/python

import ecore
import ecore.evas

import evas
from efl_utils.scroller import TextScrollerLeft

ee = ecore.evas.SoftwareX11(w=400, h=50)

canvas = ee.evas
bg = canvas.Rectangle(color=(255, 255, 255, 255))
bg.size = canvas.size
bg.show()

ee.title = "Scroll text test"
ee.data["bg"] = bg

def cb_on_resize(ee):
    ee.data["bg"].size = ee.evas.size
ee.callback_resize = cb_on_resize

scroll_text = TextScrollerLeft(canvas=canvas, text="Hello World",
                               font=("Arial", 30), color="#000000")

scroll_text.show()
scroll_text.geometry_set(0, 0, 400, 50)
scroll_text.start()

ee.show()
ecore.main_loop_begin()
