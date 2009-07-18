#include "e.h"
#include "eco_main.h"
#include <X11/Xlib.h>

#define D(x) //do { printf(__FILE__ ":%d:\t", __LINE__); printf x; fflush(stdout); } while(0)   

#define MOD(a,b) ((a) < 0 ? ((b) - ((-(a) - 1) % (b))) - 1 : (a) % (b))

#define ECO_PLUGIN_TERMINATE_NOTIFY 1

static int  _eco_cb_client_message(void *data, int ev_type, void *ev);
static int  _eco_cb_window_configure(void *data, int ev_type, void *ev);
static int  _eco_cb_zone_desk_count_set(void *data, int ev_type, void *ev);
static int  _eco_cb_border_icon_change(void *data, int ev_type, void *ev);

static int _eco_window_icon_init(void);
static Evas_Object *_eco_border_icon_add(E_Border *bd, Evas *evas, int size);
static void _eco_window_icon_shutdown(void);


static Ecore_X_Atom ECOMORPH_MANAGED_ATOM = 0;

typedef struct _Eco_Icon Eco_Icon;

struct _Eco_Icon
{
  Evas *evas;
  int  size;
  unsigned int *data;
  unsigned int *icon_ptr;
};

static Eina_List *eco_handlers = NULL;
static Eco_Icon *icon = NULL;

EAPI int
eco_event_init(void)
{
   Ecore_Event_Handler *h;
   int geom[2];
   E_Zone *zone;
   
   h = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _eco_cb_client_message, NULL);
   if (h) eco_handlers = eina_list_append(eco_handlers, h);

   /* E events */
   h = ecore_event_handler_add(E_EVENT_ZONE_DESK_COUNT_SET, _eco_cb_zone_desk_count_set, NULL);
   if (h) eco_handlers = eina_list_append(eco_handlers, h);
   h = ecore_event_handler_add(E_EVENT_BORDER_ICON_CHANGE, _eco_cb_border_icon_change, NULL);
   if (h) eco_handlers = eina_list_append(eco_handlers, h);

   ECOMORPH_MANAGED_ATOM = ecore_x_atom_get("__ECOMORPH_WINDOW_MANAGED");   
   
   _eco_window_icon_init();

   /* TODO find a better place for this */
   /* TODO remove the client_message */
   /* set desktop geometry info on root window */
   zone = e_util_zone_current_get(e_manager_current_get());
   
   geom[0] = zone->desk_x_count * zone->w;
   geom[1] = zone->desk_y_count * zone->h;
  
   ecore_x_client_message32_send(e_manager_current_get()->root, 
				 ECORE_X_ATOM_NET_DESKTOP_GEOMETRY,
				 SubstructureNotifyMask,
				 geom[0],geom[1], 0, 0, 0);

   ecore_x_window_prop_card32_set(e_manager_current_get()->root,
				  ECORE_X_ATOM_NET_DESKTOP_GEOMETRY,
				  geom, 2);  
   return 1;
}


EAPI int
eco_event_shutdown(void)
{
   Ecore_Event_Handler *h;
   
   while (eco_handlers)
     {
	h = eco_handlers->data;
	eco_handlers = eina_list_remove_list(eco_handlers, eco_handlers);
	ecore_event_handler_del(h);
     }

   return 1;
}



static int
_eco_cb_client_message(void *data, int ev_type __UNUSED__, void *ev)
{
   Ecore_X_Event_Client_Message *e;
   E_Border *bd;
   /* TODO ECOMORPH_MANAGED_ATOM for all communication with ecomp
    * ... yea the atom could get a better fitting name too*/  
   e = ev;
   //printf ("_ecomorph_cb_client_message %d\n", (int)e->data.l[0]);
  
   if ((int)e->data.l[0] != 2) return 1;
  
   if (e->message_type ==  ECORE_X_ATOM_NET_MOVERESIZE_WINDOW)
     { 
	D(("0x%x :_ecomorph_cb_client_message\n", e->win));
	D(("ECORE_X_ATOM_NET_MOVEREISZE_WINDOW %d:%d %dx%d\n", 
	   (int)e->data.l[1], (int)e->data.l[2], (int)e->data.l[3], (int)e->data.l[4]));
	bd = e_border_find_by_window(e->win);
	if (!bd) return 1;

	int x = e->data.l[1];
	int y = e->data.l[2];
	int dx = x / bd->zone->w;
	int dy = y / bd->zone->h;
      
	/* here should always be a desk. for now move it on visible desk 
	 */
	if (dx < 0) dx *= -1;
	if (dy < 0) dy *= -1;

	if ((dx != bd->desk->x) || (dy != bd->desk->y))
	  {
	     D(("move to desk: %d:%d from %d:%d\n", dx, dy, bd->desk->x, bd->desk->y));
	     E_Desk *desk = e_desk_at_xy_get(bd->zone, dx, dy);
	     if (desk)
	       {
		  e_border_desk_set(bd, desk);
	       }
	  }

	e_border_move(bd, MOD(x, bd->zone->w), MOD(y, bd->zone->h));
     }
   else if (e->message_type ==  ECORE_X_ATOM_NET_RESTACK_WINDOW)
     { 
	D(("0x%x :_ecomorph_cb_client_message\n", e->win));
	D(("ECORE_X_ATOM_NET_RESTACK_WINDOW %d %d\n", (int)e->data.l[1], (int)e->data.l[2]));

	bd = e_border_find_by_window(e->win);
	if (!bd) return 1;

	if (!bd->lock_client_stacking)
	  {
	     if (e->data.l[1])
	       {
		  E_Border *obd;
		  Ecore_X_Window sibling = e->data.l[1];

		  if (e->data.l[2] == ECORE_X_WINDOW_STACK_ABOVE)
		    {
		       obd = e_border_find_by_window(sibling);
		       if (obd)
			 e_border_stack_above(bd, obd);
		    }
		  else if (e->data.l[2] == ECORE_X_WINDOW_STACK_BELOW)
		    {
		       obd = e_border_find_by_window(sibling);
		       if (obd)
			 e_border_stack_below(bd, obd);
		    }
	       }
	     else
	       {
		  if (e->data.l[2] == ECORE_X_WINDOW_STACK_ABOVE)
		    {
		       e_border_raise(bd);
		    }
		  else if (e->data.l[2] == ECORE_X_WINDOW_STACK_BELOW)
		    {
		       e_border_lower(bd);
		    }
	       }
	  }
     }
   else if (e->message_type ==  ECORE_X_ATOM_NET_ACTIVE_WINDOW)
     { 
	D(("0x%x :_ecomorph_cb_client_message\n", e->win));
	D(("ECORE_X_ATOM_NET_ACTIVE_WINDOW\n"));
	bd = e_border_find_by_window(e->win);
	if (!bd) return 1;
      
	if (bd->desk !=  e_desk_current_get(bd->zone))
	  e_desk_show(bd->desk);

	if (bd->shaded)
	  e_border_unshade(bd, E_DIRECTION_UP);
      
	if (bd->iconic)
	  e_border_uniconify(bd);
      
	e_border_raise(bd);
      
	e_border_focus_set(bd, 1, 1);

     }
   else if (e->message_type ==  ECORE_X_ATOM_NET_DESKTOP_VIEWPORT)
     { 
	D(("0x%x :_ecomorph_cb_client_message\n", e->win));
	D(("ECORE_X_ATOM_NET_DESKTOP_VIEWPORT %d %d %d\n", (int)e->data.l[1], (int)e->data.l[2], (int)e->data.l[3]));

	E_Manager *man = e_manager_current_get();

	if (e->win == man->root)
	  { 
	     E_Zone *zone = e_util_zone_current_get(man); 
	     int dx = (int)e->data.l[1];
	     int dy = (int)e->data.l[2];
     
	     if (e_desk_current_get(zone) != e_desk_at_xy_get(zone, dx, dy))
	       {
		  e_zone_desk_flip_to(zone, dx, dy);
	       }
	  }
     }
   else if (e->message_type ==  ECOMORPH_MANAGED_ATOM)
     {
	int type = (int)e->data.l[0];
	int val =(int) e->data.l[1];
	
	switch(val) {
	 case ECO_PLUGIN_TERMINATE_NOTIFY:
	    eco_action_terminate();
	     break;
	default:
	     
	     break;
	}
     }

   return 1;
}


static int
_eco_cb_zone_desk_count_set(void *data, int ev_type, void *ev)
{
   E_Event_Zone_Desk_Count_Set *e = ev;
   int geom[2];
   geom[0] = e->zone->desk_x_count * e->zone->w;
   geom[1] = e->zone->desk_y_count * e->zone->h;
  
   ecore_x_client_message32_send(e_manager_current_get()->root, 
				 ECORE_X_ATOM_NET_DESKTOP_GEOMETRY,
				 SubstructureNotifyMask,
				 geom[0],geom[1], 0, 0, 0);

   ecore_x_window_prop_card32_set(e_manager_current_get()->root,
				  ECORE_X_ATOM_NET_DESKTOP_GEOMETRY,
				  geom, 2);  
   return 1;
}




/*********************************************************************/

static int
_eco_window_icon_init(void)
{
   Evas_Engine_Info_Buffer *einfo;
   int rmethod;

   icon = calloc(sizeof(Eco_Icon),1);
   icon->size = 64;
  
   icon->evas = evas_new();
   if (!icon->evas)
     {
	return 0;
     }
   rmethod = evas_render_method_lookup("buffer");
   evas_output_method_set(icon->evas, rmethod);
   evas_output_size_set(icon->evas, icon->size, icon->size);
   evas_output_viewport_set(icon->evas, 0, 0, icon->size, icon->size);

   icon->data = malloc((2 + icon->size * icon->size) * sizeof(int));
   if (!icon->data)
     {
	evas_free(icon->evas);
	icon->evas = NULL;
	return 0;
     }

   icon->data[0] = 64;
   icon->data[1] = 64;
   icon->icon_ptr = icon->data + 2;
  
   einfo = (Evas_Engine_Info_Buffer *)evas_engine_info_get(icon->evas);
   if (!einfo)
     {
	free(icon->data);
	icon->data = NULL;
	evas_free(icon->evas);
	icon->evas = NULL;
	return 0;
     }
   einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
   einfo->info.dest_buffer = icon->icon_ptr; //icon->pixels;
   einfo->info.dest_buffer_row_bytes = icon->size * sizeof(int);
   einfo->info.use_color_key = 0;
   einfo->info.alpha_threshold = 0;
   einfo->info.func.new_update_region = NULL;
   einfo->info.func.free_update_region = NULL;
   evas_engine_info_set(icon->evas, (Evas_Engine_Info *)einfo);


   
   /* Eina_List *l;
    * E_Border *bd;
    * 
    * for (l = e_border_client_list(); l; l = l->next)
    *   {
    * 	bd = l->data;
    *   
    * 	_eco_border_icon_add(bd, icon->evas, 128);
    * 
    *   } */
}

static void
_eco_window_icon_shutdown(void)
{
   if (icon)
     {
	if (icon->evas) evas_free(icon->evas);
	if (icon->data) free(icon->data);
	free(icon);
     }
}

static Evas_Object *
_eco_border_icon_add(E_Border *bd, Evas *evas, int size)
{
   Evas_Object *o;
 
   E_OBJECT_CHECK_RETURN(bd, NULL);
   E_OBJECT_TYPE_CHECK_RETURN(bd, E_BORDER_TYPE, NULL);

   o = NULL;
   if (bd->internal)
     {
	o = edje_object_add(evas);
	if (!bd->internal_icon) 
	  e_util_edje_icon_set(o, "enlightenment/e");
	else
	  {
	     if (!bd->internal_icon_key)
	       {
		  char *ext;
		  ext = strrchr(bd->internal_icon, '.');
		  if ((ext) && ((!strcmp(ext, ".edj"))))
		    {
		       if (!edje_object_file_set(o, bd->internal_icon, "icon"))
			 e_util_edje_icon_set(o, "enlightenment/e");	       
		    }
		  else if (ext)
		    {
		       evas_object_del(o);
		       o = e_icon_add(evas);
		       e_icon_file_set(o, bd->internal_icon);
		    }
		  else 
		    {
		       if (!e_util_edje_icon_set(o, bd->internal_icon))
			 e_util_edje_icon_set(o, "enlightenment/e"); 
		    }
	       }
	     else
	       {
		  edje_object_file_set(o, bd->internal_icon,
				       bd->internal_icon_key);
	       }
	  }
     }

   if (!o)
     {
	if ((bd->desktop) && (bd->icon_preference != E_ICON_PREF_NETWM))
	  {
	     o = e_util_desktop_icon_add(bd->desktop, 256, evas);
	  }
	else if (bd->client.netwm.icons)
	  {
	     int i, diff, tmp, found = 0;
	     o = e_icon_add(evas);

	     diff = abs(bd->client.netwm.icons[0].width - size);

	     for (i = 1; i < bd->client.netwm.num_icons; i++)
	       {
		  if ((tmp = abs(bd->client.netwm.icons[i].width - size)) < diff)
		    {
		       diff = tmp;
		       found = i;
		    }
	       }
	     
	     e_icon_data_set(o, bd->client.netwm.icons[found].data,
			     bd->client.netwm.icons[found].width,
			     bd->client.netwm.icons[found].height);
	     e_icon_alpha_set(o, 1);
	  }
     }

   if (!o)
     {
	o = edje_object_add(evas);
	e_util_edje_icon_set(o, "enlightenment/unknown");
     }

   //Evas_Object *o = _eco_border_icon_add(e->border, icon->evas, 64);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, icon->size, icon->size);
   evas_object_show(o);
  
   Eina_List *updates = evas_render_updates(icon->evas);

   if (updates)
     {
	ecore_x_window_prop_card32_set(bd->win, 
				       ECORE_X_ATOM_NET_WM_ICON, 
				       icon->data, 
				       icon->size * icon->size + 2);
      
	evas_render_updates_free(updates);
     }
 
   evas_object_del(o);
  
   
   //return o;
}

static 
int _eco_cb_border_icon_change(void *data, int ev_type, void *ev)
{
   E_Event_Border_Icon_Change *e = ev;

   if (!icon) return 0;
  
   _eco_border_icon_add(e->border, icon->evas, 128);
   
   return 1;
}
