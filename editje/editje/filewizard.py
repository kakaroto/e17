# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

import os

import evas
import elementary

from fileselector import FileSelector
from floater import Wizard


class FileSelectionWizard(Wizard):
    def __init__(self, parent, selected_cb, file_add_cb):
        Wizard.__init__(self, parent)

        self._selected_cb = selected_cb
        self._file_add_cb = file_add_cb

        self._selection = ""

        self._file_list_page_created = False
        self._add_new_file_page_created = False
        self._file_preview_page_created = False

        self._file_list_add()

        if self._files == []:
            self._create_add_new_file_page()
        else:
            self._create_file_list_page()

    def _create_file_list_page(self):
    #file list - page create and populate
        self._file_list_page_created = True
        self._add_file_list_header()

        self.content_add("file_list", self._file_list)
        self.action_add("file_list", "Cancel", self.close, key="Escape")
        self.action_add(
            "file_list", "Add New", self._goto_add_new_file, key="n")

    def _create_add_new_file_page(self):
    #file selector - page create and populate
        self._add_new_file_page_created = True
        self._add_add_new_file_header()

        self.action_add("add_new_file", "Go To List", self._back, key="Escape")
        self.action_add("add_new_file", "Select", self._new_file_added,
                        key="Return")

        self._file_selector_add()
        self.content_add("add_new_file", self._fs)

    def _create_file_preview_page(self):
    #file preview - page create and populate
        self._file_preview_page_created = True
        self._add_file_preview_header()

        self.action_add("file_preview", "Go To List", self._back, key="Escape")
        self.action_add("file_preview", "Delete", self._delete_file,
                        key="Delete")
        self.action_add("file_preview", "Use", self._file_selected,
                        key="Return")

        self._add_files_to_preview()

    def goto(self, page, alt_bg_style=None):
        Wizard.goto(self, page)

    def _goto_add_new_file(self):
        if not self._add_new_file_page_created:
            self._create_add_new_file_page()
        self.goto("add_new_file", alt_bg_style=True)

    def _goto_preview(self, obj=None, part=None, item_data=None):
        self._selection = item_data

        if not self._file_preview_page_created:
            self._create_file_preview_page()
        else:
            self._get_preview_file(self._selection)

        self.goto("file_preview", alt_bg_style=False)
        self._get_title_text()

    def _goto_files_list(self):
        self._update()
        self.goto("file_list", alt_bg_style=False)

    def _file_list_add(self):
        self._files = []
        self._file_list = elementary.Genlist(self)
        self._file_list.size_hint_align_set(
            evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._file_list.size_hint_weight_set(
            evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self._update()
        self._file_list.show()

    def _file_selector_add(self):
        self._fs_filecounter = 0
        self._fs = FileSelector(self)
        self._fs.filter = self._fs_filter
        self._fs.multi = False
        self.action_disabled_set("add_new_file", "Select", True)
        self._fs.callback_add("file.selected", self._fs_file_selected_cb)
        self._fs.callback_add("file.unselected", self._fs_file_unselected_cb)
        self._fs.show()

    def _preview_scroller_add(self):
        self._preview_scroller = elementary.Scroller(self)
        self._preview_scroller.size_hint_align_set(
            evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._preview_scroller.size_hint_weight_set(
            evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self._preview_scroller.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                                          elementary.ELM_SCROLLER_POLICY_AUTO)
        self._preview_scroller.bounce_set(False, False)
        self._preview_scroller.content_set(self._preview_file)
        self._preview_scroller.show()
        self.content_add("file_preview", self._preview_scroller)

    def _new_file_added(self):
        for i in self._fs.files:
            self._file_add_cb(i)
        new_file = os.path.basename(i)
        self._goto_preview(item_data=new_file)

    def _fs_file_selected_cb(self, obj, data):
        if data:
            self._fs_filecounter += 1
        self.action_disabled_set("add_new_file", "Select", False)

    def _fs_file_unselected_cb(self, obj, data):
        if data:
            self._fs_filecounter -= 1
        if self._fs_filecounter == 0:
            self.action_disabled_set("add_new_file", "Select", True)

    def _back(self):
        if not self._file_list_page_created:
            self._create_file_list_page()
        self._goto_files_list()

    def _back_file_preview(self):
        self._preview_file.hide()
        self._back()

    def _delete_file(self):
        print "FIXME: File deletion to be added soon"
        self.notify("File deletion to be added soon")
        self.goto("file_list")

    def _file_selected(self):
        self._selected_cb(self._selection)
        self.close()

    def _gl_label_get(self, *kargs):
        return kargs[-1]


class ImageSelectionWizard(FileSelectionWizard):
    def __init__(self, parent, selected_cb, file_add_cb, file_list_cb,
                 img_id_get_cb, workfile_name_get_cb):

        self._file_list_cb = file_list_cb
        self._img_id_get_cb = img_id_get_cb
        self._workfile_name_get_cb = workfile_name_get_cb

        FileSelectionWizard.__init__(
            self, parent, selected_cb, file_add_cb)

    def _add_file_list_header(self):
        self.page_add("file_list", "Select an image",
                      "Select an image from the group, or add a new one to it")

    def _add_add_new_file_header(self):
        self.page_add("add_new_file", "Add a new image",
                      "Choose an image to add to the group",
                      separator=True)

    def _add_file_preview_header(self):
        self.page_add("file_preview", "Image Preview",
                      "Delete or select this image")

    def _fs_filter(self, image_file):
        return image_file.endswith(".jpg") or \
                image_file.endswith(".jpeg") or \
                image_file.endswith(".png")

    def _get_title_text(self):
        self.title_text = "Image preview - \"%s\"" % self._selection

    def _update(self):
        itc = elementary.GenlistItemClass(item_style="default",
                                           label_get_func=self._gl_label_get,
                                           icon_get_func=None,
                                           state_get_func=None)

        self._files = self._file_list_cb()

        self._file_list.clear()
        self._files.sort(key=str.lower)

        for i in self._files:
            self._file_list.item_append(itc,
                i.split(',', 1)[0], None, elementary.ELM_GENLIST_ITEM_NONE,
                self._goto_preview)

    def _add_files_to_preview(self):
        self._preview_file = self._preview_method()
        self._preview_scroller_add()
        self._get_preview_file(self._selection)

    def _preview_method(self):
        return evas.FilledImage(self.evas)

    def _get_preview_file(self, selection):
        if not selection:
            return
        filename = self._workfile_name_get_cb()
        image_id = self._img_id_get_cb(selection)
        key = "edje/images/" + str(image_id)
        self._preview_file.file_set(filename, key)
        self._fix_image_size(self._preview_file)

    def _fix_image_size(self, img):
        w, h = img.image_size_get()
        img.size_hint_min_set(w, h)
        img.size_hint_max_set(w, h)
        img.size_hint_align_set(0.5, 0.5)


class FontSelectionWizard(FileSelectionWizard):
    def __init__(self, parent, selected_cb, file_add_cb,\
                 file_list_cb, workfile_name_get_cb):

        self._file_list_cb = file_list_cb
        self._workfile_name_get_cb = workfile_name_get_cb

        FileSelectionWizard.__init__(self, parent, selected_cb,\
                file_add_cb)

    def _add_file_list_header(self):
        self.page_add("file_list", "Select a font",
                      "Select a font in list, or add a new one")

    def _add_add_new_file_header(self):
        self.page_add("add_new_file", "Add a new font",
                      "Choose the fonts you want to add",
                      separator=True)

    def _add_file_preview_header(self):
        self.page_add("file_preview", "Font Preview",
                      "Delete or select this font.")

    def _fs_filter(self, font_file):
        return font_file.endswith(".otf") or \
                font_file.endswith(".ttf")

    def _get_title_text(self):
        self.title_text = "Font preview - \"%s\"" % self._selection

    def _update(self):
        itc = elementary.GenlistItemClass(item_style="default",
                                            label_get_func=self._gl_label_get,
                                            icon_get_func=None,
                                            state_get_func=None)
        self._file_list.clear()
        self._files = []

        for local in [True, False]:
            #if local is True, we'll get fonts from Edje
            #if local is False, we'll get fonts from System
            if local:
                message = "Embedded Fonts"
                self._edje_fonts = self._file_list_cb(local)
                self._files += self._edje_fonts
            else:
                message = "System Fonts"
                self._system_fonts = self._file_list_cb(local)
                self._files += self._system_fonts

            self._file_list.item_append(itc, message, None,
                   elementary.ELM_GENLIST_ITEM_SUBITEMS, self._expand_cb)

    def _expand_cb(self, obj, part, item_data):
        if obj.expanded:
            obj.expanded = False
            obj.subitems_clear()
            obj.selected = False
            return

        obj.expanded = True
        itc = elementary.GenlistItemClass(item_style="default",
                                            label_get_func=self._gl_label_get,
                                            icon_get_func=None,
                                            state_get_func=None)

        if item_data == "Embedded Fonts":
            self._files = self._edje_fonts
        else:
            self._files = self._system_fonts

        self._files.sort(key=str.lower)

        for i in self._files:
            self._file_list.item_append(itc, i.split(',', 1)[0],
                    obj, elementary.ELM_GENLIST_ITEM_NONE,
                    self._goto_preview)
        obj.selected = False

    def _add_files_to_preview(self):
        self._preview_file = self._preview_method()
        self._preview_scroller_add()
        self._get_preview_file(self._selection)

    def _preview_method(self):
        return evas.Text(self.evas)

    def _get_preview_file(self, selection):
        self._preview_file.text_set("%s. Size: 16" % selection)
        self._preview_file.color = (0, 0, 0, 255)
        if selection in self._file_list_cb(True):
            #get font from edje
            self._preview_file.font_source_set(self._workfile_name_get_cb())
            self._preview_file.font_set("edje/fonts/%s" % selection, 16)
            #enable delete button
            self.action_disabled_set("file_preview", "Delete", False)
        else:
            #get font from system
            self._preview_file.font_set(selection, 16)
            #disable delete button
            self.action_disabled_set("file_preview", "Delete", True)
        self._preview_file.size_hint_weight_set(1.0, 1.0)
        self._preview_file.size_hint_align_set(-1.0, -1.0)
        self._preview_file.show()
