
#include "define.h"
#include "enum.h"
#include "function.h"
#include "hash.h"
#include "parser.h"
#include "util.h"


static xmlDocPtr   file = NULL;
static xmlNodePtr  main_node;

int
ecrin_parse_init (char *filename)
{
  file = xmlParseFile (filename);
  if (!file)
    {
      printf ("Can't parse the file. Exiting...\n");
      return 0;
    }
  
  main_node = xmlDocGetRootElement (file);
  if (!main_node)
    {
      printf ("Empty document. Exiting...\n");
      xmlFreeDoc (file);
      return 0;
    }
  return 1;
}

void
ecrin_parse_shutdown (void)
{
  xmlFreeDoc (file);
}

void
ecrin_parse_define (xmlNodePtr memberdef)
{
  xmlNodePtr  node;

  node = memberdef;
  for (; node ; node = node->next)
    {
      if ((xmlStrcasecmp (node->name, BAD_CAST ("memberdef")) == 0))
	{
	  Ecrin_Define      *define;
	  Ecrin_Description *d = NULL;
          Ecrin_Hash_Data   *hash_data;
	  unsigned char     *name = NULL;
	  char     *value = NULL;
	  char              *static_state;
	  char              *brief = NULL;
	  char              *detailed = NULL;
	  char               state;
	  xmlNodePtr         child;

	  static_state = (char *)xmlGetProp (node, BAD_CAST ("static"));
	  if (strcmp (static_state, "no") == 0)
	    state = '0';
	  else
	    state = '1';

	  child = node->xmlChildrenNode;
	  for (; child ; child = child->next)
	    {
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("name")) == 0))
		{
		  name = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("initializer")) == 0))
		{
		  value = remove_blancks ((char *)xmlNodeListGetString (file, child->xmlChildrenNode, 1));
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("briefdescription")) == 0))
		{
		  brief = remove_blancks ((char *)xmlNodeListGetString (file, child->xmlChildrenNode, 1));
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("detaileddescription")) == 0))
		{
		  detailed = remove_blancks ((char *)xmlNodeListGetString (file, child->xmlChildrenNode, 1));
		}
	    }
	  d = ecrin_description_new (brief, detailed);
	  define = ecrin_define_new ((char *)name, value, state, d);

	  /* on teste */
	  ecrin_define_display (define);
          hash_data = ecrin_hash_data_new ("eet",
                                           define->name,
                                           HASH_DATA_DEFINE,
                                           define);
          ecrin_hash_data_add (hash_data);
	}
    }
  
}

void
ecrin_parse_enum (xmlNodePtr memberdef)
{
  xmlNodePtr  node;

  node = memberdef;
  for (; node ; node = node->next)
    {
      if ((xmlStrcasecmp (node->name, BAD_CAST ("memberdef")) == 0))
	{
	  Ecrin_Enum        *e;
	  Ecrin_Enum_Item   *item = NULL;
	  Ecrin_Description *d = NULL;
          Ecrin_Hash_Data   *hash_data;
	  unsigned char     *name = NULL;
	  char              *static_state;
	  xmlNodePtr         child;

	  e = ecrin_enum_new ();

	  static_state = (char *)xmlGetProp (node, BAD_CAST ("static"));
	  if (strcmp (static_state, "no") == 0)
	    ecrin_enum_state_set (e, '0');
	  else
	    ecrin_enum_state_set (e, '1');

	  child = node->xmlChildrenNode;
	  for (; child ; child = child->next)
	    {
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("name")) == 0))
		{
		  name = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("enumvalue")) == 0))
		{
		  xmlNodePtr node_item;
		  char      *item_name = NULL;
		  char      *item_value = NULL;
		  char      *item_brief = NULL;
		  char      *item_detailed = NULL;

		  node_item = child->xmlChildrenNode;
		  for (; node_item ; node_item = node_item->next)
		    {
		      if ((xmlStrcasecmp (node_item->name, BAD_CAST ("name")) == 0))
			{
			  item_name = remove_blancks ((char *)xmlNodeListGetString (file, node_item->xmlChildrenNode, 1));
			}
		      if ((xmlStrcasecmp (node_item->name, BAD_CAST ("initializer")) == 0))
			{
			  item_value = remove_blancks ((char *)xmlNodeListGetString (file, node_item->xmlChildrenNode, 1));
			}
		      if ((xmlStrcasecmp (node_item->name, BAD_CAST ("briefdescription")) == 0))
			{
			  item_brief = remove_blancks ((char *)xmlNodeListGetString (file, node_item->xmlChildrenNode, 1));

			}
		      if ((xmlStrcasecmp (node_item->name, BAD_CAST ("detaileddescription")) == 0))
			{
			  item_detailed = remove_blancks ((char *)xmlNodeListGetString (file, node_item->xmlChildrenNode, 1));
			}
		    }
		  d = ecrin_description_new (item_brief,
					     item_detailed);
		  item = ecrin_enum_item_new (item_name,
					      item_value,
					      d);
		  if (item_name)
		    free (item_name);
		  if (item_value)
		    free (item_value);
		  ecrin_enum_name_set (e, (char *)name);
		  ecrin_enum_item_add (e, item);
		}
	    }

	  /* on teste */
	  ecrin_enum_display (e);
          hash_data = ecrin_hash_data_new ("eet",
                                           e->name,
                                           HASH_DATA_ENUM,
                                           e);
          ecrin_hash_data_add (hash_data);
	}
    }
  
}

void
ecrin_parse_function (xmlNodePtr memberdef)
{
  xmlNodePtr  node;

  node = memberdef;
  for (; node ; node = node->next)
    {
      if ((xmlStrcasecmp (node->name, BAD_CAST ("memberdef")) == 0))
	{
	  Ecrin_Function       *function;
	  Ecrin_Function_Param *param = NULL;
	  Ecrin_Description    *d = NULL;
          Ecrin_Hash_Data      *hash_data;
	  unsigned char        *name = NULL;
	  unsigned char        *return_type = NULL;
          unsigned char        *brief = NULL;
          char                 *param_detailed = NULL;
	  xmlNodePtr            child;

	  function = ecrin_function_new ();

	  child = node->xmlChildrenNode;
	  for (; child ; child = child->next)
	    {
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("name")) == 0))
		{
		  name = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("type")) == 0))
		{
		  return_type = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("param")) == 0))
		{
		  xmlNodePtr node_param;
		  char      *param_name = NULL;
		  char      *param_type = NULL;

		  node_param = child->xmlChildrenNode;
		  for (; node_param ; node_param = node_param->next)
		    {
		      if ((xmlStrcasecmp (node_param->name, BAD_CAST ("declname")) == 0))
			{
                          if (xmlNodeListGetString (file, node_param->xmlChildrenNode, 1))
                            param_name = remove_blancks ((char *)xmlNodeListGetString (file, node_param->xmlChildrenNode, 1));
			}
		      if ((xmlStrcasecmp (node_param->name, BAD_CAST ("type")) == 0))
			{
                          if (xmlNodeListGetString (file, node_param->xmlChildrenNode, 1))
                            param_type = remove_blancks ((char *)xmlNodeListGetString (file, node_param->xmlChildrenNode, 1));
			}
		    }
		  param = ecrin_function_param_new (param_type,
                                                    param_name);
		  if (param_name)
		    free (param_name);
		  if (param_type)
		    free (param_type);
		  ecrin_function_param_add (function, param);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("briefdescription")) == 0))
		{
		  brief = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
	      if ((xmlStrcasecmp (child->name, BAD_CAST ("detaileddescription")) == 0))
		{
		  xmlNodePtr node_param;
		  brief = xmlNodeListGetString (file, child->xmlChildrenNode, 1);
		}
              ecrin_function_name_set (function, (char *)name);
              ecrin_function_return_set (function, (char *)return_type);
	    }

	  /* on teste */
	  ecrin_function_display (function);
          hash_data = ecrin_hash_data_new ("eet",
                                           function->name,
                                           HASH_DATA_FUNCTION,
                                           function);
          ecrin_hash_data_add (hash_data);
	}
    }
  
}


void
ecrin_parse_xml (void)
{
  xmlNodePtr compounddef;
  xmlNodePtr node;

  compounddef = main_node->xmlChildrenNode;
  for (; compounddef ; compounddef = compounddef->next)
    {
      if ((xmlStrcasecmp (compounddef->name, BAD_CAST ("compounddef")) == 0))
        {
          node = compounddef->xmlChildrenNode;
          for (; node ; node = node->next)
            {
              if ((xmlStrcasecmp (node->name, BAD_CAST ("sectiondef")) == 0))
                {
                  if (xmlStrcasecmp (xmlGetProp (node, BAD_CAST ("kind")), BAD_CAST ("define")) == 0)
                    ecrin_parse_define (node->xmlChildrenNode);
                  
                  if (xmlStrcasecmp (xmlGetProp (node, BAD_CAST ("kind")), BAD_CAST ("enum")) == 0)
                    ecrin_parse_enum (node->xmlChildrenNode);
                  
                  if (xmlStrcasecmp (xmlGetProp (node, BAD_CAST ("kind")), BAD_CAST ("func")) == 0)
                    ecrin_parse_function (node->xmlChildrenNode);
                }
            }
        }
    }
}
