/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#ifndef AZY_PRIV_H
#define AZY_PRIV_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <Eina.h>
#include <Ecore_Con.h>
#include <Azy.h>

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

#define AZY_SERVER_TYPE                0x0f

#define AZY_MAGIC_SERVER               0x31337
#define AZY_MAGIC_SERVER_CLIENT        0x31338
#define AZY_MAGIC_SERVER_MODULE        0x31339
#define AZY_MAGIC_SERVER_MODULE_DEF    0x31340
#define AZY_MAGIC_SERVER_MODULE_METHOD 0x31341
#define AZY_MAGIC_CLIENT               0x31342
#define AZY_MAGIC_NET                  0x31343
#define AZY_MAGIC_VALUE                0x31344
#define AZY_MAGIC_CONTENT              0x31346
#define AZY_MAGIC_CLIENT_DATA_HANDLER  0x31347
#define AZY_MAGIC_RSS                  0x66442
#define AZY_MAGIC_RSS_ITEM             0x66443

#define AZY_MAGIC_NONE                 0x1234fedc
#define AZY_MAGIC                      Azy_Magic __magic
#define AZY_MAGIC_SET(d, m)   (d)->__magic = (m)
#define AZY_MAGIC_CHECK(d, m) ((d) && ((d)->__magic == (m)))
#define AZY_MAGIC_FAIL(d, m)  _azy_magic_fail((d), (d) ? (d)->__magic : 0, (m), __PRETTY_FUNCTION__)

#ifndef __GNUC__
# define __PRETTY_FUNCTION__ __FILE__
#endif
#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

extern int azy_log_dom;
extern int azy_rpc_log_dom;

#define DBG(...)            EINA_LOG_DOM_DBG(azy_log_dom, __VA_ARGS__)
#define INFO(...)           EINA_LOG_DOM_INFO(azy_log_dom, __VA_ARGS__)
#define WARN(...)           EINA_LOG_DOM_WARN(azy_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(azy_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(azy_log_dom, __VA_ARGS__)

#define RPC_DBG(...)            EINA_LOG_DOM_DBG(azy_rpc_log_dom, __VA_ARGS__)
#define RPC_INFO(...)           EINA_LOG_DOM_INFO(azy_rpc_log_dom, __VA_ARGS__)
#define RPC_WARN(...)           EINA_LOG_DOM_WARN(azy_rpc_log_dom, __VA_ARGS__)
#define RPC_ERR(...)            EINA_LOG_DOM_ERR(azy_rpc_log_dom, __VA_ARGS__)
#define RPC_CRI(...)            EINA_LOG_DOM_CRIT(azy_rpc_log_dom, __VA_ARGS__)

#ifndef strdupa
# define strdupa(str)       strcpy(alloca(strlen(str) + 1), str)
#endif

#ifndef strndupa
# define strndupa(str, len) strncpy(alloca(len + 1), str, len)
#endif

extern Eina_Error AZY_ERROR_REQUEST_JSON_OBJECT;
extern Eina_Error AZY_ERROR_REQUEST_JSON_METHOD;
extern Eina_Error AZY_ERROR_REQUEST_JSON_PARAM;

extern Eina_Error AZY_ERROR_RESPONSE_JSON_OBJECT;
extern Eina_Error AZY_ERROR_RESPONSE_JSON_ERROR;
extern Eina_Error AZY_ERROR_RESPONSE_JSON_NULL;
extern Eina_Error AZY_ERROR_RESPONSE_JSON_INVALID;

#ifdef HAVE_XML
extern Eina_Error AZY_ERROR_REQUEST_XML_DOC;
extern Eina_Error AZY_ERROR_REQUEST_XML_ROOT;
extern Eina_Error AZY_ERROR_REQUEST_XML_METHODNAME;
extern Eina_Error AZY_ERROR_REQUEST_XML_PARAM;

extern Eina_Error AZY_ERROR_RESPONSE_XML_DOC;
extern Eina_Error AZY_ERROR_RESPONSE_XML_ROOT;
extern Eina_Error AZY_ERROR_RESPONSE_XML_RETVAL;
extern Eina_Error AZY_ERROR_RESPONSE_XML_MULTI;
extern Eina_Error AZY_ERROR_RESPONSE_XML_FAULT;
extern Eina_Error AZY_ERROR_RESPONSE_XML_INVAL;
extern Eina_Error AZY_ERROR_XML_UNSERIAL;
#else
extern Eina_Error AZY_ERROR_XML_UNSUPPORTED;
#endif

typedef struct Azy_Client_Handler_Data Azy_Client_Handler_Data;
typedef unsigned int                   Azy_Magic;

struct Azy_Content
{
                      AZY_MAGIC;
   void              *data;
   const char        *method;
   Eina_List         *params;
   Azy_Value         *retval;
   void              *ret;
   int64_t            retsize;
   Azy_Client_Call_Id id;
   Azy_Net           *recv_net;
   Azy_Content_Retval_Cb retval_cb;

   unsigned char     *buffer;
   int64_t            length;

   Eina_Bool          error_set : 1;
   Eina_Error         errcode;  //internal code
   int                faultcode;  //code to actually report
   const char        *faultmsg;  //if non-null, message to reply with instead of message associated with errcode
};

struct Azy_Rss
{
   AZY_MAGIC;
   Eina_Bool atom : 1; /* true if item is Azy_Rss_Atom */
   const char *title;
   const char *img_url;

   /* rss format only */
   const char *link;
   const char *desc;

   /* atom format only */
   const char *id;
   const char *subtitle;
   const char *rights;
   const char *logo;
   const char *generator;
   struct tm updated;
   Eina_List *categories;
   Eina_List *contributors;
   Eina_List *authors;
   Eina_List *atom_links;

   Eina_List  *items;
};

struct Azy_Rss_Item
{
   AZY_MAGIC;
   Eina_Bool atom : 1; /* true if item is Azy_Rss_Atom */
   const char *title;

   /* rss format only */
   const char *link;
   const char *desc;
   const char *date;
   const char *guid;
   const char *comment_url;
   const char *author;

   /* atom format only */
   const char *rights;
   const char *summary;
   const char *id;
   const char *icon;
   struct tm updated;
   struct tm published;
   Eina_List *categories;
   Eina_List *contributors;
   Eina_List *authors;
   Eina_List *atom_links;
};

struct Azy_Net
{
                     AZY_MAGIC;
   void             *conn;
   Eina_Bool         server_client : 1;

   int64_t           size;
   unsigned char    *buffer;
   unsigned char    *overflow;
   int64_t           overflow_length;

   Ecore_Timer      *timer;
   Eina_Bool         nodata : 1;

   Azy_Net_Type      type;
   Azy_Net_Transport transport;
   struct
   {
      struct
      {
         const char *http_path;
      } req;

      struct
      {
         const char *http_msg;
         int         http_code;
      } res;
      int        version;
      Eina_Hash *headers;
      int64_t    content_length;
   } http;
   Eina_Bool headers_read : 1;
};

struct Azy_Server
{
                        AZY_MAGIC;
   Ecore_Con_Server    *server;
   Ecore_Event_Handler *add;
   const char          *addr;
   int                  port;
   unsigned long int    clients;

   struct
   {
      Eina_Bool secure : 1;
      Eina_List *cert_files;
   } security;

   Eina_List *module_defs;
};

typedef struct Azy_Server_Client
{
   AZY_MAGIC;
   Ecore_Event_Handler *del;
   Ecore_Event_Handler *data;
   Ecore_Event_Handler *upgrade;

   Azy_Net             *net;
   Azy_Net             *current;
   Azy_Server          *server;
   Eina_List           *modules;

   Eina_Bool            handled : 1;
   Eina_Bool            dead : 1;
   Eina_Bool            resuming : 1;
   Eina_Bool            executing : 1;

   Eina_Bool            suspend : 1;
   Eina_List           *suspended_nets; /* Azy_Net* */
   Azy_Net             *resume;
   Azy_Content         *resume_rpc;
   Eina_Bool            resume_ret : 1;

   const char          *session_id;
   const char          *ip;
} Azy_Server_Client;

typedef enum
{
   AZY_SERVER_MODULE_STATE_INIT,
   AZY_SERVER_MODULE_STATE_PRE,
   AZY_SERVER_MODULE_STATE_METHOD,
   AZY_SERVER_MODULE_STATE_POST,
   AZY_SERVER_MODULE_STATE_ERR
} Azy_Server_Module_State;

struct Azy_Server_Module
{
                           AZY_MAGIC;
   void                   *data;
   Azy_Server_Module_Def  *def;
   Azy_Content            *content;
   Azy_Server_Client      *client;
   Azy_Net_Data            recv;
   Azy_Net                *new_net;
   Eina_Hash              *params;
   Eina_Bool               suspend : 1;
   Eina_Bool               run_method : 1;
   Eina_Bool               post : 1;
   Eina_Bool               rewind : 1;
   Eina_Bool               rewind_now : 1;
   Eina_Bool               executing : 1;
   Azy_Server_Module_State state;
};

struct Azy_Value
{
                  AZY_MAGIC;
   Azy_Value_Type type;
   int            ref;

   const char    *str_val;
   int            int_val;
   double         dbl_val;

   Eina_List     *children;

   const char    *member_name;
   Azy_Value     *member_value;
};

struct Azy_Client
{
                        AZY_MAGIC;
   void                *data;
   Azy_Net             *net;

   Ecore_Event_Handler *add;
   Ecore_Event_Handler *del;
   Ecore_Event_Handler *recv;
   Ecore_Event_Handler *upgrade;

   Eina_List           *conns;
   Eina_Hash           *callbacks;
   Eina_Hash           *free_callbacks;

   const char          *addr;
   int                  port;
   const char          *session_id;
   int                  secure;

   Eina_Bool            connected : 1;
};

struct Azy_Client_Handler_Data
{
                      AZY_MAGIC;
   Azy_Client_Call_Id id;
   Azy_Net_Type       type;
   Azy_Client        *client;
   Azy_Net           *recv;
   const char        *method;
   Azy_Content_Cb     callback;  //callback set to convert from Azy_Value to Return_Type
   void              *content_data;
   Eina_Strbuf       *send;
   Eina_Bool          nodelete : 1;
};

struct Azy_Server_Module_Def
{
                                 AZY_MAGIC;
   const char                   *name;
   int                           data_size;
   double                        version;
   Azy_Server_Module_Cb          init;
   Azy_Server_Module_Shutdown_Cb shutdown;
   Azy_Server_Module_Pre_Cb      pre;
   Azy_Server_Module_Content_Cb  post;
   Azy_Server_Module_Content_Cb  fallback;
   Azy_Server_Module_Cb          download;
   Azy_Server_Module_Cb          upload;
   Eina_List                    *methods;
   Eina_Module                  *module;
};

struct Azy_Server_Module_Method
{
                                AZY_MAGIC;
   const char                  *name;
   Azy_Server_Module_Content_Cb method;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void _azy_magic_fail(const void *d,
                            Azy_Magic   m,
                            Azy_Magic   req_m,
                            const char *fname);

Eina_Bool azy_value_multi_line_get_(Azy_Value *v,
                                    int        max_strlen);
int       azy_events_type_parse(Azy_Net             *net,
                                int                  type,
                                const unsigned char *header,
                                int                  len);
Eina_Bool azy_events_header_parse(Azy_Net       *net,
                                  unsigned char *event_data,
                                  size_t         event_len,
                                  int            offset);
Azy_Net_Transport azy_events_net_transport_get(const char *content_type);
Eina_Bool         azy_events_connection_kill(void       *conn,
                                             Eina_Bool   server_client,
                                             const char *msg);

Eina_Bool _azy_client_handler_add(Azy_Client                 *client,
                                  int                         type,
                                  Ecore_Con_Event_Server_Add *add);
Eina_Bool _azy_client_handler_del(Azy_Client                 *client,
                                  int                         type,
                                  Ecore_Con_Event_Server_Del *del);
Eina_Bool _azy_client_handler_data(Azy_Client_Handler_Data     *handler_data,
                                   int                          type,
                                   Ecore_Con_Event_Server_Data *ev);
Eina_Bool _azy_client_handler_upgrade(Azy_Client_Handler_Data        *hd,
                                      int                             type,
                                      Ecore_Con_Event_Server_Upgrade *ev);

Eina_Bool azy_server_client_handler_add(Azy_Server                 *server,
                                        int                         type,
                                        Ecore_Con_Event_Client_Add *ev);
void _azy_event_handler_fake_free(void *data,
                                  void *data2);

Eina_Bool
azy_content_deserialize(Azy_Content *content,
                        Azy_Net     *net);

Eina_Bool
azy_content_deserialize_json(Azy_Content *content,
                             const char  *buf,
                             ssize_t      len);

Azy_Rss      *azy_rss_new(void);
Azy_Rss_Item *azy_rss_item_new(void);


#ifdef HAVE_XML
Eina_Bool azy_content_serialize_request_xml(Azy_Content *content);
Eina_Bool azy_content_serialize_response_xml(Azy_Content *content);
Eina_Bool azy_content_deserialize_request_xml(Azy_Content *content,
                                              char        *buf,
                                              ssize_t      len);
Eina_Bool azy_content_deserialize_response_xml(Azy_Content *content,
                                               char        *buf,
                                               ssize_t      len);
Eina_Bool azy_content_deserialize_rss_xml(Azy_Content *content,
                                          char        *buf,
                                          ssize_t      len);
Eina_Bool azy_content_deserialize_atom_xml(Azy_Content *content,
                                           char        *buf,
                                           ssize_t      len);
#endif
Eina_Bool azy_content_serialize_request_json(Azy_Content *content);
Eina_Bool azy_content_serialize_response_json(Azy_Content *content);
Eina_Bool azy_content_deserialize_request_json(Azy_Content *content,
                                               const char  *buf,
                                               ssize_t      len);
Eina_Bool azy_content_deserialize_response_json(Azy_Content *content,
                                                const char  *buf,
                                                ssize_t      len);

Eina_Bool azy_content_buffer_set_(Azy_Content   *content,
                                  unsigned char *buffer,
                                  int            length);
#ifdef __cplusplus
}
#endif
#endif
