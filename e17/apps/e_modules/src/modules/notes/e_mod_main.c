#include "e.h"
#include "config.h"
#include "e_mod_main.h"

#include <time.h>
#include <Esmart/Esmart_Textarea.h>
/* TODO List:
 * 
 * 
 */

/* module private routines */
static Note  *_note_init                 (E_Module *m);
static void    _note_shutdown             (Note *n);
static E_Menu *_note_config_menu_new      (Note *n);
static void    _note_config_menu_del      (Note *n, E_Menu *m);
static void    _note_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change);

static void _note_menu_bgcolor_white (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_menu_bgcolor_blue (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_menu_bgcolor_yellow (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_menu_bgcolor_green (void *data, E_Menu *m, E_Menu_Item *mi);
    
    

static int  _note_face_init           (Note_Face *nf);
static void _note_face_free           (Note_Face *nf);

char          *_note_module_dir;
static int     _note_count;


/* public module routines. all modules must have these */
void *
init (E_Module *m)
{
   Note *n;
   
   /* check module api version */
   if (m->api->version < E_MODULE_API_VERSION)
     {
	e_error_dialog_show ("Module API Error",
			     "Error initializing Module: note\n"
			     "It requires a minimum module API version of: %i.\n"
			     "The module API advertized by Enlightenment is: %i.\n"
			     "Aborting module.",
			     E_MODULE_API_VERSION,
			     m->api->version);
	return NULL;
     }
   /* actually init ibar */
   n = _note_init (m);
   m->config_menu = _note_config_menu_new (n);

   
   return n;
}

int
shutdown (E_Module *m)
{
   Note *n;
   
   n = m->data;
   if (n)
     {
	if (m->config_menu)
	  {
	     _note_config_menu_del (n, m->config_menu);
	     m->config_menu = NULL;
	  }
	_note_shutdown (n);
     }
   
   return 1;
}

int
save (E_Module *m)
{
   Note *n;
   
   n = m->data;
   e_config_domain_save("module.note", n->conf_edd, n->conf);
   
   return 1;
}

int
info (E_Module *m)
{
   m->label = strdup("Notes");
   m->icon_file = strdup(PACKAGE_LIB_DIR "/e_modules/notes/module_icon.png");
   
   return 1;
}

int
about (E_Module *m)
{
   e_error_dialog_show ("Enlightenment Notes Module",
			"A desktop sticky notes module.");
   return 1;
}

/* module private routines */
static Note *
_note_init (E_Module *m)
{
   Note *n;
   Evas_List *managers, *l, *l2;
 
   _note_count = 0;
   n = calloc(1, sizeof(Note));
   if (!n) return NULL;
   
   /* Configuration */
   
   n->conf_edd = E_CONFIG_DD_NEW("Note_Config", Config);
#undef T
#undef D
#define T Config
#define D n->conf_edd
   E_CONFIG_VAL(D, T, height, INT);
   E_CONFIG_VAL(D, T, width, INT);
   E_CONFIG_VAL(D, T, bgcolor, INT);   

   if (!n->conf)
     {
	n->conf = E_NEW (Config, 1);
	n->conf->height = 320;
	n->conf->width = 200;
	n->conf->bgcolor = 2;
     }
   E_CONFIG_LIMIT(n->conf->height, 48, 800);
   E_CONFIG_LIMIT(n->conf->width, 48, 800);
   E_CONFIG_LIMIT(n->conf->bgcolor, 0, 10);
   
   managers = e_manager_list ();
   for (l = managers; l; l = l->next)
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;
	     Note_Face  *nf;
	     
	     con = l2->data;
	     nf = calloc(1, sizeof(Note_Face));
	     if (nf)
	       {
		  n->face = nf;
		  nf->note = n;
		  nf->con   = con;
		  nf->evas  = con->bg_evas;
		  if (!_note_face_init(nf))
		    return NULL;
	       }
	  }
     }
   
   return n;
}

static void
_note_shutdown (Note *n)
{
   free(n->conf);
   E_CONFIG_DD_FREE(n->conf_edd);
   _note_face_free(n->face);
   free(n);
}

static E_Menu *
_note_config_menu_new (Note *n)
{
   E_Menu      *mn;
   E_Menu_Item *mi;
   
   /* FIXME: hook callbacks to each menu item */
   mn = e_menu_new ();
   
   mi = e_menu_item_new (mn);
   e_menu_item_label_set (mi, "White Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (n->conf->bgcolor == BGCOLOR_WHITE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_white, n);   
   
   mi = e_menu_item_new (mn);
   e_menu_item_label_set (mi, "Yellow Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (n->conf->bgcolor == BGCOLOR_YELLOW) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_yellow, n);
   
   mi = e_menu_item_new (mn);
   e_menu_item_label_set (mi, "Blue Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (n->conf->bgcolor == BGCOLOR_BLUE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_blue, n);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Green Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (n->conf->bgcolor == BGCOLOR_GREEN) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_blue, n);
   
   n->config_menu = mn;
   
   return mn;
}

static void
_note_menu_bgcolor_green (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note *n;
   Evas_Object *bg;
      
   n = (Note *)data;
   bg = evas_object_rectangle_add(n->face->evas);
   evas_object_color_set(bg, 187, 243, 168, 100);
   esmart_textarea_bg_set(n->face->note_object, bg);
}

static void
_note_menu_bgcolor_yellow (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note *n;
   Evas_Object *bg;
      
   n = (Note *)data;
   bg = evas_object_rectangle_add(n->face->evas);
   evas_object_color_set(bg, 245, 248, 27, 100);
   esmart_textarea_bg_set(n->face->note_object, bg);
}

static void
_note_menu_bgcolor_white (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note *n;
   Evas_Object *bg;
      
   n = (Note *)data;
   bg = evas_object_rectangle_add(n->face->evas);
   evas_object_color_set(bg, 255, 255, 255, 100);
   esmart_textarea_bg_set(n->face->note_object, bg);
}

static void
_note_menu_bgcolor_blue (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note *n;
   Evas_Object *bg;
      
   n = (Note *)data;
   bg = evas_object_rectangle_add(n->face->evas);
   evas_object_color_set(bg, 149, 207, 226, 100);
   esmart_textarea_bg_set(n->face->note_object, bg);
}



static void
_note_config_menu_del (Note *n, E_Menu *m)
{
   e_object_del (E_OBJECT(m));
}

//static void
//_flame_config_palette_set (Flame *f, Flame_Palette_Type type)
//{
//   switch (type)
//     {
//      case GOLD_PALETTE:
//	_flame_palette_gold_set (f->face);
//	break;
//      case FIRE_PALETTE:
//	_flame_palette_fire_set (f->face);
//	break;
//      case PLASMA_PALETTE:
//	_flame_palette_plasma_set (f->face);
//	break;
//      default:
//	break;
//     }
//}

static int
_note_face_init (Note_Face *nf)
{
   Evas_Object *o;
   Evas_Coord   ww, hh;
   int         size;
   int         note_width, note_height;
   
   /* set up the note object */
   o = esmart_textarea_add (nf->evas);
   evas_output_viewport_get(nf->evas, NULL, NULL, &ww, &hh);
   nf->ww = ww;
   evas_object_move (o, 0, hh - nf->note->conf->height + 3);
   //evas_object_resize (o, nf->ww, nf->note->conf->height);
   evas_object_resize(o, nf->note->conf->width, nf->note->conf->height);
   evas_object_pass_events_set(o, 1);
   evas_object_layer_set (o, 20);
   evas_object_show (o);
   nf->note_object = o;
   
   evas_event_freeze(nf->con->bg_evas);
   
   /*
   o = edje_object_add(nf->evas);
   nf->note_edje = o;
   edje_object_file_set(o,
			e_path_find(path_themes, "default.edj"),
			"modules/ibar/main");
   evas_object_show(o);
   */
   
   
   nf->gmc =  e_gadman_client_new(nf->con->gadman);
   e_gadman_client_domain_set(nf->gmc, "module.note", _note_count++);
   e_gadman_client_policy_set(nf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(nf->gmc, 4, 4);
   e_gadman_client_max_size_set(nf->gmc, 512, 512);
   e_gadman_client_auto_size_set(nf->gmc, 320, 240);
   e_gadman_client_align_set(nf->gmc, 0.0, 1.0);
   e_gadman_client_aspect_set(nf->gmc, 1.0, 1.0);
   e_gadman_client_resize(nf->gmc, 40, 40);
   e_gadman_client_change_func_set(nf->gmc, _note_face_cb_gmc_change, nf);
   e_gadman_client_load(nf->gmc);
   
   evas_event_thaw(nf->con->bg_evas);
   
   
   //_flame_config_palette_set (ff->flame, ff->flame->conf->palette_type);
   
   return 1;
}

static void
_note_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change)
{
   Note_Face *nf;
   Evas_Coord x, y, w, h;
   
   nf = data;
   switch(change)
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(nf->gmc, &x, &y, &w, &h);
	evas_object_move(nf->note_object, x, y);
	evas_object_resize(nf->note_object, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(nf->note_object);
	break;
      case E_GADMAN_CHANGE_EDGE:
      case E_GADMAN_CHANGE_ZONE:
	/* FIXME
	 * Must we do something here?
	 */
	break;	
     }
}
    
static void
_note_face_free(Note_Face *nf)
{
   evas_object_del (nf->note_object);
   _note_count--;
   free (nf);
}

static int 
_note_cb_event_container_resize(void *data, int type, void *event)
{
   Note_Face *nf;
   Evas_Object *o;
   Evas_Coord   ww, hh;
   int         size;
   int         note_width, note_height;
   
   nf = data;
   evas_output_viewport_get(nf->evas, NULL, NULL, &ww, &hh);
   nf->ww = ww;
   o = nf->note_object;

   evas_object_move (o, 0, hh - nf->note->conf->height + 3);
   evas_object_resize (o,  nf->note->conf->width, nf->note->conf->height);
   
   return 1;
}
