/** @file etk_mdi_area.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_mdi_area.h"

#include "etk_mdi_window.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Mdi_Area
 * @{
 */

static void _etk_mdi_area_constructor(Etk_Mdi_Area *mdi_area);
static void _etk_mdi_area_destructor(Etk_Mdi_Area *mdi_area);
static void _etk_mdi_area_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_mdi_area_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_mdi_area_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_mdi_area_child_remove(Etk_Container *container, Etk_Widget *widget);
static Evas_List *_etk_mdi_area_children_get(Etk_Container *container);
static void _etk_mdi_area_realized_cb(Etk_Object *object, void *data);
static void _etk_mdi_area_child_moved_cb(Etk_Widget *child, void *data);
static void _etk_mdi_area_child_maximized_cb(Etk_Object *object, const char *property_name, void *data);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Mdi_Area
 * @return Returns the type of an Etk_Mdi_Area
 */
Etk_Type *etk_mdi_area_type_get(void)
{
   static Etk_Type *mdi_area_type = NULL;

   if (!mdi_area_type)
   {
      mdi_area_type = etk_type_new("Etk_Mdi_Area", ETK_CONTAINER_TYPE, sizeof(Etk_Mdi_Area),
         ETK_CONSTRUCTOR(_etk_mdi_area_constructor), ETK_DESTRUCTOR(_etk_mdi_area_destructor));
   }

   return mdi_area_type;
}

/**
 * @brief Creates a new mdi_area container
 * @return Returns the new mdi_area container
 */
Etk_Widget *etk_mdi_area_new(void)
{
   return etk_widget_new(ETK_MDI_AREA_TYPE, "theme-group", "mdi_area", NULL);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the mdi_area container */
static void _etk_mdi_area_constructor(Etk_Mdi_Area *mdi_area)
{
   if (!mdi_area)
      return;

   mdi_area->children = NULL;

   ETK_WIDGET(mdi_area)->size_request = _etk_mdi_area_size_request;
   ETK_WIDGET(mdi_area)->size_allocate = _etk_mdi_area_size_allocate;
   ETK_CONTAINER(mdi_area)->child_add = _etk_mdi_area_child_add;
   ETK_CONTAINER(mdi_area)->child_remove = _etk_mdi_area_child_remove;
   ETK_CONTAINER(mdi_area)->children_get = _etk_mdi_area_children_get;

   etk_signal_connect("realized", ETK_OBJECT(mdi_area), ETK_CALLBACK(_etk_mdi_area_realized_cb), NULL);
   etk_signal_connect_swapped("unrealized", ETK_OBJECT(mdi_area), ETK_CALLBACK(etk_callback_set_null), &mdi_area->clip);
}

/* Destroys the mdi_area container */
static void _etk_mdi_area_destructor(Etk_Mdi_Area *mdi_area)
{
   if (!mdi_area)
      return;

   while (mdi_area->children)
   {
      mdi_area->children = evas_list_remove_list(mdi_area->children, mdi_area->children);
   }
}

/* Calculates the ideal size of the mdi_area container */
static void _etk_mdi_area_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Widget *c;
   Etk_Size child_size;
   Evas_List *l;
   int x, y;

   if (!(mdi_area = ETK_MDI_AREA(widget)) || !size_requisition)
      return;

   size_requisition->w = 0;
   size_requisition->h = 0;

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;

      etk_mdi_window_position_get(ETK_MDI_WINDOW(c), &x, &y);
      etk_widget_size_request(c, &child_size);

      size_requisition->w = ETK_MAX(size_requisition->w, x + child_size.w);
      size_requisition->h = ETK_MAX(size_requisition->h, y + child_size.h);
   }

   size_requisition->w += 2 * ETK_CONTAINER(mdi_area)->border_width;
   size_requisition->h += 2 * ETK_CONTAINER(mdi_area)->border_width;
}

/* Resizes the mdi_area to the size allocation */
static void _etk_mdi_area_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Widget *c;
   Etk_Size child_size;
   Etk_Geometry child_geometry;
   Evas_List *l;
   int x, y;

   if (!(mdi_area = ETK_MDI_AREA(widget)))
      return;

   geometry.x += ETK_CONTAINER(mdi_area)->border_width;
   geometry.y += ETK_CONTAINER(mdi_area)->border_width;
   geometry.w -= 2 * ETK_CONTAINER(mdi_area)->border_width;
   geometry.h -= 2 * ETK_CONTAINER(mdi_area)->border_width;

   evas_object_move(mdi_area->clip, geometry.x, geometry.y);
   evas_object_resize(mdi_area->clip, geometry.w, geometry.h);

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;

      etk_mdi_window_position_get(ETK_MDI_WINDOW(c), &x, &y);
      etk_widget_size_request(c, &child_size);

      if (!ETK_MDI_WINDOW(c)->maximized)
      {
         child_geometry.x = geometry.x + x;
         child_geometry.y = geometry.y + y;
         child_geometry.w = child_size.w;
         child_geometry.h = child_size.h;
      }
      else
      {
         child_geometry.x = geometry.x;
         child_geometry.y = geometry.y;
         child_geometry.w = geometry.w;
         child_geometry.h = geometry.h;
      }

      etk_widget_size_allocate(c, child_geometry);
   }
}

/* Adds a child to the mdi_area container */
static void _etk_mdi_area_child_add(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Mdi_Area *mdi_area;

   if (!(mdi_area = ETK_MDI_AREA(container)) || !widget || !ETK_IS_MDI_WINDOW(widget))
      return;

   mdi_area->children = evas_list_append(mdi_area->children, widget);

   if (mdi_area->clip)
   {
      etk_widget_clip_set(widget, mdi_area->clip);
      evas_object_show(mdi_area->clip);
   }

   etk_widget_parent_set(widget, ETK_WIDGET(mdi_area));
   etk_signal_connect("moved", ETK_OBJECT(widget), ETK_CALLBACK(_etk_mdi_area_child_moved_cb), mdi_area);
   etk_object_notification_callback_add(ETK_OBJECT(widget), "maximized", _etk_mdi_area_child_maximized_cb, mdi_area);
   etk_signal_emit_by_name("child-added", ETK_OBJECT(mdi_area), NULL, widget);
   etk_object_notify(ETK_OBJECT(mdi_area), "child");
}

/* Removes the child from the mdi_area container */
static void _etk_mdi_area_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Mdi_Area *mdi_area;
   Evas_List *l;

   if (!(mdi_area = ETK_MDI_AREA(container)) || !widget)
      return;

   for (l = mdi_area->children; l; l = l->next)
   {
      if (l->data == widget)
      {
         mdi_area->children = evas_list_remove_list(mdi_area->children, l);

         if (mdi_area->clip)
         {
            etk_widget_clip_unset(widget);
            if (!mdi_area->children)
               evas_object_hide(mdi_area->clip);
         }

         etk_signal_disconnect("moved", ETK_OBJECT(widget), ETK_CALLBACK(_etk_mdi_area_child_moved_cb));
         etk_object_notification_callback_remove(ETK_OBJECT(widget), "maximized", _etk_mdi_area_child_maximized_cb);

         etk_signal_emit_by_name("child-removed", ETK_OBJECT(mdi_area), NULL, widget);
         etk_object_notify(ETK_OBJECT(mdi_area), "child");
         break;
      }
   }
}

/* Gets the list of the children */
static Evas_List *_etk_mdi_area_children_get(Etk_Container *container)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Widget *c;
   Evas_List *children, *l;

   if (!(mdi_area = ETK_MDI_AREA(container)))
      return NULL;

   children = NULL;
   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      children = evas_list_append(children, c);
   }
   return children;
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Called when the mdi_area container is realized */
static void _etk_mdi_area_realized_cb(Etk_Object *object, void *data)
{
   Etk_Mdi_Area *mdi_area;
   Etk_Widget *c;
   Evas_List *l;
   Evas *evas;

   if (!(mdi_area = ETK_MDI_AREA(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return;

   mdi_area->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(ETK_WIDGET(mdi_area), mdi_area->clip);

   for (l = mdi_area->children; l; l = l->next)
   {
      c = l->data;
      etk_widget_clip_set(c, mdi_area->clip);
   }

   if (mdi_area->children)
      evas_object_show(mdi_area->clip);
}

static void _etk_mdi_area_child_moved_cb(Etk_Widget *child, void *data)
{
   Etk_Mdi_Area *mdi_area;

   if (!(mdi_area = ETK_MDI_AREA(data)))
      return;

   etk_widget_size_recalc_queue(ETK_WIDGET(mdi_area));
}

static void _etk_mdi_area_child_maximized_cb(Etk_Object *object, const char *property_name, void *data)
{
   Etk_Mdi_Area *mdi_area;

   if (!(mdi_area = ETK_MDI_AREA(data)))
      return;

   etk_widget_size_recalc_queue(ETK_WIDGET(mdi_area));
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Mdi_Area
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Mdi_Area
 */
