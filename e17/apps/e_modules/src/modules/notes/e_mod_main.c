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
static void _note_menu_face_add (void *data, E_Menu *m, E_Menu_Item *mi);    
static void _note_face_trans_set(void *data, E_Menu *m, E_Menu_Item *mi);

static int  _note_face_init           (Note_Face *nf);
static void _note_face_free           (Note_Face *nf);
static void _note_face_menu_del       (void *data, E_Menu *m, E_Menu_Item *mi);
static int  _note_face_add            (Note *n);
static void _note_face_focus          (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _note_face_unfocus        (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _note_face_menu_new       (Note_Face *nf);  
static void _note_face_cb_mouse_down  (void *data, Evas *e, Evas_Object *obj, void *event_info);
    

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
   
   if(!_note_face_add(n))
     return NULL;
        
   return n;
}

int
_note_face_add(Note *n)
{       
   Evas_List *managers, *l, *l2;
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
		  n->faces = evas_list_append(n->faces, nf);
		  nf->note = n;
		  nf->con   = con;
		  nf->evas  = con->bg_evas;
		  if (!_note_face_init(nf))
		    return 0;
	       }
	  }
     }   
   return 1;
}

static void
_note_shutdown (Note *n)
{
   Evas_List *l;
   free(n->conf);
   E_CONFIG_DD_FREE(n->conf_edd);
   l = n->faces;
   while(l) {      
      _note_face_free(l->data);
      l = l->next;
   }
   free(n);
}

static E_Menu *
_note_config_menu_new (Note *n)
{
   E_Menu      *mn,*mnbg,*mnt;
   E_Menu_Item *mi;
   
   /* FIXME: hook callbacks to each menu item */
   mn = e_menu_new ();  // main configuration menu
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Add Note");
   e_menu_item_callback_set (mi, _note_menu_face_add, n);   
   
   return mn;
}

static void
_note_menu_bgcolor_green (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face;
   Evas_Object *bg;
      
   face = data;
   bg = evas_object_rectangle_add(face->evas);
   evas_object_color_set(bg, 187, 243, 168, 100);
   esmart_textarea_bg_set(face->note_object, bg);
}

static void
_note_menu_bgcolor_yellow (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face;
   Evas_Object *bg;
      
   face = data;
   bg = evas_object_rectangle_add(face->evas);
   evas_object_color_set(bg, 245, 248, 27, 100);
   esmart_textarea_bg_set(face->note_object, bg);
}

static void
_note_menu_bgcolor_white (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face;
   Evas_Object *bg;
      
   face = data;
   bg = evas_object_rectangle_add(face->evas);
   evas_object_color_set(bg, 255, 255, 255, 100);
   esmart_textarea_bg_set(face->note_object, bg);
}

static void
_note_menu_bgcolor_blue (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face;
   Evas_Object *bg;
      
   face = data;
   bg = evas_object_rectangle_add(face->evas);
   evas_object_color_set(bg, 149, 207, 226, 100);
   esmart_textarea_bg_set(face->note_object, bg);   
}

static void
_note_face_trans_set(void *data, E_Menu *m, E_Menu_Item *mi)
{    
   
   Note_Face *face = data;
   Evas_Object *bg;
   int r,g,b,a;   
   
   return; /* not working */
   
   bg = esmart_textarea_bg_get(face->note_object);
   evas_object_color_get(bg, &r,&g,&b,&a);
   switch(face->conf->trans)
     {
      case TRANS_0:
	a = 255;
	break;
      case TRANS_25:
	a = 191;
	break;
      case TRANS_50:
	a = 127;
	break;
      case TRANS_75:
	a = 64;
	break;
      case TRANS_100:
	a = 0;
	break;
     }   
   evas_object_color_set(bg,r,g,b,a);
}

static void
_note_menu_face_add (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note *n;

   n = (Note *)data;
   _note_face_add(n);
}

static void
_note_config_menu_del (Note *n, E_Menu *m)
{
   e_object_del (E_OBJECT(m));
}

static void
_note_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj,
			   void *event_info)
{			 
   Evas_Event_Mouse_Down *ev = event_info;
   Note_Face *nf = data;
   
   if (ev->button == 3)
     {        
	e_menu_activate_mouse(nf->menu, e_zone_current_get(nf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN);
	e_util_container_fake_mouse_up_all_later(nf->con);
     }   
}

static void
_note_face_focus(void *data, Evas *e, Evas_Object *obj,
		 void *event_info)
{
   Evas_Event_Mouse_In *ev = event_info;
   Note_Face *nf = data;
   esmart_textarea_focus_set(nf->note_object, 1);
}

static void
_note_face_unfocus(void *data, Evas *e, Evas_Object *obj,
		 void *event_info)
{
   Evas_Event_Mouse_In *ev = event_info;
   Note_Face *nf = data;
   esmart_textarea_focus_set(nf->note_object, 0);
}


static void
_note_face_menu_new(Note_Face *face)
{    
   E_Menu *mn, *mnbg, *mnt;
   E_Menu_Item *mi;

   mn = e_menu_new();
   mnbg = e_menu_new (); // options submenu
   mnt = e_menu_new (); // trans submenu
   face->menu = mn;
   face->menu_bg = mnbg;
   face->menu_trans = mnt;

   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "White Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_WHITE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_white, face);
   
   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "Yellow Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_YELLOW) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_yellow, face);
   
   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "Blue Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_BLUE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_blue, face);
   
   mi = e_menu_item_new(mnbg);
   e_menu_item_label_set(mi, "Green Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_GREEN) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_blue, face);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Backgrounds");
   e_menu_item_submenu_set(mi, face->menu_bg);         
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "0%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_0) e_menu_item_toggle_set (mi, 1);
   face->conf->trans = TRANS_0;
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "25%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == TRANS_25) e_menu_item_toggle_set (mi, 1);
   face->conf->trans = TRANS_25;   
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "50%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == TRANS_50) e_menu_item_toggle_set (mi, 1);
   face->conf->trans = TRANS_50;   
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "75%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == TRANS_75) e_menu_item_toggle_set (mi, 1);
   face->conf->trans = TRANS_75;   
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "100%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == TRANS_100) e_menu_item_toggle_set (mi, 1);
   face->conf->trans = TRANS_100;   
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Transparency (doesnt work)");
   e_menu_item_submenu_set(mi, face->menu_trans);   
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Delete Note");
   e_menu_item_callback_set(mi, _note_face_menu_del, face);   
}

static int
_note_face_init (Note_Face *face)
{
   Evas_Object *o;
   Evas_Coord   ww, hh;
   int         size;
   int         note_width, note_height;
   
   /* set up the note object */
   o = esmart_textarea_add (face->evas);
   face->note_object = o;      
   evas_output_viewport_get(face->evas, NULL, NULL, &ww, &hh);
   face->ww = ww;
   evas_object_move (o, 0, hh - face->note->conf->height + 3);
   //evas_object_resize (o, face->ww, face->note->conf->height);
   evas_object_resize(o, face->note->conf->width, face->note->conf->height);
   evas_object_pass_events_set(o, 1);
   evas_object_layer_set (o, 1);
   esmart_textarea_focus_set(o, 0);
   evas_object_show (o);
   
   o = evas_object_rectangle_add(face->evas);
   evas_object_color_set(o, 245, 248, 27, 100);
   esmart_textarea_bg_set(face->note_object, o);   
   
   o = evas_object_rectangle_add(face->evas);
   face->event_object = o;
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _note_face_cb_mouse_down, face);   
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN, 
				  _note_face_focus, face);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, 
				  _note_face_unfocus, face);
   evas_object_show(o);
      
   evas_event_freeze(face->con->bg_evas);
   
   
   face->gmc =  e_gadman_client_new(face->con->gadman);
   e_gadman_client_domain_set(face->gmc, "module.note", _note_count++);
   e_gadman_client_policy_set(face->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VSIZE);
   e_gadman_client_min_size_set(face->gmc, 4, 4);
   e_gadman_client_max_size_set(face->gmc, 512, 512);
   e_gadman_client_auto_size_set(face->gmc, 320, 240);
   e_gadman_client_align_set(face->gmc, 0.0, 1.0);
   e_gadman_client_resize(face->gmc, 40, 40);
   e_gadman_client_change_func_set(face->gmc, _note_face_cb_gmc_change, face);
   e_gadman_client_load(face->gmc);
   
   evas_event_thaw(face->con->bg_evas);
   
   face->conf_edd = E_CONFIG_DD_NEW("Note_Face_Config", Config);

   
#undef T
#undef D
#define T Config
#define D face->conf_edd
   E_CONFIG_VAL(D, T, height, INT);
   E_CONFIG_VAL(D, T, width, INT);
   E_CONFIG_VAL(D, T, bgcolor, INT);
   E_CONFIG_VAL(D, T, trans, INT);
   
   if (!face->conf)
     {
	face->conf = E_NEW (Config, 1);
	face->conf->height = 320;
	face->conf->width = 200;
	face->conf->bgcolor = BGCOLOR_YELLOW;
	face->conf->trans = TRANS_50;
     }
   E_CONFIG_LIMIT(face->conf->height, 48, 800);
   E_CONFIG_LIMIT(face->conf->width, 48, 800);
   E_CONFIG_LIMIT(face->conf->bgcolor, 0, 10);
   E_CONFIG_LIMIT(face->conf->trans, 0, 5);
   
   _note_face_menu_new(face);   
   
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
	
	evas_object_move(nf->event_object, x, y);
	evas_object_resize(nf->event_object, w, h);
	break;
      case E_GADMAN_CHANGE_RAISE:	                         
	evas_object_raise(nf->note_object);
	evas_object_raise(nf->event_object);	
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
_note_face_menu_del(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face = data;
   _note_face_free(face);     
}

static void
_note_face_free(Note_Face *nf)
{
   e_gadman_client_save(nf->gmc);
   e_object_del(E_OBJECT(nf->gmc));
   
   evas_object_del (nf->note_object);
   evas_object_del (nf->event_object);   
   _note_count--;
   free (nf);
}

/* this is NOT used */
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
   
   o = nf->event_object;   
   evas_object_move (o, 0, hh - nf->note->conf->height + 3);
   evas_object_resize (o,  nf->note->conf->width, nf->note->conf->height);      
   return 1;
}
