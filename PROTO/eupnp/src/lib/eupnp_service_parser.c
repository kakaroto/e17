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

#include <string.h>
#include <libxml/SAX2.h>

#include "Eupnp.h"
#include "eupnp_log.h"


/*
 * Private API
 */

static int _eupnp_service_parser_init = 0;
static int _log_dom = -1;

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
};

static Eupnp_Service_Action *
eupnp_service_action_new(void)
{
   Eupnp_Service_Action *a;

   a = malloc(sizeof(Eupnp_Service_Action));

   if (!a)
     {
	ERROR_D(_log_dom, "Could not alloc for service action");
	return NULL;
     }

   a->name = NULL;
   a->arguments = NULL;

   return a;
}

static void
eupnp_service_action_argument_free(Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET(arg);
   free((char *)arg->name);
   free(arg->retval);
   free((char *)arg->value);
   free(arg);
}

void
eupnp_service_action_free(Eupnp_Service_Action *a)
{
   if (!a) return;

   free((char *)a->name);

   if (a->arguments)
     {
	Eupnp_Service_Action_Argument *item;

	while (item = (void *)a->arguments)
	  {
	     a->arguments = eina_inlist_remove(a->arguments, a->arguments);
	     eupnp_service_action_argument_free(item);
	  }
     }

   free(a);
}

Eupnp_Service_Action_Argument *
eupnp_service_action_argument_new(void)
{
   Eupnp_Service_Action_Argument *arg;
   arg = malloc(sizeof(Eupnp_Service_Action_Argument));

   CHECK_NULL_RET_VAL(arg, NULL);

   arg->name = NULL;
   arg->direction = EUPNP_ARGUMENT_DIRECTION_IN;
   arg->related_state_variable = NULL;
   arg->retval = NULL;
   arg->value = NULL;

   return arg;
}

static void
eupnp_service_action_argument_dump(const Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET(arg);

   INFO_D(_log_dom, "\t\tArgument dump");
   INFO_D(_log_dom, "\t\t\tname: %s", arg->name);
   INFO_D(_log_dom, "\t\t\tdirection: %d", arg->direction);
   if (arg->related_state_variable)
     INFO_D(_log_dom, "\t\t\trelated state var: %s", arg->related_state_variable->name);
   INFO_D(_log_dom, "\t\t\tretval: %s", (char *)arg->retval);
}

static void
eupnp_service_action_dump(const Eupnp_Service_Action *a)
{
   if (!a) return;

   INFO_D(_log_dom, "\tAction dump");
   INFO_D(_log_dom, "\t\tname: %s", a->name);

   if (a->arguments)
     {
	Eupnp_Service_Action_Argument *arg;

	EINA_INLIST_FOREACH(a->arguments, arg)
	   eupnp_service_action_argument_dump(arg);
     }
}

static Eupnp_State_Variable *
eupnp_service_state_variable_new1(void)
{
   Eupnp_State_Variable *st;

   st = malloc(sizeof(Eupnp_State_Variable));

   if (!st)
     {
	ERROR_D(_log_dom, "Could not alloc for new state variable");
	return NULL;
     }

   st->name = NULL;
   st->send_events = EINA_TRUE;
   st->data_type = EUPNP_DATA_TYPE_STRING;
   st->default_value = NULL;
   st->allowed_value_list = NULL;
   st->range_min = NULL;
   st->range_max = NULL;
   st->range_step = NULL;

   return st;
}

static Eupnp_State_Variable *
eupnp_service_state_variable_new(const char *name, int name_len)
{
   Eupnp_State_Variable *st;

   st = eupnp_service_state_variable_new1();

   if (!st)
     {
	ERROR_D(_log_dom, "Could not alloc for new state variable");
	return NULL;
     }

   st->name = malloc(sizeof(char)*(name_len+1));

   if (!st->name)
     {
	ERROR_D(_log_dom, "Could not alloc for state var name");
	free(st);
	return NULL;
     }

   memcpy((char *)st->name, name, name_len);
   ((char *)st->name)[name_len] = '\0';

   return st;
}

static void
eupnp_service_state_variable_allowed_value_free(Eupnp_State_Variable_Allowed_Value *value)
{
   CHECK_NULL_RET(value);
   free((char *)value->value);
   free(value);
}

void
eupnp_service_state_variable_free(Eupnp_State_Variable *st)
{
   CHECK_NULL_RET(st);

   free((char *)st->name);
   free((char *)st->default_value);
   free((char *)st->range_min);
   free((char *)st->range_max);
   free((char *)st->range_step);

   if (st->allowed_value_list)
     {
	Eupnp_State_Variable_Allowed_Value *item;

	while (item = (void *)st->allowed_value_list)
	  {
	     st->allowed_value_list = eina_inlist_remove(st->allowed_value_list,
						         st->allowed_value_list);
	     eupnp_service_state_variable_allowed_value_free(item);
	  }
     }

   free(st);
}

static void
eupnp_service_state_variable_dump(const Eupnp_State_Variable *st)
{
   CHECK_NULL_RET(st);

   INFO_D(_log_dom, "\t\tState variable dump");
   INFO_D(_log_dom, "\t\t\tname: %s", st->name);
   INFO_D(_log_dom, "\t\t\tsendEvents: %d", st->send_events);
   INFO_D(_log_dom, "\t\t\tdefault value: %s", (char *)st->default_value);
   INFO_D(_log_dom, "\t\t\trange min: %s", (char *)st->range_min);
   INFO_D(_log_dom, "\t\t\trange max: %s", (char *)st->range_max);
   INFO_D(_log_dom, "\t\t\trange step: %s", (char *)st->range_step);
   INFO_D(_log_dom, "\t\t\tdata type: %d", st->data_type);

   if (st->allowed_value_list)
   {
      Eupnp_State_Variable_Allowed_Value *item;
      INFO_D(_log_dom, "\t\t\tAllowed value list:");

      EINA_INLIST_FOREACH(st->allowed_value_list, item)
	INFO_D(_log_dom, "\t\t\t\t%s", item->value);
   }
}

static Eina_Bool
eupnp_service_state_variable_allowed_value_add(Eupnp_State_Variable *st, Eupnp_State_Variable_Allowed_Value *v)
{
   CHECK_NULL_RET_VAL(st, EINA_FALSE);
   CHECK_NULL_RET_VAL(v, EINA_FALSE);

   st->allowed_value_list = eina_inlist_append(st->allowed_value_list, EINA_INLIST_GET(v));

   return EINA_TRUE;
}

static Eina_Bool
eupnp_service_action_argument_add(Eupnp_Service_Action *action, Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET_VAL(action, EINA_FALSE);
   CHECK_NULL_RET_VAL(arg, EINA_FALSE);

   action->arguments = eina_inlist_append(action->arguments, EINA_INLIST_GET(arg));

   return EINA_TRUE;
}

static Eupnp_State_Variable_Allowed_Value *
eupnp_service_state_variable_allowed_value_new(void)
{
   Eupnp_State_Variable_Allowed_Value *v;

   v = malloc(sizeof(Eupnp_State_Variable_Allowed_Value));

   if (!v)
     {
	ERROR_D(_log_dom, "Could not alloc for allowed value");
	return NULL;
     }

   v->value = NULL;

   return v;
}

static Eina_Bool
eupnp_service_proxy_action_add(Eupnp_Service_Proxy *proxy, Eupnp_Service_Action *action)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(action, EINA_FALSE);

   proxy->actions = eina_inlist_append(proxy->actions, EINA_INLIST_GET(action));

   return EINA_TRUE;
}

static Eina_Bool
eupnp_service_proxy_state_variable_add(Eupnp_Service_Proxy *proxy, Eupnp_State_Variable *st)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(st, EINA_FALSE);

   proxy->state_table = eina_inlist_append(proxy->state_table, EINA_INLIST_GET(st));

   return EINA_TRUE;
}

#define COPY_CHARACTERS(to, from, len)     \
   if (!to)                                \
     {                                     \
        to = malloc(sizeof(char)*(len+1)); \
	if (!to)                           \
	  {                                \
	     ERROR_D(_log_dom, "Could not alloc for device information"); \
	     return;                       \
	  }                                \
	memcpy((char *)to, from, len);     \
	((char *)to)[len] = '\0';          \
     }

static void
_characters(void *state, const xmlChar *ch, int len)
{
   Eupnp_Service_Parser_State *s = state;
   Eupnp_Service_Proxy *p = s->data;
   Eupnp_State_Variable *related = NULL;

   switch (s->state)
     {
	case INSIDE_SPECVERSION_MAJOR:
	  p->spec_version_major = strtol(ch, NULL, 10);
	  break;

	case INSIDE_SPECVERSION_MINOR:
	  p->spec_version_minor = strtol(ch, NULL, 10);
	  break;

	case INSIDE_ACTION_NAME:
	  DEBUG_D(_log_dom, "Writing action name");
	  COPY_CHARACTERS(s->building_action->name, ch, len);
	  break;

	case INSIDE_ACTION_ARGUMENT_NAME:
	  DEBUG_D(_log_dom, "Writing action argument name");
	  COPY_CHARACTERS(s->building_arg->name, ch, len);
	  break;

	case INSIDE_ACTION_ARGUMENT_DIRECTION:
	  DEBUG_D(_log_dom, "Writing action argument direction");
	  if (!strncmp(ch, "in", 2))
	    s->building_arg->direction = EUPNP_ARGUMENT_DIRECTION_IN;
	  else
	    s->building_arg->direction = EUPNP_ARGUMENT_DIRECTION_OUT;
	  break;

	case INSIDE_ACTION_ARGUMENT_RETVAL:
	  DEBUG_D(_log_dom, "Writing action argument retval");
	  COPY_CHARACTERS(s->building_arg->retval, ch, len);
	  break;

	case INSIDE_ACTION_ARGUMENT_RELATEDSTATEVARIABLE:
	  DEBUG_D(_log_dom, "Writing action argument related state variable");
	  related = eupnp_service_proxy_state_variable_get(p, ch, len);

	  if (!related)
	    {
	       // Not added yet, create it.
	       related = eupnp_service_state_variable_new(ch, len);
	       if (!related)
	         {
		    ERROR_D(_log_dom, "Could not create state variable (related stage)");
		    break;
		 }
	       eupnp_service_proxy_state_variable_add(p, related);
	    }

	  s->building_arg->related_state_variable = related;
	  related = NULL;
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_NAME:
	  DEBUG_D(_log_dom, "Writing state variable name");
	  COPY_CHARACTERS(s->building_state_var->name, ch, len);
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_DATATYPE:
	  DEBUG_D(_log_dom, "Writing state variable data type");

	  if (!strncmp(ch, "ui1", strlen("ui1")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_UI1;

	  else if (!strncmp(ch, "ui2", strlen("ui2")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_UI2;

	  else if (!strncmp(ch, "ui4", strlen("ui4")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_UI4;

	  else if (!strncmp(ch, "i1", strlen("i1")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_I1;

	  else if (!strncmp(ch, "i2", strlen("i2")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_I2;

	  else if (!strncmp(ch, "i4", strlen("i4")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_I4;

	  else if (!strncmp(ch, "int", strlen("int")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_INT;

	  else if (!strncmp(ch, "r4", strlen("r4")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_R4;

	  else if (!strncmp(ch, "r8", strlen("r8")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_R8;

	  else if (!strncmp(ch, "number", strlen("number")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_NUMBER;

	  else if (!strncmp(ch, "fixed.14.4", strlen("fixed.14.4")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_FIXED14_4;

	  else if (!strncmp(ch, "float", strlen("float")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_FLOAT;

	  else if (!strncmp(ch, "char", strlen("char")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_CHAR;

	  else if (!strncmp(ch, "char", strlen("char")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_CHAR;

	  else if (!strncmp(ch, "string", strlen("string")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_STRING;

	  else if (!strncmp(ch, "date", strlen("date")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_DATE;

	  else if (!strncmp(ch, "dateTime", strlen("dateTime")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_DATETIME;

	  else if (!strncmp(ch, "dateTime.tz", strlen("dateTime.tz")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_DATETIME_TZ;

	  else if (!strncmp(ch, "time", strlen("time")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_TIME;

	  else if (!strncmp(ch, "time.tz", strlen("time.tz")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_TIME_TZ;

	  else if (!strncmp(ch, "boolean", strlen("boolean")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_BOOLEAN;

	  else if (!strncmp(ch, "bin.base64", strlen("bin.base64")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_BIN_BASE64;

	  else if (!strncmp(ch, "bin.hex", strlen("bin.hex")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_BIN_HEX;

	  else if (!strncmp(ch, "uri", strlen("uri")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_URI;

	  else if (!strncmp(ch, "uuid", strlen("uuid")))
	    s->building_state_var->data_type = EUPNP_DATA_TYPE_UUID;

	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_DEFAULTVALUE:
	  DEBUG_D(_log_dom, "Writing service state variable default value");
	  COPY_CHARACTERS(s->building_state_var->default_value, ch, len);
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MIN:
	  DEBUG_D(_log_dom, "Writing allowed value range min");
	  COPY_CHARACTERS(s->building_state_var->range_min, ch, len);
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MAX:
	  DEBUG_D(_log_dom, "Writing allowed value range max");
	  COPY_CHARACTERS(s->building_state_var->range_max, ch, len);
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_STEP:
	  DEBUG_D(_log_dom, "Writing allowed value range step");
	  COPY_CHARACTERS(s->building_state_var->range_step, ch, len);
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE:
	  DEBUG_D(_log_dom, "Writing allowed value enum value");
	  COPY_CHARACTERS(s->building_allowed_value->value, ch, len);
	  break;
     }
}

static void
start_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
   Eupnp_Service_Parser_State *s = state;

   switch(s->state)
     {
	case START:
	  if (!strcmp(name, "scpd"))
	     s->state = INSIDE_SCPD;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SCPD:
	  if (!strcmp(name, "specVersion"))
	     s->state = INSIDE_SPECVERSION;
	  else if (!strcmp(name, "actionList"))
	     s->state = INSIDE_ACTION_LIST;
	  else if (!strcmp(name, "serviceStateTable"))
	     s->state = INSIDE_SERVICE_STATE_TABLE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SPECVERSION:
	  if (!strcmp(name, "major"))
	     s->state = INSIDE_SPECVERSION_MAJOR;
	  else if (!strcmp(name, "minor"))
	     s->state = INSIDE_SPECVERSION_MINOR;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ACTION_LIST:
	  if (!strcmp(name, "action"))
	     s->state = INSIDE_ACTION;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ACTION:
	  if (!s->building_action)
	    {
	       s->building_action = eupnp_service_action_new();

	       if (!s->building_action)
	         {
		    ERROR_D(_log_dom, "Could not create action");
		    break;
		 }
	    }
	  if (!strcmp(name, "name"))
	     s->state = INSIDE_ACTION_NAME;
	  else if (!strcmp(name, "argumentList"))
	     s->state = INSIDE_ACTION_ARGUMENT_LIST;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ACTION_ARGUMENT_LIST:
	  if (!strcmp(name, "argument"))
	     s->state = INSIDE_ACTION_ARGUMENT;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ACTION_ARGUMENT:
	  if (!s->building_arg)
	    {
	       s->building_arg = eupnp_service_action_argument_new();

	       if (!s->building_arg)
	         {
		    ERROR_D(_log_dom, "Could not create action argument.");
		    break;
		 }
	    }
	  if (!strcmp(name, "name"))
	     s->state = INSIDE_ACTION_ARGUMENT_NAME;
	  else if (!strcmp(name, "direction"))
	     s->state = INSIDE_ACTION_ARGUMENT_DIRECTION;
	  else if (!strcmp(name, "retVal"))
	     s->state = INSIDE_ACTION_ARGUMENT_RETVAL;
	  else if (!strcmp(name, "relatedStateVariable"))
	     s->state = INSIDE_ACTION_ARGUMENT_RELATEDSTATEVARIABLE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICE_STATE_TABLE:
	  if (nb_attributes > 0)
	    {
	       int i;
	       int index = 0;
	       for (i = 0; i < nb_attributes; ++i, index += 5)
		    if (!strncmp(attributes[index], "sendEvents", strlen("sendEvents")))
		       if (!strncmp(attributes[index+3], "no", strlen("no")))
			  s->send_events = EINA_FALSE;
	    }

	  if (!strcmp(name, "stateVariable"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE:
	  if (!s->building_state_var)
	    {
	       s->building_state_var = eupnp_service_state_variable_new1();

	       if (!s->building_state_var)
	         {
		    ERROR_D(_log_dom, "Could not create state variable");
		    break;
		 }
	    }
	  if (!strcmp(name, "name"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_NAME;
	  else if (!strcmp(name, "dataType"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_DATATYPE;
	  else if (!strcmp(name, "defaultValue"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_DEFAULTVALUE;
	  else if (!strcmp(name, "allowedValueList"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_LIST;
	  else if (!strcmp(name, "allowedValueRange"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_LIST:
	  if (!s->building_allowed_value)
	    {
	       s->building_allowed_value = eupnp_service_state_variable_allowed_value_new();

	       if (!s->building_allowed_value)
	         {
		    ERROR_D(_log_dom, "Could not build allowed value");
		    break;
		 }
	    }
	  if (!strcmp(name, "allowedValue"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE:
	  if (!strcmp(name, "minimum"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MIN;
	  else if (!strcmp(name, "maximum"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MAX;
	  else if (!strcmp(name, "step"))
	     s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_STEP;
	  else
	     s->state_skip++;
	  break;

     }
}

static void
end_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI)
{
   Eupnp_Service_Parser_State *s = state;
   Eupnp_Service_Proxy *p = s->data;

   if (s->state_skip)
     {
	DEBUG_D(_log_dom, "Skipped");
	s->state_skip--;
	return;
     }

   switch (s->state)
     {
	case INSIDE_SCPD:
	  s->state = FINISH;
	  break;

	case INSIDE_SPECVERSION:
	case INSIDE_ACTION_LIST:
	case INSIDE_SERVICE_STATE_TABLE:
	  s->state = INSIDE_SCPD;
	  break;

	case INSIDE_SPECVERSION_MAJOR:
	case INSIDE_SPECVERSION_MINOR:
	  s->state = INSIDE_SPECVERSION;
	  break;

	case INSIDE_ACTION:
	  if (s->building_action)
	     eupnp_service_proxy_action_add(p, s->building_action);
	  else
	     ERROR_D(_log_dom, "Failed to add action");

	  s->building_action = NULL;
	  s->state = INSIDE_ACTION_LIST;
	  break;

	case INSIDE_ACTION_NAME:
	case INSIDE_ACTION_ARGUMENT_LIST:
	  s->state = INSIDE_ACTION;
	  break;

	case INSIDE_ACTION_ARGUMENT:
	  // Finished parsing argument
	  if ((s->building_arg) && (s->building_action))
	     eupnp_service_action_argument_add(s->building_action, s->building_arg);
	  else
	     // FIXME free action if argument is not present?
	     ERROR_D(_log_dom, "Failed to add argument.");

	  s->building_arg = NULL;
	  s->state = INSIDE_ACTION_ARGUMENT_LIST;
	  break;
	
	case INSIDE_ACTION_ARGUMENT_RELATEDSTATEVARIABLE:
	case INSIDE_ACTION_ARGUMENT_RETVAL:
	case INSIDE_ACTION_ARGUMENT_NAME:
	case INSIDE_ACTION_ARGUMENT_DIRECTION:
	  s->state = INSIDE_ACTION_ARGUMENT;
	  break;

	case INSIDE_SERVICE_STATE_VARIABLE:
	  // Finished building state variable, add it.
	  if (s->building_state_var)
	    {
	      eupnp_service_proxy_state_variable_add(p, s->building_state_var);
	      s->building_state_var->send_events = s->send_events;
	    }

	  // Reset state var build
	  s->building_state_var = NULL;
	  s->send_events = EINA_TRUE; // Default value is yes
	  s->state = INSIDE_SERVICE_STATE_TABLE;
	  break;
	
	case INSIDE_SERVICE_STATE_VARIABLE_NAME:
	case INSIDE_SERVICE_STATE_VARIABLE_DEFAULTVALUE:
	case INSIDE_SERVICE_STATE_VARIABLE_DATATYPE:
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_LIST:
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE:
	  s->state = INSIDE_SERVICE_STATE_VARIABLE;
	  break;
	
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE:
	  if ((s->building_state_var) && (s->building_allowed_value))
	     eupnp_service_state_variable_allowed_value_add(s->building_state_var, s->building_allowed_value);
	  else
	     ERROR_D(_log_dom, "Failed to add allowed value");

	  s->building_allowed_value = NULL;
	  s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_LIST;
	  break;
	
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MIN:
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_MAX:
	case INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE_STEP:
	  s->state = INSIDE_SERVICE_STATE_VARIABLE_ALLOWEDVALUE_RANGE;
	  break;
     }
}

static void
error(void *state, const char *msg, ...)
{
   va_list args;
   va_start(args, msg);
   vfprintf(stdout, msg, args);
   va_end(args);
}

static Eupnp_Service_Parser *
eupnp_service_parser_new(const char *first_chunk, int first_chunk_len, Eupnp_Service_Proxy *d)
{
   if (first_chunk_len < 4)
     {
	WARN_D(_log_dom, "First chunk length less than 4 chars, user must provide more than 4.");
	return NULL;
     }

   Eupnp_Service_Parser *p;

   p = malloc(sizeof(Eupnp_Service_Parser));

   if (!p)
     {
	ERROR_D(_log_dom, "Failed to alloc for service parser");
	return NULL;
     }

   memset(&p->handler, 0, sizeof(xmlSAXHandler));

   p->handler.initialized = XML_SAX2_MAGIC;
   p->handler.characters = &_characters;
   p->handler.error = &error;
   p->handler.startElementNs = &start_element_ns;
   p->handler.endElementNs = &end_element_ns;

   /*
    * Setup parser state to START, attach the service info object that
    * will get data written into.
    */
   p->state.state = START;
   p->state.state_skip = 0;
   p->state.data = d;
   p->state.building_action = NULL;
   p->state.building_arg = NULL;
   p->state.building_state_var = NULL;
   p->state.building_allowed_value = NULL;
   p->state.send_events = EINA_TRUE; // default is YES

   p->ctx = xmlCreatePushParserCtxt(&p->handler, &p->state, first_chunk,
				    first_chunk_len, NULL);

   // Force first chunk parse. When not forced, the parser gets lazy on the
   // first time and doesn't parse one-big-chunk feeds.
   xmlParseChunk(p->ctx, NULL, 0, 0);

   if (!p->ctx)
     {
	free(p);
	return NULL;
     }

   return p;
}

static void
eupnp_service_parser_free(Eupnp_Service_Parser *p)
{
   if (!p) return;
   if (p->ctx) xmlFreeParserCtxt(p->ctx);
   free(p);
}

static Eina_Bool
eupnp_service_parse_finish(Eupnp_Service_Proxy *d)
{
   if (!d) return;
   if (!d->xml_parser) return;
   DEBUG_D(_log_dom, "Service SCPD parse finish");

   Eina_Bool ret;
   Eupnp_Service_Parser *parser = d->xml_parser;
   ret = xmlParseChunk(parser->ctx, NULL, 0, 1);
   eupnp_service_parser_free(parser);
   d->xml_parser = NULL;
   return !ret;
}

static void
eupnp_service_parse_check_finished(Eupnp_Service_Proxy *d)
{
   Eupnp_Service_Parser *p = d->xml_parser;

   if (p->state.state == FINISH)
      eupnp_service_parse_finish(d);
}



/*
 * Public API
 */

EAPI int EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED = 0;


EAPI int
eupnp_service_parser_init(void)
{
   if (_eupnp_service_parser_init)
      return ++_eupnp_service_parser_init;

   xmlInitParser();

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Could not initialize eupnp error module.\n");
	goto log_init_error;
     }

   EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED = eina_error_msg_register("Parser feeded with less than 4 chars. Feed it with at least 4 chars");

   if ((_log_dom = eina_log_domain_register("Eupnp.ServiceParser", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create logging domain for service parser module.\n");
	goto log_dom_error;
     }

   INFO_D(_log_dom, "Initializing service parser module.");

   return ++_eupnp_service_parser_init;

   log_dom_error:
     eupnp_log_shutdown();
   log_init_error:
     xmlCleanupParser();

   return 0;
}

EAPI int
eupnp_service_parser_shutdown(void)
{
   if (_eupnp_service_parser_init != 1)
      return --_eupnp_service_parser_init;

   INFO_D(_log_dom, "Shutting down service parser module.");

   eina_log_domain_unregister(_log_dom);
   eupnp_log_shutdown();
   xmlCleanupParser();

   return --_eupnp_service_parser_init;
}

EAPI Eina_Bool
eupnp_service_parse_buffer(const char *buffer, int buffer_len, Eupnp_Service_Proxy *s)
{
   Eina_Bool ret = EINA_FALSE;
   if ((!buffer) || (!buffer_len) || (!s)) return ret;

   Eupnp_Service_Parser *parser;

   if (!s->xml_parser)
     {
	/* Creates the parser, which parses the first chunk */
	DEBUG_D(_log_dom, "Creating service parser.");

	s->xml_parser = eupnp_service_parser_new(buffer, buffer_len, s);

	if (!s->xml_parser)
          {
	     ERROR_D(_log_dom, "Failed to parse first chunk");
	     goto parse_ret;
	  }

        ret = EINA_TRUE;
	goto parse_ret;
     }

   parser = s->xml_parser;

   if (!parser->ctx)
     {
	// first_chunk_len < 4 case
	eina_error_set(EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED);
	return EINA_FALSE;
     }

   // Progressive feeds
   if (parser->state.state == FINISH)
     {
	WARN_D(_log_dom, "Already finished parsing");
	ret = EINA_TRUE;
	goto parse_ret;
     }

   DEBUG_D(_log_dom, "Parsing XML (%d) at %p", buffer_len, buffer);

   if (!xmlParseChunk(parser->ctx, buffer, buffer_len, 0))
     {
	ret = EINA_TRUE;
	goto parse_ret;
     }

   parse_ret:
      eupnp_service_parse_check_finished(s);
      return ret;
}
