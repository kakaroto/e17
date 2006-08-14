#include "border_props.h"

struct _E_Config_Dialog_Data
{
   char *ID;
   char *name;
   char *class;
   char *title;
   char *role;
   char *type;
   char *transient;
   char *position;
   char *size;
   char *layer;
   char *border;
   char *sticky;
   char *desk;
   char *shaded;
   char *zone;
   char *winlist;
   char *fullscreen;
   char *icon;
};

/* Protos */
static void *_border_props_dialog_create_data(E_Config_Dialog *cfd);
static void _border_props_dialog_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_border_props_dialog_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static Evas_Object *_border_props_dialog_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

static char *_true = "true";
static char *_false = "false";

EAPI void
border_props_dialog(E_Container *con, E_Border * bd)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   if (!bd)
      return;
   v = E_NEW(E_Config_Dialog_View, 1);  /* This gets freed by e_config_dialog. */
   if (v)
     {
        /* Dialog Methods */
        v->create_cfdata = _border_props_dialog_create_data;
        v->free_cfdata = _border_props_dialog_free_data;
        v->basic.create_widgets = _border_props_dialog_basic_create_widgets;
        v->advanced.create_widgets = _border_props_dialog_advanced_create_widgets;

        /* Create The Dialog */
        cfd = e_config_dialog_new(con, D_("Window properties"), "Emu", "_e_modules_emu_border_dialog", NULL, 0, v, bd);
     }
}

EAPI char *
border_props_substitute(E_Border * bd, char type)
{
   int found = 1;
   char buf[32];
   const char *result = NULL;

   buf[0] = '\0';
   switch (type)
     {
     case 'C':
        result = bd->client.icccm.class;
        break;
     case 'H':
        result = (bd->client.icccm.transient_for != 0) ? _true : _false;
        break;
     case 'I':
        snprintf(buf, sizeof(buf), "%#x", bd->win);
        break;
     case 'N':
        result = bd->client.icccm.name;
        break;
     case 'R':
        result = bd->client.icccm.window_role;
        break;
     case 'S':
        snprintf(buf, sizeof(buf), "%d", bd->client.netwm.type);
        break;
     case 'T':
        result = (bd->client.netwm.name) ? bd->client.netwm.name : bd->client.icccm.title;
        break;
     case 'b':
        result = bd->client.border.name;
        break;
     case 'd':
        result = bd->desk->name;
        break;
     case 'f':
        result = (bd->fullscreen) ? _true : _false;
        break;
     case 'i':
        result = (bd->iconic) ? _true : _false;
        break;
     case 'l':
        snprintf(buf, sizeof(buf), "%d", bd->layer);
        break;
     case 'p':
        snprintf(buf, sizeof(buf), "%d,%d", bd->x, bd->y);
        break;
     case 'r':
        result = (bd->shaded) ? _true : _false;
        break;
     case 's':
        snprintf(buf, sizeof(buf), "%dx%d", bd->w, bd->h);
        break;
     case 'w':
        result = (bd->user_skip_winlist) ? _true : _false;
        break;
     case 'y':
        result = (bd->sticky) ? _true : _false;
        break;
     case 'z':
        result = bd->zone->name;
        break;
     default:
        found = 0;
        break;
     }
   if (!result)
      result = buf;
   if (found)
      return strdup(result);
   else
      return NULL;
}

static void
_border_props_dialog_fill_data(E_Border * bd, E_Config_Dialog_Data *cfdata)
{
   if (!cfdata->ID)
      cfdata->ID = border_props_substitute(bd, 'I');
   if (!cfdata->name)
      cfdata->name = border_props_substitute(bd, 'N');
   if (!cfdata->class)
      cfdata->class = border_props_substitute(bd, 'C');
   if (!cfdata->title)
      cfdata->title = border_props_substitute(bd, 'T');
   if (!cfdata->role)
      cfdata->role = border_props_substitute(bd, 'R');
   if (!cfdata->type)
      cfdata->type = border_props_substitute(bd, 'S');
   if (!cfdata->transient)
      cfdata->transient = border_props_substitute(bd, 'H');
   if (!cfdata->position)
      cfdata->position = border_props_substitute(bd, 'p');
   if (!cfdata->size)
      cfdata->size = border_props_substitute(bd, 's');
   if (!cfdata->layer)
      cfdata->layer = border_props_substitute(bd, 'l');
   if (!cfdata->border)
      cfdata->border = border_props_substitute(bd, 'b');
   if (!cfdata->sticky)
      cfdata->sticky = border_props_substitute(bd, 'y');
   if (!cfdata->desk)
      cfdata->desk = border_props_substitute(bd, 'd');
   if (!cfdata->shaded)
      cfdata->shaded = border_props_substitute(bd, 'r');
   if (!cfdata->zone)
      cfdata->zone = border_props_substitute(bd, 'z');
   if (!cfdata->winlist)
      cfdata->winlist = border_props_substitute(bd, 'w');
   if (!cfdata->fullscreen)
      cfdata->fullscreen = border_props_substitute(bd, 'f');
   if (!cfdata->icon)
      cfdata->icon = border_props_substitute(bd, 'i');
}

static void *
_border_props_dialog_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   E_Border *bd;

   bd = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _border_props_dialog_fill_data(bd, cfdata);
   return cfdata;
}

static void
_border_props_dialog_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   E_FREE(cfdata->icon);
   E_FREE(cfdata->fullscreen);
   E_FREE(cfdata->winlist);
   E_FREE(cfdata->zone);
   E_FREE(cfdata->shaded);
   E_FREE(cfdata->desk);
   E_FREE(cfdata->sticky);
   E_FREE(cfdata->border);
   E_FREE(cfdata->layer);
   E_FREE(cfdata->size);
   E_FREE(cfdata->position);
   E_FREE(cfdata->transient);
   E_FREE(cfdata->type);
   E_FREE(cfdata->role);
   E_FREE(cfdata->title);
   E_FREE(cfdata->class);
   E_FREE(cfdata->name);
   E_FREE(cfdata->ID);

   free(cfdata);
}

static Evas_Object *
_border_props_dialog_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   E_Border *bd;
   Evas_Object *o, *ob, *of;

   bd = cfd->data;
   _border_props_dialog_fill_data(bd, cfdata);
   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_frametable_add(evas, D_("Window identification"), 0);
   ob = e_widget_label_add(evas, D_("Title (%T)"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->title);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Properties"), 0);
   ob = e_widget_label_add(evas, D_("Position (%p)"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->position);     // bd->x, bd->y
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Size (%s)"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->size); // bd->w, bd->h
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Border style (%b)"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->border);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Stickiness (%p)"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->sticky);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Virtual desktop (%d)"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->desk);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Shaded state (%r)"));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->shaded);
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Fullscreen (%f)"));
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->fullscreen);
   e_widget_frametable_object_append(of, ob, 1, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Iconified (%i)"));
   e_widget_frametable_object_append(of, ob, 0, 7, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->icon);
   e_widget_frametable_object_append(of, ob, 1, 7, 1, 1, 1, 1, 1, 1);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static Evas_Object *
_border_props_dialog_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   E_Border *bd;
   Evas_Object *o, *ob, *of;

   bd = cfd->data;
   _border_props_dialog_fill_data(bd, cfdata);
   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_frametable_add(evas, D_("Window identification"), 0);
   ob = e_widget_label_add(evas, D_("Window ID (%I)"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->ID);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Window name (%N)"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->name);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Window class (%C)"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->class);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Title (%T)"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->title);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Window role (%R)"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->role);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Window type (%S)"));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->type);
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Transient (%H)"));
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->transient);
   e_widget_frametable_object_append(of, ob, 1, 6, 1, 1, 1, 1, 1, 1);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Properties"), 0);
   ob = e_widget_label_add(evas, D_("Position (%p)"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->position);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Size (%s)"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->size);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Layer (%l)"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->layer);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Border style (%b)"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->border);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Stickiness (%y)"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->sticky);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Virtual desktop (%d)"));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->desk);
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Shaded state (%r)"));
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->shaded);
   e_widget_frametable_object_append(of, ob, 1, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Screen zone (%z)"));
   e_widget_frametable_object_append(of, ob, 0, 7, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->zone);
   e_widget_frametable_object_append(of, ob, 1, 7, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Window list skip (%w)"));
   e_widget_frametable_object_append(of, ob, 0, 8, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->winlist);
   e_widget_frametable_object_append(of, ob, 1, 8, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Fullscreen (%f)"));
   e_widget_frametable_object_append(of, ob, 0, 9, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->fullscreen);
   e_widget_frametable_object_append(of, ob, 1, 9, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, D_("Iconified (%i)"));
   e_widget_frametable_object_append(of, ob, 0, 10, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, cfdata->icon);
   e_widget_frametable_object_append(of, ob, 1, 10, 1, 1, 1, 1, 1, 1);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}
