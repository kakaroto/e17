/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "Azy.h"
#include "azy_private.h"

#define for_each_node(parent, child)                                   \
  {                                                                    \
     xmlNodePtr child;                                                 \
     for (child = parent->xmlChildrenNode; child; child = child->next) \
       {

#define for_each_node_end() }}

#define match_node(_node, _name) \
  (_node->type == XML_ELEMENT_NODE && !strcmp((char *)_node->name, _name))

#define match_comment(_node) \
  (_node->type == XML_COMMENT_NODE)

static const char *
xml_get_cont_str(xmlNodePtr n)
{
   xmlChar *str;
   const char *ret;

   if (!(str = xmlNodeListGetString(n->doc, n->xmlChildrenNode, 1)))
     return NULL;
   ret = eina_stringshare_add((const char *)str);
   xmlFree(str);
   return ret;
}

static int
xml_get_cont_int(xmlNodePtr n)
{
   xmlChar *str;
   int ret;

   if (!(str = xmlNodeListGetString(n->doc, n->xmlChildrenNode, 1)))
     return 0;

   ret = strtol((const char *)str, NULL, 10);
   xmlFree(str);
   return ret;
}

static double
xml_get_cont_double(xmlNodePtr n)
{
   xmlChar *str;
   double ret;

   if (!(str = xmlNodeListGetString(n->doc, n->xmlChildrenNode, 1)))
     return 0;

   ret = strtod((const char *)str, NULL);
   xmlFree(str);
   return ret;
}

static int
xml_get_cont_bool(xmlNodePtr n)
{
   xmlChar *str;
   char bool;

   if (!(str = xmlNodeListGetString(n->doc, n->xmlChildrenNode, 1)))
     return 0;

   bool = *str;
   xmlFree(str);

   if (bool == '0')
     return 0;
   if (bool == '1')
     return 1;
   return -1;
}

/* easy xpath wrapper */

struct nodeset
{
   xmlXPathObjectPtr obj;
   xmlNodePtr       *nodes;
   long              count;
};

static struct nodeset *
xp_eval_nodes(xmlXPathContextPtr ctx,
              const char        *path)
{
   struct nodeset *ns = calloc(1, sizeof(struct nodeset));
   xmlXPathObjectPtr o = xmlXPathEvalExpression((xmlChar *)path, ctx);
   if (o && o->type == XPATH_NODESET && o->nodesetval)
     {
        ns->obj = o;
        ns->nodes = o->nodesetval->nodeTab;
        ns->count = o->nodesetval->nodeNr;
     }
   if (!ns->obj)
     xmlXPathFreeObject(o);
   return ns;
}

static void
xp_free_nodes(struct nodeset *ns)
{
   if (!ns)
     return;
   if (ns->obj)
     xmlXPathFreeObject(ns->obj);
   if (ns)
     free(ns);
}

static const char *
xp_eval_cont_str(xmlXPathContextPtr ctx,
                 const char        *path)
{
   const char *ret = NULL;
   xmlXPathObjectPtr o = xmlXPathEvalExpression((xmlChar *)path, ctx);
   if (o && o->type == XPATH_NODESET && o->nodesetval && o->nodesetval->nodeNr > 0)
     ret = xml_get_cont_str(o->nodesetval->nodeTab[0]);
   if (o)
     xmlXPathFreeObject(o);
   return ret;
}

static int
xp_eval_cont_int(xmlXPathContextPtr ctx,
                 const char        *path,
                 int                fallback)
{
   int ret = fallback;
   xmlXPathObjectPtr o = xmlXPathEvalExpression((xmlChar *)path, ctx);
   if (o && o->type == XPATH_NODESET && o->nodesetval && o->nodesetval->nodeNr > 0)
     ret = xml_get_cont_int(o->nodesetval->nodeTab[0]);
   if (o)
     xmlXPathFreeObject(o);
   return ret;
}

void
azy_value_serialize_xml(xmlNode    *node,
                         Azy_Value *val)
{
   xmlNode *value;
   char buf[50];

   EINA_SAFETY_ON_NULL_RETURN(node);
   EINA_SAFETY_ON_NULL_RETURN(val);

   if (azy_value_type_get(val) != AZY_VALUE_MEMBER)
     node = xmlNewChild(node, NULL, BAD_CAST "value", NULL);

   switch (azy_value_type_get(val))
     {
        Eina_List *l;
        Azy_Value *v;

      case AZY_VALUE_ARRAY:
      {
         value = xmlNewChild(node, NULL, BAD_CAST "array", NULL);
         value = xmlNewChild(value, NULL, BAD_CAST "data", NULL);
         EINA_LIST_FOREACH(azy_value_children_items_get(val), l, v)
           azy_value_serialize_xml(value, v);
         break;
      }

      case AZY_VALUE_STRUCT:
      {
         value = xmlNewChild(node, NULL, BAD_CAST "struct", NULL);
         EINA_LIST_FOREACH(azy_value_children_items_get(val), l, v)
           azy_value_serialize_xml(value, v);
         break;
      }

      case AZY_VALUE_MEMBER:
      {
         value = xmlNewChild(node, NULL, BAD_CAST "member", NULL);
         xmlNewChild(value, NULL, BAD_CAST "name", BAD_CAST azy_value_struct_member_name_get(val));
         azy_value_serialize_xml(value, azy_value_struct_member_value_get(val));
         break;
      }

      case AZY_VALUE_INT:
      {
         int int_val = -1;
         azy_value_int_get(val, &int_val);
         snprintf(buf, sizeof(buf), "%d", int_val);
         xmlNewChild(node, NULL, BAD_CAST "int", BAD_CAST buf);
         break;
      }

      case AZY_VALUE_STRING:
      {
         const char *str_val;
         azy_value_string_get(val, &str_val);
         xmlNewChild(node, NULL, BAD_CAST "string", BAD_CAST str_val);
         eina_stringshare_del(str_val);
         break;
      }

      case AZY_VALUE_BOOL:
      {
         Eina_Bool bool_val = -1;
         azy_value_bool_get(val, &bool_val);
         xmlNewChild(node, NULL, BAD_CAST "boolean", BAD_CAST (bool_val ? "1" : "0"));
         break;
      }

      case AZY_VALUE_DOUBLE:
      {
         double dbl_val = -1;
         azy_value_double_get(val, &dbl_val);
         snprintf(buf, sizeof(buf), "%g", dbl_val);
         xmlNewChild(node, NULL, BAD_CAST "double", BAD_CAST buf);
         break;
      }

      case AZY_VALUE_TIME:
      {
         const char *str_val;
         azy_value_string_get(val, &str_val);
         xmlNewChild(node, NULL, BAD_CAST "dateTime.iso8601", BAD_CAST str_val);
         eina_stringshare_del(str_val);
         break;
      }

      case AZY_VALUE_BASE64:
      {
         xmlNewChild(node, NULL, BAD_CAST "base64", BAD_CAST val->str_val);
         break;
      }
     }
}

Azy_Value *
azy_value_unserialize_xml(xmlNode *node)
{
   Eina_Bool is_string_without_element = EINA_TRUE;

   if (!node)
     return NULL;

   for_each_node(node, tn)

   /* check if node contains only text and entity reference nodes */
   if (tn->type != XML_TEXT_NODE && tn->type != XML_ENTITY_REF_NODE)
     is_string_without_element = EINA_FALSE;

   if (match_node(tn, "int") || match_node(tn, "i4"))
     return azy_value_int_new(xml_get_cont_int(tn));
   else if (match_node(tn, "string"))
     {
        const char *str = xml_get_cont_str(tn);
        Azy_Value *val = azy_value_string_new(str);
        eina_stringshare_del(str);
        return val;
     }
   else if (match_node(tn, "boolean"))
     return azy_value_bool_new(xml_get_cont_bool(tn));
   else if (match_node(tn, "double"))
     return azy_value_double_new(xml_get_cont_double(tn));
   else if (match_node(tn, "dateTime.iso8601"))
     {
        const char *str = xml_get_cont_str(tn);
        Azy_Value *val = azy_value_time_new(str);
        eina_stringshare_del(str);
        return val;
     }
   else if (match_node(tn, "base64"))
     {
        Azy_Value *b;
        const char *base64;

        base64 = xml_get_cont_str(tn);        
        char *buf = azy_base64_decode(base64, eina_stringshare_strlen(base64));
        eina_stringshare_del(base64);
        b = azy_value_base64_new(buf);
        free(buf);
        return b;
     }
   else if (match_node(tn, "array"))
     {
        Azy_Value *arr = azy_value_array_new();
        for_each_node(tn, d)

        if (match_node(d, "data"))
          {
             for_each_node(d, v)

             if (match_node(v, "value"))
               {
                  Azy_Value *elem = azy_value_unserialize_xml(v);

                  if (!elem)
                    {
                       azy_value_unref(arr);
                       return NULL;
                    }

                  azy_value_array_push(arr, elem);
               }

             for_each_node_end()
             return arr;
          }

        for_each_node_end()
        return arr;
     }
   else if (match_node(tn, "struct"))
     {
        Azy_Value *str = azy_value_struct_new();
        for_each_node(tn, m)

        if (match_node(m, "member"))
          {
             const char *name = NULL;
             Azy_Value *val = NULL;
             int names = 0, values = 0;

             for_each_node(m, me)

             if (match_node(me, "name"))
               {
                  if (names++ == 0)
                    name = xml_get_cont_str(me);
               }
             else if (match_node(me, "value"))
               if (values++ == 0)
                 val = azy_value_unserialize_xml(me);

             for_each_node_end()

             if (values != 1 || names != 1)
               {
                  eina_stringshare_del(name);
                  azy_value_unref(val);
                  azy_value_unref(str);
                  return NULL;
               }

             azy_value_struct_member_set(str, name, val);
             eina_stringshare_del(name);
          }

        for_each_node_end()
        return str;
     }

   for_each_node_end()

   if (is_string_without_element)
     {
        xmlChar *str = xmlNodeGetContent(node);
        Azy_Value *val = azy_value_string_new((const char *)str);
        xmlFree(str);
        return val;
     }

   return NULL;
}

Eina_Bool
azy_content_serialize_request_xml(Azy_Content *content)
{
   Eina_List *l;
   Azy_Value *val;
   xmlNode *params;

   if ((!content) || (content->buffer))
     return EINA_FALSE;

   xmlDoc *doc = xmlNewDoc(BAD_CAST "1.0");
   xmlNode *root = xmlNewNode(NULL, BAD_CAST "methodCall");
   xmlDocSetRootElement(doc, root);
   xmlNewChild(root, NULL, BAD_CAST "methodName", (xmlChar *)content->method);
   if (content->params)
     params = xmlNewChild(root, NULL, BAD_CAST "params", NULL);

   EINA_LIST_FOREACH(content->params, l, val)
     {
        xmlNode *param = xmlNewChild(params, NULL, BAD_CAST "param", NULL);
        azy_value_serialize_xml(param, val);
     }

   if (eina_log_domain_level_check(azy_log_dom, EINA_LOG_LEVEL_DBG))
     xmlDocDumpFormatMemoryEnc(doc, (xmlChar **)&content->buffer, (int*)&content->length, "UTF-8", 1);
   else
     xmlDocDumpMemoryEnc(doc, (xmlChar **)&content->buffer, (int*)&content->length, "UTF-8");

   xmlFreeDoc(doc);
   return EINA_TRUE;
}

Eina_Bool
azy_content_serialize_response_xml(Azy_Content *content)
{
   if ((!content) || (content->buffer))
     return EINA_FALSE;

   xmlDoc *doc = xmlNewDoc(BAD_CAST "1.0");
   xmlNode *root = xmlNewNode(NULL, BAD_CAST "methodResponse");
   xmlDocSetRootElement(doc, root);

   if (content->error_set)
     {
        xmlNode *fault = xmlNewChild(root, NULL, BAD_CAST "fault", NULL);
        Azy_Value *v = azy_value_struct_new();
        azy_value_struct_member_set(v, "faultCode", azy_value_int_new(content->faultcode));
        azy_value_struct_member_set(v, "faultString", azy_value_string_new(azy_content_error_message_get(content)));
        azy_value_serialize_xml(fault, v);
        azy_value_unref(v);
     }
   else if (content->retval)
     {
        xmlNode *params = xmlNewChild(root, NULL, BAD_CAST "params", NULL);
        xmlNode *param = xmlNewChild(params, NULL, BAD_CAST "param", NULL);
        azy_value_serialize_xml(param, content->retval);
     }

   if (eina_log_domain_level_check(azy_log_dom, EINA_LOG_LEVEL_DBG))
     xmlDocDumpFormatMemoryEnc(doc, (xmlChar **)&content->buffer, (int*)&content->length, "UTF-8", 1);
   else
     xmlDocDumpMemoryEnc(doc, (xmlChar **)&content->buffer, (int*)&content->length, "UTF-8");

   xmlFreeDoc(doc);
   return EINA_TRUE;
}

Eina_Bool
azy_content_unserialize_request_xml(Azy_Content *content,
                                     const char *buf,
                                     ssize_t     len)
{
   if ((!content) || (!buf))
     return EINA_FALSE;

   xmlDoc *doc = xmlReadMemory(buf, len, 0, 0, XML_PARSE_NOWARNING | XML_PARSE_NOERROR | XML_PARSE_NONET);

   if (!doc)
     {
        azy_content_error_faultcode_set(content, AZY_ERROR_REQUEST_XML_DOC, -1);
        goto err_0;
     }

   xmlNode *root = xmlDocGetRootElement(doc);

   if (!root)
     {
        azy_content_error_faultcode_set(content, AZY_ERROR_REQUEST_XML_ROOT, -1);
        goto err_1;
     }

   xmlXPathContext *ctx = xmlXPathNewContext(doc);

   content->method = xp_eval_cont_str(ctx, "/methodCall/methodName");

   if (!content->method)
     {
        azy_content_error_faultcode_set(content, AZY_ERROR_REQUEST_XML_METHODNAME, -1);
        goto err_2;
     }

   int i;
   struct nodeset *ns = xp_eval_nodes(ctx, "/methodCall/params/param/value");

   for (i = 0; i < ns->count; i++)
     {
        Azy_Value *v = azy_value_unserialize_xml(ns->nodes[i]);

        if (!v)
          {
             azy_content_error_faultmsg_set(content, -1,
                                             "Can't parse XML-RPC XML request. Failed to unserialize parameter %d.", i);
             goto err_3;
          }

        azy_content_param_add(content, v);
     }

   xp_free_nodes(ns);

   xmlXPathFreeContext(ctx);
   xmlFreeDoc(doc);
   return EINA_TRUE;

err_3:
   xp_free_nodes(ns);
err_2:
   xmlXPathFreeContext(ctx);
err_1:
   xmlFreeDoc(doc);
err_0:
   return EINA_FALSE;
}

Eina_Bool
azy_content_unserialize_response_xml(Azy_Content *content,
                                      const char *buf,
                                      ssize_t     len)
{
   if ((!content) || (!buf))
     return EINA_FALSE;

   xmlDoc *doc = xmlReadMemory(buf, len, 0, 0, XML_PARSE_NOWARNING | XML_PARSE_NOERROR | XML_PARSE_NONET);

   if (!doc)
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_DOC);
        goto err_0;
     }

   xmlNode *root = xmlDocGetRootElement(doc);

   if (!root)
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_ROOT);
        goto err_1;
     }

   xmlXPathContext *ctx = xmlXPathNewContext(doc);

   struct nodeset *ns = xp_eval_nodes(ctx, "/methodResponse/params/param/value");

   if (ns->count == 1)
     {
        content->retval = azy_value_unserialize_xml(ns->nodes[0]);

        if (content->retval == NULL)
          {
             azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_RETVAL);
             goto err_2;
          }

        goto done;
     }
   else if (ns->count > 1)     // more than one param is bad
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_MULTI);
        goto err_2;
     }

   xp_free_nodes(ns);

   // ok no params/param, check for fault
   ns = xp_eval_nodes(ctx, "/methodResponse/fault/value");

   if (ns->count == 1)
     {
        content->retval = azy_value_unserialize_xml(ns->nodes[0]);

        if (content->retval == NULL)
          {
             azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_FAULT);
             goto err_2;
          }

        // check if client returned standard XML-RPC error message, we want to process
        // it differently than normal retval
        int errcode = 0;
        const char *errmsg;

        if (azy_value_retval_is_error(content->retval, &errcode, &errmsg))
          {
             azy_content_error_faultmsg_set(content, errcode, "%s", errmsg);
             azy_value_unref(content->retval);
             content->retval = NULL;
             xp_free_nodes(ns);
             xmlXPathFreeContext(ctx);
             xmlFreeDoc(doc);
             return EINA_TRUE;
          }
        else
          {
             azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_INVAL);
             goto err_3;
          }
     }
   else     // no fault either
     {
        azy_content_error_code_set(content, AZY_ERROR_XML_UNSERIAL);
        goto err_2;
     }

done:
   xp_free_nodes(ns);
   xmlXPathFreeContext(ctx);
   xmlFreeDoc(doc);
   return EINA_TRUE;

err_3:
   if (content->retval)
     azy_value_unref(content->retval);
   content->retval = NULL;
err_2:
   xp_free_nodes(ns);
   xmlXPathFreeContext(ctx);
err_1:
   xmlFreeDoc(doc);
err_0:
   return EINA_FALSE;
}

