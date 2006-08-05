/** @file etk_main.c */
#include "etk_main.h"
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <limits.h>

#include <Ecore.h>
#include <Ecore_Job.h>
#include <Evas.h>
#include <Edje.h>
#include "etk_argument.h"
#include "etk_engine.h"
#include "etk_type.h"
#include "etk_signal.h"
#include "etk_object.h"
#include "etk_toplevel_widget.h"
#include "etk_utils.h"
#include "etk_theme.h"
#include "etk_dnd.h"
#include "etk_tooltips.h"
#include "etk_textblock.h"
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
static int _etk_main_init_count = 0;
static Ecore_Job *_etk_main_iterate_job = NULL;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Initializes Etk. This function needs to be called before any other call to an etk_* function. @n
 * You can call safely etk_init() several times, it will have an effect only the first time you call it. The other times,
 * it will just increment a counter. etk_shutdown() will decrement this counter and will effectively shutdown Etk when
 * the counter reaches 0. So you need to call etk_shutdown() the same number of times as etk_init().
 * @param argc the location of the "argc" paramater passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to NULL.
 * @param argv the location of the "argv" paramater passed to main(). It is used to parse the arguments specific to Etk.
 * It can be set to NULL.
 * @return Returns the number of times Etk has been initialized, or 0 on failure
 * @note It initializes Evas, Ecore and Edje so you don't need to initialize them after an etk_init()
 * @see etk_shutdown()
 */
int etk_init(int *argc, char ***argv)
{
   char *engine_name = NULL;

   if (_etk_main_init_count > 0)
   {
      _etk_main_init_count++;
      return _etk_main_init_count;
   }
   else
   {
      /* Parse the arguments */
      if (argc && argv)
      {
         etk_argument_value_get(argc, argv, "etk-engine", 'e', ETK_TRUE, &engine_name);
      }
      
      /* Initialize the EFL */
      if (!evas_init())
      {
         ETK_WARNING("Evas initialization failed!");
         return 0;
      }
      if (!ecore_init())
      {
         ETK_WARNING("Ecore initialization failed!");
         return 0;
      }
      if (!edje_init())
      {
         ETK_WARNING("Edje initialization failed!");
         return 0;
      }
      
      /* Initialize the subsystems of Etk */
      etk_theme_init();
      if (!etk_engine_init())
      {
         ETK_WARNING("Etk_Engine initialization failed!");
         return 0;
      }
      if (!etk_engine_load(engine_name ? engine_name : "ecore_evas_software_x11"))
      {
         ETK_WARNING("Etk can not load the requested engine!");
         return 0;
      }
      if (!etk_dnd_init())
      {
         ETK_WARNING("Etk_dnd initialization failed!");
         return 0;
      }
      etk_tooltips_init();
      
      /* Initialize Gettext */
      setlocale(LC_ALL, "");
      bindtextdomain(PACKAGE, LOCALEDIR);
      textdomain(PACKAGE);
      
      free(engine_name);
      _etk_main_init_count++;
      return _etk_main_init_count;
   }
}

/**
 * @brief Shuts down Etk. It decrements the counter of initializations. If the counter reaches 0, it frees all the
 * resources used by Etk. @n
 * @return Returns the new number of times Etk has been initialized. 0 means that the resources has been freed.
 */
int etk_shutdown()
{
   if (_etk_main_init_count <= 0)
      return 0;
   
   _etk_main_init_count--;
   if (_etk_main_init_count == 0)
   {
      /* Shutdown the subsystem of Etk */
      etk_object_destroy_all_objects();
      etk_signal_shutdown();
      etk_type_shutdown();
      
      etk_textblock_shutdown();
      etk_tooltips_shutdown();
      etk_dnd_shutdown();
      etk_engine_shutdown();
      etk_theme_shutdown();
      
      _etk_main_toplevel_widgets = evas_list_free(_etk_main_toplevel_widgets);
      
      /* Shutdown the EFL*/
      edje_shutdown();
      ecore_shutdown();
      evas_shutdown();
   }
   
   return _etk_main_init_count;
}

/**
 * @brief Runs the Etk's main loop until etk_main_quit() is called.
 * @note It calls ecore_main_loop_begin() so you should not call ecore_main_loop_begin() or ecore_main_loop_quit()
 * if you are using etk_main() in your program.
 */
void etk_main()
{
   if (_etk_main_init_count <= 0 || _etk_main_running)
      return;
   
   _etk_main_running = ETK_TRUE;
   ecore_main_loop_begin();
}

/**
 * @brief Leaves the main loop of Etk. It will quit the main loop of Ecore (ecore_main_loop_quit())
 * and will make etk_main() return.
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

   if (_etk_main_init_count <= 0)
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
 * @internal
 * @brief Queues an iteration: it will run an iteration as soon as possible
 */
void etk_main_iteration_queue()
{
   if (!_etk_main_iterate_job)
      _etk_main_iterate_job = ecore_job_add(_etk_main_iterate_job_cb, NULL);
}

/**
 * @internal
 * @brief Adds the widget to the list of toplevel widgets
 * @param widget the toplevel widget to add
 */
void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget)
{
   if (!widget)
      return;
   _etk_main_toplevel_widgets = evas_list_append(_etk_main_toplevel_widgets, widget);
}

/**
 * @internal
 * @brief Removes the widget from the list of toplevel widgets
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
   
   if (!widget)
      return;
   
   etk_widget_size_request(widget, &unused_size);
   for (l = widget->children; l; l = l->next)
      _etk_main_size_request_recursive(ETK_WIDGET(l->data));
}

/* Recursively allocates the size of all the widgets */
static void _etk_main_size_allocate_recursive(Etk_Widget *widget, Etk_Bool is_top_level)
{
   Evas_List *l;
   Etk_Geometry geometry;
   
   if (!widget)
      return;
   
   if (is_top_level)
   {
      etk_toplevel_widget_evas_position_get(ETK_TOPLEVEL_WIDGET(widget), &geometry.x, &geometry.y);
      etk_toplevel_widget_size_get(ETK_TOPLEVEL_WIDGET(widget), &geometry.w, &geometry.h);
   }
   else
      etk_widget_geometry_get(widget, &geometry.x, &geometry.y, &geometry.w, &geometry.h);
   etk_widget_size_allocate(widget, geometry);
   
   for (l = widget->children; l; l = l->next)
      _etk_main_size_allocate_recursive(ETK_WIDGET(l->data), ETK_FALSE);
}

/** @} */
