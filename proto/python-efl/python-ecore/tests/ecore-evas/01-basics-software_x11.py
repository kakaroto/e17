import ecore.evas
import unittest

class TestSoftwareX11(unittest.TestCase):
    def setUp(self):
        self.ee = ecore.evas.SoftwareX11(w=400, h=200)

    def tearDown(self):
        del self.ee

    def testConstructor(self):
        print self.ee


if ecore.evas.engine_type_supported_get("software_x11"):
    unittest.main()
ecore.evas.shutdown()
