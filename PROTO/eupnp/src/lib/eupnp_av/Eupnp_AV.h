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

#ifndef _EUPNP_AV_H
#define _EUPNP_AV_H

#include <Eina.h>

typedef struct _DIDL_Resource  DIDL_Resource;
typedef struct _DIDL_Object    DIDL_Object;
typedef struct _DIDL_Container DIDL_Container;
typedef struct _DIDL_Item      DIDL_Item;

typedef void (*Eupnp_AV_DIDL_Container_Parsed_Cb) (void *data,
						DIDL_Container *container);

typedef void (*Eupnp_AV_DIDL_Item_Parsed_Cb)      (void *data,
						DIDL_Item *item);


struct _DIDL_Resource {
   const char *importUri;
   const char *protocolInfo;
   unsigned long size;
   const char *duration;
   unsigned int bitrate;
   unsigned int sampleFrequency;
   unsigned int bitsPerSample;
   unsigned int nrAudioChannels;
   const char *resolution;
   unsigned int colorDepth;
   const char *protection;
};

struct _DIDL_Object {
   const char *id;
   const char *parentID;
   const char *title;
   const char *creator;
   Eina_List *res;
   const char *cls;
   Eina_Bool restricted;
   const char *writeStatus;
};

struct _DIDL_Item {
   DIDL_Object parent;

   const char *refID;
   Eina_List *res;
};

struct _DIDL_Container {
   DIDL_Object parent;

   int childCount;
   const char *createClass;
   const char *searchClass;
   Eina_Bool searchable;
};

Eina_Bool eupnp_av_init(void);
Eina_Bool eupnp_av_shutdown(void);

Eina_Bool eupnp_av_didl_parse(const char *didl_xml,
			      int didl_xml_len,
			      Eupnp_AV_DIDL_Item_Parsed_Cb item_cb,
			      Eupnp_AV_DIDL_Container_Parsed_Cb container_cb,
			      void *data);

#endif
