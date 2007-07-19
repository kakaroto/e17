import evas
import evas.c_evas
import unittest

class CanvasFactory(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

    def tearDown(self):
        del self.canvas

    def testRectangle(self):
        obj = self.canvas.Rectangle(geometry=(10, 20, 30, 40), color="#ff0000")
        self.assertEqual(isinstance(obj, evas.c_evas.Rectangle), True)
        self.assertEqual(obj.geometry, (10, 20, 30, 40))
        self.assertEqual(obj.color, (255, 0, 0, 255))

    def testLine(self):
        start = (0, 0)
        end = (100, 200)
        s = (20, 30)
        p = (11, 22)
        obj = self.canvas.Line(start=start, end=end, size=s, pos=p)
        self.assertEqual(isinstance(obj, evas.c_evas.Line), True)
        self.assertEqual(obj.size_get(), s)
        self.assertEqual(obj.pos_get(), p)
        self.assertEqual(obj.xy_get(),
                         (p[0] + start[0],
                          p[1] + start[1],
                          p[0] + end[0],
                          p[1] + end[1]))


unittest.main()
evas.shutdown()
