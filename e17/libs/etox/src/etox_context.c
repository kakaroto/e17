#include "../etox-config.h"
#include "Etox_private.h"

/**
 * etox_context_new - create a new context for loading values to an etox
 *
 * Returns a newly allocated and zeroed out Etox_Context on success, NULL on
 * failure.
 */
Etox_Context *etox_context_new()
{
	Etox_Context *context;

	context = (Etox_Context *) calloc(1, sizeof(Etox_Context));

	/*
	 * Setup the default color
	 */
	context->r = 255;
	context->g = 255;
	context->b = 255;
	context->a = 255;

	/*
	 * Setup the default style
	 */
	context->style = strdup("none");

	/*
	 * Set up the default font
	 */
	context->font = strdup("nationff");
	context->font_size = 14;

	/*
	 * Setup the default alignment
	 */
	context->flags = ETOX_ALIGN_LEFT | ETOX_ALIGN_BOTTOM | ETOX_SOFT_WRAP;

	/*
	 * Set up a default blank wrap marker
	 */
	context->marker.text = "+";
	context->marker.style = "plain";
	context->marker.r = 255;
	context->marker.g = 0;
	context->marker.b = 0;
	context->marker.a = 255;

	return context;
}

/**
 * etox_context_save - save a copy of the current context for restoring later
 * @et: the etox to retrieve a copy of the current context
 *
 * Returns a copy of the current context on success, NULL on failure. The
 * returned context can later be restored with a call to etox_context_load.
 */
Etox_Context *etox_context_save(Evas_Object * obj)
{
	Etox *et;
	Etox_Context *ret;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	ret = (Etox_Context *) malloc(sizeof(Etox_Context));

	/*
	 * Copy the contents of the old context
	 */
	memcpy(ret, et->context, sizeof(Etox_Context));
	if (et->context->style)
		ret->style = strdup(et->context->style);
	if (et->context->font)
		ret->font = strdup(et->context->font);

	if (et->context->marker.text)
		ret->marker.text = strdup(et->context->marker.text);

	if (et->context->marker.style)
		ret->marker.style = strdup(et->context->marker.style);

        ret->flags = et->context->flags;

	return ret;
}

/**
 * etox_context_load - load a saved context into the current context for an etox
 * @et: the etox to assign the context
 * @context: the saved context to load for the etox
 *
 * Returns no value. The context pointed to by @context is loaded into the etox
 * @et as the current context.
 */
void etox_context_load(Evas_Object * obj, Etox_Context * context)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("context", context);

	et = evas_object_smart_data_get(obj);

	/*
	 * Dereference the style before overwriting
	 */
	IF_FREE(et->context->style);
	IF_FREE(et->context->font);
	IF_FREE(et->context->marker.text);
	IF_FREE(et->context->marker.style);

	memcpy(et->context, context, sizeof(Etox_Context));

	if (context->style)
		et->context->style = strdup(context->style);
	if (context->font)
		et->context->font = strdup(context->font);

	if (et->context->marker.text)
		et->context->marker.text = strdup(context->marker.text);

	if (et->context->marker.style)
		et->context->marker.style = strdup(context->marker.style);

        et->context->flags = context->flags;
}

/**
 * etox_context_free - free the memory used by the context
 * @context: the context to be freed
 *
 * Returns no value. The memory used by the saved
 * context @context is freed and the context becomes invalid.
 */
void etox_context_free(Etox_Context * context)
{
	CHECK_PARAM_POINTER("context", context);

        if (!context) return;

	IF_FREE(context->font);
	IF_FREE(context->style);
	IF_FREE(context->marker.text);
	IF_FREE(context->marker.style);

	FREE(context);
}

/**
 * etox_context_get_color - retrieve the color of the current context
 * @et: the etox to retrieve the current context's color
 * @r: a pointer to an integer to hold the red value of the color
 * @g: a pointer to an integer to hold the green value of the color
 * @b: a pointer to an integer to hold the blue value of the color
 * @a: a pointer to an integer to hold the alpha value of the color
 *
 * Returns no value. The color of the current context for the etox @et are
 * stored into any non-NULL parameters @r, @g, @b, and @a.
 */
void etox_context_get_color(Evas_Object * obj, int *r, int *g, int *b, int *a)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	*a = et->context->a;
	*r = et->context->r;
	*b = et->context->b;
	*g = et->context->g;
}

/**
 * etox_context_set_color - change the default color for the etox
 * @et: the etox to change the color context 
 * @r: the red component of the color context to use
 * @g: the green component of the color context to use
 * @b: the blue component of the color context to use
 * @a: the alpha component of the color context to use
 *
 * Returns no value. Sets the color context for the etox @et.
 */
void etox_context_set_color(Evas_Object * obj, int r, int g, int b, int a)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->context->a = a;
	et->context->r = r;
	et->context->g = g;
	et->context->b = b;
}

/**
 * etox_context_set_color_db - set current color based on values read from db
 * @et: the etox to set the current contexts color
 * @name: the name of the color in the db to load for the current color
 *
 * Returns TRUE on success, FALSE on failure. The color information is loaded
 * from the color databases based on @name, and then assigned as the current
 * context's color.
 */
void etox_context_set_color_db(Evas_Object *obj, char *name)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	estyle_lookup_color_db(name, &et->context->r, &et->context->g,
			       &et->context->b, &et->context->a);
}

/**
 * etox_context_clear_callbacks - clears all callbacks of specifed type
 * @et: the etox to clear callbacks
 * @type: the type of the callbacks that are to be cleared
 *
 * Returns TRUE on success, FALSE on failure. All callbacks associated with
 * the event @type are cleared from the current context of @et.
 */
/*
void etox_context_clear_callbacks(Etox *et)
{
	Evas_List l;
	Etox_Callback *cb;

	CHECK_PARAM_POINTER("et", et);

	l = et->context->callbacks;
	while (l) {
		cb = l->data;
		l = evas_list_remove(l, cb);
		FREE(cb);
	}

	et->context->callbacks = NULL;
}
*/

/**
 * etox_context_add_callback - add an event callback to the current context
 */
/*
void etox_context_add_callback(Etox *et, Evas_Callback_Type type,
		Etox_Cb_Func func, void *data)
{
	Etox_Callback *cb;

	CHECK_PARAM_POINTER("et", et);

	cb = calloc(1, sizeof(Etox_Callback));
	cb->type = type;
	cb->func = func;
	cb->data = data;

	et->context->callbacks = evas_list_append(et->context->callbacks, cb);
}
*/

/**
 * etox_context_del_callback - delete the callback events of a certain type
 */
/*
void etox_context_del_callback(Etox *et, Evas_Callback_Type callback)
{
	Evas_List l;

	CHECK_PARAM_POINTER("et", et);

	l = et->context->callbacks;
	while (l) {
		Etox_Callback *cb;

		cb = l->data;
		l = l->next;
		if (cb->type == callback)
			et->context->callbacks =
				evas_list_remove(et->context->callbacks, cb);
	}
}
*/

/**
 * etox_context_set_font - change the default font for the etox
 * @et: the etox to change the default font
 * @name: the name of the default font to use
 * @size: the size of the default font to use
 *
 * Returns no value. Sets the default font for the etox @et to @name with size
 * @size.
 */
void etox_context_set_font(Evas_Object * obj, char *name, int size)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("name", name);

	et = evas_object_smart_data_get(obj);

	IF_FREE(et->context->font);
	et->context->font = strdup(name);
	et->context->font_size = size;
}

/*
 * etox_context_get_style - retrieve the name of current style
 * @et: the etox to query for current style.
 *
 * Returns a pointer to the style name on success, NULL on failure.
 */
char *etox_context_get_style(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	if (!et->context->style)
		return NULL;

	return strdup(et->context->style);
}

/**
 * etox_context_set_style - change the default style of the etox
 * @et: the etox to change the default style
 * @name: the name of the style to be used
 *
 * Returns no value. Changes the default style used by the etox to the one
 * represented by @name.
 */
void etox_context_set_style(Evas_Object * obj, char *name)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	/*
	 * Release this instance of the old style, and then get an instance of
	 * the new style.
	 */
	IF_FREE(et->context->style);

	if (name && *name)
		et->context->style = strdup(name);
	else
		et->context->style = NULL;
}

/*
 * etox_context_get_align - retrieve the current alignment of an etox
 * @et: the etox to query for alignment
 *
 * Returns the alignment value for @et on success, NULL on failure.
 */
int etox_context_get_align(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("obj", obj, FALSE);

	et = evas_object_smart_data_get(obj);

	return et->context->flags & ETOX_ALIGN_MASK;
}

/*
 * etox_context_set_align - set the current alignment for an etox
 * @et: the etox to change the alignment
 *
 * Returns no value. Changes @et's current context's alignment value to
 * @align.
 */
void etox_context_set_align(Evas_Object * obj, int align)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->context->flags = align | (et->context->flags & ~ETOX_ALIGN_MASK);
}

/**
 * etox_context_set_soft_wrap - turns on soft wrapping of lines that are
 * longer than the etox is wide
 * @et: the etox to set for
 * @boolean: 0 is off, anything else is on
 * 
 * Returns no value. changes current context alignment value.
 */
void etox_context_set_soft_wrap(Evas_Object *obj, int boolean)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (boolean)
		et->context->flags |= ETOX_SOFT_WRAP;
	else
		et->context->flags &= ~ETOX_SOFT_WRAP;
}

/**
 * etox_set_wrap_marker - sets string to mark wrapped lines
 * @et: the etox to set for
 * @marker: the string to mark wrapped lines
 * @style: the style of the marker
 *
 * Returns nothing, changes context
 */
void etox_context_set_wrap_marker(Evas_Object *obj, char *marker, char *style)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	IF_FREE(et->context->marker.text);
	IF_FREE(et->context->marker.style);
	et->context->marker.text = strdup(marker);
	et->context->marker.style = strdup(style);
}

/*
 * etox_set_wrap_marker_color - sets color of wrap marker
 * @et: the etox to set for
 * @r: red value
 * @g: green value
 * @b: blue value
 * @a: alpha value
 *
 * Returns nothing, changes context
 */
void
etox_context_set_wrap_marker_color(Evas_Object *obj, int r, int g, int b, int a)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->context->marker.r = r;
	et->context->marker.g = g;
	et->context->marker.b = b;
	et->context->marker.a = a;
}
