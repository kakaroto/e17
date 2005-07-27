#include "e.h"
#include "config.h"
#include "e_mod_main.h"

#include <time.h>
#include <Esmart/Esmart_Textarea.h>

/* module private routines */
static Note    *_note_init            (E_Module *m);
static void    _note_shutdown         (Note *n);
static E_Menu *_note_config_menu_new  (Note *n);
static void    _note_config_menu_del  (Note *n, E_Menu *m);

static void _note_menu_bgcolor_set    (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_menu_face_add       (void *data, E_Menu *m, E_Menu_Item *mi);    

static int  _note_face_init           (Note_Face *nf);
static void _note_face_free           (Note_Face *nf);
static int  _note_face_add            (Note *n, Note_Face *f);
static void _note_face_focus          (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _note_face_unfocus        (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _note_face_bgcolor_set    (Note_Face *face, int color);
static void _note_face_trans_set      (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_face_font_change    (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_face_menu_new       (Note_Face *nf);  
static void _note_face_menu_del       (void *data, E_Menu *m, E_Menu_Item *mi);
static void _note_face_cb_mouse_down  (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _note_face_cb_gmc_change  (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);


char          *_note_module_dir;
static int     _note_count;
static E_Config_DD *_notes_config_faces_edd = NULL;

/* public module routines. all modules must have these */
void *
e_modapi_init (E_Module *m)
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
e_modapi_shutdown (E_Module *m)
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
e_modapi_save (E_Module *m)
{
   Note *n;
   Evas_List *l, *l2;
   
   n = m->data;
        
   l = n->conf->faces;
   l2 = n->faces;
   
   while(l&&l2) {
      int pos;
      Note_Face_Config *c = l->data;
      Note_Face *face = l2->data;
      pos = esmart_textarea_cursor_pos_get(face->note_object);
      esmart_textarea_cursor_pos_set(face->note_object, 0);
      c->text = esmart_textarea_text_get(face->note_object,
						  esmart_textarea_length_get(face->note_object)
						  );
      l = l->next;
      l2 = l2->next;
   }
      
   e_config_domain_save("module.note", n->conf_edd, n->conf);   
   return 1;
}

int
e_modapi_info (E_Module *m)
{
   m->label = strdup("Notes");
   m->icon_file = strdup(PACKAGE_LIB_DIR "/e_modules/notes/module_icon.png");
   
   return 1;
}

int
e_modapi_about (E_Module *m)
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
   
   n->conf_edd = E_CONFIG_DD_NEW("Note_Config", Note_Config);
   _notes_config_faces_edd = E_CONFIG_DD_NEW("Note_Face_Config",Note_Face_Config);
#undef T
#undef D
#define T Note_Face_Config
#define D _notes_config_faces_edd
   E_CONFIG_VAL(D, T, height, INT);
   E_CONFIG_VAL(D, T, width, INT);
   E_CONFIG_VAL(D, T, bgcolor, INT);
   E_CONFIG_VAL(D, T, trans, INT);
   E_CONFIG_VAL(D, T, text, STR);
   E_CONFIG_VAL(D, T, fsize, INT);
   
#undef T
#undef D
#define T Note_Config
#define D n->conf_edd
   E_CONFIG_VAL(D, T, height, INT);
   E_CONFIG_VAL(D, T, width, INT);
   E_CONFIG_VAL(D, T, bgcolor, INT);
   E_CONFIG_LIST(D, T, faces, _notes_config_faces_edd);

   n->conf = e_config_domain_load("module.note", n->conf_edd);
   if (!n->conf)
     {
	/* no saved faced */
	n->conf = E_NEW (Note_Config, 1);
	n->conf->height = 320;
	n->conf->width = 200;
	n->conf->bgcolor = 2;
	n->conf->faces = NULL;
     } 
   else 
     {
	/* relaunch saved faces */	
	Evas_List *l = n->conf->faces;
	while(l)
	  {
	     Note_Face_Config *c = l->data;
	     Note_Face *face = E_NEW(Note_Face, 1);
	     face->conf = c;
	     _note_face_add(n, face);
	     l = l->next;
	  }
     }
   /* set some limits on config values */
   E_CONFIG_LIMIT(n->conf->height, 48, 800);
   E_CONFIG_LIMIT(n->conf->width, 48, 800);
   E_CONFIG_LIMIT(n->conf->bgcolor, 0, 10);
           
   return n;
}

int
_note_face_add(Note *n, Note_Face *f)
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
	     Note_Face  *face;
	     
	     con = l2->data;	     
	     if(!f)
	       face = E_NEW(Note_Face, 1);
	     else face = f;
	     if (face)
	       {
		  n->faces = evas_list_append(n->faces, face);
		  face->note = n;
		  face->con   = con;
		  face->evas  = con->bg_evas;
		  if (!_note_face_init(face))
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
   
   /* main configuration menu */
   mn = e_menu_new ();  
   /* "Add Note" button */
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Add Note");
   e_menu_item_callback_set (mi, _note_menu_face_add, n);      
   return mn;
}

static void
_note_menu_bgcolor_set (void *data, E_Menu *m, E_Menu_Item *mi)
{
   Note_Face *face;   
   face = data;
   _note_face_bgcolor_set(face, e_menu_item_num_get(mi));
}

static void
_note_face_bgcolor_set(Note_Face *face, int color)
{    
   Evas_Object *bg;
   
   bg = evas_object_rectangle_add(face->evas);   
   switch(color)
     {
      case 0:
	evas_object_color_set(bg, 255, 255, 255, 100);
	break;
      case 1:
	evas_object_color_set(bg, 149, 207, 226, 100);	
	break;
      case 2:
	evas_object_color_set(bg, 187, 243, 100, 100);	
	break;
      case 3:
	evas_object_color_set(bg, 245, 248, 27, 100);	
	break;
     }
   esmart_textarea_bg_set(face->note_object, bg);
   face->conf->bgcolor = color;
}

static void
_note_face_font_change(void *data, E_Menu *m, E_Menu_Item *mi)
{       
   Note_Face *face = data;
   Evas_Object *bg;
   Esmart_Text_Area_Format *format;
   char f[10];
   int pos;
   
   pos = esmart_textarea_cursor_pos_get(face->note_object);
   esmart_textarea_cursor_pos_set(face->note_object, 0);
   format = esmart_textarea_format_get(face->note_object);
        
   switch(e_menu_item_num_get(mi))
     {
      case 0:
	sprintf(f, "font=%s size=%d",format->font, atoi(format->size)+1);
	esmart_textarea_format_insert(face->note_object, f);
	face->conf->fsize = atoi(format->size)+1;
	break;
      case 1:
	sprintf(f, "font=%s size=%d",format->font, atoi(format->size)-1);
	esmart_textarea_format_insert(face->note_object, f);
	face->conf->fsize = atoi(format->size)-1;	
	break;
     }   
   esmart_textarea_cursor_pos_set(face->note_object, pos);
}

static void
_note_face_trans_set(void *data, E_Menu *m, E_Menu_Item *mi)
{    
   
   Note_Face *face = data;
   Evas_Object *bg;
   int r,g,b,a;   
      
   bg = esmart_textarea_bg_get(face->note_object);
   evas_object_color_get(bg, &r,&g,&b,&a);
   switch(e_menu_item_num_get(mi))
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
   _note_face_add(n, NULL);
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
			      E_MENU_POP_DIRECTION_DOWN,ev->timestamp);
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
   E_Menu *mn, *mnbg, *mnt, *mnf;
   E_Menu_Item *mi;

   mn = e_menu_new();    // main menu
   mnbg = e_menu_new (); // bg submenu
   mnt = e_menu_new ();  // trans submenu
   mnf = e_menu_new ();  // fonts submenu
   face->menu = mn;
   face->menu_bg = mnbg;
   face->menu_trans = mnt;
   face->menu_font = mnf;

   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "White Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_WHITE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_set, face);
      
   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "Blue Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_BLUE) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_set, face);
   
   mi = e_menu_item_new(mnbg);
   e_menu_item_label_set(mi, "Green Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_GREEN) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_set, face);
   
   mi = e_menu_item_new (mnbg);
   e_menu_item_label_set (mi, "Yellow Bg");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->bgcolor == BGCOLOR_YELLOW) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_menu_bgcolor_set, face);   
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Backgrounds");
   e_menu_item_submenu_set(mi, face->menu_bg);         
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "0%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_0) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "25%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_25) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "50%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_50) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "75%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_75) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new (mnt);
   e_menu_item_label_set (mi, "100%");
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (face->conf->trans == TRANS_100) e_menu_item_toggle_set (mi, 1);
   e_menu_item_callback_set (mi, _note_face_trans_set, face);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Transparency");
   e_menu_item_submenu_set(mi, face->menu_trans);   
   
   mi = e_menu_item_new (mnf);
   e_menu_item_label_set (mi, "Larger");
   e_menu_item_callback_set (mi, _note_face_font_change, face);
   
   mi = e_menu_item_new (mnf);
   e_menu_item_label_set (mi, "Smaller");
   e_menu_item_callback_set (mi, _note_face_font_change, face);   
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Font");
   e_menu_item_submenu_set(mi, face->menu_font);
   
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, "Delete Note");
   e_menu_item_callback_set(mi, _note_face_menu_del, face);   
}

static int
_note_face_init (Note_Face *face)
{
   Evas_Object *o;
   Evas_Coord   ww, hh;
   char       *face_dom;
   int         size;
   int         note_width, note_height;
   Note_Face_Config *c;
   
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
   e_gadman_client_domain_set(face->gmc, "module.note.face", _note_count);
   
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
   e_gadman_client_resize(face->gmc, 256, 256);
   e_gadman_client_change_func_set(face->gmc, _note_face_cb_gmc_change, face);
   e_gadman_client_load(face->gmc);
   
   evas_event_thaw(face->con->bg_evas);
      
   if (!face->conf)
     {	
	/* new face, config with defaults */
	c = E_NEW (Note_Face_Config, 1);
	c->height = 320;
	c->width = 200;
	c->bgcolor = BGCOLOR_YELLOW;
	c->trans = TRANS_50;
	c->fsize = 0;
	face->conf = c;
	face->note->conf->faces = evas_list_append(face->note->conf->faces,c);
     }
   else 
     {
	/* loaded face, add its values */
	
	esmart_textarea_text_insert(face->note_object, face->conf->text);
	
     }
   
   _note_face_bgcolor_set(face, face->conf->bgcolor);
   _note_face_menu_new(face);   
   _note_count++;
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
   Evas_List *l;

   l = face->note->faces;
   while(l) {
      Note_Face *f = l->data;
      if(f == face) {
	 f->note->faces = evas_list_remove(f->note->faces, l->data);
	 face->note->conf->faces = evas_list_remove(face->note->conf->faces, face->conf);
	 e_config_save_queue();
	 e_config_domain_save("module.note", f->note->conf_edd, f->note->conf);
	 
	 /* this is a fucked up attempt at defaulting gadman settings, change */	 
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
	 e_gadman_client_resize(face->gmc, 256, 256);
	 e_gadman_client_change_func_set(face->gmc, _note_face_cb_gmc_change, face);
	 e_gadman_client_save(face->gmc);
	 
	 break;
      }
      l = l->next;
   }      
   _note_face_free(face);
}

static void
_note_face_free(Note_Face *face)
{   
   e_object_del(E_OBJECT(face->gmc));   
   evas_object_del (face->note_object);
   evas_object_del (face->event_object);   
   _note_count--;
   free (face);
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
