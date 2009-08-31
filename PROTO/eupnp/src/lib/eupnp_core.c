/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Eupnp.h"
#include "eupnp_core.h"

/*
 * Private API
 */

static Eupnp_Fd_Handler_Add_Func _fd_handler_add_func = NULL;
static Eupnp_Fd_Handler_Del_Func _fd_handler_del_func = NULL;
static Eupnp_Timer_Add_Func _timer_add_func = NULL;
static Eupnp_Timer_Del_Func _timer_del_func = NULL;
static Eupnp_Idler_Add_Func _idler_add_func = NULL;
static Eupnp_Idler_Del_Func _idler_del_func = NULL;
static Eupnp_Request_Func _request_func = NULL;
static Eupnp_Request_Free_Func _request_free_func = NULL;
static Eupnp_Server_Add_Func _server_add = NULL;
static Eupnp_Server_Free_Func _server_free = NULL;
static Eupnp_Server_Url_Get_Func _server_listen_url_get = NULL;

/*
 * Public Integration API
 *
 * Used for integrating with main loops.
 */

EAPI void
eupnp_core_fd_handler_add_func_set(Eupnp_Fd_Handler_Add_Func func)
{
   if (func) _fd_handler_add_func = func;
}

EAPI void
eupnp_core_fd_handler_del_func_set(Eupnp_Fd_Handler_Del_Func func)
{
   if (func) _fd_handler_del_func = func;
}

EAPI void
eupnp_core_timer_add_func_set(Eupnp_Timer_Add_Func func)
{
   if (func) _timer_add_func = func;
}

EAPI void
eupnp_core_timer_del_func_set(Eupnp_Timer_Del_Func func)
{
   if (func) _timer_del_func = func;
}

EAPI void
eupnp_core_idler_add_func_set(Eupnp_Idler_Add_Func func)
{
   if (func) _idler_add_func = func;
}

EAPI void
eupnp_core_idler_del_func_set(Eupnp_Idler_Del_Func func)
{
   if (func) _idler_del_func = func;
}

EAPI void
eupnp_core_request_func_set(Eupnp_Request_Func func)
{
   if (func) _request_func = func;
}

EAPI void
eupnp_core_request_free_func_set(Eupnp_Request_Free_Func func)
{
   if (func) _request_free_func = func;
}

EAPI void
eupnp_core_server_add_func_set(Eupnp_Server_Add_Func func)
{
   if (func) _server_add = func;
}

EAPI void
eupnp_core_server_free_func_set(Eupnp_Server_Free_Func func)
{
   if (func) _server_free = func;
}

EAPI void
eupnp_core_server_listen_url_get_func_set(Eupnp_Server_Url_Get_Func func)
{
   if (func) _server_listen_url_get = func;
}

/*
 * Public API
 */

EAPI Eupnp_Fd_Handler
eupnp_core_fd_handler_add(int fd, Eupnp_Fd_Flags flags, Eupnp_Fd_Handler_Cb cb, void *data)
{
   return _fd_handler_add_func(fd, flags, cb, data);
}

EAPI Eina_Bool
eupnp_core_fd_handler_del(Eupnp_Fd_Handler handler)
{
   return _fd_handler_del_func(handler);
}

EAPI Eupnp_Timer
eupnp_core_timer_add(double interval, Eupnp_Timer_Cb timer, void *data)
{
   return _timer_add_func(interval, timer, data);
}

EAPI Eina_Bool
eupnp_core_timer_del(Eupnp_Timer timer)
{
   return _timer_del_func(timer);
}

EAPI Eupnp_Idler
eupnp_core_idler_add(Eupnp_Idler_Cb idler_func, void *data)
{
   return _idler_add_func(idler_func, data);
}

EAPI Eina_Bool
eupnp_core_idler_del(Eupnp_Idler idler)
{
   return _idler_del_func(idler);
}

EAPI Eupnp_Request
eupnp_core_http_request_send(const char *url, const char *request, Eina_Array *additional_headers, const char *content_type, size_t body_length, const char *body, Eupnp_Request_Data_Cb data_cb, Eupnp_Request_Completed_Cb completed_cb, void *data)
{
   return _request_func(url, request, additional_headers, content_type, body_length, body, data_cb, completed_cb, data);
}

EAPI void
eupnp_core_http_request_free(Eupnp_Request request)
{
   _request_free_func(request);
}

EAPI Eupnp_Server
eupnp_core_server_add(const char *name, int port, Eupnp_Client_Data_Cb cb, void *data)
{
   _server_add(name, port, cb, data);
}

EAPI void
eupnp_core_server_free(Eupnp_Server server)
{
   _server_free(server);
}

EAPI const char *
eupnp_core_server_listen_url_get(Eupnp_Server server)
{
   return _server_listen_url_get(server);
}
