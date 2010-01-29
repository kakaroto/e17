#!/usr/bin/env python

import edje
import edje.edit
import ecore.evas
import unittest

class Basics(unittest.TestCase):
    def setUp(self):
        import shutil
        shutil.copyfile("theme.edj","test.edj")
        self.canvas = ecore.evas.Buffer(300, 300)
        self.edj = edje.edit.EdjeEdit(self.canvas.evas, file="test.edj", group="main")
        self.group = self.edj.current_group

    def tearDown(self):
        self.edj.delete()
        del self.canvas
        self.canvas = None
        self.edj = None
        self.group = None
        import os
        os.remove("test.edj")

    def test_rename(self):
        self.assertFalse(self.group.rename("g1"))
        self.assertTrue(self.group.rename("main"))
        self.assertTrue(self.group.rename("gNewName"))
        self.edj.save()
        self.assertTrue("gNewName" in edje.file_collection_list("test.edj"))

    def test_w_min(self):
        self.assertEqual(self.group.w_min, 123)
        self.group.w_min = 555
        self.assertEqual(self.group.w_min, 555)

    def test_w_max(self):
        self.assertEqual(self.group.w_max, 789)
        self.group.w_max = 888
        self.assertEqual(self.group.w_max, 888)

    def test_h_min(self):
        self.assertEqual(self.group.h_min, 456)
        self.group.h_min = 600
        self.assertEqual(self.group.h_min, 600)

    def test_h_max(self):
        self.assertEqual(self.group.h_max, 999)
        self.group.h_max = 1111
        self.assertEqual(self.group.h_max, 1111)

    def test_delete(self):
        self.assertTrue(self.edj.group_exist("g1"))
        self.assertTrue(self.edj.group_del("g1"))
        self.assertFalse(self.edj.group_exist("g1"))

edje.file_cache_set(0)
unittest.main()
edje.shutdown()
ecore.evas.shutdown()
