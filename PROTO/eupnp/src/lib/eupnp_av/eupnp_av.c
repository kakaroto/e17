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
#include "eupnp_private.h"

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
	if (MATCH("parentID"))
	  obj->parentID = COPYATTR(index);
	if (MATCH("title"))
	  obj->title = COPYATTR(index);
	if (MATCH("creator"))
	  obj->creator = COPYATTR(index);
	if (MATCH("restricted"))
	  obj->restricted = !strncmp(attributes[index+3], "true",
				     strlen("true"));
	if (MATCH("writeStatus"))
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
	if (MATCH("childCount"))
	  {
	     char *copy = COPYATTR(index);
	     c->childCount = atoi(copy);
	     free(copy);
	  }
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
	  break;
	case ITEM_TAG:
	  if (!strcmp(c->tag, "class"))
	     c->item->parent.cls = strndup(ch, len);
	  else if (!strcmp(c->tag, "refID"))
	     c->item->refID= strndup(ch, len);
	  break;
	case ITEM:
	  break;
	case CONTAINER:
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
	ERROR("Failed to create logging domain for device parser module.");
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
