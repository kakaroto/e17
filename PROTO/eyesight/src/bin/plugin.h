#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <Ecore_Data.h>
#include <Evas.h>

#define FRAMETIME (1.0/30.0)

/*
 * = STUFF FOR PLUGINS =
 */

typedef struct _Controls_Resize_Cbdata
{
   void *data;
   void (*func) (void *data, Evas *e, Evas_Object *controls, void *event_info);
} Controls_Resize_Cbdata;
 
/* 
 * _Plugin_Data struct:
 * this structure is passed to plugins to manipulate eyesight's GUI
 */
typedef struct _Plugin_Data
{
   Evas *evas;                // Main window's canvas
   Evas_Object *main_window;  // Main window's edje object
} Plugin_Data;

/* 
 * Plugin type:
 * used for keeping track of plugin data
 */
typedef struct _Plugin
{
   Ecore_Plugin *plugin;
   void *plugin_data; // A place where plugin can store its stuff
   char *file;
   char *name;
   char *version;
   char *email;
   char state;
} Plugin;

/* Plugin init() return codes */
#define PLUGIN_INIT_API_MISMATCH 1
#define PLUGIN_INIT_FAIL 2
#define PLUGIN_INIT_SUCCESS 0

/* Check this in plugin init() */
#define PLUGIN_API_VERSION 1

/*
 * Plugin identify() function:
 * This function should set the supplied pointers to the according strings. If
 * any of these strings are NULL (which is default) plugin won't be loaded.
 */
typedef void (*Plugin_Identify_Func) (char **name, char **version, char **email);

/*
 * Plugin init() function:
 * This function should check the api version and init the libs used
 * Return 0 on fail.
 */
typedef char (*Plugin_Init_Func) (void **plugin_data);

/*
 * Plugin open_file() function:
 * Open a file, load it and get it ready to show. If the file cannot be opened,
 * 0 must be returned as soon as possible, because every plugin is tested for
 * every file.
 */
typedef char (*Plugin_Open_File_Func) (void **plugin_data, char *filename,
                                       Evas_Object *main_window, Evas *evas);

/* 
 * Plugin close_file() function:
 * Close the file and free all resources allocated for it.
 */
typedef void (*Plugin_Close_File_Func) (void **plugin_data, char *filename,
                                        Evas_Object *main_window, Evas *evas);

/* 
 * Plugin show() function:
 * Show file's evas_object outside the window, then move to the center
 * with decelerating speed animation in 500 ms.
 */
typedef void (*Plugin_Show_Func) (void **plugin_data, char *filename,
                                  Evas *evas);

/*
 * Plugin hide() function:
 * Move the file's evas_object out of the window with accelerating
 * speed animation in 500 ms, then hide it.
 */
typedef void (*Plugin_Hide_Func) (void **plugin_data, char *filename,
                                  Evas_Object *main_window, Evas *evas);


#endif /*PLUGIN_H_*/
