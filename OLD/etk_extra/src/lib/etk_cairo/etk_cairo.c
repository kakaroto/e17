/** @file etk_cairo.c */
#include "etk_cairo.h"

int ETK_CAIRO_REDRAW_REQUIRED_SIGNAL;

enum Etk_Cairo_Property_Id
{
  ETK_CAIRO_MIN_WIDTH_PROPERTY,
  ETK_CAIRO_MIN_HEIGHT_PROPERTY
};

static void _etk_cairo_constructor(Etk_Cairo *cairo);
static void _etk_cairo_destructor(Etk_Cairo *cairo);
static void _etk_cairo_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_cairo_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static Etk_Bool _etk_cairo_size_requested_cb(Etk_Object *object, Etk_Size *size, void *data);
static void _etk_cairo_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_cairo_rebuild(Etk_Cairo *cairo);
static Etk_Bool _etk_cairo_redraw_required_handler(Etk_Cairo *cairo);

/**
 * @internal
 * @brief Gets the type of an Etk_Cairo
 * @return Returns the type of an Etk_Cairo
 */
Etk_Type *etk_cairo_type_get(void)
{
  static Etk_Type *cairo_type = NULL;

  if (!cairo_type)
  {
    const Etk_Signal_Description signals[] = {
      ETK_SIGNAL_DESC_HANDLER(ETK_CAIRO_REDRAW_REQUIRED_SIGNAL,
        "redraw-required", Etk_Cairo, redraw_required_handler,
        etk_marshaller_VOID),
      ETK_SIGNAL_DESCRIPTION_SENTINEL
    };

    cairo_type = etk_type_new("Etk_Cairo", ETK_WIDGET_TYPE, sizeof(Etk_Cairo),
      ETK_CONSTRUCTOR(_etk_cairo_constructor), ETK_DESTRUCTOR(_etk_cairo_destructor), signals);

    etk_type_property_add(cairo_type, "min-width", ETK_CAIRO_MIN_WIDTH_PROPERTY,
      ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
    etk_type_property_add(cairo_type, "min-height", ETK_CAIRO_MIN_HEIGHT_PROPERTY,
      ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));

    cairo_type->property_set = _etk_cairo_property_set;
    cairo_type->property_get = _etk_cairo_property_get;
  }

  return cairo_type;
}

/**
 * @brief Creates a new empty surface
 * @param min_width The minimum width this surface can have
 * @param min_height The minimum height this surface can have
 * @return Returns a new cairo surface
 */
Etk_Widget *etk_cairo_new(int min_width, int min_height)
{
  Etk_Widget *cairo;

  cairo = etk_widget_new(ETK_CAIRO_TYPE, "min-width", min_width, "min-height", min_height, NULL);
  return cairo;
}

/* Initialized the cairo widget */
static void _etk_cairo_constructor(Etk_Cairo *cairo)
{
  Etk_Widget *widget;

  if (!(widget = ETK_WIDGET(cairo)))
    return;

  cairo->cairo.cr = NULL;
  cairo->cairo.surface = NULL;
  cairo->min_size.w = 0;
  cairo->min_size.h = 0;
  cairo->image = etk_image_new();
  etk_widget_internal_set(cairo->image, ETK_TRUE);
  etk_widget_parent_set(cairo->image, widget);
  etk_widget_show(cairo->image);

  widget->size_allocate = _etk_cairo_size_allocate;

  cairo->redraw_required_handler = _etk_cairo_redraw_required_handler;
  etk_signal_connect("size-requested", ETK_OBJECT(cairo), ETK_CALLBACK(_etk_cairo_size_requested_cb), NULL);
}

/**
 * @brief Returns the cairo_t* object from an Etk_Cairo
 * @param cairo The Etk_Cairo widget
 * @return Returns the cairo_t* object
 */
cairo_t *etk_cairo_get(Etk_Cairo *cairo)
{
  if (!ETK_IS_CAIRO(cairo))
    return NULL;

  return cairo->cairo.cr;
}

/* Destroyes the cairo widget */
static void _etk_cairo_destructor(Etk_Cairo *cairo)
{
  if (!cairo)
    return;

  if (cairo->cairo.surface)
    cairo_surface_destroy(cairo->cairo.surface);
  if (cairo->cairo.cr)
    cairo_destroy(cairo->cairo.cr);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_cairo_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
  Etk_Cairo *cairo;

  if (!(cairo = ETK_CAIRO(object)) || !value)
    return;

  switch (property_id)
  {
    case ETK_CAIRO_MIN_WIDTH_PROPERTY:
      cairo->min_size.w = etk_property_value_int_get(value);
      break;
    case ETK_CAIRO_MIN_HEIGHT_PROPERTY:
      cairo->min_size.h = etk_property_value_int_get(value);
      break;
  }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_cairo_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
  Etk_Cairo *cairo;

  if (!(cairo = ETK_CAIRO(object)) || !value)
    return;

  switch (property_id)
  {
    case ETK_CAIRO_MIN_WIDTH_PROPERTY:
      etk_property_value_int_set(value, cairo->min_size.w);
      break;
    case ETK_CAIRO_MIN_HEIGHT_PROPERTY:
      etk_property_value_int_set(value, cairo->min_size.h);
      break;
  }
}

static Etk_Bool _etk_cairo_size_requested_cb(Etk_Object *object, Etk_Size *size, void *data)
{
  Etk_Cairo *cairo;
  unsigned char *pixels = NULL;
  int static old_w = -1, old_h = -1;
  int w, h;

  if (!(cairo = ETK_CAIRO(object)))
    return ETK_TRUE;

  etk_widget_geometry_get(ETK_WIDGET(cairo), NULL, NULL, &w, &h);

  if (old_w == w && old_h == h)
    return ETK_TRUE;

  old_w = w;
  old_h = h;

  _etk_cairo_rebuild(cairo);
  cairo_rectangle(cairo->cairo.cr, 0, 0, w, h);
  etk_signal_emit(ETK_CAIRO_REDRAW_REQUIRED_SIGNAL, object, NULL);
  pixels =  cairo_image_surface_get_data(cairo->cairo.surface);

  if (pixels)
    etk_image_set_from_data(ETK_IMAGE(cairo->image), w, h, pixels, ETK_FALSE);

  return ETK_TRUE;
}

static void _etk_cairo_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
  Etk_Cairo *cairo;

  if (!(cairo = ETK_CAIRO(widget)))
    return;

  etk_widget_size_allocate(cairo->image, geometry);
}

/* Rebuilds the cairo surface */
void _etk_cairo_rebuild(Etk_Cairo *cairo)
{
  int w, h;

  if (!cairo)
    return;

  if (cairo->cairo.surface)
    cairo_surface_destroy(cairo->cairo.surface);

  if (cairo->cairo.cr)
    cairo_destroy(cairo->cairo.cr);

  etk_widget_geometry_get(ETK_WIDGET(cairo), NULL, NULL, &w, &h);

  cairo->cairo.surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
  cairo->cairo.cr = cairo_create(cairo->cairo.surface);
}

static Etk_Bool _etk_cairo_redraw_required_handler(Etk_Cairo *cairo)
{
  if (!cairo)
    return ETK_TRUE;

}
