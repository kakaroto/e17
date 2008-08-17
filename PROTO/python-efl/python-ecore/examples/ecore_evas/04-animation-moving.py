#!/usr/bin/python

import ecore.evas
import ecore
import math


pi2 = math.pi * 2
def get_pos(center_x, center_y, w, h):
    t = ecore.time_get()
    t = (t % pi2) - math.pi # keep time between -pi and pi
    x = center_x + w * math.cos(t)
    y = center_y + h * math.sin(t)
    return x, y


def animate_obj(ee, obj):
    center_x, center_y = ee.evas.rect.center
    w, h = ee.evas.size
    x, y = get_pos(center_x, center_y, w * 0.3, h * 0.3)
    obj.center = (x, y)
    return True


def resize_cb(ee):
    bg = ee.data["bg"]
    obj = ee.data["obj"]
    canvas = ee.evas
    bg.size = canvas.size
    obj.center = canvas.rect.center


if __name__ == "__main__":
    ee = ecore.evas.SoftwareX11(w=800, h=600)

    canvas = ee.evas
    bg = canvas.Rectangle(color=(255, 255, 255, 255))
    bg.size = canvas.size
    bg.show()

    obj = canvas.Image(file="icon.png")
    w, h = obj.image_size
    obj.size = (w, h)
    obj.fill = (0, 0, w, h)
    obj.center = canvas.rect.center
    obj.show()

    ee.data["bg"] = bg
    ee.data["obj"] = obj
    ee.callback_resize = resize_cb

    ecore.animator_add(animate_obj, ee, obj)

    ee.show()
    ecore.animator_frametime_set(1.0 / 60.0)
    ecore.main_loop_begin()
