#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Mem *_mem_init                   (E_Module *m);
static void _mem_shutdown               (Mem *n);
static void _mem_config_menu_new        (Mem *n);
static int  _mem_face_init              (Mem_Face *cf);
static void _mem_face_menu_new          (Mem_Face *cf);
static void _mem_face_enable            (Mem_Face *cf);
static void _mem_face_disable           (Mem_Face *cf);
static void _mem_face_free              (Mem_Face *cf);
static void _mem_face_cb_gmc_change     (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void _mem_face_cb_mouse_down     (void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _mem_face_cb_menu_edit      (void *data, E_Menu *mn, E_Menu_Item *mi);
static void _mem_face_cb_menu_configure (void *data, E_Menu *mn, E_Menu_Item *mi);
static int  _mem_face_update_values     (void *data);
static void _mem_face_get_mem_values    (Mem_Face *cf, int *real, int *swap, int *total_real, int *total_swap);
static void _mem_face_graph_values      (Mem_Face *mf, int rval, int sval);
static void _mem_face_graph_clear       (Mem_Face *mf);
static int mem_count;

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Mem"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Mem *c;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   c = _mem_init(m);
   if (!c)
     return NULL;
   
   m->config_menu = c->config_menu;
   return c;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Mem *c;
   
   c = m->data;
   if (!c)
     return 0;
   
   if (m->config_menu) 
     {
	e_menu_deactivate(m->config_menu);
	e_object_del(E_OBJECT(m->config_menu));
	m->config_menu = NULL;
     }
   if (c->cfd) 
     {
	e_object_del(E_OBJECT(c->cfd));
	c->cfd = NULL;
     }
   _mem_shutdown(c);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Mem *c;
   
   c = m->data;
   if (!c)
     return 0;
   e_config_domain_save("module.mem", c->conf_edd, c->conf);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m) 
{
   m->icon_file = strdup(PACKAGE_DATA_DIR"/module_icon.png");
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(D_("Enlightenment Mem Monitor Module"),
			D_("This module is used to monitor memory."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Mem *c;
   E_Container *con;
   
   c = m->data;
   if (!c)
     return 0;
   if (!c->face)
     return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (c->face->con == con)
     _configure_mem_module(con, c);
   
   return 1;
}

static Mem *
_mem_init(E_Module *m) 
{
   Mem *c;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2;
   
   c = E_NEW(Mem, 1);
   if (!c)
     return NULL;
   
   c->conf_edd = E_CONFIG_DD_NEW("Mem_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D c->conf_edd
   E_CONFIG_VAL(D, T, check_interval, INT);
   E_CONFIG_VAL(D, T, show_text, INT);
   E_CONFIG_VAL(D, T, show_percent, INT);   
   E_CONFIG_VAL(D, T, show_graph, INT);   
   E_CONFIG_VAL(D, T, real_ignore_buffers, UCHAR);
   E_CONFIG_VAL(D, T, real_ignore_cached, UCHAR);
   
   c->conf = e_config_domain_load("module.mem", c->conf_edd);
   if (!c->conf) 
     {
	c->conf = E_NEW(Config, 1);
	c->conf->check_interval = 1;
	c->conf->show_text = 1;
	c->conf->show_graph = 1;
	c->conf->show_percent = 1;
	c->conf->real_ignore_buffers = 0;
	c->conf->real_ignore_cached = 0;
     }
   E_CONFIG_LIMIT(c->conf->check_interval, 0, 60);
   E_CONFIG_LIMIT(c->conf->show_text, 0, 1);
   E_CONFIG_LIMIT(c->conf->show_graph, 0, 1);
   E_CONFIG_LIMIT(c->conf->show_percent, 0, 1);
   
   _mem_config_menu_new(c);
   
   mans = e_manager_list();
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Mem_Face *cf;
	     
	     con = l2->data;
	     cf = E_NEW(Mem_Face, 1);
	     if (cf) 
	       {
		  cf->conf_face_edd = E_CONFIG_DD_NEW("Mem_Face_Config", Config_Face);
		  #undef T
		  #undef D
		  #define T Config_Face
		  #define D cf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);
		  
		  c->face = cf;
		  cf->mem = c;		  
		  cf->con = con;
		  cf->evas = con->bg_evas;
		  
		  cf->conf = E_NEW(Config_Face, 1);
		  cf->conf->enabled = 1;
		  
		  if (!_mem_face_init(cf))
		    return NULL;
		  
		  _mem_face_menu_new(cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _mem_face_cb_menu_configure, cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, cf->menu);
		 
		  if (!cf->conf->enabled)
		    _mem_face_disable(cf);
		  else
		    _mem_face_enable(cf);
	       }
	  }
     }
   return c;
}

static void
_mem_shutdown(Mem *c) 
{
   _mem_face_free(c->face);
   
   E_FREE(c->conf);
   E_CONFIG_DD_FREE(c->conf_edd);
   E_FREE(c);
}

static void
_mem_config_menu_new(Mem *c) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   c->config_menu = mn;
}

static int
_mem_face_init(Mem_Face *cf) 
{
   Evas_Object *o;
   char buf[4096];
   
   evas_event_freeze(cf->evas);
   
   o = edje_object_add(cf->evas);
   cf->mem_obj = o;
   
   if (!e_theme_edje_object_set(o, "base/theme/modules/mem", "modules/mem/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mem.edj");	
	edje_object_file_set(o, buf, "modules/mem/main");
     }   
   evas_object_show(o);

   o = edje_object_add(cf->evas);
   cf->chart_obj = o;
   evas_object_layer_set(o, 1);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
   
   o = edje_object_add(cf->evas);
   cf->rtxt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/mem", "modules/mem/real_text")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mem.edj");	
	edje_object_file_set(o, buf, "modules/mem/real_text");
     }
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 0);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   o = edje_object_add(cf->evas);
   cf->stxt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/mem", "modules/mem/swap_text")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/mem.edj");	
	edje_object_file_set(o, buf, "modules/mem/swap_text");
     }
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 0);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);
      
   o = evas_object_rectangle_add(cf->evas);
   cf->event_obj = o;
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _mem_face_cb_mouse_down, cf);
   evas_object_show(o);
   
   cf->gmc = e_gadman_client_new(cf->con->gadman);
   e_gadman_client_domain_set(cf->gmc, "module.mem", mem_count++);
   e_gadman_client_policy_set(cf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_VSIZE);
   e_gadman_client_auto_size_set(cf->gmc, 40, 40);
   e_gadman_client_align_set(cf->gmc, 1.0, 1.0);
   e_gadman_client_resize(cf->gmc, 40, 40);
   e_gadman_client_change_func_set(cf->gmc, _mem_face_cb_gmc_change, cf);
   e_gadman_client_load(cf->gmc);
   evas_event_thaw(cf->evas);

   cf->monitor = ecore_timer_add((double)cf->mem->conf->check_interval, _mem_face_update_values, cf);
   return 1;
}

static void
_mem_face_menu_new(Mem_Face *cf) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   cf->menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");         
   e_menu_item_callback_set(mi, _mem_face_cb_menu_configure, cf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");         
   e_menu_item_callback_set(mi, _mem_face_cb_menu_edit, cf);
}

static void
_mem_face_enable(Mem_Face *cf) 
{
   cf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(cf->mem_obj);
   evas_object_show(cf->chart_obj);   
   evas_object_show(cf->event_obj);
   evas_object_show(cf->rtxt_obj);
   evas_object_show(cf->stxt_obj);   
}

static void
_mem_face_disable(Mem_Face *cf) 
{
   cf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(cf->event_obj);
   evas_object_hide(cf->chart_obj);   
   evas_object_hide(cf->mem_obj);
   evas_object_hide(cf->rtxt_obj);
   evas_object_hide(cf->stxt_obj);
}

static void 
_mem_face_free(Mem_Face *cf) 
{
   if (cf->monitor)
     ecore_timer_del(cf->monitor);
   if (cf->menu)
     e_object_del(E_OBJECT(cf->menu));
   if (cf->event_obj)
     evas_object_del(cf->event_obj);
   if (cf->mem_obj)
     evas_object_del(cf->mem_obj);
   if (cf->chart_obj)
     evas_object_del(cf->chart_obj);
   if (cf->rtxt_obj)
     evas_object_del(cf->rtxt_obj);
   if (cf->stxt_obj)
     evas_object_del(cf->stxt_obj);
   if (cf->old_real || cf->old_swap)
     _mem_face_graph_clear(cf);
   
   if (cf->gmc) 
     {
	e_gadman_client_save(cf->gmc);
	e_object_del(E_OBJECT(cf->gmc));
     }
   
   E_FREE(cf->conf);
   E_FREE(cf);
   mem_count--;
}

static void 
_mem_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Mem_Face *cf;
   Evas_Coord x, y, w, h;
   
   cf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(cf->gmc, &x, &y, &w, &h);
	evas_object_move(cf->mem_obj, x, y);
	evas_object_move(cf->chart_obj, x, y);
	evas_object_move(cf->event_obj, x, y);
	evas_object_move(cf->rtxt_obj, x, y);
	evas_object_move(cf->stxt_obj, x, y);	
	evas_object_resize(cf->mem_obj, w, h);
	evas_object_resize(cf->chart_obj, w, h);
	evas_object_resize(cf->event_obj, w, h);
	evas_object_resize(cf->rtxt_obj, w, h);
	evas_object_resize(cf->stxt_obj, w, h);
	_mem_face_graph_clear(cf);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(cf->mem_obj);
	evas_object_raise(cf->chart_obj);
	evas_object_raise(cf->event_obj);
	evas_object_raise(cf->rtxt_obj);
	evas_object_raise(cf->stxt_obj);	
	break;
      default:
	break;
     }   
}

static void 
_mem_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info) 
{
   Mem_Face *cf;
   Evas_Event_Mouse_Down *ev;
   
   ev = event_info;
   cf = data;
   if (ev->button == 3) 
     {
	e_menu_activate_mouse(cf->menu, e_zone_current_get(cf->con),
			      ev->output.x, ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_container_fake_mouse_up_all_later(cf->con);
     }
}

static void 
_mem_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Mem_Face *cf;
   
   cf = data;
   e_gadman_mode_set(cf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_mem_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Mem_Face *cf;

   cf = data;
   _configure_mem_module(cf->con, cf->mem);
}

static int 
_mem_face_update_values(void *data) 
{
   Mem_Face *cf;
   int real, swap, total_real, total_swap;
   char real_str[100];
   char swap_str[100];
   Edje_Message_Float msg;
   
   cf = data;
   _mem_face_get_mem_values(cf, &real, &swap, &total_real, &total_swap);

   if (cf->mem->conf->show_text) 
     {
	if (!cf->mem->conf->show_percent) 
	  {
	     snprintf(real_str, sizeof(real_str), "%d/%d MB", (real / 1024), (total_real / 1024));
	     snprintf(swap_str, sizeof(swap_str), "%d/%d MB", (swap / 1024), (total_swap / 1024));
	  }
	 else 
	  {
	     double tr;
	     tr = (((double)real / (double)total_real) * 100);   
	     snprintf(real_str, sizeof(real_str), "%1.2f%%", tr);
	     tr = (((double)swap / (double)total_swap) * 100);   
	     snprintf(swap_str, sizeof(swap_str), "%1.2f%%", tr);
	  }
	edje_object_part_text_set(cf->rtxt_obj, "real-text", real_str);
	edje_object_part_text_set(cf->stxt_obj, "swap-text", swap_str);
     }
   else
     {
	edje_object_part_text_set(cf->rtxt_obj, "real-text", "");
	edje_object_part_text_set(cf->stxt_obj, "swap-text", "");	
     }

   double tr = ((double)real / (double)total_real);
   msg.val = tr;
   edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 9, &msg);

   double ts = ((double)swap / (double)total_swap);   
   msg.val = ts;
   edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 10, &msg);

   if ((cf->mem->conf->show_graph) && 
       (edje_object_part_exists (cf->mem_obj,"lines")))
     _mem_face_graph_values(cf, (tr * 100), (ts * 100));
   else
     _mem_face_graph_clear(cf);
   
   return 1;
}

static void
_mem_face_get_mem_values(Mem_Face *cf, int *real, int *swap, int *total_real, int *total_swap) 
{
   FILE *pmeminfo = NULL;
   int cursor = 0;
   char *line, *field;
   unsigned char c;
   long int value = 0, mtotal = 0, stotal = 0, mfree = 0, sfree = 0;
   ldiv_t ldresult;
   long int liresult;
   Edje_Message_Float msg;
   
   /* open /proc/meminfo */
   if (!(pmeminfo = fopen("/proc/meminfo", "r")))
     {
        fprintf(stderr, "can't open /proc/meminfo");
        return;
     }

   /* parse /proc/meminfo */
   line = (char *)calloc(64, sizeof(char));
   while (fscanf(pmeminfo, "%c", &c) != EOF)
     {
        if (c != '\n')
	  line[cursor++] = c;
        else
          {
             field = (char *)malloc(strlen(line) * sizeof(char));
             sscanf(line, "%s %ld kB", field, &value);
             if (!strcmp(field, "MemTotal:")) 
	       {
		  mtotal = value;
		  msg.val = value;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 0, &msg);
	       }
             else if (!strcmp(field, "MemFree:"))
	       {
		  mfree = value;
		  msg.val = value;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 1, &msg);
	       } 
             else if (cf->mem->conf->real_ignore_buffers && (!strcmp(field, "Buffers:")))
	       {
		  mfree += value;
		  msg.val = value;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 2, &msg);
	       }
             else if (cf->mem->conf->real_ignore_cached && (!strcmp(field, "Cached:")))
	       {
		  mfree += value;
		  msg.val = value;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 3, &msg);
	       }
	     else if (cf->mem->conf->real_ignore_cached && (!strcmp(field, "SwapCached:")))
	       {
		  sfree += value;
		  msg.val = value;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 4, &msg);
	       }
	     else if (!strcmp(field, "SwapTotal:"))
	       {
		  stotal = value;
		  msg.val = stotal;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 5, &msg);
	       }
	     else if (!strcmp(field, "SwapFree:"))
	       {
		  sfree = value;
		  msg.val = sfree;
		  edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 6, &msg);
	       }
             free(line);
             free(field);
             cursor = 0;
             line = (char *)calloc(64, sizeof(char));
          }
     }
   fclose(pmeminfo);

   /* calculate swap usage in percent */
   if (stotal >= 1)
     {
        ldresult = ldiv(stotal, 100);
        liresult = ldresult.quot;
        ldresult = ldiv((stotal - sfree), liresult);
     }
   
   /* calculate memory usage in percent */
   /* FIXME : figure out a better way to do this */
   ldresult = ldiv(mtotal, 100);
   liresult = ldresult.quot;
   ldresult = ldiv((mtotal - mfree), liresult);
   
   *real = (mtotal - mfree);
   msg.val = *real;
   edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 7, &msg);
   
   *swap = (stotal - sfree);
   msg.val = *swap;
   edje_object_message_send(cf->mem_obj, EDJE_MESSAGE_FLOAT, 8, &msg);

   *total_real = mtotal;
   *total_swap = stotal;   
   return;
}

static void 
_mem_face_graph_values(Mem_Face *mf, int rval, int sval) 
{
   int x, y, w, h;
   Evas_Object *o;
   Evas_Object *last = NULL;
   Evas_List *l;
   int i, j = 0;

   evas_event_freeze(mf->evas);
   
   evas_object_geometry_get(mf->chart_obj, &x, &y, &w, &h);
   
   rval = (int)(((double)rval) * (((double)h) / ((double)100)));      
   sval = (int)(((double)sval) * (((double)h) / ((double)100)));      

   o = evas_object_line_add(mf->evas);
   edje_object_part_swallow(mf->chart_obj, "lines", o);
   evas_object_layer_set(o, 1);
   if (rval == 0)
     evas_object_hide(o);
   else 
     {
	evas_object_line_xy_set(o, (x + w), y, (x + w), (y + rval));
	evas_object_color_set(o, 213, 91, 91, 125);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
     }
   
   /* Add new value to list */
   mf->old_real = evas_list_prepend(mf->old_real, o);   
   l = mf->old_real;
   for (i = (x + w); l && (j -2) < w; l = l->next, j++) 
     {
	Evas_Coord oy;
	Evas_Object *lo;
	
	lo = (Evas_Object *)evas_list_data(l);
	evas_object_geometry_get(lo, NULL, &oy, NULL, NULL);
	evas_object_move(lo, i--, oy);
	last = lo;
     }
   
   if ((j - 2) >= w) 
     {
	mf->old_real = evas_list_remove(mf->old_real, last);
	edje_object_part_unswallow(mf->chart_obj, last);
	evas_object_del(last);
     }   

   o = evas_object_line_add(mf->evas);
   edje_object_part_swallow(mf->chart_obj, "lines", o);
   evas_object_layer_set(o, 1);
   if (sval == 0)
     evas_object_hide(o);
   else 
     {
	evas_object_line_xy_set(o, (x + w), (y + h), (x + w), (y + h - sval));
	evas_object_color_set(o, 218, 195, 35, 125);
	evas_object_pass_events_set(o, 1);
	evas_object_show(o);
     }
   
   mf->old_swap = evas_list_prepend(mf->old_swap, o);
   l = mf->old_swap;
   for (i = (x + w); l && (j -2) < w; l = l->next, j++) 
     {
	Evas_Coord oy;
	Evas_Object *lo;
	
	lo = (Evas_Object *)evas_list_data(l);
	evas_object_geometry_get(lo, NULL, &oy, NULL, NULL);
	evas_object_move(lo, i--, oy);
	last = lo;
     }
   
   if ((j - 2) >= w) 
     {
	mf->old_swap = evas_list_remove(mf->old_swap, last);
	edje_object_part_unswallow(mf->chart_obj, last);
	evas_object_del(last);
     }   
   
   evas_event_thaw(mf->evas);
}

static void 
_mem_face_graph_clear(Mem_Face *mf) 
{
   Evas_List *l;

   for (l = mf->old_real; l; l = l->next) 
     {
	Evas_Object *o;
	o = evas_list_data(l);
	evas_object_del(o);
     }
   evas_list_free(mf->old_real);
   mf->old_real = NULL;   

   for (l = mf->old_swap; l; l = l->next) 
     {
	Evas_Object *o;
	o = evas_list_data(l);
	evas_object_del(o);
     }
   evas_list_free(mf->old_swap);
   mf->old_swap = NULL;      
}

