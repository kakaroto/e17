/* Azy - Lazy RPC library
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AZY_H
#define AZY_H

#include <Eina.h>
#include <Ecore.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

/**
 * @defgroup Azy_Events Azy Events
 * @brief Events that are emitted from the library
 * @{
 */
EAPI extern int AZY_CLIENT_DISCONNECTED; /**< Event emitted upon client disconnecting, sends #Azy_Client object */
EAPI extern int AZY_CLIENT_CONNECTED; /**< Event emitted upon client connecting, sends #Azy_Client object */
EAPI extern int AZY_CLIENT_UPGRADE; /**< Event emitted upon client successfully upgrading to TLS, sends #Azy_Client object */
EAPI extern int AZY_CLIENT_RETURN; /**< Event emitted upon client method returning if
                                   no callback was set, sends #Azy_Content */
EAPI extern int AZY_CLIENT_RESULT; /**< Event emitted upon client method returning if
                                   a callback for the method has been set, sends #Eina_Error */
EAPI extern int AZY_CLIENT_ERROR; /**< Event emitted upon client method encountering
                                  an error, sends #Azy_Content containing error */

EAPI extern int AZY_SERVER_CLIENT_ADD; /**< Event emitted upon client connecting to server,
                                       sends #Azy_Server object */
EAPI extern int AZY_SERVER_CLIENT_UPGRADE; /**< Event emitted upon client successfully upgrading to TLS, sends #Azy_Server_Module object */
EAPI extern int AZY_SERVER_CLIENT_DEL; /**< Event emitted upon client disconnecting from server,
                                       sends #Azy_Server object */

EAPI extern int AZY_EVENT_DOWNLOAD_STATUS; /**< Event emitted when a GET request makes progress, sends #Azy_Event_Download_Status */
/**@}*/
/**
 * @defgroup Azy_Typedefs Azy types
 * @brief These types are used throughout the library
 * @{
 */

/**
 * @typedef Azy_Server
 * A server object for hosting a server, and is used by the
 * azy_server namespace
 */
typedef struct Azy_Server               Azy_Server;
/**
 * @typedef Azy_Server_Module
 * A module object which contains all of the connection info
 * for the associated client
 */
typedef struct Azy_Server_Module        Azy_Server_Module;
/**
 * @typedef Azy_Server_Module_Method
 * A method object containing the name and callback
 */
typedef struct Azy_Server_Module_Method Azy_Server_Module_Method;
/**
 * @typedef Azy_Server_Module_Def
 * A module definition object.  This type is the means by which
 * #Azy_Server_Module objects are created and manipulated by users
 * of the api; contains all methods present in the module
 */
typedef struct Azy_Server_Module_Def    Azy_Server_Module_Def;
/**
 * @typedef Azy_Client
 * A client object for connecting to a server, used by the
 * azy_client namespace
 */
typedef struct Azy_Client               Azy_Client;
/**
 * @typedef Azy_Net
 * A network object containing all connection and http
 * information
 */
typedef struct Azy_Net                  Azy_Net;
/**
 * @typedef Azy_Rss
 * An object representing an RSS feed's content
 */
typedef struct Azy_Rss                  Azy_Rss;
/**
 * @typedef Azy_Rss_Item
 * An object representing a content item from an RSS feed
 */
typedef struct Azy_Rss_Item             Azy_Rss_Item;
/**
 * @typedef Azy_Rss_Link
 * An object representing a link from an RSS feed
 */
typedef struct Azy_Rss_Link
{
   const char *title; /**< The title of the link */
   const char *href; /**< The link URI */
   const char *rel; /**< The relation type */
   const char *type; /**< The content-type */
   const char *hreflang; /**< The language of the URI */
   size_t length; /**< The size of the URI */
} Azy_Rss_Link;
/**
 * @typedef Azy_Rss_Contact
 * An object representing a contact from an RSS feed
 */
typedef struct Azy_Rss_Contact
{
   const char *name; /**< Name of the contact */
   const char *uri; /**< URI associated with the contact */
   const char *email; /**< Email address of the contact */
} Azy_Rss_Contact;
/**
 * @typedef Azy_Value
 * A general struct which can hold any type of value
 */
typedef struct Azy_Value                Azy_Value;
/**
 * @typedef Azy_Content
 * A struct which holds the content being sent/received in an rpc method call
 * in a list of #Azy_Value objects
 */
typedef struct Azy_Content              Azy_Content;
/**
 * @typedef Azy_Client_Call_Id
 * A unique identifier for every azy_client_call and azy_client_put
 * which can be used to set callbacks for the transmission
 */
typedef unsigned int                    Azy_Client_Call_Id;

/**
 * @typedef Azy_Event_Download_Status
 * The event object for AZY_EVENT_DOWNLOAD_STATUS
 */
typedef struct Azy_Event_Download_Status
{
   Azy_Client_Call_Id id; /**< The id of the transfer */
   size_t size; /**< The number of bytes transferred in the event */
   Azy_Net *net; /**< The receiving net object */
   Azy_Client *client; /**< The client making the transfer */
} Azy_Event_Download_Status;

/**
 * @typedef Azy_Server_Type
 * A simple enum for easily specifying the type of server to run
 */
typedef enum
{
   AZY_SERVER_NONE = 0, /**< Server listen address must be set using azy_server_addr_set */
   AZY_SERVER_LOCAL = 1, /**< Server listen address is 127.0.0.1 */
   AZY_SERVER_BROADCAST = 2, /**< Server listen address is 0.0.0.0 */
   AZY_SERVER_TLS = (1 << 4) /**< If bitwise ORed into the type, server will use TLS */
} Azy_Server_Type;

typedef struct
{
   unsigned char *data;
   int64_t        size;
} Azy_Net_Data;

/**
 * @typedef Azy_Client_Error
 * Enum for client errors
 */
/* FIXME: THIS NEEDS TO GO AWAY OR SOMETHING!!!! */
typedef enum
{
   AZY_CLIENT_ERROR_MARSHALIZER,
   AZY_CLIENT_ERROR_CLOSED,
   AZY_CLIENT_ERROR_CONNECT,
   AZY_CLIENT_ERROR_IO,
   AZY_CLIENT_ERROR_FAILED
} Azy_Client_Error;

/**
 * @typedef Azy_Value_Type
 * Represents the type of value stored in the #Azy_Value object
 */
typedef enum {
   AZY_VALUE_ARRAY, /**< Array object */
   AZY_VALUE_STRUCT, /**< Struct object */
   AZY_VALUE_MEMBER, /**< Struct member object */
   AZY_VALUE_INT, /**< Int object */
   AZY_VALUE_STRING, /**< String (stringshared) object */
   AZY_VALUE_BOOL, /**< Boolean object */
   AZY_VALUE_DOUBLE, /**< Double object */
   AZY_VALUE_TIME, /**< Time (stringshared) object */
   AZY_VALUE_BASE64 /**< Base64 encoded string (stringshared) object */
} Azy_Value_Type;

/**
 * @typedef Azy_Net_Type
 * Represents the type of http method in the header
 */
typedef enum
{
   AZY_NET_TYPE_NONE,
   AZY_NET_TYPE_GET,
   AZY_NET_TYPE_POST,
   AZY_NET_TYPE_PUT,
   AZY_NET_TYPE_RESPONSE,
   AZY_NET_TYPE_RESPONSE_ERROR
} Azy_Net_Type;

/**
 * @typedef Azy_Net_Transport
 * Represents the content-type in the http headers
 */
typedef enum
{
   AZY_NET_TRANSPORT_UNKNOWN,
   AZY_NET_TRANSPORT_XML,
   AZY_NET_TRANSPORT_JSON,
   AZY_NET_TRANSPORT_EET,
   AZY_NET_TRANSPORT_TEXT,
   AZY_NET_TRANSPORT_HTML,
   AZY_NET_TRANSPORT_ATOM
} Azy_Net_Transport;

/**
 * @typedef Azy_Server_Module_Cb
 * Function called on client connect (__init__) as well as to serve GET/PUT requests.
 * If #EINA_FALSE is returned in __init__, client will be disconnected immediately.
 * Must return #EINA_FALSE on error to prevent a response in __download__ or __upload__,
 * otherwise response will be sent.
 */
typedef Eina_Bool  (*Azy_Server_Module_Cb)(Azy_Server_Module *);
/**
 * @typedef Azy_Server_Module_Shutdown_Cb
 * Function called when module is unloaded for given connection.
 */
typedef void       (*Azy_Server_Module_Shutdown_Cb)(Azy_Server_Module *);
/**
 * @typedef Azy_Server_Module_Pre_Cb
 * Function called prior to a regular method.  The #Azy_Server_Module object
 * contains the network data for the client, and the #Azy_Net object contains
 * the network data which will be used for sending.
 * Note that after this call, the network data received from the client will no longer be
 * available.
 */
typedef Eina_Bool  (*Azy_Server_Module_Pre_Cb)(Azy_Server_Module *, Azy_Net *);
/**
 * @typedef Azy_Server_Module_Content_Cb
 * Function called after rpc method is handled as well as for fallback method.
 */
typedef Eina_Bool  (*Azy_Server_Module_Content_Cb)(Azy_Server_Module *, Azy_Content *);
/**
 * @typedef Azy_Content_Cb
 * Function to convert Azy_Value* to user type.
 */
typedef void *     (*Azy_Content_Cb)(Azy_Value *, void **);
/**
 * @typedef Azy_Content_Retval_Cb
 * Function to convert user type to Azy_Value*.
 */
typedef Azy_Value *(*Azy_Content_Retval_Cb)(void *);
/**
 * @typedef Azy_Client_Return_Cb
 * Function must return AZY_ERROR_NONE (0) on success, else
 * an error number.
 */
typedef Eina_Error (*Azy_Client_Return_Cb)(Azy_Client *cli, Azy_Content *ret_content, void *ret);

/**
 * @typedef Azy_Server_Module_Def_Cb
 * Function which creates an #Azy_Server_Module_Def, used in azy_server_module_def_load().
 */
typedef Azy_Server_Module_Def *(*Azy_Server_Module_Def_Cb)(void);
#define AZY_ERROR_NONE 0 /**< More explicit define for #Azy_Client_Return_Cb functions. */
/** }@ */
#ifdef __cplusplus
extern "C" {
#endif

   /* library */
   EAPI int  azy_init(void);
   EAPI int  azy_shutdown(void);
   EAPI void azy_rpc_log_enable(void);

   /* utils */
   EAPI char          *azy_base64_encode(const char *string,
                                         double      len);
   EAPI char          *azy_base64_decode(const char *string,
                                         int         len);
   EAPI unsigned char *azy_memstr(const unsigned char *big,
                                  const unsigned char *small,
                                  size_t               big_len,
                                  size_t               small_len);
   EAPI const char *azy_uuid_new(void);

   /* server */
   EAPI void              azy_server_stop(Azy_Server *server);
   EAPI Azy_Server       *azy_server_new(Eina_Bool secure);
   EAPI void              azy_server_free(Azy_Server *server);
   EAPI Eina_Bool         azy_server_cert_add(Azy_Server *server,
                                              const char *cert_file);
   EAPI unsigned long int azy_server_clients_count(Azy_Server *server);
   EAPI Eina_List        *azy_server_module_defs_get(Azy_Server *server);
   EAPI Eina_Bool         azy_server_run(Azy_Server *server);
   EAPI Eina_Bool azy_server_basic_run(int                     port,
                                       int                     type,
                                       const char             *cert,
                                       Azy_Server_Module_Def **modules);
   EAPI Eina_Bool   azy_server_addr_set(Azy_Server *server,
                                        const char *addr);
   EAPI const char *azy_server_addr_get(Azy_Server *server);
   EAPI Eina_Bool   azy_server_port_set(Azy_Server *server,
                                        int         port);
   EAPI int         azy_server_port_get(Azy_Server *server);
   EAPI void        azy_server_ssl_enable(Azy_Server *server);

   /* server module */
   EAPI Eina_Bool                 azy_server_module_send(Azy_Server_Module  *module,
                                                         Azy_Net            *net,
                                                         const Azy_Net_Data *data);
   EAPI void                     *azy_server_module_data_get(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_params_exist(Azy_Server_Module *module);
   EAPI Azy_Net_Data             *azy_server_module_recv_get(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_param_set(Azy_Server_Module *module,
                                                              const char        *name,
                                                              const void        *value,
                                                              Eina_Free_Cb       free_func);
   EAPI void                     *azy_server_module_param_get(Azy_Server_Module *module,
                                                              const char        *name);
   EAPI Azy_Net                  *azy_server_module_net_get(Azy_Server_Module *module);
   EAPI Azy_Server_Module_Def    *azy_server_module_def_find(Azy_Server *server,
                                                             const char *name);
   EAPI void                      azy_server_module_events_resume(Azy_Server_Module *module,
                                                                  Eina_Bool ret);
   EAPI void                      azy_server_module_events_suspend(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_events_rewind(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_events_suspended_get(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_active_get(Azy_Server_Module *module);
   EAPI Azy_Content              *azy_server_module_content_get(Azy_Server_Module *module);
   EAPI Eina_Bool                 azy_server_module_add(Azy_Server            *server,
                                                        Azy_Server_Module_Def *module);
   EAPI Eina_Bool                 azy_server_module_del(Azy_Server            *server,
                                                        Azy_Server_Module_Def *module);
   EAPI Eina_Bool                 azy_server_module_name_del(Azy_Server *server,
                                                             const char *name);
   EAPI Azy_Server_Module_Method *azy_server_module_method_new(const char                  *name,
                                                               Azy_Server_Module_Content_Cb cb);
   EAPI void                      azy_server_module_method_free(Azy_Server_Module_Method *method);
   EAPI Azy_Server_Module_Def    *azy_server_module_def_new(const char *name);
   EAPI Azy_Server_Module_Def    *azy_server_module_def_load(const char *file,
                                                             const char *modname);
   EAPI void                      azy_server_module_def_free(Azy_Server_Module_Def *def);
   EAPI void                      azy_server_module_def_init_shutdown_set(Azy_Server_Module_Def        *def,
                                                                          Azy_Server_Module_Cb          init,
                                                                          Azy_Server_Module_Shutdown_Cb shutdown);
   EAPI void azy_server_module_def_pre_post_set(Azy_Server_Module_Def       *def,
                                                Azy_Server_Module_Pre_Cb     pre,
                                                Azy_Server_Module_Content_Cb post);
   EAPI void azy_server_module_def_download_upload_set(Azy_Server_Module_Def *def,
                                                       Azy_Server_Module_Cb   download,
                                                       Azy_Server_Module_Cb   upload);
   EAPI void      azy_server_module_def_fallback_set(Azy_Server_Module_Def       *def,
                                                     Azy_Server_Module_Content_Cb fallback);
   EAPI void      azy_server_module_def_method_add(Azy_Server_Module_Def    *def,
                                                   Azy_Server_Module_Method *method);
   EAPI Eina_Bool azy_server_module_def_method_del(Azy_Server_Module_Def    *def,
                                                   Azy_Server_Module_Method *method);
   EAPI int       azy_server_module_def_size_get(Azy_Server_Module_Def *def);
   EAPI Eina_Bool azy_server_module_size_set(Azy_Server_Module_Def *def,
                                             int                    size);
   EAPI double    azy_server_module_version_get(Azy_Server_Module *m);
   EAPI void      azy_server_module_def_version_set(Azy_Server_Module_Def *def,
                                                    double                 version);
   EAPI Eina_Bool azy_server_module_session_set(Azy_Server_Module *module,
                                                const char *sessid);
   EAPI const char *azy_server_module_session_get(Azy_Server_Module *module);
   EAPI Eina_Bool   azy_server_module_upgrade(Azy_Server_Module *module);

   /* net */
   EAPI Azy_Net    *azy_net_new(void *conn);
   EAPI void        azy_net_free(Azy_Net *net);
   EAPI const char *azy_net_header_get(Azy_Net    *net,
                                       const char *name);
   EAPI Eina_Bool   azy_net_auth_set(Azy_Net    *net,
                                     const char *username,
                                     const char *password);
   EAPI Eina_Bool azy_net_auth_get(Azy_Net     *net,
                                   const char **username,
                                   const char **password);
   EAPI const char  *azy_net_uri_get(Azy_Net *net);
   EAPI Eina_Bool    azy_net_uri_set(Azy_Net    *net,
                                     const char *path);
   EAPI int          azy_net_version_get(Azy_Net *net);
   EAPI Eina_Bool    azy_net_version_set(Azy_Net *net,
                                         int      version);
   EAPI int          azy_net_code_get(Azy_Net *net);
   EAPI void         azy_net_code_set(Azy_Net *net,
                                      int      code);
   EAPI const char  *azy_net_ip_get(Azy_Net *net);
   EAPI Azy_Net_Type azy_net_type_get(Azy_Net *net);
   EAPI int          azy_net_message_length_get(Azy_Net *net);
   EAPI void         azy_net_header_set(Azy_Net    *net,
                                        const char *name,
                                        const char *value);
   EAPI void              azy_net_header_reset(Azy_Net *net);
   EAPI void              azy_net_type_set(Azy_Net     *net,
                                           Azy_Net_Type type);
   EAPI void              azy_net_transport_set(Azy_Net          *net,
                                                Azy_Net_Transport transport);
   EAPI Azy_Net_Transport azy_net_transport_get(Azy_Net *net);
   EAPI void              azy_net_message_length_set(Azy_Net *net,
                                                     int      length);
   EAPI Eina_Strbuf      *azy_net_header_create(Azy_Net *net);
   EAPI const char       *azy_net_http_msg_get(int code);

   /* values */
   EAPI Azy_Value     *azy_value_ref(Azy_Value *val);
   EAPI void           azy_value_unref(Azy_Value *val);
   EAPI Azy_Value     *azy_value_base64_new(const char *val);
   EAPI Azy_Value     *azy_value_string_new(const char *val);
   EAPI Azy_Value     *azy_value_int_new(int val);
   EAPI Azy_Value     *azy_value_bool_new(Eina_Bool val);
   EAPI Azy_Value     *azy_value_double_new(double val);
   EAPI Azy_Value     *azy_value_time_new(const char *val);
   EAPI Eina_Bool      azy_value_int_get(Azy_Value *val,
                                         int       *nval);
   EAPI Eina_Bool      azy_value_string_get(Azy_Value   *val,
                                            const char **nval);
   EAPI Eina_Bool      azy_value_base64_get(Azy_Value   *val,
                                            const char **nval);
   EAPI Eina_Bool      azy_value_bool_get(Azy_Value *val,
                                          Eina_Bool *nval);
   EAPI Eina_Bool      azy_value_double_get(Azy_Value *val,
                                            double    *nval);
   EAPI Eina_Bool      azy_value_value_get(Azy_Value  *val,
                                           Azy_Value **nval);
   EAPI Azy_Value_Type azy_value_type_get(Azy_Value *val);
   EAPI void           azy_value_type_set(Azy_Value *val,
                                          Azy_Value_Type type);
   EAPI Azy_Value     *azy_value_array_new(void);
   EAPI void           azy_value_array_push(Azy_Value *arr,
                                            Azy_Value *val);
   EAPI Eina_List     *azy_value_children_items_get(Azy_Value *arr);
   EAPI Azy_Value     *azy_value_struct_new(void);
   EAPI void           azy_value_struct_member_set(Azy_Value  *str,
                                                   const char *name,
                                                   Azy_Value  *val);
   EAPI Azy_Value  *azy_value_struct_member_get(Azy_Value  *str,
                                                const char *name);
   EAPI const char *azy_value_struct_member_name_get(Azy_Value *mem);
   EAPI Azy_Value  *azy_value_struct_member_value_get(Azy_Value *mem);
   EAPI Eina_Bool   azy_value_retval_is_error(Azy_Value   *val,
                                              int         *errcode,
                                              const char **errmsg);
   EAPI void azy_value_dump(Azy_Value   *v,
                            Eina_Strbuf *string,
                            unsigned int indent);

   /* content */
   EAPI Azy_Content       *azy_content_new(const char *method);
   EAPI void               azy_content_free(Azy_Content *content);
   EAPI Azy_Content_Cb     azy_content_callback_get(Azy_Content *content);
   EAPI const char        *azy_content_method_get(Azy_Content *content);
   EAPI const char        *azy_content_method_full_get(Azy_Content *content);
   EAPI const char        *azy_content_module_name_get(Azy_Content *content,
                                                       const char  *fallback);
   EAPI Azy_Net           *azy_content_net_get(Azy_Content *content);
   EAPI void               azy_content_param_add(Azy_Content *content,
                                                 Azy_Value   *val);
   EAPI Azy_Value         *azy_content_param_get(Azy_Content *content,
                                                 unsigned int pos);
   EAPI Eina_List         *azy_content_params_get(Azy_Content *content);
   EAPI void               azy_content_retval_set(Azy_Content *content,
                                                  Azy_Value   *val);
   EAPI void              *azy_content_return_get(Azy_Content *content);
   EAPI uint64_t           azy_content_return_size_get(Azy_Content *content);
   EAPI Azy_Client_Call_Id azy_content_id_get(Azy_Content *content);
   EAPI Azy_Value         *azy_content_retval_get(Azy_Content *content);
   EAPI void               azy_content_error_code_set(Azy_Content *content,
                                                      Eina_Error   code);
   EAPI void               azy_content_error_faultcode_set(Azy_Content *content,
                                                           Eina_Error   code,
                                                           int          faultcode);
   EAPI void azy_content_error_faultmsg_set(Azy_Content *content,
                                            int          faultcode,
                                            const char  *fmt,
                                            ...);
   EAPI Eina_Bool   azy_content_error_is_set(Azy_Content *content);
   EAPI void        azy_content_error_reset(Azy_Content *content);
   EAPI Eina_Error  azy_content_error_code_get(Azy_Content *content);
   EAPI const char *azy_content_error_message_get(Azy_Content *content);
   EAPI void        azy_content_error_copy(Azy_Content *from,
                                           Azy_Content *to);
   EAPI char       *azy_content_dump_string(const Azy_Content *content,
                                            unsigned int       indent);
   EAPI void        azy_content_data_set(Azy_Content *content,
                                         const void  *data);
   EAPI void       *azy_content_data_get(Azy_Content *content);
   EAPI void        azy_content_retval_cb_set(Azy_Content *content,
                                              Azy_Content_Retval_Cb cb);
   EAPI Azy_Content_Retval_Cb azy_content_retval_cb_get(Azy_Content *content);
   EAPI Eina_Bool   azy_content_serialize_request(Azy_Content      *content,
                                                  Azy_Net_Transport type);
   EAPI Eina_Bool   azy_content_deserialize_request(Azy_Content      *content,
                                                    Azy_Net_Transport type,
                                                    char             *buf,
                                                    ssize_t           len);
   EAPI Eina_Bool azy_content_serialize_response(Azy_Content      *content,
                                                 Azy_Net_Transport type);
   EAPI Eina_Bool azy_content_deserialize_response(Azy_Content      *content,
                                                   Azy_Net_Transport type,
                                                   char             *buf,
                                                   ssize_t           len);

   /* client */
   EAPI void       *azy_client_data_get(Azy_Client *client);
   EAPI void        azy_client_data_set(Azy_Client *client,
                                        const void *data);
   EAPI Azy_Client *azy_client_new(void);
   EAPI Eina_Bool   azy_client_callback_set(Azy_Client          *client,
                                            Azy_Client_Call_Id   id,
                                            Azy_Client_Return_Cb callback);
   EAPI Eina_Bool azy_client_callback_free_set(Azy_Client        *client,
                                               Azy_Client_Call_Id id,
                                               Ecore_Cb           callback);
   EAPI void      azy_client_free(Azy_Client *client);
   EAPI Eina_Bool azy_client_port_set(Azy_Client *client,
                                      int         port);
   EAPI Eina_Bool azy_client_connected_get(Azy_Client *client);
   EAPI Eina_Bool azy_client_host_set(Azy_Client *client,
                                      const char *addr,
                                      int         port);
   EAPI Azy_Net    *azy_client_net_get(Azy_Client *client);
   EAPI void        azy_client_net_set(Azy_Client *client,
                                       Azy_Net    *net);
   EAPI const char *azy_client_addr_get(Azy_Client *client);
   EAPI Eina_Bool   azy_client_addr_set(Azy_Client *client,
                                        const char *addr);
   EAPI int         azy_client_port_get(Azy_Client *client);
   EAPI Eina_Bool   azy_client_connect(Azy_Client *client,
                                       Eina_Bool   secure);
   EAPI void        azy_client_close(Azy_Client *client);
   EAPI Eina_Bool   azy_client_call_checker(Azy_Client          *cli,
                                            Azy_Content         *err_content,
                                            Azy_Client_Call_Id   ret,
                                            Azy_Client_Return_Cb cb,
                                            const char          *func);
   EAPI Azy_Client_Call_Id azy_client_call(Azy_Client       *client,
                                           Azy_Content      *content,
                                           Azy_Net_Transport transport,
                                           Azy_Content_Cb    cb);
   EAPI Azy_Client_Call_Id azy_client_blank(Azy_Client    *client,
                                            Azy_Net_Type   type,
                                            Azy_Net_Data  *netdata,
                                            Azy_Content_Cb cb,
                                            void          *data);
   EAPI Azy_Client_Call_Id azy_client_put(Azy_Client         *client,
                                          const Azy_Net_Data *send,
                                          void               *data);
   EAPI Eina_Bool          azy_client_redirect(Azy_Client *cli);
   EAPI Azy_Client_Call_Id azy_client_current(Azy_Client *cli);
   EAPI Eina_Bool          azy_server_module_upgrade(Azy_Server_Module *module);

   /* RSS */
   EAPI void          azy_rss_free(Azy_Rss *rss);
   EAPI void          azy_rss_link_free(Azy_Rss_Link *li);
   EAPI void          azy_rss_contact_free(Azy_Rss_Contact *c);
   EAPI void          azy_rss_item_free(Azy_Rss_Item *item);
   EAPI Eina_List    *azy_rss_items_get(Azy_Rss *rss);
   EAPI Eina_List    *azy_rss_authors_get(Azy_Rss *rss);
   EAPI Eina_List    *azy_rss_contributors_get(Azy_Rss *rss);
   EAPI Eina_List    *azy_rss_categories_get(Azy_Rss *rss);
   EAPI Eina_List    *azy_rss_links_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_title_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_link_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_img_url_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_desc_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_rights_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_id_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_logo_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_generator_get(Azy_Rss *rss);
   EAPI const char   *azy_rss_subtitle_get(Azy_Rss *rss);
   EAPI Eina_List    *azy_rss_item_authors_get(Azy_Rss_Item *item);
   EAPI Eina_List    *azy_rss_item_contributors_get(Azy_Rss_Item *item);
   EAPI Eina_List    *azy_rss_item_categories_get(Azy_Rss_Item *item);
   EAPI Eina_List    *azy_rss_item_links_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_title_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_link_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_desc_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_date_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_guid_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_comment_url_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_author_get(Azy_Rss_Item *item);
   EAPI const char   *azy_rss_item_rights_get(Azy_Rss_Item *rss);
   EAPI const char   *azy_rss_item_summary_get(Azy_Rss_Item *rss);
   EAPI const char   *azy_rss_item_id_get(Azy_Rss_Item *rss);
   EAPI void          azy_rss_print(const char *pre,
                                    int         indent,
                                    Azy_Rss    *rss);
   EAPI void          azy_rss_link_print(const char *pre,
                                         int         indent,
                                         Azy_Rss_Link *li);
   EAPI void          azy_rss_contact_print(const char *pre,
                                            int         indent,
                                            Azy_Rss_Contact *c);
   EAPI void azy_rss_item_print(const char   *pre,
                                int           indent,
                                Azy_Rss_Item *item);
#ifdef __cplusplus
}
#endif

#endif
