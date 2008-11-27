# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Caio Marcelo de Oliveira Filho
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

import evas

def event_callback(event_type):
    def deco(func):
        func.evas_event_callback = event_type
        return staticmethod(func)
    return deco

def mouse_in_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_IN
    return staticmethod(func)

def mouse_out_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_OUT
    return staticmethod(func)

def mouse_down_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_DOWN
    return staticmethod(func)

def mouse_up_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_UP
    return staticmethod(func)

def mouse_move_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_MOVE
    return staticmethod(func)

def mouse_wheel_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOUSE_WHEEL
    return staticmethod(func)

def free_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FREE
    return staticmethod(func)

def key_down_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_KEY_DOWN
    return staticmethod(func)

def key_up_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_KEY_UP
    return staticmethod(func)

def focus_in_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FOCUS_IN
    return staticmethod(func)

def focus_out_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_FOCUS_OUT
    return staticmethod(func)

def show_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_SHOW
    return staticmethod(func)

def hide_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_HIDE
    return staticmethod(func)

def move_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_MOVE
    return staticmethod(func)

def resize_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_RESIZE
    return staticmethod(func)

def restack_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_RESTACK
    return staticmethod(func)

def del_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_DEL
    return staticmethod(func)

def hold_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_HOLD
    return staticmethod(func)

def changed_size_hints_callback(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_CHANGED_SIZE_HINTS
    return staticmethod(func)

def image_preloaded(func):
    func.evas_event_callback = evas.EVAS_CALLBACK_IMAGE_PRELOADED
    return staticmethod(func)
