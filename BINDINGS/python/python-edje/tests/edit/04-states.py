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
        self.parts = self.edj.parts

    def tearDown(self):
        self.edj.delete()
        del self.canvas
        self.canvas = None
        self.edj = None
        self.group = None
        import os
        os.remove("test.edj")

    def test_state_add(self):
        for p in self.parts:
            part = self.edj.part_get(p)
            self.assertFalse(part.state_exist("test_state_add", 0.0))
            self.assertTrue(part.state_add("test_state_add", 0.0))
            self.assertTrue(part.state_exist("test_state_add", 0.0))
            self.assertFalse(part.state_add("test_state_add", 0.0))
            self.assertTrue(part.state_exist("default", 0.0))
            self.assertFalse(part.state_add("default", 0.0))

    def test_state_copy(self):
        for p in self.parts:
            part = self.edj.part_get(p)
            self.assertTrue(part.state_exist("default", 0.0))
            self.assertFalse(part.state_exist("new_state", 0))
            self.assertTrue(part.state_copy("default", 0.0, "new_state", 0))
            self.assertTrue(part.state_exist("new_state", 0.0))
            self.assertFalse(part.state_exist("another_new_state", 0.0))
            self.assertTrue(
                    part.state_copy("new_state", 0, "another_new_state", 0))
            self.assertTrue(part.state_exist("another_new_state", 0.0))
            self.assertTrue(
                    part.state_copy("another_new_state", 0.0, "default", 0))
            self.assertFalse(part.state_exist("non-existing", 0.0))
            self.assertFalse(
                    part.state_copy("non-existing", 0.0, "non-existing2", 0))
            self.assertFalse(part.state_exist("non-existing2", 0.0))

    def test_state_del(self):
        for p in self.parts:
            part = self.edj.part_get(p)
            self.assertTrue(part.state_add("test_state_del", 0.0))
            self.assertTrue(part.state_exist("test_state_del", 0.0))
            self.assertTrue(part.state_del("test_state_del", 0))
            self.assertFalse(part.state_exist("test_state_del", 0.0))
            self.assertFalse(part.state_del("test_state_del", 0.0))
            #we can't del "default" 0.0 state
            self.assertTrue(part.state_exist("default", 0.0))
            self.assertFalse(part.state_del("default", 0.0))
            self.assertTrue(part.state_exist("default", 0.0))


edje.file_cache_set(0)
suite = unittest.TestLoader().loadTestsFromTestCase(Basics)
unittest.TextTestRunner(verbosity=2).run(suite)
edje.shutdown()
ecore.evas.shutdown()
