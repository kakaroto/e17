#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include "e.h"
#include "eco_main.h"

#define ECOMORPH_EVENT_MAPPED  0
#define ECOMORPH_EVENT_STATE   1
#define ECOMORPH_EVENT_DESK    2
#define ECOMORPH_EVENT_RESTART 3
#define ECOMORPH_EVENT_GRAB    4
#define ECOMORPH_WINDOW_STATE_INVISIBLE 0
#define ECOMORPH_WINDOW_STATE_VISIBLE 1


typedef struct _Eco_Border_Data Eco_Border_Data;


struct _Eco_Border_Data
{
  E_Border *border;
  Ecore_Event_Handler *damage_handler;
  Ecore_X_Damage damage;
  Ecore_X_Rectangle damage_rect;
  Ecore_Timer *damage_timeout;
  
  /* this is used to ind*/
  //unsigned char border_move_by : 1;    
};

static void _eco_border_cb_hook_new_border(void *data, E_Border *bd);
static void _eco_border_cb_hook_pre_new_border(void *data, E_Border *bd);
static void _eco_border_cb_hook_post_new_border(void *data, E_Border *bd);
static void _eco_border_cb_hook_pre_fetch(void *data, E_Border *bd);
static void _eco_border_cb_hook_post_fetch(void *data, E_Border *bd);
static int _eco_cb_border_remove(void *data, int ev_type, void *ev);
static int _eco_cb_border_show(void *data, int ev_type, void *ev);
static Eco_Border_Data *_eco_border_data_find(E_Border *bd);

static Eina_List *eco_handlers = NULL;
static Eina_List *eco_border_hooks = NULL;
static Eina_List *eco_borders = NULL;


EAPI
int eco_border_init(void)
{
   Ecore_Event_Handler *h;
   E_Border_Hook *hook;
   
   h = ecore_event_handler_add(E_EVENT_BORDER_REMOVE, _eco_cb_border_remove, NULL);
   if (h) eco_handlers = eina_list_append(eco_handlers, h);
   h = ecore_event_handler_add(E_EVENT_BORDER_SHOW, _eco_cb_border_show, NULL);
   if (h) eco_handlers = eina_list_append(eco_handlers, h);
   
   hook = e_border_hook_add(E_BORDER_HOOK_NEW_BORDER, _eco_border_cb_hook_new_border, NULL);
   if (hook) eco_border_hooks = eina_list_append(eco_border_hooks, hook);
   hook = e_border_hook_add(E_BORDER_HOOK_EVAL_PRE_NEW_BORDER, _eco_border_cb_hook_pre_new_border, NULL);
   if (hook) eco_border_hooks = eina_list_append(eco_border_hooks, hook);
   hook = e_border_hook_add(E_BORDER_HOOK_EVAL_POST_NEW_BORDER, _eco_border_cb_hook_post_new_border, NULL);
   if (hook) eco_border_hooks = eina_list_append(eco_border_hooks, hook);
   hook = e_border_hook_add(E_BORDER_HOOK_EVAL_PRE_FETCH, _eco_border_cb_hook_pre_fetch, NULL);
   if (hook) eco_border_hooks = eina_list_append(eco_border_hooks, hook);
   hook = e_border_hook_add(E_BORDER_HOOK_EVAL_POST_FETCH, _eco_border_cb_hook_post_fetch, NULL);
   if (hook) eco_border_hooks = eina_list_append(eco_border_hooks, hook);
}

EAPI int
eco_border_shutdown(void)
{
   Ecore_Event_Handler *h;
   E_Border_Hook *hook;
   Eco_Border_Data *bdd;
   
   EINA_LIST_FREE (eco_handlers, h)
     {
       if (h) ecore_event_handler_del(h);
     }

   EINA_LIST_FREE (eco_border_hooks, hook)
     {
       if (hook) e_border_hook_del(hook);
     }

   EINA_LIST_FREE (eco_borders, bdd)
     {
       if (bdd->damage) ecore_x_damage_free(bdd->damage);
       if (bdd->damage_handler) ecore_event_handler_del(bdd->damage_handler);
       free(bdd);
     }
}

static Eco_Border_Data *
_eco_border_data_find(E_Border *bd)
{
   Eco_Border_Data *bdd;
   Eina_List *l;
   for (l = eco_borders; l; l = l->next)
     {
	bdd = l->data;
	if (bdd->border == bd)
	  return bdd;
     }
   return NULL;
}

/* hacks for this issue http://lists.freedesktop.org/archives/xorg/2008-August/038022.html */
/* delay mapping until damage arrives + some delay for slow drawers */
static int
_e_border_damage_wait_time_out(void *data)
{
   E_Border *bd;
   Eco_Border_Data *bdd = data;
   E_OBJECT_TYPE_CHECK_RETURN(bdd->border, E_BORDER_TYPE, 0);

   bd = bdd->border;
   
   if(bdd->damage)
     {
	XRectangle rect = { .x = 0, .y = 0, .width = 0, .height = 0 };
	XShapeCombineMask(ecore_x_display_get(), bd->win, ShapeInput, 0, 0, 0, ShapeSet);

	ecore_x_client_message32_send(e_manager_current_get()->root,
				      ECOMORPH_ATOM_MANAGED,
				      SubstructureRedirectMask | SubstructureNotifyMask,
				      (unsigned int)bd->win, 
				      ECOMORPH_EVENT_MAPPED, 
				      ECOMORPH_WINDOW_STATE_VISIBLE, 0, 0);

	ecore_x_damage_free(bdd->damage);
	bdd->damage = 0;
	bdd->damage_timeout = NULL;
	printf("e_border_damage_wait_timeout end 0x%x\n", (unsigned int) bd->win);
	/* e_object_unref(E_OBJECT(bd)); */
     }
   return 0;
}

static int 
_e_border_cb_damage_notify(void *data, int ev_type, void *ev)
{
  Ecore_X_Event_Damage *e;
  Eco_Border_Data *bdd;
  E_Border *bd;
  
  e = ev;
  bdd = data;
  /* E_OBJECT_TYPE_CHECK_RETURN(bdd->border, E_BORDER_TYPE, 0); */
  bd = bdd->border;
  //if(!bd || !E_OBJECT_TYPE_CHECK(bd, E_BORDER_TYPE) || !bd->damage)  return 0;

  if(e->area.x < bdd->damage_rect.x) bdd->damage_rect.x = e->area.x;
  if(e->area.y < bdd->damage_rect.y) bdd->damage_rect.y = e->area.y;
  if(e->area.width + e->area.x > bdd->damage_rect.width) bdd->damage_rect.width = e->area.width + e->area.x;
  if(e->area.height + e->area.y > bdd->damage_rect.height) bdd->damage_rect.height = e->area.height + e->area.y;

  if((bdd->damage_rect.width  >= bd->w - (bd->client_inset.l + bd->client_inset.r)) &&
     (bdd->damage_rect.height >= bd->h - (bd->client_inset.t + bd->client_inset.b)))
    {
      /* bdd->damage_rect.x = 0xffffff;
       * bdd->damage_rect.y = 0xffffff;
       * bdd->damage_rect.width = 0;
       * bdd->damage_rect.height = 0; */

      ecore_event_handler_del(bdd->damage_handler);
      bdd->damage_handler = NULL;
      /* slow drawer - reduce ugly pixmap distortion on mapping */
      /* if(bd->client.icccm.class && 
       * 	 ((!strcmp(bd->client.icccm.class, "Firefox")) ||
       * 	  (!strncmp(bd->client.icccm.class, "OpenOffice.org", 14))))
       * 	{
       * 	  ecore_timer_add(0.1, _e_border_damage_wait_time_out, bdd);
       * 	}
       * else */
	{
	  XRectangle rect = { .x = 0, .y = 0, .width = 0, .height = 0 };
	  XShapeCombineMask(ecore_x_display_get(), bd->win, ShapeInput, 0, 0, 0, ShapeSet);

	  ecore_x_client_message32_send(e_manager_current_get()->root,
					ECOMORPH_ATOM_MANAGED,
					SubstructureRedirectMask | SubstructureNotifyMask,
					(unsigned int)bd->win, 
					ECOMORPH_EVENT_MAPPED, 
					ECOMORPH_WINDOW_STATE_VISIBLE, 0, 0);

	  ecore_x_damage_free(bdd->damage);
	  
	  bdd->damage = 0;
	  if (bdd->damage_timeout) ecore_timer_del(bdd->damage_timeout);
	  bdd->damage_timeout = NULL;
	  printf("e_border_damage_notify end 0x%x\n", (unsigned int) bd->win);
	  /* e_object_unref(E_OBJECT(bd)); */
	}
      
      return 0;
    }
   
  return 1;
}


static void
_e_border_wait_damage(E_Border *bd)
{
   printf("e_border_wait_damage 0x%x\n", (unsigned int) bd->win);
   Eco_Border_Data *bdd;
   
   if (!(bdd = _eco_border_data_find(bd)))
     {
	printf("- create bdd\n");
	bdd = calloc(1, sizeof(Eco_Border_Data));
	bdd->border = bd;
	eco_borders = eina_list_append(eco_borders, bdd);
     }

   if (bdd->damage) ecore_x_damage_free(bdd->damage);      
   if (bdd->damage_handler) ecore_event_handler_del(bdd->damage_handler);
   if (bdd->damage_timeout) ecore_timer_del(bdd->damage_timeout);
   bdd->damage = ecore_x_damage_new(bd->client.win, ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
   bdd->damage_handler = ecore_event_handler_add(ECORE_X_EVENT_DAMAGE_NOTIFY,
						 _e_border_cb_damage_notify, bdd);
   bdd->damage_timeout = ecore_timer_add(2.0, _e_border_damage_wait_time_out, bdd);
   
   bdd->damage_rect.x = 0xffffff;
   bdd->damage_rect.y = 0xffffff;
   bdd->damage_rect.width = 0;
   bdd->damage_rect.height = 0;   
       
   XRectangle rect = { .x = 0, .y = 0, .width = 0, .height = 0 };
   XShapeCombineRectangles(ecore_x_display_get(), bd->win, ShapeInput, 0, 0, &rect, 1, ShapeSet, Unsorted);
}


/* XXX this is not called in the case that e is 'stopping' so cleanup
   on shutdown. move border members to own struct */
static int
_eco_cb_border_remove(void *data, int ev_type, void *ev)
{
   E_Event_Border_Remove *e = ev;
   E_Border *bd = e->border;
   Eco_Border_Data *bdd;
   
   if (!(bdd = _eco_border_data_find(bd))) return 1;

   eco_borders = eina_list_remove(eco_borders, bdd);
   if (bdd->damage) ecore_x_damage_free(bdd->damage);
   if (bdd->damage_handler) ecore_event_handler_del(bdd->damage_handler);
   if (bdd->damage_timeout) ecore_timer_del(bdd->damage_timeout);
   free(bdd);
   
   return 1;
}

static int
_eco_cb_border_show(void *data, int ev_type, void *ev)
{
   E_Event_Border_Show *e = ev;
   E_Border *bd = e->border;

   _e_border_wait_damage(bd);   
}

static void
_eco_border_cb_hook_new_border(void *data, E_Border *bd)
{
   printf("window managed: 0x%x : 0x%x\n",
	  (unsigned int)bd->win,
	  (unsigned int)bd->client.win);

   ecore_x_window_prop_card32_set(bd->win, ECOMORPH_ATOM_MANAGED,
				  &(bd->client.win), 1);
 
}
#define MOD(a,b) ((a) < 0 ? ((b) - ((-(a) - 1) % (b))) - 1 : (a) % (b))

static void
_eco_border_cb_hook_pre_new_border(void *data, E_Border *bd)
{
   if (bd->new_client)
     {
	E_Zone *zone = bd->zone;
	
	/* the border was moved according to its viewport */
	printf("pre  0x%x - %d:%d \n",
	       (unsigned int) bd->win, bd->x, bd->y);
     	/* bd->x = MOD(bd->x, zone->w);
     	 * bd->y = MOD(bd->y, zone->h); */
	bd->fx.x = (bd->desk->x - zone->desk_x_current) * zone->w; 
	bd->fx.y = (bd->desk->y - zone->desk_y_current) * zone->h;
	
	bd->changes.pos = 1;
	bd->changed = 1;

	if (bd->client.netwm.state.hidden &&
	    !bd->client.netwm.state.shaded)
	  {
	     bd->visible = 1;
	     bd->iconic = 0;
	     e_border_iconify(bd);
	  }

	printf("pre  0x%x - %d:%d %d:%d\n\n",
	       (unsigned int) bd->win, bd->x, bd->y, bd->desk->x, bd->desk->y);
     }
}

static void
_eco_border_cb_hook_post_new_border(void *data, E_Border *bd)
{
  if (bd->new_client && bd->client.icccm.request_pos)
    {
      bd->x = MOD(bd->x, bd->zone->w);
      bd->y = MOD(bd->y, bd->zone->h);
    }
}

static int _eco_border_changes_title = 0;
static int _eco_border_changes_name = 0;
static int _eco_border_changes_state = 0;
static int _eco_border_changes_type = 0;

static void
_eco_border_cb_hook_pre_fetch(void *data, E_Border *bd)
{

  if (bd->client.icccm.fetch.title)
    _eco_border_changes_title = 1;
  if (bd->client.netwm.fetch.name)
    _eco_border_changes_name = 1;
  if (bd->client.netwm.fetch.state)
    _eco_border_changes_state = 1;
  if (bd->client.netwm.fetch.type)
    _eco_border_changes_type = 1;
}

static void
_eco_border_cb_hook_post_fetch(void *data, E_Border *bd)
{
   if (_eco_border_changes_title)
     {
	if(bd->client.icccm.title)
	  ecore_x_netwm_name_set(bd->win, bd->client.icccm.title);
	_eco_border_changes_title = 0;
     }

   if (_eco_border_changes_name)
     {
	if(bd->client.netwm.name)
	  ecore_x_netwm_name_set(bd->win, bd->client.netwm.name);
	_eco_border_changes_name = 0;
     }

   if (_eco_border_changes_type)
     {
       	ecore_x_netwm_window_type_set(bd->win, bd->client.netwm.type);
	_eco_border_changes_type = 0;
     }

   if (_eco_border_changes_state)
     {
	Ecore_X_Window_State state[10];
	int num = 0;

	if (bd->client.netwm.state.modal)
	  state[num++] = ECORE_X_WINDOW_STATE_MODAL;
	if (bd->client.netwm.state.sticky)
	  state[num++] = ECORE_X_WINDOW_STATE_STICKY;
	if (bd->client.netwm.state.maximized_v)
	  state[num++] = ECORE_X_WINDOW_STATE_MAXIMIZED_VERT;
	if (bd->client.netwm.state.maximized_h)
	  state[num++] = ECORE_X_WINDOW_STATE_MAXIMIZED_HORZ;
	if (bd->client.netwm.state.shaded)
	  state[num++] = ECORE_X_WINDOW_STATE_SHADED;
	if (bd->client.netwm.state.skip_taskbar)
	  state[num++] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
	if (bd->client.netwm.state.skip_pager)
	  state[num++] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
	if (bd->client.netwm.state.hidden)
	  state[num++] = ECORE_X_WINDOW_STATE_HIDDEN;
	if (bd->client.netwm.state.fullscreen)
	  state[num++] = ECORE_X_WINDOW_STATE_FULLSCREEN;

	switch (bd->client.netwm.state.stacking)
	  {
	   case E_STACKING_ABOVE:
	      state[num++] = ECORE_X_WINDOW_STATE_ABOVE;
	      break;
	   case E_STACKING_BELOW:
	      state[num++] = ECORE_X_WINDOW_STATE_BELOW;
	      break;
	   case E_STACKING_NONE:
	   default:
	      break;
	  }
	ecore_x_netwm_window_state_set(bd->win, state, num);	
	_eco_border_changes_state = 0;
     }
}
