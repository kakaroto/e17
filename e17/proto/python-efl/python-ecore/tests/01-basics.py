#!/usr/bin/env python

import ecore
import unittest


class TestBasics(unittest.TestCase):
    def testInit(self):
        count = ecore.init()
        self.assertEqual(count, 1)
        count = ecore.shutdown()
        self.assertEqual(count, 0)

unittest.main()
