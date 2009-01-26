# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of python-efl_utils.
#
# python-efl_utils is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# python-efl_utils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this python-efl_utils.  If not, see <http://www.gnu.org/licenses/>.

def generate_multiple_wait(n_wait, end_callback):
    """Generate a callback that will call end_callback after it's called n_wait
       times.

       Signature: C{function(return_values)}

       where return_values is a list of tuples with (args, kargs) given to
       the generated callback.
    """
    finished = []
    def cb(*args, **kargs):
        finished.append((args, kargs))
        if len(finished) == n_wait:
            end_callback(finished)
    return cb
