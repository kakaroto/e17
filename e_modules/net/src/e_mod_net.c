#include <e.h>
#include "e_mod_main.h"
#include "e_mod_net.h"
#include "e_mod_config.h"
#include "e_mod_configure.h"

static void _cb_post(void *data, E_Menu *m);
static void _cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);

EAPI int 
_cb_poll(void *data) 
{
   Instance *inst;
   Config_Item *ci;
   FILE *f;
   char buf[256], dev[64], tmp[100];
   int found = 0;
   long bin, bout;
   unsigned long in, out, dummy = 0;
   
   inst = data;
   ci = _config_item_get(inst->gcc->id);
   
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
   
   bin = in - inst->in;
   bout = out - inst->out;
   bin = bin / ci->poll_time;
   bout = bout / ci->poll_time;
   inst->in = in;
   inst->out = out;
   
   if (bin <= 0)
     edje_object_signal_emit(inst->o_net, "e,state,receive,idle", "e");
   else
     edje_object_signal_emit(inst->o_net, "e,state,receive,active", "e");
   if (bout <= 0)
     edje_object_signal_emit(inst->o_net, "e,state,send,idle", "e");
   else
     edje_object_signal_emit(inst->o_net, "e,state,send,active", "e");

   if (!inst->popup) return 1;

   if (bin > 1048576)
     snprintf(tmp, sizeof(tmp), "Rx: %ld Mb", (bin / 1048576));
   else if ((bin > 1024) && (bin < 1048576))
     snprintf(tmp, sizeof(tmp), "Rx: %ld Kb", (bin / 1024));
   else
     snprintf(tmp, sizeof(tmp), "Rx: %ld B", bin);
   edje_object_part_text_set(inst->popup->o_bg, "e.text.recv", tmp);

   if (bout > 1048576)
     snprintf(tmp, sizeof(tmp), "Tx: %ld Mb", (bout / 1048576));
   else if ((bout > 1024) && (bout < 1048576))
     snprintf(tmp, sizeof(tmp), "Tx: %ld Kb", (bout / 1024));
   else
     snprintf(tmp, sizeof(tmp), "Tx: %ld B", bout);
   edje_object_part_text_set(inst->popup->o_bg, "e.text.send", tmp);

   return 1;
}

EAPI void 
_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event;
   if ((ev->button == 3) && (!cfg->menu)) 
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int x, y;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _cb_post, inst);
	cfg->menu = mn;
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
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
_cb_mouse_in(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst;
   Config_Item *ci;
   E_Container *con;
   Evas_Object *bg, *box, *icon, *label;
   Evas_Coord x, y, w, h;
   Evas_Coord ox, oy, ow, oh;
   int top, wx, wy, ww, wh;
   char buf[PATH_MAX];
   
   inst = data;
   if (inst->popup) return;
   
   inst->popup = E_NEW(Popup, 1);
   ci = _config_item_get(inst->gcc->id);
   
   snprintf(buf, sizeof(buf), "%s/net.edj", e_module_dir_get(cfg->mod));
   evas_object_geometry_get(inst->o_net, &ox, &oy, &ow, &oh);
   
   con = e_container_current_get(e_manager_current_get());
   inst->popup->win = e_popup_new(e_zone_current_get(con), 0, 0, 0, 0);
   bg = edje_object_add(inst->popup->win->evas);
   inst->popup->o_bg = bg;
   if (!e_theme_edje_object_set(bg, "base/theme/modules", 
				"modules/net/popup"))
     edje_object_file_set(bg, buf, "modules/net/popup");
   snprintf(buf, sizeof(buf), _("Device - %s"), ci->device);
   edje_object_part_text_set(bg, "e.text.title", buf);
   evas_object_show(bg);

   snprintf(buf, sizeof(buf), "Rx: 0 B");
   edje_object_part_text_set(bg, "e.text.recv", buf);
   snprintf(buf, sizeof(buf), "Tx: 0 B");
   edje_object_part_text_set(bg, "e.text.send", buf);
   
   edje_object_size_min_calc(bg, &ww, &wh);
   evas_object_move(bg, 0, 0);
   evas_object_resize(bg, ww, wh);
   
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
   wx = x + ox;
   wy = y + oy;
   if ((wy - con->y) < (con->h / 2))
     top = 0;
   else
     top = 1;
   
   switch (inst->gcc->gadcon->orient) 
     {
      case E_GADCON_ORIENT_CORNER_LT:
	wx += ox + ow;
	if (top) wy += (oh - 5);
	break;
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LB:
	wx += ox + ow;
	wy = y;
	break;
      case E_GADCON_ORIENT_CORNER_RT:
	wx -= ww - (ox - 5);
	if (top) wy += oh;
	break;
      case E_GADCON_ORIENT_RIGHT:
	wx -= ww - (ox - 5);
	if (top) wy -= (oh - 5);
	break;
      case E_GADCON_ORIENT_CORNER_RB:
	wx -= ww - (ox - 5);
	wy -= oy;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	wx += ((ow - ww) / 2);
	if (wx < x) wx = x;
	if ((wx + ww) > (x + w)) wx = ((x + w) - ww);
	if (!top) wy += oh;
	break;
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_BL:
	wx += ((ow - ww) / 2);
	if (wx < x) wx = x;
	wy = (y - wh);
	break;
      case E_GADCON_ORIENT_CORNER_BR:
	wx += ((ow - ww) / 2);
	if (wx < x) wx = x;
	if ((wx + ww) > (x + w)) wx = ((x + w) - ww);
	wy = (y - wh);
	break;
     }
   e_popup_move_resize(inst->popup->win, wx, wy, ww, wh);
   e_popup_show(inst->popup->win);
}

EAPI void 
_cb_mouse_out(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst;
   
   inst = data;
   if (!inst->popup) return;
   evas_object_del(inst->popup->o_bg);
   e_object_del(E_OBJECT(inst->popup->win));
   E_FREE(inst->popup);
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
