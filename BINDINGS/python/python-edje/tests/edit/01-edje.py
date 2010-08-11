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
        self.edj = edje.edit.EdjeEdit(
            self.canvas.evas, file="test.edj", group="main")

    def tearDown(self):
        self.edj.delete()
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
        self.assertTrue(self.edj.part_del("main_group"))
        self.assertTrue(self.edj.part_del("main_box"))
        self.assertTrue(self.edj.part_del("main_table"))
        for count in xrange(1000):
            self.assertTrue(
                self.edj.part_add(str(count), edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.save())

        self.edj.delete()
	self.edj = None
        self.edj = edje.edit.EdjeEdit(
            self.canvas.evas, file="test.edj", group="main")

        self.assertFalse(self.edj.part_exist("main_rect"))
        self.assertFalse(self.edj.part_exist("main_text"))
        self.assertFalse(self.edj.part_exist("main_image"))
        self.assertFalse(self.edj.part_exist("main_swallow"))
        self.assertFalse(self.edj.part_exist("main_textblock"))
        self.assertFalse(self.edj.part_exist("main_group"))
        self.assertFalse(self.edj.part_exist("main_box"))
        self.assertFalse(self.edj.part_exist("main_table"))
        for count in xrange(1000):
            self.assertTrue(self.edj.part_exist(str(count)))

    def test_save_all(self):
        self.assertTrue(self.edj.save_all())
        self.assertTrue(self.edj.part_del("main_rect"))
        self.assertTrue(self.edj.part_del("main_text"))
        self.assertTrue(self.edj.part_del("main_image"))
        self.assertTrue(self.edj.part_del("main_swallow"))
        self.assertTrue(self.edj.part_del("main_textblock"))
        self.assertTrue(self.edj.part_del("main_group"))
        self.assertTrue(self.edj.part_del("main_box"))
        self.assertTrue(self.edj.part_del("main_table"))
        other = edje.edit.EdjeEdit(
            self.canvas.evas, file="test.edj", group="g1")
        for count in xrange(1000):
            self.assertTrue(
                other.part_add(str(count), edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(other.save_all())

        self.edj.delete()
        other.delete()
	self.edj = None
	other = None
        self.edj = edje.edit.EdjeEdit(
            self.canvas.evas, file="test.edj", group="main")
        other = edje.edit.EdjeEdit(
            self.canvas.evas, file="test.edj", group="g1")

        self.assertFalse(self.edj.part_exist("main_rect"))
        self.assertFalse(self.edj.part_exist("main_text"))
        self.assertFalse(self.edj.part_exist("main_image"))
        self.assertFalse(self.edj.part_exist("main_swallow"))
        self.assertFalse(self.edj.part_exist("main_textblock"))
        self.assertFalse(self.edj.part_exist("main_group"))
        self.assertFalse(self.edj.part_exist("main_box"))
        self.assertFalse(self.edj.part_exist("main_table"))
        for count in xrange(1000):
            self.assertTrue(other.part_exist(str(count)))

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
	new.delete()
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
        self.assertEqual(self.edj.group_data_get("pref_size"), "10x17")
        self.assertFalse(self.edj.group_data_get("foo"))
        self.assertTrue(self.edj.group_data_add("foo", "bar"))
        self.assertTrue(self.edj.group_data_set("pref_size", "100x100"))
        self.edj.save()
        other = edje.edit.EdjeEdit(self.canvas.evas, file="test.edj", group="main")
        self.assertEqual(other.group_data_get("pref_size"), "100x100")
        self.assertTrue(other.group_data_get("foo"))
        self.assertEqual(other.group_data_get("foo"), "bar")
        other.delete()

    def test_data_add(self):
        self.assertFalse(self.edj.group_data_get("foo"))
        self.assertTrue(self.edj.group_data_add("foo", "bar"))
        self.assertFalse(self.edj.group_data_add("foo", "bar"))
        self.assertEqual(self.edj.group_data_get("foo"), "bar")
        for count in xrange(1000):
            self.assertTrue(self.edj.group_data_add(str(count), str(count)))
            self.assertFalse(self.edj.group_data_add(str(count), str(count)))
            self.assertEqual(self.edj.group_data_get(str(count)), str(count))

    def test_data_del(self):
        self.assertTrue(self.edj.group_data_get("pref_size"))
        self.assertTrue(self.edj.group_data_del("pref_size"))
        self.assertFalse(self.edj.group_data_get("pref_size"))
        self.assertTrue(self.edj.group_data_add("foo", "bar"))
        self.assertEqual(self.edj.group_data_get("foo"), "bar")
        self.assertTrue(self.edj.group_data_del("foo"))
        self.assertFalse(self.edj.group_data_get("get"))


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
        self.assertEqual(self.edj.parts,
                         ['main_rect', 'main_text', 'main_image',
                          'main_swallow', 'main_textblock', 'main_group',
                          'main_box', 'main_table'])

    def test_part_get(self):
#        self.assertEqual(self.edj.part_get(""), None)
        self.assertNotEqual(self.edj.part_get("main_rect"), None)
        self.assertEqual(self.edj.part_get("noexist"), None)

    def test_part_add(self):
#        self.assertFalse(self.edj.part_add("fail1", edje.EDJE_PART_TYPE_NONE))
#        self.assertFalse(self.edj.part_add("fail2", edje.EDJE_PART_TYPE_LAST))
#        self.assertFalse(self.edj.part_add("fail3", edje.EDJE_PART_TYPE_LAST + 1))
#        self.assertFalse(self.edj.part_add("", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(
            self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(
            self.edj.part_add("new_image", edje.EDJE_PART_TYPE_IMAGE))
        self.assertTrue(
            self.edj.part_add("new_swallow", edje.EDJE_PART_TYPE_SWALLOW))

    def test_part_exist(self):
#        self.assertFalse(self.edj.part_add("", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertFalse(self.edj.part_exist(""))
        self.assertFalse(self.edj.part_exist("fail"))
        self.assertTrue(
            self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_exist("new_rect"))

    def test_part_del(self):
        self.assertFalse(self.edj.part_del(""))
        self.assertFalse(self.edj.part_del("fail"))
        self.assertTrue(self.edj.part_del("main_rect"))
        self.assertTrue(self.edj.part_del("main_text"))
        self.assertTrue(self.edj.part_del("main_image"))
        self.assertTrue(
            self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertTrue(self.edj.part_del("new_rect"))
        self.assertTrue(self.edj.part_del("main_swallow"))
        self.assertTrue(self.edj.part_del("main_group"))
        self.assertTrue(self.edj.part_del("main_box"))
        self.assertTrue(self.edj.part_del("main_table"))
        self.assertTrue(self.edj.part_del("main_textblock"))
        self.assertEqual(self.edj.parts, [])
        self.assertTrue(
            self.edj.part_add("new_rect", edje.EDJE_PART_TYPE_RECTANGLE))
        self.assertEqual(self.edj.parts, ["new_rect"])
        self.assertTrue(self.edj.part_del("new_rect"))
        self.assertEqual(self.edj.parts, [])

edje.file_cache_set(0)
suite = unittest.TestLoader().loadTestsFromTestCase(Basics)
unittest.TextTestRunner(verbosity=2).run(suite)
edje.shutdown()
ecore.evas.shutdown()

