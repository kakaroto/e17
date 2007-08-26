/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2(0
 */
#include <e.h>
#include "e_mod_main.h"

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#endif

typedef struct _Instance Instance;
typedef struct _Cpu Cpu;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Cpu             *cpu;
   Ecore_Timer     *timer;

   E_Gadcon_Popup  *popup;
   Evas_Object     *o_bg, *o_chart[4], *o_poly[4];
   Evas_List       *points[4];
   int		    chart_colors[4][4];
};

struct _Cpu 
{
   Instance *inst;
   Evas_Object *o_icon;
};

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static Config_Item *_config_item_get(const char *id);
static int _set_cpu_load(void *data);
static int _get_cpu_count(void);
static int _get_cpu_load(void);
static void _graph_values(Instance *inst);
static void _menu_cb_post(void *data, E_Menu *m);
static void _cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cpu_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *cpu_conf = NULL;

static int cpu_count;
static int cpu_stats[4];
static float update_poll_time;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "cpu", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Cpu             *cpu;
   Instance        *inst;
   Config_Item     *ci;
   E_Gadcon_Client *gcc;
   const char	   *color_string;
   char             buf[4096];
   int		    i;


   cpu_count = _get_cpu_count();
   
   inst = E_NEW(Instance, 1);   

   ci = _config_item_get(id);
   if (!ci->id)
     ci->id = evas_stringshare_add(id);

   cpu = E_NEW(Cpu, 1);
   cpu->inst = inst;

   snprintf(buf, sizeof(buf), "%s/cpu.edj", 
	    e_module_dir_get(cpu_conf->module));
   
   cpu->o_icon = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(cpu->o_icon, 
				"base/theme/modules", "modules/cpu/main"))
     edje_object_file_set(cpu->o_icon, buf, "modules/cpu/main");
   evas_object_show(cpu->o_icon);
   
   gcc = e_gadcon_client_new(gc, name, id, style, cpu->o_icon);
   gcc->data   = inst;
   inst->gcc   = gcc;
   inst->cpu   = cpu;

   inst->popup = e_gadcon_popup_new(gcc, NULL);
   inst->o_bg  = edje_object_add(inst->popup->win->evas);
   if (!e_theme_edje_object_set(inst->o_bg, 
				"base/theme/modules", "modules/cpu/popup"))
     edje_object_file_set(inst->o_bg, buf, "modules/cpu/popup");

   for (i = 0; i < cpu_count && i < 4; i++)
     {
	snprintf(buf, sizeof(buf), "chart%d", i + 1);
	inst->o_chart[i] = edje_object_add(inst->popup->win->evas);
	evas_object_layer_set(inst->o_chart[i], 1);
	edje_object_part_swallow(inst->o_bg, buf, inst->o_chart[i]);

	inst->o_poly[i] = evas_object_polygon_add(inst->popup->win->evas);
	evas_object_layer_set(inst->o_poly[i], 1);
	evas_object_anti_alias_set(inst->o_poly[i], 1);

	evas_object_show(inst->o_chart[i]);
	evas_object_show(inst->o_poly[i]);

	snprintf(buf, sizeof(buf), "chart_color%d", i + 1);
	color_string = edje_object_data_get(inst->o_bg, buf);
	if (!color_string || sscanf(color_string, "%d %d %d %d",
				    &inst->chart_colors[i][0],
				    &inst->chart_colors[i][1],
				    &inst->chart_colors[i][2],
				    &inst->chart_colors[i][3]) != 4)
	  {
	     inst->chart_colors[i][0] = 55;
	     inst->chart_colors[i][1] = 110;
	     inst->chart_colors[i][2] = 250;
	     inst->chart_colors[i][3] = 155;
	  }
     }

   snprintf(buf, sizeof(buf), "e,state,orientation,%d",
	    cpu_count > 4 ? 4 : cpu_count);
   edje_object_signal_emit(inst->o_bg, buf, "e");

   e_gadcon_popup_content_set(inst->popup, inst->o_bg);

   cpu_conf->instances = evas_list_append(cpu_conf->instances, inst);

   evas_object_event_callback_add(cpu->o_icon, EVAS_CALLBACK_MOUSE_DOWN,
				  _cb_mouse_down, inst);
   evas_object_event_callback_add(cpu->o_icon, EVAS_CALLBACK_MOUSE_IN,
				   _cb_mouse_in, inst);
   evas_object_event_callback_add(cpu->o_icon, EVAS_CALLBACK_MOUSE_OUT,
				   _cb_mouse_out, inst);

   inst->timer = ecore_timer_add(ci->poll_time, _set_cpu_load, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   Cpu      *cpu;
   
   inst = gcc->data;
   cpu = inst->cpu;

   if (inst->popup)
     e_object_del(E_OBJECT(inst->popup));
   if (inst->timer)
     ecore_timer_del(inst->timer);
   if (cpu->o_icon)
     evas_object_del(cpu->o_icon);
   
   cpu_conf->instances = evas_list_remove(cpu_conf->instances, inst);
   E_FREE(cpu);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void) 
{
   return _("Cpu");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char         buf[4096];

   if (!cpu_conf->module) return NULL;
   
   snprintf(buf, sizeof(buf), "%s/e-module-cpu.edj", 
	    e_module_dir_get(cpu_conf->module));
   
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static Config_Item *
_config_item_get(const char *id) 
{
   Evas_List   *l;
   Config_Item *ci;

   for (l = cpu_conf->items; l; l = l->next) 
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id))
	  {
	     update_poll_time = ci->poll_time;
	     return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->poll_time = 1;
   ci->show_popup = 1;
   ci->max_points = 20;
   update_poll_time = ci->poll_time;
   
   cpu_conf->items = evas_list_append(cpu_conf->items, ci);
   return ci;
}

static int 
_set_cpu_load(void *data) 
{
   Instance    *inst;
   Cpu         *cpu;
   Config_Item *ci;
   Evas_List   *l;
   int          i = 0, *p;
   char         str[100], str_tmp[100];

   if (cpu_count == -1) return 0;

   inst = data;
   if (!inst) return 1;
   cpu = inst->cpu;
   if (!cpu) return 1;
   
   _get_cpu_load();

   ci = _config_item_get(inst->gcc->id);

   for (i = 0; i < cpu_count; i++)
     {
	p = (int *) malloc(sizeof(int));
	*p = cpu_stats[i];

	if (evas_list_count(inst->points[i]) < ci->max_points)
	  {
	     inst->points[i] = evas_list_prepend(inst->points[i], p);
	     l = evas_list_last(inst->points[i]);
	     l->next = inst->points[i];
	     inst->points[i]->prev = l;
	  }
	else
	  {
	     inst->points[i] = inst->points[i]->prev;
	     if (inst->points[i]->data) free(inst->points[i]->data);
	     inst->points[i]->data = p;
	  }
     }

   _graph_values(inst);

   snprintf(str, sizeof(str), "%d%%", cpu_stats[0]);
   
   i = 1;
   while (i < cpu_count)
     {
	snprintf(str_tmp, sizeof(str_tmp), "<br>%d%%", cpu_stats[i]);
	strncat(str, str_tmp, sizeof(str));
	i++;
     }
   edje_object_part_text_set(cpu->o_icon, "load", str);
   return 1;
}

static int
_get_cpu_count(void)
{
   FILE *f;
   char tmp[4];
   int cpu = -1;

#ifdef __FreeBSD__
   cpu = 1;
#else
   if (!(f = fopen("/proc/stat", "r"))) return cpu;

   while (fscanf(f, "cp%s %*u %*u %*u %*u %*u %*u %*u %*u\n", (char *) &tmp) == 1)
     cpu++;

   fclose(f);
#endif

   return cpu;
}

static int
_get_cpu_load(void) 
{
#ifdef __FreeBSD__
   long cp_time[CPUSTATES];
   static long old_used, old_tot;
   long new_used, new_tot;
   size_t len;
#else
   FILE *stat;
   static unsigned long old_u[4], old_n[4], old_s[4], old_i[4], old_wa[4], old_hi[4], old_si[4];
   unsigned long new_u, new_n, new_s, new_i, new_wa = 0, new_hi = 0, new_si = 0, dummy2, ticks_past;
   int tmp_u = 0, tmp_n = 0, tmp_s = 0;
   char dummy[16];
   int i = 0;
#endif

#ifdef __FreeBSD__
   len = sizeof(cp_time);

   if (sysctlbyname("kern.cp_time", &cp_time, &len, NULL, 0) < 0)
     {
	warn("sysctl()");
	return 0;
     }

   new_used = cp_time[CP_USER] + cp_time[CP_NICE] + cp_time[CP_SYS];
   new_tot = new_used + cp_time[CP_IDLE];

   cpu_stats[0] = (100 * (float)(new_used - old_used) / (float)(new_tot - old_tot)) / update_poll_time;

   old_tot = new_tot;
   old_used = new_used; 
   
   cpu_stats[0]=(cpu_stats[0]>100?100:cpu_stats[0]);
#else
   if (!(stat = fopen("/proc/stat", "r"))) return -1;

   /* since if there are more than 1 CPUs, the first entry is the summary:
	cpu  366384 274786 214744 7129029 1975609 12775 353729 0
	cpu0 167188 137966 127694 3664600 704402 12775 353588 0
	cpu1 199195 136820 87050 3464429 1271207 0 140 0
	
	In this case the first line is read and forgotten
   */
   if(cpu_count>1){
      /* I had to add another %lu (linux 2.6.17) */
      fscanf(stat, "%s %lu %lu %lu %lu %lu %lu %lu %lu", dummy, &new_u, &new_n, &new_s, &new_i, &new_wa, &new_hi, &new_si, &dummy2);
   }
   
   while (i < cpu_count)
     {

        /* I had to add another %lu (linux 2.6.17) */
	if (fscanf(stat, "%s %lu %lu %lu %lu %lu %lu %lu %lu", dummy, &new_u, &new_n,
	     &new_s, &new_i, &new_wa, &new_hi, &new_si, &dummy2) < 5)
	  {
	     fclose (stat);
	     return -1;
	  }

	ticks_past = ((new_u + new_n + new_s + new_i + new_wa + new_hi + new_si) -
		      (old_u[i] + old_n[i] + old_s[i] + old_i[i] + old_wa[i] + old_hi[i] + old_si[i]));

	if (ticks_past)
	  {
	     tmp_u = ((new_u - old_u[i]));
	     tmp_n = ((new_n - old_n[i]));
	     tmp_s = ((new_s - old_s[i]));
	  }
	
	cpu_stats[i] = (tmp_u + tmp_n + tmp_s) / update_poll_time;

	old_u[i] = new_u;
	old_n[i] = new_n;
	old_s[i] = new_s;
	old_wa[i] = new_wa;
	old_hi[i] = new_hi;
	old_si[i] = new_si;
	
	cpu_stats[i]=(cpu_stats[i]>100?100:cpu_stats[i]);

	i++;
     }
   fclose (stat);
#endif
   return 0;
}

static void
_graph_values(Instance *inst)
{
   Config_Item *ci;
   Evas_List *l;
   int i, m, p, v, step = 3;
   float coeff;

   if (!inst->popup->win->visible) return;

   ci = _config_item_get(inst->gcc->id);
   for (i = 0; i < cpu_count; i++)
     {
	Evas_Coord w = 0, h = 0, x = 0, y = 0;

	evas_object_polygon_points_clear(inst->o_poly[i]);
	evas_object_geometry_get(inst->o_chart[i], &x, &y, &w, &h);

	coeff = (float) -h / 100;
	step = ((int) ((float) w / (float) ci->max_points + 0.5));
	if (step < 1) step = 1;
	if (step * ci->max_points < w) step++;

	/* Lower right */
	evas_object_polygon_point_add(inst->o_poly[i], p = (x + w), m = (y + h));
	for (l = inst->points[i]; l && p >= x; l = l->next, p = p - step)
	  {
	     v = m + (*(int *) l->data) * coeff;
	     evas_object_polygon_point_add(inst->o_poly[i], p, v);
	     if (l->next == inst->points[i]) break;
	  }
	if (p > x)
	  evas_object_polygon_point_add(inst->o_poly[i], p, m);
	/* Lower left */
	evas_object_polygon_point_add(inst->o_poly[i], x, m);
	evas_object_color_set(inst->o_poly[i],
			      inst->chart_colors[i][0],
			      inst->chart_colors[i][1],
			      inst->chart_colors[i][2],
			      inst->chart_colors[i][3]);
     }
}

void
_cpu_config_updated(const char *id)
{
  Evas_List *l;
  Config_Item *ci;

  if (!cpu_conf)
    return;
  ci = _config_item_get(id);
  for (l = cpu_conf->instances; l; l = l->next)
    {
      Instance *inst;

      inst = l->data;
      if (!inst->gcc->id)
	continue;

      if (!strcmp(inst->gcc->id, ci->id))
	{
	  if (inst->timer)
	    ecore_timer_del(inst->timer);
	  inst->timer =
	    ecore_timer_add(ci->poll_time, _set_cpu_load, inst);

	  if (!ci->show_popup)
	    {
	       if (inst->popup->pinned)
		 e_gadcon_popup_toggle_pinned(inst->popup);
	       e_gadcon_popup_hide(inst->popup);
	    }

	  break;
	}
    }
}

static void
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event_info;
   if (ev->button == 1)
     e_gadcon_popup_toggle_pinned(inst->popup);
   if ((ev->button == 3) && (!cpu_conf->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;

	mn = e_menu_new();
	cpu_conf->menu = mn;
	e_menu_post_deactivate_callback_set(mn, _menu_cb_post, inst);
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _cpu_menu_cb_configure, inst);

	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

        e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

static void 
_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Config_Item *ci;

   if (!(inst = data)) return;
   ci = _config_item_get(inst->gcc->id);
   if (!ci->show_popup) return;

   e_gadcon_popup_show(inst->popup);
   _graph_values(inst);
}

static void 
_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Config_Item *ci;
   
   if (!(inst = data)) return;
   ci = _config_item_get(inst->gcc->id);
   if (!ci->show_popup) return;

   e_gadcon_popup_hide(inst->popup);
}

static void
_menu_cb_post(void *data, E_Menu *m)
{
   if (!cpu_conf->menu) return;
   e_object_del(E_OBJECT(cpu_conf->menu));
   cpu_conf->menu = NULL;
}
   
static void
_cpu_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
   Instance *inst;
   Config_Item *ci;
   int i, max = 0, w = 0;

   inst = data;
   ci = _config_item_get (inst->gcc->id);

   for (i = 0; i < cpu_count && i < 4; i++)
     {
	evas_object_geometry_get(inst->o_chart[i],
				 NULL, NULL,
				 &w, NULL);

	if (w > max)
	  max = w;
     }

   _config_cpu_module(ci, max);
}

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "Cpu"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   conf_item_edd = E_CONFIG_DD_NEW("Cpu_Config_Item", Config_Item);
   conf_edd = E_CONFIG_DD_NEW("Cpu_Config", Config);
   
   #undef T
   #define T Config_Item
   #undef D
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, show_popup, INT);
   E_CONFIG_VAL(D, T, max_points, INT);
   
   #undef T
   #define T Config
   #undef D
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);
   
   cpu_conf = e_config_domain_load("module.cpu", conf_edd);
   if (!cpu_conf) 
     {
	Config_Item *ci;
	
	cpu_conf = E_NEW(Config, 1);
	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	ci->poll_time = 1;
	ci->show_popup = 1;
	ci->max_points = 20;
	
	cpu_conf->items = evas_list_append(cpu_conf->items, ci);
     }
   
   cpu_conf->module = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   cpu_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);
   if (cpu_conf->config_dialog)
     e_object_del(E_OBJECT(cpu_conf->config_dialog));
   if (cpu_conf->menu) 
     {
	e_menu_post_deactivate_callback_set(cpu_conf->menu, NULL, NULL);
	e_object_del(E_OBJECT(cpu_conf->menu));
	cpu_conf->menu = NULL;
     }

   while (cpu_conf->items) 
     {
	Config_Item *ci;
	
	ci = cpu_conf->items->data;
	if (ci->id)
	  evas_stringshare_del(ci->id);
	cpu_conf->items = evas_list_remove_list(cpu_conf->items, cpu_conf->items);
	E_FREE(ci);
     }

   E_FREE(cpu_conf);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Evas_List *l;
   
   for (l = cpu_conf->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _config_item_get(inst->gcc->id);
	if (ci->id)
	  evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.cpu", conf_edd, cpu_conf);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
  e_module_dialog_show (m, _("Enlightenment Cpu Monitor Module"),
			_("This module is used to monitor cpu load."));
  return 1;
}
