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
#include <ctype.h>
#include <string.h>
#include <Eina.h>

#include <libxml/SAX2.h>


#include "Eupnp_AV.h"

#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)

#define STR(x) #x
#define XSTR(x) STR(x)

#define CHECK_NULL_RET(x, ...)						\
  do {									\
        if (x == NULL) {                                                \
	   WRN("%s == NULL!", XSTR(x));					\
	   return __VA_ARGS__;						\
        }                                                               \
    } while (0)



#define MATCH(attr) !strncmp(attributes[index], attr, strlen(attr))

#define COPYATTR(index) strndup(attributes[index+3], \
			        attributes[index+4] - attributes[index+3])

#define SHAREATTR(index) eina_stringshare_add_length(attributes[index+3], \
						     attributes[index+4] - \
						     attributes[index+3])

#define UNSHARE(p) eina_stringshare_del(p)


static int _log_dom  = -1;

enum {
   START,
   DIDLLITE,
   ITEM,
   ITEM_TAG,
   RES,
   CONTAINER,
   CONTAINER_TAG,
   ERROR,
   END
};

typedef struct _Context Context;

struct _Context
{
   int state;

   /* build callbacks */
   Eupnp_AV_DIDL_Item_Parsed_Cb      parsed_item;
   Eupnp_AV_DIDL_Container_Parsed_Cb parsed_container;

   /* building objects */
   DIDL_Container *container;
   DIDL_Item      *item;
   DIDL_Resource  *res;
   int skip;

   /* user data */
   void *data;
   char *tag;
};

static void
eupnp_av_item_dump(DIDL_Item *item)
{
   CHECK_NULL_RET(item);

   if (!eina_log_domain_level_check(_log_dom, EINA_LOG_LEVEL_DBG))
      return;

   Eina_List *l;
   DIDL_Resource *res;

   DBG("Item refId=%s", item->refID);

   EINA_LIST_FOREACH(item->res, l, res)
     {
	DBG("\tResource");
	DBG("\t\timportUri: %s", res->importUri);
	DBG("\t\tprotocolInfo: %s", res->protocolInfo);
	DBG("\t\tsize: %ld", res->size);
	DBG("\t\tduration: %s", res->duration);
	DBG("\t\tbitrate: %u", res->bitrate);
	DBG("\t\tsampleFrequency: %u", res->sampleFrequency);
	DBG("\t\tbitsPerSample: %u", res->bitsPerSample);
	DBG("\t\tnrAudioChannels: %u", res->nrAudioChannels);
	DBG("\t\tresolution: %s", res->resolution);
	DBG("\t\tcolorDepth: %u", res->colorDepth);
	DBG("\t\tprotection: %s", res->protection);
	DBG("\t\tvalue: %s", res->value);
     }
}

static void
eupnp_av_item_res_append(void *_item, void *_res)
{
   CHECK_NULL_RET(_item);
   CHECK_NULL_RET(_res);

   DIDL_Item *item = _item;
   DIDL_Resource *res = _res;

   item->res = eina_list_append(item->res, res);
}

static void
eupnp_av_didl_object_parse_basic_attrs(DIDL_Object *obj, int nb_attributes,
					    int nb_defaulted __UNUSED__,
					    const char **attributes)
{
   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
	if (MATCH("id"))
	  obj->id = COPYATTR(index);
	else if (MATCH("parentID"))
	  obj->parentID = SHAREATTR(index);
	else if (MATCH("creator"))
	  obj->creator = SHAREATTR(index);
	else if (MATCH("restricted"))
	  obj->restricted = !strncmp(attributes[index+3], "true",
				     strlen("true"));
	else if (MATCH("writeStatus"))
	  obj->writeStatus = SHAREATTR(index);
     }
}

static void
eupnp_av_didl_object_parse_item_attrs(DIDL_Item *item, int nb_attributes,
				      int nb_defaulted,
				      const char **attributes)
{
   eupnp_av_didl_object_parse_basic_attrs(&item->parent, nb_attributes,
					  nb_defaulted, attributes);

   // Parse item-specific attributes
}

static void
eupnp_av_didl_object_parse_container_attrs(DIDL_Container *c, int nb_attributes,
					    int nb_defaulted,
					    const char **attributes)
{
   eupnp_av_didl_object_parse_basic_attrs(&c->parent, nb_attributes,
					  nb_defaulted, attributes);

   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
	if (MATCH("searchable"))
	  c->searchable = !strcmp(attributes[index+3], "true");
	else if (MATCH("childCount"))
	  {
	     char *copy = COPYATTR(index);
	     c->childCount = atoi(copy);
	     free(copy);
	  }
     }
}

static void
eupnp_av_didl_object_parse_res_attrs(DIDL_Resource *r, int nb_attributes,
				     int nb_defaulted __UNUSED__,
				     const char **attributes)
{
   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
	char *copy;

	if (MATCH("importUri"))
	   r->importUri = COPYATTR(index);
	else if (MATCH("protocolInfo"))
	   r->protocolInfo = SHAREATTR(index);
	else if (MATCH("size"))
	  {
	     copy = COPYATTR(index);
	     r->size = atol(copy);
	     free(copy);
	  }
	else if (MATCH("duration"))
	   r->duration = COPYATTR(index);
	else if (MATCH("bitrate"))
	  {
	     copy = COPYATTR(index);
	     r->bitrate = atoi(copy);
	     free(copy);
	  }
	else if (MATCH("sampleFrequency"))
	  {
	     copy = COPYATTR(index);
	     r->sampleFrequency= atoi(copy);
	     free(copy);
	  }
	else if (MATCH("bitsPerSample"))
	  {
	     copy = COPYATTR(index);
	     r->bitsPerSample = atoi(copy);
	     free(copy);
	  }
	else if (MATCH("nrAudioChannels"))
	  {
	     copy = COPYATTR(index);
	     r->nrAudioChannels = atoi(copy);
	     free(copy);
	  }
	else if (MATCH("resolution"))
	  r->resolution = SHAREATTR(index);
	else if (MATCH("colorDepth"))
	  {
	     copy = COPYATTR(index);
	     r->colorDepth = atoi(copy);
	     free(copy);
	  }
	else if (MATCH("protection"))
	  r->protection = SHAREATTR(index);
     }
}

static void
eupnp_av_element_ns_start(void *ctx, const char *name,
			  const xmlChar *prefix __UNUSED__,
			  const xmlChar *URI __UNUSED__, int nb_namespaces __UNUSED__,
			  const xmlChar **namespaces __UNUSED__, int nb_attributes,
			  int nb_defaulted, const char **attributes)
{
   Context *c = ctx;

   switch (c->state)
     {
	case START:
	  if (!strcmp(name, "DIDL-Lite"))
	     c->state = DIDLLITE;
	  else
	     c->skip++;
	  break;
	case DIDLLITE:
	  if (!strcmp(name, "item"))
	    {
	       c->state = ITEM;
	       c->item = calloc(1, sizeof(DIDL_Item));
	       eupnp_av_didl_object_parse_item_attrs(c->item, nb_attributes,
						     nb_defaulted, attributes);
	    }
	  else if (!strcmp(name, "container"))
	    {
	       c->state = CONTAINER;
	       c->container = calloc(1, sizeof(DIDL_Container));
	       eupnp_av_didl_object_parse_container_attrs(c->container,
							  nb_attributes,
							  nb_defaulted,
							  attributes);
	    }
	  else
	     c->skip++;
	  break;
	case ITEM:
	  if (!strcmp(name, "res"))
	    {
	       c->state = RES;
	       c->res = calloc(1, sizeof(DIDL_Resource));
	       eupnp_av_didl_object_parse_res_attrs(c->res, nb_attributes,
						    nb_defaulted, attributes);
	    }
	  else
	    {
	       c->state = ITEM_TAG;
	       c->tag = strdup(name);
	    }
	  break;
	case CONTAINER:
	  c->state = CONTAINER_TAG;
	  c->tag = strdup(name);
	  break;
	default:
	  ERR("Unexpected tag %s", name);
	  c->state = ERROR;
     }
}

static void
eupnp_av_on_characters(void *ctx, const char *ch, int len)
{
   Context *c = ctx;
   const char *end;

   /* trim spaces */
   while (isspace(*ch)) { len--; ch++; }
   end = ch + len - 1;
   while (end > (const char *)ch && isspace(*end)) { end--; len--; }

   switch (c->state)
     {
	case CONTAINER_TAG:
	  if (!strcmp(c->tag, "class"))
	     c->container->parent.cls = eina_stringshare_add_length(ch, len);
	  else if (!strcmp(c->tag, "searchClass"))
	     c->container->searchClass = eina_stringshare_add_length(ch, len);
	  else if (!strcmp(c->tag, "createClass"))
	     c->container->createClass = eina_stringshare_add_length(ch, len);
	  else if (!strcmp(c->tag, "title"))
	    {
	       if (!c->container->parent.title)
		 c->container->parent.title = strndup(ch, len);
	       else
	         {
		    char *tmp = strndup(ch, len);
		    asprintf((char **) &c->container->parent.title, "%s%s", c->container->parent.title, tmp);
		    free(tmp);
		 }
	    }
	  break;
	case ITEM_TAG:
	  if (!strcmp(c->tag, "class"))
	     c->item->parent.cls = eina_stringshare_add_length(ch, len);
	  else if (!strcmp(c->tag, "refID"))
	     c->item->refID= strndup(ch, len);
	  else if (!strcmp(c->tag, "title"))
	    {
	       if (!c->item->parent.title)
		c->item->parent.title = strndup(ch, len);
	       else
	         {
		    char *tmp = strndup(ch, len);
		    asprintf((char **) &c->item->parent.title, "%s%s", c->item->parent.title, tmp);
		    free(tmp);
		 }
	    }
	  break;
	case ITEM:
	  break;
	case CONTAINER:
	  break;
	case RES:
	  c->res->value = strndup(ch, len);
	  break;
     }
}

static void
eupnp_av_element_ns_end(void *ctx, const xmlChar *name,
			const xmlChar *prefix __UNUSED__,
			const xmlChar *URI __UNUSED__)
{
   Context *c = ctx;

   if (c->skip)
     {
	c->skip--;
	return;
     }

   switch (c->state)
     {
	case DIDLLITE:
	  c->state = END;
	  break;
	case CONTAINER:
	  c->parsed_container(c->data, c->container);
	  c->container = NULL;
	  c->state = DIDLLITE;
	  break;
	case ITEM:
	  eupnp_av_item_dump(c->item);
	  c->parsed_item(c->data, c->item);
	  c->item = NULL;
	  c->state = DIDLLITE;
	  break;
	case RES:
	  eupnp_av_item_res_append(c->item, c->res);
	  c->res = NULL;
	  c->state = ITEM;
	  break;
	case ITEM_TAG:
	  c->state = ITEM;
	  if (c->tag) free(c->tag);
	  c->tag = NULL;
	  break;
	case CONTAINER_TAG:
	  c->state = CONTAINER;
	  if (c->tag) free(c->tag);
	  c->tag = NULL;
	  break;
	default:
	  ERR("End element error at tag %s", name);
     }
}

static void
error(void *state __UNUSED__ , const char *msg, ...)
{
   va_list args;
   va_start(args, msg);
   vfprintf(stdout, msg, args);
   va_end(args);
}


/**
 * Parses a DIDL XML fragment.
 *
 * Parses a DIDL XML fragment into DIDL objects (items and containers) and
 * forwards them to the appropriate callback.
 *
 * @param didl_xml DIDL xml fragment
 * @param didl_xml_len Length of the DIDL xml fragment
 * @param item_cb Callback to push items into
 * @param container_cb Callback to push containers into
 * @param data Data to be passed to the callbacks
 *
 * @return EINA_TRUE on success, EINA_FALSE on error.
 * */
EAPI Eina_Bool
eupnp_av_didl_parse(const char *didl_xml,
		    int didl_xml_len,
		    Eupnp_AV_DIDL_Item_Parsed_Cb item_cb,
		    Eupnp_AV_DIDL_Container_Parsed_Cb container_cb,
		    void *data)
{
   Context *c;
   xmlSAXHandler handler;
   xmlParserCtxtPtr ctx;
   Eina_Bool ret = EINA_FALSE;

   CHECK_NULL_RET(didl_xml, EINA_FALSE);
   if (didl_xml_len <= 0) return EINA_FALSE;
   CHECK_NULL_RET(item_cb, EINA_FALSE);
   CHECK_NULL_RET(container_cb, EINA_FALSE);

   c = calloc(1, sizeof(Context));
   c->parsed_item = item_cb;
   c->parsed_container = container_cb;
   c->data = data;

   memset(&handler, 0, sizeof(xmlSAXHandler));
   handler.initialized = XML_SAX2_MAGIC;
   handler.characters = (charactersSAXFunc) &eupnp_av_on_characters;
   handler.startElementNs = (startElementNsSAX2Func) &eupnp_av_element_ns_start;
   handler.endElementNs = &eupnp_av_element_ns_end;
   handler.error = &error;


   ctx = xmlCreatePushParserCtxt(&handler, c, didl_xml, didl_xml_len, NULL);
   if (!xmlParseChunk(ctx, NULL, 0, 1))
      ret = EINA_TRUE;

   free(c);
   return ret;
}

EAPI Eina_Bool
eupnp_av_init(void)
{
   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina.\n");
	return EINA_FALSE;
     }

   if ((_log_dom = eina_log_domain_register("EupnpAV", EINA_COLOR_BLUE)) < 0)
     {
	ERR("Failed to create logging domain for device parser module.");
	goto log_dom_err;
     }

   xmlInitParser();

   return EINA_TRUE;

   log_dom_err:
      eina_shutdown();
      return EINA_FALSE;
}

EAPI Eina_Bool
eupnp_av_shutdown(void)
{
   eina_log_domain_unregister(_log_dom);
   xmlCleanupParser();
   eina_shutdown();
   return EINA_TRUE;
}

static void
eupnp_av_didl_resource_free(DIDL_Resource *res)
{
   CHECK_NULL_RET(res);

   free((char *)res->importUri);
   UNSHARE(res->protocolInfo);
   free((char *)res->duration);
   UNSHARE(res->resolution);
   UNSHARE(res->protection);
   free((char *)res->value);
   free(res);
}

static void
eupnp_av_didl_object_free(DIDL_Object *obj)
{
   CHECK_NULL_RET(obj);

   free((char *)obj->id);
   UNSHARE(obj->parentID);
   free((char *)obj->title);
   UNSHARE(obj->creator);
   UNSHARE(obj->cls);
   UNSHARE(obj->writeStatus);
}

EAPI void
eupnp_av_didl_item_free(DIDL_Item *item)
{
   Eina_List *l;
   DIDL_Resource *res;

   CHECK_NULL_RET(item);

   eupnp_av_didl_object_free(&item->parent);

   UNSHARE(item->refID);

   EINA_LIST_FOREACH(item->res, l, res)
     eupnp_av_didl_resource_free(res);

   free(item);
}

EAPI void
eupnp_av_didl_container_free(DIDL_Container *c)
{
   CHECK_NULL_RET(c);

   eupnp_av_didl_object_free(&c->parent);

   UNSHARE(c->createClass);
   UNSHARE(c->searchClass);
}

EAPI const char *
eupnp_av_didl_object_title_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->title;
}

EAPI const char *
eupnp_av_didl_object_id_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->id;
}

EAPI const char *
eupnp_av_didl_object_parent_id_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->parentID;
}

EAPI const char *
eupnp_av_didl_object_creator_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->creator;
}

EAPI const char *
eupnp_av_didl_object_class_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->cls;
}

EAPI Eina_Bool
eupnp_av_didl_object_restricted_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, EINA_FALSE);
   return obj->restricted;
}

EAPI const char *
eupnp_av_didl_object_write_status_get(const DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->writeStatus;
}

EAPI const char *
eupnp_av_didl_item_ref_id_get(const DIDL_Item *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->refID;
}

EAPI const DIDL_Object *
eupnp_av_didl_item_parent_get(const DIDL_Item *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return (const DIDL_Object *)&obj->parent;
}

EAPI const Eina_List *
eupnp_av_didl_item_resources_get(const DIDL_Item *item)
{
   CHECK_NULL_RET(item, NULL);
   return (const Eina_List *)item->res;
}

EAPI int
eupnp_av_didl_container_child_count_get(const DIDL_Container *obj)
{
   CHECK_NULL_RET(obj, -1);
   return obj->childCount;
}

EAPI const char *
eupnp_av_didl_container_create_class_get(const DIDL_Container *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->createClass;
}

EAPI const char *
eupnp_av_didl_container_search_class_get(const DIDL_Container *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->searchClass;
}

EAPI Eina_Bool
eupnp_av_didl_container_searchable_get(const DIDL_Container *obj)
{
   CHECK_NULL_RET(obj, EINA_FALSE);
   return obj->searchable;
}

EAPI const DIDL_Object *
eupnp_av_didl_container_parent_get(const DIDL_Container *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return (const DIDL_Object *)&obj->parent;
}

EAPI const char *
eupnp_av_didl_resource_import_uri_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->importUri;
}

EAPI const char *
eupnp_av_didl_resource_protocol_info_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->protocolInfo;
}

EAPI unsigned long
eupnp_av_didl_resource_size_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->size;
}

EAPI const char *
eupnp_av_didl_resource_duration_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->duration;
}

EAPI unsigned int
eupnp_av_didl_resource_bitrate_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->bitrate;
}

EAPI unsigned int
eupnp_av_didl_resource_sample_frequency_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->sampleFrequency;
}

EAPI unsigned int
eupnp_av_didl_resource_bits_per_sample_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->bitsPerSample;
}

EAPI unsigned int
eupnp_av_didl_resource_num_audio_channels_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->nrAudioChannels;
}

EAPI const char *
eupnp_av_didl_resource_num_resolution_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->resolution;
}

EAPI unsigned int
eupnp_av_didl_resource_color_depth_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, 0);
   return res->colorDepth;
}

EAPI const char *
eupnp_av_didl_resource_protection_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->protection;
}

EAPI const char *
eupnp_av_didl_resource_value_get(const DIDL_Resource *res)
{
   CHECK_NULL_RET(res, NULL);
   return res->value;
}
