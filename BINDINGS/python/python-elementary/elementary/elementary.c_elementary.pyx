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

flag = False    

cdef object _callback_mappings
_callback_mappings = dict()

cdef void _object_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _callback_mappings.get(<long>obj,None)
        if mapping is not None:
            func = mapping.get(<char*>data,None)
        
            if not callable(func):
                raise TypeError("func is not callable")
        
            func(mapping["__class__"],<char*>data)
    except Exception, e:
        traceback.print_exc()


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
