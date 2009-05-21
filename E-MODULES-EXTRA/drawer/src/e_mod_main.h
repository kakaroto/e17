/*
 * vim:ts=8:sw=3:sts=8:et:cino=>5n-3f0^-2{2,t0,(0
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#include <e.h>

#ifndef __UNUSED__
#define __UNUSED__
#endif

/* Macros used for config file versioning */
#define MOD_CONFIG_FILE_EPOCH 0x0001
#define MOD_CONFIG_FILE_GENERATION 0x008e
#define MOD_CONFIG_FILE_VERSION \
   ((MOD_CONFIG_FILE_EPOCH << 16) | MOD_CONFIG_FILE_GENERATION)

/* Gettext: you need to use the D_ prefix for all your messages,
 * like  printf D_("Hello World\n");  so can be replaced by gettext */
#define D_(str) dgettext(PACKAGE, str)

typedef enum {
     DRAWER_SOURCES,
     DRAWER_VIEWS,
     DRAWER_COMPOSITES
} Drawer_Plugin_Category;

typedef enum {
     SOURCE_DATA_TYPE_OTHER = 1,
     SOURCE_DATA_TYPE_DESKTOP,
     SOURCE_DATA_TYPE_FILE_PATH
} Drawer_Source_Data_Type;

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

typedef struct _Drawer_Plugin Drawer_Plugin;
typedef struct _Drawer_Plugin_Api Drawer_Plugin_Api;
typedef struct _Drawer_Plugin_Type Drawer_Plugin_Type;

typedef struct _Drawer_Source Drawer_Source;
typedef struct _Drawer_View Drawer_View;
typedef struct _Drawer_Composite Drawer_Composite;

typedef struct _Drawer_Source_Item Drawer_Source_Item;

typedef struct _Drawer_Event_Source_Update Drawer_Event_Source_Update;
typedef struct _Drawer_Event_Source_Main_Icon_Update Drawer_Event_Source_Main_Icon_Update;
typedef struct _Drawer_Event_View_Activate Drawer_Event_View_Activate;
typedef struct _Drawer_Event_View_Context Drawer_Event_View_Context;

typedef struct _Drawer_Content_Margin Drawer_Content_Margin;

#define DRAWER_PLUGIN(obj)	((Drawer_Plugin *) obj)
#define DRAWER_SOURCE(obj) 	((Drawer_Source *) obj)
#define DRAWER_VIEW(obj)   	((Drawer_View *) obj)
#define DRAWER_COMPOSITE(obj)   ((Drawer_Composite *) obj)
#define DRAWER_PLUGIN_API_VERSION 1
/* Base config struct. Store Item Count, etc
 * 
 * *module (not written to disk) (E Interaction)
 * *cfd (not written to disk) (config dialog)
 * 
 * Store list of your items that you want to keep. (sorting)
 * Can define per-module config properties here.
 * 
 * Version used to know when user config too old */
struct _Config 
{
   E_Module *module;
   E_Config_Dialog *cfd;

   Eina_List *conf_items;

   /* config file version */
   int version;
};

/* This struct used to hold config for individual items from above list */
struct _Config_Item 
{
   /* unique id */
   const char *id;

   /* Source and view plugin names */
   const char *source;
   const char *view;
   const char *composite;
};

/* Generic plugin API */
struct _Drawer_Plugin_Api
{
   int         version;
   const char *name;
};

struct _Drawer_Plugin_Type
{
   const char *name, *title, *comment;
};

/* Generic plugin struct */
struct _Drawer_Plugin
{
   Drawer_Plugin_Api   *api;
   
   const char          *name;
   const char          *dir;
   void                *handle;
   
   Eina_Bool            enabled : 1;
   Eina_Bool            error : 1;

   struct {
      void        * (*init)            (Drawer_Plugin *p, const char *id);
      int           (*shutdown)        (Drawer_Plugin *p);

      /* optional */
      Evas_Object * (*config_get)  (Drawer_Plugin *p, Evas *evas);
      void	    (*config_save) (Drawer_Plugin *p);
   } func;

   /* the module is allowed to modify these */
   void                *data;
};

/* Source plugin struct */
struct _Drawer_Source
{
   Drawer_Plugin	plugin;

   struct {
      Eina_List * (*list)        (Drawer_Source *s);

      /* optional */

      /* Activates the given item */
      void	    (*activate)        (Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone);
      /* Triggers a plugin-specific action on gadget middle-click */
      void	    (*trigger)         (Drawer_Source *s, E_Zone *zone);
      /* Creates a context menu for the given item */
      void	    (*context)         (Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone, Drawer_Event_View_Context *ev);
      /* Returns a description of the source */
      const char  * (*description_get) (Drawer_Source *s);
      /* Returns an icon for the given source item */
      Evas_Object * (*render_item)     (Drawer_Source *s, Drawer_Source_Item *si, Evas *evas);
   } func;
};

/* View plugin struct */
struct _Drawer_View
{
   Drawer_Plugin	plugin;
   
   struct {
      Evas_Object * (*render)      (Drawer_View *v, Evas *evas, Eina_List *items);

      /* optional */
      /* Calculates the size of the content */
      void	    (*content_size_get)		(Drawer_View *v, E_Gadcon_Client *gcc,
                                                 Drawer_Content_Margin *margin, int *w, int *h);
      /* Called when the container is resized based on the previously calculated size, for further tweaking of the content */
      void	    (*container_resized)	(Drawer_View *v);
      /* Sets the orientation of the gadget container */
      void	    (*orient_set)		(Drawer_View *v, E_Gadcon_Orient orient);
   } func;
};

/* Composite plugin struct */
struct _Drawer_Composite
{
   Drawer_Plugin	plugin;

   struct {
      Evas_Object * (*render)          (Drawer_Composite *c, Evas *evas);

      /* optional */
      /* Activates the given item */
      void	    (*activate)        (Drawer_Composite *c, E_Zone *zone);
      /* Triggers a plugin-specific action on gadget middle-click */
      void	    (*trigger)         (Drawer_Composite *c, E_Zone *zone);
      /* Creates a context menu for the given item */
      void	    (*context)         (Drawer_Composite *c, E_Zone *zone, Drawer_Event_View_Context *ev);
      /* Returns a description of the source */
      const char  * (*description_get) (Drawer_Composite *c);

      /* Calculates the size of the content */
      void	    (*content_size_get)	 (Drawer_Composite *c, E_Gadcon_Client *gcc,
                                          Drawer_Content_Margin *margin, int *w, int *h);
      /* Called when the container is resized based on the previously calculated size, for further tweaking of the content */
      void	    (*container_resized) (Drawer_Composite *c);
      /* Sets the orientation of the gadget container */
      void	    (*orient_set)	 (Drawer_Composite *c, E_Gadcon_Orient orient);

      /* Creates a shelf icon */
      Evas_Object * (*get_main_icon)    (Drawer_Composite *c, Evas *evas, Evas_Coord w, Evas_Coord h);
   } func;
};

/* Struct for each list item from the source */
struct _Drawer_Source_Item
{
   void				*data;
   Drawer_Source_Data_Type	 data_type;

   const char     *label;
   const char     *description;
   const char	  *category;
   /* For short couple of characters info */
   const char	  *info;

   Drawer_Source  *source;
   void		  *priv;
};

struct _Drawer_Event_Source_Update
{
   Drawer_Source *source;
   const char *id;
};

struct _Drawer_Event_Source_Main_Icon_Update
{
   Drawer_Source *source;
   Drawer_Source_Item *si;
   const char *id;
};

struct _Drawer_Event_View_Activate
{
   Drawer_View *view;
   const char *id;
   void *data;
};

struct _Drawer_Event_View_Context
{
   Drawer_View *view;
   const char *id;
   void *data;
   Evas_Coord x, y;
};

struct _Drawer_Content_Margin
{
   Evas_Coord top, right, bottom, left;
};

/* Setup the E Module Version, Needed to check if module can run. */
EAPI extern E_Module_Api e_modapi;

/* E API Module Interface Declarations
 * 
 * Need to initialize, shutdown, save the module */
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

EAPI Eina_List * drawer_plugins_list(Drawer_Plugin_Category cat);
EAPI void drawer_plugins_list_free(Eina_List *list);
EAPI Drawer_Plugin * drawer_plugin_load(Config_Item *ci, Drawer_Plugin_Category cat, const char *name);
EAPI Evas_Object * drawer_plugin_config_button_get(Config_Item *ci, Evas *evas, Drawer_Plugin_Category cat);

EAPI Evas_Object * drawer_util_icon_create(Drawer_Source_Item *si, Evas *evas, int w, int h);

/* Function for calling the modules config dialog */
EAPI E_Config_Dialog *e_int_config_drawer_module(E_Container *con, Config_Item *ci);

extern Config *drawer_conf;
extern EAPI int DRAWER_EVENT_SOURCE_UPDATE;
extern EAPI int DRAWER_EVENT_SOURCE_MAIN_ICON_UPDATE;
extern EAPI int DRAWER_EVENT_VIEW_ITEM_ACTIVATE;
extern EAPI int DRAWER_EVENT_VIEW_ITEM_CONTEXT;

#endif
