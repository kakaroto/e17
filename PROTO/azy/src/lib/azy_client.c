/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <regex.h>
#include <ctype.h>
#include <inttypes.h>
#include "Azy.h"
#include "azy_private.h"

static Azy_Client_Call_Id azy_client_send_id__ = 0;

EAPI int AZY_CLIENT_DISCONNECTED;
EAPI int AZY_CLIENT_CONNECTED;
EAPI int AZY_CLIENT_UPGRADE;
EAPI int AZY_CLIENT_RETURN;
EAPI int AZY_CLIENT_RESULT;
EAPI int AZY_CLIENT_ERROR;

/**
 * @defgroup Azy_Client Client Functions
 * @brief Functions which affect #Azy_Client
 * @{
 */

/**
 * @brief Allocate a new client object
 *
 * This function creates a new client object for use in connecting to a
 * server.
 * @return The new client, or #NULL on failure
 */
Azy_Client *
azy_client_new(void)
{
   Azy_Client *client;

   if (!(client = calloc(1, sizeof(Azy_Client))))
     return NULL;

   client->add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, (Ecore_Event_Handler_Cb)_azy_client_handler_add, client);
   client->del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, (Ecore_Event_Handler_Cb)_azy_client_handler_del, client);
   client->upgrade = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_UPGRADE,
                                             (Ecore_Event_Handler_Cb)_azy_client_handler_upgrade, client);

   AZY_MAGIC_SET(client, AZY_MAGIC_CLIENT);
   return client;
}

/**
 * @brief Get the data previously associated with a client
 *
 * This function retrieves the data previously set to @p client
 * with azy_client_data_set.
 * @param client The client object (NOT #NULL)
 * @return The data, or #NULL on error
 */
void *
azy_client_data_get(Azy_Client *client)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return NULL;
     }
   return client->data;
}

/**
 * @brief Set the data previously associated with a client
 *
 * This function sets the data associated with @p client to @p data
 * for retrieval with azy_client_data_get.
 * @param client The client object (NOT #NULL)
 * @param data The data to associate
 */
void
azy_client_data_set(Azy_Client *client,
                    const void *data)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return;
     }
   client->data = (void *)data;
}

/**
 * @brief Set the connection info for a client
 *
 * This function sets the server address and port for a client to
 * connect to.  The address can be either an ip string (ipv6 supported)
 * or a web address.
 * @param client The client object (NOT #NULL)
 * @param addr The server's address (NOT #NULL)
 * @param port The port on the server (-1 < port < 65536)
 * @return #EINA_TRUE on success, else #EINA_FALSE
 */
Eina_Bool
azy_client_host_set(Azy_Client *client,
                    const char *addr,
                    int         port)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   if ((!addr) || (port < 0) || (port > 65536))
     return EINA_FALSE;

   if (client->addr)
     eina_stringshare_del(client->addr);
   if (!strncmp(addr, "http://", 7))
     addr += 7;
   else if (!strncmp(addr, "https://", 8))
     addr += 8;
   client->addr = eina_stringshare_add(addr);
   client->port = port;

   return EINA_TRUE;
}

/**
 * @brief Get the address of the server that the client connects to
 *
 * This function returns the address string of the server that @p client
 * connects to.  The returned string is stringshared but still
 * belongs to the client object.
 * @param client The client object (NOT #NULL)
 * @return The address string, or #NULL on failure
 */
const char *
azy_client_addr_get(Azy_Client *client)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return NULL;
     }

   return client->addr;
}

/**
 * @brief Set the address of the server that the client connects to
 *
 * This function sets the address string of the server that @p client
 * connects to.
 * @param client The client object (NOT #NULL)
 * @param addr The address string (NOT #NULL)
 * @return The address string
 */
Eina_Bool
azy_client_addr_set(Azy_Client *client,
                    const char *addr)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(addr, EINA_FALSE);
   if (!strncmp(addr, "http://", 7))
     addr += 7;
   else if (!strncmp(addr, "https://", 8))
     addr += 8;
   client->addr = eina_stringshare_add(addr);
   return EINA_TRUE;
}

/**
 * @brief Get the port that the client connects to
 *
 * This function returns the port number on the server that @p client
 * connects to.
 * @param client The client object (NOT #NULL)
 * @return The port number, or -1 on failure
 */
int
azy_client_port_get(Azy_Client *client)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return -1;
     }

   return client->port;
}

/**
 * @brief Set the port that the client connects to
 *
 * This function sets the port number on the server that @p client
 * connects to.
 * @param client The client object (NOT #NULL)
 * @param port The port number (-1 < port < 65536)
 * @return #EINA_TRUE on success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_client_port_set(Azy_Client *client,
                    int         port)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   if ((port < 0) || (port > 65535))
     return EINA_FALSE;

   client->port = port;
   return EINA_TRUE;
}

/**
 * @brief Upgrade a client's connection to SSL/TLS
 *
 * This function begins the SSL handshake process on connected client @p client.
 * An AZY_CLIENT_UPGRADE event will be emitted on success, and EINA_FALSE will be
 * returned immediately on failure.
 * @param client The client object (NOT #NULL)
 * @return #EINA_TRUE if successful, or #EINA_FALSE on failure
 */
Eina_Bool
azy_client_upgrade(Azy_Client *client)
{
   DBG("(client=%p)", client);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   if (!client->connected) return EINA_FALSE;

   return ecore_con_server_upgrade(client->net->conn, ECORE_CON_USE_MIXED);
}

/**
 * @brief Connect the client to its server
 *
 * This function begins the connection process for @p client to its
 * previously set server.  This will return EINA_FALSE immediately if an error occurs.
 * @param client The client object (NOT #NULL)
 * @param secure If #EINA_TRUE, TLS will be used in the connection
 * @return #EINA_TRUE if successful, or #EINA_FALSE on failure
 */
Eina_Bool
azy_client_connect(Azy_Client *client,
                   Eina_Bool   secure)
{
   DBG("(client=%p)", client);
   Ecore_Con_Server *svr;
   int flags = ECORE_CON_REMOTE_NODELAY;

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   if ((client->connected) || (!client->addr) || (!client->port))
     return EINA_FALSE;

   client->secure = !!secure;

   if (secure) flags |= ECORE_CON_USE_MIXED;

   if (!(svr = ecore_con_server_connect(flags, client->addr, client->port, NULL)))
     return EINA_FALSE;

   ecore_con_server_data_set(svr, client);

   ecore_con_server_timeout_set(svr, 1);

   client->net = azy_net_new(svr);
   azy_net_header_set(client->net, "host", NULL);
   azy_net_header_set(client->net, "host", client->addr);

   return EINA_TRUE;
}

/**
 * @brief Get the network object associated with the client
 *
 * This function returns the #Azy_Net object associated with @p client
 * which is used for all outgoing data transmissions.  From here, azy_net
 * namespaced functions can be called as normal upon the returned object.
 * Note that the returned object belongs to the client, and will only exist
 * if the client is connected.
 * @param client The client object (NOT #NULL)
 * @return The #Azy_Net object, or #NULL on failure
 */
Azy_Net *
azy_client_net_get(Azy_Client *client)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return NULL;
     }

   return client->net;
}

/**
 * @brief Check whether a client is connected
 *
 * This function returns true only when the client is connected.
 * @param client The client (NOT #NULL)
 * @return #EINA_TRUE if the client is connected, else #EINA_FALSE
 */
Eina_Bool
azy_client_connected_get(Azy_Client *client)
{
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   return client->connected;
}

/**
 * @brief Close a client's connection
 *
 * This function is the opposite of azy_client_connect, it
 * terminates an existing connection.
 * @param client The client (NOT #NULL)
 */
void
azy_client_close(Azy_Client *client)
{
   DBG("(client=%p)", client);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return;
     }
   EINA_SAFETY_ON_FALSE_RETURN(client->connected);
   EINA_SAFETY_ON_NULL_RETURN(client->net);

   ecore_con_server_del(client->net->conn);

   azy_net_free(client->net);
   client->net = NULL;

   client->connected = EINA_FALSE;
}

/**
 * @brief Set a callback for an #Azy_Client_Call_Id
 *
 * This function is used to setup a callback to be called for the response of
 * a transmission with @p id, overriding (disabling) the AZY_CLIENT_RETURN event
 * for that call.  If a previous callback was set for @p id, this will overwrite it.
 * @param client The client (NOT #NULL)
 * @param id The transmission id (> 0)
 * @param callback The callback to use (NOT #NULL)
 * @return #EINA_TRUE on success, or #EINA_FALSE on failure
 */
Eina_Bool
azy_client_callback_set(Azy_Client          *client,
                        Azy_Client_Call_Id   id,
                        Azy_Client_Return_Cb callback)
{
   DBG("(client=%p, id=%u)", client, id);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(id < 1, EINA_FALSE);

   if (!client->callbacks)
     client->callbacks = eina_hash_int32_new(NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(client->callbacks, EINA_FALSE);

   return eina_hash_add(client->callbacks, &id, callback);
}

/**
 * @brief Set a callback to free the return struct of @p id
 *
 * This function, when set, frees the returned user-type struct of a call.
 * @param client The client
 * @param id The transmission id
 * @param callback The free callback
 */
Eina_Bool
azy_client_callback_free_set(Azy_Client        *client,
                             Azy_Client_Call_Id id,
                             Ecore_Cb           callback)
{
   DBG("(client=%p, id=%u)", client, id);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(callback, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(id < 1, EINA_FALSE);

   if (!client->free_callbacks)
     client->free_callbacks = eina_hash_int32_new(NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(client->free_callbacks, EINA_FALSE);

   return eina_hash_add(client->free_callbacks, &id, callback);
}

/**
 * @brief Make a method call using a connected client
 *
 * This function is used to make a method call on @p client as defined in
 * @p content, using content-type defined by @p transport and the deserialization
 * function specified by @p cb.  This should generally not be used by users, as azy_parser
 * will automatically generate the correct calls from a .azy file.
 * @param client The client (NOT #NULL)
 * @param content The content containing the method name and parameters (NOT #NULL)
 * @param transport The content-type (xml/json/etc) to use
 * @param cb The deserialization callback to use for the response
 * @return The #Azy_Client_Call_Id of the transmission, to be used with azy_client_callback_set,
 * or 0 on failure
 */
Azy_Client_Call_Id
azy_client_call(Azy_Client       *client,
                Azy_Content      *content,
                Azy_Net_Transport transport,
                Azy_Content_Cb    cb)
{
   Eina_Strbuf *msg;
   Azy_Client_Handler_Data *hd;

   DBG("(client=%p, net=%p, content=%p)", client, client->net, content);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return 0;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(client->net, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content->method, 0);

   INFO("New method call: '%s'", content->method);

   while (++azy_client_send_id__ < 1) ;

   content->id = azy_client_send_id__;

   azy_net_transport_set(client->net, transport);
   if (!azy_content_serialize_request(content, transport))
     return 0;
   azy_net_type_set(client->net, AZY_NET_TYPE_POST);
   if (!client->net->http.req.http_path)
     {
        WARN("URI currently set to NULL, defaulting to \"/\"");
        azy_net_uri_set(client->net, "/");
     }

   azy_net_message_length_set(client->net, content->length);
   msg = azy_net_header_create(client->net);
   EINA_SAFETY_ON_NULL_GOTO(msg, error);

   if (azy_rpc_log_dom >= 0)
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "\nSENDING >>>>>>>>>>>>>>>>>>>>>>>>\n%%.%is%%.%llis\n>>>>>>>>>>>>>>>>>>>>>>>>",
            eina_strbuf_length_get(msg), content->length);
        RPC_DBG(buf, eina_strbuf_string_get(msg), content->buffer);
     }

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, eina_strbuf_string_get(msg), eina_strbuf_length_get(msg)), error);
   INFO("Send [1/2] complete! %zu bytes queued for sending.", eina_strbuf_length_get(msg));
   eina_strbuf_free(msg);
   msg = NULL;

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, content->buffer, content->length), error);
   INFO("Send [2/2] complete! %lli bytes queued for sending.", content->length);
   ecore_con_server_flush(client->net->conn);

   hd = calloc(1, sizeof(Azy_Client_Handler_Data));
   EINA_SAFETY_ON_NULL_RETURN_VAL(hd, 0);
   hd->client = client;
   hd->method = eina_stringshare_ref(content->method);
   hd->callback = cb;
   hd->type = AZY_NET_TYPE_POST;
   hd->content_data = content->data;
   hd->send = eina_strbuf_new();
   eina_strbuf_append_length(hd->send, (char *)content->buffer, content->length);

   hd->id = azy_client_send_id__;
   AZY_MAGIC_SET(hd, AZY_MAGIC_CLIENT_DATA_HANDLER);
   if (!client->conns)
     {
        client->recv = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                               (Ecore_Event_Handler_Cb)_azy_client_handler_data, hd);
        ecore_con_server_data_set(client->net->conn, client);
     }

   client->conns = eina_list_append(client->conns, hd);

   DBG("(client=%p, net=%p, content=%p, hd=%p)", client, client->net, content, hd);
   return azy_client_send_id__;
error:
   if (msg)
     eina_strbuf_free(msg);
   return 0;
}

/**
 * @brief Make an HTTP GET or POST request using a connected client with no HTTP BODY
 *
 * This function is used to make a GET or POST request using @p client to the uri of the client's
 * #Azy_Net object (azy_net_get(client)) using HTTP method @p type, content-type
 * defined by @p transport, and the optional deserialization function specified by @p cb.
 * @param client The client (NOT #NULL)
 * @param type The HTTP method to use (NOT #NULL)
 * @param netdata The HTTP BODY to send with a POST
 * @param cb The deserialization callback to use for the response
 * @param data The user data to be passed to resulting callbacks
 * @return The #Azy_Client_Call_Id of the transmission, to be used with azy_client_callback_set,
 * or 0 on failure
 */
Azy_Client_Call_Id
azy_client_blank(Azy_Client    *client,
                 Azy_Net_Type   type,
                 Azy_Net_Data  *netdata,
                 Azy_Content_Cb cb,
                 void          *data)
{
   Eina_Strbuf *msg;
   Azy_Client_Handler_Data *hd;

   DBG("(client=%p, net=%p)", client, client->net);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return 0;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(client->net, 0);
   EINA_SAFETY_ON_TRUE_RETURN_VAL((type != AZY_NET_TYPE_GET) && (type != AZY_NET_TYPE_POST), 0);

   while (++azy_client_send_id__ < 1) ;

   client->net->type = type;

   if (!client->net->http.req.http_path)
     {
        WARN("NULL URI passed, defaulting to \"/\"");
        azy_net_uri_set(client->net, "/");
     }
   if (netdata && netdata->size && (type == AZY_NET_TYPE_POST))
     azy_net_message_length_set(client->net, netdata->size);

   msg = azy_net_header_create(client->net);
   EINA_SAFETY_ON_NULL_GOTO(msg, error);

#ifdef ISCOMFITOR
   char buf[64];
   snprintf(buf, sizeof(buf), "\nSENDING >>>>>>>>>>>>>>>>>>>>>>>>\n%%.%zus\n>>>>>>>>>>>>>>>>>>>>>>>>",
            eina_strbuf_length_get(msg));
   DBG(buf, eina_strbuf_string_get(msg));
#endif

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, eina_strbuf_string_get(msg), eina_strbuf_length_get(msg)), error);
   if (netdata && netdata->size && (type == AZY_NET_TYPE_POST))
     {
        INFO("Send [1/2] complete! %zu bytes queued for sending.", eina_strbuf_length_get(msg));
        EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, netdata->data, netdata->size), error);
        INFO("Send [2/2] complete! %" PRIi64 " bytes queued for sending.", netdata->size);
     }
   else
     INFO("Send [1/1] complete! %zu bytes queued for sending.", eina_strbuf_length_get(msg));
   eina_strbuf_free(msg);
   msg = NULL;

   ecore_con_server_flush(client->net->conn);

   hd = calloc(1, sizeof(Azy_Client_Handler_Data));
   EINA_SAFETY_ON_NULL_RETURN_VAL(hd, 0);
   hd->client = client;
   hd->callback = cb;
   hd->type = type;
   hd->content_data = data;
   if (netdata && netdata->size && (type == AZY_NET_TYPE_POST))
     {
        hd->send = eina_strbuf_new();
        eina_strbuf_append_length(hd->send, (char *)netdata->data, netdata->size);
     }

   hd->id = azy_client_send_id__;
   AZY_MAGIC_SET(hd, AZY_MAGIC_CLIENT_DATA_HANDLER);
   if (!client->conns)
     {
        client->recv = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                               (Ecore_Event_Handler_Cb)_azy_client_handler_data, hd);
        ecore_con_server_data_set(client->net->conn, client);
     }

   client->conns = eina_list_append(client->conns, hd);

   DBG("(client=%p, net=%p, hd=%p)", client, client->net, hd);
   return azy_client_send_id__;
error:
   if (msg) eina_strbuf_free(msg);
   return 0;
}

/**
 * @brief Send arbitrary data to a connected server
 *
 * This function is used to send arbitrary data to a connected server using @p client through HTTP PUT.
 * It relies on the user to set required headers by operating on the client's #Azy_Net object.
 * @param client The client (NOT #NULL)
 * @param send The data+length to send (NOT #NULL)
 * @param data Optional data to pass to associated callbacks
 * @return The #Azy_Client_Call_Id of the transmission, to be used with azy_client_callback_set,
 * or 0 on failure
 * @see azy_net_header_set
 */
Azy_Client_Call_Id
azy_client_put(Azy_Client         *client,
               const Azy_Net_Data *send,
               void               *data)
{
   Eina_Strbuf *msg;
   Azy_Client_Handler_Data *hd;

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return 0;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(send, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(send->data, 0);

   azy_net_message_length_set(client->net, send->size);
   azy_net_type_set(client->net, AZY_NET_TYPE_PUT);
   msg = azy_net_header_create(client->net);
   EINA_SAFETY_ON_NULL_GOTO(msg, error);
#ifdef ISCOMFITOR
   DBG("\nSENDING >>>>>>>>>>>>>>>>>>>>>>>>\n%.*s%.*s\n>>>>>>>>>>>>>>>>>>>>>>>>",
       eina_strbuf_length_get(msg), eina_strbuf_string_get(msg), (int)send->size, send->data);
#endif
   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, eina_strbuf_string_get(msg), eina_strbuf_length_get(msg)), error);
   INFO("Send [1/2] complete! %zi bytes queued for sending.", eina_strbuf_length_get(msg));
   eina_strbuf_free(msg);
   msg = NULL;

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(client->net->conn, send->data, send->size), error);
   INFO("Send [2/2] complete! %" PRIi64 " bytes queued for sending.", send->size);
   ecore_con_server_flush(client->net->conn);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(!(hd = calloc(1, sizeof(Azy_Client_Handler_Data))), 0);

   if (!client->conns)
     {
        client->recv = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                               (Ecore_Event_Handler_Cb)_azy_client_handler_data, hd);
        ecore_con_server_data_set(client->net->conn, client);
     }

   hd->client = client;
   hd->content_data = data;
   hd->type = AZY_NET_TYPE_PUT;
   AZY_MAGIC_SET(hd, AZY_MAGIC_CLIENT_DATA_HANDLER);

   while (++azy_client_send_id__ < 1) ;

   hd->id = azy_client_send_id__;
   client->conns = eina_list_append(client->conns, hd);

   return azy_client_send_id__;

error:
   if (msg)
     eina_strbuf_free(msg);
   return 0;
}

/**
 * @brief Validate a transmission attempt
 *
 * This function is used to check both the #Azy_Client_Call_Id and the #Azy_Content
 * of an azy_client_call or azy_client_put attempt, and will additionally set
 * an #Azy_Client_Return_Cb and log the calling function name upon failure.
 * Note that this function also calls azy_content_error_reset.
 * Also note: THIS FUNCTION IS MEANT TO BE USED IN A MACRO!!!!
 * @param cli The client (NOT #NULL)
 * @param err_content The content used to make the call which may contain an error (NOT #NULL)
 * @param ret The call id
 * @param cb The callback to set for @p ret (NOT #NULL)
 * @param func The function name of the calling function
 * @return This function returns #EINA_TRUE only if the call was successful and @p cb was set, else #EINA_FALSE
 */
Eina_Bool
azy_client_call_checker(Azy_Client          *cli,
                        Azy_Content         *err_content,
                        Azy_Client_Call_Id   ret,
                        Azy_Client_Return_Cb cb,
                        const char          *func)
{
   DBG("(cli=%p, cb=%p, func='%s')", cli, cb, func);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cli, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(err_content, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cb, EINA_FALSE);
   if (!ret) return EINA_FALSE;

   if (azy_content_error_is_set(err_content))
     {
        ERR("%s:\n%s", func ? func : "<calling function not specified>", azy_content_error_message_get(err_content));
        azy_content_error_reset(err_content);
        return EINA_FALSE;
     }

   azy_content_error_reset(err_content);
   if (!azy_client_callback_set(cli, ret, cb))
     {
        CRI("Could not set callback from %s!", func ? func : "<calling function not specified>");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

/**
 * @brief Helper function to automatically handle redirection
 *
 * This function is used inside an AZY_CLIENT_DISCONNECTED callback to automatically
 * reconnect to the server if necessary (HTTP 301/302/303 returned).
 * @param cli The client object (NOT #NULL)
 * @return #EINA_TRUE only if reconnection has succeeded, else #EINA_FALSE
 */
Eina_Bool
azy_client_redirect(Azy_Client *cli)
{
   int code;
   Azy_Net *net;

   if (!AZY_MAGIC_CHECK(cli, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(cli, AZY_MAGIC_CLIENT);
        return EINA_FALSE;
     }

   net = azy_client_net_get(cli);
   if (!net) return EINA_FALSE;
   code = azy_net_code_get(net);

   if ((code >= 301) && (code <= 303))
     {
        azy_client_connect(cli, cli->secure);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

/**
 * @brief Returns the currently active call id
 *
 * This function returns the currently active #Azy_Client_Call_Id, or 0 if no call
 * is currently active/pending.
 * @param cli The client object (NOT #NULL)
 * @return The currently active/pending call id, or 0 on failure
 */
Azy_Client_Call_Id
azy_client_current(Azy_Client *cli)
{
   Azy_Client_Handler_Data *hd;

   if (!AZY_MAGIC_CHECK(cli, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(cli, AZY_MAGIC_CLIENT);
        return 0;
     }

   if (!cli->conns) return 0;

   hd = cli->conns->data;
   return hd->id;
}

/**
 * @brief Free an #Azy_Client
 *
 * This function frees a client and ALL associated data.  If called
 * on a connected client, azy_client_close will be called and then the client
 * will be freed.
 * @param client The client (NOT #NULL)
 */
void
azy_client_free(Azy_Client *client)
{
   DBG("(client=%p)", client);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_CLIENT);
        return;
     }

   if (client->connected)
     azy_client_close(client);
   AZY_MAGIC_SET(client, AZY_MAGIC_NONE);
   if (client->addr)
     eina_stringshare_del(client->addr);
   if (client->session_id)
     eina_stringshare_del(client->session_id);
   if (client->add)
     ecore_event_handler_del(client->add);
   if (client->del)
     ecore_event_handler_del(client->del);
   if (client->upgrade)
     ecore_event_handler_del(client->upgrade);
   if (client->callbacks)
     eina_hash_free(client->callbacks);
   if (client->free_callbacks)
     eina_hash_free(client->free_callbacks);
   if (client->conns)
     client->conns = eina_list_free(client->conns);
   free(client);
}

/** @} */
