/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifndef AZY_H
#define AZY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#else
#include <stddef.h>
void *alloca (size_t);
#endif

#include <Eina.h>
#include <Ecore.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_ECORE_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_ECORE_BUILD */
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
 * Convenience define for Azy_Client_Return_Cb functions.
 */
#define AZY_ERROR_NONE 0

extern int azy_log_dom;

extern int AZY_CLIENT_DISCONNECTED; /**< Event emitted upon client disconnecting */
extern int AZY_CLIENT_CONNECTED; /**< Event emitted upon client connecting */
extern int AZY_CLIENT_RESULT; /**< Event emitted upon client method returning if
                                   no callback was set */
extern int AZY_CLIENT_RETURN; /**< Event emitted upon client method returning if
                                   a callback for the method has been set */
extern int AZY_CLIENT_ERROR; /**< Event emitted upon client method encountering
                                  an error */

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
typedef struct Azy_Server_Module_Def Azy_Server_Module_Def;
/**
 * @typedef Azy_Client
 * A client object for connecting to a server, used by the
 * azy_client namespace
 */
typedef struct Azy_Client Azy_Client;
/**
 * @typedef Azy_Net
 * A network object containing all connection and http
 * information
 */
typedef struct Azy_Net Azy_Net;
/**
 * @typedef Azy_Value
 * A general struct which can hold any type of value
 */
typedef struct Azy_Value Azy_Value;
/**
 * @typedef Azy_Content
 * A struct which holds the content being sent/received in an rpc method call
 * in a list of #Azy_Value objects
 */
typedef struct Azy_Content Azy_Content;
/**
 * @typedef Azy_Client_Call_Id
 * A unique identifier for every azy_client_call and azy_client_send
 * which can be used to set callbacks for the transmission
 */
typedef unsigned int Azy_Client_Call_Id;

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
   AZY_VALUE_STRUCT, /** Struct object */
   AZY_VALUE_MEMBER, /** Struct member object */
   AZY_VALUE_INT, /** Int object */
   AZY_VALUE_STRING, /** String (stringshared) object */
   AZY_VALUE_BOOL, /** Boolean object */
   AZY_VALUE_DOUBLE, /** Double object */
   AZY_VALUE_TIME, /** Time (stringshared) object */
   AZY_VALUE_BASE64 /** Base64 encoded string (stringshared) object */
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
   AZY_NET_TRANSPORT_XML,
   AZY_NET_TRANSPORT_JSON,
   AZY_NET_TRANSPORT_TEXT,
   AZY_NET_TRANSPORT_HTML,
   AZY_NET_TRANSPORT_UNKNOWN
} Azy_Net_Transport;


typedef Eina_Bool (*Azy_Server_Module_Cb)(Azy_Server_Module *);
typedef void (*Azy_Server_Module_Shutdown_Cb)(Azy_Server_Module *);
typedef Eina_Bool (*Azy_Server_Module_Content_Cb)(Azy_Server_Module *, Azy_Content *);
typedef void *(*Azy_Content_Cb)(Azy_Value *, void **);
/**
 * @typedef Azy_Client_Return_Cb
 * Function must return AZY_ERROR_NONE (0) on success, else
 * an error number.
 */
typedef Eina_Error (*Azy_Client_Return_Cb)(Azy_Client *, Azy_Content *);
/** }@ */
#ifdef __cplusplus
extern "C" {
#endif

   /* library */
   int azy_init(void);
   int azy_shutdown(void);

   /* utils */
   char *azy_base64_encode(const char *string,
                            double      len);
   char *azy_base64_decode(const char *string,
                            int         len);
   unsigned char *azy_memstr(const unsigned char *big,
                              const unsigned char *small,
                              size_t               big_len,
                              size_t               small_len);
   const char *   azy_uuid_new(void);

   /* server */
   void                    azy_server_stop(Azy_Server *server);
   void                   *azy_server_module_data_get(Azy_Server_Module *module);
   Azy_Net               *azy_server_module_net_get(Azy_Server_Module *module);
   Azy_Server_Module_Def *azy_server_module_def_find(Azy_Server *server,
                                                       const char  *name);
   Eina_Bool               azy_server_module_add(Azy_Server            *server,
                                                  Azy_Server_Module_Def *module);
   Eina_Bool               azy_server_module_del(Azy_Server            *server,
                                                  Azy_Server_Module_Def *module);
   Azy_Server_Module_Method *azy_server_module_method_new(const char *name,
                                                          Azy_Server_Module_Content_Cb cb);
   void                      azy_server_module_method_free(Azy_Server_Module_Method *method);
   Azy_Server_Module_Def    *azy_server_module_def_new(const char *name);
   void                      azy_server_module_def_free(Azy_Server_Module_Def *def);
   void                      azy_server_module_def_init_shutdown_set(Azy_Server_Module_Def *def,
                                                                     Azy_Server_Module_Cb init,
                                                                     Azy_Server_Module_Shutdown_Cb shutdown);
   void                      azy_server_module_def_pre_post_set(Azy_Server_Module_Def *def,
                                                                Azy_Server_Module_Content_Cb pre,
                                                                Azy_Server_Module_Content_Cb post);
   void                      azy_server_module_def_download_upload_set(Azy_Server_Module_Def *def,
                                                                       Azy_Server_Module_Cb download,
                                                                       Azy_Server_Module_Cb upload);
   void                      azy_server_module_def_fallback_set(Azy_Server_Module_Def *def,
                                                                Azy_Server_Module_Content_Cb fallback);
   void                      azy_server_module_def_method_add(Azy_Server_Module_Def *def,
                                                              Azy_Server_Module_Method *method);
   Eina_Bool                 azy_server_module_def_method_del(Azy_Server_Module_Def    *def,
                                                              Azy_Server_Module_Method *method);
   int                       azy_server_module_def_size_get(Azy_Server_Module_Def *def);
   Eina_Bool                 azy_server_module_size_set(Azy_Server_Module_Def *def,
                                                        int                    size);
   Azy_Server            *azy_server_new(Eina_Bool   secure);
   void                    azy_server_free(Azy_Server *server);
   Eina_List              *azy_server_module_defs_get(Azy_Server *server);
   Eina_Bool               azy_server_client_send(Azy_Net      *net,
                                            unsigned char *data,
                                            int            length);
   Eina_Bool azy_server_run(Azy_Server *server,
                             int          type,
                             int          port);
   Eina_Bool azy_server_basic_run(int                      port,
                                   int                      type,
                                   const char              *cert,
                                   Azy_Server_Module_Def **modules);
   Eina_Bool azy_server_addr_set(Azy_Server *server,
                                 const char *addr);
   const char *azy_server_addr_get(Azy_Server *server);

   /* net */
   Azy_Net   *azy_net_new(void *conn);
   void        azy_net_free(Azy_Net *net);
   const char *azy_net_header_get(Azy_Net   *net,
                                   const char *name);
   Eina_Bool     azy_net_auth_set(Azy_Net   *net,
                                 const char *username,
                                 const char *password);
   Eina_Bool azy_net_auth_get(Azy_Net    *net,
                               const char **username,
                               const char **password);
   const char   *azy_net_uri_get(Azy_Net *net);
   Eina_Bool     azy_net_uri_set(Azy_Net *net,
                                  const char *path);
   int           azy_net_version_get(Azy_Net *net);
   Eina_Bool     azy_net_version_set(Azy_Net *net,
                                      int version);
   int           azy_net_code_get(Azy_Net *net);
   void          azy_net_code_set(Azy_Net *net,
                                   int       code);
   const char   *azy_net_ip_get(Azy_Net *net);
   Azy_Net_Type azy_net_type_get(Azy_Net *net);
   int           azy_net_message_length_get(Azy_Net *net);
   void          azy_net_header_set(Azy_Net   *net,
                                     const char *name,
                                     const char *value);
   void         azy_net_header_reset(Azy_Net *net);
   void         azy_net_type_set(Azy_Net     *net,
                                  Azy_Net_Type type);
   void         azy_net_transport_set(Azy_Net          *net,
                                       Azy_Net_Transport transport);
   Azy_Net_Transport azy_net_transport_get(Azy_Net *net);
   void         azy_net_message_length_set(Azy_Net *net,
                                            int       length);
   Eina_Strbuf *azy_net_header_create(Azy_Net *net);
   const char  *azy_net_http_msg_get(int code);
   Eina_Bool    azy_net_send(Azy_Net      *net,
                              unsigned char *data,
                              int            length);


   /* values */
   Azy_Value *azy_value_ref(Azy_Value *val);
   void        azy_value_unref(Azy_Value *val);
   Azy_Value *azy_value_base64_new(const char *val);
   Azy_Value *azy_value_string_new(const char *val);
   Azy_Value *azy_value_int_new(int val);
   Azy_Value *azy_value_bool_new(Eina_Bool val);
   Azy_Value *azy_value_double_new(double val);
   Azy_Value *azy_value_time_new(const char *val);
   Eina_Bool   azy_value_int_get(Azy_Value *val,
                                 int       *nval);
   Eina_Bool   azy_value_string_get(Azy_Value  *val,
                                    const char **nval);
   Eina_Bool   azy_value_base64_get(Azy_Value  *val,
                                    const char **nval);
   Eina_Bool   azy_value_bool_get(Azy_Value *val,
                                  Eina_Bool *nval);
   Eina_Bool   azy_value_double_get(Azy_Value *val,
                                    double     *nval);
   Eina_Bool   azy_value_value_get(Azy_Value  *val,
                                   Azy_Value **nval);
   Azy_Value_Type azy_value_type_get(Azy_Value *val);
   Azy_Value  *azy_value_array_new(void);
   void        azy_value_array_push(Azy_Value *arr,
                                    Azy_Value *val);
   Eina_List  *azy_value_children_items_get(Azy_Value *arr);
   Azy_Value  *azy_value_struct_new(void);
   void        azy_value_struct_member_set(Azy_Value *str,
                                            const char *name,
                                            Azy_Value *val);
   Azy_Value *azy_value_struct_member_get(Azy_Value *str,
                                            const char *name);
   const char *azy_value_struct_member_name_get(Azy_Value *mem);
   Azy_Value *azy_value_struct_member_value_get(Azy_Value *mem);
   Eina_Bool   azy_value_retval_is_error(Azy_Value  *val,
                                          int         *errcode,
                                          const char **errmsg);
   void azy_value_dump(Azy_Value  *v,
                        Eina_Strbuf *string,
                        unsigned int indent);

   /* content */
   Azy_Content  *azy_content_new(const char *method);
   void           azy_content_free(Azy_Content *content);
   Azy_Content_Cb azy_content_callback_get(Azy_Content *content);
   const char *azy_content_method_get(Azy_Content *content);
   const char *azy_content_method_full_get(Azy_Content *content);
   const char *azy_content_module_name_get(Azy_Content *content,
                                            const char   *fallback);
   Azy_Net    *azy_content_net_get(Azy_Content *content);
   void        azy_content_param_add(Azy_Content *content,
                                      Azy_Value   *val);
   Azy_Value *azy_content_param_get(Azy_Content *content,
                                      unsigned int  pos);
   Eina_List  *azy_content_params_get(Azy_Content *content);
   void        azy_content_retval_set(Azy_Content *content,
                                       Azy_Value   *val);
   void       *azy_content_return_get(Azy_Content *content);
   Azy_Client_Call_Id azy_content_id_get(Azy_Content *content);
   Azy_Value *azy_content_retval_get(Azy_Content *content);
   void        azy_content_error_code_set(Azy_Content *content,
                                           Eina_Error    code);
   void        azy_content_error_faultcode_set(Azy_Content *content,
                                                Eina_Error    code,
                                                int           faultcode);
   void azy_content_error_faultmsg_set(Azy_Content *content,
                                        int           faultcode,
                                        const char   *fmt,
                                        ...);
   Eina_Bool   azy_content_error_is_set(Azy_Content *content);
   void        azy_content_error_reset(Azy_Content *content);
   Eina_Error  azy_content_error_code_get(Azy_Content *content);
   const char *azy_content_error_message_get(Azy_Content *content);
   char       *azy_content_dump_string(const Azy_Content *content,
                                        unsigned int  indent);
   void        azy_content_data_set(Azy_Content *content,
                                    const void  *data);
   void       *azy_content_data_get(Azy_Content *content);
   Eina_Bool   azy_content_serialize_request(Azy_Content *content,
                                              Azy_Net_Transport type);
   Eina_Bool   azy_content_unserialize_request(Azy_Content *content,
                                                Azy_Net_Transport type,
                                                const char   *buf,
                                                ssize_t       len);
   Eina_Bool azy_content_serialize_response(Azy_Content *content,
                                             Azy_Net_Transport type);
   Eina_Bool azy_content_unserialize_response(Azy_Content *content,
                                               Azy_Net_Transport type,
                                               const char   *buf,
                                               ssize_t       len);

   /* client */
   void            *azy_client_data_get(Azy_Client *client);
   void             azy_client_data_set(Azy_Client *client,
                                        const void *data);
   Azy_Client     *azy_client_new(void);
   Eina_Bool        azy_client_callback_set(Azy_Client *client,
                                             Azy_Client_Call_Id id,
                                             Azy_Client_Return_Cb callback);
   Eina_Bool        azy_client_callback_free_set(Azy_Client *client,
                                                  Azy_Client_Call_Id id,
                                                  Ecore_Cb callback);
   void             azy_client_free(Azy_Client *client);
   Eina_Bool        azy_client_port_set(Azy_Client *client,
                                         int          port);
   Eina_Bool        azy_client_connected_get(Azy_Client *client);
   Eina_Bool        azy_client_host_set(Azy_Client *client,
                                         const char  *addr,
                                         int          port);
   Azy_Net          *azy_client_net_get(Azy_Client *client);
   void               azy_client_net_set(Azy_Client *client,
                                          Azy_Net    *net);
   const char        *azy_client_addr_get(Azy_Client *client);
   Eina_Bool          azy_client_addr_set(Azy_Client *client,
                                        const char  *addr);
   int                azy_client_port_get(Azy_Client *client);
   Eina_Bool          azy_client_connect(Azy_Client *client,
                                          Eina_Bool    secure);
   void               azy_client_close(Azy_Client *client);
   Azy_Client_Call_Id azy_client_call(Azy_Client       *client,
                                       Azy_Content      *content,
                                       Azy_Net_Transport transport,
                                       Azy_Content_Cb    cb);
   Azy_Client_Call_Id azy_client_send(Azy_Client   *client,
                                      unsigned char *data,
                                      int            length);
#ifdef __cplusplus
}
#endif

#endif
