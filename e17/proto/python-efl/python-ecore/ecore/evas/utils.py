# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

__callbacks = (
    "resize",
    "move",
    "show",
    "hide",
    "delete_request",
    "destroy",
    "focus_in",
    "focus_out",
    "sticky",
    "unsticky",
    "mouse_in",
    "mouse_out",
    "pre_render",
    "post_render",
    )

def __get_callback(observer, name):
    try:
        attr = getattr(observer, "cb_on_%s" % name)
        if callable(attr):
            return attr
    except AttributeError, e:
        return None

def connect_observer(ecore_evas, observer):
    """Connect methods from observer to Ecore_Evas callbacks.

    Observer must have methods with name scheme: cb_on_<callback>,
    examples:
     - cb_on_resize
     - cb_on_move
     - cb_on_show
    """
    for cb_name in __callbacks:
        cb = __get_callback(observer, cb_name)
        if cb:
            setter = getattr(ecore_evas, "callback_%s_set" % cb_name)
            setter(cb)


def connect_callbacks_by_name(ecore_evas, mapping):
    """Connect callbacks specified in mapping to Ecore_Evas callbacks.

    Mapping must be a dict or a list of tuples with callback name and
    desired function, example:
     - mapping = (("resize", my_on_resize), ("show", my_on_show))
     - mapping = {"resize": my_on_resize, "show": my_on_show}
    """
    if isinstance(mapping, dict):
        mapping = mapping.iteritems()
    for name, func in mapping:
        try:
            setter = getattr(ecore_evas, "callback_%s_set" % name)
        except AttributeError, e:
            raise ValueError("invalid callback name: %s" % name)
        setter(func)
