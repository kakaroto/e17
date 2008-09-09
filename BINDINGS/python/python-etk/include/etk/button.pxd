# Copyright (C) 2007-2008 Caio Marcelo de Oliveira Filho
#
# This file is part of Python-Etk.
#
# Python-Etk is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Etk is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Etk.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from "etk_button.h":
    ####################################################################
    # Signals
    int ETK_BUTTON_PRESSED_SIGNAL
    int ETK_BUTTON_RELEASED_SIGNAL
    int ETK_BUTTON_CLICKED_SIGNAL

    ####################################################################
    # Enumerations
    ctypedef enum Etk_Button_Style:
        ETK_BUTTON_ICON
        ETK_BUTTON_TEXT
        ETK_BUTTON_BOTH_HORIZ
        ETK_BUTTON_BOTH_VERT

    ####################################################################
    # Structures
    ctypedef struct Etk_Button

    ####################################################################
    # Functions
    Etk_Widget* etk_button_new_from_stock(int stock_id)
    Etk_Widget* etk_button_new_with_label(char* label)
    Etk_Type* etk_button_type_get()
    Etk_Widget* etk_button_new()
    void etk_button_alignment_get(Etk_Button* __self, float* xalign, float* yalign)
    void etk_button_alignment_set(Etk_Button* __self, float xalign, float yalign)
    void etk_button_click(Etk_Button* __self)
    Etk_Image* etk_button_image_get(Etk_Button* __self)
    void etk_button_image_set(Etk_Button* __self, Etk_Image* image)
    char* etk_button_label_get(Etk_Button* __self)
    void etk_button_label_set(Etk_Button* __self, char* label)
    void etk_button_press(Etk_Button* __self)
    void etk_button_release(Etk_Button* __self)
    void etk_button_set_from_stock(Etk_Button* __self, int stock_id)
    int etk_button_stock_size_get(Etk_Button* __self)
    void etk_button_stock_size_set(Etk_Button* __self, int size)
    int etk_button_style_get(Etk_Button* __self)
    void etk_button_style_set(Etk_Button* __self, int style)

#########################################################################
# Objects
cdef public class Button(Bin) [object PyEtk_Button, type PyEtk_Button_Type]:
    pass

