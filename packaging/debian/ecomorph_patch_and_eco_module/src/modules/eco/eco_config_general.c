#include "e.h"
#include "eco_config.h"

/* #define NUM_PLUGINS 23 */

static Eco_Option *opt_active_plugins;
/* static int active_plugins[NUM_PLUGINS];
 * static char *plugin_table[NUM_PLUGINS] =
 *   {
 *     "ini", "inotify" /\*, "dbus"*\/, "text",
 *     "regex", "png", "decoration", "plane",
 *     "wall", "cube", "rotate", "move",
 *     "wobbly", "animation", "scale", "scaleaddon",
 *     "switch", "expo", "shift", "ring",
 *     "water", "zoom", "blur", "mblur" 
 *   }; */

#define NUM_BASE 7
static int active_base[NUM_BASE];
static char *plugin_base[NUM_BASE][2] =
  {
    {"ini",        "Ini"},
    {"inotify",    "Inotify"},
    /* {"dbus",       "DBus"}, */
    {"text",       "Text"},
    {"regex",      "Regex"},
    {"png",        "PNG"},
    {"decoration", "Decoration"},
    {"move",       "Move"}
  };

#define NUM_VIEWPORT 4
static int active_viewport[NUM_VIEWPORT];
static char *plugin_viewport[NUM_VIEWPORT][2] =
  {
    {"plane",      "Plane (immediate switch)"},
    {"wall",        "Wall (slide desks)"},
    {"cube",       "Cube (rotate desks)"},
    {"rotate",     "Rotate (required by cube)"}
  };

#define NUM_SWITCHER 4
static int active_switcher[NUM_SWITCHER];
static char *plugin_switcher[NUM_SWITCHER][2] =
  {
    {"switch",     "Simple Switcher"},
    {"shift",      "Shift Switcher"},
    {"ring",       "Ring Switcher"},
    {"scale",      "Scale Windows"}
  };

#define NUM_OTHER 10
static int active_other[NUM_OTHER];
static char *plugin_other[NUM_OTHER][2] =
  {
    {"expo",       "Expo Pager"},
    {"animation",  "Window Animations"},
    {"scaleaddon", "Scale Addons"},
    {"water",      "Water Effect"},
    {"zoom",       "Manual Zoom"},
    {"blur",       "Blur Windows"},
    {"mblur",      "Motion Blur"},
    {"wobbly",     "Wobbly Windows"},   
    {"thumbnail",  "Itask-NG Thumbnails"},
    {"reflex",     "Cube Reflexions"}
  };


/* Apply Function */
static void
_apply(E_Config_Dialog_Data *cfdata)
{
  int i;
  Eina_List *l = NULL;
  Eco_Option *plugin, *item;

  EINA_LIST_FREE(opt_active_plugins->listValue, plugin)
    {
      if (plugin->stringValue) free (plugin->stringValue);
      free(plugin);
    }
    
  for (i = 0; i < NUM_BASE; i++)
    {
      if (active_base[i])
	{
	  plugin = calloc (1, sizeof(Eco_Option));
	  plugin->stringValue = strdup(plugin_base[i][0]);
	  l = eina_list_append(l, plugin);
	}
    }
  for (i = 0; i < NUM_VIEWPORT; i++)
    {
       if (active_viewport[i])
	 {
	    plugin = calloc (1, sizeof(Eco_Option));
	    plugin->stringValue = strdup(plugin_viewport[i][0]);
	    l = eina_list_append(l, plugin);
	 }
    }
  for (i = 0; i < NUM_SWITCHER; i++)
    {
       if (active_switcher[i])
	 {
	    plugin = calloc (1, sizeof(Eco_Option));
	    plugin->stringValue = strdup(plugin_switcher[i][0]);
	    l = eina_list_append(l, plugin);
	 }
    }
  for (i = 0; i < NUM_OTHER; i++)
    {
       if (active_other[i])
	 {
	    plugin = calloc (1, sizeof(Eco_Option));
	    plugin->stringValue = strdup(plugin_other[i][0]);
	    l = eina_list_append(l, plugin);
	 }
    }

  opt_active_plugins->listValue = l;
  
  eco_config_group_apply("core");  
}


/* Main creation function */
EAPI void
eco_config_general(void *data)
{
  ECO_PAGE_BEGIN("core");
  ECO_PAGE_TABLE("Options") ;

  ECO_CREATE_CHECKBOX(0, unredirect_fullscreen_windows, "Unredirect fullscreen windows", 0, 0);
  ECO_CREATE_CHECKBOX(0, lighting, "Enable lighting", 0, 1);
  ECO_CREATE_CHECKBOX(0, sync_to_vblank, "Sync to vblank", 0, 2);
  ECO_CREATE_CHECKBOX(0, detect_refresh_rate, "Autodetect refresh rate", 0, 3);
  ECO_CREATE_SLIDER_INT(0, refresh_rate, "Refresh rate", 1, 200, "%1.0f fps", 0, 4);
  ECO_CREATE_CHECKBOX(0, detect_outputs, "Autodetect outputs", 0, 5);
  /* ECO_CREATE_ENTRY(outputs, "Outputs", 0, 6); */
  ECO_PAGE_TABLE_END;
   
  Eco_Option *plugin;
  opt_active_plugins = eco_config_option_get(cfg_display, "active_plugins");   

  for (i = 0; i < NUM_BASE;     i++) active_base[i] = 0;
  for (i = 0; i < NUM_VIEWPORT; i++) active_viewport[i] = 0;
  for (i = 0; i < NUM_SWITCHER; i++) active_switcher[i] = 0;
  for (i = 0; i < NUM_OTHER;    i++) active_other[i] = 0;

  for (l = opt_active_plugins->listValue; l; l = l->next)
    {
       plugin = l->data;
       for (i = 0; i < NUM_BASE; i++)
	 {
	    if (!strcmp(plugin->stringValue, plugin_base[i][0]))
	      active_base[i] = 1;
	 }
       for (i = 0; i < NUM_VIEWPORT; i++)
	 {
	    if (!strcmp(plugin->stringValue, plugin_viewport[i][0]))
	      active_viewport[i] = 1;
	 }
       for (i = 0; i < NUM_SWITCHER; i++)
	 {
	    if (!strcmp(plugin->stringValue, plugin_switcher[i][0]))
	      active_switcher[i] = 1;
	 }
       for (i = 0; i < NUM_OTHER; i++)
	 {
	    if (!strcmp(plugin->stringValue, plugin_other[i][0]))
	      active_other[i] = 1;
	 }
    }

  ECO_PAGE_TABLE("Base Plugins");
  int max = 3;
  for (i = 0; i < NUM_BASE; i++)
    {
      o = e_widget_check_add(cfdata->evas, plugin_base[i][1], &active_base[i]); 
      e_widget_check_checked_set(o, active_base[i]);
      e_widget_frametable_object_append(ta, o, (i / max), (i % max), 1, 1, 1, 0, 1, 0);
    }
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE("Viewport Plugins");
  max = 2;
  for (i = 0; i < NUM_VIEWPORT; i++)
    {
       o = e_widget_check_add(cfdata->evas, plugin_viewport[i][1], &active_viewport[i]); 
       e_widget_check_checked_set(o, active_viewport[i]);
       e_widget_frametable_object_append(ta, o, (i / max), (i % max), 1, 1, 1, 0, 1, 0);
    }
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE("Switcher Plugins");
  max = 2;
  for (i = 0; i < NUM_SWITCHER; i++)
    {
       o = e_widget_check_add(cfdata->evas, plugin_switcher[i][1], &active_switcher[i]); 
       e_widget_check_checked_set(o, active_switcher[i]);
       e_widget_frametable_object_append(ta, o, (i / max), (i % max), 1, 1, 1, 0, 1, 0);
    }
  ECO_PAGE_TABLE_END;

  ECO_PAGE_TABLE("Other Plugins");
  max = 3;
  for (i = 0; i < NUM_OTHER; i++)
    {
       o = e_widget_check_add(cfdata->evas, plugin_other[i][1], &active_other[i]); 
       e_widget_check_checked_set(o, active_other[i]);
       e_widget_frametable_object_append(ta, o, (i / max), (i % max), 1, 1, 1, 0, 1, 0);
    }  
  ECO_PAGE_TABLE_END;
  
  
  ECO_PAGE_END;
}

