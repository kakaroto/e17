#!/usr/bin/env python

import sys
import ecore
import evas
import ecore.evas

if 'x11' in sys.argv:
    ee = ecore.evas.SoftwareX11(w=800, h=480)
else:
    ee = ecore.evas.SoftwareX11_16(w=800, h=480)

bg = ee.evas.Rectangle(color=(255, 255, 255, 255))
bg.size = ee.evas.size
bg.show()

img = ee.evas.Image()
img.file_set("icon.png")
img.move(380, 200)
w, h = img.image_size_get()
img.resize(w, h)
img.fill_set(0, 0, w, h)
img.show()

rotation = [evas.EVAS_IMAGE_ROTATE_90]
def rotate_img(rotation):
    img.rotate(rotation[0])
    rotation[0] += 1
    return rotation[0] <= evas.EVAS_IMAGE_ROTATE_270

ee.fullscreen = False
ee.show()

ecore.timer_add(2.0, rotate_img, rotation)
ecore.main_loop_begin()

# prevent segfault
del ee
