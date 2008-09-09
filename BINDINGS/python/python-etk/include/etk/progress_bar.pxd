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

cdef extern from "etk_progress_bar.h":

    ####################################################################
    # Enumerations
    ctypedef enum Etk_Progress_Bar_Direction:
        ETK_PROGRESS_BAR_LEFT_TO_RIGHT
        ETK_PROGRESS_BAR_RIGHT_TO_LEFT

    ####################################################################
    # Structures
    ctypedef struct Etk_Progress_Bar

    ####################################################################
    # Functions
    Etk_Widget* etk_progress_bar_new_with_text(char* label)
    Etk_Type* etk_progress_bar_type_get()
    Etk_Widget* etk_progress_bar_new()
    int etk_progress_bar_direction_get(Etk_Progress_Bar* __self)
    void etk_progress_bar_direction_set(Etk_Progress_Bar* __self, int direction)
    double etk_progress_bar_fraction_get(Etk_Progress_Bar* __self)
    void etk_progress_bar_fraction_set(Etk_Progress_Bar* __self, double fraction)
    void etk_progress_bar_pulse(Etk_Progress_Bar* __self)
    double etk_progress_bar_pulse_step_get(Etk_Progress_Bar* __self)
    void etk_progress_bar_pulse_step_set(Etk_Progress_Bar* __self, double pulse_step)
    char* etk_progress_bar_text_get(Etk_Progress_Bar* __self)
    void etk_progress_bar_text_set(Etk_Progress_Bar* __self, char* label)

#########################################################################
# Objects
cdef public class ProgressBar(Widget) [object PyEtk_Progress_Bar, type PyEtk_Progress_Bar_Type]:
    pass

