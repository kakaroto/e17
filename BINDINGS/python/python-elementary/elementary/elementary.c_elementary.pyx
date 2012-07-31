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

"""
What is Elementary?
===================

This is a VERY SIMPLE toolkit. It is not meant for writing extensive desktop
applications (yet). Small simple ones with simple needs.

It is meant to make the programmers work almost brainless but give them lots
of flexibility.

To write an Elementary app, you can get started with the following::

    from elementary import *

    # create window(s) here and do any application init
    run() # run main loop
    shutdown() # after mainloop finishes running, shutdown


There is a more advanced way of making use of the quicklaunch infrastructure
in Elementary (which will not be covered here due to its more advanced
nature).

Now let's actually create an interactive "Hello World" gui that you can
click the ok button to exit. It's more code because this now does something
much more significant, but it's still very simple::

    from elementary import *

    def on_done(obj):
        # quit the mainloop
        exit()

    class Spam:
        def __init__(self):
            # new window - do the usual and give it a name (hello) and title (Hello)
            win = StandardWindow("hello", "Hello")
            # when the user clicks "close" on a window there is a request to delete
            win.callback_delete_request_add(on_done)

            # add a box object - default is vertical. a box holds children in a row,
            # either horizontally or vertically. nothing more.
            box = Box(win)
            # make the box horizontal
            box.horizontal = True
            # add object as a resize object for the window (controls window minimum
            # size as well as gets resized if window is resized)
            win.resize_object_add(box)
            box.show()

            # add a label widget, set the text and put it in the pad frame
            lab = Label(win)
            # set default text of the label
            lab.text_set("Hello out there world!")
            # pack the label at the end of the box
            box.pack_end(lab)
            lab.show()

            # add an ok button
            btn = Button(win)
            # set default text of button to "OK"
            btn.text_set("OK")
            # pack the button at the end of the box
            box.pack_end(btn)
            btn.show()
            # call on_done when button is clicked
            btn.callback_clicked_add(on_done)

            # now we are done, show the window
            win.show()

    if __name__ == "__main__":
        food = Spam()
        # run the mainloop and process events and callbacks
        run()
        shutdown()


Acknowledgements
================

There is a lot that goes into making a widget set, and they don't happen out
of nothing. It's like trying to make everyone everywhere happy, regardless
of age, gender, race or nationality - and that is really tough. So thanks to
people and organizations behind this, as listed below:

@copyright: Elementary is Copyright (C) 2008-2012 Carsten Haitzler and various contributors (see AUTHORS)

@license: Elementary is licensed LGPL-2.1 (see its COPYING file), the python bindings LGPL-3

@author: U{Carsten Haitzler<mailto:raster@rasterman.com>}
@author: U{Gustavo Sverzut Barbieri<mailto:barbieri@profusion.mobi>}
@author: U{Cedric Bail<mailto:cedric.bail@free.fr>}
@author: U{Vincent Torri<mailto:vtorri@univ-evry.fr>}
@author: U{Daniel Kolesa<mailto:quaker66@gmail.com>}
@author: U{Jaime Thomas<mailto:avi.thomas@gmail.com>}
@author: U{Swisscom<http://www.swisscom.ch/>}
@author: U{Christopher Michael<mailto:devilhorns@comcast.net>}
@author: U{Marco Trevisan (Treviño)<mailto:mail@3v1n0.net>}
@author: U{Michael Bouchaud<mailto:michael.bouchaud@gmail.com>}
@author: U{Jonathan Atton (Watchwolf)<mailto:jonathan.atton@gmail.com>}
@author: U{Brian Wang<mailto:brian.wang.0721@gmail.com>}
@author: U{Mike Blumenkrantz (discomfitor/zmike)<mailto:michael.blumenkrantz@gmail.com>}
@author: Samsung Electronics tbd
@author: Samsung SAIT tbd
@author: U{Brett Nash<mailto:nash@nash.id.au>}
@author: U{Bruno Dilly<mailto:bdilly@profusion.mobi>}
@author: U{Rafael Fonseca<mailto:rfonseca@profusion.mobi>}
@author: U{Chuneon Park<mailto:hermet@hermet.pe.kr>}
@author: U{Woohyun Jung<mailto:wh0705.jung@samsung.com>}
@author: U{Jaehwan Kim<mailto:jae.hwan.kim@samsung.com>}
@author: U{Wonguk Jeong<mailto:wonguk.jeong@samsung.com>}
@author: U{Leandro A. F. Pereira<mailto:leandro@profusion.mobi>}
@author: U{Helen Fornazier<mailto:helen.fornazier@profusion.mobi>}
@author: U{Gustavo Lima Chaves<mailto:glima@profusion.mobi>}
@author: U{Fabiano Fidêncio<mailto:fidencio@profusion.mobi>}
@author: U{Tiago Falcão<mailto:tiago@profusion.mobi>}
@author: U{Otavio Pontes<mailto:otavio@profusion.mobi>}
@author: U{Viktor Kojouharov<mailto:vkojouharov@gmail.com>}
@author: U{Daniel Juyung Seo (SeoZ)<mailto:juyung.seo@samsung.com>} (U{alt<mailto:seojuyung2@gmail.com>})
@author: U{Sangho Park <mailto:sangho.g.park@samsung.com>} (U{alt<mailto:gouache95@gmail.com>})
@author: U{Rajeev Ranjan (Rajeev) <mailto:rajeev.r@samsung.com>} (U{alt<mailto:rajeev.jnnce@gmail.com>})
@author: U{Seunggyun Kim <mailto:sgyun.kim@samsung.com>} (U{alt<mailto:tmdrbs@gmail.com>})
@author: U{Sohyun Kim <mailto:anna1014.kim@samsung.com>} (U{alt<mailto:sohyun.anna@gmail.com>})
@author: U{Jihoon Kim<mailto:jihoon48.kim@samsung.com>}
@author: U{Jeonghyun Yun (arosis)<mailto:jh0506.yun@samsung.com>}
@author: U{Tom Hacohen<mailto:tom@stosb.com>}
@author: U{Aharon Hillel<mailto:a.hillel@samsung.com>}
@author: U{Jonathan Atton (Watchwolf)<mailto:jonathan.atton@gmail.com>}
@author: U{Shinwoo Kim<mailto:kimcinoo@gmail.com>}
@author: U{Govindaraju SM <mailto:govi.sm@samsung.com>} (U{alt<mailto:govism@gmail.com>})
@author: U{Prince Kumar Dubey <mailto:prince.dubey@samsung.com>} (U{alt<mailto:prince.dubey@gmail.com>})
@author: U{Sung W. Park<mailto:sungwoo@gmail.com>}
@author: U{Thierry el Borgi<mailto:thierry@substantiel.fr>}
@author: U{Shilpa Singh <mailto:shilpa.singh@samsung.com>} (U{alt<mailto:shilpasingh.o@gmail.com>})
@author: U{Chanwook Jung<mailto:joey.jung@samsung.com>}
@author: U{Hyoyoung Chang<mailto:hyoyoung.chang@samsung.com>}
@author: U{Guillaume "Kuri" Friloux<mailto:guillaume.friloux@asp64.com>}
@author: U{Kim Yunhan<mailto:spbear@gmail.com>}
@author: U{Bluezery<mailto:ohpowel@gmail.com>}
@author: U{Nicolas Aguirre<mailto:aguirre.nicolas@gmail.com>}
@author: U{Sanjeev BA<mailto:iamsanjeev@gmail.com>}
@author: U{Hyunsil Park<mailto:hyunsil.park@samsung.com>}
@author: U{Goun Lee<mailto:gouni.lee@samsung.com>}
@author: U{Mikael Sans<mailto:sans.mikael@gmail.com>}
@author: U{Doyoun Kang<mailto:doyoun.kang@samsung.com>}
@author: U{M.V.K. Sumanth <mailto:sumanth.m@samsung.com>} (U{alt<mailto:mvksumanth@gmail.com>})
@author: U{Jérôme Pinot<mailto:ngc891@gmail.com>}
@author: U{Davide Andreoli (davemds)<mailto:dave@gurumeditation.it>}
@author: U{Michal Pakula vel Rutka<mailto:m.pakula@samsung.com>}

@contact: U{Enlightenment developer mailing list<mailto:enlightenment-devel@lists.sourceforge.net>}

@group Infrastructure: Canvas, Configuration, ElementaryObjectMeta,
    EntryAnchor*, Object, Theme

@group Widgets: Actionslider, Background, Box, Bubble, Button, Calendar,
    Check, Clock, Colorselector, Conformant, Ctxpopup, Dayselector,
    Diskselector, Entry, Fileselector*, Flip, Frame, Gengrid, Genlist, Grid,
    Hover, Hoversel, Icon, Image, Index, InnerWindow, Label, Layout, List,
    Map, Mapbuf, Menu, MultiButtonEntry, Naviframe, Notify, Panel, Panes,
    Photo, Photocam, Player, Plug, Popup, Progressbar, Radio, Scroller,
    SegmentControl, Separator, Slider, Slideshow, Spinner, StandardWindow,
    Table, Thumb, Toolbar, Transit, Video, Web, Window

@group Widgets' items: *Item, *ItemClass, CalendarMark, MapName,
    MapOverlay*, MapRoute

"""

from cpython cimport PyObject, Py_INCREF, Py_DECREF
from cpython cimport PyMem_Malloc, PyMem_Free
from cpython cimport bool
#cimport evas.c_evas as c_evas
from evas.c_evas cimport Object_from_instance

import sys
#import evas.c_evas
from evas.c_evas import _object_mapping_register
from evas.c_evas import _object_mapping_unregister
import traceback
import logging

logging.basicConfig(level=logging.DEBUG)
log = logging.getLogger("elementary")

cdef int PY_REFCOUNT(object o):
    cdef PyObject *obj = <PyObject *>o
    return obj.ob_refcnt

cdef _METHOD_DEPRECATED(self, replacement=None, message=None):
    stack = traceback.extract_stack()
    caller = stack[-1]
    caller_module, caller_line, caller_name, caller_code = caller
    if caller_code:
        msg = "%s:%s %s (class %s) is deprecated." % \
            (caller_module, caller_line, caller_code,
            self.__class__.__name__ if self else 'None')
    else:
        msg = "%s:%s %s.%s() is deprecated." % \
            (caller_module, caller_line,
            self.__class__.__name__ if self else 'None', caller_name)
    if replacement:
        msg += " Use %s() instead." % (replacement,)
    if message:
        msg += " " + message
    log.warn(msg)

cdef inline unicode _touni(char* s):
    return s.decode('UTF-8', 'strict')

cdef inline unicode _ctouni(const_char_ptr s):
    return s.decode('UTF-8', 'strict')

cdef inline char* _fruni(s):
    cdef char* c_string
    if isinstance(s, unicode):
        string = s.encode('UTF-8')
        c_string = string
    elif isinstance(s, str):
        c_string = s
    elif s is None:
        return NULL
    else:
        raise TypeError("Expected str or unicode object, got %s" % (type(s).__name__))
    return c_string

cdef inline const_char_ptr _cfruni(s):
    cdef const_char_ptr c_string
    if isinstance(s, unicode):
        string = s.encode('UTF-8')
        c_string = string
    elif isinstance(s, str):
        c_string = s
    elif s is None:
        return NULL
    else:
        raise TypeError("Expected str or unicode object, got %s" % (type(s).__name__))
    return c_string

def init():
    cdef int argc, i, arg_len
    cdef char **argv, *arg
    argc = len(sys.argv)
    argv = <char **>PyMem_Malloc(argc * sizeof(char *))
    for i from 0 <= i < argc:
        arg = _fruni(sys.argv[i])
        arg_len = len(arg)
        argv[i] = <char *>PyMem_Malloc(arg_len + 1)
        memcpy(argv[i], arg, arg_len + 1)

    elm_init(argc, argv)

def shutdown():
    elm_shutdown()

def run():
    with nogil:
        elm_run()

def exit():
    elm_exit()

def policy_set(policy, value):
    """Set new policy value.

    This will emit the ecore event ELM_EVENT_POLICY_CHANGED in the main
    loop giving the event information Elm_Event_Policy_Changed with
    policy identifier, new and old values.

    @param policy: policy identifier as in Elm_Policy.
    @param value: policy value, depends on identifiers, usually there is
        an enumeration with the same prefix as the policy name, for
        example: ELM_POLICY_QUIT and Elm_Policy_Quit
        (ELM_POLICY_QUIT_NONE, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED).

    @return: True on success or False on error (right
        now just invalid policy identifier, but in future policy
        value might be enforced).

    """
    return elm_policy_set(policy, value)

def policy_get(policy):
    """Gets the policy value set for given identifier.

    @param policy: policy identifier as in Elm_Policy.

    @return: policy value. Will be 0 if policy identifier is invalid.

    """
    return elm_policy_get(policy)

def coords_finger_size_adjust(times_w, w, times_h, h):
    cdef Evas_Coord width
    cdef Evas_Coord height
    width = w
    height = h
    elm_coords_finger_size_adjust(times_w, &width, times_h, &height)

cdef object _elm_widget_type_mapping

_elm_widget_type_mapping = {}

cdef _elm_widget_type_register(name, cls):
    if name in _elm_widget_type_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    _elm_widget_type_mapping[name] = cls
    # TODO: Widget types become evas object types as they are being ported
    # to the new model. The class resolver can be removed when it's done.
    _object_mapping_register("elm_"+name, cls)

cdef _elm_widget_type_unregister(name):
    _elm_widget_type_mapping.pop(name)
    # TODO: Widget types become evas object types as they are being ported
    # to the new model. The class resolver can be removed when it's done.
    _object_mapping_unregister("elm_"+name)

cdef extern from "Python.h":
    ctypedef struct PyTypeObject:
        PyTypeObject *ob_type

cdef void _install_metaclass(PyTypeObject *ctype, object metaclass):
    Py_INCREF(metaclass)
    ctype.ob_type = <PyTypeObject*>metaclass

class ElementaryObjectMeta(type):
    def __init__(cls, name, bases, dict_):
        type.__init__(cls, name, bases, dict_)
        cls._fetch_evt_callbacks()

    def _fetch_evt_callbacks(cls):
        if "__evas_event_callbacks__" in cls.__dict__:
            return

        cls.__evas_event_callbacks__ = []
        append = cls.__evas_event_callbacks__.append

        for name in dir(cls):
            val = getattr(cls, name)
            if not callable(val) or not hasattr(val, "evas_event_callback"):
                continue
            evt = getattr(val, "evas_event_callback")
            append((name, evt))

include "elementary.c_elementary_configuration.pxi"
include "elementary.c_elementary_need.pxi"
include "elementary.c_elementary_theme.pxi"
include "elementary.c_elementary_object.pxi"
include "elementary.c_elementary_object_item.pxi"
include "elementary.c_elementary_gesture_layer.pxi"
include "elementary.c_elementary_layout_class.pxi"
include "elementary.c_elementary_layout.pxi"
include "elementary.c_elementary_image.pxi"
include "elementary.c_elementary_button.pxi"
include "elementary.c_elementary_window.pxi"
include "elementary.c_elementary_innerwindow.pxi"
include "elementary.c_elementary_background.pxi"
include "elementary.c_elementary_icon.pxi"
include "elementary.c_elementary_box.pxi"
include "elementary.c_elementary_frame.pxi"
include "elementary.c_elementary_flip.pxi"
include "elementary.c_elementary_scroller.pxi"
include "elementary.c_elementary_label.pxi"
include "elementary.c_elementary_table.pxi"
include "elementary.c_elementary_clock.pxi"
include "elementary.c_elementary_hover.pxi"
include "elementary.c_elementary_entry.pxi"
include "elementary.c_elementary_bubble.pxi"
include "elementary.c_elementary_photo.pxi"
include "elementary.c_elementary_hoversel.pxi"
include "elementary.c_elementary_toolbar.pxi"
include "elementary.c_elementary_list.pxi"
include "elementary.c_elementary_slider.pxi"
include "elementary.c_elementary_naviframe.pxi"
include "elementary.c_elementary_radio.pxi"
include "elementary.c_elementary_check.pxi"
include "elementary.c_elementary_genlist.pxi"
include "elementary.c_elementary_gengrid.pxi"
include "elementary.c_elementary_spinner.pxi"
include "elementary.c_elementary_notify.pxi"
include "elementary.c_elementary_fileselector.pxi"
include "elementary.c_elementary_fileselector_entry.pxi"
include "elementary.c_elementary_fileselector_button.pxi"
include "elementary.c_elementary_separator.pxi"
include "elementary.c_elementary_progressbar.pxi"
include "elementary.c_elementary_menu.pxi"
include "elementary.c_elementary_panel.pxi"
include "elementary.c_elementary_web.pxi"
include "elementary.c_elementary_actionslider.pxi"
include "elementary.c_elementary_calendar.pxi"
include "elementary.c_elementary_colorselector.pxi"
include "elementary.c_elementary_index.pxi"
include "elementary.c_elementary_ctxpopup.pxi"
include "elementary.c_elementary_grid.pxi"
include "elementary.c_elementary_video.pxi"
include "elementary.c_elementary_conformant.pxi"
include "elementary.c_elementary_dayselector.pxi"
include "elementary.c_elementary_panes.pxi"
include "elementary.c_elementary_thumb.pxi"
include "elementary.c_elementary_diskselector.pxi"
include "elementary.c_elementary_datetime.pxi"
include "elementary.c_elementary_map.pxi"
include "elementary.c_elementary_mapbuf.pxi"
include "elementary.c_elementary_multibuttonentry.pxi"
include "elementary.c_elementary_transit.pxi"
include "elementary.c_elementary_slideshow.pxi"
include "elementary.c_elementary_segment_control.pxi"
include "elementary.c_elementary_popup.pxi"
include "elementary.c_elementary_plug.pxi"
include "elementary.c_elementary_photocam.pxi"
include "elementary.c_elementary_flipselector.pxi"
