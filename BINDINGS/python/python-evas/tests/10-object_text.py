# -*- coding: UTF-8 -*-
import evas
import evas.c_evas
import unittest

class TextBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="buffer",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))
        self.canvas.engine_info_set(self.canvas.engine_info_get())

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        o = evas.Text(self.canvas, text="Hello World",
                      font=("sans serif", 20), color="#ff0000")
        self.assertEqual(o.color, (255, 0, 0, 255))

    def testConstructor2(self):
        o = evas.Text(self.canvas, text="Hello World",
                      font="sans serif", color="#ff0000")
        self.assertEqual(o.color, (255, 0, 0, 255))

    def testCanvasFactory(self):
        o = self.canvas.Text(text="Hello World", font=("sans serif", 20),
                             color="#ff0000")
        self.assertEqual(isinstance(o, evas.c_evas.Text), True)
        self.assertEqual(o.color, (255, 0, 0, 255))

    def testASCII(self):
        """make sure that you can supply a unicode object as text

        This must both be accepted and preserved as Unicode string.
        """
        text = "ASCII"
        o = self.canvas.Text(text=text, font=("sans serif", 20),
                             color="#ff0000")
        self.assertEqual(isinstance(o.text_get(), str), True)
        self.assertEqual(o.text_get(), text)

    def testUnicode1(self):
        """make sure that you can supply a unicode object as text

        This must both be accepted and preserved as Unicode string.
        """
        text = u"Unicode"
        o = self.canvas.Text(text=text, font=("sans serif", 20),
                             color="#ff0000")
        self.assertEqual(isinstance(o.text_get(), unicode), True)
        self.assertEqual(o.text_get(), text)

    def testUnicode2(self):
        """make sure that non-ASCII characters are treated correctly

        The used string is the word 'mojibake', btw.
        """
        text = u"文字化け"
        o = self.canvas.Text(text=text, font=("sans serif", 20),
                             color="#ff0000")
        self.assertEqual(o.text_get(), text)

unittest.main()
evas.shutdown()
