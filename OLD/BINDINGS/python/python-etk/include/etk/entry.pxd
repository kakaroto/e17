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

cdef extern from "etk_entry.h":
    ####################################################################
    # Signals
    int ETK_ENTRY_TEXT_CHANGED_SIGNAL
    int ETK_ENTRY_TEXT_ACTIVATED_SIGNAL

    ####################################################################
    # Enumerations
    ctypedef enum Etk_Entry_Image_Position:
        ETK_ENTRY_IMAGE_PRIMARY
        ETK_ENTRY_IMAGE_SECONDARY

    ####################################################################
    # Structures
    ctypedef struct Etk_Entry

    ####################################################################
    # Functions
    Etk_Type* etk_entry_type_get()
    Etk_Widget* etk_entry_new()
    void etk_entry_clear(Etk_Entry* __self)
    void etk_entry_clear_button_add(Etk_Entry* __self)
    Etk_Image* etk_entry_image_get(Etk_Entry* __self, int position)
    void etk_entry_image_highlight_set(Etk_Entry* __self, int position, int highlight)
    void etk_entry_image_set(Etk_Entry* __self, int position, Etk_Image* image)
    int etk_entry_password_mode_get(Etk_Entry* __self)
    void etk_entry_password_mode_set(Etk_Entry* __self, int password_mode)
    char* etk_entry_text_get(Etk_Entry* __self)
    void etk_entry_text_set(Etk_Entry* __self, char* text)

#########################################################################
# Objects
cdef public class Entry(Widget) [object PyEtk_Entry, type PyEtk_Entry_Type]:
    pass

