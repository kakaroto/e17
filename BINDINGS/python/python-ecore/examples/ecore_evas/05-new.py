#!/usr/bin/python

import sys
import ecore.evas
import evas
import ecore

try:
    engine_name = sys.argv[1]
except IndexError:
    engine_name = None

try:
    extra_options = sys.argv[2]
except IndexError:
    extra_options = None

ee = ecore.evas.new(engine_name, 10, 20, 300, 400, extra_options)

canvas = ee.evas
o = evas.Rectangle(canvas, size=canvas.size, color="#ff0000")
o.show()

o.event_callback_add(evas.EVAS_CALLBACK_DEL, lambda *a: ecore.main_loop_quit())
ee.associate(o)

ee.show()
ecore.main_loop_begin()
