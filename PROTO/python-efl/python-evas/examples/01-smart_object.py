#!/usr/bin/python

import evas
import ecore.evas
import ecore
import math


class MySmartObject(evas.ClippedSmartObject):
    def __init__(self, *args, **kargs):
        evas.ClippedSmartObject.__init__(self, *args, **kargs)
        # It's recommended to use self.XXX to create member objects
        self.bg = self.Rectangle(color=(200, 200, 200, 255))
        self.bg.show()
        # But one can do the regular way, just remember to member_add()!
        # This is required by non-primitive objects, like Edje and other
        # SmartObjects
        self.child = evas.Rectangle(self.evas, color=(255, 0, 0, 255),
                                    size=(10, 10))
        self.member_add(self.child)
        self.child.show()

        self.vx, self.vy = 1, 2
        self.animator = ecore.animator_add(self._animator_cb)

    def resize(self, w, h):
        # The only required virtual to be implemented
        self.bg.resize(w, h)

    def delete(self):
        # Just implement to stop the animator, then call the super method
        # to delete children (bg, child)
        self.animator.stop()
        evas.ClippedSmartObject.delete(self)

    def _animator_cb(self):
        # Move child, but keep inside bounding box (bg)
        self.child.move_relative(self.vx, self.vy)
        cr = self.child.rect
        br = self.bg.rect
        if cr.top < br.top:
            cr.top = br.top
            self.vy = -self.vy
        elif cr.bottom > br.bottom:
            cr.bottom = br.bottom
            self.vy = -self.vy

        if cr.left < br.left:
            cr.left = br.left
            self.vx = -self.vx
        elif cr.right > br.right:
            cr.right = br.right
            self.vx = -self.vx

        self.child.rect = cr
        return True


def resize_cb(ee):
    bg = ee.data["bg"]
    obj = ee.data["obj"]
    canvas = ee.evas
    bg.size = canvas.size
    w, h = canvas.size
    obj.size = (w / 2, h / 2)
    obj.center = canvas.rect.center


if __name__ == "__main__":
    ee = ecore.evas.SoftwareX11(w=800, h=600)

    canvas = ee.evas
    bg = canvas.Rectangle(color=(255, 255, 255, 255))
    bg.size = canvas.size
    bg.show()

    w, h = canvas.size
    obj = MySmartObject(canvas)
    obj.size = (w / 2, h / 2)
    obj.center = canvas.rect.center
    obj.show()

    ee.data["bg"] = bg
    ee.data["obj"] = obj
    ee.callback_resize = resize_cb

    ee.show()
    ecore.main_loop_begin()
