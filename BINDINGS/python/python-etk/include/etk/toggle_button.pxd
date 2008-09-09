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

cdef extern from "etk_toggle_button.h":
    ####################################################################
    # Signals
    int ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Toggle_Button


    ####################################################################
    # Functions
    Etk_Widget* etk_toggle_button_new_with_label(char* label)
    Etk_Type* etk_toggle_button_type_get()
    Etk_Widget* etk_toggle_button_new()
    int etk_toggle_button_active_get(Etk_Toggle_Button* __self)
    void etk_toggle_button_active_set(Etk_Toggle_Button* __self, int active)
    void etk_toggle_button_toggle(Etk_Toggle_Button* __self)

#########################################################################
# Objects
cdef public class ToggleButton(Button) [object PyEtk_Toggle_Button, type PyEtk_Toggle_Button_Type]:
    pass

