#
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.

import os

import evas
import elementary

from fileselector import FileSelector
from floater import Wizard

class FileSelectionWizard(Wizard):
    def __init__(self, parent, selected_cb,\
                 file_add_cb, file_list_cb):

        Wizard.__init__(self, parent)
        self._parent = parent
        self._selected_cb = selected_cb
        self._file_add_cb = file_add_cb
        self._file_list_cb = file_list_cb
        self._selection = ""

        self._parent.block(True)

        #file list - page create and populate
        self._add_file_list_header()

        self._file_list_add()

        self.action_add("file_list", "Close", self.close)
        self.action_add("file_list", "New", self._goto_add_new_file)

        self._file_list.callback_selected_add(self._goto_preview)

        #file selector - page create and populate
        self._add_add_new_file_header()

        self.action_add("add_new_file", "Back", self._back)
        self.action_add("add_new_file", "Add", self._new_file_added)

        self._file_selector_add()

        #file preview - page create and populate
        self._add_file_preview_header()

        self._preview_file = self._preview_method()
        self._get_preview_file(self._selection)
        self._preview_scroller_add()

        self.action_add("file_preview", "Back", self._back)
        self.action_add("file_preview", "Delete", self._delete_file)
        self.action_add("file_preview", "Select", self._file_selected)

        self.goto("file_list")

    def goto(self, page, alt_bg_style=None):
        Wizard.goto(self, page)
        if alt_bg_style is True:
            self.alternate_background_set(True)
        else:
            self.alternate_background_set(False)

    def _goto_add_new_file(self):
        self.goto("add_new_file", alt_bg_style=True)

    def _goto_preview(self, obj=None, data=None):
        if obj:
            self._selection = data.label_get()
        else:
            self._selection = data
        self._get_preview_file(self._selection)
        self.goto("file_preview")

    def _goto_files_list(self):
        self._update()
        self.goto("file_list")

    def _file_list_add(self):
        self._files = []
        self._file_list = elementary.List(self)
        self._file_list.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._file_list.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self._update()
        self._file_list.show()
        self.content_add("file_list", self._file_list)

    def _file_selector_add(self):
        self._fs_filecounter = 0
        self._fs = FileSelector(self)
        self._fs.filter = self._fs_filter
        self._fs.multi = False
        self.action_disabled_set("add_new_file", "Add", True)
        self._fs.callback_add("file.selected", self._fs_file_selected_cb)
        self._fs.callback_add("file.unselected", self._fs_file_unselected_cb)
        self._fs.show()
        self.content_add("add_new_file", self._fs)

    def _preview_scroller_add(self):
        self._preview_scroller = elementary.Scroller(self)
        self._preview_scroller.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        self._preview_scroller.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        self._preview_scroller.policy_set(elementary.ELM_SCROLLER_POLICY_AUTO,
                elementary.ELM_SCROLLER_POLICY_AUTO)
        self._preview_scroller.bounce_set(False, False)
        self._preview_scroller.content_set(self._preview_file)
        self._preview_scroller.show()
        self.content_add("file_preview", self._preview_scroller)

    def _new_file_added(self):
        for i in self._fs.files:
            self._file_add_cb(i)
        file_ = os.path.basename(i)
        self._goto_preview(data=file_)

    def _update(self):
        self._files = self._file_list_cb()

        self._file_list.clear()
        self._files.sort(key=str.lower)

        for i in self._files:
            self._file_list.item_append(i, None, None, None, i)
        self._file_list.go()

    def _fs_file_selected_cb(self, obj, data):
        if data:
            self._fs_filecounter += 1
        self.action_disabled_set("add_new_file", "Add", False)

    def _fs_file_unselected_cb(self, obj, data):
        if data:
            self._fs_filecounter -= 1
        if self._fs_filecounter == 0:
            self.action_disabled_set("add_new_file", "Add", True)

    def _back(self):
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

class ImageSelectionWizard(FileSelectionWizard):

    def __init__(self, parent, selected_cb, file_add_cb,\
                 file_list_cb, img_id_get_cb, workfile_get_cb):

        FileSelectionWizard.__init__(self, parent, selected_cb,\
                file_add_cb, file_list_cb)

        self._img_id_get_cb = img_id_get_cb
        self._workfile_get_cb = workfile_get_cb

    def _add_file_list_header(self):
        self.page_add("file_list", "Select an image",
                      "Select an image in list, or add a new one")

    def _add_add_new_file_header(self):
        self.page_add("add_new_file", "Add a new image",
                      "Choose the images you want to add",
                      separator=True)

    def _add_file_preview_header(self):
        self.page_add("file_preview", "Image Preview",
                      "Delete or select this image.")

    def _fs_filter(self, file):
        return file.endswith(".edb") or \
                file.endswith(".eet") or \
                file.endswith(".gif") or \
                file.endswith(".jpg") or \
                file.endswith(".jpeg") or \
                file.endswith(".pmaps") or \
                file.endswith(".png") or \
                file.endswith(".svg") or \
                file.endswith(".tiff") or \
                file.endswith(".xpm")

    def _preview_method(self):
        return self._parent.evas.FilledImage()

    def _get_preview_file(self, selection):
        if not selection:
            return
        filename = self._workfile_get_cb()
        id = self._img_id_get_cb(selection)
        key = "images/" + str(id)
        self._preview_file.file_set(filename, key)
        self._fix_image_size(self._preview_file)

    def _fix_image_size(self, img):
        w, h = img.image_size_get()
        win_x, win_y, win_w, win_h = self._preview_scroller.region_get()

        if (w < win_w) and (h < win_h):
            img.size_hint_min_set(w, h)
            img.size_hint_max_set(w, h)
        else:
            img.size_hint_min_set(win_w, win_h)
            img.size_hint_max_set(win_w, win_h)
        img.size_hint_align_set(0.5, 0.5)
