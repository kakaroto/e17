#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

static Cpu *_cpu_init                   (E_Module *m);
static void _cpu_shutdown               (Cpu *n);
static void _cpu_config_menu_new        (Cpu *n);
static int  _cpu_face_init              (Cpu_Face *cf);
static void _cpu_face_menu_new          (Cpu_Face *cf);
static void _cpu_face_enable            (Cpu_Face *cf);
static void _cpu_face_disable           (Cpu_Face *cf);
static void _cpu_face_free              (Cpu_Face *cf);
static void _cpu_face_cb_gmc_change     (void *data, E_Gadman_Client *gmc, E_Gadman_Change change);
static void _cpu_face_cb_mouse_down     (void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _cpu_face_cb_menu_edit      (void *data, E_Menu *mn, E_Menu_Item *mi);
static void _cpu_face_cb_menu_configure (void *data, E_Menu *mn, E_Menu_Item *mi);
static int  _cpu_face_update_values     (void *data);
static int  _cpu_face_get_cpu_count     (Cpu_Face *cf); 
static void _cpu_face_get_load          (Cpu_Face *cf); 
static void _cpu_face_graph_values      (Cpu_Face *cf);
static void _cpu_face_graph_clear       (Cpu_Face *cf);

static int cpu_count;
static int cpu_stats[4];

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Cpu"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   Cpu *c;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   c = _cpu_init(m);
   if (!c)
     return NULL;
   
   m->config_menu = c->config_menu;
   return c;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   Cpu *c;
   
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
   _cpu_shutdown(c);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Cpu *c;
   
   c = m->data;
   if (!c)
     return 0;
   e_config_domain_save("module.cpu", c->conf_edd, c->conf);
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
   e_module_dialog_show(D_("Enlightenment Cpu Monitor Module"),
			D_("This module is used to monitor a cpu."));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m) 
{
   Cpu *c;
   E_Container *con;
   
   c = m->data;
   if (!c)
     return 0;
   if (!c->face)
     return 0;
   
   con = e_container_current_get(e_manager_current_get());
   if (c->face->con == con)
     _configure_cpu_module(con, c);
   
   return 1;
}

static Cpu *
_cpu_init(E_Module *m) 
{
   Cpu *c;
   E_Menu_Item *mi;
   Evas_List *mans, *l, *l2;
   
   c = E_NEW(Cpu, 1);
   if (!c)
     return NULL;
   
   c->conf_edd = E_CONFIG_DD_NEW("Cpu_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D c->conf_edd
   E_CONFIG_VAL(D, T, check_interval, INT);
   E_CONFIG_VAL(D, T, show_text, INT);
   E_CONFIG_VAL(D, T, show_graph, INT);
   
   c->conf = e_config_domain_load("module.cpu", c->conf_edd);
   if (!c->conf) 
     {
	c->conf = E_NEW(Config, 1);
	c->conf->check_interval = 1;
	c->conf->show_text = 1;
	c->conf->show_graph = 1;
     }
   E_CONFIG_LIMIT(c->conf->check_interval, 0, 60);
   E_CONFIG_LIMIT(c->conf->show_text, 0, 1);
   E_CONFIG_LIMIT(c->conf->show_graph, 0, 1);
   
   _cpu_config_menu_new(c);
   
   mans = e_manager_list();
   for (l = mans; l; l = l->next) 
     {
	E_Manager *man;
	
	man = l->data;
	for (l2 = man->containers; l2; l2 = l2->next) 
	  {
	     E_Container *con;
	     Cpu_Face *cf;
	     
	     con = l2->data;
	     cf = E_NEW(Cpu_Face, 1);
	     if (cf) 
	       {
		  cf->conf_face_edd = E_CONFIG_DD_NEW("Cpu_Face_Config", Config_Face);
		  #undef T
		  #undef D
		  #define T Config_Face
		  #define D cf->conf_face_edd
		  E_CONFIG_VAL(D, T, enabled, UCHAR);
		  
		  c->face = cf;
		  cf->cpu = c;		  
		  cf->con = con;
		  cf->evas = con->bg_evas;
		  
		  cf->conf = E_NEW(Config_Face, 1);
		  cf->conf->enabled = 1;
		  
		  if (!_cpu_face_init(cf))
		    return NULL;
		  
		  _cpu_face_menu_new(cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, _("Configuration"));
		  e_menu_item_callback_set(mi, _cpu_face_cb_menu_configure, cf);
		  
		  mi = e_menu_item_new(c->config_menu);
		  e_menu_item_label_set(mi, con->name);
		  e_menu_item_submenu_set(mi, cf->menu);
		 
		  if (!cf->conf->enabled)
		    _cpu_face_disable(cf);
		  else
		    _cpu_face_enable(cf);
	       }
	  }
     }
   return c;
}

static void
_cpu_shutdown(Cpu *c) 
{
   _cpu_face_free(c->face);
   
   E_FREE(c->conf);
   E_CONFIG_DD_FREE(c->conf_edd);
   E_FREE(c);
}

static void
_cpu_config_menu_new(Cpu *c) 
{
   E_Menu *mn;
   
   mn = e_menu_new();
   c->config_menu = mn;
}

static int
_cpu_face_init(Cpu_Face *cf) 
{
   Evas_Object *o;
   char buf[4096];
   
   evas_event_freeze(cf->evas);
   
   o = edje_object_add(cf->evas);
   cf->cpu_obj = o;   
   if (!e_theme_edje_object_set(o, "base/theme/modules/cpu", "modules/cpu/main")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/cpu.edj");	
	edje_object_file_set(o, buf, "modules/cpu/main");
     }   
   evas_object_pass_events_set(o, 1);
   evas_object_show(o);

   o = edje_object_add(cf->evas);
   cf->chart_obj = o;
   evas_object_layer_set(o, 1);
   evas_object_repeat_events_set(o, 0);
   evas_object_pass_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);

   o = edje_object_add(cf->evas);
   cf->txt_obj = o;
   if (!e_theme_edje_object_set(o, "base/theme/modules/cpu", "modules/cpu/text")) 
     {
	snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/cpu.edj");	
	edje_object_file_set(o, buf, "modules/cpu/text");
     }      
   evas_object_layer_set(o, 2);
   evas_object_repeat_events_set(o, 0);
   evas_object_pass_events_set(o, 1);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);
   
   o = evas_object_rectangle_add(cf->evas);
   cf->event_obj = o;
   evas_object_layer_set(o, 3);
   evas_object_repeat_events_set(o, 1);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _cpu_face_cb_mouse_down, cf);
   evas_object_show(o);
   
   cf->gmc = e_gadman_client_new(cf->con->gadman);
   e_gadman_client_domain_set(cf->gmc, "module.cpu", cpu_count++);
   e_gadman_client_policy_set(cf->gmc,
			      E_GADMAN_POLICY_ANYWHERE |
			      E_GADMAN_POLICY_HMOVE |
			      E_GADMAN_POLICY_HSIZE |
			      E_GADMAN_POLICY_VMOVE |
			      E_GADMAN_POLICY_VSIZE);
   e_gadman_client_auto_size_set(cf->gmc, 45, 50);
   e_gadman_client_align_set(cf->gmc, 1.0, 1.0);
   e_gadman_client_resize(cf->gmc, 45, 50);
   e_gadman_client_change_func_set(cf->gmc, _cpu_face_cb_gmc_change, cf);
   e_gadman_client_load(cf->gmc);
   
   evas_event_thaw(cf->evas);

   cf->monitor = ecore_timer_add((double)cf->cpu->conf->check_interval, _cpu_face_update_values, cf);
   return 1;
}

static void
_cpu_face_menu_new(Cpu_Face *cf) 
{
   E_Menu *mn;
   E_Menu_Item *mi;
   
   mn = e_menu_new();
   cf->menu = mn;
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");      
   e_menu_item_callback_set(mi, _cpu_face_cb_menu_configure, cf);
   
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/gadgets");      
   e_menu_item_callback_set(mi, _cpu_face_cb_menu_edit, cf);
}

static void
_cpu_face_enable(Cpu_Face *cf) 
{
   cf->conf->enabled = 1;
   e_config_save_queue();
   evas_object_show(cf->cpu_obj);
   evas_object_show(cf->event_obj);
   if (cf->cpu->conf->show_graph)
     evas_object_show(cf->chart_obj);
   if (cf->cpu->conf->show_text)
     evas_object_show(cf->txt_obj);
}

static void
_cpu_face_disable(Cpu_Face *cf) 
{
   cf->conf->enabled = 0;
   e_config_save_queue();
   evas_object_hide(cf->event_obj);
   evas_object_hide(cf->chart_obj);
   evas_object_hide(cf->cpu_obj);
   evas_object_hide(cf->txt_obj);   
}

static void 
_cpu_face_free(Cpu_Face *cf) 
{
   if (cf->monitor)
     ecore_timer_del(cf->monitor);
   if (cf->menu)
     e_object_del(E_OBJECT(cf->menu));
   if (cf->event_obj)
     evas_object_del(cf->event_obj);
   if (cf->cpu_obj)
     evas_object_del(cf->cpu_obj);
   if (cf->old_values)
     _cpu_face_graph_clear(cf);
   if (cf->chart_obj)
     evas_object_del(cf->chart_obj);
   if (cf->txt_obj)
     evas_object_del(cf->txt_obj);
   
   if (cf->gmc) 
     {
	e_gadman_client_save(cf->gmc);
	e_object_del(E_OBJECT(cf->gmc));
     }
   
   E_FREE(cf->conf);
   E_FREE(cf);
   cpu_count--;
}

static void 
_cpu_face_cb_gmc_change(void *data, E_Gadman_Client *gmc, E_Gadman_Change change) 
{
   Cpu_Face *cf;
   Evas_Coord x, y, w, h;
   
   cf = data;
   switch (change) 
     {
      case E_GADMAN_CHANGE_MOVE_RESIZE:
	e_gadman_client_geometry_get(cf->gmc, &x, &y, &w, &h);
	evas_object_move(cf->chart_obj, x, y);
	evas_object_move(cf->event_obj, x, y);
	evas_object_move(cf->cpu_obj, x, y);
	evas_object_move(cf->txt_obj, x, y);	
	evas_object_resize(cf->chart_obj, w, h);
	evas_object_resize(cf->event_obj, w, h);
	evas_object_resize(cf->cpu_obj, w, h);
	evas_object_resize(cf->txt_obj, w, h);	
	_cpu_face_graph_clear(cf);
	break;
      case E_GADMAN_CHANGE_RAISE:
	evas_object_raise(cf->cpu_obj);
	evas_object_raise(cf->chart_obj);
	evas_object_raise(cf->event_obj);
	evas_object_raise(cf->txt_obj);	
	break;
      default:
	break;
     }   
}

static void 
_cpu_face_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info) 
{
   Cpu_Face *cf;
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
_cpu_face_cb_menu_edit(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Cpu_Face *cf;
   
   cf = data;
   e_gadman_mode_set(cf->gmc->gadman, E_GADMAN_MODE_EDIT);
}

static void 
_cpu_face_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   Cpu_Face *cf;

   cf = data;
   _configure_cpu_module(cf->con, cf->cpu);
}

static int 
_cpu_face_update_values(void *data) 
{
   Cpu_Face *cf;
   char str[100];
   //Edje_Message_Float msg;
   int i = 0;
   char str_tmp[100];

   cf = data;
   _cpu_face_get_load(cf);
   
   if (cpu_stats[0] == -1)
     return 1;

   if (cf->cpu->conf->show_text) 
     {
	snprintf(str, sizeof(str), "%d%%", cpu_stats[0]);
	i = 1;
	while (i < cpu_count)
	  {
	     snprintf(str_tmp, sizeof(str_tmp), " / %d%%", cpu_stats[i]);
	     strncat(str, str_tmp, sizeof(str));
	     i++;
	  }
	edje_object_part_text_set(cf->txt_obj, "in-text", str);   
     }
   else
     edje_object_part_text_set(cf->txt_obj, "in-text", "");   
     
   if ((cf->cpu->conf->show_graph) && 
       (edje_object_part_exists (cf->cpu_obj,"lines")))
     _cpu_face_graph_values(cf);
   else 
     _cpu_face_graph_clear(cf);

   return 1;
}

static int
_cpu_face_get_cpu_count(Cpu_Face *cf) 
{
   char tmp[4];
   FILE *f;
   int cpu = -1;
   
   if (!(f = fopen("/proc/stat", "r")))
     return -1;

   while (fscanf(f, "cp%s %*u %*u %*u %*u %*u %*u %*u %*u\n", (char *)&tmp) == 1)
     cpu++;
   
   fclose(f);
   return cpu;   
}

static void 
_cpu_face_get_load(Cpu_Face *cf) 
{
   static unsigned long old_u[4], old_n[4], old_s[4], old_i[4], old_wa[4], old_hi[4], old_si[4];
   unsigned long new_u, new_n, new_s, new_i, new_wa = 0, new_hi = 0, new_si = 0, ticks_past;
   int tmp_u, tmp_n, tmp_s, tmp_i;
   char dummy[16];
   FILE *stat;
   int cpu_count;
   //Edje_Message_Float msg;
   
   cpu_count = _cpu_face_get_cpu_count(cf);
   if (cpu_count == -1)
     return;

   if (!(stat = fopen("/proc/stat", "r")))
     return;

   int i = 0;

   while (i < cpu_count)
   {
   
      if (fscanf(stat, "%s %lu %lu %lu %lu %lu %lu %lu", dummy,
              &new_u, &new_n, &new_s, &new_i, &new_wa, &new_hi, &new_si) < 5)
        {
           fclose(stat);
           return;
  	}

      ticks_past = ((new_u + new_n + new_s + new_i + new_wa + new_hi + new_si) -
                 (old_u[i] + old_n[i] + old_s[i] + old_i[i] + old_wa[i] + old_hi[i] + old_si[i]));

      if (ticks_past)
        {
           tmp_u = ((new_u - old_u[i]));
           tmp_n = ((new_n - old_n[i]));
           tmp_s = ((new_s - old_s[i]));
           tmp_i = ((new_i - old_i[i]));
        }
      else
        {
           tmp_u = 0;
           tmp_n = 0;
           tmp_s = 0;
           tmp_i = 0;
        }

      /* Update the values */   
      cpu_stats[i] = (tmp_u + tmp_n + tmp_s) / cpu_count;
   
      old_u[i] = new_u;
      old_n[i] = new_n;
      old_s[i] = new_s;
      old_i[i] = new_i;
      old_wa[i] = new_wa;
      old_hi[i] = new_hi;
      old_si[i] = new_si;

      if (cpu_stats[i] >= 100)
        cpu_stats[i] = 100;

      /*
      msg.val = cpu_stats[i];
      edje_object_message_send(cf->cpu_obj, EDJE_MESSAGE_FLOAT, i, &msg);
      */
      
      i++;
   }
   fclose(stat);   
}

static void 
_cpu_face_graph_values(Cpu_Face *cf) 
{
   int x, y, w, h;
   Evas_Object *o;
   Evas_Object *last = NULL;
   Evas_List *l;
   int i, j = 0;
   int v;
   int a = 255;
   int b = 0;
   int c = 100;
   int d = 0;
   
   evas_object_geometry_get(cf->chart_obj, &x, &y, &w, &h);

   while (d < cpu_count)
     {
	v = (int)((double)cpu_stats[d] * ((double)h / (double)100));      
	o = evas_object_line_add(cf->evas);
	edje_object_part_swallow(cf->chart_obj, "lines", o);
	evas_object_layer_set(o, 1);
	if (cpu_stats[d] == 0)
	  evas_object_hide(o);
	else 
	  {
	     evas_object_line_xy_set(o, (x + w), (y + h), (x + w), ((y + h) - v));
	     switch (d) 
	       {
		case 0:
		  evas_object_color_set(o, a, b, b, c);
	          break;
	        case 1:
                  evas_object_color_set(o, b, a, b, c);
                  break;
                case 2:
                  evas_object_color_set(o, b, b, a, c);
                  break;
                case 3:
                  evas_object_color_set(o, a, a, b, c);
                  break;
                default:
                  break;
	       }	       
	     evas_object_pass_events_set(o, 1);
	     evas_object_show(o);
	  }
   
	cf->old_values[d] = evas_list_prepend(cf->old_values[d], o);
	l = cf->old_values[d];
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
	     cf->old_values[d] = evas_list_remove(cf->old_values[d], last);
	     evas_object_del(last);
	  }
	d++;
     }
}

static void 
_cpu_face_graph_clear(Cpu_Face *cf) 
{
   int i = 0;	
   Evas_List *l;

   while (i < cpu_count)
     {
	for (l = cf->old_values[i]; l; l = l->next) 
	  {
	     Evas_Object *o;
	     o = evas_list_data(l);
	     evas_object_del(o);
	  }
	evas_list_free(cf->old_values[i]);
	cf->old_values[i] = NULL;
	if (!cf->cpu->conf->show_graph)
	  evas_object_hide(cf->chart_obj);
	i++;
     }
}
