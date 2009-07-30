#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(w) ((w)->private)
#define TYPE_NAME "Widget"

struct _Widget_Private
{
	int x, y, w, h;
	char *theme;
};


static void _ctor(void *instance)
{
	Widget *widget = (Widget*) instance;

	widget->private = type_instance_private_get(widget_type_get(), instance);
	widget->private->theme = NULL;
	printf("[widget] ctor %p %p\n", widget, widget->private);
}

static void _dtor(void *widget)
{
  printf("[widget] dtor %p\n", widget);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Type *widget_type_get(void)
{
	static Type *widget_type = NULL;

	if (!widget_type)
	{
		widget_type = type_new(TYPE_NAME, sizeof(Widget),
				sizeof(Widget_Private), object_type_get(),
				_ctor, _dtor, NULL);
		EKEKO_TYPE_PROP_SINGLE_ADD(widget_type, "theme", EKEKO_PROPERTY_STRING, OFFSET(Widget_Private, theme));
	}

	return widget_type;
}

Widget *widget_new(void)
{
	Widget *widget;

	widget = type_instance_new(widget_type_get());

	return widget;
}

void widget_delete(Widget *w)
{
	type_instance_delete(w);
}

void widget_geom_set(Widget *widget, int x, int y, int w, int h)
{
	Widget_Private *private;

	private = PRIVATE(widget);
	private->x = x;
	private->y = y;
	private->w = w;
	private->h = h;
}

void widget_geom_get(Widget *widget, int *x, int *y, int *w, int *h)
{
	Widget_Private *private;

	private  = PRIVATE(widget);

	if (x) *x = private->x;
	if (y) *y = private->y;
	if (w) *w = private->w;
	if (h) *h = private->h;
}

void widget_theme_set(Widget *widget, char *theme)
{
	Ekeko_Value value;

	ekeko_value_str_from(&value, theme);
	object_property_value_set((Object *)widget, "theme", &value);
}

char *widget_theme_get(Widget *widget)
{
	Widget_Private *private;

	RETURN_NULL_IF(widget == NULL);

	private = PRIVATE(widget);
	return private->theme;
}
