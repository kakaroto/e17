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

    def tearDown(self):
        self.edj.delete()
        del self.canvas
        self.canvas = None
        self.edj = None
        import os
        os.remove("test.edj")

    def test_compiler_get(self):
        self.assertEqual(self.edj.compiler_get(), "edje_cc")

    def test_save(self):
        self.assertTrue(self.edj.save())
        self.assertTrue(self.edj.part_del("main_rect"))
        self.assertTrue(self.edj.part_del("main_text"))
        self.assertTrue(self.edj.part_del("main_image"))
        self.assertTrue(self.edj.part_del("main_swallow"))
        self.assertTrue(self.edj.part_del("main_textblock"))
        self.assertTrue(self.edj.part_del("main_gradient"))
        self.assertTrue(self.edj.part_del("main_group"))
        self.assertTrue(self.edj.part_del("main_box"))
        self.assertTrue(self.edj.part_del("main_table"))
        for count in xrange(1000):
            self.assertTrue(self.edj.part_add(str(count), edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.save())

    # Group

    def test_current_group(self):
        self.assertNotEqual(self.edj.current_group, None)

    def test_group_add(self):
        self.assertTrue(self.edj.group_add("new"))
        self.assertFalse(self.edj.group_add("main"))
        self.assertFalse(self.edj.group_add("g1"))
        self.assertFalse(self.edj.group_add("new"))
        new = edje.edit.EdjeEdit(self.canvas.evas, file="test.edj", group="new")
        new.save()
        self.assertTrue("new" in edje.file_collection_list("test.edj"))
        self.assertTrue(self.edj.group_add("new2"))
        self.assertTrue(self.edj.group_add(""))

    def test_group_exist(self):
        self.assertTrue(self.edj.group_exist("main"))
        self.assertTrue(self.edj.group_exist("g2"))
        self.assertFalse(self.edj.group_exist("gNEW"))
        self.assertFalse(self.edj.group_exist(""))
        self.assertTrue(self.edj.group_add("gNEW"))
        self.assertFalse(self.edj.group_exist("gNew"))
        self.assertTrue(self.edj.group_add(""))
        self.assertTrue(self.edj.group_exist("gNEW"))
        self.assertTrue(self.edj.group_exist(""))

    # Data

    def test_data(self):
        pass

    def test_data_add(self):
        pass

    def test_data_del(self):
        pass

    # Text Style

    def test_text_styles(self):
        pass

    def test_text_styles_get(self):
        pass

    def test_text_styles_add(self):
        pass

    def test_text_styles_del(self):
        pass

    # Color Class

    def test_color_classes(self):
        pass

    def text_color_class_get(self):
        pass

    def test_color_class_add(self):
        pass

    def test_color_class_del(self):
        pass

    # Part

    def test_parts(self):
        self.assertEqual(self.edj.parts, ['main_rect', 'main_text', 'main_image', 'main_swallow', 'main_textblock', 'main_gradient', 'main_group', 'main_box', 'main_table'])

    def test_part_get(self):
#        self.assertEqual(self.edj.part_get(""), None)
        self.assertNotEqual(self.edj.part_get("main_rect"), None)
        self.assertEqual(self.edj.part_get("noexist"), None)

    def test_part_add(self):
#        self.assertFalse(self.edj.part_add("fail1", edje.EDJE_PART_TYPE_NONE))
#        self.assertFalse(self.edj.part_add("fail2", edje.EDJE_PART_TYPE_LAST))
#        self.assertFalse(self.edj.part_add("fail3", edje.EDJE_PART_TYPE_LAST + 1))
#        self.assertFalse(self.edj.part_add("", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_add("new_image", edje.EDJE_PART_TYPE_IMAGE))
        self.assertTrue(self.edj.part_add("new_swallow", edje.EDJE_PART_TYPE_SWALLOW))

    def test_part_exist(self):
#        self.assertFalse(self.edj.part_exist(""))
        self.assertFalse(self.edj.part_exist("fail"))
        self.assertTrue(self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_exist("new_rect"))
        self.assertTrue(self.edj.part_exist("main_gradient"))

    def test_part_del(self):
        self.assertFalse(self.edj.part_exist(""))
        self.assertFalse(self.edj.part_exist("fail"))
        self.assertTrue(self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_del("new_rect"))
        self.assertTrue(self.edj.part_del("main_gradient"))

edje.file_cache_set(0)
unittest.main()
edje.shutdown()
ecore.evas.shutdown()
