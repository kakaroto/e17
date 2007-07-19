import evas
import evas.c_evas
import unittest

class GradientBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        self.assertEqual(o.geometry_get(), (10, 20, 30, 40))

    def testCanvasFactory(self):
        o = self.canvas.Gradient(geometry=(10, 20, 30, 40))
        self.assertEqual(isinstance(o, evas.c_evas.Gradient), True)
        self.assertEqual(o.geometry_get(), (10, 20, 30, 40))

    def testColorStopAdd(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        o.color_stop_add(255, 255, 255, 255, 10)
        o.color_stop_add(180 * 250 / 255, 180 * 240 / 255, 180 * 50 / 255,
                            180, 10)
        o.color_stop_add(120 * 220 / 255,  120 * 60 / 255, 0, 120, 10)
        o.color_stop_add(80 * 200 / 255, 0, 0, 80, 10)
        o.color_stop_add(0, 0, 0, 0, 10)
        o.show()

    def testGradientTypeGet(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        self.assertEqual(o.gradient_type, ("linear", None))

    def testGradientTypeSet(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        o.gradient_type = ("radial", )
        self.assertEqual(o.gradient_type, ("radial", None))

    def testFillSpreadGet(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        self.assertEqual(o.fill_spread, 0)

    def testFillSpreadSet(self):
        o = evas.Gradient(self.canvas, geometry=(10, 20, 30, 40))
        o.fill_spread = evas.EVAS_TEXTURE_RESTRICT_REPEAT
        self.assertEqual(o.fill_spread, evas.EVAS_TEXTURE_RESTRICT_REPEAT)


unittest.main()
evas.shutdown()
