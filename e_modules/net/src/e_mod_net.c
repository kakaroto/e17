/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_net.h"
#include "e_mod_config.h"
#include "e_mod_configure.h"
#ifdef __FreeBSD__
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_mib.h>
#endif

typedef unsigned long bytes_t;
static void _bytes_to_string(bytes_t bytes, char *string, int size);
static void _cb_post(void *data, E_Menu *m);
static void _cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);

#ifdef __FreeBSD__
static int get_ifmib_general(int row, struct ifmibdata *ifmd); 

static int 
get_ifmib_general(int row, struct ifmibdata *ifmd) 
{ 
   int name[6]; 
   size_t len; 

   name[0] = CTL_NET; 
   name[1] = PF_LINK; 
   name[2] = NETLINK_GENERIC; 
   name[3] = IFMIB_IFDATA; 
   name[4] = row; 
   name[5] = IFDATA_GENERAL; 
   
   len = sizeof(*ifmd); 
   
   return sysctl(name, 6, ifmd, &len, (void *)0, 0);
}
#endif

EAPI int 
_cb_poll(void *data) 
{
   Instance *inst;
   Config_Item *ci;
   char buf[256], popbuf[256], tmp[100];
   long bin, bout;
   bytes_t in, out = 0;

   inst = data;
   ci = _config_item_get(inst->gcc->id);

#ifndef __FreeBSD__	
   FILE *f;
   char dev[64];
   bytes_t dummy = 0;
   int found = 0;
    
   f = fopen("/proc/net/dev", "r");
   if (!f) return 1;
   
   while (fgets(buf, 256, f))
     {
	int i = 0;

	for (; buf[i] != 0; i++)
	  if (buf[i] == ':') buf[i] = ' ';
	
	if (sscanf (buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
		    "%lu %lu %lu %lu\n", dev, &in, &dummy, &dummy,
		    &dummy, &dummy, &dummy, &dummy, &dummy, &out, &dummy,
		    &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 17)
	  continue;

	if (!strcmp(dev, ci->device)) found = 1;
	if (found) break;
     }
   fclose(f);
   if (!found) return 1;
#else
   struct ifmibdata *ifmd;
   int i, count, len;
   
   len = sizeof(count);
   sysctlbyname("net.link.generic.system.ifcount", &count, &len, (void *)0, 0);

   ifmd = malloc(sizeof(struct ifmibdata));
   for(i=1; i <= count; ++i)
     { 
	get_ifmib_general(i, ifmd);
	if (!strcmp(ifmd->ifmd_name, ci->device)) break;
     }

   in = ifmd->ifmd_data.ifi_ibytes; 
   out = ifmd->ifmd_data.ifi_obytes;

   free(ifmd);
#endif

   bin = in - inst->in;
   bout = out - inst->out;
   bin = bin / 0.5;
   bout = bout / 0.5;
   inst->in = in;
   inst->out = out;

   if (bin <= ci->limit)
     edje_object_signal_emit(inst->o_net, "e,state,receive,idle", "e");
   else
     edje_object_signal_emit(inst->o_net, "e,state,receive,active", "e");
   if (bout <= ci->limit)
     edje_object_signal_emit(inst->o_net, "e,state,send,idle", "e");
   else
     edje_object_signal_emit(inst->o_net, "e,state,send,active", "e");

   _bytes_to_string(bin, tmp, sizeof(tmp));
   snprintf(buf, sizeof(buf), "%s/s", tmp);
   edje_object_part_text_set(inst->o_net, "e.text.recv", buf);
   if (inst->popup) 
     {
	_bytes_to_string(in, tmp, sizeof(tmp));
	snprintf(popbuf, sizeof(popbuf), "Rx: %s", tmp);
	edje_object_part_text_set(inst->popup->o_bg, "e.text.recv", popbuf);
     }
   
   _bytes_to_string(bout, tmp, sizeof(tmp));
   snprintf(buf, sizeof(buf), "%s/s", tmp);
   edje_object_part_text_set(inst->o_net, "e.text.send", buf);
   if (inst->popup) 
     {
	_bytes_to_string(out, tmp, sizeof(tmp));
	snprintf(popbuf, sizeof(popbuf), "Tx: %s", tmp);
	edje_object_part_text_set(inst->popup->o_bg, "e.text.send", popbuf);
     }
   
   return 1;
}

EAPI void 
_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Exe *x;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event;
   ci = _config_item_get(inst->gcc->id);
   if ((ev->button == 1) && (ev->flags & EVAS_BUTTON_DOUBLE_CLICK)) 
     {
	if (ci->app != NULL) 
	  {
	     x = ecore_exe_run(ci->app, NULL);
	     if (x) ecore_exe_free(x);
	  }
     }
   else if ((ev->button == 1) && (!cfg->menu))
     {
	if (inst->popup_locked) 
	  inst->popup_locked = 0;
	else
	  inst->popup_locked = 1;
     }
   else if ((ev->button == 3) && (!cfg->menu)) 
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int x, y;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _cb_post, inst);
	cfg->menu = mn;
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _cb_configure, inst);
	
	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, 
					  NULL, NULL);
	e_menu_activate_mouse(mn, 
			      e_util_zone_current_get(e_manager_current_get()),
			      x + ev->output.x, y + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	e_util_evas_fake_mouse_up_later(inst->gcc->gadcon->evas, ev->button);
     }
}

EAPI void 
_cb_mouse_in(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;
   Config_Item *ci;
   E_Container *con;
   Evas_Object *bg;
   Evas_Coord cx, cy, cw, ch;
   Evas_Coord ox, oy, ow, oh;
   Evas_List *l;
   int layer = -1;
   int wx, wy, ww, wh;
   char buf[PATH_MAX];

   inst = data;
   if (inst->popup != NULL) return;
   ci = _config_item_get(inst->gcc->id);
   if (!ci->show_popup) return;
   
   inst->popup = E_NEW(Popup, 1);
   snprintf(buf, sizeof(buf), "%s/net.edj", e_module_dir_get(cfg->mod));
   
   con = e_container_current_get(e_manager_current_get());
   inst->popup->win = e_popup_new(e_zone_current_get(con), 0, 0, 0, 0);
   bg = edje_object_add(inst->popup->win->evas);
   if (!e_theme_edje_object_set(bg, "base/theme/modules", 
				"modules/net/popup"))
     edje_object_file_set(bg, buf, "modules/net/popup");
   snprintf(buf, sizeof(buf), D_("Device - %s"), ci->device);
   edje_object_part_text_set(bg, "e.text.title", buf);
   evas_object_show(bg);

   snprintf(buf, sizeof(buf), "Rx: 0 B");
   edje_object_part_text_set(bg, "e.text.recv", buf);
   snprintf(buf, sizeof(buf), "Tx: 0 B");
   edje_object_part_text_set(bg, "e.text.send", buf);
   
   edje_object_size_min_calc(bg, &ww, &wh);
   evas_object_move(bg, 0, 0);
   evas_object_resize(bg, ww, wh);
   inst->popup->o_bg = bg;

   /* Begin Butt Ugly hack for shelf "layer"/position changes */
   cx = cy = cw = ch = -1;
   for (l = e_shelf_list(); l; l = l->next) 
     {
	E_Shelf *es;
	
	es = l->data;
	if (es->gadcon != inst->gcc->gadcon) continue;
	layer = es->layer;
	cx = es->x;
	cy = es->y;
	cw = es->w;
	ch = es->h;
	break;
     }
   
   if (cx == -1) return;
   evas_object_geometry_get(inst->o_net, &ox, &oy, &ow, &oh);
   switch (inst->gcc->gadcon->orient) 
     {
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
      case E_GADCON_ORIENT_RIGHT:
	wx = (cx - ww);
	if (layer == 1) 
	  wy = oy;
	else 
	  wy = (cy + oy);
	if ((wy + wh) > (cy + ch))
	  wy = (cy + ch) - wh;
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	wx = (cx + cw);
	if (layer == 1)
	  wy = oy;
	else
	  wy = (cy + oy);
	if ((wy + wh) > (cy + ch))
	  wy = (cy + ch) - wh;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
	if (layer == 1)
	  wx = ox;
	else
	  wx = (cx + ox);
	wy = (cy + ch);
	break;
      case E_GADCON_ORIENT_CORNER_TR:
	if (layer == 1)
	  wx = ox;
	else
	  wx = (cx + ox);
	wy = (cy + ch);
	if ((wx + ww) > (cx + cw))
	  wx = (cx + cw) - ww;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
	if (layer == 1)
	  wx = ox;
	else
	  wx = (cx + ox);
	wy = (cy - wh);
	break;
      case E_GADCON_ORIENT_CORNER_BR:
	if (layer == 1)
	  wx = ox;
	else
	  wx = (cx + ox);
	wy = (cy - wh);
	if ((wx + ww) > (cx + cw))
	  wx = (cx + cw) - ww;
	break;
      default:
	break;
     }
   e_popup_move_resize(inst->popup->win, wx, wy, ww, wh);
   e_popup_show(inst->popup->win);
}

EAPI void 
_cb_mouse_out(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;
   
   inst = data;
   if (!inst->popup) return;
   if (inst->popup_locked) return;
   evas_object_del(inst->popup->o_bg);
   e_object_del(E_OBJECT(inst->popup->win));
   E_FREE(inst->popup);
}

static void
_bytes_to_string(bytes_t bytes, char *string, int size)
{
   if (bytes > 1048576)
     snprintf(string, size, "%lu Mb", (bytes / 1048576));
   else if ((bytes > 1024) && (bytes < 1048576))
     snprintf(string, size, "%lu Kb", (bytes / 1024));
   else
     snprintf(string, size, "%lu B", bytes);
}

static void 
_cb_post(void *data, E_Menu *m) 
{
   if (!cfg->menu) return;
   e_menu_post_deactivate_callback_set(cfg->menu, NULL, NULL);
   e_object_del(E_OBJECT(cfg->menu));
   cfg->menu = NULL;
}

static void 
_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Instance *inst;
   Config_Item *ci;
   
   inst = data;
   ci = _config_item_get(inst->gcc->id);
   _configure_net_module(ci);
}
