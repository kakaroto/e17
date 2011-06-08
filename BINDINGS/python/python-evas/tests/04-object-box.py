"""tests for the evas.Box object class"""
import evas
import unittest

class BoxBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="buffer",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))
        self.canvas.engine_info_set(self.canvas.engine_info_get())

    def tearDown(self):
        del self.canvas

    def testConstructor(self):
        """create box object using default construction"""
        box = evas.Box(self.canvas)
        self.assertEqual(type(box), evas.Box)

    def testCanvasFactory(self):
        """create box object from canvas factory function"""
        self.assertNotEqual(getattr(self.canvas, 'Box', None), None, "missing factory function in canvas")
        box = self.canvas.Box()
        self.assertEqual(type(box), evas.Box)

    def testConstructorBaseParameters(self):
        """make sure that parameters are correctly passed to the baseclass"""
        size = (20, 30)
        pos = (40, 50)
        geometry = (60, 70, 80, 90)
        color = (110, 120, 130, 140)

        # create box using size/pos
        box1 = evas.Box(self.canvas, name="box1", color=color, size=size, pos=pos)
        self.assertEqual(box1.name, "box1")
        self.assertEqual(box1.color, color)
        self.assertEqual(box1.size, size)
        self.assertEqual(box1.pos, pos)

        # cleate box2 using geometry
        box2 = evas.Box(self.canvas, name="box2", color=color, geometry=geometry)
        self.assertEqual(box2.name, "box2")
        self.assertEqual(box2.color, color)
        self.assertEqual(box2.geometry, geometry)


unittest.main()
evas.shutdown()
