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

#ifndef _EUPNP_DEVICE_PARSER_H
#define _EUPNP_DEVICE_PARSER_H

#include <libxml/SAX2.h>
#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_log.h"
#include "eupnp_device_info.h"
#include "eupnp_service_info.h"
#include "eupnp_service_proxy.h"

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

EAPI extern int EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED;

EAPI Eina_Bool eupnp_device_parse_xml_buffer(const char *buffer, int buffer_len, Eupnp_Device_Info *d);

#endif /* _EUPNP_DEVICE_PARSER_H */
