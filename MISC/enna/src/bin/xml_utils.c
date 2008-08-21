#include "enna.h"
#include "xml_utils.h"

xmlNode *
get_node_xml_tree (xmlNode *root, const char *prop)
{
  xmlNode *n, *children_node;
  
  for (n = root; n; n = n->next)
  {
    if (n->type == XML_ELEMENT_NODE
        && xmlStrcmp ((unsigned char *) prop, n->name) == 0)
      return n;
    
    if ((children_node = get_node_xml_tree(n->children, prop)))
      return children_node;
  }
  
  return NULL;
}

xmlChar *
get_prop_value_from_xml_tree (xmlNode *root, const char *prop)
{
  xmlNode *node;
  
  node = get_node_xml_tree (root, prop);
  if (!node)
    return NULL;
  
  if (xmlNodeGetContent (node))
    return xmlNodeGetContent (node);
  
  return NULL;
}
