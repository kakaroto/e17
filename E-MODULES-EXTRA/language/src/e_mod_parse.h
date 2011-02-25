#ifndef E_MOD_CONFIG_H
#define E_MOD_CONFIG_H

#include "e_mod_main.h"

typedef struct _language_xml_node Language_XML_Node;
typedef struct _language_xml Language_XML;

struct _language_xml_node {
    char *tag;
    char *value;
    Eina_Hash *attributes;
    Eina_List *children;
    Language_XML_Node *parent;
};

struct _language_xml {
    Language_XML_Node *top;
    Language_XML_Node *current;
};

Language_XML *languages_load();

Eina_Bool language_next(Language_XML *xml);
Eina_Bool language_first(Language_XML *xml);

void language_xml_clear(Language_XML *xml);

#endif
