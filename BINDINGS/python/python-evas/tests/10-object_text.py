import evas
import evas.c_evas
import unittest

class TextBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

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


unittest.main()
evas.shutdown()
