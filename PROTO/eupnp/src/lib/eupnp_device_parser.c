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

#include <stdio.h>
#include <string.h>
#include <libxml/SAX2.h>

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_private.h"

/**
 * Private API
 */

#define COPY_CHARACTERS(to, from, len)                          \
   if (!to)                                                     \
     {                                                          \
        to = malloc(sizeof(char)*(len+1));                      \
	if (!to)                                                \
	  {                                                     \
	     ERROR_D(_log_dom,                                  \
	             "Could not alloc for device information"); \
	     return;                                            \
	  }                                                     \
	memcpy((char *)to, from, len);                          \
	((char *)to)[len] = '\0';                               \
     }

static int _log_dom = -1;

typedef struct _Eupnp_Device_Parser_State Eupnp_Device_Parser_State;
typedef struct _Eupnp_Device_Parser Eupnp_Device_Parser;

typedef enum {
   START,
   INSIDE_ROOT,
   INSIDE_SPEC_VERSION,
   INSIDE_SPEC_VERSION_MAJOR,
   INSIDE_SPEC_VERSION_MINOR,
   INSIDE_URLBASE,
   INSIDE_DEVICE,
   INSIDE_DEVICETYPE,
   INSIDE_DEVICE_FRIENDLYNAME,
   INSIDE_MANUFACTURER,
   INSIDE_MANUFACTURER_URL,
   INSIDE_MODEL_DESCRIPTION,
   INSIDE_MODEL_NAME,
   INSIDE_MODEL_URL,
   INSIDE_SERIAL_NUMBER,
   INSIDE_UDN,
   INSIDE_UPC,
   INSIDE_ICON_LIST,
   INSIDE_ICON,
   INSIDE_ICON_MIMETYPE,
   INSIDE_ICON_WIDTH,
   INSIDE_ICON_HEIGHT,
   INSIDE_ICON_DEPTH,
   INSIDE_ICON_URL,
   INSIDE_PRESENTATION_URL,
   INSIDE_DEVICELIST,
   INSIDE_SERVICELIST,
   INSIDE_SERVICE,
   INSIDE_SERVICE_TYPE,
   INSIDE_SERVICE_ID,
   INSIDE_SERVICE_SCPDURL,
   INSIDE_SERVICE_CONTROL_URL,
   INSIDE_SERVICE_EVENTSUB_URL,
   FINISH,
   ERROR
} Eupnp_Device_Parser_State_Enum;

struct _Eupnp_Device_Parser_State {
   Eupnp_Device_Parser_State *parent;
   Eupnp_Device_Parser_State_Enum state;
   /* Building Context */
   int state_skip; /* Used for skipping unknown tags */
   Eupnp_Device_Info *device;
   Eupnp_Service_Info *service;
   Eupnp_Device_Icon *icon;
   xmlParserCtxtPtr ctx;
};

struct _Eupnp_Device_Parser {
   Eupnp_Device_Parser_State state;
   xmlSAXHandler handler;
   xmlParserCtxtPtr ctx;
};

/**
 * Finishes parsing embedded devices by adding missing fields with inherited
 * information (e.g. import missing URLBase from parent device).
 *
 * This information filling process must be done top-down so that a child device
 * always get the correct information, if it is present on the higher layers.
 *
 * Fields filled with this function are automatically collected during
 * eupnp_device_info_free(), since it knows whether it is an embedded device or
 * not (= if fields were filled here or belong to an external resource).
 */
static void
_embedded_devices_parse_finish(Eupnp_Device_Info *d)
{
   CHECK_NULL_RET(d);

   Eupnp_Device_Info *embedded;

   EINA_INLIST_FOREACH(d->embedded_devices, embedded)
     {
	embedded->spec_version_major = d->spec_version_major;
	embedded->spec_version_minor = d->spec_version_minor;

	if (d->location && !embedded->location)
	   embedded->location = strdup(d->location);

	if (d->base_url && !embedded->base_url)
	   embedded->base_url = strdup(d->base_url);

	_embedded_devices_parse_finish(embedded);
     }
}

static Eupnp_Device_Icon *
eupnp_icon_new(void)
{
   Eupnp_Device_Icon *icon;
   icon = calloc(1, sizeof(Eupnp_Device_Icon));
   return icon;
}

static void
_characters(void *state, const xmlChar *ch, int len)
{
   Eupnp_Device_Parser_State *s = state;
   Eupnp_Device_Info *d = s->device;

   switch(s->state)
     {
	case INSIDE_SPEC_VERSION_MAJOR:
	  d->spec_version_major = strtol(ch, NULL, 10);
	  break;

	case INSIDE_SPEC_VERSION_MINOR:
	  d->spec_version_minor = strtol(ch, NULL, 10);
	  break;

	case INSIDE_URLBASE:
	  DEBUG_D(_log_dom, "Writing device URL base");
	  COPY_CHARACTERS(d->base_url, ch, len);
	  break;

	case INSIDE_DEVICETYPE:
	  DEBUG_D(_log_dom, "Writing device type");
	  COPY_CHARACTERS(d->device_type, ch, len);
	  break;

	case INSIDE_DEVICE_FRIENDLYNAME:
	  DEBUG_D(_log_dom, "Writing device friendly name");
	  COPY_CHARACTERS(d->friendly_name, ch, len);
	  break;

	case INSIDE_MANUFACTURER:
	  DEBUG_D(_log_dom, "Writing device manufacturer");
	  COPY_CHARACTERS(d->manufacturer, ch, len);
	  break;

	case INSIDE_MANUFACTURER_URL:
	  DEBUG_D(_log_dom, "Writing device manufacturer URL");
	  COPY_CHARACTERS(d->manufacturer_url, ch, len);
	  break;

	case INSIDE_MODEL_DESCRIPTION:
	  DEBUG_D(_log_dom, "Writing device model descr.");
	  COPY_CHARACTERS(d->model_description, ch, len);
	  break;

	case INSIDE_MODEL_NAME:
	  DEBUG_D(_log_dom, "Writing device model name");
	  COPY_CHARACTERS(d->model_name, ch, len);
	  break;

	case INSIDE_MODEL_URL:
	  DEBUG_D(_log_dom, "Writing device model URL");
	  COPY_CHARACTERS(d->model_url, ch, len);
	  break;

	case INSIDE_SERIAL_NUMBER:
	  DEBUG_D(_log_dom, "Writing device serial number");
	  COPY_CHARACTERS(d->serial_number, ch, len);
	  break;

	case INSIDE_UPC:
	  DEBUG_D(_log_dom, "Writing device's UPC");
	  COPY_CHARACTERS(d->upc, ch, len);
	  break;

	case INSIDE_UDN:
	  /* Device must have an UDN received on the NOTIFY message and must
	   * match the one on the XML
	   */
	  DEBUG_D(_log_dom, "Checking for device's UDN consistency");
	  if (!d->udn)
	    {
	       DEBUG_D(_log_dom, "Writing device's udn");
	       COPY_CHARACTERS(d->udn, ch, len);
	    }
	  else DEBUG_D(_log_dom, "Device XML UDN already parsed from HTTP message.");
	  break;

	case INSIDE_PRESENTATION_URL:
	  DEBUG_D(_log_dom, "Writing device presentation URL");
	  COPY_CHARACTERS(d->presentation_url, ch, len);
	  break;

	case INSIDE_ICON_MIMETYPE:
	  DEBUG_D(_log_dom, "Writing a device icon's mimetype");
	  COPY_CHARACTERS(s->icon->mimetype, ch, len);
	  break;

	case INSIDE_ICON_URL:
	  DEBUG_D(_log_dom, "Writing a device icon's url");
	  COPY_CHARACTERS(s->icon->url, ch, len);
	  break;

	case INSIDE_ICON_WIDTH:
	  DEBUG_D(_log_dom, "Writing a device icon's width");
	  s->icon->width = strtol(ch, NULL, 10);
	  break;

	case INSIDE_ICON_HEIGHT:
	  DEBUG_D(_log_dom, "Writing a device icon's height");
	  s->icon->height = strtol(ch, NULL, 10);
	  break;

	case INSIDE_ICON_DEPTH:
	  DEBUG_D(_log_dom, "Writing a device icon's depth");
	  s->icon->depth = strtol(ch, NULL, 10);
	  break;

	case INSIDE_SERVICE_TYPE:
	  DEBUG_D(_log_dom, "Writing a service's type");
	  COPY_CHARACTERS(s->service->service_type, ch, len);
	  break;

	case INSIDE_SERVICE_ID:
	  DEBUG_D(_log_dom, "Writing a service's ID");
	  COPY_CHARACTERS(s->service->id, ch, len);
	  break;

	case INSIDE_SERVICE_SCPDURL:
	  DEBUG_D(_log_dom, "Writing a service's SCPD URL");
	  COPY_CHARACTERS(s->service->scpd_url, ch, len);
	  break;

	case INSIDE_SERVICE_EVENTSUB_URL:
	  DEBUG_D(_log_dom, "Writing a service's eventsub URL");
	  COPY_CHARACTERS(s->service->eventsub_url, ch, len);
	  break;

	case INSIDE_SERVICE_CONTROL_URL:
	  DEBUG_D(_log_dom, "Writing a service's control URL");
	  COPY_CHARACTERS(s->service->control_url, ch, len);
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

static void
start_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
   DEBUG_D(_log_dom, "Start NS at %s, prefix=%s, uri=%s, ndefs=%d, nattrs=%d", name, prefix, URI, nb_namespaces, nb_attributes);

   Eupnp_Device_Parser_State *s = state;
   Eupnp_Device_Info *d = s->device;

   switch (s->state)
     {
	case START:
	  if (!strcmp(name, "root"))
	     s->state = INSIDE_ROOT;
	  else
	     s->state = ERROR;
	  break;

	case INSIDE_ROOT:
	  if (!strcmp(name, "specVersion"))
	     s->state = INSIDE_SPEC_VERSION;
	  else if (!strcmp(name, "URLBase"))
	     s->state = INSIDE_URLBASE;
	  else if (!strcmp(name, "device"))
	     s->state = INSIDE_DEVICE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SPEC_VERSION:
	  if (!strcmp(name, "major"))
	     s->state = INSIDE_SPEC_VERSION_MAJOR;
	  else if (!strcmp(name, "minor"))
	     s->state = INSIDE_SPEC_VERSION_MINOR;
	  else
	     s->state_skip++; /* Invalid XML but we skip the unknown tag */
	  break;

	case INSIDE_DEVICE:
	  if (!s->device)
	    {
	       // Create embedded device. The condition above is only true for
	       // embedded devices.
	       s->device = eupnp_device_info_new(NULL, NULL, NULL, NULL);

	       if (!s->device)
	         {
		    ERROR_D(_log_dom, "Failed to alloc for new device");
		    s->state_skip++;
		    break;
		 }
	    }

	  if (!strcmp(name, "deviceType"))
	     s->state = INSIDE_DEVICETYPE;
	  else if (!strcmp(name, "friendlyName"))
	     s->state = INSIDE_DEVICE_FRIENDLYNAME;
	  else if (!strcmp(name, "manufacturer"))
	     s->state = INSIDE_MANUFACTURER;
	  else if (!strcmp(name, "manufacturerURL"))
	     s->state = INSIDE_MANUFACTURER_URL;
	  else if (!strcmp(name, "modelDescription"))
	     s->state = INSIDE_MODEL_DESCRIPTION;
	  else if (!strcmp(name, "modelName"))
	     s->state = INSIDE_MODEL_NAME;
	  else if (!strcmp(name, "modelURL"))
	     s->state = INSIDE_MODEL_URL;
	  else if (!strcmp(name, "serialNumber"))
	     s->state = INSIDE_SERIAL_NUMBER;
	  else if (!strcmp(name, "presentationURL"))
	     s->state = INSIDE_PRESENTATION_URL;
	  else if (!strcmp(name, "deviceList"))
	    {
	       Eupnp_Device_Parser_State *new;

	       new = calloc(1, sizeof(Eupnp_Device_Parser_State));

	       if (!new)
		  {
		     ERROR_D(_log_dom, "Could not alloc for device list, skipping it!");
		     s->state_skip++;
		     break;
		  }

	       DEBUG_D(_log_dom, "Switching context %p -> %p", s, new);

	       // Save parent for switching back context
	       new->parent = s;
	       new->ctx = s->ctx;

	       // Setup new context startup position
	       new->state = INSIDE_ROOT;

	       // Inject new state on context -> alters parser target
	       s->ctx->userData = new;

	       DEBUG_D(_log_dom, "Finished switch");
	       break;
	    }
	  else if (!strcmp(name, "serviceList"))
	     s->state = INSIDE_SERVICELIST;
	  else if (!strcmp(name, "UPC"))
	     s->state = INSIDE_UPC;
	  else if (!strcmp(name, "UDN"))
	     s->state = INSIDE_UDN;
	  else if (!strcmp(name, "iconList"))
	     s->state = INSIDE_ICON_LIST;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ICON_LIST:
	  if (!strcmp(name, "icon"))
	     s->state = INSIDE_ICON;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_ICON:
	  if (!s->icon)
	    {
		s->icon = eupnp_icon_new();
		if (!s->icon)
		  {
		     ERROR_D(_log_dom, "Could not alloc memory for icon");
		     s->state_skip++;
		     break;
		  }
	    }
	  if (!strcmp(name, "mimetype"))
	     s->state = INSIDE_ICON_MIMETYPE;
	  else if (!strcmp(name, "width"))
	     s->state = INSIDE_ICON_WIDTH;
	  else if (!strcmp(name, "height"))
	     s->state = INSIDE_ICON_HEIGHT;
	  else if (!strcmp(name, "depth"))
	     s->state = INSIDE_ICON_DEPTH;
	  else if (!strcmp(name, "url"))
	     s->state = INSIDE_ICON_URL;
	  else
	    {
		WARN_D(_log_dom, "Unknown tag on building icon");
		s->state_skip++;
	    }
	  break;

	case INSIDE_DEVICELIST:
	  if (!strcmp(name, "device"))
	     s->state = INSIDE_DEVICE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICELIST:
	  if (!strcmp(name, "service"))
	     s->state = INSIDE_SERVICE;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_SERVICE:
	  if (!s->service)
	    {
	       if (!d->base_url)
	         {
		     DEBUG_D(_log_dom, "Base URL not set, parsing from location %s", d->location);
		     int base_url_len = eupnp_utils_url_base_get(d->location);
		     DEBUG_D(_log_dom, "Base URL len is %d", base_url_len);
		     if (base_url_len) COPY_CHARACTERS(d->base_url, d->location, base_url_len);
		     DEBUG_D(_log_dom, "Base URL set is %s", d->base_url);
		 }

	       s->service = eupnp_service_info_new(d->udn, d->base_url, NULL, NULL, NULL);
	       if (!s->service)
	         {
		    ERROR_D(_log_dom, "Could not alloc memory for service");
		    s->state_skip++;
		    break;
		 }
	    }
	  if (!strcmp(name, "serviceType"))
	     s->state = INSIDE_SERVICE_TYPE;
	  else if (!strcmp(name, "serviceId"))
	     s->state = INSIDE_SERVICE_ID;
	  else if (!strcmp(name, "SCPDURL"))
	     s->state = INSIDE_SERVICE_SCPDURL;
	  else if (!strcmp(name, "controlURL"))
	     s->state = INSIDE_SERVICE_CONTROL_URL;
	  else if (!strcmp(name, "eventSubURL"))
	     s->state = INSIDE_SERVICE_EVENTSUB_URL;
	  else
	     s->state_skip++;
	  break;

	default:
	   s->state_skip++;
     }
}

static void
end_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI)
{
   Eupnp_Device_Parser_State *s = state;
   Eupnp_Device_Info *d = s->device;

   DEBUG_D(_log_dom, "End element at %s, prefix=%s, URI=%s", name, prefix, URI);

   if (s->state_skip)
     {
	s->state_skip--;
	return;
     }

   switch(s->state)
     {
	case INSIDE_ROOT:
	  s->state = FINISH;
	  _embedded_devices_parse_finish(d);
	  break;

	case INSIDE_DEVICE:
	  if (!s->parent)
	    {
	       DEBUG_D(_log_dom, "Finished building device.");
	       s->state = INSIDE_ROOT;
	       break;
	    }
	  else
	    {
	       eupnp_device_info_device_add(s->parent->device, s->device);
	       eupnp_device_info_unref(s->device);
	       DEBUG_D(_log_dom, "Finished building embedded device");
	    }

	  s->state = INSIDE_DEVICELIST;
	  break;

	case INSIDE_DEVICELIST:
	  if (!s->parent)
	    {
	       // No parent means root
	       DEBUG_D(_log_dom, "Finished building device list");
	       s->state = INSIDE_ROOT;
	       break;
	    }

	  DEBUG_D(_log_dom, "Switching context %p -> %p", s, s->parent);

	  // Switch back to previous context
	  s->ctx->userData = s->parent;
	  free(s);
	  break;

	case INSIDE_SPEC_VERSION:
	case INSIDE_URLBASE:
	  s->state = INSIDE_ROOT;
	  break;

	case INSIDE_SPEC_VERSION_MAJOR:
	case INSIDE_SPEC_VERSION_MINOR:
	  s->state = INSIDE_SPEC_VERSION;
	  break;

	case INSIDE_ICON_LIST:
	case INSIDE_UDN:
	case INSIDE_SERIAL_NUMBER:
	case INSIDE_MODEL_URL:
	case INSIDE_MODEL_DESCRIPTION:
	case INSIDE_MODEL_NAME:
	case INSIDE_UPC:
	case INSIDE_PRESENTATION_URL:
	case INSIDE_MANUFACTURER_URL:
	case INSIDE_MANUFACTURER:
	case INSIDE_DEVICE_FRIENDLYNAME:
	case INSIDE_DEVICETYPE:
	case INSIDE_SERVICELIST:
	  s->state = INSIDE_DEVICE;
	  break;

	case INSIDE_ICON_MIMETYPE:
	case INSIDE_ICON_WIDTH:
	case INSIDE_ICON_HEIGHT:
	case INSIDE_ICON_DEPTH:
	case INSIDE_ICON_URL:
	  s->state = INSIDE_ICON;
	  break;

	case INSIDE_ICON:
	  s->state = INSIDE_ICON_LIST;
	  eupnp_device_info_icon_add(d, s->icon);
	  s->icon = NULL;
	  break;

	case INSIDE_SERVICE:
	  s->state = INSIDE_SERVICELIST;
	  eupnp_device_info_service_add(d, s->service);
	  eupnp_service_info_unref(s->service);
	  s->service = NULL;
	  break;

	case INSIDE_SERVICE_TYPE:
	case INSIDE_SERVICE_ID:
	case INSIDE_SERVICE_SCPDURL:
	case INSIDE_SERVICE_CONTROL_URL:
	case INSIDE_SERVICE_EVENTSUB_URL:
	  s->state = INSIDE_SERVICE;
	  break;
     }

}



static Eupnp_Device_Parser *
eupnp_device_parser_new(const char *first_chunk, int first_chunk_len, Eupnp_Device_Info *d)
{
   if (first_chunk_len < 4)
     {
	WARN_D(_log_dom, "First chunk length less than 4 chars, user must provide more than 4.");
	return NULL;
     }

   Eupnp_Device_Parser *p;

   p = calloc(1, sizeof(Eupnp_Device_Parser));
   if (!p)
     {
	ERROR_D(_log_dom, "Failed to alloc for device parser");
	return NULL;
     }

   p->handler.initialized = XML_SAX2_MAGIC;
   p->handler.characters = &_characters;
   p->handler.error = &error;
   p->handler.startElementNs = &start_element_ns;
   p->handler.endElementNs = &end_element_ns;

   /*
    * Setup parser state to START, attach the device info object that
    * will get data written into.
    */
   p->state.device = d;

   p->ctx = xmlCreatePushParserCtxt(&p->handler, &p->state, first_chunk,
				    first_chunk_len, NULL);

   p->state.ctx = p->ctx;

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
eupnp_device_parser_free(Eupnp_Device_Parser *p)
{
   if (!p) return;
   if (p->ctx) xmlFreeParserCtxt(p->ctx);
   free(p);
}

static Eina_Bool
eupnp_device_parse_finish(Eupnp_Device_Info *d)
{
   if (!d) return;
   if (!d->xml_parser) return;
   DEBUG_D(_log_dom, "Device parse finish");

   Eina_Bool ret;
   Eupnp_Device_Parser *parser = d->xml_parser;
   ret = xmlParseChunk(parser->ctx, NULL, 0, 1);
   eupnp_device_parser_free(parser);
   d->xml_parser = NULL;
   return !ret;
}

static void
eupnp_device_parse_check_finished(Eupnp_Device_Info *d)
{
   Eupnp_Device_Parser *p = d->xml_parser;

   if (p->state.state == FINISH)
      eupnp_device_parse_finish(d);
}

/*
 * Public API
 */

int EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED = 0;

Eina_Bool
eupnp_device_parser_init(void)
{
   xmlInitParser();

   EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED = eina_error_msg_register("Parser feeded with less than 4 chars. Feed it with at least 4 chars");

   if ((_log_dom = eina_log_domain_register("Eupnp.DeviceParser", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for device parser module.");
	goto log_dom_error;
     }

   INFO_D(_log_dom, "Initializing device parser module.");

   return EINA_TRUE;

   log_dom_error:
      xmlCleanupParser();
      return EINA_FALSE;
}


Eina_Bool
eupnp_device_parser_shutdown(void)
{
   INFO_D(_log_dom, "Shutting down device parser module.");

   eina_log_domain_unregister(_log_dom);

   /* http://xmlsoft.org/html/libxml-parser.html#xmlCleanupParser
    *
    * "One should call xmlCleanupParser() only when the process has finished
    * using the library and all XML/HTML documents built with it."
    *
    */
   xmlCleanupParser();

   return EINA_TRUE;
}


/**
 * @return EINA_TRUE if parsed succesfully, EINA_FALSE otherwise.
 */
Eina_Bool
eupnp_device_parse_xml_buffer(const char *buffer, int buffer_len, Eupnp_Device_Info *d)
{
   Eina_Bool ret = EINA_FALSE;
   if (!d) return ret;
   if (!buffer) return ret;
   if (!buffer_len) return ret;

   Eupnp_Device_Parser *parser;

   if (!d->xml_parser)
     {
	/* Creates the parser, which parses the first chunk */

	d->xml_parser = eupnp_device_parser_new(buffer, buffer_len, d);

	if (!d->xml_parser)
          {
	     ERROR_D(_log_dom, "Failed to parse first chunk");
	     goto parse_ret;
	  }

        ret = EINA_TRUE;
	goto parse_ret;
     }

   parser = d->xml_parser;

   if (!parser->ctx)
     {
	// first_chunk_len < 4 case
	eina_error_set(EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED);
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
      eupnp_device_parse_check_finished(d);
      return ret;
}

