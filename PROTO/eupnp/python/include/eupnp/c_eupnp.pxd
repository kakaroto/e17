# Copyright (C) 2010 André Dieb Martins <andre.dieb@gmail.com>
#
# This file is part of Python-Eupnp.
#
# Python-Eupnp is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Eupnp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Eupnp.  If not, see <http://www.gnu.org/licenses/>.

ctypedef int Eina_Bool

cdef extern from "Eupnp.h":
    int eupnp_init()
    int eupnp_shutdown()

cdef extern from "eupnp_ecore.h":
    int eupnp_ecore_init()
    int eupnp_ecore_shutdown()

cdef extern from "eupnp_control_point.h":

    ctypedef struct Eupnp_Control_Point

    Eupnp_Control_Point *eupnp_control_point_new()
    void eupnp_control_point_free(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_start(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_stop(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_discovery_request_send(Eupnp_Control_Point *c, int mx, char *search_target)
