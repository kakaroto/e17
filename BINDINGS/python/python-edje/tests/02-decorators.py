#!/usr/bin/env python

import edje
import edje.decorators
import ecore.evas
import unittest


class Basics(unittest.TestCase):
    def setUp(self):
        self.canvas = ecore.evas.Buffer(300, 300)

    def tearDown(self):
        del self.canvas

    def testSignalCallback(self):
        class C(edje.Edje):
            @edje.decorators.signal_callback("bla", "mouse,clicked,1")
            def bla_clicked(self, emission, source):
                self.ok = True

        c = C(self.canvas.evas, file="theme.edj", group="main")
        c.signal_emit("bla", "mouse,clicked,1")
        c.message_signal_process()
        self.assertEqual(c.ok, True)

    def testTextChangeCallback(self):
        class C(edje.Edje):
            @edje.decorators.text_change_callback
            def on_text_change(self, part):
                self.ok = True

        c = C(self.canvas.evas, file="theme.edj", group="main")
        c.part_text_set("label", "bla")
        c.message_signal_process()
        self.assertEqual(c.ok, True)

    def testMessageHandler(self):
        class C(edje.Edje):
            @edje.decorators.message_handler
            def on_text_change(self, msg):
                self.ok = True

        c = C(self.canvas.evas, file="theme.edj", group="main")
        c.signal_emit("emit,message", "")
        c.message_signal_process()
        c.message_signal_process()
        self.assertEqual(c.ok, True)


unittest.main()
edje.shutdown()
ecore.evas.shutdown()
