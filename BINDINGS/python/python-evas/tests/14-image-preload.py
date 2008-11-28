#!/usr/bin/env python

import sys
import os
import ecore
import evas
import ecore.evas

ee = None
required_image = "icon.png"
for a in sys.argv[1:]:
    if a == 'x11':
        ee = ecore.evas.SoftwareX11(w=800, h=480)
    elif a == 'x11-16':
        ee = ecore.evas.SoftwareX11_16(w=800, h=480)
    else:
        if os.path.exists(a):
            required_image = a
        else:
            raise SystemExit(("invalid argument: %s. "
                              "Expected x11, x11-16 or image path.") % a)

if not ee:
    ee = ecore.evas.SoftwareX11_16(w=800, h=480)

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
