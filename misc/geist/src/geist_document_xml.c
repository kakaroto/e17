#include "geist_document_xml.h"

static geist_object *geist_parse_image_xml(xmlDocPtr doc, xmlNsPtr ns,
                                           xmlNodePtr cur);
static geist_object *geist_parse_text_xml(xmlDocPtr doc, xmlNsPtr ns,
                                          xmlNodePtr cur);
static geist_object *geist_parse_rect_xml(xmlDocPtr doc, xmlNsPtr ns,
                                          xmlNodePtr cur);
static geist_fill *geist_fill_parse_xml(xmlDocPtr doc, xmlNsPtr ns,

                                        xmlNodePtr cur);
static geist_object *geist_object_parse_xml(xmlDocPtr doc, xmlNsPtr ns,
                                            xmlNodePtr cur,

                                            geist_layer * parent);
static geist_list *geist_object_list_parse_xml(xmlDocPtr doc, xmlNsPtr ns,
                                               xmlNodePtr cur,

                                               geist_layer * parent);
static geist_layer *geist_layer_parse_xml(xmlDocPtr doc, xmlNsPtr ns,
                                          xmlNodePtr cur,

                                          geist_document * parent);
static geist_list *geist_layer_list_parse_xml(xmlDocPtr doc, xmlNsPtr ns,
                                              xmlNodePtr cur,

                                              geist_document * parent);
static geist_document *geist_document_parse_xml(xmlDocPtr doc, xmlNsPtr ns,
                                                xmlNodePtr cur,

                                                char *filename);



static void geist_save_layer_xml(geist_layer * layer, xmlNodePtr parent,
                                 xmlNsPtr ns);
static void geist_save_object_xml(geist_object * obj, xmlNodePtr parent,
                                  xmlNsPtr ns);
static void geist_save_image_xml(geist_image * img, xmlNodePtr parent,
                                 xmlNsPtr ns);
static void geist_save_text_xml(geist_text * txt, xmlNodePtr parent,

                                xmlNsPtr ns);
static void geist_save_rect_xml(geist_rect * rect, xmlNodePtr parent,

                                xmlNsPtr ns);
static void geist_save_fill_xml(geist_fill * fill, xmlNodePtr parent,

                                xmlNsPtr ns);

/* Utility functions */
int geist_xml_read_int(xmlNodePtr cur, char *key, int def);
void geist_xml_write_int(xmlNodePtr cur, char *key, int val);

/***********/
/* LOADING */
/***********/

geist_document *
geist_document_load_xml(char *xml, gint * err_return)
{
   geist_document *d;
   xmlDocPtr doc;
   xmlNsPtr ns;
   xmlNodePtr cur;

   D_ENTER(3);

   doc = xmlParseFile(xml);
   if (doc == NULL)
   {
      *err_return = ERR_NO_FILE;
      D_RETURN(3, NULL);
   }

   /* Check the document is of the right kind */
   cur = doc->root;
   if (cur == NULL)
   {
      *err_return = ERR_EMPTY_FILE;
      weprintf("%s is an empty document\n", xml);
      xmlFreeDoc(doc);
      D_RETURN(3, NULL);
   }
   ns = xmlSearchNsByHref(doc, doc->root, "http://www.linuxbrit.co.uk/geist");
   if (ns == NULL)
   {
      *err_return = ERR_WRONG_TYPE;
      weprintf
         ("document %s is of the wrong type, Geist namespace not found\n",
          xml);
      xmlFreeDoc(doc);
      D_RETURN(3, NULL);
   }
   if (strcasecmp(cur->name, "Geist"))
   {
      *err_return = ERR_WRONG_TYPE;
      fprintf(stderr, "document of the wrong type, root node != Geist\n");
      xmlFreeDoc(doc);
      D_RETURN(3, NULL);
   }

   cur = cur->childs;

   /* Now, walk the tree. */
   /* First level we expect just a Document */
   if ((strcmp(cur->name, "Document")) || (cur->ns != ns))
   {
      *err_return = ERR_WRONG_TYPE;
      weprintf("document of the wrong type, Document expected\n");
      weprintf("got %s instead\n", cur->name);
      weprintf("node ns: %p, global ns: %p\n", cur->ns, ns);
      xmlFreeDoc(doc);
      D_RETURN(3, NULL);
   }

   /* So let's parse the project then */
   d = geist_document_parse_xml(doc, ns, cur, xml);

   xmlFreeDoc(doc);

   D_RETURN(3, d);
}

static geist_document *
geist_document_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur,
                         char *filename)
{
   geist_document *ret;
   int w, h;
   char *name = NULL;
   geist_fill *fill = NULL;
   geist_list *layers = NULL;

   D_ENTER(3);

   w = geist_xml_read_int(cur, "Width", 500);
   h = geist_xml_read_int(cur, "Height", 500);

   ret = geist_document_new(w, h);
   ret->filename = estrdup(filename);

   cur = cur->childs;
   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Name")) && (cur->ns == ns))
         name = xmlNodeGetContent(cur->childs);
      else if ((!strcmp(cur->name, "Fill")) && (cur->ns == ns))
         fill = geist_fill_parse_xml(doc, ns, cur);
      else if ((!strcmp(cur->name, "Layers")) && (cur->ns == ns))
      {
         layers = geist_layer_list_parse_xml(doc, ns, cur, ret);
      }
      cur = cur->next;
   }

   geist_document_rename(ret, name);
   if (ret->bg_fill)
      geist_fill_free(ret->bg_fill);
   ret->bg_fill = fill;
   geist_list_free(ret->layers);
   ret->layers = layers;

   D_RETURN(3, ret);
}

static geist_fill *
geist_fill_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
   geist_fill *ret;
   int a, r, g, b;

   D_ENTER(3);

   a = geist_xml_read_int(cur, "A", 255);
   r = geist_xml_read_int(cur, "R", 255);
   g = geist_xml_read_int(cur, "G", 255);
   b = geist_xml_read_int(cur, "B", 255);

   ret = geist_fill_new_coloured(r, g, b, a);

   D_RETURN(3, ret);
}

static geist_list *
geist_layer_list_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur,
                           geist_document * parent)
{
   geist_list *ret = NULL;
   geist_layer *layer = NULL;

   D_ENTER(3);
   cur = cur->childs;

   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Layer")) && (cur->ns == ns))
         layer = geist_layer_parse_xml(doc, ns, cur, parent);
      if (layer)
         ret = geist_list_add_end(ret, layer);
      else
         weprintf("invalid layer found\n");

      cur = cur->next;
   }
   D_RETURN(3, ret);
}


static geist_layer *
geist_layer_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur,
                      geist_document * parent)
{
   geist_layer *ret;

   D_ENTER(3);

   ret = geist_layer_new();
   ret->doc = parent;

   ret->alpha = geist_xml_read_int(cur, "Alpha", 255);
   ret->x_offset = geist_xml_read_int(cur, "X_Offset", 0);
   ret->y_offset = geist_xml_read_int(cur, "Y_Offset", 0);
   ret->visible = geist_xml_read_int(cur, "Visible", 0);

   cur = cur->childs;

   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Name")) && (cur->ns == ns))
         ret->name = estrdup(xmlNodeGetContent(cur->childs));
      else if ((!strcmp(cur->name, "Objects")) && (cur->ns == ns))
         ret->objects = geist_object_list_parse_xml(doc, ns, cur, ret);

      cur = cur->next;
   }

   D_RETURN(3, ret);
}

static geist_list *
geist_object_list_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur,
                            geist_layer * parent)
{
   geist_list *ret = NULL;
   geist_object *obj;

   D_ENTER(3);

   cur = cur->childs;

   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Object")) && (cur->ns == ns))
      {
         obj = geist_object_parse_xml(doc, ns, cur, parent);
         if (obj)
            ret = geist_list_add_end(ret, obj);
         else
            weprintf("invalid object\n");
      }
      cur = cur->next;
   }

   D_RETURN(3, ret);
}

static geist_object *
geist_object_parse_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur,
                       geist_layer * parent)
{
   geist_object *ret = NULL;
   xmlNodePtr c;
   char *type = NULL;
   int type_int;

   D_ENTER(3);

   c = cur->childs;

   while (c != NULL)
   {
      if ((!strcmp(c->name, "Type")) && (c->ns == ns))
      {
         type = xmlNodeGetContent(c->childs);
         break;
      }
      c = c->next;
   }

   if (!type)
   {
      weprintf("invalid object found\n");
      D_RETURN(3, NULL);
   }

   /* specific object stuffs */
   /* TODO this breaks the object model right now */

   type_int = geist_object_get_type_from_string(type);
   switch (type_int)
   {
     case GEIST_TYPE_RECT:
        ret = geist_parse_rect_xml(doc, ns, cur);
        break;
     case GEIST_TYPE_TEXT:
        ret = geist_parse_text_xml(doc, ns, cur);
        break;
     case GEIST_TYPE_IMAGE:
        ret = geist_parse_image_xml(doc, ns, cur);
        break;
     default:
        weprintf("invalid object type found\n");
        break;
   }

   ret->layer = parent;
   ret->alpha = geist_xml_read_int(cur, "Alpha", 255);
   ret->x = geist_xml_read_int(cur, "X", 0);
   ret->y = geist_xml_read_int(cur, "Y", 0);
   ret->w = geist_xml_read_int(cur, "W", 10);
   ret->h = geist_xml_read_int(cur, "H", 10);
   ret->alias = geist_xml_read_int(cur, "Alias", 0);

   if (geist_xml_read_int(cur, "Visible", 1))
      geist_object_show(ret);
   else
      geist_object_hide(ret);

   cur = cur->childs;
   while (cur != NULL)
   {
      if ((!strcmp(c->name, "Name")) && (c->ns == ns))
      {
         if (ret->name)
            efree(ret->name);
         ret->name = estrdup(xmlNodeGetContent(cur->childs));
      }
      else if ((!strcmp(c->name, "Sizemode")) && (c->ns == ns))
         ret->sizemode =
            geist_object_get_sizemode_from_string(xmlNodeGetContent
                                                  (cur->childs));
      else if ((!strcmp(c->name, "Alignment")) && (c->ns == ns))
         ret->alignment =
            geist_object_get_alignment_from_string(xmlNodeGetContent
                                                   (cur->childs));
      cur = cur->next;
   }
   geist_object_update_alignment(ret);
   geist_object_update_sizemode(ret);

   D_RETURN(3, ret);
}

static geist_object *
geist_parse_rect_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
   geist_object *ret;
   int r, g, b, a;

   D_ENTER(3);

   a = geist_xml_read_int(cur, "A", 255);
   r = geist_xml_read_int(cur, "R", 255);
   g = geist_xml_read_int(cur, "G", 255);
   b = geist_xml_read_int(cur, "B", 255);

   ret = geist_rect_new_of_size(0, 0, 10, 10, a, r, g, b);

   D_RETURN(3, ret);
}

static geist_object *
geist_parse_text_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
   geist_object *ret;
   int r, g, b, a;
   char *fontname = NULL;
   int fontsize = 0;
   char *text = NULL;
   int wordwrap = 0;

   D_ENTER(3);

   a = geist_xml_read_int(cur, "A", 255);
   r = geist_xml_read_int(cur, "R", 255);
   g = geist_xml_read_int(cur, "G", 255);
   b = geist_xml_read_int(cur, "B", 255);
   fontsize = geist_xml_read_int(cur, "Fontsize", 12);
   wordwrap = geist_xml_read_int(cur, "Wordwrap", 0);

   cur = cur->childs;
   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Fontname")) && (cur->ns == ns))
         fontname = xmlNodeGetContent(cur->childs);
      else if ((!strcmp(cur->name, "Text")) && (cur->ns == ns))
         text = xmlNodeGetContent(cur->childs);
      cur = cur->next;
   }

   ret = geist_text_new_with_text(0, 0, fontname, fontsize, text, a, r, g, b);

   D_RETURN(3, ret);
}

static geist_object *
geist_parse_image_xml(xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
   geist_object *ret;
   char *filename = NULL;

   D_ENTER(3);

   cur = cur->childs;
   while (cur != NULL)
   {
      if ((!strcmp(cur->name, "Filename")) && (cur->ns == ns))
         filename = xmlNodeGetContent(cur->childs);
      cur = cur->next;
   }

   ret = geist_image_new_from_file(0, 0, filename);

   D_RETURN(3, ret);
}


/**********/
/* SAVING */
/**********/

int
geist_document_save_xml(geist_document * document, char *filename)
{
   xmlDocPtr doc;
   xmlNodePtr tree, subtree;
   xmlNsPtr ns;
   geist_list *kids;

   D_ENTER(3);

   doc = xmlNewDoc("1.0");
   doc->root = xmlNewDocNode(doc, NULL, "Geist", NULL);
   ns = xmlNewNs(doc->root, "http://www.linuxbrit.co.uk/geist", "geist");

   tree = xmlNewChild(doc->root, ns, "Document", NULL);

   /* Document properties */
   xmlNewTextChild(tree, ns, "Name", document->name);

   geist_xml_write_int(tree, "Width", document->w);
   geist_xml_write_int(tree, "Height", document->h);

   /* Bg Fill */
   geist_save_fill_xml(document->bg_fill, tree, ns);

   /* Document layers */
   subtree = xmlNewChild(tree, ns, "Layers", NULL);
   kids = document->layers;
   while (kids)
   {
      geist_save_layer_xml(kids->data, subtree, ns);
      kids = kids->next;
   }

   xmlSaveFile(filename, doc);

   xmlFreeDoc(doc);

   D_RETURN(3, 0);
}

static void
geist_save_layer_xml(geist_layer * layer, xmlNodePtr parent, xmlNsPtr ns)
{
   /* recursive */
   xmlNodePtr newlayer, subtree = NULL;
   geist_list *kids;

   D_ENTER(3);

   newlayer = xmlNewChild(parent, ns, "Layer", NULL);

   xmlNewTextChild(newlayer, ns, "Name", layer->name);
   geist_xml_write_int(newlayer, "Alpha", layer->alpha);
   geist_xml_write_int(newlayer, "X_Offset", layer->x_offset);
   geist_xml_write_int(newlayer, "Y_Offset", layer->y_offset);
   geist_xml_write_int(newlayer, "Visible", layer->visible);

   /* Other properties go here */

   /* OBJECTS */
   if (layer->objects)
      subtree = xmlNewChild(newlayer, ns, "Objects", NULL);

   kids = layer->objects;
   while (kids)
   {
      geist_save_object_xml(kids->data, subtree, ns);
      kids = kids->next;
   }
   D_RETURN_(3);
}

static void
geist_save_object_xml(geist_object * obj, xmlNodePtr parent, xmlNsPtr ns)
{
   xmlNodePtr newobject;

   D_ENTER(3);

   newobject = xmlNewChild(parent, ns, "Object", NULL);

   xmlNewTextChild(newobject, ns, "Type", geist_object_get_type_string(obj));
   xmlNewTextChild(newobject, ns, "Name", obj->name);
   geist_xml_write_int(newobject, "Alpha", obj->alpha);
   geist_xml_write_int(newobject, "X", obj->x);
   geist_xml_write_int(newobject, "Y", obj->y);
   geist_xml_write_int(newobject, "W", obj->w);
   geist_xml_write_int(newobject, "H", obj->h);
   geist_xml_write_int(newobject, "Alias", obj->alias);
   geist_xml_write_int(newobject, "Visible",
                       geist_object_get_state(obj, VISIBLE));
   xmlNewTextChild(newobject, ns, "Sizemode",
                   geist_object_get_sizemode_string(obj));
   xmlNewTextChild(newobject, ns, "Alignment",
                   geist_object_get_alignment_string(obj));

   /* Other generic object properties go here */


   /* Type-specific properties */
   switch (obj->type)
   {
     case GEIST_TYPE_IMAGE:
        geist_save_image_xml(GEIST_IMAGE(obj), newobject, ns);
        break;
     case GEIST_TYPE_TEXT:
        geist_save_text_xml(GEIST_TEXT(obj), newobject, ns);
        break;
     case GEIST_TYPE_RECT:
        geist_save_rect_xml(GEIST_RECT(obj), newobject, ns);
        break;
     default:
        printf("IMPLEMENT ME!\n");
        break;
   }
   D_RETURN_(3);
}

static void
geist_save_image_xml(geist_image * img, xmlNodePtr parent, xmlNsPtr ns)
{
   D_ENTER(3);

   /* FILENAME */
   xmlNewTextChild(parent, ns, "Filename", img->filename);

   D_RETURN_(3);
}


static void
geist_save_text_xml(geist_text * txt, xmlNodePtr parent, xmlNsPtr ns)
{
   D_ENTER(3);

   xmlNewTextChild(parent, ns, "Fontname", txt->fontname);
   geist_xml_write_int(parent, "Fontsize", txt->fontsize);
   xmlNewTextChild(parent, ns, "Text", txt->text);
   geist_xml_write_int(parent, "R", txt->r);
   geist_xml_write_int(parent, "G", txt->g);
   geist_xml_write_int(parent, "B", txt->b);
   geist_xml_write_int(parent, "A", txt->a);
   geist_xml_write_int(parent, "Wordwrap", txt->wordwrap);

   D_RETURN_(3);
}

static void
geist_save_rect_xml(geist_rect * rect, xmlNodePtr parent, xmlNsPtr ns)
{
   D_ENTER(3);

   geist_xml_write_int(parent, "R", rect->r);
   geist_xml_write_int(parent, "G", rect->g);
   geist_xml_write_int(parent, "B", rect->b);
   geist_xml_write_int(parent, "A", rect->a);

   D_RETURN_(3);

}

static void
geist_save_fill_xml(geist_fill * fill, xmlNodePtr parent, xmlNsPtr ns)
{
   xmlNodePtr newfill;

   D_ENTER(3);

   newfill = xmlNewChild(parent, ns, "Fill", NULL);

   geist_xml_write_int(newfill, "R", fill->r);
   geist_xml_write_int(newfill, "G", fill->g);
   geist_xml_write_int(newfill, "B", fill->b);
   geist_xml_write_int(newfill, "A", fill->a);

   D_RETURN_(3);
}

int
geist_xml_read_int(xmlNodePtr cur, char *key, int def)
{
   int i;
   char *prop;

   D_ENTER(5);

   prop = xmlGetProp(cur, key);
   if (prop)
      i = atoi(prop);
   else
      i = def;

   D_RETURN(5, i);
}

void
geist_xml_write_int(xmlNodePtr cur, char *key, int val)
{
   char buf[20];

   D_ENTER(5);

   snprintf(buf, sizeof(buf), "%d", val);
   xmlNewProp(cur, key, buf);

   D_RETURN_(5);
}
