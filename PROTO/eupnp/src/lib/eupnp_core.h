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

#ifndef _EUPNP_CORE_H
#define _EUPNP_CORE_H

#include <Eina.h>
#include "eupnp_http_message.h"

typedef void* Eupnp_Fd_Handler;
typedef void* Eupnp_Timer;
typedef void* Eupnp_Idler;
typedef void* Eupnp_Request;
typedef void* Eupnp_Server;

typedef Eina_Bool (*Eupnp_Fd_Handler_Cb) (void *data, Eupnp_Fd_Handler fd_handler);
typedef Eina_Bool (*Eupnp_Timer_Cb) (void *data);
typedef Eina_Bool (*Eupnp_Idler_Cb) (void *data);
typedef void (*Eupnp_Request_Data_Cb) (void *buffer, int size, void *data);
typedef void (*Eupnp_Request_Completed_Cb) (Eupnp_Request *request, void *data, const Eupnp_HTTP_Request *req);
typedef void (*Eupnp_Client_Data_Cb) (void *buffer, int size, void *data);


typedef enum {
   EUPNP_FD_EVENT_READ = 1,
   EUPNP_FD_EVENT_WRITE = 2,
   EUPNP_FD_EVENT_ERROR = 4
} Eupnp_Fd_Flags;

#define EUPNP_IDLER_CB(f) ((Eupnp_Idler_Cb)f)
#define EUPNP_TIMER_CB(f) ((Eupnp_Timer_Cb)f)
#define EUPNP_FD_HANDLER_CB(f) ((Eupnp_Fd_Handler_Cb)f)
#define EUPNP_REQUEST_DATA_CB(f) ((Eupnp_Request_Data_Cb)f)
#define EUPNP_REQUEST_COMPLETED_CB(f) ((Eupnp_Request_Completed_Cb)f)
#define EUPNP_CLIENT_DATA_CB(f) ((Eupnp_Client_Data_Cb)f)

/*
 * Integration setup
 */
typedef Eupnp_Fd_Handler (*Eupnp_Fd_Handler_Add_Func) (int fd, Eupnp_Fd_Flags flags, Eupnp_Fd_Handler_Cb callback, void *data);
typedef Eina_Bool (*Eupnp_Fd_Handler_Del_Func) (Eupnp_Fd_Handler handler);

typedef Eupnp_Timer (*Eupnp_Timer_Add_Func) (double interval, Eupnp_Timer_Cb timer, void *data);
typedef Eina_Bool (*Eupnp_Timer_Del_Func) (Eupnp_Timer timer);

typedef Eupnp_Idler (*Eupnp_Idler_Add_Func) (Eupnp_Idler_Cb idler_func, void *data);
typedef Eina_Bool (*Eupnp_Idler_Del_Func) (Eupnp_Idler idler);

typedef Eupnp_Request (*Eupnp_Request_Func) (const char *url, const char *request, Eina_Array *additional_headers, const char *content_type, size_t body_length, const char *body, Eupnp_Request_Data_Cb data_cb, Eupnp_Request_Completed_Cb completed_cb, void *data);
typedef void (*Eupnp_Request_Free_Func) (Eupnp_Request request);

typedef Eupnp_Server (*Eupnp_Server_Add_Func) (const char *name, int port, Eupnp_Client_Data_Cb cb, void *data);
typedef void (*Eupnp_Server_Free_Func) (Eupnp_Server server);
typedef const char * (*Eupnp_Server_Url_Get_Func) (Eupnp_Server server);


void        eupnp_core_fd_handler_add_func_set(Eupnp_Fd_Handler_Add_Func func);
void        eupnp_core_fd_handler_del_func_set(Eupnp_Fd_Handler_Del_Func func);
void        eupnp_core_timer_add_func_set(Eupnp_Timer_Add_Func func);
void        eupnp_core_timer_del_func_set(Eupnp_Timer_Del_Func func);
void        eupnp_core_idler_add_func_set(Eupnp_Idler_Add_Func func);
void        eupnp_core_idler_del_func_set(Eupnp_Idler_Del_Func func);

void        eupnp_core_request_func_set(Eupnp_Request_Func func);
void        eupnp_core_request_free_func_set(Eupnp_Request_Free_Func func);

void        eupnp_core_server_add_func_set(Eupnp_Server_Add_Func func);
void        eupnp_core_server_free_func_set(Eupnp_Server_Free_Func func);
void        eupnp_core_server_listen_url_get_func_set(Eupnp_Server_Url_Get_Func func);


/*
 * Public API
 */

Eupnp_Fd_Handler eupnp_core_fd_handler_add(int fd, Eupnp_Fd_Flags flags, Eupnp_Fd_Handler_Cb cb, void *data);
Eina_Bool        eupnp_core_fd_handler_del(Eupnp_Fd_Handler handler);

Eupnp_Timer      eupnp_core_timer_add(double interval, Eupnp_Timer_Cb timer, void *data);
Eina_Bool        eupnp_core_timer_del(Eupnp_Timer timer);

Eupnp_Idler      eupnp_core_idler_add(Eupnp_Idler_Cb idler_func, void *data);
Eina_Bool        eupnp_core_idler_del(Eupnp_Idler idler);

Eupnp_Request    eupnp_core_http_request_send(const char *url, const char *request, Eina_Array *additional_headers, const char *content_type, size_t body_length, const char *body, Eupnp_Request_Data_Cb data_cb, Eupnp_Request_Completed_Cb completed_cb, void *data);
void             eupnp_core_http_request_free(Eupnp_Request request);

Eupnp_Server     eupnp_core_server_add(const char *name, int port, Eupnp_Client_Data_Cb cb, void *data);
void             eupnp_core_server_free(Eupnp_Server server);

const char      *eupnp_core_server_listen_url_get(Eupnp_Server server);

#endif /* _EUPNP_CORE_H */
