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

    def test_name(self):
        for part in self.edj.parts:
            self.assertEqual(self.edj.part_get(part).name, part)

    def test_above_get(self):
        # ['main_rect', 'main_text', 'main_image', 'main_swallow',
        # 'main_textblock', 'main_gradient', 'main_group', 'main_box', 'main_table']
        self.assertEqual(self.edj.part_get(self.parts[0]).above_get(), "main_text")
        self.assertEqual(self.edj.part_get(self.parts[-1]).above_get(), None)
        self.assertEqual(self.edj.part_get(self.parts[-2]).above_get(), "main_table")
        self.assertEqual(self.edj.part_get(self.parts[-3]).above_get(), "main_box")
        self.assertEqual(self.edj.part_get(self.parts[-4]).above_get(), "main_group")

    def test_below_get(self):
        self.assertEqual(self.edj.part_get(self.parts[0]).below_get(), None)
        self.assertEqual(self.edj.part_get(self.parts[1]).below_get(), "main_rect")
        self.assertEqual(self.edj.part_get(self.parts[2]).below_get(), "main_text")
        self.assertEqual(self.edj.part_get(self.parts[3]).below_get(), "main_image")
        self.assertEqual(self.edj.part_get(self.parts[-1]).below_get(), "main_box")

    def test_restack_below(self):
        # You can't pre-load parts
        self.assertFalse(self.edj.part_get(self.parts[0]).restack_below())
        self.assertTrue(self.edj.part_get(self.parts[1]).restack_below())
        self.assertFalse(self.edj.part_get(self.parts[1]).restack_below())
        self.assertTrue(self.edj.part_get(self.parts[0]).restack_below())
        self.assertTrue(self.edj.part_get(self.parts[8]).restack_below())
        self.assertTrue(self.edj.part_get(self.parts[8]).restack_below())
        self.assertTrue(self.edj.part_get(self.parts[8]).restack_below())
        self.assertEqual(self.edj.parts, ['main_rect', 'main_text', 'main_image', 'main_swallow', 'main_textblock', 'main_table', 'main_gradient', 'main_group', 'main_box'])

    def test_restack_above(self):
        # You can't pre-load parts
        self.assertFalse(self.edj.part_get(self.parts[8]).restack_above())
        self.assertTrue(self.edj.part_get(self.parts[7]).restack_above())
        self.assertFalse(self.edj.part_get(self.parts[7]).restack_above())
        self.assertTrue(self.edj.part_get(self.parts[8]).restack_above())
        self.assertTrue(self.edj.part_get(self.parts[0]).restack_above())
        self.assertTrue(self.edj.part_get(self.parts[0]).restack_above())
        self.assertTrue(self.edj.part_get(self.parts[0]).restack_above())
        self.assertEqual(self.edj.parts, ['main_text', 'main_image', 'main_swallow', 'main_rect', 'main_textblock', 'main_gradient', 'main_group', 'main_box', 'main_table'])

    def test_rename(self):
        for part_name in self.parts[:-1]:
            part = self.edj.part_get(part_name)
            self.assertFalse(part.rename(self.parts[8]))
            self.assertTrue(part.rename(part_name))
            self.assertTrue(part.rename("New Part"))
            self.assertTrue(part.rename(part_name))

edje.file_cache_set(0)
unittest.main()
edje.shutdown()
ecore.evas.shutdown()
