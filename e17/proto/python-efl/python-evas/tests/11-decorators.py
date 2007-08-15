import evas
import evas.decorators
import unittest

class DecoratorsTests(unittest.TestCase):
    def setUp(self):
        self.canvas = evas.Canvas(method="software_x11",
                                  size=(400, 500),
                                  viewport=(0, 0, 400, 500))

    def tearDown(self):
        del self.canvas

    def testGeneric(self):
        class R(evas.Rectangle):
            @evas.decorators.event_callback(evas.EVAS_CALLBACK_SHOW)
            def on_show(self):
                self.ok = True

        r = R(self.canvas)
        r.show()
        self.assertEqual(r.ok, True)

    def testSpecific(self):
        class R(evas.Rectangle):
            @evas.decorators.show_callback
            def on_show(self):
                self.ok = True

        r = R(self.canvas)
        r.show()
        self.assertEqual(r.ok, True)


unittest.main()
evas.shutdown()
