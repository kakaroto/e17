/* alb_document_xml.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "alb_document_xml.h"

static alb_object *alb_parse_image_xml(xmlDocPtr doc,
                                       xmlNsPtr ns,
                                       xmlNodePtr cur);
static alb_object *alb_parse_text_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static alb_object *alb_parse_rect_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static alb_fill *alb_fill_parse_xml(xmlDocPtr doc,
                                    xmlNsPtr ns,
                                    xmlNodePtr cur);
static alb_object *alb_parse_line_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static alb_object *alb_object_parse_xml(xmlDocPtr doc,
                                        xmlNsPtr ns,
                                        xmlNodePtr cur,
                                        alb_layer * parent);
static gib_list *alb_object_list_parse_xml(xmlDocPtr doc,
                                           xmlNsPtr ns,
                                           xmlNodePtr cur,
                                           alb_layer * parent);
static alb_layer *alb_layer_parse_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur,
                                      alb_document * parent);
static gib_list *alb_layer_list_parse_xml(xmlDocPtr doc,
                                          xmlNsPtr ns,
                                          xmlNodePtr cur,
                                          alb_document * parent);
static alb_document *alb_document_parse_xml(xmlDocPtr doc,
                                            xmlNsPtr ns,
                                            xmlNodePtr cur,
                                            char *filename);
static alb_object *alb_parse_poly_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static gib_list *alb_parse_point_list_xml(xmlDocPtr doc,
                                          xmlNsPtr ns,
                                          xmlNodePtr cur);
static alb_point *alb_point_parse_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static gib_style *gib_style_parse_xml(xmlDocPtr doc,
                                      xmlNsPtr ns,
                                      xmlNodePtr cur);
static gib_list *gib_style_bits_parse_xml(xmlDocPtr doc,
                                          xmlNsPtr ns,
                                          xmlNodePtr cur);


static void alb_save_layer_xml(alb_layer * layer,
                               xmlNodePtr parent,
                               xmlNsPtr ns);
static void alb_save_object_xml(alb_object * obj,
                                xmlNodePtr parent,
                                xmlNsPtr ns);
static void alb_save_image_xml(alb_image * img,
                               xmlNodePtr parent,
                               xmlNsPtr ns);
static void alb_save_text_xml(alb_text * txt,
                              xmlNodePtr parent,
                              xmlNsPtr ns);
static void alb_save_rect_xml(alb_rect * rect,
                              xmlNodePtr parent,
                              xmlNsPtr ns);
static void alb_save_fill_xml(alb_fill * fill,
                              xmlNodePtr parent,
                              xmlNsPtr ns);
static void alb_save_line_xml(alb_line * line,
                              xmlNodePtr parent,
                              xmlNsPtr ns);
static void alb_save_point_xml(alb_point * point,
                               xmlNodePtr parent,
                               xmlNsPtr ns);
static void alb_save_poly_xml(alb_poly * poly,
                              xmlNodePtr parent,
                              xmlNsPtr ns);
static void alb_save_style_xml(alb_text * txt,
                               xmlNodePtr parent,
                               xmlNsPtr ns);
static void alb_save_style_bit_xml(gib_style_bit * b,
                                   xmlNodePtr parent,
                                   xmlNsPtr ns);

/* Utility functions */
int alb_xml_read_int(xmlNodePtr cur,
                     char *key,
                     int def);
void alb_xml_write_int(xmlNodePtr cur,
                       char *key,
                       int val);

/***********/
/* LOADING */
/***********/

alb_document *
alb_document_load_xml(char *xml,
                      gint * err_return)
{
  alb_document *d = NULL;
  xmlDocPtr doc;
  xmlNsPtr ns;
  xmlNodePtr cur;
  int found = 0;

  D_ENTER(3);

  doc = xmlParseFile(xml);
  if (doc == NULL) {
    *err_return = ERR_NO_FILE;
    D_RETURN(3, NULL);
  }

  /* Check the document is of the right type */
  cur = doc->children;
  if (cur == NULL) {
    *err_return = ERR_EMPTY_FILE;
    weprintf("%s is an empty document\n", xml);
    xmlFreeDoc(doc);
    D_RETURN(3, NULL);
  }
  ns =
    xmlSearchNsByHref(doc, doc->children, "http://www.linuxbrit.co.uk/alb");
  if (ns == NULL) {
    *err_return = ERR_WRONG_TYPE;
    weprintf("document %s is of the wrong type, Geist namespace not found\n",
             xml);
    xmlFreeDoc(doc);
    D_RETURN(3, NULL);
  }
  if (strcasecmp(cur->name, "Geist")) {
    *err_return = ERR_WRONG_TYPE;
    fprintf(stderr, "document of the wrong type, root node != Geist\n");
    xmlFreeDoc(doc);
    D_RETURN(3, NULL);
  }

  cur = cur->children;
  while (cur != NULL) {
    /* Now, walk the tree. */
    /* First level we expect just a Document */
    if ((!strcmp(cur->name, "Document")) && (cur->ns == ns)) {
      found++;
      /* So let's parse the project then */
      d = alb_document_parse_xml(doc, ns, cur, xml);
      break;
    }
    cur = cur->next;
  }
  if (!found) {
    *err_return = ERR_WRONG_TYPE;
    weprintf("document of the wrong type, Document expected, not found");
    xmlFreeDoc(doc);
    D_RETURN(3, NULL);
  }

  xmlFreeDoc(doc);

  D_RETURN(3, d);
}

static alb_document *
alb_document_parse_xml(xmlDocPtr doc,
                       xmlNsPtr ns,
                       xmlNodePtr cur,
                       char *filename)
{
  alb_document *ret;
  int w, h;
  char *name = NULL;
  alb_fill *fill = NULL;
  gib_list *layers = NULL;

  D_ENTER(3);

  w = alb_xml_read_int(cur, "Width", 500);
  h = alb_xml_read_int(cur, "Height", 500);

  ret = alb_document_new(w, h);
  ret->filename = estrdup(filename);

  cur = cur->children;
  while (cur != NULL) {
    if ((!strcmp(cur->name, "Name")) && (cur->ns == ns))
      name = xmlNodeGetContent(cur->children);
    else if ((!strcmp(cur->name, "Fill")) && (cur->ns == ns))
      fill = alb_fill_parse_xml(doc, ns, cur);
    else if ((!strcmp(cur->name, "Layers")) && (cur->ns == ns)) {
      layers = alb_layer_list_parse_xml(doc, ns, cur, ret);
    }
    cur = cur->next;
  }

  alb_document_rename(ret, name);
  xmlFree(name);
  if (ret->bg_fill)
    alb_fill_free(ret->bg_fill);
  ret->bg_fill = fill;
  gib_list_free(ret->layers);
  ret->layers = layers;

  D_RETURN(3, ret);
}

static alb_fill *
alb_fill_parse_xml(xmlDocPtr doc,
                   xmlNsPtr ns,
                   xmlNodePtr cur)
{
  alb_fill *ret;
  int a, r, g, b;

  D_ENTER(3);

  a = alb_xml_read_int(cur, "A", 255);
  r = alb_xml_read_int(cur, "R", 255);
  g = alb_xml_read_int(cur, "G", 255);
  b = alb_xml_read_int(cur, "B", 255);

  ret = alb_fill_new_coloured(r, g, b, a);

  D_RETURN(3, ret);
}

static gib_list *
alb_layer_list_parse_xml(xmlDocPtr doc,
                         xmlNsPtr ns,
                         xmlNodePtr cur,
                         alb_document * parent)
{
  gib_list *ret = NULL;
  alb_layer *layer = NULL;

  D_ENTER(3);
  cur = cur->children;

  while (cur != NULL) {
    if ((!strcmp(cur->name, "Layer")) && (cur->ns == ns)) {
      layer = alb_layer_parse_xml(doc, ns, cur, parent);
      if (layer)
        ret = gib_list_add_end(ret, layer);
      else
        weprintf("invalid layer found\n");
    }
    cur = cur->next;
  }
  D_RETURN(3, ret);
}


static alb_layer *
alb_layer_parse_xml(xmlDocPtr doc,
                    xmlNsPtr ns,
                    xmlNodePtr cur,
                    alb_document * parent)
{
  alb_layer *ret;

  D_ENTER(3);

  ret = alb_layer_new();
  ret->doc = parent;

  ret->alpha = alb_xml_read_int(cur, "Alpha", 255);
  ret->x_offset = alb_xml_read_int(cur, "X_Offset", 0);
  ret->y_offset = alb_xml_read_int(cur, "Y_Offset", 0);
  ret->visible = alb_xml_read_int(cur, "Visible", 0);

  cur = cur->children;

  while (cur != NULL) {
    if ((!strcmp(cur->name, "Name")) && (cur->ns == ns))
      ret->name = xmlNodeGetContent(cur->children);
    else if ((!strcmp(cur->name, "Objects")) && (cur->ns == ns))
      ret->objects = alb_object_list_parse_xml(doc, ns, cur, ret);
    cur = cur->next;
  }

  D_RETURN(3, ret);
}

static gib_list *
alb_object_list_parse_xml(xmlDocPtr doc,
                          xmlNsPtr ns,
                          xmlNodePtr cur,
                          alb_layer * parent)
{
  gib_list *ret = NULL;
  alb_object *obj;

  D_ENTER(3);

  cur = cur->children;

  while (cur != NULL) {
    if ((!strcmp(cur->name, "Object")) && (cur->ns == ns)) {
      obj = alb_object_parse_xml(doc, ns, cur, parent);
      if (obj)
        ret = gib_list_add_end(ret, obj);
      else
        weprintf("invalid object found");
    }
    cur = cur->next;
  }

  D_RETURN(3, ret);
}

static alb_object *
alb_object_parse_xml(xmlDocPtr doc,
                     xmlNsPtr ns,
                     xmlNodePtr cur,
                     alb_layer * parent)
{
  alb_object *ret = NULL;
  xmlNodePtr c;
  char *type = NULL;
  int type_int;

  D_ENTER(3);

  c = cur->children;

  while (c != NULL) {
    if ((!strcmp(c->name, "Type")) && (c->ns == ns)) {
      type = xmlNodeGetContent(c->children);
      break;
    }
    c = c->next;
  }

  if (!type) {
    weprintf("invalid object found, no 'type' element\n");
    D_RETURN(3, NULL);
  }

  /* specific object stuffs */
  /* TODO this breaks the object model right now */

  type_int = alb_object_get_type_from_string(type);
  xmlFree(type);
  switch (type_int) {
    case ALB_TYPE_RECT:
      ret = alb_parse_rect_xml(doc, ns, cur);
      break;
    case ALB_TYPE_TEXT:
      ret = alb_parse_text_xml(doc, ns, cur);
      break;
    case ALB_TYPE_IMAGE:
      ret = alb_parse_image_xml(doc, ns, cur);
      break;
    case ALB_TYPE_LINE:
      ret = alb_parse_line_xml(doc, ns, cur);
      break;
    case ALB_TYPE_POLY:
      ret = alb_parse_poly_xml(doc, ns, cur);
      break;
    default:
      weprintf("invalid object type found\n");
      break;
  }

  if (ret) {
    ret->layer = parent;
    ret->x = alb_xml_read_int(cur, "X", 0);
    ret->y = alb_xml_read_int(cur, "Y", 0);
    ret->w = alb_xml_read_int(cur, "W", 10);
    ret->h = alb_xml_read_int(cur, "H", 10);
    ret->alias = alb_xml_read_int(cur, "Alias", 0);

    if (alb_xml_read_int(cur, "Visible", 1))
      alb_object_show(ret);
    else
      alb_object_hide(ret);

    cur = cur->children;
    while (cur != NULL) {
      if ((!strcmp(cur->name, "Name")) && (cur->ns == ns)) {
        if (ret->name)
          efree(ret->name);
        ret->name = xmlNodeGetContent(cur->children);
      } else if ((!strcmp(cur->name, "Sizemode")) && (cur->ns == ns)) {
        char *temp;

        temp = xmlNodeGetContent(cur->children);
        ret->sizemode = alb_object_get_sizemode_from_string(temp);
        xmlFree(temp);
      } else if ((!strcmp(cur->name, "Alignment")) && (cur->ns == ns)) {
        char *temp;

        temp = xmlNodeGetContent(cur->children);
        ret->alignment = alb_object_get_alignment_from_string(temp);
        xmlFree(temp);
      }

      cur = cur->next;
    }
    alb_object_update_positioning(ret);
  }
  D_RETURN(3, ret);
}

static alb_object *
alb_parse_rect_xml(xmlDocPtr doc,
                   xmlNsPtr ns,
                   xmlNodePtr cur)
{
  alb_object *ret;
  int r, g, b, a;

  D_ENTER(3);

  a = alb_xml_read_int(cur, "A", 255);
  r = alb_xml_read_int(cur, "R", 255);
  g = alb_xml_read_int(cur, "G", 255);
  b = alb_xml_read_int(cur, "B", 255);

  ret = alb_rect_new_of_size(0, 0, 10, 10, a, r, g, b);

  D_RETURN(3, ret);
}

static alb_object *
alb_parse_line_xml(xmlDocPtr doc,
                   xmlNsPtr ns,
                   xmlNodePtr cur)
{
  alb_object *ret;
  int r, g, b, a, start_x, start_y, end_x, end_y;

  D_ENTER(3);

  a = alb_xml_read_int(cur, "A", 255);
  r = alb_xml_read_int(cur, "R", 255);
  g = alb_xml_read_int(cur, "G", 255);
  b = alb_xml_read_int(cur, "B", 255);
  start_x = alb_xml_read_int(cur, "Start_X", 0);
  start_y = alb_xml_read_int(cur, "Start_Y", 0);
  end_x = alb_xml_read_int(cur, "End_X", 10);
  end_y = alb_xml_read_int(cur, "End_Y", 10);

  ret = alb_line_new_from_to(start_x, start_y, end_x, end_y, a, r, g, b);

  D_RETURN(3, ret);
}

static alb_object *
alb_parse_poly_xml(xmlDocPtr doc,
                   xmlNsPtr ns,
                   xmlNodePtr cur)
{
  alb_object *ret;
  int filled, closed, r, g, b, a;
  gib_list *points = NULL;
  alb_poly *poly;

  D_ENTER(3);

  a = alb_xml_read_int(cur, "A", 255);
  r = alb_xml_read_int(cur, "R", 255);
  g = alb_xml_read_int(cur, "G", 255);
  b = alb_xml_read_int(cur, "B", 255);
  filled = alb_xml_read_int(cur, "Filled", 0);
  closed = alb_xml_read_int(cur, "Closed", 0);

  cur = cur->children;
  while (cur != NULL) {
    if ((!strcmp(cur->name, "Points")) && (cur->ns == ns))
      points =
        gib_list_add_end(points, alb_parse_point_list_xml(doc, ns, cur));
    cur = cur->next;
  }

  ret = alb_poly_new_from_points(points, a, r, g, b);
  poly = ALB_POLY(ret);
  poly->filled = filled;
  poly->closed = closed;

  D_RETURN(3, ret);
}

static gib_list *
alb_parse_point_list_xml(xmlDocPtr doc,
                         xmlNsPtr ns,
                         xmlNodePtr cur)
{
  gib_list *ret = NULL;
  alb_point *point = NULL;

  D_ENTER(3);
  cur = cur->children;

  while (cur != NULL) {
    if ((!strcmp(cur->name, "Point")) && (cur->ns == ns)) {
      point = alb_point_parse_xml(doc, ns, cur);
      if (point)
        ret = gib_list_add_end(ret, point);
      else
        weprintf("invalid point found\n");
    }
    cur = cur->next;
  }
  D_RETURN(3, ret);
}

static alb_point *
alb_point_parse_xml(xmlDocPtr doc,
                    xmlNsPtr ns,
                    xmlNodePtr cur)
{
  alb_point *ret;

  D_ENTER(3);

  ret =
    alb_point_new(alb_xml_read_int(cur, "X", 1),
                  alb_xml_read_int(cur, "Y", 1));

  D_RETURN(3, ret);
}

static alb_object *
alb_parse_text_xml(xmlDocPtr doc,
                   xmlNsPtr ns,
                   xmlNodePtr cur)
{
  alb_object *ret = NULL;
  char *fontname = NULL;
  int fontsize = 0;
  char *text = NULL;
  int wordwrap = 0;
  int justification = 0;
  gib_style *style = NULL;
  int r, g, b, a;

  D_ENTER(3);

  fontsize = alb_xml_read_int(cur, "Fontsize", 12);
  wordwrap = alb_xml_read_int(cur, "Wordwrap", 1);
  a = alb_xml_read_int(cur, "A", 255);
  r = alb_xml_read_int(cur, "R", 255);
  g = alb_xml_read_int(cur, "G", 255);
  b = alb_xml_read_int(cur, "B", 255);

  cur = cur->children;
  while (cur != NULL) {
    if ((!strcmp(cur->name, "Fontname")) && (cur->ns == ns))
      fontname = xmlNodeGetContent(cur->children);
    else if ((!strcmp(cur->name, "Text")) && (cur->ns == ns))
      text = xmlNodeGetContent(cur->children);
    else if ((!strcmp(cur->name, "Justification")) && (cur->ns == ns)) {
      char *temp;

      temp = xmlNodeGetContent(cur->children);
      justification = alb_text_get_justification_from_string(temp);
      xmlFree(temp);
    } else if ((!strcmp(cur->name, "Style")) && (cur->ns == ns))
      style = gib_style_parse_xml(doc, ns, cur);
    cur = cur->next;
  }

  if (fontname && text) {
    ret =
      alb_text_new_with_text(0, 0, fontname, fontsize, text, justification,
                             wordwrap, 0, 0, 0, 0);
    ALB_TEXT(ret)->style = style;
    ALB_TEXT(ret)->r = r;
    ALB_TEXT(ret)->g = g;
    ALB_TEXT(ret)->b = b;
    ALB_TEXT(ret)->a = a;
    alb_text_update_image(ALB_TEXT(ret), FALSE);
  }

  D_RETURN(3, ret);
}

static gib_style *
gib_style_parse_xml(xmlDocPtr doc,
                    xmlNsPtr ns,
                    xmlNodePtr cur)
{
  gib_style *ret = NULL;
  char *name = NULL;
  gib_list *bits = NULL;

  D_ENTER(3);

  cur = cur->children;
  while (cur != NULL) {
    if ((!strcmp(cur->name, "Bits")) && (cur->ns == ns)) {
      bits = gib_style_bits_parse_xml(doc, ns, cur);
    } else if ((!strcmp(cur->name, "Name")) && (cur->ns == ns))
      name = xmlNodeGetContent(cur->children);
    cur = cur->next;
  }

  ret = gib_style_new(name);
  xmlFree(name);
  ret->bits = bits;

  D_RETURN(3, ret);
}

static gib_list *
gib_style_bits_parse_xml(xmlDocPtr doc,
                         xmlNsPtr ns,
                         xmlNodePtr cur)
{
  gib_list *ret = NULL;

  D_ENTER(3);

  cur = cur->children;
  while (cur != NULL) {
    int x_off, y_off, r, g, b, a;

    if ((!strcmp(cur->name, "Bit")) && (cur->ns == ns)) {
      x_off = alb_xml_read_int(cur, "X_Offset", 0);
      y_off = alb_xml_read_int(cur, "Y_Offset", 0);
      a = alb_xml_read_int(cur, "A", 255);
      r = alb_xml_read_int(cur, "R", 255);
      g = alb_xml_read_int(cur, "G", 255);
      b = alb_xml_read_int(cur, "B", 255);
      ret =
        gib_list_add_end(ret, gib_style_bit_new(x_off, y_off, r, g, b, a));
    }
    cur = cur->next;
  }

  D_RETURN(3, ret);
}

static alb_object *
alb_parse_image_xml(xmlDocPtr doc,
                    xmlNsPtr ns,
                    xmlNodePtr cur)
{
  alb_object *ret = NULL;
  char *filename = NULL;
  int mod_a, mod_r, mod_g, mod_b;

  D_ENTER(3);

  mod_a = alb_xml_read_int(cur, "Colormod_A", FULL_OPACITY);
  mod_r = alb_xml_read_int(cur, "Colormod_R", 100);
  mod_g = alb_xml_read_int(cur, "Colormod_G", 100);
  mod_b = alb_xml_read_int(cur, "Colormod_B", 100);
  cur = cur->children;
  while (cur != NULL) {
    if ((!strcmp(cur->name, "Filename")) && (cur->ns == ns)) {
      filename = xmlNodeGetContent(cur->children);
    }
    cur = cur->next;
  }

  if (filename) {
    ret = alb_image_new_from_file(0, 0, filename);
    if (ret) {
      ALB_IMAGE(ret)->image_mods[A] = mod_a;
      ALB_IMAGE(ret)->image_mods[R] = mod_r;
      ALB_IMAGE(ret)->image_mods[G] = mod_g;
      ALB_IMAGE(ret)->image_mods[B] = mod_b;
    }
    xmlFree(filename);
  } else
    weprintf("invalid image object found - no 'Filename' element\n");

  D_RETURN(3, ret);
}


/**********/
/* SAVING */
/**********/

int
alb_document_save_xml(alb_document * document,
                      char *filename)
{
  xmlDocPtr doc;
  xmlNodePtr tree, subtree;
  xmlNsPtr ns;
  gib_list *kids;

  D_ENTER(3);

  doc = xmlNewDoc("1.0");
  if (!doc) {
    weprintf("couldn't create XML document");
    D_RETURN(3, 1);
  }
  doc->children = xmlNewDocNode(doc, NULL, "Geist", NULL);
  ns = xmlNewNs(doc->children, "http://www.linuxbrit.co.uk/alb", "alb");

  tree = xmlNewChild(doc->children, ns, "Document", NULL);

  /* Document properties */
  xmlNewTextChild(tree, ns, "Name", document->name);

  alb_xml_write_int(tree, "Width", document->w);
  alb_xml_write_int(tree, "Height", document->h);

  /* Bg Fill */
  alb_save_fill_xml(document->bg_fill, tree, ns);

  /* Document layers */
  subtree = xmlNewChild(tree, ns, "Layers", NULL);
  kids = document->layers;
  while (kids) {
    alb_save_layer_xml(kids->data, subtree, ns);
    kids = kids->next;
  }

  xmlSaveFile(filename, doc);

  xmlFreeDoc(doc);

  D_RETURN(3, 0);
}

static void
alb_save_layer_xml(alb_layer * layer,
                   xmlNodePtr parent,
                   xmlNsPtr ns)
{
  /* recursive */
  xmlNodePtr newlayer, subtree = NULL;
  gib_list *kids;

  D_ENTER(3);

  newlayer = xmlNewChild(parent, ns, "Layer", NULL);

  xmlNewTextChild(newlayer, ns, "Name", layer->name);
  alb_xml_write_int(newlayer, "Alpha", layer->alpha);
  alb_xml_write_int(newlayer, "X_Offset", layer->x_offset);
  alb_xml_write_int(newlayer, "Y_Offset", layer->y_offset);
  alb_xml_write_int(newlayer, "Visible", layer->visible);

  /* Other properties go here */

  if (layer->objects)
    subtree = xmlNewChild(newlayer, ns, "Objects", NULL);

  kids = layer->objects;
  while (kids) {
    alb_save_object_xml(kids->data, subtree, ns);
    kids = kids->next;
  }
  D_RETURN_(3);
}

static void
alb_save_object_xml(alb_object * obj,
                    xmlNodePtr parent,
                    xmlNsPtr ns)
{
  xmlNodePtr newobject;

  D_ENTER(3);

  newobject = xmlNewChild(parent, ns, "Object", NULL);

  xmlNewTextChild(newobject, ns, "Type", alb_object_get_type_string(obj));
  xmlNewTextChild(newobject, ns, "Name", obj->name);
  alb_xml_write_int(newobject, "X", obj->x);
  alb_xml_write_int(newobject, "Y", obj->y);
  alb_xml_write_int(newobject, "W", obj->w);
  alb_xml_write_int(newobject, "H", obj->h);
  alb_xml_write_int(newobject, "Alias", obj->alias);
  alb_xml_write_int(newobject, "Visible", alb_object_get_state(obj, VISIBLE));
  xmlNewTextChild(newobject, ns, "Sizemode",
                  alb_object_get_sizemode_string(obj));
  xmlNewTextChild(newobject, ns, "Alignment",
                  alb_object_get_alignment_string(obj));

  /* Other generic object properties go here */


  /* Type-specific properties */
  switch (obj->type) {
    case ALB_TYPE_IMAGE:
      alb_save_image_xml(ALB_IMAGE(obj), newobject, ns);
      break;
    case ALB_TYPE_TEXT:
      alb_save_text_xml(ALB_TEXT(obj), newobject, ns);
      break;
    case ALB_TYPE_RECT:
      alb_save_rect_xml(ALB_RECT(obj), newobject, ns);
      break;
    case ALB_TYPE_LINE:
      alb_save_line_xml(ALB_LINE(obj), newobject, ns);
      break;
    case ALB_TYPE_POLY:
      alb_save_poly_xml(ALB_POLY(obj), newobject, ns);
      break;
    default:
      weprintf("IMPLEMENT ME!\n");
      break;
  }
  D_RETURN_(3);
}

static void
alb_save_image_xml(alb_image * img,
                   xmlNodePtr parent,
                   xmlNsPtr ns)
{
  D_ENTER(3);

  alb_xml_write_int(parent, "Colormod_A", img->image_mods[A]);
  alb_xml_write_int(parent, "Colormod_R", img->image_mods[R]);
  alb_xml_write_int(parent, "Colormod_G", img->image_mods[G]);
  alb_xml_write_int(parent, "Colormod_B", img->image_mods[B]);
  xmlNewTextChild(parent, ns, "Filename", img->filename);

  D_RETURN_(3);
}


static void
alb_save_text_xml(alb_text * txt,
                  xmlNodePtr parent,
                  xmlNsPtr ns)
{
  D_ENTER(3);

  xmlNewTextChild(parent, ns, "Fontname", txt->fontname);
  alb_xml_write_int(parent, "Fontsize", txt->fontsize);
  alb_xml_write_int(parent, "Wordwrap", txt->wordwrap);
  alb_xml_write_int(parent, "R", txt->r);
  alb_xml_write_int(parent, "G", txt->g);
  alb_xml_write_int(parent, "B", txt->b);
  alb_xml_write_int(parent, "A", txt->a);
  xmlNewTextChild(parent, ns, "Text", txt->text);
  xmlNewTextChild(parent, ns, "Justification",
                  alb_text_get_justification_string(txt->justification));
  alb_save_style_xml(txt, parent, ns);

  D_RETURN_(3);
}

static void
alb_save_style_xml(alb_text * txt,
                   xmlNodePtr parent,
                   xmlNsPtr ns)
{
  xmlNodePtr subtree;
  gib_list *l;

  D_ENTER(3);
  subtree = xmlNewChild(parent, ns, "Style", NULL);
  xmlNewTextChild(subtree, ns, "Name", txt->style->name);

  subtree = xmlNewChild(subtree, ns, "Bits", NULL);
  l = txt->style->bits;
  while (l) {
    alb_save_style_bit_xml(l->data, subtree, ns);
    l = l->next;
  }

  D_RETURN_(3);
}

static void
alb_save_style_bit_xml(gib_style_bit * b,
                       xmlNodePtr parent,
                       xmlNsPtr ns)
{
  xmlNodePtr subtree;

  D_ENTER(3);

  subtree = xmlNewChild(parent, ns, "Bit", NULL);
  alb_xml_write_int(subtree, "X_Offset", b->x_offset);
  alb_xml_write_int(subtree, "Y_Offset", b->y_offset);
  alb_xml_write_int(subtree, "R", b->r);
  alb_xml_write_int(subtree, "G", b->g);
  alb_xml_write_int(subtree, "B", b->b);
  alb_xml_write_int(subtree, "A", b->a);

  D_RETURN_(3);
}

static void
alb_save_poly_xml(alb_poly * poly,
                  xmlNodePtr parent,
                  xmlNsPtr ns)
{
  gib_list *kids;
  xmlNodePtr subtree;

  D_ENTER(3);

  alb_xml_write_int(parent, "R", poly->r);
  alb_xml_write_int(parent, "G", poly->g);
  alb_xml_write_int(parent, "B", poly->b);
  alb_xml_write_int(parent, "A", poly->a);
  alb_xml_write_int(parent, "Filled", poly->filled);
  alb_xml_write_int(parent, "Closed", poly->closed);

  if (poly->points) {
    subtree = xmlNewChild(parent, ns, "Points", NULL);

    kids = poly->points;
    while (kids) {
      alb_save_point_xml(kids->data, subtree, ns);
      kids = kids->next;
    }
  }
  D_RETURN_(3);
}

static void
alb_save_point_xml(alb_point * point,
                   xmlNodePtr parent,
                   xmlNsPtr ns)
{
  xmlNodePtr subtree;

  D_ENTER(3);

  subtree = xmlNewChild(parent, ns, "Point", NULL);
  alb_xml_write_int(subtree, "X", point->x);
  alb_xml_write_int(subtree, "Y", point->y);

  D_RETURN_(3);
}

static void
alb_save_rect_xml(alb_rect * rect,
                  xmlNodePtr parent,
                  xmlNsPtr ns)
{
  D_ENTER(3);

  alb_xml_write_int(parent, "R", rect->r);
  alb_xml_write_int(parent, "G", rect->g);
  alb_xml_write_int(parent, "B", rect->b);
  alb_xml_write_int(parent, "A", rect->a);

  D_RETURN_(3);
}

static void
alb_save_line_xml(alb_line * line,
                  xmlNodePtr parent,
                  xmlNsPtr ns)
{
  alb_object *obj;

  D_ENTER(3);

  obj = ALB_OBJECT(line);

  alb_xml_write_int(parent, "R", line->r);
  alb_xml_write_int(parent, "G", line->g);
  alb_xml_write_int(parent, "B", line->b);
  alb_xml_write_int(parent, "A", line->a);

  alb_xml_write_int(parent, "Start_X",
                    line->start.x + obj->x + obj->rendered_x);
  alb_xml_write_int(parent, "Start_Y",
                    line->start.y + obj->y + obj->rendered_y);
  alb_xml_write_int(parent, "End_X", line->end.x + obj->x + obj->rendered_x);
  alb_xml_write_int(parent, "End_Y", line->end.y + obj->y + obj->rendered_y);

  D_RETURN_(3);
}


static void
alb_save_fill_xml(alb_fill * fill,
                  xmlNodePtr parent,
                  xmlNsPtr ns)
{
  xmlNodePtr newfill;

  D_ENTER(3);

  newfill = xmlNewChild(parent, ns, "Fill", NULL);

  alb_xml_write_int(newfill, "R", fill->r);
  alb_xml_write_int(newfill, "G", fill->g);
  alb_xml_write_int(newfill, "B", fill->b);
  alb_xml_write_int(newfill, "A", fill->a);

  D_RETURN_(3);
}

int
alb_xml_read_int(xmlNodePtr cur,
                 char *key,
                 int def)
{
  int i;
  char *prop;

  D_ENTER(5);

  prop = xmlGetProp(cur, key);
  if (prop)
    i = atoi(prop);
  else {
    weprintf("no value found for property '%s', using default value of %d",
             key, def);
    i = def;
  }
  xmlFree(prop);

  D_RETURN(5, i);
}

void
alb_xml_write_int(xmlNodePtr cur,
                  char *key,
                  int val)
{
  char buf[20];

  D_ENTER(5);

  snprintf(buf, sizeof(buf), "%d", val);
  xmlNewProp(cur, key, buf);

  D_RETURN_(5);
}
