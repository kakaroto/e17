#!/usr/bin/env python

import edje
import ecore.evas
import unittest


class Basics(unittest.TestCase):
    def setUp(self):
        self.canvas = ecore.evas.Buffer(300, 300)

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        o = edje.Edje(self.canvas.evas, file="theme.edj", group="main")
        o.show()

    def testConstructorNotExist(self):
        def f():
            o = edje.Edje(self.canvas.evas, file="inexistent", group="")
        self.assertRaises(edje.EdjeLoadError, f)

    def testPartExists(self):
        o = edje.Edje(self.canvas.evas, file="theme.edj", group="main")
        self.assertEqual(o.part_exists("bg"), True)

    def testPartNotExists(self):
        o = edje.Edje(self.canvas.evas, file="theme.edj", group="main")
        self.assertEqual(o.part_exists("bg123"), False)


unittest.main()
edje.shutdown()
ecore.evas.shutdown()
