import evas
import unittest

class RectBasics(unittest.TestCase):
    def _test_values(self, r):
        self.assertEqual(r.x, 1)
        self.assertEqual(r.y, 2)
        self.assertEqual(r.w, 3)
        self.assertEqual(r.h, 4)

    def testConstructorList(self):
        r = evas.Rect(1, 2, 3, 4)
        self._test_values(r)

    def testConstructorTuple(self):
        r = evas.Rect((1, 2, 3, 4))
        self._test_values(r)

    def testConstructorRect(self):
        r = evas.Rect(evas.Rect(1, 2, 3, 4))
        self._test_values(r)

    def testConstructorKeywordsExplicit1(self):
        r = evas.Rect(x=1, y=2, w=3, h=4)
        self._test_values(r)

    def testConstructorKeywordsExplicit2(self):
        r = evas.Rect(left=1, top=2, width=3, height=4)
        self._test_values(r)

    def testConstructorKeywordsExplicit3(self):
        r = evas.Rect(left=1, right=4, top=2, bottom=6)
        self._test_values(r)

    def testConstructorKeywordsExplicit4(self):
        r = evas.Rect(right=4, bottom=6, width=3, height=4)
        self._test_values(r)

    def testConstructorKeywordsPosSize(self):
        r = evas.Rect(pos=(1, 2), size=(3, 4))
        self._test_values(r)

    def testConstructorKeywordsGeometry(self):
        r = evas.Rect(geometry=(1, 2, 3, 4))
        self._test_values(r)

    def testConstructorKeywordsRect(self):
        r = evas.Rect(rect=evas.Rect(1, 2, 3, 4))
        self._test_values(r)


class RectOperations(unittest.TestCase):
    def setUp(self):
        self.r = evas.Rect(10, 20, 30, 40)

    def testContains(self):
        r2 = evas.Rect(11, 21, 5, 5)
        self.assertEqual(r2 in self.r, True)

    def testContainsExactly(self):
        r2 = evas.Rect(self.r)
        self.assertEqual(r2 in self.r, True)

    def testContainsLarger(self):
        r2 = evas.Rect(0, 0, 100, 100)
        self.assertEqual(r2 in self.r, False)

    def testContainsOverlap1(self):
        r2 = evas.Rect(0, 0, 20, 30)
        self.assertEqual(r2 in self.r, False)

    def testContainsOverlap2(self):
        r2 = evas.Rect(15, 25, 50, 50)
        self.assertEqual(r2 in self.r, False)

    def testContainsTuple(self):
        self.assertEqual((11, 21, 5, 5) in self.r, True)

    def testContainsTupleLarger(self):
        self.assertEqual((0, 0, 100, 100) in self.r, False)

    def testContainsPoint(self):
        self.assertEqual((15, 25) in self.r, True)

    def testContainsPointNo(self):
        self.assertEqual((0, 0) in self.r, False)

    def testEqual(self):
        self.assertEqual(self.r == self.r, True)

    def testEqualOther(self):
        self.assertEqual(self.r == evas.Rect(self.r), True)

    def testEqualTuple(self):
        self.assertEqual(self.r == (10, 20, 30, 40), True)

    def testEqualOtherNo(self):
        self.assertEqual(self.r == evas.Rect(1, 2, 3, 4), False)

    def testEqualTupleNo(self):
        self.assertEqual(self.r == (1, 2, 3, 4), False)

    def testNotEqual(self):
        self.assertEqual(self.r != self.r, False)

    def testNotEqualOther(self):
        self.assertEqual(self.r != evas.Rect(self.r), False)

    def testNotEqualTuple(self):
        self.assertEqual(self.r != (10, 20, 30, 40), False)

    def testNotEqualOtherNo(self):
        self.assertEqual(self.r != evas.Rect(1, 2, 3, 4), True)

    def testNotEqualTupleNo(self):
        self.assertEqual(self.r != (1, 2, 3, 4), True)

    def testNormalize(self):
        r2 = evas.Rect(left=10, right=0, top=20, bottom=0)
        self.assertEqual(r2.left, 10)
        self.assertEqual(r2.right, 0)
        self.assertEqual(r2.top, 20)
        self.assertEqual(r2.bottom, 0)
        self.assertEqual(r2.w, -10)
        self.assertEqual(r2.h, -20)
        r2.normalize()
        self.assertEqual(r2.left, 0)
        self.assertEqual(r2.right, 10)
        self.assertEqual(r2.top, 0)
        self.assertEqual(r2.bottom, 20)
        self.assertEqual(r2.w, 10)
        self.assertEqual(r2.h, 20)

    def testNonZero(self):
        self.assertEqual(bool(self.r), True)

    def testNonZero2(self):
        self.assertEqual(bool(evas.Rect(0, 0, 0, 0)), False)


class RectIntercepts(unittest.TestCase):
    def setUp(self):
        self.r = evas.Rect(0, 0, 10, 10)

    def testOver(self):
        r2 = evas.Rect(self.r)
        self.assert_(self.r.intercepts(r2))

    def testInterceptTopLeft(self):
        r2 = evas.Rect(-5, -5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptTopRight(self):
        r2 = evas.Rect(5, -5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptBottomLeft(self):
        r2 = evas.Rect(-5, 5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptBottomRight(self):
        r2 = evas.Rect(5, 5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptLeft1(self):
        r2 = evas.Rect(-5, 0, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptLeft2(self):
        r2 = evas.Rect(-5, 2, 10, 5)
        self.assert_(self.r.intercepts(r2))

    def testInterceptLeft3(self):
        r2 = evas.Rect(-5, 5, 30, 5)
        self.assert_(self.r.intercepts(r2))

    def testInterceptRight1(self):
        r2 = evas.Rect(5, 0, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptRight2(self):
        r2 = evas.Rect(5, 2, 10, 5)
        self.assert_(self.r.intercepts(r2))

    def testInterceptRight3(self):
        r2 = evas.Rect(5, 5, 30, 5)
        self.assert_(self.r.intercepts(r2))

    def testInterceptTop1(self):
        r2 = evas.Rect(0, -5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptTop2(self):
        r2 = evas.Rect(2, -5, 5, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptTop3(self):
        r2 = evas.Rect(5, -5, 5, 30)
        self.assert_(self.r.intercepts(r2))

    def testInterceptBottom1(self):
        r2 = evas.Rect(0, 5, 10, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptBottom2(self):
        r2 = evas.Rect(2, 5, 5, 10)
        self.assert_(self.r.intercepts(r2))

    def testInterceptBottom3(self):
        r2 = evas.Rect(5, 5, 5, 30)
        self.assert_(self.r.intercepts(r2))

    def testNoIntercept1(self):
        r2 = evas.Rect(-10, -10, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept2(self):
        r2 = evas.Rect(0, -10, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept3(self):
        r2 = evas.Rect(10, -10, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept4(self):
        r2 = evas.Rect(-10, -10, 30, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept5(self):
        r2 = evas.Rect(-10, 15, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept6(self):
        r2 = evas.Rect(0, 15, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept7(self):
        r2 = evas.Rect(10, 15, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept8(self):
        r2 = evas.Rect(-10, 15, 30, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept9(self):
        r2 = evas.Rect(-10, 5, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept10(self):
        r2 = evas.Rect(15, 5, 5, 5)
        self.assert_(not self.r.intercepts(r2))

    def testNoIntercept11(self):
        r2 = evas.Rect(15, 15, 5, 5)
        self.assert_(not self.r.intercepts(r2))



class RectClip(unittest.TestCase):
    def setUp(self):
        self.r = evas.Rect(0, 0, 10, 10)

    def testClipContained(self):
        r2 = evas.Rect(-10, -10, 30, 30)
        self.assertEqual(self.r.clip(r2), self.r)

    def testClipContains(self):
        r2 = evas.Rect(5, 5, 2, 2)
        self.assertEqual(self.r.clip(r2), r2)

    def testClipOutside1(self):
        r2 = evas.Rect(-10, -10, 5, 5)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 0, 0, 0))

    def testClipOutside2(self):
        r2 = evas.Rect(15, 15, 5, 5)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 0, 0, 0))

    def testClipOutside3(self):
        r2 = evas.Rect(-10, -10, 30, 5)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 0, 0, 0))

    def testInterceptTopLeft(self):
        r2 = evas.Rect(-5, -5, 10, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 0, 5, 5))

    def testInterceptTopRight(self):
        r2 = evas.Rect(5, -5, 10, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(5, 0, 5, 5))

    def testInterceptBottomLeft(self):
        r2 = evas.Rect(-5, 5, 10, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 5, 5, 5))

    def testInterceptBottomRight(self):
        r2 = evas.Rect(5, 5, 10, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(5, 5, 5, 5))

    def testInterceptLeft(self):
        r2 = evas.Rect(-5, 2, 10, 5)
        self.assertEqual(self.r.clip(r2), evas.Rect(0, 2, 5, 5))

    def testInterceptRight(self):
        r2 = evas.Rect(5, 2, 10, 5)
        self.assertEqual(self.r.clip(r2), evas.Rect(5, 2, 5, 5))

    def testInterceptTop(self):
        r2 = evas.Rect(2, -5, 5, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(2, 0, 5, 5))

    def testInterceptBottom(self):
        r2 = evas.Rect(2, 5, 5, 10)
        self.assertEqual(self.r.clip(r2), evas.Rect(2, 5, 5, 5))


class RectUnion(unittest.TestCase):
    def setUp(self):
        self.r = evas.Rect(0, 0, 10, 10)

    def testUnionContained(self):
        r2 = evas.Rect(-10, -10, 30, 30)
        self.assertEqual(self.r.union(r2), r2)

    def testUnionContains(self):
        r2 = evas.Rect(5, 5, 2, 2)
        self.assertEqual(self.r.union(r2), self.r)

    def testUnion(self):
        r2 = evas.Rect(-10, -10, 5, 5)
        self.assertEqual(self.r.union(r2), evas.Rect(-10, -10, 20, 20))


class RectClamp(unittest.TestCase):
    def setUp(self):
        self.r = evas.Rect(0, 0, 10, 10)

    def testClampContained(self):
        r2 = evas.Rect(-10, -10, 30, 30)
        self.assertEqual(self.r.clamp(r2), self.r)

    def testClampContains(self):
        r2 = evas.Rect(0, 0, 4, 4)
        # -3 = 4/2 - 5/2 (centered)
        self.assertEqual(self.r.clamp(r2), evas.Rect(-3, -3, 10, 10))

    def testClampTopLeft(self):
        r2 = evas.Rect(5, 5, 20, 20)
        self.assertEqual(self.r.clamp(r2), evas.Rect(5, 5, 10, 10))

    def testClampTopRight(self):
        r2 = evas.Rect(-15, 5, 20, 20)
        self.assertEqual(self.r.clamp(r2), evas.Rect(-5, 5, 10, 10))

    def testClampBottomLeft(self):
        r2 = evas.Rect(5, -15, 20, 20)
        self.assertEqual(self.r.clamp(r2), evas.Rect(5, -5, 10, 10))

    def testClampBottomRight(self):
        r2 = evas.Rect(-15, -15, 20, 20)
        self.assertEqual(self.r.clamp(r2), evas.Rect(-5, -5, 10, 10))


unittest.main()
evas.shutdown()
