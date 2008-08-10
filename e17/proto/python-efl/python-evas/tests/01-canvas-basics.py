import evas
import unittest

class CanvasConstructor(unittest.TestCase):
    def testNoConstructorArgs(self):
        c = evas.Canvas()
        self.assertEqual(c.output_method_get(), 0)
        self.assertEqual(c.size_get(), (0, 0))
        self.assertEqual(c.viewport_get(), (0, 0, 1, 1))

    def testConstructorArgs(self):
        m = "software_x11"
        s = (400, 500)
        v = (0, 0, 30, 40)
        c = evas.Canvas(method=m, size=s, viewport=v)
        self.assertEqual(c.output_method_get(), evas.render_method_lookup(m))
        self.assertEqual(c.size_get(), s)
        self.assertEqual(c.viewport_get(), v)
        self.assertEqual(isinstance(str(c), str), True)
        self.assertEqual(isinstance(repr(c), str), True)


class CanvasMethods(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11", size=(400, 500))

    def tearDown(self):
        del self.canvas

    def testSizeGet(self):
        self.assertEqual(self.canvas.size_get(), (400, 500))

    def testSizeSet(self):
        self.canvas.size_set(200, 300)
        self.assertEqual(self.canvas.size_get(), (200, 300))


class CanvasProperties(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11", size=(400, 500))

    def tearDown(self):
        del self.canvas

    def testSizeGet(self):
        self.assertEqual(self.canvas.size, (400, 500))

    def testSizeSet(self):
        self.canvas.size = (200, 300)
        self.assertEqual(self.canvas.size_get(), (200, 300))

    def testRectGet(self):
        self.assertEqual(self.canvas.rect, (0, 0, 400, 500))

    def testRectSetTuple(self):
        self.canvas.rect = (0, 0, 200, 300)
        self.assertEqual(self.canvas.size_get(), (200, 300))

    def testRectSetRect(self):
        self.canvas.rect = evas.Rect(size=(200, 300))
        self.assertEqual(self.canvas.size_get(), (200, 300))


unittest.main()
evas.shutdown()
