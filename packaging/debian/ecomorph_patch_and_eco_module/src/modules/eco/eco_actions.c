#include "e.h"
#include "eco_config.h"
#include <X11/Xlib.h>

static void _eco_cb_key_action(E_Object *obj, const char *params, Ecore_Event_Key *ev);
static void _eco_cb_mouse_action(E_Object *obj, const char *params, Ecore_Event_Mouse_Button *ev);
static void _eco_cb_mouse_wheel_action(E_Object *obj, const char *params, Ecore_Event_Mouse_Wheel *ev);
static void _eco_cb_action(E_Object *obj, const char *params, int modifiers);

static void _eco_modifiers_set(int mod);
static int  _eco_cb_key_down(void *data, int type, void *event);
static int  _eco_cb_key_up(void *data, int type, void *event);
static int  _eco_cb_mouse_wheel(void *data, int type, void *event);
static int  _eco_cb_mouse_move(void *data, int type, void *event);
static int  _eco_cb_mouse_down(void *data, int type, void *event);
static int  _eco_cb_mouse_up(void *data, int type, void *event);


typedef struct _Eco_Action Eco_Action;
struct _Eco_Action
{
  int toggle;
  int plugin;
  int action;
  int option;
  int option2;

  int terminate_edge;
  
  int ignore_mouse_event;

  double time_start;
};

static Eco_Action eco_action;
static Eina_List *act_handlers = NULL;
static Ecore_X_Window input_window = 0;
static int hold_count = 0;
static int hold_mod = 0;
static Ecore_X_Atom ECOMORPH_ATOM_PLUGIN = 0;

static void
_eco_modifiers_set(int mod)
{
   if (!act_handlers) return;
   hold_mod = mod;
   hold_count = 0;
   if (hold_mod & ECORE_EVENT_MODIFIER_SHIFT) hold_count++;
   if (hold_mod & ECORE_EVENT_MODIFIER_CTRL) hold_count++;
   if (hold_mod & ECORE_EVENT_MODIFIER_ALT) hold_count++;
   if (hold_mod & ECORE_EVENT_MODIFIER_WIN) hold_count++;
}



#define ECO_PLUGIN_SCALE   0
#define ECO_PLUGIN_SWITCH  1
#define ECO_PLUGIN_EXPO    2
#define ECO_PLUGIN_SHIFT   3
#define ECO_PLUGIN_RING    4
#define ECO_PLUGIN_ROTATE  5
#define ECO_PLUGIN_WATER   6
#define ECO_PLUGIN_ZOOM    7
#define ECO_PLUGIN_OPACITY 8

#define ECO_ACT_INITIATE   0
#define ECO_ACT_TERMINATE  1
#define ECO_ACT_CYCLE      2
#define ECO_ACT_MOUSE_MOVE 3
#define ECO_ACT_MOUSE_DOWN 4
#define ECO_ACT_MOUSE_UP   5

#define ECO_ACT_SEND_TO_CURRENT_DESK 6 

#define ECO_ACT_OPT_INITIATE       0
#define ECO_ACT_OPT_INITIATE_ALL   1
#define ECO_ACT_OPT_INITIATE_GROUP 2

#define ECO_ACT_OPT_TERMINATE        0
#define ECO_ACT_OPT_TERMINATE_CANCEL 1

#define ECO_ACT_OPT_CYCLE_NEXT  1
#define ECO_ACT_OPT_CYCLE_PREV  2
#define ECO_ACT_OPT_CYCLE_LEFT  3
#define ECO_ACT_OPT_CYCLE_RIGHT 4
#define ECO_ACT_OPT_CYCLE_UP    5
#define ECO_ACT_OPT_CYCLE_DOWN  6

static char *eco_plugin_table[9] =
{
    "scale", "switch", "expo",
    "shift", "ring", "rotate",
    "water", "zoom", "opcacity"
};
   

static void
_eco_plugin_message_send(void)
{
     ecore_x_client_message32_send(e_manager_current_get()->root,
				   ECOMORPH_ATOM_PLUGIN,
				   SubstructureRedirectMask | SubstructureNotifyMask,
				   e_manager_current_get()->root, // better send
								  // active window
				   eco_action.plugin,
				   eco_action.action,
				   eco_action.option,
				   eco_action.option2);
}


EAPI void
eco_action_terminate(void) /* TODO add arg if message should be send
			      or if we just update state from ecomp */
{
   if (act_handlers)
     {
	hold_count = 0;
	hold_mod = 0;
	while (act_handlers)
	  {
	     ecore_event_handler_del(act_handlers->data);
	     act_handlers = eina_list_remove_list(act_handlers, act_handlers);
	  }
	e_grabinput_release(input_window, input_window);/// XXX before
							/// or after >>?
	ecore_x_window_free(input_window);

	input_window = 0;

	eco_action.ignore_mouse_event = 0;
	eco_action.terminate_edge = 0;
	
	eco_action.action = ECO_ACT_TERMINATE;
	eco_action.option = ECO_ACT_OPT_TERMINATE;
     }
}

/* when a plugin is in active state we filter all key events 
   input_window has grabbed the keybord. so all key events go
   through this function */
static int
_eco_cb_key_down(void *data, int type, void *event)
{
   Ecore_Event_Key *ev = event;
   Eina_List *l;
   E_Config_Binding_Key *bind;
   E_Binding_Modifier mod = 0;
   int plugin, action, option, option2, toggle;

   if (ev->event_window != input_window) return 1;

   if (!strcmp(ev->key, "Escape"))
     {
	if (act_handlers)
	  {
	     eco_action_terminate();
	     eco_action.option = ECO_ACT_OPT_TERMINATE_CANCEL;
	     _eco_plugin_message_send();
	     return 1;
	  }
     }
   else
     {
	for (l = e_config->key_bindings; l; l = l->next)
	  {
	     bind = l->data;
	     
	     if (!bind->key) continue;
	     if (!bind->action) continue;
	     if (!bind->params) continue;

	     if (ev->modifiers & ECORE_EVENT_MODIFIER_SHIFT) mod |= E_BINDING_MODIFIER_SHIFT;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_CTRL) mod |= E_BINDING_MODIFIER_CTRL;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_ALT) mod |= E_BINDING_MODIFIER_ALT;
	     if (ev->modifiers & ECORE_EVENT_MODIFIER_WIN) mod |= E_BINDING_MODIFIER_WIN;

	     if ((bind->modifiers == mod) &&
		 (!strcmp(bind->key, ev->key)) &&
		 (!strncmp(bind->action,"Eco_", 4)))
	       {
		  if (sscanf(bind->params,"%d %d %d %d %d",
			     &toggle, &plugin, &action, &option, &option2) == 5)
		    {
		       if (plugin == eco_action.plugin)
			 {
			    if (eco_action.toggle && toggle)
			      {
				 /* XXX delay this until term notify from
				    plugin !!!*/
				 
				 eco_action_terminate(); 
				 eco_action.action = ECO_ACT_TERMINATE;
				 eco_action.option = ECO_ACT_OPT_TERMINATE;
				 _eco_plugin_message_send();
			      }
			    else
			      {
				 eco_action.action = action;
				 eco_action.option = option;
				 eco_action.option2 = option2;
				 _eco_plugin_message_send();
			      }
			    return 1;
			 }
		    }
	       }
	  }
     }
   /* TODO make this an option */
   if (!strcmp(ev->key, "n"))
     {
	eco_action.action = ECO_ACT_CYCLE;
	eco_action.option2 = ECO_ACT_OPT_CYCLE_NEXT;
	_eco_plugin_message_send();
     }
   else if (!strcmp(ev->key, "p"))
     {
	eco_action.action = ECO_ACT_CYCLE;
	eco_action.option2 = ECO_ACT_OPT_CYCLE_PREV;
	_eco_plugin_message_send();
     }
   else if (!strcmp(ev->key, "Up"))
     {
	eco_action.action = ECO_ACT_CYCLE;
	eco_action.option2 = ECO_ACT_OPT_CYCLE_PREV;
	_eco_plugin_message_send();
     }
   else if (!strcmp(ev->key, "Down"))
     {
	eco_action.action = ECO_ACT_CYCLE;
	eco_action.option2 = ECO_ACT_OPT_CYCLE_NEXT;
	_eco_plugin_message_send();
     }
   else if (!strcmp(ev->key, "Return"))
     {
	eco_action_terminate();
	_eco_plugin_message_send();
     }
   return 1;
}


static int
_eco_cb_mouse_wheel(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Wheel *ev = event;

   eco_action.action = ECO_ACT_CYCLE;
   
   if (ev->direction > 0)
     eco_action.option2 = ECO_ACT_OPT_CYCLE_NEXT;
   else
     eco_action.option2 = ECO_ACT_OPT_CYCLE_PREV;

   _eco_plugin_message_send();

   return 1;
}


static int _edge_enable_timer_cb(void *data)
{
   e_zone_flip_win_restore();
   return 0;
}


static int
_eco_cb_mouse_move(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Move *ev = event;

   if ((eco_action.toggle) &&
       (eco_action.terminate_edge) &&
       (ecore_time_get() > (eco_action.time_start + 0.3)))
     {
	E_Zone *zone = e_util_zone_current_get(e_manager_current_get()); 
	int end = 0;
	
	switch(eco_action.terminate_edge)
	  {
	   case E_ZONE_EDGE_TOP_LEFT:
	      if ((ev->x == 0) && (ev->y == 0))
		{
		   ecore_x_pointer_warp(zone->container->win, 10, 10); 
		   end = 1;
		}
	      break;
	   case E_ZONE_EDGE_BOTTOM_LEFT:
	      if ((ev->x == 0) && (ev->y == zone->h - 1))
		{
		   ecore_x_pointer_warp(zone->container->win, 10, zone->h - 10); 
		   end = 1;
		}
	      break;
	   case E_ZONE_EDGE_TOP_RIGHT:
	      if ((ev->x == zone->w - 1) && (ev->y == 0))
		{
		   ecore_x_pointer_warp(zone->container->win, zone->w - 10, 10); 
		   end = 1;
		}
	      break;
	   case E_ZONE_EDGE_BOTTOM_RIGHT:
	      if ((ev->x == zone->w - 1) && (ev->y == zone->h - 1))
		{
		   ecore_x_pointer_warp(zone->container->win, zone->w - 10, zone->h - 10); 
		   end = 1;
		}
	      break;
	   default:
	      break;
	  }

	if (end)
	  {
	     e_zone_flip_win_disable();
	     eco_action_terminate();
	     eco_action.option = ECO_ACT_OPT_TERMINATE_CANCEL;
	     _eco_plugin_message_send();
	     /* XXX */
	     ecore_timer_add(0.3, _edge_enable_timer_cb, NULL);
	     return 1;
	  }
     }
   

   eco_action.action  = ECO_ACT_MOUSE_MOVE;
   eco_action.option  = ev->x;
   eco_action.option2 = ev->y;
   
   _eco_plugin_message_send();
   
   return 1;
}


static int
_eco_cb_mouse_down(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;

   if(eco_action.ignore_mouse_event)
     {

	// of the mouse event, so we will receive the
	// initiating mouse event a second time. this
	// would brake toggles... 
	  eco_action.ignore_mouse_event--;
     }
   else
     {
	eco_action.action = ECO_ACT_MOUSE_DOWN;
	eco_action.option  = ev->buttons | (ev->double_click << 4);
	eco_action.option2 = (ev->x << 16) | (ev->y & 0x000fff);
	
	_eco_plugin_message_send();
     }
   
   return 1;
}


static int
_eco_cb_mouse_up(void *data, int type, void *event)
{
   Ecore_Event_Mouse_Button *ev = event;
   if(eco_action.ignore_mouse_event)
     {
	eco_action.ignore_mouse_event--;
     }
   else
     {
	eco_action.action  = ECO_ACT_MOUSE_UP;
	eco_action.option  = ev->buttons;
	eco_action.option2 = (ev->x << 16) | (ev->y & 0x000fff);
   
	_eco_plugin_message_send();
     }
   
   return 1;
}


static int
_eco_cb_key_up(void *data, int type, void *event)
{
   Ecore_Event_Key *ev = event;
   /* printf("key up\n"); */
   
   if (!(act_handlers)) return 1;

   if (hold_mod && !eco_action.toggle)
   {
      if      ((hold_mod & ECORE_EVENT_MODIFIER_SHIFT) && (!strcmp(ev->key, "Shift_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_SHIFT) && (!strcmp(ev->key, "Shift_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_CTRL) && (!strcmp(ev->key, "Control_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_CTRL) && (!strcmp(ev->key, "Control_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Alt_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "ISO_Prev_Group"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Alt_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Meta_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Meta_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Super_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_ALT) && (!strcmp(ev->key, "Super_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_WIN) && (!strcmp(ev->key, "Super_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_WIN) && (!strcmp(ev->key, "Super_R"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_WIN) && (!strcmp(ev->key, "Mode_switch"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_WIN) && (!strcmp(ev->key, "Meta_L"))) hold_count--;
      else if ((hold_mod & ECORE_EVENT_MODIFIER_WIN) && (!strcmp(ev->key, "Meta_R"))) hold_count--;

      if (hold_count <= 0)
      {
	/* printf("key up terminate\n"); */
	 eco_action_terminate();
         _eco_plugin_message_send();
      }
   }
  return 1;
}


static void
_eco_cb_go_action(E_Object *obj, const char *params)
{
   Eina_List *l;
   E_Config_Binding_Edge *bi;
   /* printf("go action\n"); */   
   for (l = e_config->edge_bindings; l; l = l->next)
     {
	bi = l->data;
	if (bi->params && !strcmp(params, bi->params))
	  {
	     eco_action.terminate_edge = bi->edge;
	     break;
	  }
     }

   _eco_cb_action(obj, params, 0);
}


static void
_eco_cb_key_action(E_Object *obj, const char *params, Ecore_Event_Key *ev)
{
  /* printf("key action\n"); */
   _eco_cb_action(obj, params, ev->modifiers);
}


static void 
_eco_cb_mouse_action(E_Object *obj, const char *params, Ecore_Event_Mouse_Button *ev)
{
  /* printf("mouse action\n"); */
   _eco_cb_action(obj, params, ev->modifiers);
   eco_action.ignore_mouse_event = 2;
}


static int
_eco_grab_window(int modifiers)
{
   ecore_x_keyboard_ungrab();
   ecore_x_pointer_ungrab();
   ecore_x_sync();
   
   E_Zone *zone = e_util_zone_current_get(e_manager_current_get());
   input_window = ecore_x_window_input_new(zone->container->win, 0, 0, 1, 1);
   ecore_x_window_show(input_window);

   if (!e_grabinput_get(input_window, 0, input_window))
     {
	ecore_x_window_free(input_window);
	input_window = 0;
	return 0;
     }
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_KEY_DOWN, _eco_cb_key_down, NULL));
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_KEY_UP, _eco_cb_key_up, NULL));
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_MOVE, _eco_cb_mouse_move, NULL));
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_BUTTON_DOWN, _eco_cb_mouse_down, NULL));
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_BUTTON_UP, _eco_cb_mouse_up, NULL));
   act_handlers = eina_list_append
     (act_handlers, ecore_event_handler_add
      (ECORE_EVENT_MOUSE_WHEEL, _eco_cb_mouse_wheel, NULL));

   /* Remember current modifiers */
   _eco_modifiers_set(modifiers);
   
   return 1;
}


static void 
_eco_cb_action(E_Object *obj, const char *params, int modifiers)
{
   if (sscanf(params, "%d %d %d %d %d",
	      &eco_action.toggle,
	      &eco_action.plugin,
	      &eco_action.action,
	      &eco_action.option,
	      &eco_action.option2) == 5)
     {
	//eco_plugin_is_loaded(eco_plugin_table[eco_action.plugin]) && 
       if ((eco_action.toggle == 2) || _eco_grab_window(modifiers))
	  {
	     _eco_plugin_message_send();
	     eco_action.time_start = ecore_time_get();

	     /* update ecomp pointer */
	     eco_action.action = ECO_ACT_MOUSE_MOVE;
	     int x, y;
	     ecore_x_pointer_xy_get(e_manager_current_get()->root, &x, &y);
	     eco_action.option = x;
	     eco_action.option2 = y;
	     _eco_plugin_message_send();
	  }
     }
}


EAPI char*
eco_get_bind_text(const char* action)
{
   E_Binding_Key *bind;
   char b[256] = "";

   bind = e_bindings_key_get(action);
   if ((bind) && (bind->key))
   {
      if ((bind->mod) & (E_BINDING_MODIFIER_CTRL))
         strcat(b, _("CTRL"));

      if ((bind->mod) & (E_BINDING_MODIFIER_ALT))
      {
         if (b[0]) strcat(b, " + ");
         strcat(b, _("ALT"));
      }

      if ((bind->mod) & (E_BINDING_MODIFIER_SHIFT))
      {
         if (b[0]) strcat(b, " + ");
         strcat(b, _("SHIFT"));
      }

      if ((bind->mod) & (E_BINDING_MODIFIER_WIN))
      {
         if (b[0]) strcat(b, " + ");
         strcat(b, _("WIN"));
      }

      if ((bind->key) && (bind->key[0]))
      {
         char *l;

         if (b[0]) strcat(b, " + ");
         l = strdup(bind->key);
         l[0] = (char)toupper(bind->key[0]);
         strcat(b, l);
         free(l);
      }
      return strdup(b);
   }
   return NULL;
}



#define ECO_ACTION_NEW(NAME, CAT, LABEL, PARAM) \
   if ((action = e_action_add(NAME))) { \
      action->func.go = _eco_cb_go_action; \
      action->func.go_key = _eco_cb_key_action; \
      action->func.go_mouse = _eco_cb_mouse_action; \
      e_action_predef_name_set(CAT, LABEL, NAME, PARAM, NULL, 0); }
      //action->func.go_wheel = _eco_cb_mouse_wheel_action; \

#define ECO_ACTION_FREE(NAME, CAT, LABEL) \
   e_action_del(NAME); \
   e_action_predef_name_del(CAT, LABEL);


#define ECO_ACT_NONTOGGLE 0
#define ECO_ACT_TOGGLE 1

#define ECO_PLUGIN_SCALE 0
// <toggle> <plugin> <action> <option> <option2>
#define SCALE_INITIATE		"1 0 0 0 0"
#define SCALE_INITIATE_ALL	"1 0 0 1 0"
#define SCALE_NEXT		"0 0 2 0 1"
#define SCALE_PREV		"0 0 2 0 2"
#define SCALE_NEXT_ALL		"0 0 2 1 1"
#define SCALE_PREV_ALL		"0 0 2 1 2"

#define SWITCHER_NEXT		"0 1 2 0 1"
#define SWITCHER_PREV		"0 1 2 0 2"
#define SWITCHER_NEXT_ALL	"0 1 2 1 1"
#define SWITCHER_PREV_ALL	"0 1 2 1 2"

#define EXPO_INITIATE		"1 2 0 0 0"
#define EXPO_NEXT		"0 2 2 0 1"
#define EXPO_PREV		"0 2 2 0 2"

#define SHIFT_INITIATE		"1 3 0 0 0"
#define SHIFT_INITIATE_ALL	"1 3 0 1 0"
#define SHIFT_NEXT		"0 3 2 0 1"
#define SHIFT_PREV		"0 3 2 0 2"
#define SHIFT_NEXT_ALL		"0 3 2 1 1"
#define SHIFT_PREV_ALL		"0 3 2 1 2"

#define RING_INITIATE		"1 4 0 0 0"
#define RING_INITIATE_ALL	"1 4 0 1 0"
#define RING_NEXT		"0 4 2 0 1"
#define RING_PREV		"0 4 2 0 2"
#define RING_NEXT_ALL		"0 4 2 1 1"
#define RING_PREV_ALL		"0 4 2 1 2"

#define ROTATE_INITIATE_TOGGLE	"1 5 0 0 0"
#define ROTATE_INITIATE		"0 5 0 0 0"

#define ZOOM_INITIATE   	"1 6 0 0 0"

#define OPACITY_INCREASE        "2 8 0 0 1"
#define OPACITY_DECREASE   	"2 8 0 0 2"

EAPI void
eco_actions_create(void)
{

   ECOMORPH_ATOM_PLUGIN = ecore_x_atom_get("__ECOMORPH_PLUGIN");   
   
   
   E_Action *action;

   if (action = e_action_add("Eco_Custom_Action"))
     { 
	action->func.go = _eco_cb_go_action;
	action->func.go_key = _eco_cb_key_action;
	action->func.go_mouse = _eco_cb_mouse_action;
	e_action_predef_name_set("Ecomorph", "Custom Action", "Eco_Custom_Action", NULL,
				 "<toggle=1|0> <plugin> <action> <option1> <option2>", 1);
     }

   
   // Expo
   ECO_ACTION_NEW("Eco_Expo_Initiate", "Ecomorph", "Expo Initiate", EXPO_INITIATE);
   ECO_ACTION_NEW("Eco_Expo_Next", "Ecomorph", "Expo Next", EXPO_NEXT);
   ECO_ACTION_NEW("Eco_Expo_Prev", "Ecomorph", "Expo Prev", EXPO_PREV);

   // Switcher
   ECO_ACTION_NEW("Eco_Switcher_Next", "Ecomorph", "Switcher Next Window", SWITCHER_NEXT);
   ECO_ACTION_NEW("Eco_Switcher_Prev", "Ecomorph", "Switcher Prev Window", SWITCHER_PREV);
   ECO_ACTION_NEW("Eco_Switcher_Next_All", "Ecomorph", "Switcher Next All", SWITCHER_NEXT_ALL);
   ECO_ACTION_NEW("Eco_Switcher_Prev_All", "Ecomorph", "Switcher Prev All", SWITCHER_PREV_ALL);

   // Water
   /* ECO_ACTION_NEW("Eco_Water_Touch", "Ecomorph", "Water Touch", "0 water initiate");
    * ECO_ACTION_NEW("Eco_Water_Rain", "Ecomorph", "Water Toggle Rain", "1 water toggle_rain");
    * ECO_ACTION_NEW("Eco_Water_Title_Wave", "Ecomorph", "Water Title Wave", "1 water title_wave"); */

   // Scale
   ECO_ACTION_NEW("Eco_Scale_Initiate", "Ecomorph", "Scale Initiate", SCALE_INITIATE);
   ECO_ACTION_NEW("Eco_Scale_Initiate_All", "Ecomorph", "Scale Initiate All", SCALE_INITIATE_ALL);
   ECO_ACTION_NEW("Eco_Scale_Next", "Ecomorph", "Scale Next Window", SCALE_NEXT);
   ECO_ACTION_NEW("Eco_Scale_Prev", "Ecomorph", "Scale Prev Window", SCALE_PREV);
   ECO_ACTION_NEW("Eco_Scale_Next_All", "Ecomorph", "Scale Next All", SCALE_NEXT_ALL);
   ECO_ACTION_NEW("Eco_Scale_Prev_All", "Ecomorph", "Scale Prev All", SCALE_PREV_ALL);

   // Ring   
   ECO_ACTION_NEW("Eco_Ring_Initiate", "Ecomorph", "Ring Initiate", RING_INITIATE);
   ECO_ACTION_NEW("Eco_Ring_Initiate_All", "Ecomorph", "Ring Initiate All", RING_INITIATE_ALL);
   ECO_ACTION_NEW("Eco_Ring_Next", "Ecomorph", "Ring Next Window", RING_NEXT);
   ECO_ACTION_NEW("Eco_Ring_Prev", "Ecomorph", "Ring Prev Window", RING_PREV);
   ECO_ACTION_NEW("Eco_Ring_Next_All", "Ecomorph", "Ring Next All", RING_NEXT_ALL);
   ECO_ACTION_NEW("Eco_Ring_Prev_All", "Ecomorph", "Ring Prev All", RING_PREV_ALL);

   // Shift
   ECO_ACTION_NEW("Eco_Shift_Initiate", "Ecomorph", "Shift Initiate", SHIFT_INITIATE);
   ECO_ACTION_NEW("Eco_Shift_Initiate_All", "Ecomorph", "Shift Initiate All", SHIFT_INITIATE_ALL);
   ECO_ACTION_NEW("Eco_Shift_Next", "Ecomorph", "Shift Next Window", SHIFT_NEXT);
   ECO_ACTION_NEW("Eco_Shift_Prev", "Ecomorph", "Shift Prev Window", SHIFT_PREV);
   ECO_ACTION_NEW("Eco_Shift_Next_All", "Ecomorph", "Shift Next All", SHIFT_NEXT_ALL);
   ECO_ACTION_NEW("Eco_Shift_Prev_All", "Ecomorph", "Shift Prev All", SHIFT_PREV_ALL);

   // Rotate Cube
   /* ECO_ACTION_NEW("Eco_Rotate_Initiate", "Ecomorph", "Rotate Cube", ROTATE_INITIATE);
    * ECO_ACTION_NEW("Eco_Rotate_Initiate_Toggle", "Ecomorph", "Rotate Cube Toggle", ROTATE_INITIATE_TOGGLE); */

   ECO_ACTION_NEW("Eco_Opacity_Increase", "Ecomorph", "Increase Window Opacity", OPACITY_INCREASE);
   ECO_ACTION_NEW("Eco_Opacity_Decrease", "Ecomorph", "Decrease Window Opacity", OPACITY_DECREASE);
  
}

EAPI void
eco_actions_free(void)
{
   ECO_ACTION_FREE("Eco_Custom_Action", "Ecomorph", "Custom Action");
   
   // Expo
   ECO_ACTION_FREE("Eco_Expo_Initiate", "Ecomorph", "Expo Initiate");
   ECO_ACTION_FREE("Eco_Expo_Next", "Ecomorph", "Expo Next");
   ECO_ACTION_FREE("Eco_Expo_Prev", "Ecomorph", "Expo Prev");

   // Switcher
   ECO_ACTION_FREE("Eco_Switcher_Next", "Ecomorph", "Switcher Next window");
   ECO_ACTION_FREE("Eco_Switcher_Prev", "Ecomorph", "Switcher Prev window");
   ECO_ACTION_FREE("Eco_Switcher_Next_All", "Ecomorph", "Switcher Next All");
   ECO_ACTION_FREE("Eco_Switcher_Prev_All", "Ecomorph", "Switcher Prev All");

   // Water
   /* ECO_ACTION_FREE("Eco_Water_Touch", "Ecomorph", "Water Touch");
    * ECO_ACTION_FREE("Eco_Water_Rain", "Ecomorph", "Water Toggle Rain");
    * ECO_ACTION_FREE("Eco_Water_Title_Wave", "Ecomorph", "Water Title Wave"); */

   // Scale
   ECO_ACTION_FREE("Eco_Scale_Initiate", "Ecomorph", "Scale Initiate");
   ECO_ACTION_FREE("Eco_Scale_Initiate_All", "Ecomorph", "Scale Initiate All");
   ECO_ACTION_FREE("Eco_Scale_Next", "Ecomorph", "Scale Next");
   ECO_ACTION_FREE("Eco_Scale_Prev", "Ecomorph", "Scale Prev");
   ECO_ACTION_FREE("Eco_Scale_Next_All", "Ecomorph", "Scale Next All");
   ECO_ACTION_FREE("Eco_Scale_Prev_All", "Ecomorph", "Scale Prev All");

   // Ring
   ECO_ACTION_FREE("Eco_Ring_Initiate", "Ecomorph", "Ring Initiate");
   ECO_ACTION_FREE("Eco_Ring_Initiate_All", "Ecomorph", "Ring Initiate All");
   ECO_ACTION_FREE("Eco_Ring_Next", "Ecomorph", "Ring Next");
   ECO_ACTION_FREE("Eco_Ring_Prev", "Ecomorph", "Ring Prev");
   ECO_ACTION_FREE("Eco_Ring_Next_All", "Ecomorph", "Ring Next All");
   ECO_ACTION_FREE("Eco_Ring_Prev_All", "Ecomorph", "Ring Prev All");

   // Shift
   ECO_ACTION_FREE("Eco_Shift_Initiate", "Ecomorph", "Shift Initiate");
   ECO_ACTION_FREE("Eco_Shift_Initiate_All", "Ecomorph", "Shift Initiate All");
   ECO_ACTION_FREE("Eco_Shift_Next", "Ecomorph", "Shift Next");
   ECO_ACTION_FREE("Eco_Shift_Prev", "Ecomorph", "Shift Prev");
   ECO_ACTION_FREE("Eco_Shift_Next_All", "Ecomorph", "Shift Next All");
   ECO_ACTION_FREE("Eco_Shift_Prev_All", "Ecomorph", "Shift Prev All");
   
   // Rotate Cube
   /* ECO_ACTION_FREE("Eco_Rotate_Initiate", "Ecomorph", "Rotate Cube");
    * ECO_ACTION_FREE("Eco_Rotate_Initiate_Toggle", "Ecomorph", "Rotate Cube Toggle"); */

   ECO_ACTION_FREE("Eco_Opacity_Increase", "Ecomorph", "Increase Window Opacity");
   ECO_ACTION_FREE("Eco_Opacity_Decrease", "Ecomorph", "Decrease Window Opacity");
   
}

