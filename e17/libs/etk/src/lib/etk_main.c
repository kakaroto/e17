/** @file etk_main.c */
#include "etk_main.h"
#include <locale.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Job.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_type.h"
#include "etk_signal.h"
#include "etk_object.h"
#include "etk_toplevel_widget.h"
#include "etk_utils.h"
#include "etk_theme.h"
#include "etk_dnd.h"
#include "etk_tooltips.h"
#include "config.h"

/**
 * @addtogroup Etk_Main
 * @{
 */

static void _etk_main_iterate_job_cb(void *data);
static void _etk_main_size_request_recursive(Etk_Widget *widget);
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level);

static Evas_List *_etk_main_toplevel_widgets = NULL;
static Etk_Bool _etk_main_running = ETK_FALSE;
static Etk_Bool _etk_main_initialized = ETK_FALSE;
static Ecore_Job *_etk_main_iterate_job = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes Etk. This function needs to be called before any other call to an etk_* function. @n
 * It initializes Evas, Ecore, Ecore_Evas, Ecore_X and Edje so you do not need to initialize them manually
 * if you call etk_init().
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
 * @see etk_shutdown()
 */
Etk_Bool etk_init()
{
   if (_etk_main_initialized)
      return ETK_TRUE;
   
   if (!evas_init())
   {
      ETK_WARNING("Evas initialization failed!");
      return ETK_FALSE;
   }
   if (!ecore_init())
   {
      ETK_WARNING("Ecore initialization failed!");
      return ETK_FALSE;
   }
   if (!ecore_evas_init())
   {
      ETK_WARNING("Ecore_Evas initialization failed!");
      return ETK_FALSE;
   }
   if (!ecore_x_init(NULL))
   {
      ETK_WARNING("Ecore_X initialization failed!");
      return ETK_FALSE;
   }
   if (!edje_init())
   {
      ETK_WARNING("Edje initialization failed!");
      return ETK_FALSE;
   }
   if (!etk_dnd_init())
   {
      ETK_WARNING("Etk_dnd initialization failed!");
      return ETK_FALSE;
   }
   etk_theme_init();
   etk_tooltips_init();
   
   /* Initialize Gettext */
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   _etk_main_initialized = ETK_TRUE;
   return ETK_TRUE;
}

/**
 * @brief Shuts down Etk. @n
 * You need to call it at the shutdown of your program to free all the resources used by Etk. @n
 * It also shutdown Edje, Ecore_Evas, Ecore and Evas, so you do not need to shut them down manually
 */
void etk_shutdown()
{
   if (!_etk_main_initialized)
      return;

   etk_object_destroy_all_objects();
   etk_signal_shutdown();
   etk_type_shutdown();
   etk_theme_shutdown();
   etk_tooltips_shutdown();
   etk_dnd_shutdown();
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   _etk_main_toplevel_widgets = evas_list_free(_etk_main_toplevel_widgets);

   _etk_main_initialized = ETK_FALSE;
}

/**
 * @brief Runs the Etk's main loop (and Ecore's too) until etk_main_quit() is called. @n
 * The main look updates the widgets that need to be.
 */
void etk_main()
{
   if (!_etk_main_initialized || _etk_main_running)
      return;
   
   _etk_main_running = ETK_TRUE;
   ecore_main_loop_begin();
}

/**
 * @brief Leaves the main loop of Etk. @n
 * It will quit the main loop of Ecore and will make etk_main() return.
 */
void etk_main_quit()
{
   if (!_etk_main_running)
      return;

   ecore_main_loop_quit();
   _etk_main_running = ETK_FALSE;
   if (_etk_main_iterate_job)
      ecore_job_del(_etk_main_iterate_job);
   _etk_main_iterate_job = NULL;
}

/**
 * @brief Runs an iteration of the main loop: it updates the widgets that need to be updated. @n
 * You usually do not need to call it manually, you might want to use etk_main() instead.
 */
void etk_main_iterate()
{
   Evas_List *l;
   Etk_Widget *widget;

   if (!_etk_main_initialized)
      return;

   /* TODO: only update the toplevel widgets that need to be updated */
   for (l = _etk_main_toplevel_widgets; l; l = l->next)
   {
      widget = ETK_WIDGET(l->data);
      _etk_main_size_request_recursive(widget);
      _etk_main_size_allocate_recursive(widget, ETK_TRUE);
   }
}

/**
 * @brief Queues an iteration: it will run an iteration as soon as possible.
 * You do not need to call it manually.
 */
void etk_main_iteration_queue()
{
   if (!_etk_main_iterate_job)
      _etk_main_iterate_job = ecore_job_add(_etk_main_iterate_job_cb, NULL);
}

/**
 * @brief Adds the widget to the list of toplevel widgets. For internal use only!
 * @param widget the toplevel widget to add
 */
void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_append(_etk_main_toplevel_widgets, widget);
}

/**
 * @brief Removes the widget from the list of toplevel widgets. For internal use only!
 * @param widget the toplevel widget to remove
 */
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_remove(_etk_main_toplevel_widgets, widget);
}

/**
 * @brief Gets the list of the created toplevel widgets (windows, ...)
 * @return Returns the list of the created toplevel widgets
 */
Evas_List *etk_main_toplevel_widgets_get()
{
   return _etk_main_toplevel_widgets;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Runs an iteration (used as callback for an ecore_job) */
static void _etk_main_iterate_job_cb(void *data)
{
   etk_main_iterate();
   _etk_main_iterate_job = NULL;
}

/* Recursively requests the size of all the widgets */
static void _etk_main_size_request_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   Etk_Size unused_size;
   
   etk_widget_size_request(widget, &unused_size);
   for (l = widget->children; l; l = l->next)
      _etk_main_size_request_recursive(ETK_WIDGET(l->data));
}

/* Recursively allocates the size of all the widgets */
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level)
{
   Evas_List *l;
   Etk_Geometry geometry;
   
   if (is_top_level)
      etk_toplevel_widget_geometry_get(ETK_TOPLEVEL_WIDGET(widget), &geometry.x, &geometry.y, &geometry.w, &geometry.h);
   else
      etk_widget_geometry_get(widget, &geometry.x, &geometry.y, &geometry.w, &geometry.h);
   etk_widget_size_allocate(widget, geometry);
   
   for (l = widget->children; l; l = l->next)
      _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), ETK_FALSE);
}

/** @} */
