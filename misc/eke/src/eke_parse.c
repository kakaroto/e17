#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "eke_feed.h"
#include "eke_feed_item.h"

typedef enum Eke_Parse_Type Eke_Parse_Type;
enum Eke_Parse_Type {
    EKE_PARSE_TYPE_FILE,
    EKE_PARSE_TYPE_MEMORY
};

static void eke_parse_handler(Eke_Feed *feed, Eke_Parse_Type type);
static void eke_parse_xml(xmlDoc *doc, Eke_Feed *feed);
static void eke_parse_item_free(void *val, void *data);

static void eke_parse_rss_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_rss_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_rss_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node);

static void eke_parse_atom_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_atom_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_atom_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node);

static void eke_parse_rdf_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_rdf_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node);
static void eke_parse_rdf_channel_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node);
static void eke_parse_rdf_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node);

static void strtrim(char *str);

void
eke_parse_file(Eke_Feed *feed)
{
    eke_parse_handler(feed, EKE_PARSE_TYPE_FILE);
}

void
eke_parse_memory(Eke_Feed *feed)
{
    eke_parse_handler(feed, EKE_PARSE_TYPE_MEMORY);
}

static void
eke_parse_handler(Eke_Feed *feed, Eke_Parse_Type type)
{
    xmlDoc *doc = NULL;

    switch (type) {
        case EKE_PARSE_TYPE_MEMORY:
            doc = xmlParseMemory(feed->data.data, feed->data.size);
            break;

        case EKE_PARSE_TYPE_FILE:
            doc = xmlReadFile(feed->data.data, NULL, 0);
            break;
    } 

    IF_FREE(feed->data.data);
    feed->data.size = 0;

    if (!doc) {
        feed->data.type = EKE_FEED_DATA_RESPONSE_ERROR;
        feed->data.data = strdup("Unable to parse buffer");
        return;
    }

    eke_parse_xml(doc, feed);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}

static void
eke_parse_xml(xmlDoc *doc, Eke_Feed *feed)
{
    xmlNode *node;

    IF_FREE(feed->title);
    IF_FREE(feed->link);
    IF_FREE(feed->desc);
    feed->rss_version = 0.0;

    ecore_list_for_each(feed->items, eke_parse_item_free, NULL);
    ecore_list_clear(feed->items);

    node = xmlDocGetRootElement(doc);
    if (!strcasecmp(node->name, "rss")) 
        eke_parse_rss_feed(doc, feed, node);

    else if (!strcasecmp(node->name, "feed"))
        eke_parse_atom_feed(doc, feed, node);

    else if (!strcasecmp(node->name, "rdf"))
        eke_parse_rdf_feed(doc, feed, node);

    else
        printf("Got a feed of type %s\n", node->name);
}

static void
eke_parse_rss_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    char *c;

    c = xmlGetProp(node, "version");
    feed->rss_version = atof(c);
    FREE(c);

    eke_parse_rss_xml_node(doc, feed, node->xmlChildrenNode);
}

static void
eke_parse_rss_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    xmlNode *tmp;

    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            feed->title = xmlNodeListGetString(doc, tmp, 1);
            strtrim(feed->title);

        } else if (!strcasecmp(node->name, "link")) {
            tmp = node->xmlChildrenNode;
            feed->link = xmlNodeListGetString(doc, tmp, 1);
            strtrim(feed->link);

        } else if (!strcasecmp(node->name, "description")) {
            tmp = node->xmlChildrenNode;
            feed->desc = xmlNodeListGetString(doc, tmp, 1);
            strtrim(feed->desc);

        } else if (!strcasecmp(node->name, "language")) {
            /* ignore for now */

        } else if (!strcasecmp(node->name, "item")) {
            eke_parse_rss_item_parse(feed, doc, node->xmlChildrenNode);

        } else if (!strcasecmp(node->name, "channel")) {
            eke_parse_rss_xml_node(doc, feed, node->xmlChildrenNode);

        }
        node = node->next;
    }
    feed->data.type = EKE_FEED_DATA_RESPONSE_CHANGED;
}

static void
eke_parse_rss_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node)
{
    Eke_Feed_Item *item;
    xmlNode *tmp;
    char *ptr;

    item = eke_feed_item_new();
    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_title_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "link")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_link_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "description")) {
            ptr = xmlNodeGetContent(node);
            strtrim(ptr);

            eke_feed_item_description_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "pupdate")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_date_set(item, ptr);
            IF_FREE(ptr);
        }
        node = node->next;
    }
    ecore_list_append(feed->items, item);
}

static void
eke_parse_item_free(void *val, void *data)
{
    Eke_Feed_Item *item;

    item = val;
    eke_feed_item_free(item);

    return;
    data = NULL;
}

static void
strtrim(char *str)
{
    char *ptr;
    if (!str) return;

    ptr = str;
    while ((*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n')) 
        ptr++;

    str = memmove(str, ptr, strlen(ptr) + 1);
    ptr = str + strlen(str);
    while ((*ptr == '\0') || (*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n')) {
        *ptr = '\0';
        ptr --;
    }
}

static void
eke_parse_atom_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    char *c;

    c = xmlGetProp(node, "version");
    feed->rss_version = atof(c);
    FREE(c);

    eke_parse_atom_xml_node(doc, feed, node->xmlChildrenNode);
}

static void
eke_parse_atom_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    xmlNode *tmp;

    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            feed->title = xmlNodeListGetString(doc, tmp, 1);
            strtrim(feed->title);

        } else if (!strcasecmp(node->name, "link")) {
            feed->link = xmlGetProp(node, "href");

        } else if (!strcasecmp(node->name, "entry")) {
            eke_parse_atom_item_parse(feed, doc, node->xmlChildrenNode);

        } else if (!strcasecmp(node->name, "tagline")) {
            tmp = node->xmlChildrenNode;
            feed->desc = xmlNodeListGetString(doc, tmp, 1);
            strtrim(feed->desc);
        }

        node = node->next;
    }
    feed->data.type = EKE_FEED_DATA_RESPONSE_CHANGED;
}

static void
eke_parse_atom_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node)
{
    Eke_Feed_Item *item;
    xmlNode *tmp;
    char *ptr;

    item = eke_feed_item_new();
    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_title_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "link")) {
            ptr = xmlGetProp(node, "href");
            strtrim(ptr);

            eke_feed_item_link_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "content")) {
            ptr = xmlNodeGetContent(node);
            strtrim(ptr);

            eke_feed_item_description_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "modified")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_date_set(item, ptr);
            IF_FREE(ptr);
        }
        node = node->next;
    }
    ecore_list_append(feed->items, item);
}

static void
eke_parse_rdf_feed(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    feed->rss_version = 0.0;
    eke_parse_rdf_xml_node(doc, feed, node->xmlChildrenNode);
}

static void
eke_parse_rdf_xml_node(xmlDoc *doc, Eke_Feed *feed, xmlNode *node)
{
    while (node) {
        if (!strcasecmp(node->name, "channel")) {
            eke_parse_rdf_channel_parse(feed, doc, node->xmlChildrenNode);

        } else if (!strcasecmp(node->name, "item")) {
            eke_parse_rdf_item_parse(feed, doc, node->xmlChildrenNode);
        }

        node = node->next;
    }
    feed->data.type = EKE_FEED_DATA_RESPONSE_CHANGED;
}

static void
eke_parse_rdf_channel_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node)
{
    xmlNode *tmp;
    char *ptr;

    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);
            feed->title = ptr;

        } else if (!strcasecmp(node->name, "link")) {
            ptr = xmlGetProp(node, "href");
            strtrim(ptr);
            feed->link = ptr;

        } else if (!strcasecmp(node->name, "description")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);
            feed->desc = ptr;

        }
        node = node->next;
    }
}

static void
eke_parse_rdf_item_parse(Eke_Feed *feed, xmlDoc *doc, xmlNode *node)
{
    Eke_Feed_Item *item;
    xmlNode *tmp;
    char *ptr;

    item = eke_feed_item_new();
    while (node) {
        if (!strcasecmp(node->name, "title")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_title_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "link")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_link_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "description")) {
            ptr = xmlNodeGetContent(node);
            strtrim(ptr);

            eke_feed_item_description_set(item, ptr);
            IF_FREE(ptr);

        } else if (!strcasecmp(node->name, "date")) {
            tmp = node->xmlChildrenNode;
            ptr = xmlNodeListGetString(doc, tmp, 1);
            strtrim(ptr);

            eke_feed_item_date_set(item, ptr);
            IF_FREE(ptr);
        }
        node = node->next;
    }
    ecore_list_append(feed->items, item);
}




