#include "geist_document_xml.h"

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


/**********/
/* SAVING */
/**********/

int
geist_project_save_xml(geist_document * document, char *filename)
{
   xmlDocPtr doc;
   xmlNodePtr tree, subtree;
   xmlNsPtr ns;
   geist_list *kids;
   char *buf;

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

   return 0;
}

static void
geist_save_layer_xml(geist_layer * layer, xmlNodePtr parent, xmlNsPtr ns)
{
   /* recursive */
   xmlNodePtr newlayer, subtree = NULL;
   geist_list *kids;
   gchar *buf;

   newlayer = xmlNewChild(parent, ns, "Layer", NULL);

   /* NAME */
   xmlNewTextChild(newlayer, ns, "Name", layer->name);

   /* ALPHA */
   buf = g_strdup_printf("%d", layer->alpha);
   xmlNewProp(newlayer, "Alpha", buf);
   efree(buf);

   /* X_OFFSET */
   buf = g_strdup_printf("%d", layer->x_offset);
   xmlNewProp(newlayer, "X Offset", buf);
   efree(buf);

   /* Y_OFFSET */
   buf = g_strdup_printf("%d", layer->y_offset);
   xmlNewProp(newlayer, "Y Offset", buf);
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
}

static void
geist_save_object_xml(geist_object * obj, xmlNodePtr parent, xmlNsPtr ns)
{
   xmlNodePtr newobject;
   gchar *buf;

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

   /* VISIBLE */
   buf = g_strdup_printf("%ld", geist_object_get_state(obj,VISIBLE));
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
}

static void
geist_save_image_xml(geist_image * img, xmlNodePtr parent, xmlNsPtr ns)
{
   char *buf;

   /* FILENAME */
   xmlNewTextChild(parent, ns, "Filename", img->filename);

   /* ALIAS */
   buf = g_strdup_printf("%d", img->alias);
   xmlNewProp(parent, "Alias", buf);
   efree(buf);
}


static void
geist_save_text_xml(geist_text * txt, xmlNodePtr parent, xmlNsPtr ns)
{
   char *buf;

   /* FONTNAME */
   xmlNewTextChild(parent, ns, "Fontname", txt->fontname);

   /* FONTSIZE */
   buf = g_strdup_printf("%d", txt->fontsize);
   xmlNewProp(parent, "Fontsize", buf);
   efree(buf);

   /* TEXT */
   xmlNewTextChild(parent, ns, "Text", txt->text);

   /* ALIAS */
   buf = g_strdup_printf("%d", txt->alias);
   xmlNewProp(parent, "Alias", buf);
   efree(buf);

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
}

static void
geist_save_rect_xml(geist_rect * rect, xmlNodePtr parent, xmlNsPtr ns)
{
   gchar *buf;

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

}

static void
geist_save_fill_xml(geist_fill * fill, xmlNodePtr parent, xmlNsPtr ns)
{
   xmlNodePtr newfill;
   gchar *buf;

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
}
