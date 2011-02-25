#include "e_mod_parse.h"

static Eina_Bool _lng_read(Language_XML *xml, xmlTextReaderPtr reader);
static Eina_Bool _lng_start(Language_XML *xml);
static Eina_Bool _lng_set(Language_XML *xml, char *value, char *attrvalue, const char *type);

static Eina_Bool _lng_read(Language_XML *xml, xmlTextReaderPtr reader)
{
  Eina_Bool empty;
  xmlChar *name, *value;

  if (!reader) return EINA_FALSE;
  language_xml_clear(xml);

  while (xmlTextReaderRead(reader) == 1)
    {
      name = xmlTextReaderName(reader);
      value = xmlTextReaderValue(reader);
      empty = xmlTextReaderIsEmptyElement(reader);

      switch (xmlTextReaderNodeType(reader))
        {
          case XML_READER_TYPE_ELEMENT:
            {
              _lng_start(xml);
              _lng_set(xml, (char *) name, NULL, "tag");

              if (xmlTextReaderHasAttributes(reader))
                {
                  xmlTextReaderMoveToFirstAttribute(reader);
                  do
                  {
                    xmlChar *attr_name, *attr_value;

                    attr_name = xmlTextReaderName(reader);
                    attr_value = xmlTextReaderValue(reader);

                    _lng_set(xml, (char *) attr_name, (char *) attr_value, "atr");

                    xmlFree(attr_name);
                    xmlFree(attr_value);
                  } while (xmlTextReaderMoveToNextAttribute(reader) == 1);
                }

              if (!empty) break;
            }
          case XML_READER_TYPE_END_ELEMENT:
            {
              if (!xml) return EINA_FALSE;
              if (!xml->current)
                xml->current = xml->top;
              else
                xml->current = xml->current->parent;
              break;
            }
          case XML_READER_TYPE_WHITESPACE:
            break;
          case XML_READER_TYPE_TEXT:
            {
              _lng_set(xml, (char *) value, NULL, "val");
              break;
            }
        }
      xmlFree(name);
      xmlFree(value);
    }

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);

  xml->current = xml->top;

  return EINA_TRUE;
}

static Eina_Bool _lng_start(Language_XML *xml)
{
  Language_XML_Node *node;

  if (!xml)
    {
      DBG("One of values is NULL, returning with error.");
      return EINA_FALSE;
    }

  if (!xml->current && xml->top) return EINA_FALSE;

  node = calloc(sizeof(Language_XML_Node), 1);
  if (!node)
    {
      E_FREE(node);
      return EINA_FALSE;
    }

  node->attributes = eina_hash_string_superfast_new(free);
  node->parent = xml->current;

  if (!xml->top)
    xml->current = xml->top = node;
  else
    {
      xml->current->children = eina_list_append( xml->current->children, node );
      xml->current = node;
    }

  return EINA_TRUE;
}

static Eina_Bool _lng_set(Language_XML *xml, char *value, char *attrvalue, const char *type)
{
  if ((!xml) || (!value))
    {
      DBG("One of values is NULL, returning with error.");
      return EINA_FALSE;
    }

  if (!strcmp(type, "atr"))
    {
      if (attrvalue) attrvalue = eina_stringshare_add(attrvalue);
      eina_hash_direct_add( xml->current->attributes, eina_stringshare_add(value), attrvalue );
    }
  else if (!strcmp(type, "tag"))
    {
      xml->current->tag = eina_stringshare_add(value);
      if (!xml->current->tag) return EINA_FALSE;
    }
  else if (!strcmp(type, "val"))
    {
      xml->current->value = eina_stringshare_add(value);
      if (!xml->current->value) return EINA_FALSE;
    }
  else return EINA_FALSE;

  return EINA_TRUE;
}

Language_XML *languages_load()
{
  Language_XML *xml;
  char filename[PATH_MAX];
  xmlTextReaderPtr reader;
  Eina_Bool result;

  xml = calloc(sizeof(Language_XML), 1);
  if (!xml)
    {
      DBG("One of values is NULL, returning with error.");
      return NULL;
    }

  snprintf(filename, sizeof(filename), "%s.xml", default_xkb_rules_file);
  reader = xmlReaderForFile( filename, NULL, XML_PARSE_RECOVER );

  result = _lng_read(xml, reader);
  if (!result) return NULL;
    
  return xml;
}

Eina_Bool language_next(Language_XML *xml)
{
    Eina_List *p_list;
    Language_XML_Node *parent, *cur;

    if (!xml)
      {
        DBG("One of values is NULL, returning with error.");
        return EINA_FALSE;
      }

    if (xml->current) {
        cur = xml->current;
        parent = cur->parent;

        if (parent) {
            p_list = parent->children;

            p_list = eina_list_data_find_list(p_list, xml->current);
            p_list = eina_list_next( p_list );
            if (!(xml->current = eina_list_data_get(p_list))) {
                xml->current = cur;
                return EINA_FALSE;
            }
        } else
            xml->current = EINA_FALSE;
    }

    return xml->current ? EINA_TRUE : EINA_FALSE;
}

Eina_Bool language_first(Language_XML *xml)
{
  Eina_Bool ret;
  if (!xml)
  {
    DBG("One of values is NULL, returning with error.");
    return EINA_FALSE;
  }

  if (xml->current && xml->current->children)
    if (eina_list_count(xml->current->children) > 0)
      ret = EINA_TRUE;
    else
      ret = EINA_FALSE;
  else
    ret = EINA_FALSE;

  if (ret)
    xml->current = eina_list_nth(xml->current->children, 0);
  else
    return EINA_TRUE;

  return xml->current ? EINA_TRUE : EINA_FALSE;
}

void language_xml_clear(Language_XML *xml)
{
  Language_XML_Node *n_cur;

  if ((!xml) || (!xml->current))
    {
      DBG("One of values is NULL, returning with error.");
      return;
    }

  xml->current = xml->top;

  n_cur = xml->current;
  if (!n_cur)
    {
      DBG("One of values is NULL, returning with error.");
      return;
    }

  n_cur = n_cur->parent;

  if (n_cur)
    {
      Language_XML_Node *c_parent = n_cur;
      Eina_List *c_list = c_parent->children;
      void *data;

      c_list = eina_list_data_find_list(c_list, xml->current);
      EINA_LIST_FREE(c_list, data) E_FREE(data);
      if (!(n_cur = eina_list_data_get(c_list)))
        if (!(n_cur = eina_list_last(c_list))) n_cur = c_parent;
    }
  else
    {
      xml->top = NULL;
      void *data;
      if (xml->current)
        {
          eina_hash_free(xml->current->attributes);
          EINA_LIST_FREE(xml->current->children, data) E_FREE(data);
          E_FREE(xml->current);
        }
    }

    xml->current = n_cur;
}
