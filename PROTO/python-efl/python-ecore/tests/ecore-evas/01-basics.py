import ecore.evas
import unittest

class BasicsTest(unittest.TestCase):
    def testTypeFromName(self):
        self.assertEqual(ecore.evas.engine_type_from_name("software_x11"), 0)

    def testNameFromType(self):
        self.assertEqual(ecore.evas.engine_name_from_type(0), "software_x11")


unittest.main()
ecore.evas.shutdown()
