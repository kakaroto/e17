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

   w = atoi(xmlGetProp(cur, "width"));
   h = atoi(xmlGetProp(cur, "height"));

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

   a = atoi(xmlGetProp(cur, "A"));
   r = atoi(xmlGetProp(cur, "R"));
   g = atoi(xmlGetProp(cur, "G"));
   b = atoi(xmlGetProp(cur, "B"));

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

   ret->alpha = atoi(xmlGetProp(cur, "Alpha"));
   ret->x_offset = atoi(xmlGetProp(cur, "X_Offset"));
   ret->y_offset = atoi(xmlGetProp(cur, "Y_Offset"));
   ret->visible = atoi(xmlGetProp(cur, "Visible"));

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
   ret->alpha = atoi(xmlGetProp(cur, "Alpha"));
   ret->x = atoi(xmlGetProp(cur, "X"));
   ret->y = atoi(xmlGetProp(cur, "Y"));
   ret->w = atoi(xmlGetProp(cur, "W"));
   ret->h = atoi(xmlGetProp(cur, "H"));
   ret->alias = atoi(xmlGetProp(cur, "Alias"));
   if (atoi(xmlGetProp(cur, "Visible")))
      geist_object_show(ret);
   else
      geist_object_hide(ret);
   ret->alias = atoi(xmlGetProp(cur, "Alias"));

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

   a = atoi(xmlGetProp(cur, "A"));
   r = atoi(xmlGetProp(cur, "R"));
   g = atoi(xmlGetProp(cur, "G"));
   b = atoi(xmlGetProp(cur, "B"));

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

   D_ENTER(3);

   a = atoi(xmlGetProp(cur, "A"));
   r = atoi(xmlGetProp(cur, "R"));
   g = atoi(xmlGetProp(cur, "G"));
   b = atoi(xmlGetProp(cur, "B"));
   fontsize = atoi(xmlGetProp(cur, "Fontsize"));

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
   char *buf;

   D_ENTER(3);

   doc = xmlNewDoc("1.0");
   doc->root = xmlNewDocNode(doc, NULL, "Geist", NULL);
   ns = xmlNewNs(doc->root, "http://www.linuxbrit.co.uk/geist", "geist");

   tree = xmlNewChild(doc->root, ns, "Document", NULL);

   /* Document properties */
   xmlNewTextChild(tree, ns, "Name", document->name);

   buf = g_strdup_printf("%d", document->w);
   xmlNewProp(tree, "width", buf);
   efree(buf);

   buf = g_strdup_printf("%d", document->h);
   xmlNewProp(tree, "height", buf);
   efree(buf);

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
   gchar *buf;

   D_ENTER(3);

   newlayer = xmlNewChild(parent, ns, "Layer", NULL);

   /* NAME */
   xmlNewTextChild(newlayer, ns, "Name", layer->name);

   /* ALPHA */
   buf = g_strdup_printf("%d", layer->alpha);
   xmlNewProp(newlayer, "Alpha", buf);
   efree(buf);

   /* X_OFFSET */
   buf = g_strdup_printf("%d", layer->x_offset);
   xmlNewProp(newlayer, "X_Offset", buf);
   efree(buf);

   /* Y_OFFSET */
   buf = g_strdup_printf("%d", layer->y_offset);
   xmlNewProp(newlayer, "Y_Offset", buf);
   efree(buf);

   /* VISIBLE */
   buf = g_strdup_printf("%d", layer->visible);
   xmlNewProp(newlayer, "Visible", buf);
   efree(buf);

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
   gchar *buf;

   D_ENTER(3);

   newobject = xmlNewChild(parent, ns, "Object", NULL);

   /* TYPE */
   xmlNewTextChild(newobject, ns, "Type", geist_object_get_type_string(obj));

   /* NAME */
   xmlNewTextChild(newobject, ns, "Name", obj->name);

   /* ALPHA */
   buf = g_strdup_printf("%d", obj->alpha);
   xmlNewProp(newobject, "Alpha", buf);
   efree(buf);

   /* X */
   buf = g_strdup_printf("%d", obj->x);
   xmlNewProp(newobject, "X", buf);
   efree(buf);

   /* Y */
   buf = g_strdup_printf("%d", obj->y);
   xmlNewProp(newobject, "Y", buf);
   efree(buf);

   /* W */
   buf = g_strdup_printf("%d", obj->w);
   xmlNewProp(newobject, "W", buf);
   efree(buf);

   /* H */
   buf = g_strdup_printf("%d", obj->h);
   xmlNewProp(newobject, "H", buf);
   efree(buf);

   /* ALIAS */
   buf = g_strdup_printf("%d", obj->alias);
   xmlNewProp(newobject, "Alias", buf);
   efree(buf);

   /* VISIBLE */
   buf = g_strdup_printf("%ld", geist_object_get_state(obj, VISIBLE));
   xmlNewProp(newobject, "Visible", buf);
   efree(buf);

   /* SIZEMODE */
   xmlNewTextChild(newobject, ns, "Sizemode",
                   geist_object_get_sizemode_string(obj));

   /* ALIGNMENT */
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
        printf("IMLEMENT ME!\n");
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
   char *buf;

   D_ENTER(3);
   /* FONTNAME */
   xmlNewTextChild(parent, ns, "Fontname", txt->fontname);

   /* FONTSIZE */
   buf = g_strdup_printf("%d", txt->fontsize);
   xmlNewProp(parent, "Fontsize", buf);
   efree(buf);

   /* TEXT */
   xmlNewTextChild(parent, ns, "Text", txt->text);

   /* R */
   buf = g_strdup_printf("%d", txt->r);
   xmlNewProp(parent, "R", buf);
   efree(buf);

   /* G */
   buf = g_strdup_printf("%d", txt->g);
   xmlNewProp(parent, "G", buf);
   efree(buf);

   /* B */
   buf = g_strdup_printf("%d", txt->b);
   xmlNewProp(parent, "B", buf);
   efree(buf);

   /* A */
   buf = g_strdup_printf("%d", txt->a);
   xmlNewProp(parent, "A", buf);
   efree(buf);
   D_RETURN_(3);
}

static void
geist_save_rect_xml(geist_rect * rect, xmlNodePtr parent, xmlNsPtr ns)
{
   gchar *buf;

   D_ENTER(3);

   /* R */
   buf = g_strdup_printf("%d", rect->r);
   xmlNewProp(parent, "R", buf);
   efree(buf);

   /* G */
   buf = g_strdup_printf("%d", rect->g);
   xmlNewProp(parent, "G", buf);
   efree(buf);

   /* B */
   buf = g_strdup_printf("%d", rect->b);
   xmlNewProp(parent, "B", buf);
   efree(buf);

   /* A */
   buf = g_strdup_printf("%d", rect->a);
   xmlNewProp(parent, "A", buf);
   efree(buf);
   D_RETURN_(3);

}

static void
geist_save_fill_xml(geist_fill * fill, xmlNodePtr parent, xmlNsPtr ns)
{
   xmlNodePtr newfill;
   gchar *buf;

   D_ENTER(3);

   newfill = xmlNewChild(parent, ns, "Fill", NULL);

   /* R */
   buf = g_strdup_printf("%d", fill->r);
   xmlNewProp(newfill, "R", buf);
   efree(buf);

   /* G */
   buf = g_strdup_printf("%d", fill->g);
   xmlNewProp(newfill, "G", buf);
   efree(buf);

   /* B */
   buf = g_strdup_printf("%d", fill->b);
   xmlNewProp(newfill, "B", buf);
   efree(buf);

   /* A */
   buf = g_strdup_printf("%d", fill->a);
   xmlNewProp(newfill, "A", buf);
   efree(buf);
   D_RETURN_(3);
}
