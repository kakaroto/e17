#ifndef DRAWER_H
#define DRAWER_H

#include <e.h>

#ifndef __UNUSED__
#define __UNUSED__
#endif

#define DBG(...) EINA_LOG_DBG(__VA_ARGS__)
#define ERR(...) EINA_LOG_ERR(__VA_ARGS__)

typedef enum
{
     DRAWER_SOURCES,
     DRAWER_VIEWS,
     DRAWER_COMPOSITES
} Drawer_Plugin_Category;

typedef enum
{
     SOURCE_DATA_TYPE_OTHER = 1,
     SOURCE_DATA_TYPE_DESKTOP,
     SOURCE_DATA_TYPE_FILE_PATH
} Drawer_Source_Data_Type;

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

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;

#define DRAWER_PLUGIN(obj)	((Drawer_Plugin *) obj)
#define DRAWER_SOURCE(obj) 	((Drawer_Source *) obj)
#define DRAWER_VIEW(obj)   	((Drawer_View *) obj)
#define DRAWER_COMPOSITE(obj)   ((Drawer_Composite *) obj)
#define DRAWER_PLUGIN_API_VERSION 1
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

   struct
     {
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

   struct
     {
        /* Returns a list of Drawer_Source_Item objects */
        Eina_List * (*list)              (Drawer_Source *s);

        /* optional */

        /* Activates the given item */
        void	    (*activate)          (Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone);
        /* Triggers a plugin-specific action on gadget middle-click */
        void	    (*trigger)           (Drawer_Source *s, E_Zone *zone);
        /* Creates a context menu for the given item */
        void	    (*context)           (Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone, Drawer_Event_View_Context *ev);
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
   
   struct
     {
        /* Returns an Evas_Object from the given Drawer_Source_Items */
        Evas_Object * (*render)                 (Drawer_View *v,
                                                 Evas *evas, Eina_List *items);

        /*
         * optional
         * */
        /* Called when the container is resized by the user */
        void	    (*container_resized)	(Drawer_View *v);
        /* Sets the orientation of the gadget container */
        void	    (*orient_set)		(Drawer_View *v, E_Gadcon_Orient orient);
        /* Called when the view is shown/hidden */
        void        (*toggle_visibility)        (Drawer_View *v, Eina_Bool show);
     } func;
};

/* Composite plugin struct */
struct _Drawer_Composite
{
   Drawer_Plugin	plugin;

   struct
     {
        /* Returns an Evas_Object */
        Evas_Object * (*render)          (Drawer_Composite *c, Evas *evas);

        /* optional */
        /* Triggers a plugin-specific action on gadget middle-click */
        void	    (*trigger)           (Drawer_Composite *c, E_Zone *zone);
        /* Creates a context menu for the given item */
        void	    (*context)           (Drawer_Composite *c, E_Zone *zone, Drawer_Event_View_Context *ev);
        /* Returns a description of the source */
        const char  * (*description_get) (Drawer_Composite *c);

        /* Called when the container is resized by the user */
        void	    (*container_resized) (Drawer_Composite *c);
        /* Sets the orientation of the gadget container */
        void	    (*orient_set)	 (Drawer_Composite *c, E_Gadcon_Orient orient);
        /* Called when the view is shown/hidden */
        void        (*toggle_visibility)        (Drawer_View *v, Eina_Bool show);

        /* Creates a shelf icon */
        Evas_Object * (*get_main_icon)   (Drawer_Composite *c, Evas *evas, Evas_Coord w, Evas_Coord h);
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

/* Returns an Evas_Object icon, based on the given Drawer_Source_Item */
EAPI Evas_Object * drawer_util_icon_create(Drawer_Source_Item *si, Evas *evas, int w, int h);

/* Returns the module directory */
EAPI const char  * drawer_module_dir_get();

extern EAPI int DRAWER_EVENT_SOURCE_UPDATE;
extern EAPI int DRAWER_EVENT_SOURCE_MAIN_ICON_UPDATE;
extern EAPI int DRAWER_EVENT_VIEW_ITEM_ACTIVATE;
extern EAPI int DRAWER_EVENT_VIEW_ITEM_CONTEXT;
#endif

