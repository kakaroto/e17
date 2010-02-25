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

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef E_UPNP_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! E_UPNP_BUILD */
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

typedef struct _DIDL_Resource  DIDL_Resource;
typedef struct _DIDL_Object    DIDL_Object;
typedef struct _DIDL_Container DIDL_Container;
typedef struct _DIDL_Item      DIDL_Item;

#define DIDL_OBJECT_GET(i) ((DIDL_Object *)i)

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

   const char *value;
};

struct _DIDL_Object {
   const char *id;
   const char *parentID;
   const char *title;
   const char *creator;
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

EAPI Eina_Bool eupnp_av_init       (void);
EAPI Eina_Bool eupnp_av_shutdown   (void);

EAPI Eina_Bool eupnp_av_didl_parse (const char *didl_xml, int didl_xml_len,
				    Eupnp_AV_DIDL_Item_Parsed_Cb item_cb,
				    Eupnp_AV_DIDL_Container_Parsed_Cb container_cb,
				    void *data) EINA_ARG_NONNULL(1, 3, 4);

EAPI const char      *eupnp_av_didl_object_title_get        (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const char      *eupnp_av_didl_object_id_get           (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const char      *eupnp_av_didl_object_parent_id_get    (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const char      *eupnp_av_didl_object_creator_get      (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const Eina_List *eupnp_av_didl_object_resources_get    (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const char      *eupnp_av_didl_object_class_get        (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI Eina_Bool        eupnp_av_didl_object_restricted_get   (DIDL_Object *obj) EINA_ARG_NONNULL(1);
EAPI const char      *eupnp_av_didl_object_write_status_get (DIDL_Object *obj) EINA_ARG_NONNULL(1);

EAPI const char        *eupnp_av_didl_item_ref_id_get (DIDL_Item *obj) EINA_ARG_NONNULL(1);
EAPI const DIDL_Object *eupnp_av_didl_item_parent_get (DIDL_Item *obj) EINA_ARG_NONNULL(1);
EAPI void               eupnp_av_didl_item_free       (DIDL_Item *obj) EINA_ARG_NONNULL(1);
EAPI const Eina_List   *eupnp_av_didl_item_resources_get(DIDL_Item *item) EINA_ARG_NONNULL(1);

EAPI int                eupnp_av_didl_container_child_count_get  (DIDL_Container *obj) EINA_ARG_NONNULL(1);
EAPI const char        *eupnp_av_didl_container_create_class_get (DIDL_Container *obj) EINA_ARG_NONNULL(1);
EAPI const char        *eupnp_av_didl_container_search_class_get (DIDL_Container *obj) EINA_ARG_NONNULL(1);
EAPI Eina_Bool          eupnp_av_didl_container_searchable_get   (DIDL_Container *obj) EINA_ARG_NONNULL(1);
EAPI const DIDL_Object *eupnp_av_didl_container_parent_get       (DIDL_Container *obj) EINA_ARG_NONNULL(1);

EAPI const char    *eupnp_av_didl_resource_import_uri_get        (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI const char    *eupnp_av_didl_resource_protocol_info_get     (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned long  eupnp_av_didl_resource_size_get              (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI const char    *eupnp_av_didl_resource_duration_get          (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned int   eupnp_av_didl_resource_bitrate_get           (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned int   eupnp_av_didl_resource_sample_frequency_get  (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned int   eupnp_av_didl_resource_bits_per_sample_get   (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned int   eupnp_av_didl_resource_num_audio_channels_get(DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI const char    *eupnp_av_didl_resource_num_resolution_get    (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI unsigned int   eupnp_av_didl_resource_color_depth_get       (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI const char    *eupnp_av_didl_resource_protection_get        (DIDL_Resource *res) EINA_ARG_NONNULL(1);
EAPI const char    *eupnp_av_didl_resource_value_get             (DIDL_Resource *res) EINA_ARG_NONNULL(1);

#endif
