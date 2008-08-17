#include "../config.h"
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
	context->marker.text = strdup("+");
	context->marker.style = strdup("plain");
	context->marker.r = 255;
	context->marker.g = 0;
	context->marker.b = 0;
	context->marker.a = 255;

	return context;
}

/**
 * etox_context_copy - copy the contents of one context into another
 * @dst: the destination context to receive the contents
 * @src: the source context that is copied into the destination
 *
 * Returns no value. Copies the context data from @src to @dst.
 */
void etox_context_copy(Etox_Context *dst, Etox_Context *src)
{

	CHECK_PARAM_POINTER("dst", dst);
	CHECK_PARAM_POINTER("src", src);

	/*
	 * Dereference the style before overwriting
	 */
	IF_FREE(dst->style);
	IF_FREE(dst->font);
	IF_FREE(dst->marker.text);
	IF_FREE(dst->marker.style);

	/*
	 * Copy the contents of the old context
	 */
	memcpy(dst, src, sizeof(Etox_Context));
	if (src->style)
		dst->style = strdup(src->style);
	else
		dst->style = NULL;

	if (src->font)
		dst->font = strdup(src->font);

	if (src->marker.text)
		dst->marker.text = strdup(src->marker.text);

	if (src->marker.style)
		dst->marker.style = strdup(src->marker.style);

        dst->flags = src->flags;

	return;
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

	ret = (Etox_Context *) calloc(sizeof(Etox_Context), 1);
	etox_context_copy(ret, et->context);

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

	etox_context_copy(et->context, context);
}

/**
 * etox_context_get - get a pointer to the current context of an etox
 * @et: the etox to retrieve the current context
 *
 * Returns a pointer to the current context. 
 */
Etox_Context *etox_get_context(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

        return et->context;
}

/**
 * etox_context_set - set the current context of an etox
 * @et: the etox 
 * @context: the context
 *
 * Set the current context of an etox. 
 */
void etox_set_context(Evas_Object * obj, Etox_Context *context)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("context", context);

	et = evas_object_smart_data_get(obj);

        et->context = context;
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
void etox_context_get_color(Etox_Context * context, int *r, int *g, int *b, int *a)
{
	CHECK_PARAM_POINTER("context", context);

	*a = context->a;
	*r = context->r;
	*b = context->b;
	*g = context->g;
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
void etox_context_set_color(Etox_Context * context, int r, int g, int b, int a)
{
	CHECK_PARAM_POINTER("context", context);

	context->a = a;
	context->r = r;
	context->g = g;
	context->b = b;
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
void etox_context_set_color_db(Etox_Context * context, char *name)
{
	CHECK_PARAM_POINTER("context", context);

	etox_style_lookup_color_db(name, &context->r, &context->g,
			       &context->b, &context->a);
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
void etox_context_set_font(Etox_Context * context, char *name, int size)
{
	CHECK_PARAM_POINTER("context", context);
	CHECK_PARAM_POINTER("name", name);

	IF_FREE(context->font);
	context->font = strdup(name);
	context->font_size = size;
}

 /**
  * etox_context_get_font - get the default font for the etox
  * @et: the etox to get the font from
  * @size: where the size will be placed
  * @return Returns the font name on success, NULL on failure
  */
char *etox_context_get_font(Etox_Context *context, int *size)
{
	CHECK_PARAM_POINTER_RETURN("context", context, NULL);

    *size = context->font_size;

    if (!context->font)
        return NULL;

    return strdup(context->font);
}

/**
 * etox_context_get_style - retrieve the name of current style
 * @et: the etox to query for current style.
 *
 * Returns a pointer to the style name on success, NULL on failure.
 */
char *etox_context_get_style(Etox_Context * context)
{
	CHECK_PARAM_POINTER_RETURN("context", context, NULL);

	if (!context->style)
		return NULL;

	return strdup(context->style);
}

/**
 * etox_context_set_style - change the default style of the etox
 * @et: the etox to change the default style
 * @name: the name of the style to be used
 *
 * Returns no value. Changes the default style used by the etox to the one
 * represented by @name.
 */
void etox_context_set_style(Etox_Context * context, char *name)
{
	CHECK_PARAM_POINTER("context", context);

	/*
	 * Release this instance of the old style, and then get an instance of
	 * the new style.
	 */
	IF_FREE(context->style);

	if (name && *name)
		context->style = strdup(name);
	else
		context->style = NULL;
}

/*
 * etox_context_get_align - retrieve the current alignment of an etox
 * @et: the etox to query for alignment
 *
 * Returns the alignment value for @et on success, NULL on failure.
 */
int etox_context_get_align(Etox_Context * context)
{
	CHECK_PARAM_POINTER_RETURN("context", context, FALSE);

	return context->flags & ETOX_ALIGN_MASK;
}

/*
 * etox_context_set_align - set the current alignment for an etox
 * @et: the etox to change the alignment
 *
 * Returns no value. Changes @et's current context's alignment value to
 * @align.
 */
void etox_context_set_align(Etox_Context * context, int align)
{
	CHECK_PARAM_POINTER("context", context);

	context->flags = align | (context->flags & ~ETOX_ALIGN_MASK);
}

/**
 * etox_set_wrap_marker - sets string to mark wrapped lines
 * @et: the etox to set for
 * @marker: the string to mark wrapped lines
 * @style: the style of the marker
 *
 * Returns nothing, changes context
 */
void etox_context_set_wrap_marker(Etox_Context * context, char *marker, char *style)
{
	CHECK_PARAM_POINTER("context", context);

	IF_FREE(context->marker.text);
	IF_FREE(context->marker.style);
	if (marker)
		context->marker.text = strdup(marker);
	if (style)
		context->marker.style = strdup(style);
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
etox_context_set_wrap_marker_color(Etox_Context * context, int r, int g, int b, int a)
{
	CHECK_PARAM_POINTER("context", context);

	context->marker.r = r;
	context->marker.g = g;
	context->marker.b = b;
	context->marker.a = a;
}

/*
 * @param context: the context to change current wrapping marker
 * @param place: the placement to use for the wrap marker
 */
void
etox_context_set_wrap_marker_place(Etox_Context *context,
				   Etox_Marker_Placement place)
{
	CHECK_PARAM_POINTER("context", context);

	context->marker.placement = place;
}

Etox_Marker_Placement
etox_context_get_wrap_marker_place(Etox_Context *context)
{
	CHECK_PARAM_POINTER_RETURN("context", context, 0);

	return context->marker.placement;
}
