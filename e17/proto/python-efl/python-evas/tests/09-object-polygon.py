import evas
import evas.c_evas
import unittest

class PolygonBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        points = ((0, 5), (5, 0), (10, 5), (5, 10), (0, 5))
        o = evas.Polygon(self.canvas, points=points, color="#ff0000")
        self.assertEqual(o.color, (255, 0, 0, 255))

    def testCanvasFactory(self):
        points = ((0, 5), (5, 0), (10, 5), (5, 10))
        o = self.canvas.Polygon(points=points, color="#ff0000")
        self.assertEqual(isinstance(o, evas.c_evas.Polygon), True)
        self.assertEqual(o.color, (255, 0, 0, 255))


unittest.main()
evas.shutdown()
