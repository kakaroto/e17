/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Azy.h"
#include "azy_private.h"
#include <errno.h>

#include "pugixml.hpp"
#include <iterator>

using namespace pugi;

struct xml_memory_writer : xml_writer
{
   char  *buffer;
   size_t capacity;

   size_t result;

   xml_memory_writer() : buffer(0), capacity(0), result(0)
   {
   }

   xml_memory_writer(char  *buffer,
                     size_t capacity) : buffer(buffer), capacity(capacity), result(0)
   {
   }

   size_t
   written_size() const
   {
      return result < capacity ? result : capacity;
   }

   virtual void
   write(const void *data,
         size_t      size)
   {
      if (result < capacity)
        {
           size_t chunk = (capacity - result < size) ? capacity - result : size;
           memcpy(buffer + result, data, chunk);
        }

      result += size;
   }
};

static char *
azy_content_xmlnode_to_buf(xml_node       node,
                           int64_t *len)
{
   xml_memory_writer counter;
   char *buffer;

   node.print(counter);
   buffer = static_cast<char*>(calloc(1, counter.result + 1));
   xml_memory_writer writer(buffer, counter.result);
   node.print(writer);
   buffer[writer.written_size()] = 0;
   *len = static_cast<int64_t> (writer.written_size());

   return buffer;
}

static Azy_Value *
azy_value_deserialize_xml(xml_node node)
{
   const char *name;

   if (node.empty()) return NULL;

   name = node.name();
   if ((!name) || (!name[0])) return NULL;

   switch (name[0])
     { /* yaaay micro-optimizing! */
      case 'b':
        if (std::distance(node.begin(), node.end()) != 1)
          return NULL;

        if (!strcmp(name + 1, "oolean"))
          {
             int x;

             x = strtol(node.child_value(), NULL, 10);
             if ((x != 0) && (x != 1)) return NULL;
             return azy_value_bool_new(x);
          }
        if (!strcmp(name + 1, "ase64"))
          return azy_value_base64_new(node.child_value());

      case 'i':
        if (std::distance(node.begin(), node.end()) != 1)
          return NULL;
      
        if ((!strcmp(name + 1, "nt")) || (!strcmp(name + 1, "4")))
          {
             int x;

             errno = 0;
             x = strtol(node.child_value(), NULL, 10);
             if (errno) return NULL;
             return azy_value_int_new(x);
          }

      case 's':
        if (!strcmp(name + 1, "tring"))
          {
             if (std::distance(node.begin(), node.end()) > 1)
               return NULL;
             return azy_value_string_new(node.child_value());
          }
        if (!strcmp(name + 1, "truct"))
          {
             Azy_Value *v;

             if (std::distance(node.begin(), node.end()) < 1)
               return NULL;
             node = node.child("member");
             if (node.empty()) return NULL;

             v = azy_value_struct_new();
             if (!v) return NULL;
             for (xml_node it = node; it; it = it.next_sibling())
               {
                  Azy_Value *val;
                  const char *name;

                  if ((std::distance(it.begin(), it.end()) != 2) || /* member */
                      (std::distance(it.first_child().begin(), it.first_child().end()) != 1) || /* name/value */
                      (std::distance(it.first_child().first_child().begin(), it.first_child().first_child().end()))) /* name */
                    {
                       azy_value_unref(v);
                       return NULL;
                    }
                  name = it.child("name").child_value();
                  if (!name)
                    {
                       ERR("%s", eina_error_msg_get(AZY_ERROR_XML_UNSERIAL));
                       azy_value_unref(v);
                       return NULL;
                    }

                  val = azy_value_deserialize_xml(it.child("value").first_child());
                  azy_value_struct_member_set(v, name, val);
               }
             return v;
          }

      case 'd':
        if (std::distance(node.begin(), node.end()) != 1)
          return NULL;
          
        if (!strcmp(name + 1, "ouble"))
          {
             double x;

             errno = 0;
             x = strtod(node.child_value(), NULL);
             if (errno) return NULL;
             return azy_value_double_new(x);
          }
        if (!strcmp(name + 1, "ateTime.iso8601"))
          return azy_value_time_new(node.child_value());

      case 'a':
        if (std::distance(node.begin(), node.end()) != 1)
          return NULL;
          
        if (!strcmp(name + 1, "rray"))
          {
             Azy_Value *v;

             node = node.child("data");
             if (node.empty()) return NULL;
             if (std::distance(node.begin(), node.end()) < 1)
               return NULL;
             node = node.child("value");
             if (node.empty()) return NULL;

             v = azy_value_array_new();
             if (!v) return NULL;
             for (xml_node it = node; it; it = it.next_sibling())
               {
                  Azy_Value *val;

                  if (std::distance(node.begin(), node.end()) != 1)
                    {
                       azy_value_unref(v);
                       ERR("%s", eina_error_msg_get(AZY_ERROR_XML_UNSERIAL));
                       return NULL;
                    }
                  val = azy_value_deserialize_xml(it.first_child());
                  if (!val)
                    {
                       azy_value_unref(v);
                       ERR("%s", eina_error_msg_get(AZY_ERROR_XML_UNSERIAL));
                       return NULL;
                    }
                  azy_value_array_push(v, val);
               }
             return v;
          }

      default:
        return NULL;
     }

   return NULL;
}

static void
azy_value_serialize_xml(xml_node   node,
                        Azy_Value *val)
{
   EINA_SAFETY_ON_TRUE_RETURN(node.empty());
   EINA_SAFETY_ON_NULL_RETURN(val);

   if (val->type != AZY_VALUE_MEMBER)
     node = node.append_child("value");

   switch (val->type)
     {
        Eina_List *l;
        void *v;
        char buf[128];

      case AZY_VALUE_ARRAY:
        node = node.append_child("array").append_child("data");
        EINA_LIST_FOREACH(val->children, l, v)
          azy_value_serialize_xml(node, static_cast<Azy_Value *>(v));
        return;

      case AZY_VALUE_STRUCT:
        node = node.append_child("struct");
        EINA_LIST_FOREACH(val->children, l, v)
          azy_value_serialize_xml(node, static_cast<Azy_Value *>(v));
        return;

      case AZY_VALUE_MEMBER:
        node = node.append_child("member");
        node.append_child("name").append_child(node_pcdata).set_value(val->member_name);
        azy_value_serialize_xml(node, val->member_value);
        return;

      case AZY_VALUE_INT:
        snprintf(buf, sizeof(buf), "%i", val->int_val);
        node.append_child("int").append_child(node_pcdata).set_value(buf);
        return;

      case AZY_VALUE_STRING:
        node.append_child("string").append_child(node_pcdata).set_value(val->str_val);
        return;

      case AZY_VALUE_BOOL:
        node.append_child("boolean").append_child(node_pcdata).set_value(val->int_val ? "1" : "0");
        return;

      case AZY_VALUE_DOUBLE:
        snprintf(buf, sizeof(buf), "%g", val->dbl_val);
        node.append_child("double").append_child(node_pcdata).set_value(buf);
        return;

      case AZY_VALUE_TIME:
        node.append_child("dateTime.iso8601").append_child(node_pcdata).set_value(val->str_val);
        return;

      case AZY_VALUE_BASE64:
        node.append_child("base64").append_child(node_pcdata).set_value(val->str_val);
        return;
     }
}

Eina_Bool
azy_content_serialize_request_xml(Azy_Content *content)
{
   xml_document doc;
   xml_node node;
   Eina_List *l;
   void *val;

   node = doc.append_child("methodCall");
   node.append_child("methodName").append_child(node_pcdata).set_value(content->method);
   if (content->params)
     node = node.append_child("params");

   EINA_LIST_FOREACH(content->params, l, val)
     {
        xml_node p;

        p = node.append_child("param");
        azy_value_serialize_xml(p, static_cast<Azy_Value *>(val));
     }
   content->buffer = (unsigned char *)azy_content_xmlnode_to_buf(doc, &content->length);
   return EINA_TRUE;
}

Eina_Bool
azy_content_serialize_response_xml(Azy_Content *content)
{
   xml_document doc;
   xml_node node, m;

   node = doc.append_child("methodResponse");
   if (content->error_set)
     {
        char buf[16];

        node = node.append_child("fault").append_child("value").append_child("struct");
        m = node.append_child("member");
        m.append_child("name").append_child(node_pcdata).set_value("faultCode");
        snprintf(buf, sizeof(buf), "%i", content->faultcode);
        m.append_child("value").append_child("int").append_child(node_pcdata).set_value(buf);
        m = node.append_child("member");
        m.append_child("name").append_child(node_pcdata).set_value("faultString");
        m.append_child("value").append_child("string").append_child(node_pcdata).set_value(azy_content_error_message_get(content));
     }
   else
     {
        node = node.append_child("params").append_child("param");
        azy_value_serialize_xml(node, content->retval);
     }

   content->buffer = (unsigned char *)azy_content_xmlnode_to_buf(doc, &content->length);
   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_request_xml(Azy_Content *content,
                                    const char  *buf,
                                    ssize_t      len)
{
   xml_document doc;
   xpath_node_set params, name;
   const char *mn;
   static xpath_query pquery("/methodCall/params/param/value");
   static xpath_query nquery("/methodCall/methodName");
   ptrdiff_t num;

   if ((!content) || (!buf)) return EINA_FALSE; /* FIXME: ESNV? */

   if (!doc.load_buffer(const_cast<char *>(buf), len))
     {
        azy_content_error_code_set(content, AZY_ERROR_REQUEST_XML_DOC);
        return EINA_FALSE;
     }

   if ((!doc.first_child()) || (doc.first_child() != doc.last_child())) /* more than methodCall at root */
     return EINA_FALSE;

   num = std::distance(doc.first_child().begin(), doc.first_child().end());

   if ((num != 2) && (num != 1)) /* not params/methodName */
     return EINA_FALSE;
   
   name = nquery.evaluate_node_set(doc);
   if (name.empty())
     {
        azy_content_error_code_set(content, AZY_ERROR_REQUEST_XML_METHODNAME);
        return EINA_FALSE;
     }

   if (!name.first().node().first_child().first_child().empty()) /* that's an error */
     return EINA_FALSE;
    
   mn = name.first().node().child_value();
   if ((!mn) || (!mn[0])) return EINA_FALSE;
   content->method = eina_stringshare_add(mn);

   params = pquery.evaluate_node_set(doc);
   if (!params.empty())
     {
        xpath_node_set fc, fs;
        xml_node v, p;

        v = params.first().node(); /* <value> */
        p = v.parent(); /* <param> */
        if (std::distance(p.parent().begin(), p.parent().end()) != static_cast<int64_t>(params.size())) /* that's a paddlin */
          return EINA_FALSE;
        if (std::distance(p.begin(), p.end()) != 1) /* that's a paddlin */
          return EINA_FALSE;
        if (std::distance(v.begin(), v.end()) != 1) /* that's also a paddlin */
          return EINA_FALSE;
        
        for (xpath_node_set::const_iterator it = params.begin(); it != params.end(); ++it)
          {
             xpath_node n;
             Azy_Value *val;

             n = *it;

             if (std::distance(n.node().begin(), n.node().end()) != 1) return EINA_FALSE;
             val = azy_value_deserialize_xml(n.node().first_child());
             if (!val)
               {
                  azy_content_error_faultmsg_set(content, -1,
                                                 "Can't parse XML-RPC XML request. Failed to deserialize parameter %i.", 1 + std::distance(params.begin(), it));
                  return EINA_FALSE;
               }
             azy_content_param_add(content, val);
          }
     }

   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_response_xml(Azy_Content *content,
                                     const char  *buf,
                                     ssize_t      len)
{
   xml_document doc;
   xpath_node_set params;
   static xpath_query pquery("/methodResponse/params/param/value");
   static xpath_query fcquery("/methodResponse/fault/value/struct/member/value/int");
   static xpath_query fsquery("/methodResponse/fault/value/struct/member/value/string");

   if ((!content) || (!buf)) return EINA_FALSE; /* FIXME: ESNV? */

   if (!doc.load_buffer(const_cast<char *>(buf), len))
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_DOC);
        return EINA_FALSE;
     }

   if ((!doc.first_child()) || (doc.first_child() != doc.last_child())) /* more than methodResponse at root */
     return EINA_FALSE;

   if (std::distance(doc.first_child().begin(), doc.first_child().end()) != 1) /* more than params/fault */
     return EINA_FALSE;
     
   params = pquery.evaluate_node_set(doc);
   if (params.empty())
     {
        xpath_node_set fc, fs;
        int c;
        const char *s = NULL;
        xml_node f;

        f = doc.first_child().first_child(); /* <fault> */
        if (f.empty() || (std::distance(f.begin(), f.end()) != 1)) /* again, a paddlin */
          return EINA_FALSE;
        f = f.first_child(); /* <value> */
        if (f.empty() || (std::distance(f.begin(), f.end()) != 1)) /* yet another paddlin */
          return EINA_FALSE;
        f = f.first_child(); /* <struct> */
        if (f.empty() || (std::distance(f.begin(), f.end()) != 2)) /* yet another paddlin */
          return EINA_FALSE;
          
        fc = fcquery.evaluate_node_set(doc);
        if (fc.empty())
          {
             azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_FAULT);
             return EINA_FALSE;
          }
        f = fc.first().node(); /* <int> */
        if (std::distance(f.begin(), f.end()) != 1) /* more paddlins */
          return EINA_FALSE;
        f = f.parent(); /* <value> */
        if (std::distance(f.begin(), f.end()) != 1) /* some paddlins here too */
          return EINA_FALSE;
          
        fs = fsquery.evaluate_node_set(doc);
        if (fs.empty())
          {
             azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_FAULT);
             return EINA_FALSE;
          }
        f = fs.first().node(); /* <string> */
        if (std::distance(f.begin(), f.end()) != 1) /* more paddlins */
          return EINA_FALSE;
        f = f.parent(); /* <value> */
        if (std::distance(f.begin(), f.end()) != 1) /* some paddlins here too */
          return EINA_FALSE;
        errno = 0;
        c = strtol(fc.first().node().child_value(), NULL, 10);
        if (errno) c = -1;
        if (!fs.empty()) s = fs.first().node().child_value();

        if (s)
          {
#if defined(HAVE_EINA_ERROR_FIND)
             Eina_Error e;

             e = eina_error_find(s);
             if (e) azy_content_error_faultcode_set(content, e, c);
             else
#endif
               azy_content_error_faultmsg_set(content, c, "%s", s);
          }
        else
          azy_content_error_faultmsg_set(content, c, "%s", "");
        return EINA_TRUE;
     }
   else if (params.size() > 1)
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_MULTI);
        return EINA_FALSE;
     }

   content->retval = azy_value_deserialize_xml(params.first().node().first_child());
   if (!content->retval)
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_RETVAL);
        return EINA_FALSE;
     }
   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_rss_xml(Azy_Content *content,
                                const char  *buf,
                                ssize_t      len)
{
   xml_document doc;
   xpath_node_set channel, img;
   Azy_Rss *rss;
   static xpath_query cquery("/rss/channel");
   static xpath_query iquery("/rss/channel/image/url");
   
   if (!doc.load_buffer(const_cast<char *>(buf), len))
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_DOC);
        return EINA_FALSE;
     }

   channel = cquery.evaluate_node_set(doc);

   if (channel.empty())
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_FAULT);
        return EINA_FALSE;
     }

   rss = azy_rss_new();
   if (!rss)
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_XML_FAULT);
        return EINA_FALSE;
     }
   img = iquery.evaluate_node_set(doc);
   if (!img.empty())
     rss->img_url = eina_stringshare_add(img.first().node().child_value());

   for (xml_node::iterator it = channel.first().node().begin(); it != channel.first().node().end(); ++it)
     {
        xml_node n;
        const char *name;

        n = *it;
        name = n.name();

        if ((!rss->title) && (!strcmp(name, "title")))
          rss->title = eina_stringshare_add(n.child_value());
        else if ((!rss->link) && (!strcmp(name, "link")))
          rss->link = eina_stringshare_add(n.child_value());
        else if ((!rss->desc) && (!strcmp(name, "description")))
          rss->desc = eina_stringshare_add(n.child_value());
        else if (!strcmp(name, "item") && (!n.empty()))
          {
             Azy_Rss_Item *i;

             i = azy_rss_item_new();
             if (!i) goto error;
             
             for (xml_node::iterator x = n.begin(); x != n.end(); ++x)
               {
                  xml_node nn;

                  nn = *x;
                  name = nn.name();

                  if ((!i->title) && (!strcmp(name, "title")))
                    i->title = eina_stringshare_add(nn.child_value());
                  else if ((!i->link) && (!strcmp(name, "link")))
                    i->link = eina_stringshare_add(nn.child_value());
                  else if ((!i->desc) && (!strcmp(name, "description")))
                    i->desc = eina_stringshare_add(nn.child_value());
                  else if ((!i->author) && (!strcmp(name, "author")))
                    i->author = eina_stringshare_add(nn.child_value());
                  else if ((!i->date) && (!strcmp(name, "pubDate")))
                    i->date = eina_stringshare_add(nn.child_value());
                  else if ((!i->guid) && (!strcmp(name, "guid")))
                    i->guid = eina_stringshare_add(nn.child_value());
                  else if ((!i->comment_url) && (!strcmp(name, "comments")))
                    i->comment_url = eina_stringshare_add(nn.child_value());

               }
             rss->items = eina_list_append(rss->items, i);
          }
     }
   content->ret = rss;
   return EINA_TRUE;

error:
   azy_rss_free(rss);
   return EINA_FALSE;
}
