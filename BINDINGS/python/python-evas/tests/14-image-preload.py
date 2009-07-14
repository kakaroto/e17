#!/usr/bin/env python

import sys
import os
import ecore
import evas
import ecore.evas

try:
    required_image = sys.argv[1]
except IndexError:
    required_image = os.path.join(os.path.dirname(sys.argv[0]), "icon.png")

ee = ecore.evas.new(w=800, h=400)
bg = ee.evas.Rectangle(color=(255, 255, 255, 255))
bg.size = ee.evas.size
bg.show()

step = -10

def pulse_stop():
    global pulser
    if not pulser:
        return
    pulser.delete()
    pulser = None
    bg.color = (255, 255, 255, 255)
    global step
    step = -10


def pulse_animation(bg):
    global step

    r, g, b, a = bg.color
    if not 0 <= r + step < 255:
        step = -step

    r = g = b = r + step
    bg.color = (r, g, b, a)
    return True

pulser = ecore.animator_add(pulse_animation, bg)

def img_preloaded(img):
    pulse_stop()
    img.show()

img = ee.evas.Image()
img.file_set(required_image)
img.fill_set(0, 0, 780, 460)
img.geometry = (10, 10, 780, 460)
img.on_image_preloaded_add(img_preloaded)
img.preload()

ee.fullscreen = False
ee.show()

ecore.main_loop_begin()

# prevent segfault
del ee
