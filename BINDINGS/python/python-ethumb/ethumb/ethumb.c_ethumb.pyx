# Copyright (C) 2009 by ProFUSION embedded systems
#
# This file is part of Python-Ethumb.
#
# Python-Ethumb is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ethumb is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ethumb.  If not, see <http://www.gnu.org/licenses/>.

def init():
    return ethumb_init()

def shutdown():
    ethumb_shutdown()

cdef class PyEthumb:
    """Ethumb thumbnail generator"""
    def __init__(self):
        """Ethumb constructor."""


init()
