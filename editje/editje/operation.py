# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.


class Operation(object):
    def __init__(self, name=""):
        self._name = name
        self._undo_cbs = []
        self._redo_cbs = []

    def _name_get(self):
        return self._name

    name = property(fget=_name_get)

    def _callback_add(self, list_, func, *args, **kargs):
        if (func, args, kargs) in list_:
            return

        if not callable(func):
            raise TypeError("%s is not callable" % func)

        list_.append((func, args, kargs))

    def _do(self, list_):
        for func, args, kargs in list_:
            if args and kargs:
                func(*args, **kargs)
            elif args:
                func(*args)
            elif kargs:
                func(**kargs)
            else:
                func()

    def undo_callback_add(self, func, *args, **kargs):
        self._callback_add(self._undo_cbs, func, *args, **kargs)

    def redo_callback_add(self, func, *args, **kargs):
        self._callback_add(self._redo_cbs, func, *args, **kargs)

    def undo(self):
        self._do(self._undo_cbs)

    def redo(self):
        self._do(self._redo_cbs)

    def __str__(self):
        return ("%s(name=%s, callbacks=(%s))") % \
            (self.__class__.__name__, self._name, self._undo_cbs)
