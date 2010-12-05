/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include <Azy.h>
#include <azy_private.h>

static const char AZY_ERROR_REQUEST_JSON_OBJECT_err[] = "Can't parse JSON-RPC request. Invalid JSON object.";
static const char AZY_ERROR_REQUEST_JSON_METHOD_err[] = "Can't parse JSON-RPC request. Missing method.";
static const char AZY_ERROR_REQUEST_JSON_PARAM_err[] = "Can't parse JSON-RPC request. Failed to unserialize a parameter.";

static const char AZY_ERROR_RESPONSE_JSON_OBJECT_err[] = "Can't parse JSON-RPC response. Invalid JSON object.";
static const char AZY_ERROR_RESPONSE_JSON_ERROR_err[] = "Can't parse JSON-RPC response. Invalid error object.";
static const char AZY_ERROR_RESPONSE_JSON_NULL_err[] = "Can't parse JSON-RPC response. Null result.";
static const char AZY_ERROR_RESPONSE_JSON_INVALID_err[] = "Can't parse JSON-RPC response. Invalid result.";

static const char AZY_ERROR_REQUEST_XML_DOC_err[] = "Can't parse XML-RPC XML request. Invalid XML document.";
static const char AZY_ERROR_REQUEST_XML_ROOT_err[] = "Can't parse XML-RPC XML request. Root element is missing.";
static const char AZY_ERROR_REQUEST_XML_METHODNAME_err[] = "Can't parse XML-RPC XML request. Missing methodName.";
static const char AZY_ERROR_REQUEST_XML_PARAM_err[] = "Can't parse XML-RPC XML request. Failed to unserialize a parameter.";

static const char AZY_ERROR_RESPONSE_XML_DOC_err[] = "Can't parse XML-RPC XML response. Invalid XML document.";
static const char AZY_ERROR_RESPONSE_XML_ROOT_err[] = "Can't parse XML-RPC XML response. Root element is missing.";
static const char AZY_ERROR_RESPONSE_XML_RETVAL_err[] = "Can't parse XML-RPC XML response. Failed to unserialize retval.";
static const char AZY_ERROR_RESPONSE_XML_MULTI_err[] = "Can't parse XML-RPC XML response. Too many return values.";
static const char AZY_ERROR_RESPONSE_XML_FAULT_err[] = "Can't parse XML-RPC XML response. Failed to unserialize fault response.";
static const char AZY_ERROR_RESPONSE_XML_INVAL_err[] = "Can't parse XML-RPC XML response. Invalid fault response.";
static const char AZY_ERROR_RESPONSE_XML_UNSERIAL_err[] = "Can't parse XML-RPC XML response. Failed to unserialize retval.";

Eina_Error AZY_ERROR_REQUEST_JSON_OBJECT;
Eina_Error AZY_ERROR_REQUEST_JSON_METHOD;
Eina_Error AZY_ERROR_REQUEST_JSON_PARAM;

Eina_Error AZY_ERROR_RESPONSE_JSON_OBJECT;
Eina_Error AZY_ERROR_RESPONSE_JSON_ERROR;
Eina_Error AZY_ERROR_RESPONSE_JSON_NULL;
Eina_Error AZY_ERROR_RESPONSE_JSON_INVALID;

Eina_Error AZY_ERROR_REQUEST_XML_DOC;
Eina_Error AZY_ERROR_REQUEST_XML_ROOT;
Eina_Error AZY_ERROR_REQUEST_XML_METHODNAME;
Eina_Error AZY_ERROR_REQUEST_XML_PARAM;

Eina_Error AZY_ERROR_RESPONSE_XML_DOC;
Eina_Error AZY_ERROR_RESPONSE_XML_ROOT;
Eina_Error AZY_ERROR_RESPONSE_XML_RETVAL;
Eina_Error AZY_ERROR_RESPONSE_XML_MULTI;
Eina_Error AZY_ERROR_RESPONSE_XML_FAULT;
Eina_Error AZY_ERROR_RESPONSE_XML_INVAL;
Eina_Error AZY_ERROR_RESPONSE_XML_UNSERIAL;

int azy_log_dom = -1;
static int azy_init_count_ = 0;

static void
azy_lib_register_errors_(void)
{
   AZY_ERROR_REQUEST_JSON_OBJECT = eina_error_msg_static_register(AZY_ERROR_REQUEST_JSON_OBJECT_err);
   AZY_ERROR_REQUEST_JSON_METHOD = eina_error_msg_static_register(AZY_ERROR_REQUEST_JSON_METHOD_err);
   AZY_ERROR_REQUEST_JSON_PARAM = eina_error_msg_static_register(AZY_ERROR_REQUEST_JSON_PARAM_err);

   AZY_ERROR_RESPONSE_JSON_OBJECT = eina_error_msg_static_register(AZY_ERROR_RESPONSE_JSON_OBJECT_err);
   AZY_ERROR_RESPONSE_JSON_ERROR = eina_error_msg_static_register(AZY_ERROR_RESPONSE_JSON_ERROR_err);
   AZY_ERROR_RESPONSE_JSON_NULL = eina_error_msg_static_register(AZY_ERROR_RESPONSE_JSON_NULL_err);
   AZY_ERROR_RESPONSE_JSON_INVALID = eina_error_msg_static_register(AZY_ERROR_RESPONSE_JSON_INVALID_err);

   AZY_ERROR_REQUEST_XML_DOC = eina_error_msg_static_register(AZY_ERROR_REQUEST_XML_DOC_err);
   AZY_ERROR_REQUEST_XML_ROOT = eina_error_msg_static_register(AZY_ERROR_REQUEST_XML_ROOT_err);
   AZY_ERROR_REQUEST_XML_METHODNAME = eina_error_msg_static_register(AZY_ERROR_REQUEST_XML_METHODNAME_err);
   AZY_ERROR_REQUEST_XML_PARAM = eina_error_msg_static_register(AZY_ERROR_REQUEST_XML_PARAM_err);

   AZY_ERROR_RESPONSE_XML_DOC = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_DOC_err);
   AZY_ERROR_RESPONSE_XML_ROOT = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_ROOT_err);
   AZY_ERROR_RESPONSE_XML_RETVAL = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_RETVAL_err);
   AZY_ERROR_RESPONSE_XML_MULTI = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_MULTI_err);
   AZY_ERROR_RESPONSE_XML_FAULT = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_FAULT_err);
   AZY_ERROR_RESPONSE_XML_INVAL = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_INVAL_err);
   AZY_ERROR_RESPONSE_XML_UNSERIAL = eina_error_msg_static_register(AZY_ERROR_RESPONSE_XML_UNSERIAL_err);
}

void
_azy_magic_fail(const void *d, Azy_Magic m, Azy_Magic req_m, const char *fname)
{
   ERR("\n"
       "*** AZY ERROR: Azy Magic Check Failed!!!\n"
       "*** IN FUNCTION: %s()", fname);
   if (!d)
     ERR("  Input handle pointer is NULL!");
   else if (m == AZY_MAGIC_NONE)
     ERR("  Input handle has already been freed!");
   else if (m != req_m)
     ERR("  Input handle is wrong type\n"
         "    Expected: %08x - %s\n"
         "    Supplied: %08x - %s",
         (unsigned int)req_m, eina_magic_string_get(req_m),
         (unsigned int)m, eina_magic_string_get(m));
     ERR("*** NAUGHTY PROGRAMMER!!!\n"
         "*** SPANK SPANK SPANK!!!\n"
         "*** Now go fix your code. Tut tut tut!\n"
         "*** This message brought to you by Ecore.");
   if (getenv("AZY_ERROR_ABORT")) abort();
}

/**
 * @brief Initialize Azy
 * This function initializes error handlers, events, and
 * logging functions for Azy and must be called prior to making any
 * calls.
 * @return The number of times the function has been called, or -1 on failure
 */
EAPI int
azy_init(void)
{
 /* FIXME: make this like other libs with init count */
   if (++azy_init_count_ != 1)
     return azy_init_count_;
   if (!eina_init()) return 0;
   azy_log_dom = eina_log_domain_register("azy", EINA_COLOR_BLUE);
   if (azy_log_dom < 0)
     {
        ERR("Could not register 'azy' log domain!");
        goto eina_fail;
     }
   if (!ecore_init()) goto fail;
   if (!ecore_con_init()) goto ecore_fail;

   azy_lib_register_errors_();

   AZY_CLIENT_DISCONNECTED = ecore_event_type_new();
   AZY_CLIENT_CONNECTED = ecore_event_type_new();
   AZY_CLIENT_RETURN = ecore_event_type_new();
   AZY_CLIENT_RESULT = ecore_event_type_new();
   AZY_CLIENT_ERROR = ecore_event_type_new();


   eina_magic_string_set(AZY_MAGIC_SERVER, "Azy_Server");
   eina_magic_string_set(AZY_MAGIC_SERVER_CLIENT, "Azy_Server_Client");
   eina_magic_string_set(AZY_MAGIC_SERVER_MODULE, "Azy_Server_Module");
   eina_magic_string_set(AZY_MAGIC_SERVER_MODULE_DEF, "Azy_Server_Module_Def");
   eina_magic_string_set(AZY_MAGIC_SERVER_MODULE_METHOD, "Azy_Server_Module_Method");
   eina_magic_string_set(AZY_MAGIC_CLIENT, "Azy_Client");
   eina_magic_string_set(AZY_MAGIC_CLIENT_DATA_HANDLER, "Azy_Client_Handler_Data");
   eina_magic_string_set(AZY_MAGIC_NET, "Azy_Net");
   eina_magic_string_set(AZY_MAGIC_VALUE, "Azy_Value");
   eina_magic_string_set(AZY_MAGIC_CONTENT, "Azy_Content");

ecore_fail:
   ecore_shutdown();
fail:
   eina_log_domain_unregister(azy_log_dom);
   azy_log_dom = -1;
eina_fail:
   eina_shutdown();
   return 0;
}

/**
 * @brief Shut down Azy
 * This function uninitializes memory allocated by azy_init.
 * Call when no further Azy functions will be used.
 * @return The number of times the az_init has been called, or -1 if 
 * all occurrences of azy have been shut down
 */
EAPI int
azy_shutdown(void)
{
   if (--azy_init_count_ != 0)
     return azy_init_count_;

   eina_log_domain_unregister(azy_log_dom);
   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   azy_log_dom = -1;
   return azy_init_count_;
}

