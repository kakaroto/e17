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

cdef public class ToggleButton(Button) [object PyEtk_Toggle_Button, type PyEtk_Toggle_Button_Type]:
    def __init__(self, label=None, **kargs):
        if self.obj == NULL:
            if label is None:
                self._set_obj(<Etk_Object*>etk_toggle_button_new())
            else:
                self._set_obj(<Etk_Object*>etk_toggle_button_new_with_label(label))
        self._set_common_params(**kargs)

    def active_get(self):
        __ret = bool(<int> etk_toggle_button_active_get(<Etk_Toggle_Button*>self.obj))
        return (__ret)

    def active_set(self, int active):
        etk_toggle_button_active_set(<Etk_Toggle_Button*>self.obj, <Etk_Bool>active)

    def toggle(self):
        etk_toggle_button_toggle(<Etk_Toggle_Button*>self.obj)

    property active:
        def __get__(self):
            return self.active_get()

        def __set__(self, active):
            self.active_set(active)

    def _set_common_params(self, active=None, **kargs):
        if active is not None:
            self.active_set(active)

        if kargs:
            Button._set_common_params(self, **kargs)

    property TOGGLED_SIGNAL:
        def __get__(self):
            return ETK_TOGGLE_BUTTON_TOGGLED_SIGNAL

    def on_toggled(self, func, *a, **ka):
        self.connect(self.TOGGLED_SIGNAL, func, *a, **ka)


class ToggleButtonEnums:
    pass
