#include "Estyle_private.h"
#include <ctype.h>
#include <Evas.h>

#define ESTYLE_TABWIDTH 8
#define DEFAULT_FONT "nationff"
#define DEFAULT_SIZE 12

void __estyle_update_position(Estyle *es);
void __estyle_update_dimensions(Estyle *es);
void __estyle_callback_dispatcher(void *_data, Evas *_e, Evas_Object *_o,
				  void *event_info);

static int estyle_setup_complete = 0;

/**
 * estyle_new - create a estyle with specified text
 * @evas: the evas to display the text
 * @text: the text to use when creating the evas object
 * @style: the style to display the text
 *
 * Returns a pointer to the newly allocated estyle on success, NULL on
 * failure. The estyle doesn't get displayed until estyle_show gets called.
 */
Estyle *estyle_new(Evas *evas, char *text, char *style)
{
	Estyle *es;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Check to see if the internal data structures has been completed.
	 */
	if (!estyle_setup_complete) {
		_estyle_color_init();
		_estyle_style_add_path(PACKAGE_DATA_DIR "/styles");
		estyle_setup_complete = 1;
	}

	es = (Estyle *) malloc(sizeof(Estyle));
	if (!es)
		return NULL;
	memset(es, 0, sizeof(Estyle));

	es->evas = evas;
	evas_font_path_append(es->evas, PACKAGE_DATA_DIR"/fonts");

	/*
	 * Create the Evas_Object and set the default font and font size.
	 */
	es->bit = evas_object_text_add(es->evas);
	evas_object_text_font_set(es->bit,DEFAULT_FONT,DEFAULT_SIZE);
	evas_object_text_text_set(es->bit,text);

	/*
	 * Set some default colors, font, and font size.
	 */
	es->color = _estyle_color_instance(255, 255, 255, 255);

	/*
	 * Create the style and set the default color.
	 */
	if (style)
		es->style = _estyle_style_instance(style);
	estyle_set_color(es, 255, 255, 255, 255);
	if (es->style)
		_estyle_style_draw(es, text);

	/*
	 * Set length and dimensions.
	 */
	es->length = strlen(text);
	__estyle_update_dimensions(es);

	/*
	 * move to (0,0) by default. (is this necessary? -redalb)
	 */
	/* estyle_move(es, 0, 0); */

	return es;
}

/**
 * estyle_free - free the data associated with a estyle
 * @es: the estyle to free along with its data
 *
 * Returns no value. Frees the data in @es as well as @es itself.
 */
void estyle_free(Estyle *es)
{
	Evas_List *l;

	CHECK_PARAM_POINTER("es", es);

	if (es->bit)
		evas_object_del(es->bit);

	if (es->style)
		_estyle_style_release(es->style, es->evas);

	if (es->callbacks) {
		for (l = es->callbacks; l; l = l->next)
			FREE(l->data);
		evas_list_free(es->callbacks);
	}

	FREE(es);
}

/**
 * estyle_show - display an estyle or any changes made to a estyle
 * @estyle: the estyle that needs display updates
 *
 * Returns no value. Updates the display to reflect changes to @estyle.
 */
void estyle_show(Estyle *es)
{
	CHECK_PARAM_POINTER("es", es);

	es->flags |= ESTYLE_BIT_VISIBLE;
	_estyle_style_show(es);
	evas_object_show(es->bit);
}


/**
 * estyle_hide - hide the estyle from the evas 
 * @es: the estyle to hide
 *
 * Returns no value
 */
void estyle_hide(Estyle *es)
{
	CHECK_PARAM_POINTER("es", es);

	es->flags &= ~ESTYLE_BIT_VISIBLE;
	_estyle_style_hide(es);
	evas_object_hide(es->bit);
}

/**
 * estyle_move - move the estyle into the correct position
 * @es: the estyle to be moved
 * @x: the new x coordinate for the estyle
 * @y: the new y coordinate for the estyle
 *
 * Returns no value. Moves the requested estyle into the requested position.
 */
void estyle_move(Estyle * es, int x, int y)
{
	CHECK_PARAM_POINTER("es", es);

	if (es->x == x && es->y == y)
		return;

	es->x = x;
	es->y = y;
	evas_object_move(es->bit, 
	      (double)(x + (es->style ? es->style->info->left_push : 0)),
	      (double)(y + (es->style ? es->style->info->top_push : 0)));
	
	if (es->style)
		_estyle_style_move(es);
}

/**
 * estyle_get_color - get the color of the estyle
 * @es: the estyle to get the color
 * @r: the pointer to the int to store the red value of the estyle
 * @g: the pointer to the int to store the green value of the estyle
 * @b: the pointer to the int to store the blue value of the estyle
 * @a: the pointer to the int to store the alpha value of the estyle
 *
 * Returns no value. Retrieves the color of the estyle into the integer pointers
 * @r, @g, @b and @a.
 */
void estyle_get_color(Estyle * es, int *r, int *g, int *b, int *a)
{
	CHECK_PARAM_POINTER("es", es);

	if (r)
		*r = es->color->r;
	if (g)
		*g = es->color->g;
	if (b)
		*b = es->color->b;
	if (a)
		*a = es->color->a;
}

/**
 * estyle_set_color - change the color of the estyle
 * @es: the estyle to change color
 * @r: the red value of the estyle
 * @g: the green value of the estyle
 * @b: the blue value of the estyle
 * @a: the alpha value of the estyle
 *
 * Returns no value. Changes the color of the estyle to that represented by
 * @r, @g, @b and @a.
 */
void estyle_set_color(Estyle * es, int r, int g, int b, int a)
{
	CHECK_PARAM_POINTER("es", es);

	es->color = _estyle_color_instance(r, g, b, a);
	evas_object_color_set(es->bit, r, g, b, a);
	_estyle_style_set_color(es);
}

/**
 * estyle_lookup_color_db - lookup a colors values by name in color db
 * @name: the name of the color to lookup in the color db
 * @r: the pointer to the int to store the red value of the estyle
 * @g: the pointer to the int to store the green value of the estyle
 * @b: the pointer to the int to store the blue value of the estyle
 * @a: the pointer to the int to store the alpha value of the estyle
 *
 * Returns no value. Looks up the color name in the color db and stores the
 * values in the integers pointed to by @r, @g, @b, and @a.
 */
void estyle_lookup_color_db(char *name, int *r, int *g, int *b, int *a)
{
	Estyle_Color *color;

	CHECK_PARAM_POINTER("name", name);

	color = _estyle_color_instance_db(name);

	if (r)
		*r = color->r;
	if (g)
		*g = color->g;
	if (b)
		*b = color->b;
	if (a)
		*a = color->a;
}

/**
 * estyle_set_color_db - set the color of an estyle based on color name
 * @es: the estyle to change the color
 */
void estyle_set_color_db(Estyle *es, char *name)
{
	CHECK_PARAM_POINTER("es", es);
	CHECK_PARAM_POINTER("name", name);

	es->color = _estyle_color_instance_db(name);
	evas_object_color_set(es->bit, es->color->r, es->color->g,
			es->color->b, es->color->a);
	if (es->style)
		_estyle_style_set_color(es);
}

/**
 * estyle_get_style - get the style of the specified estyle
 * @es: the estyle to get the style of
 *
 * Returns the name the style or NULL if no style specified.
 */
char *estyle_get_style(Estyle *es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, NULL);

	if (!es->style)
		return NULL;

	return es->style->info->name;
}

/**
 * estyle_set_style - change the style of the specified estyle
 * @es: the estyle to change style
 * @name: the name of the style to set for the estyle
 *
 * Returns no value. The style of @es is changed to @name.
 */
void estyle_set_style(Estyle *es, char *name)
{
	int layer;
	const char *text;
	Evas_Object *clip;

	CHECK_PARAM_POINTER("es", es);
	CHECK_PARAM_POINTER("name", name);

	/*
	 * Set the style for this particular es.
	 */
	if (es->style)
		_estyle_style_release(es->style, es->evas);

	if ((es->style = _estyle_style_instance(name)) == NULL)
		return;

	text = evas_object_text_text_get(es->bit);

	/*
	 * Draw the new style bits and set the correct layer for all of the
	 * estyle contents. Also clip the contents to the clip rectangle.
	 */
	_estyle_style_draw(es, (char *)text);

	layer = evas_object_layer_get(es->bit);
	estyle_set_layer(es, layer);

	_estyle_style_move(es);

	if ((clip = evas_object_clip_get(es->bit)) != NULL)
		_estyle_style_set_clip(es, clip);

	__estyle_update_position(es);
	__estyle_update_dimensions(es);

	if (es->flags & ESTYLE_BIT_VISIBLE)
		_estyle_style_show(es);
}

/**
 * estyle_get_text - retrieve the text in the specified estyle
 * @es: the estyle to retrieve text
 *
 * Returns a pointer to a copy of the text in the estyle @es on success, NULL
 * on failure.
 */
char *estyle_get_text(Estyle * es)
{
	const char *ret;

	CHECK_PARAM_POINTER_RETURN("es", es, NULL);

	ret = evas_object_text_text_get(es->bit);
	if (ret)
		ret = strdup(ret);

	return (char *)ret;
}

/**
 * estyle_set_text - change the text on a specified estyle
 * @es: the estyle to change text
 * @text: the new text for the estyle
 *
 * Returns no value. Changes the text in @es to @text and updates the display
 * if visible.
 */
void estyle_set_text(Estyle *es, char *text)
{
	CHECK_PARAM_POINTER("es", es);

	evas_object_text_text_set(es->bit, text);

	/*
	 * Set the text for the style bits.
	 */
	if (es->style)
		_estyle_style_set_text(es);

	/*
	 * Set new length and dimensions.
	 */
	es->length = strlen(text);
	__estyle_update_dimensions(es);
}

/**
 * estyle_get_layer - retrieve the layer of the estyle
 * @es: the estyle to retrieve the current layer
 *
 * Returns the current layer of the estyle @es.
 */
int estyle_get_layer(Estyle * es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, FALSE);

	return evas_object_layer_get(es->bit);
}

/**
 * estyle_set_layer - change the layer of a estyle and it's style
 * @es: the estyle to change layer
 * @layer: the new layer for the estyle
 *
 * Returns no value. Changes the layer of @es to @layer along with all of
 * it's style bits.
 */
void estyle_set_layer(Estyle *es, int layer)
{
	int index = 0;

	CHECK_PARAM_POINTER("es", es);

	evas_object_layer_set(es->bit, layer - 1);

	/*
	 * Changing the layer of the style bits must be broken into to steps to
	 * ensure the relative ordering remains the same.
	 */
	if (es->style)
		index = _estyle_style_set_layer_lower(es, layer);

	evas_object_layer_set(es->bit, layer);

	if (es->style)
		_estyle_style_set_layer_upper(es, layer, index);
}

/**
 * estyle_get_font - retrieve a copy of the current font for an estyle
 * @es: the estyle to retrieve the current font
 *
 * Returns a pointer to a copy of the current font of the estyle @es.
 */
char *estyle_get_font(Estyle * es)
{
	char *font;
	double size;
	CHECK_PARAM_POINTER_RETURN("es", es, NULL);
	evas_object_text_font_get(es->bit, &font, &size);
	return font;
}

/**
 * estyle_get_font_size - retrieve the font size
 * @es: the estyle to get the font size from
 *
 * Returns the font size (an int) or FALSE if an error occurs.
 */  
int estyle_get_font_size(Estyle *es)
{
	char *font;
	double size;
	CHECK_PARAM_POINTER_RETURN("es", es, 0);
	evas_object_text_font_get(es->bit, &font, &size);
	return (int)size;
}

/**
 * estyle_set_font - change the font used for the specified estyle
 * @es: the estyle to change fonts
 * @font: the name of the font to use for the estyle
 *
 * Returns no value. Changes the font for the specified estyle to @name.
 */
void estyle_set_font(Estyle *es, char *font, int size)
{
	CHECK_PARAM_POINTER("es", es);
	CHECK_PARAM_POINTER("font", font);

	evas_object_text_font_set(es->bit, font, size);
	if (es->style)
		_estyle_style_set_font(es, font, size);

	__estyle_update_dimensions(es);
}

/**
 * estyle_get_clip - get the clip box of an estyle
 * @es: the estyle to get the clip box from
 *
 * Returns an Evas_Object or NULL if no clip box specified.
 */
Evas_Object *estyle_get_clip(Estyle *es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, NULL);

	return evas_object_clip_get(es->bit);
} 

/**
 * estyle_set_clip - set the clip box for the estyle
 * @es: the estyle to set the clip box
 * @clip: the clip box to set
 *
 * Returns no value. Sets the clip box of the estyle to @clip.
 */
void estyle_set_clip(Estyle *es, Evas_Object *clip)
{
	CHECK_PARAM_POINTER("es", es);

	if (clip)
		evas_object_clip_set(es->bit, clip);
	else
		evas_object_clip_unset(es->bit);

	if (es->style)
		_estyle_style_set_clip(es, clip);
}

/**
 * estyle_merge - merge two estyles into a single estyle
 * @es1: the destination estyle that will contain the final text
 * @es2: the estyle that will be freed, but it's text will be added to @es1
 *
 * Returns TRUE if the bits are able to be merged, otherwise FALSE. Appends the
 * text in @es2 to the text in @es1 and destroys @es2.
 */
int estyle_merge(Estyle *es1, Estyle *es2)
{
	char *new_text, *text1, *text2;

	CHECK_PARAM_POINTER_RETURN("es1", es1, FALSE);
	CHECK_PARAM_POINTER_RETURN("es2", es2, FALSE);

	if (!BIT_MERGEABLE(es1, es2))
		return FALSE;

	/*
	 * These return a pointer to the actual text in the evas object, not a
	 * copy of it. So don't free them.
	 */
	text1 = (char *)evas_object_text_text_get(es1->bit);
	text2 = (char *)evas_object_text_text_get(es2->bit);

	/*
	 * When the evas changes text it free's the old text, so malloc a new
	 * text area set the evas object with it, and then free that string
	 * since evas will strdup it.
	 */
	new_text = (char *) malloc(strlen(text1) + strlen(text2) + 1);
	if (!new_text)
		return FALSE;

	strcpy(new_text, text1);
	strcat(new_text, text2);

	estyle_set_text(es1, new_text);

	FREE(new_text);

	estyle_free(es2);

	return TRUE;
}

/**
 * estyle_split - break a estyle into multiple estyles at a specified index
 * @es: the estyle to be split
 * @index: the index into the text to be split
 *
 * Returns the newly created estyle on success, NULL on failure. The old
 * estyle is updated to reflect the changes.
 */
Estyle *estyle_split(Estyle *es, unsigned int index)
{
	char temp;
	char *content;
	Estyle *new_es;
	int r, g, b, a;

	CHECK_PARAM_POINTER_RETURN("es", es, NULL);

	/*
	 * Don't bother to split if it will result in an empty string.
	 */
	if (!index)
		return NULL;

	/*
	 * Retrieve the contents of the estyle to be split.
	 */
	content = estyle_get_text(es);
	if (!content)
		return NULL;

	/*
	 * Check if the index is within the bounds of the string, and won't
	 * result in an empty string.
	 */
	if (index >= strlen(content)) {
		FREE(content);
		return NULL;
	}

	/*
	 * Terminate the string at the index and set the old estyle's content to
	 * the resulting string.
	 */
	temp = content[index];
	content[index] = '\0';
	estyle_set_text(es, content);

	/*
	 * Now place the content from index onwards into a new estyle.
	 */
	content[index] = temp;
	new_es =
	    estyle_new(es->evas, &(content[index]), es->style->info->name);

	/*
	 * Give this new estyle the same settings as the previous estyle.
	 */
	estyle_set_clip(new_es, estyle_get_clip(es));
	estyle_set_font(new_es, estyle_get_font(es),
			estyle_get_font_size(es));

	/*
	 * Set the new color for the estyle.
	 */
	estyle_get_color(es, &r, &g, &b, &a);
	estyle_set_color(new_es, r, g, b, a);

	/*
	 * Put it on the same layer as the old estyle
	 */
	estyle_set_layer(new_es, estyle_get_layer(es));

	if (evas_object_visible_get(es->bit))
		estyle_show(new_es);

	FREE(content);

	return new_es;
}

/**
 * estyle_length - retrieve the length of the text in an estyle
 * @es: the estyle to retrieve the length
 *
 * Returns the length of the text in the estyle @es.
 */
inline int estyle_length(Estyle * es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, FALSE);

	return es->length;
}

/**
 * estyle_geometry - retrieve the dimensions of the estyle
 * @es: the estyle to retrieve dimensions
 * @x: a pointer to an integer to store the x coordinate of the estyle
 * @x: a pointer to an integer to store the y coordinate of the estyle
 * @w: a pointer to an integer to store the width of the estyle
 * @h: a pointer to an integer to store the height of the estyle
 *
 * Returns no value. Stores the current x, y coordinates and the width, height
 * dimensions into @x, @y, @w, and @h respectively.
 */
void estyle_geometry(Estyle * es, int *x, int *y, int *w, int *h)
{
	CHECK_PARAM_POINTER("es", es);

	if (x)
		*x = es->x;
	if (y)
		*y = es->y;
	if (w)
		*w = es->w;
	if (h)
		*h = es->h;
}

/**
 * estyle_text_at - retrieve the character geometry for character at an index
 * @es: the estyle to search for the character
 * @index: the position in the estyle of the character
 * @char_x: a pointer to the integer to store the x coordinate of the character
 * @char_y: a pointer to the integer to store the y coordinate of the character
 * @char_w: a pointer to the integer to store the width of the character
 * @char_h: a pointer to the integer to store the height of the character
 *
 * Returns no value. The dimensions of the found character are stored in
 * @char_x, @char_y, @char_w, and @char_h.
 */
void estyle_text_at(Estyle * es, int index, int *char_x, int *char_y,
		    int *char_w, int *char_h)
{
	double xx, yy, ww, hh;

	CHECK_PARAM_POINTER("es", es);

	evas_object_text_char_pos_get(es->bit, index,  &xx, &yy, &ww, &hh);

	if (char_x)
		*char_x = D2I_ROUND(xx) + es->x;
	if (char_y)
		*char_y = D2I_ROUND(yy) + es->y;
	if (char_w)
		*char_w = D2I_ROUND(ww) + (es->style ? 
			es->style->info->left_push
			+ es->style->info->right_push : 0);
        if (char_h)
		*char_h = D2I_ROUND(hh) + (es->style ? 
			es->style->info->top_push
			+ es->style->info->bottom_push : 0);
}

/**
 * estyle_text_at_position - find the text at the given coords
 * @es: the estyle to check for text intersects coordinates
 * @x: the x coordinate to check for intersecting character
 * @y: the y coordinate to check for intersecting character
 * @char_x: a pointer to the integer to store the found characters x coord
 * @char_y: a pointer to the integer to store the found characters y coord
 * @char_w: a pointer to the integer to store the found characters width
 * @char_h: a pointer to the integer to store the found characters height
 *
 * Returns -1 on failure, the index of the found character on success. The
 * dimensions of the found character are stored in @char_x, @char_y, @char_w,
 * and @char_h.
 */
int estyle_text_at_position(Estyle * es, int x, int y, int *char_x,
			    int *char_y, int *char_w, int *char_h)
{
	int ret;
	double xx = 0, yy = 0, ww = 0, hh = 0;

	CHECK_PARAM_POINTER_RETURN("es", es, 0);

	ret = evas_object_text_char_coords_get(es->bit, (double)(x - es->x),
			(double)(y - es->y), &xx, &yy, &ww, &hh);

	if (char_x)
		*char_x = D2I_ROUND(xx) + es->x;
	if (char_y)
		*char_y = D2I_ROUND(yy) + es->y;
	if (char_w)
		*char_w = D2I_ROUND(ww) + (es->style ? 
			es->style->info->left_push
			+ es->style->info->right_push : 0);
	if (char_h)
		*char_h = D2I_ROUND(hh) + (es->style ? 
			es->style->info->top_push
			+ es->style->info->bottom_push : 0);

	return ret;
}

/**
 * estyle_fixed - determine if the specified estyle has fixed geometry
 * @es: the estyle to check for fixed geometry
 *
 * Returns 0 if the estyle does not have fixed geometry, > 0 otherwise.
 */
inline int estyle_fixed(Estyle * es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, 0);

	return es->flags & ESTYLE_BIT_FIXED;
}

/**
 * estyle_fix_geometry - report a fixed geometry to querying functions
 * @es: the estyle to fix the geometry
 * @x: the x coordinate to report
 * @y: the y coordinate to report
 * @w: the width to report
 * @h: the height to report
 *
 * Returns no value. Fixes the reported geometry of @es to @x, @y, @w, and @h
 * respectively.
 */
void estyle_fix_geometry(Estyle * es, int x, int y, int w, int h)
{
	CHECK_PARAM_POINTER("es", es);

	es->flags |= ESTYLE_BIT_FIXED;
	es->x = x;
	es->y = y;
	es->w = w;
	es->h = h;
}

/**
 * estyle_unfix_geometry - remove the fixed property of the estyle
 * @es: the estyle to remove the fixed property
 *
 * Returns no value. Removes the fixed property of the estyle and updates its
 * geometry to the actual geometry of the contents.
 */
void estyle_unfix_geometry(Estyle * es)
{
	double x, y, w, h;

	CHECK_PARAM_POINTER("es", es);

	evas_object_geometry_get(es->bit, &x, &y, &w, &h);
	es->x = D2I_ROUND(x);
	es->y = D2I_ROUND(y);
	es->w = D2I_ROUND(w);
	es->h = D2I_ROUND(h);
}

/*
 * __estyle_update_position - update the position of the estyle
 */
void __estyle_update_position(Estyle *es)
{
	if (es->style)
		evas_object_move(es->bit,
			(double)(es->x + (es->style ?
				es->style->info->left_push : 0)),
			(double)(es->y + (es->style ?
				es->style->info->top_push : 0)));
}

/*
 * __estyle_update_dimensions - update the dimensions of the estyle
 */
void __estyle_update_dimensions(Estyle *es)
{
	double x, y, w, h;
	
	/*
	 * If the estyle doesn't have fixed dimensions then set it to the
	 * geometry of it's contents.
	 */
	if (!(es->flags & ESTYLE_BIT_FIXED)) {
		evas_object_geometry_get(es->bit, &x, &y, &w, &h);

		es->w = D2I_ROUND(w) + (es->style ? es->style->info->left_push
			+ es->style->info->right_push : 0);
		es->h = D2I_ROUND(h) + (es->style ? es->style->info->top_push
			+ es->style->info->bottom_push : 0);
	}
}

/**
 * estyle_callback_add - add a callback to the desired estyle
 * @es: the estyle to add the callback
 * @callback: the event type that triggers the callback function
 * @func: the function to call when the event occurs
 * @data: the data to pass to the function when called
 *
 * Returns no value. Adds a callback to @es that will be triggered when event
 * @callback occurs, function @func will be called with argument @data.
 */
void estyle_callback_add(Estyle * es, Evas_Callback_Type callback,
			 void (*func) (void *_data, Estyle * _es, 
				       void *event_info), void *data)
{
	Estyle_Callback *cb;
	Evas_List *l;

	if (!es)
		return;
	if (!es->evas)
		return;

	/*
	 * Allocate the new estyle callback
	 */
	cb = malloc(sizeof(Estyle_Callback));
	cb->estyle = es;
	cb->type = callback;
	cb->data = data;
	cb->callback = func;


	/*
	 * Append the callback to the estyle's list, and add the callback to
	 * the evas bits so that the wrapper function gets called.
	 */
	es->callbacks = evas_list_append(es->callbacks, (const void *)cb);
	evas_object_event_callback_add(es->bit, callback,
			  __estyle_callback_dispatcher, cb);
	if (es->style->bits) {
		for (l = es->style->bits; l; l = l->next) {
			Evas_Object *o;

			o = l->data;
			evas_object_event_callback_add(o, callback,
					__estyle_callback_dispatcher, cb);
		}
	}
}

/*
 * The dispatcher redirects the callback to the appropriate function with the
 * correct arguments.
 */
void __estyle_callback_dispatcher(void *_data, Evas *_e, Evas_Object *_o,
				  void *event_info)
{
	Estyle_Callback *cb = _data;
	cb->callback(cb->data, cb->estyle, event_info);
}

/*
 * estyle_callback_del - remove all callbacks of the type @callback from estyle
 * @es: the estyle to delete the callback
 * @callback: the event type to be removed
 *
 * Returns no value.
 */
void estyle_callback_del(Estyle * es, Evas_Callback_Type callback,
		Estyle_Callback_Function func)
{
	Evas_List *l;

	if (!es)
		return;
	if (!es->evas)
		return;

	for (l = es->callbacks; l; l = l->next) {
		Estyle_Callback *cb;

		cb = l->data;
		if (cb->type == callback && cb->callback == func) {

			/*
			 * Move to the next item in the list, remove and free
			 * the matched item.
			 */
			l = l->next;
			es->callbacks = evas_list_remove(es->callbacks, cb);
			FREE(cb);
		}
	}
}
