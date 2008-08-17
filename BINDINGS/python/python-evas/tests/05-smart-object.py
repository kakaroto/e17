#!/usr/bin/env python

import evas
import unittest


class MyObject(evas.SmartObject):
    def __init__(self, canvas, *args, **kargs):
        evas.SmartObject.__init__(self, canvas, *args, **kargs)
        w, h = self.size
        w2 = w / 2
        h2 = h / 2
        self.r1 = evas.Rectangle(canvas, geometry=(0, 0, w2, h2),
                                 color="#ff0000")
        self.member_add(self.r1)

        self.r2 = evas.Rectangle(canvas, geometry=(w2, h2, w2, h2),
                                 color="#00ff00")
        self.member_add(self.r2)

    def resize(self, w, h):
        w2 = w / 2
        h2 = h / 2
        self.r1.geometry = (0, 0, w2, h2)
        self.r2.geometry = (w2, h2, w2, h2)


class SmartObjectTest(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))
        self.obj = MyObject(self.canvas)

    def testMembers(self):
        self.assertEqual(self.obj.members, (self.obj.r1, self.obj.r2))

    def testResize(self):
        self.obj.resize(100, 100)
        self.assertEqual(self.obj.r1.geometry, (0, 0, 50, 50))
        self.assertEqual(self.obj.r2.geometry, (50, 50, 50, 50))


unittest.main()
evas.shutdown()
