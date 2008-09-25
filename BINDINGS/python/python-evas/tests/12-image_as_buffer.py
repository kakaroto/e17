#!/usr/bin/python

import ecore, ecore.evas, sys, os

if 'x11-16' in sys.argv:
    ee = ecore.evas.SoftwareX11_16(w=800, h=480)
else:
    ee = ecore.evas.SoftwareX11(w=800, h=480)

canvas = ee.evas

bg = canvas.Rectangle(color=(255, 255, 255, 255), size=canvas.size)
bg.show()

i1 = canvas.Image()
i1.file_set("icon.png")
i1.move(100, 100)
w, h = i1.image_size_get()
i1.resize(w, h)
i1.fill_set(0, 0, w, h)
i1.show()

wf = open("test.raw", "wb")
#print i1.stride, i1.image_size
wf.write(i1)
wf.close()

i2 = canvas.Image()
i2.alpha_set(True)
i2.image_size_set(w, h)
i2.move(100 + w + 10, 100)
i2.resize(w, h)
i2.fill_set(0, 0, w, h)
i2.show()

rf = open("test.raw", "rb")
b = rf.read()
#print len(b), i2.stride, i2.image_size
i2.image_data_set(b)
rf.close()

os.unlink("test.raw")

i3 = canvas.Image()
i3.alpha_set(True)
i3.image_size_set(w, h)
i3.move(100 + w + 10, 100 + h + 10)
i3.resize(w, h)
i3.fill_set(0, 0, w, h)
i3.show()
i3.image_data_set(i1)

ee.show()
ecore.main_loop_begin()
