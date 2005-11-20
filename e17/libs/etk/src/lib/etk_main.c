/** @file etk_main.c */
#include "etk_main.h"
#include <locale.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Job.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_type.h"
#include "etk_signal.h"
#include "etk_object.h"
#include "etk_toplevel_widget.h"
#include "etk_utils.h"
#include "config.h"

/**
 * @addtogroup Etk_Main
 * @{
 */

static void _etk_main_iterate_job_cb(void *data);
static void _etk_main_size_request_recursive(Etk_Widget *widget);
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level);

static Ecore_List *_etk_main_toplevel_widgets = NULL;
static Etk_Bool _etk_main_running = FALSE;
static Etk_Bool _etk_main_initialized = FALSE;
static Ecore_Job *_etk_main_iterate_job = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes etk
 * @return Returns TRUE on success, FALSE on failure
 */
Etk_Bool etk_init()
{
   if (_etk_main_initialized)
      return TRUE;
   
   if (!evas_init())
   {
      ETK_WARNING("Evas initialization failed!");
      return FALSE;
   }
   if (!ecore_init())
   {
      ETK_WARNING("Ecore initialization failed!");
      return FALSE;
   }
   if (!ecore_evas_init())
   {
      ETK_WARNING("Ecore_Evas initialization failed!");
      return FALSE;
   }
   if (!edje_init())
   {
      ETK_WARNING("Edje initialization failed!");
      return FALSE;
   }
   if (!etk_type_init())
   {
      ETK_WARNING("Etk_Type initialization failed!");
      return FALSE;
   }
   if (!etk_signal_init())
   {
      ETK_WARNING("Etk_Signal initialization failed!");
      return FALSE;
   }
   if (!etk_object_init())
   {
      ETK_WARNING("Etk_Object initialization failed!");
      return FALSE;
   }
   _etk_main_toplevel_widgets = ecore_list_new();

   /* Gettext */
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   _etk_main_initialized = TRUE;
   return TRUE;
}

/** @brief Shutdowns etk and frees the memory */
void etk_shutdown()
{
   if (!_etk_main_initialized)
      return;

   etk_object_shutdown();
   etk_signal_shutdown();
   etk_type_shutdown();
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   ecore_list_destroy(_etk_main_toplevel_widgets);

   _etk_main_initialized = FALSE;
}

/** @brief Enters the main loop */
void etk_main()
{
   if (!_etk_main_initialized || _etk_main_running)
      return;
   
   _etk_main_running = TRUE;
   ecore_main_loop_begin();
}

/** @brief Leaves the main loop */
void etk_main_quit()
{
   if (!_etk_main_running)
      return;

   ecore_main_loop_quit();
   _etk_main_running = FALSE;
   if (_etk_main_iterate_job)
      ecore_job_del(_etk_main_iterate_job);
   _etk_main_iterate_job = NULL;
}

/** @brief Runs an iteration of the main loop */
void etk_main_iterate()
{
   Etk_Widget *widget;

   if (!_etk_main_initialized)
      return;

   ecore_list_goto_first(_etk_main_toplevel_widgets);
   while ((widget = ETK_WIDGET(ecore_list_next(_etk_main_toplevel_widgets))))
   {
      //printf("Iter Begin\n");
      _etk_main_size_request_recursive(widget);
      _etk_main_size_allocate_recursive(widget, TRUE);
      //printf("Iter End\n\n");
   }
}

/** @brief Will run an iteration as soon as possible */
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
   if (!widget || !_etk_main_toplevel_widgets)
      return;

   ecore_list_append(_etk_main_toplevel_widgets, widget);
}

/**
 * @brief Removes the widget from the list of toplevel widgets. Never call it manually
 * @param widget the toplevel widget to remove
 */
void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget)
{
   if (!widget || !_etk_main_toplevel_widgets)
      return;

   if (ecore_list_goto(_etk_main_toplevel_widgets, widget))
      ecore_list_remove(_etk_main_toplevel_widgets);
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

static void _etk_main_size_request_recursive(Etk_Widget *widget)
{
   Evas_List *l;
   Etk_Size unused_size;
   
   etk_widget_size_request(widget, &unused_size);
   
   if (ETK_IS_CONTAINER(widget))
   {
      for (l = ETK_CONTAINER(widget)->children; l; l = l->next)
         _etk_main_size_request_recursive(ETK_WIDGET(l->data));
   }
}

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
   
   if (ETK_IS_CONTAINER(widget))
   {
      for (l = ETK_CONTAINER(widget)->children; l; l = l->next)
         _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), FALSE);
   }
}

/** @} */
