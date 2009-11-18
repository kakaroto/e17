# Copyright (c) 2008-2009 Simon Busch
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
import evas.c_evas
cimport evas.c_evas as c_evas
cimport evas.python
import traceback

def init():
    # Partly from python-etk
    cdef int argc, i, arg_len
    cdef char **argv, *arg
    argc_orig = argc = len(sys.argv)
    argv = <char **>PyMem_Malloc(argc * sizeof(char *))
    for i from 0 <= i < argc:
        arg = sys.argv[i]
        arg_len = len(sys.argv[i])
        argv[i] = <char *>PyMem_Malloc(arg_len + 1)
        memcpy(argv[i], arg, arg_len + 1)

    elm_init(argc, argv)

def shutdown():
    elm_shutdown()

def run():
    elm_run()

def exit():
    elm_exit()

def scale_get():
    return elm_scale_get()

def scale_set(scale):
    elm_scale_set(scale)

def finger_size_get():
    return elm_finger_size_get()

def finger_size_set(size):
    elm_finger_size_set(size)

def coords_finger_size_adjust(times_w, w, times_h, h):
    cdef c_evas.Evas_Coord width
    cdef c_evas.Evas_Coord height
    width = w
    height = h
    elm_coords_finger_size_adjust(times_w, &width, times_h, &height)

def theme_overlay_add(item):
    elm_theme_overlay_add(item)

def theme_extension_add(item):
    elm_theme_extension_add(item)




include "elementary.c_elementary_object.pxi"
include "elementary.c_elementary_window.pxi"
include "elementary.c_elementary_innerwindow.pxi"
include "elementary.c_elementary_background.pxi"
include "elementary.c_elementary_icon.pxi"
include "elementary.c_elementary_box.pxi"
include "elementary.c_elementary_frame.pxi"
include "elementary.c_elementary_button.pxi"
include "elementary.c_elementary_scroller.pxi"
include "elementary.c_elementary_label.pxi"
include "elementary.c_elementary_toggle.pxi"
include "elementary.c_elementary_table.pxi"
include "elementary.c_elementary_clock.pxi"
include "elementary.c_elementary_layout.pxi"
include "elementary.c_elementary_hover.pxi"
include "elementary.c_elementary_entry.pxi"
include "elementary.c_elementary_anchorview.pxi"
include "elementary.c_elementary_anchorblock.pxi"
include "elementary.c_elementary_bubble.pxi"
include "elementary.c_elementary_photo.pxi"
include "elementary.c_elementary_hoversel.pxi"
include "elementary.c_elementary_toolbar.pxi"
include "elementary.c_elementary_list.pxi"
include "elementary.c_elementary_slider.pxi"
include "elementary.c_elementary_pager.pxi"
include "elementary.c_elementary_radio.pxi"
include "elementary.c_elementary_check.pxi"
include "elementary.c_elementary_image.pxi"
include "elementary.c_elementary_notepad.pxi"
#include "elementary.c_elementary_genlist.pxi"
include "elementary.c_elementary_spinner.pxi"
include "elementary.c_elementary_notify.pxi"
include "elementary.c_elementary_fileselector.pxi"
include "elementary.c_elementary_separator.pxi"
include "elementary.c_elementary_progressbar.pxi"
