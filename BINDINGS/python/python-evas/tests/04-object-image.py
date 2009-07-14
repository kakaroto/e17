import evas
import unittest

class ImageBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="buffer",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))
        self.canvas.engine_info_set(self.canvas.engine_info_get())

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        o = evas.Image(self.canvas, file="icon.png", geometry=(10, 20, 30, 40))
        self.assertEqual(o.geometry_get(), (10, 20, 30, 40))


unittest.main()
evas.shutdown()
