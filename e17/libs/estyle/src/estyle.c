#include "estyle-config.h"
#include "Estyle_private.h"
#include <ctype.h>

#define ESTYLE_TABWIDTH 8

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
Estyle *estyle_new(Evas evas, char *text, char *style)
{
	Estyle *es;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Check to see if the internal data structures has been completed.
	 */
	if (!estyle_setup_complete) {
		estyle_color_init();
		estyle_style_add_path(PACKAGE_DATA_DIR "/styles");
		estyle_setup_complete = 1;
	}

	es = (Estyle *) malloc(sizeof(Estyle));
	if (!es)
		return NULL;

	memset(es, 0, sizeof(Estyle));
	es->evas = evas;

	/*
	 * Set some default colors, font, and font size.
	 */
	es->color = estyle_color_instance(255, 255, 255, 255);
	es->font = estyle_font_instance("nationff");
	es->font->size = 14;

	if (style)
		es->style = estyle_style_instance(style);

	/*
	 * Display the text on the evas
	 */
	evas_font_add_path(evas, PACKAGE_DATA_DIR"/fonts");
	estyle_set_text(es, text);

	return es;
}

/**
 * estyle_free - free the data associated with a estyle
 * @es: the estyle to free along with its data
 *
 * Returns no value. Frees the data in @es as well as @es itself.
 */
void estyle_free(Estyle * es)
{
	CHECK_PARAM_POINTER("es", es);

	if (es->bit)
		evas_del_object(es->evas, es->bit);

	if (es->style)
		estyle_style_release(es->style, es->evas);

	FREE(es);
}

/**
 * estyle_show - display an estyle or any changes made to a estyle
 * @estyle: the estyle that needs display updates
 *
 * Returns no value. Updates the display to reflect changes to @estyle.
 */
void estyle_show(Estyle * es)
{
	CHECK_PARAM_POINTER("es", es);

	es->flags |= ESTYLE_BIT_VISIBLE;
	estyle_style_show(es);
	evas_show(es->evas, es->bit);
}


/**
 * estyle_hide - hide the estyle from the evas 
 * @es: the estyle to hide
 *
 * Returns no value
 */
void estyle_hide(Estyle * es)
{
	CHECK_PARAM_POINTER("es", es);

	es->flags &= ~ESTYLE_BIT_VISIBLE;
	estyle_style_hide(es);
	evas_hide(es->evas, es->bit);
}

/**
 * estyle_move - move the estyle into the correct position
 * @es: the estyle to be moved
 * @x: the new x coordinate for the estyle
 * @y: the new y coordinate for the estyle
 *
 * Returns no value. Moves the requested estyle into the requested position.
 */
void estyle_move(Estyle *es, int x, int y)
{
	CHECK_PARAM_POINTER("es", es);

	es->x = x;
	es->y = y;
	evas_move(es->evas, es->bit, (double)(x), (double)(y));
	estyle_style_move(es);
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
void estyle_set_color(Estyle *es, int r, int g, int b, int a)
{
	CHECK_PARAM_POINTER("es", es);

	es->color = estyle_color_instance(r, g, b, a);
	evas_set_color(es->evas, es->bit, r, g, b, a);
	estyle_style_set_color(es);
}

/**
 * estyle_set_color_db - set the color of an estyle based on color name
 * @es: the estyle to change the color
 */
void estyle_set_color_db(Estyle * es, char *name)
{
	CHECK_PARAM_POINTER("es", es);
	CHECK_PARAM_POINTER("name", name);

	es->color = estyle_color_instance_db(name);
	evas_set_color(es->evas, es->bit, es->color->r, es->color->g,
			es->color->b, es->color->a);
	estyle_style_set_color(es);
}

/**
 * estyle_set_style - change the style of the specified estyle
 * @es: the estyle to change style
 * @name: the name of the style to set for the estyle
 *
 * Returns no value. The style of @es is changed to @name.
 */
void estyle_set_style(Estyle * es, char *name)
{
	int layer;
	char *text;
	Evas_Object clip;

	CHECK_PARAM_POINTER("es", es);

	/*
	 * Set the style for this particular es.
	 */
	if (es->style)
		estyle_style_release(es->style, es->evas);

	if (!name)
		return;

	es->style = estyle_style_instance(name);

	text = evas_get_text_string(es->evas, es->bit);

	/*
	 * Draw the new style bits and set the correct layer for all of the
	 * estyle contents. Also clip the contents to the clip rectangle.
	 */
	estyle_style_draw(es, text);

	layer = evas_get_layer(es->evas, es->bit);
	estyle_set_layer(es, layer);

	estyle_style_move(es);

	clip = evas_get_clip_object(es->evas, es->bit);
	estyle_style_set_clip(es, clip);

	if (es->flags & ESTYLE_BIT_VISIBLE)
		estyle_style_show(es);
}

/**
 * estyle_set_text - change the text on a specified estyle
 * @es: the estyle to change text
 * @text: the new text for the estyle
 *
 * Returns no value. Changes the text in @es to @text and updates the display
 * if visible.
 */
void estyle_set_text(Estyle * es, char *text)
{
	double x, y, w, h;

	CHECK_PARAM_POINTER("es", es);

	/*
	 * Create the text if no evas object is present, otherwise just change
	 * of the evas object.
	 */
	if (es->bit)
		evas_set_text(es->evas, es->bit, text);

	else {
		es->bit = evas_add_text(es->evas, es->font->name,
					 es->font->size, text);
	}

	es->length = strlen(text);
	evas_get_geometry(es->evas, es->bit, &x, &y, &w, &h);

	/*
	 * This essentially performs a round operation, since
	 * casting a double's value to an int discards the decimal
	 * portion of the number. Adding 0.5 to the value will round
	 * it up if it is appropriate.
	 */
	es->w = (int) (w + 0.5);
	es->h = (int) (h + 0.5);

	evas_set_color(es->evas, es->bit, es->color->r, es->color->g,
			es->color->b, es->color->a);
	evas_move(es->evas, es->bit, (double)(es->x), (double)(es->y));

	/*
	 * Draw style altering bits below the text.
	 */
	estyle_style_draw(es, text);

	if (es->flags & ESTYLE_BIT_VISIBLE)
		estyle_style_show(es);
}

/**
 * estyle_set_layer - change the layer of a estyle and it's style
 * @es: the estyle to change layer
 * @layer: the new layer for the estyle
 *
 * Returns no value. Changes the layer of @es to @layer along with all of
 * it's style bits.
 */
void estyle_set_layer(Estyle * es, int layer)
{
	int index = 0;

	evas_set_layer(es->evas, es->bit, layer - 1);

	/*
	 * Changing the layer of the style bits must be broken into to steps to
	 * ensure the relative ordering remains the same.
	 */
	if (es->style)
		index = estyle_style_set_layer_lower(es, layer);

	evas_set_layer(es->evas, es->bit, layer);

	if (es->style)
		estyle_style_set_layer_upper(es, layer, index);
}

/**
 * estyle_set_font - change the font used for the specified estyle
 * @es: the estyle to change fonts
 * @font: the name of the font to use for the estyle
 *
 * Returns no value. Changes the font for the specified estyle to @name.
 */
void estyle_set_font(Estyle * es, char *name)
{
	CHECK_PARAM_POINTER("es", es);
	CHECK_PARAM_POINTER("name", name);
}

/**
 * estyle_set_clip - set the clip box for the estyle
 * @es: the estyle to set the clip box
 * @clip: the clip box to set
 *
 * Returns no value. Sets the clip box of the estyle to @clip.
 */
void estyle_set_clip(Estyle *es, Evas_Object clip)
{
	CHECK_PARAM_POINTER("es", es);

	if (clip)
		evas_set_clip(es->evas, es->bit, clip);
	else
		evas_unset_clip(es->evas, es->bit);

	estyle_style_set_clip(es, clip);
}

/**
 * estyle_merge - merge two estyles into a single estyle
 * @es1: the destination estyle that will contain the final text
 * @es2: the estyle that will be freed, but it's text will be added to @es1
 *
 * Returns no value. Appends the text in @es2 to the text in @es1 and destroys
 * @es2.
 */
void estyle_merge(Estyle * es1, Estyle * es2)
{
	char *new_text, *text1, *text2;

	CHECK_PARAM_POINTER("es1", es1);
	CHECK_PARAM_POINTER("es2", es2);

	/*
	 * These return a pointer to the actual text in the evas object, not a
	 * copy of it. So don't free them.
	 */
	text1 = evas_get_text_string(es1->evas, es1->bit);
	text2 = evas_get_text_string(es2->evas, es2->bit);

	/*
	 * When the evas changes text it free's the old text, so malloc a new
	 * text area set the evas object with it, and then free that string
	 * since evas will strdup it.
	 */
	new_text = (char *) malloc(strlen(text1) + strlen(text2) + 1);
	strcpy(new_text, text1);
	strcat(new_text, text2);

	estyle_set_text(es1, new_text);

	FREE(new_text);

	estyle_free(es2);
}

/**
 * estyle_split - break a estyle into multiple estyles at a specified index
 * @es: the estyle to be split
 * @index: the index into the text to be split
 *
 * Returns the newly created estyle on success, NULL on failure. The old
 * estyle is updated to reflect the changes.
 */
Estyle *estyle_split(Estyle * es, int index)
{
	char temp;
	char *content;
	Estyle *new_es;

	content = strdup(evas_get_text_string(es->evas, es->bit));
	if (strlen(content) > index)
		return NULL;

	temp = content[index];
	content[index] = '\0';
	estyle_set_text(es->bit, content);

	content[index] = temp;
	new_es = estyle_new(es->evas, &(content[index]), es->style->info->name);

	return new_es;
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
void estyle_geometry(Estyle *es, int *x, int *y, int *w, int *h)
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
void estyle_text_at(Estyle *es, int index, int *char_x, int *char_y,
		int *char_w, int *char_h)
{
	double xx, yy, ww, hh;

	*char_x = 0;
	*char_y = 0;
	*char_w = 0;
	*char_h = 0;

	evas_text_at(es->evas, es->bit, index, &xx, &yy, &ww, &hh);

	*char_x = (int)(xx + 0.5) + es->x;
	*char_y = (int)(yy + 0.5) + es->y;
	*char_w = (int)(ww + 0.5);
	*char_h = (int)(hh + 0.5);
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
int estyle_text_at_position(Estyle *es, int x, int y, int *char_x, int *char_y,
		int *char_w, int *char_h)
{
	int ret;
	double xx, yy, ww, hh;

	*char_x = 0;
	*char_y = 0;
	*char_w = 0;
	*char_h = 0;

	ret = evas_text_at_position(es->evas, es->bit, (double)(x - es->x),
			(double)(y - es->y), &xx, &yy, &ww, &hh);

	*char_x = (int)(xx + 0.5) + es->x;
	*char_y = (int)(yy + 0.5) + es->y;
	*char_w = (int)(ww + 0.5);
	*char_h = (int)(hh + 0.5);

	return ret;
}
