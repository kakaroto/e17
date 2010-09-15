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


class Widget(object):
    def __init__(self):
        self._callbacks = dict()
        self._in_parser = None
        self._out_parser = None

    def _value_set(self, *val):
        if self._in_parser:
            v = self._in_parser(*val)
            self._internal_value_set(v)
        else:
            self._internal_value_set(*val)

    def _value_get(self):
        r = self._internal_value_get()
        if self._out_parser:
            return self._out_parser(r)
        return r

    value = property(_value_get, _value_set)

    def _visible_set(self, visible):
        self.obj.visible = visible

    def _visible_get(self):
        return self.obj.visible

    visible = property(_visible_get, _visible_set)

    def disabled_set(self, val):
        self.obj.disabled_set(val)

    def _validate_cb(self, cb):
        if type(cb) == tuple:
            callback, data = cb
        else:
            callback = cb
            data = None
        if not callable(callback):
            raise TypeError("callback is not callable")
        return (callback, data)

    def callback_add(self, name, cb):
        if cb is None:
            if name in self._callbacks:
                del self._callbacks[name]
                return
        self._callbacks[name] = self._validate_cb(cb)

    def _callback_call(self, name, event=None):
        cb = self._callbacks.get(name)
        if not cb:
            return
        func, data = cb
        func(self, event, data)

    def _changed_cb_set(self, cb):
        self.callback_add("changed", cb)

    changed = property(fset=_changed_cb_set)

    def parsers_set(self, pin, pout):
        if ((pin is not None and not callable(pin)) or
            (pout is not None and not callable(pout))):
            raise TypeError("callbacks are not callable")
        self._in_parser = pin
        self._out_parser = pout

    parsers = property(fset=parsers_set)

    def parser_in_set(self, pin):
        if not callable(pin):
            raise TypeError("callback is not callable")
        self._in_parser = pin

    parser_in = property(fset=parser_in_set)

    def parser_out_set(self, pout):
        if not callable(pout):
            raise TypeError("callback is not callable")
        self._out_parser = pout

    parser_out = property(fset=parser_out_set)

    def tooltip_set(self, label):
        print "WARNING: not implemented tooltip support in", self
