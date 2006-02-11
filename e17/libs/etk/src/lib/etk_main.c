/** @file etk_main.c */
#include "etk_main.h"
#include "etk_dnd.h"
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
#include "config.h"

/**
 * @addtogroup Etk_Main
 * @{
 */

static void _etk_main_iterate_job_cb(void *data);
static void _etk_main_size_request_recursive(Etk_Widget *widget);
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level);

/* we need this in etk_dnd */
Evas_List *_etk_main_toplevel_widgets = NULL;

static Etk_Bool _etk_main_running = ETK_FALSE;
static Etk_Bool _etk_main_initialized = ETK_FALSE;
static Ecore_Job *_etk_main_iterate_job = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes etk
 * @return Returns ETK_TRUE on success, ETK_FALSE on failure
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
   if (!edje_init())
   {
      ETK_WARNING("Edje initialization failed!");
      return ETK_FALSE;
   }
#if HAVE_ECORE_X
   if (!etk_dnd_init())
   {
      ETK_WARNING("Etk_dnd and Ecore_X initialzation failed!");
      return ETK_FALSE;
   }
#endif
   etk_theme_init();

   /* Gettext */
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   _etk_main_initialized = ETK_TRUE;
   return ETK_TRUE;
}

/**
 * @brief Shutdowns etk and frees the memory
 */
void etk_shutdown()
{
   if (!_etk_main_initialized)
      return;

   etk_object_destroy_all_objects();
   etk_signal_shutdown();
   etk_type_shutdown();
   etk_theme_shutdown();
   etk_dnd_shutdown();
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   _etk_main_toplevel_widgets = evas_list_free(_etk_main_toplevel_widgets);

   _etk_main_initialized = ETK_FALSE;
}

/**
 * @brief Enters the main loop
*/
void etk_main()
{
   if (!_etk_main_initialized || _etk_main_running)
      return;
   
   _etk_main_running = ETK_TRUE;
   ecore_main_loop_begin();
}

/**
 * @brief Leaves the main loop
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
 * @brief Runs an iteration of the main loop
 */
void etk_main_iterate()
{
   Evas_List *l;
   Etk_Widget *widget;

   if (!_etk_main_initialized)
      return;

   for (l = _etk_main_toplevel_widgets; l; l = l->next)
   {
      widget = ETK_WIDGET(l->data);
      _etk_main_size_request_recursive(widget);
      _etk_main_size_allocate_recursive(widget, ETK_TRUE);
   }
}

/**
 * @brief Will run an iteration as soon as possible
 */
void etk_main_iteration_queue()
{
   if (_etk_main_iterate_job)
      return;
   _etk_main_iterate_job = ecore_job_add(_etk_main_iterate_job_cb, NULL);
}

/**
 * @brief Adds the widget to the list of toplevel widgets. Never call it manually
 * @param widget the toplevel widget to add
 */
void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_append(_etk_main_toplevel_widgets, widget);
}

/**
 * @brief Removes the widget from the list of toplevel widgets. Never call it manually!
 * @param widget the toplevel widget to remove
 */
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_remove(_etk_main_toplevel_widgets, widget);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Runs an iteration (used as callback for an ecore_job)*/
static void _etk_main_iterate_job_cb(void *data)
{
   _etk_main_iterate_job = NULL;
   etk_main_iterate();
}

/* Recusively requests the size of all the widgets */
static void _etk_main_size_request_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   Etk_Size unused_size;
   
   etk_widget_size_request(widget, &unused_size);
   for (l = widget->children; l; l = l->next)
      _etk_main_size_request_recursive(ETK_WIDGET(l->data));
}

/* Recusively allocates the size of all the widgets */
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level)
{
   Evas_List *l;
   Etk_Geometry geometry;
   
   if (is_top_level)
   {
      geometry.x = 0;
      geometry.y = 0;
      etk_toplevel_widget_geometry_get(ETK_TOPLEVEL_WIDGET(widget), NULL, NULL, &geometry.w, &geometry.h);
   }
   else
      geometry = widget->geometry;
   etk_widget_size_allocate(widget, geometry);
   
   for (l = widget->children; l; l = l->next)
      _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), ETK_FALSE);
}

/** @} */
