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

int azy_log_dom;
static int _azy_initialized = 0;

static void
_register_errors(void)
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
azy_init()
{
   if (_azy_initialized)
     return;
   eina_init();
   ecore_init();
   ecore_con_init();

   _register_errors();

   AZY_CLIENT_DISCONNECTED = ecore_event_type_new();
   AZY_CLIENT_CONNECTED = ecore_event_type_new();
   AZY_CLIENT_RETURN = ecore_event_type_new();
   AZY_CLIENT_ERROR = ecore_event_type_new();

   azy_log_dom = eina_log_domain_register("azy", EINA_COLOR_BLUE);

   _azy_initialized = 1;
}

void
azy_shutdown()
{
   if (!_azy_initialized)
     return;

   eina_log_domain_unregister(azy_log_dom);
   ecore_con_shutdown();
   ecore_shutdown();
   eina_shutdown();
   _azy_initialized = 0;
}

