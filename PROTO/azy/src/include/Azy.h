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
#include <Ecore_Con.h>

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

extern int azy_debug_enabled;
extern int azy_log_dom;

extern int AZY_CLIENT_DISCONNECTED;
extern int AZY_CLIENT_CONNECTED;
extern int AZY_CLIENT_RETURN;
extern int AZY_CLIENT_ERROR;

typedef struct _Azy_Server               Azy_Server;
typedef struct _Azy_Server_Module        Azy_Server_Module;
typedef struct _Azy_Server_Module_Method Azy_Server_Module_Method;
typedef struct _Azy_Client Azy_Client;
typedef struct _Azy_Net Azy_Net;
typedef struct _Azy_Server_Module_Def Azy_Server_Module_Def;
typedef struct _Azy_Value Azy_Value;
typedef struct _Azy_Blob Azy_Blob;
typedef struct _Azy_Content Azy_Content;

typedef Eina_Bool (*Azy_Server_Module_Cb)(Azy_Server_Module *);
typedef void (*Azy_Server_Module_Shutdown_Cb)(Azy_Server_Module *);
typedef Eina_Bool (*Azy_Server_Module_Content_Cb)(Azy_Server_Module *, Azy_Content *);
typedef void *(*Azy_Content_Cb)(Azy_Value *, void **);
typedef void (*Azy_Client_Return_Cb)(Azy_Client *, Azy_Content *);

typedef enum
{
   AZY_SERVER_LOCAL = 1,
   AZY_SERVER_BROADCAST = 2,
   AZY_SERVER_TLS = (1 << 4)
} Azy_Server_Type;

typedef enum
{
   AZY_CLIENT_ERROR_MARSHALIZER,
   AZY_CLIENT_ERROR_CLOSED,
   AZY_CLIENT_ERROR_CONNECT,
   AZY_CLIENT_ERROR_IO,
   AZY_CLIENT_ERROR_FAILED
} Azy_Client_Error;

typedef enum {
   AZY_VALUE_ARRAY,
   AZY_VALUE_STRUCT,
   AZY_VALUE_MEMBER,
   AZY_VALUE_INT,
   AZY_VALUE_STRING,
   AZY_VALUE_BOOLEAN,
   AZY_VALUE_DOUBLE,
   AZY_VALUE_TIME,
   AZY_VALUE_BLOB
} Azy_Value_Type;

typedef enum
{
   AZY_NET_TYPE_NONE,
   AZY_NET_TYPE_GET,
   AZY_NET_TYPE_POST,
   AZY_NET_TYPE_PUT,
   AZY_NET_TYPE_RESPONSE,
   AZY_NET_TYPE_RESPONSE_ERROR
} Azy_Net_Type;

typedef enum
{
   AZY_NET_XML,
   AZY_NET_JSON,
   AZY_NET_TEXT,
   AZY_NET_HTML,
   AZY_NET_UNKNOWN
} Azy_Net_Transport;



struct _Azy_Server_Module_Def
{
   const char *name;
   int         data_size;
   Azy_Server_Module_Cb init;
   Azy_Server_Module_Shutdown_Cb shutdown;
   Azy_Server_Module_Content_Cb pre;
   Azy_Server_Module_Content_Cb post;
   Azy_Server_Module_Content_Cb fallback;
   Azy_Server_Module_Cb download;
   Azy_Server_Module_Cb upload;
   Eina_List *methods;
};

struct _Azy_Server_Module_Method
{
   const char *name;
   Azy_Server_Module_Content_Cb method;
};

struct _Azy_Blob
{
   const char *buf;
   int         len;
   char        refs;
};
#ifdef __cplusplus
extern "C" {
#endif

   /* library */
   void azy_init(void);
   void azy_shutdown(void);

   /* utils */
   char *azy_base64_encode(const char *string,
                            double      len);
   char *azy_base64_decode(const char *string,
                            int         len);
   //DOES NOT ALLOC
   unsigned char *azy_rand(unsigned char *buf,
                            int            num);
   uint32_t       azy_rand_uint32(void);
   const char    *azy_str_strip(const char *str);
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
   Azy_Server            *azy_server_new(Eina_Bool   secure);
   void                    azy_server_free(Azy_Server *server);
   Eina_List              *azy_server_module_defs_get(Azy_Server *server);
   Eina_Bool               azy_server_send(Azy_Net      *net,
                                            unsigned char *data,
                                            int            length);
   Eina_Bool azy_server_run(Azy_Server *server,
                             int          type,
                             int          port);
   Eina_Bool azy_server_basic_run(int                      port,
                                   int                      type,
                                   const char              *cert,
                                   Azy_Server_Module_Def **modules);

   /* net */
   Azy_Net   *azy_net_new(void *conn);
   void        azy_net_free(Azy_Net *net);
   const char *azy_net_header_get(Azy_Net   *net,
                                   const char *name);
   void        azy_net_auth_set(Azy_Net   *net,
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
   Azy_Blob  *azy_blob_new(const char *buf,
                             int         len);
   void        azy_blob_unref(Azy_Blob *blob);
   Azy_Blob  *azy_blob_ref(Azy_Blob *blob);
   Azy_Value *azy_value_ref(Azy_Value *val);
   void        azy_value_unref(Azy_Value *val);
   Azy_Value *azy_value_string_new(const char *val);
   Azy_Value *azy_value_int_new(int val);
   Azy_Value *azy_value_bool_new(Eina_Bool val);
   Azy_Value *azy_value_double_new(double val);
   Azy_Value *azy_value_time_new(const char *val);
   Azy_Value *azy_value_blob_new(Azy_Blob *val);
   Eina_Bool   azy_value_to_int(Azy_Value *val,
                                 int        *nval);
   Eina_Bool   azy_value_to_string(Azy_Value  *val,
                                    const char **nval);
   Eina_Bool   azy_value_to_bool(Azy_Value *val,
                                  int        *nval);
   Eina_Bool   azy_value_to_double(Azy_Value *val,
                                    double     *nval);
   Eina_Bool   azy_value_to_time(Azy_Value  *val,
                                  const char **nval);
   Eina_Bool   azy_value_to_blob(Azy_Value *val,
                                  Azy_Blob **nval);
   Eina_Bool   azy_value_to_value(Azy_Value  *val,
                                   Azy_Value **nval);
   Azy_Value_Type azy_value_type_get(Azy_Value *val);
   Azy_Value *azy_value_array_new(void);
   void        azy_value_array_append(Azy_Value *arr,
                                       Azy_Value *val);
   Eina_List  *azy_value_items_get(Azy_Value *arr);
   Azy_Value *azy_value_struct_new(void);
   Azy_Value *azy_value_struct_new_from_string(const char *name,
                                                 const char *value);
   Azy_Value *azy_value_struct_new_from_double(const char *name,
                                                 double      value);
   Azy_Value *azy_value_struct_new_from_int(const char *name,
                                              int         value);
   void        azy_value_struct_member_set(Azy_Value *str,
                                            const char *name,
                                            Azy_Value *val);
   Azy_Value *azy_value_struct_member_get(Azy_Value *str,
                                            const char *name);
   Eina_List  *azy_value_struct_members_get(Azy_Value *str);
   const char *azy_value_struct_member_name_get(Azy_Value *mem);
   Azy_Value *azy_value_struct_member_value_get(Azy_Value *mem);
   Eina_Bool   azy_value_retval_is_error(Azy_Value  *val,
                                          int         *errcode,
                                          const char **errmsg);
   void azy_value_dump(Azy_Value  *v,
                        Eina_Strbuf *string,
                        unsigned int indent);

   /* content */
   Eina_Bool azy_content_serialize_request_xml(void *content);
   Eina_Bool azy_content_serialize_response_xml(void *content);
   Eina_Bool azy_content_unserialize_request_xml(void       *content,
                                                  const char *buf,
                                                  ssize_t     len);
   Eina_Bool azy_content_unserialize_response_xml(void       *content,
                                                   const char *buf,
                                                   ssize_t     len);
   Eina_Bool azy_content_serialize_request_json(void *content);
   Eina_Bool azy_content_serialize_response_json(void *content);
   Eina_Bool azy_content_unserialize_request_json(void       *content,
                                                   const char *buf,
                                                   ssize_t     len);
   Eina_Bool azy_content_unserialize_response_json(void       *content,
                                                    const char *buf,
                                                    ssize_t     len);

   Azy_Content  *azy_content_new(const char *method);
   void           azy_content_free(Azy_Content *content);
   unsigned char *azy_content_buffer_get(Azy_Content *content);
   int            azy_content_length_get(Azy_Content *content);
   Eina_Bool      azy_content_buffer_set(Azy_Content  *content,
                                          unsigned char *buffer,
                                          int            length);
   void        azy_content_buffer_reset(Azy_Content *content);
   Azy_Content_Cb azy_content_callback_get(Azy_Content *content);
   const char *azy_content_method_get(Azy_Content *content);
   const char *azy_content_module_name_get(Azy_Content *content,
                                            const char   *fallback);
   void        azy_content_param_add(Azy_Content *content,
                                      Azy_Value   *val);
   Azy_Value *azy_content_param_get(Azy_Content *content,
                                      unsigned int  pos);
   Eina_List  *azy_content_params_get(Azy_Content *content);
   void        azy_content_retval_set(Azy_Content *content,
                                       Azy_Value   *val);
   void       *azy_content_return_get(Azy_Content *content);
   unsigned int azy_content_id_get(Azy_Content *content);
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
   void        azy_content_dump(const Azy_Content *content,
                                 unsigned int  indent);
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
                                         void        *data);
   Azy_Client     *azy_client_new(void);
   Eina_Bool        azy_client_callback_set(Azy_Client *client,
                                             unsigned int id,
                                             Azy_Client_Return_Cb callback);
   Eina_Bool        azy_client_callback_free_set(Azy_Client *client,
                                                  unsigned int id,
                                                  void (*callback)(void*));
   void             azy_client_free(Azy_Client *client);
   Eina_Bool        azy_client_port_set(Azy_Client *client,
                                         int          port);
   Eina_Bool        azy_client_connected_get(Azy_Client *client);
   Eina_Bool        azy_client_host_set(Azy_Client *client,
                                         const char  *host,
                                         int          port);
   Azy_Net          *azy_client_net_get(Azy_Client *client);
   void               azy_client_net_set(Azy_Client *client,
                                          Azy_Net    *net);
   const char        *azy_client_hostname_get(Azy_Client *client);
   Eina_Bool          azy_client_hostname_set(Azy_Client *client,
                                        const char  *hostname);
   int                azy_client_port_get(Azy_Client *client);
   Eina_Bool          azy_client_connect(Azy_Client *client,
                                          Eina_Bool    secure);
   void               azy_client_close(Azy_Client *client);
   unsigned int       azy_client_call(Azy_Client       *client,
                                       Azy_Content      *content,
                                       Azy_Net_Transport transport,
                                       Azy_Content_Cb    cb);
#ifdef __cplusplus
}
#endif

#endif
