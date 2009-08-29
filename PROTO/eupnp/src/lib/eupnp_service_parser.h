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

#ifndef _EUPNP_SERVICE_PARSER_H
#define _EUPNP_SERVICE_PARSER_H

#include <libxml/SAX2.h>
#include <Eina.h>
#include "eupnp_service_proxy.h"

typedef struct _Eupnp_Service_Parser_State Eupnp_Service_Parser_State;
typedef struct _Eupnp_Service_Parser Eupnp_Service_Parser;

typedef enum {
   START,
   INSIDE_XMLVER,
   INSIDE_SCPD,
   INSIDE_SPECVERSION,
   INSIDE_SPECVERSION_MAJOR,
   INSIDE_SPECVERSION_MINOR,
   INSIDE_ACTION_LIST,
   INSIDE_ACTION,
   INSIDE_ACTION_NAME,
   INSIDE_ACTION_ARGUMENT,
   INSIDE_ACTION_ARGUMENT_LIST,
   INSIDE_ACTION_ARGUMENT_NAME,
   INSIDE_ACTION_ARGUMENT_DIRECTION,
   INSIDE_ACTION_ARGUMENT_RETVAL,
   INSIDE_ACTION_ARGUMENT_RELATEDSTATEVARIABLE,
   INSIDE_SERVICE_STATE_TABLE,
   INSIDE_SERVICE_STATE_VARIABLE,
   INSIDE_SERVICE_STATE_VARIABLE_NAME,
   INSIDE_SERVICE_STATE_VARIABLE_DATATYPE,
   INSIDE_SERVICE_STATE_VARIABLE_DEFAULTVALUE,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_LIST,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MIN,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MAX,
   INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_STEP,
   FINISH,
   ERROR
} Eupnp_Service_Parser_State_Enum;

struct _Eupnp_Service_Parser_State {
   Eupnp_Service_Parser_State_Enum state;
   int state_skip;
   void *data;
   Eupnp_Service_Action *building_action;
   Eupnp_Service_Action_Argument *building_arg;
   Eupnp_State_Variable *building_state_var;
   Eupnp_State_Variable_Allowed_Value *building_allowed_value;
   Eina_Bool send_events;
};

struct _Eupnp_Service_Parser {
   Eupnp_Service_Parser_State state;
   xmlSAXHandler handler;
   xmlParserCtxtPtr ctx;
   char *chunk_acum;
   int chunk_acum_len;
};

int       eupnp_service_parser_init(void);
int       eupnp_service_parser_shutdown(void);
Eina_Bool eupnp_service_parse_buffer(const char *buffer, int buffer_len, Eupnp_Service_Proxy *s) EINA_ARG_NONNULL(1,2,3);


#endif /* _EUPNP_SERVICE_PARSER_H */
