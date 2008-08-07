# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

def signal_callback(emission, source):
    def deco(func):
        func.edje_signal_callback = (emission, source)
        return staticmethod(func)
    return deco


def text_change_callback(func):
    func.edje_text_change_callback = True
    return staticmethod(func)


def message_handler(func):
    func.edje_message_handler = True
    return staticmethod(func)
