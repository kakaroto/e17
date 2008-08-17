#!/usr/bin/env python

import ecore
import unittest


class TestBasics(unittest.TestCase):
    def testInit(self):
        count = ecore.shutdown()
        self.assertEqual(count, 0)

unittest.main()
