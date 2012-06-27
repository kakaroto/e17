# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

def need_efreet():
    """Request that your elementary application needs Efreet

    This initializes the Efreet library when called and if support exists
    it returns True, otherwise returns False. This must be called
    before any efreet calls.

    @return: True if support exists and initialization succeeded.
    @rtype: bool

    """
    return bool(elm_need_efreet())

def need_e_dbus():
    """Request that your elementary application needs e_dbus

    This initializes the E_dbus library when called and if support exists
    it returns True, otherwise returns False. This must be called
    before any e_dbus calls.

    @return: True if support exists and initialization succeeded.
    @rtype: bool

    """
    return bool(elm_need_e_dbus())

def need_ethumb():
    """Request that your elementary application needs ethumb

    This initializes the Ethumb library when called and if support exists
    it returns True, otherwise returns False.
    This must be called before any other function that deals with
    elm_thumb objects or ethumb_client instances.

    @return: True if support exists and initialization succeeded.
    @rtype: bool

    """
    return bool(elm_need_ethumb())

def need_web():
    """Request that your elementary application needs web support

    This initializes the Ewebkit library when called and if support exists
    it returns True, otherwise returns False.
    This must be called before any other function that deals with
    elm_web objects or ewk_view instances.

    @return: True if support exists and initialization succeeded.
    @rtype: bool

    """
    return bool(elm_need_web())
