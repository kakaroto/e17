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

#include <libxml/SAX2.h>

#include <string.h>
#include <Eina.h>

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

#define MATCH(attr) !strncmp(attributes[index], attr, strlen(attr))
#define COPYATTR(index) strndup(attributes[index+3], \
			        attributes[index+4] - attributes[index+3])

static void
eupnp_av_didl_object_parse_basic_attrs(DIDL_Object *obj, int nb_attributes,
					    int nb_defaulted,
					    const xmlChar **attributes)
{
   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
	if (MATCH("id"))
	  obj->id = COPYATTR(index);
	else if (MATCH("parentID"))
	  obj->parentID = COPYATTR(index);
	else if (MATCH("creator"))
	  obj->creator = COPYATTR(index);
	else if (MATCH("restricted"))
	  obj->restricted = !strncmp(attributes[index+3], "true",
				     strlen("true"));
	else if (MATCH("writeStatus"))
	  obj->writeStatus = COPYATTR(index);
     }
}

static void
eupnp_av_didl_object_parse_item_attrs(DIDL_Item *item, int nb_attributes,
				      int nb_defaulted,
				      const xmlChar **attributes)
{
   eupnp_av_didl_object_parse_basic_attrs(&item->parent, nb_attributes,
					  nb_defaulted, attributes);

   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
     }
}

static void
eupnp_av_didl_object_parse_container_attrs(DIDL_Container *c, int nb_attributes,
					    int nb_defaulted,
					    const xmlChar **attributes)
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
				     int nb_defaulted,
				     const xmlChar **attributes)
{
   int i;
   int index = 0;

   for (i = 0; i < nb_attributes; ++i, index += 5)
     {
	char *copy;

	if (MATCH("importUri"))
	   r->importUri = COPYATTR(index);
	else if (MATCH("protocolInfo"))
	   r->protocolInfo = COPYATTR(index);
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
	  r->resolution = COPYATTR(index);
	else if (MATCH("colorDepth"))
	  {
	     copy = COPYATTR(index);
	     r->colorDepth = atoi(copy);
	     free(copy);
	  }
	else if (MATCH("protection"))
	  r->protection = COPYATTR(index);
     }
}

static void
eupnp_av_element_ns_start(void *ctx, const xmlChar *name, const xmlChar *prefix,
			  const xmlChar *URI, int nb_namespaces,
			  const xmlChar **namespaces, int nb_attributes,
			  int nb_defaulted, const xmlChar **attributes)
{
   Context *c = ctx;
   int i;

   // DBG("Start element %s state %d", name, c->state);

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

   // DBG("After start element %s state %d", name, c->state);
}

static void
eupnp_av_on_characters(void *ctx, const xmlChar *ch, int len)
{
   Context *c = ctx;
   const char *end;

   /* trim spaces */
   while (isspace(*ch)) { len--; ch++; }
   end = ch + len - 1;
   while (end > (const char *)ch && isspace(*end)) { end--; len--; }

   // DBG("On characters state %d", c->state);

   switch (c->state)
     {
	case CONTAINER_TAG:
	  if (!strcmp(c->tag, "class"))
	     c->container->parent.cls = strndup(ch, len);
	  else if (!strcmp(c->tag, "searchClass"))
	     c->container->searchClass = strndup(ch, len);
	  else if (!strcmp(c->tag, "createClass"))
	     c->container->createClass = strndup(ch, len);
	  else if (!strcmp(c->tag, "title"))
	     c->container->parent.title = strndup(ch, len);
	  break;
	case ITEM_TAG:
	  if (!strcmp(c->tag, "class"))
	     c->item->parent.cls = strndup(ch, len);
	  else if (!strcmp(c->tag, "refID"))
	     c->item->refID= strndup(ch, len);
	  else if (!strcmp(c->tag, "title"))
	     c->item->parent.title = strndup(ch, len);
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
eupnp_av_element_ns_end(void *ctx, const xmlChar *name, const xmlChar *prefix,
			const xmlChar *URI)
{
   Context *c = ctx;

   // DBG("End element %s state %d", name, c->state);

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

   // DBG("End element %s state %d", name, c->state);
}

static void
error(void *state, const char *msg, ...)
{
   va_list args;
   va_start(args, msg);
   vfprintf(stdout, msg, args);
   va_end(args);
}

Eina_Bool
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

   c = calloc(1, sizeof(Context));
   c->parsed_item = item_cb;
   c->parsed_container = container_cb;
   c->data = data;

   memset(&handler, 0, sizeof(xmlSAXHandler));
   handler.initialized = XML_SAX2_MAGIC;
   handler.characters = &eupnp_av_on_characters;
   handler.startElementNs = &eupnp_av_element_ns_start;
   handler.endElementNs = &eupnp_av_element_ns_end;
   handler.error = &error;


   ctx = xmlCreatePushParserCtxt(&handler, c, didl_xml, didl_xml_len, NULL);
   if (!xmlParseChunk(ctx, NULL, 0, 1))
      ret = EINA_TRUE;

   free(c);
   return ret;
}

Eina_Bool
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

Eina_Bool
eupnp_av_shutdown(void)
{
   eina_log_domain_unregister(_log_dom);
   xmlCleanupParser();
   eina_shutdown();
   return EINA_TRUE;
}

const char *
eupnp_av_didl_object_title_get(DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->title;
}

const char *
eupnp_av_didl_object_id_get(DIDL_Object *obj)
{
   CHECK_NULL_RET(obj, NULL);
   return obj->id;
}
