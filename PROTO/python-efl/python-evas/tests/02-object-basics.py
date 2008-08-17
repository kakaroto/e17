import evas
import unittest

class ObjectBasics(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

    def tearDown(self):
        del self.canvas

    def testDoubleDelete(self):
        obj = evas.Rectangle(self.canvas)
        obj.delete()
        self.assertRaises(ValueError, obj.delete)

    def testConstructor(self):
        s = (20, 30)
        p = (11, 22)
        obj = evas.Rectangle(self.canvas, size=s, pos=p)
        self.assertEqual(obj.size_get(), s)
        self.assertEqual(obj.pos_get(), p)
        obj.delete()

    def testConstructorGeometry(self):
        g = (11, 22, 20, 30)
        obj = evas.Rectangle(self.canvas, geometry=g)
        self.assertEqual(obj.geometry_get(), g)
        obj.delete()

    def testLineConstructor(self):
        start = (0, 0)
        end = (100, 200)
        s = (20, 30)
        p = (11, 22)
        obj = evas.Line(self.canvas, start=start, end=end, size=s, pos=p)
        self.assertEqual(obj.size_get(), s)
        self.assertEqual(obj.pos_get(), p)
        self.assertEqual(obj.xy_get(),
                         (p[0] + start[0],
                          p[1] + start[1],
                          p[0] + end[0],
                          p[1] + end[1]))
        obj.delete()

    def testTopGetSelf(self):
        o1 = evas.Rectangle(self.canvas)
        o2 = self.canvas.top_get()
        self.assertEqual(o1, o2)
        o1.delete()

    def testTopGetOther(self):
        o1 = evas.Rectangle(self.canvas)
        o2 = evas.Rectangle(self.canvas)
        self.assertNotEqual(o1, o2)

        o3 = self.canvas.top_get()
        self.assertEqual(o2, o3)
        self.assertNotEqual(o1, o3)

        o1.delete()
        o2.delete()

    def testBottomGetSelf(self):
        o1 = evas.Rectangle(self.canvas)
        o2 = self.canvas.bottom_get()
        self.assertEqual(o1, o2)
        o1.delete()

    def testBottomGetOther(self):
        o1 = evas.Rectangle(self.canvas)
        o2 = evas.Rectangle(self.canvas)
        self.assertNotEqual(o1, o2)

        o3 = self.canvas.bottom_get()
        self.assertEqual(o1, o3)
        self.assertNotEqual(o2, o3)

        o1.delete()
        o2.delete()

    def testClip(self):
        o1 = evas.Rectangle(self.canvas, geometry=(10, 10, 30, 30))
        o2 = evas.Rectangle(self.canvas, geometry=(0, 0, 100, 100))
        o2.clip_set(o1)
        self.assertEqual(o2.clip_get(), o1)

    def testClipDelete(self):
        o1 = evas.Rectangle(self.canvas, geometry=(10, 10, 30, 30))
        o2 = evas.Rectangle(self.canvas, geometry=(0, 0, 100, 100))
        o2.clip_set(o1)
        o1.delete()
        del o1
        self.assertEqual(o2.clip_get(), None)

    def testObjectNameFind(self):
        o1 = evas.Rectangle(self.canvas, name="Test 123")
        o2 = self.canvas.object_name_find("Test 123")
        self.assertEqual(o1, o2)

    def testStr(self):
        o = evas.Rectangle(self.canvas)
        self.assertEqual(isinstance(str(o), str), True)

    def testRepr(self):
        o = evas.Rectangle(self.canvas)
        self.assertEqual(isinstance(repr(o), str), True)



class ObjectProperties(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))
        self.obj = evas.Rectangle(self.canvas,
                                  geometry=(10, 20, 30, 40))

    def tearDown(self):
        self.obj.delete()
        del self.obj
        del self.canvas

    def testSizeGet(self):
        self.assertEqual(self.obj.size, (30, 40))

    def testSizeSet(self):
        self.obj.size = (33, 44)
        self.assertEqual(self.obj.size_get(), (33, 44))

    def testGeometryGet(self):
        self.assertEqual(self.obj.geometry, (10, 20, 30, 40))

    def testGeometrySet(self):
        self.obj.geometry = (11, 22, 33, 44)
        self.assertEqual(self.obj.geometry_get(), (11, 22, 33, 44,))

    def testPosGet(self):
        self.assertEqual(self.obj.pos, (10, 20))

    def testPosSet(self):
        self.obj.pos = (11, 22)
        self.assertEqual(self.obj.pos_get(), (11, 22))

    def testTopLeftGet(self):
        self.assertEqual(self.obj.top_left, (10, 20))

    def testTopLeftSet(self):
        self.obj.top_left = (11, 22)
        self.assertEqual(self.obj.pos_get(), (11, 22))

    def testTopRightGet(self):
        self.assertEqual(self.obj.top_right, (10 + 30, 20))

    def testTopRightSet(self):
        self.obj.top_right = (50, 22)
        self.assertEqual(self.obj.pos_get(), (50 - 30, 22))

    def testBottomLeftGet(self):
        self.assertEqual(self.obj.bottom_left, (10, 20 + 40))

    def testBottomLeftSet(self):
        self.obj.bottom_left = (11, 60)
        self.assertEqual(self.obj.pos_get(), (11, 60 - 40))

    def testBottomRightGet(self):
        self.assertEqual(self.obj.bottom_right, (10 + 30, 20 + 40))

    def testBottomRightSet(self):
        self.obj.bottom_right = (50, 60)
        self.assertEqual(self.obj.pos_get(), (50 - 30, 60 - 40))

    def testCenterGet(self):
        self.assertEqual(self.obj.center, (10 + 30/2, 20 + 40/2))

    def testCenterSet(self):
        self.obj.center = (50, 60)
        self.assertEqual(self.obj.pos_get(), (50 - 30/2, 60 - 40/2))

    def testData(self):
        self.obj.data["test"] = 123
        self.assertEqual(self.obj.data["test"], 123)

    def testName(self):
        self.obj.name = "My Object"
        self.assertEqual(self.obj.name_get(), "My Object")

    def testRectGet(self):
        self.assertEqual(self.obj.rect, evas.Rect(10, 20, 30, 40))

    def testRectSet(self):
        self.obj.rect = evas.Rect(1, 2, 3, 4)
        self.assertEqual(self.obj.geometry_get(), (1, 2, 3, 4))


unittest.main()
evas.shutdown()
