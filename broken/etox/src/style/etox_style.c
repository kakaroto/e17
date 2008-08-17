#include "etox_style_private.h"
#include <ctype.h>
#include <Evas.h>

#define ETOX_STYLE_TABWIDTH 8
#define DEFAULT_FONT "nationff"
#define DEFAULT_SIZE 12

//#define DEBUG ON

void __etox_style_update_position(Etox_Style *es);
void __etox_style_update_dimensions(Etox_Style *es);
void __etox_style_callback_dispatcher(void *_data, Evas *_e, Evas_Object *_o,
				  void *event_info);

static int etox_style_setup_complete = 0;
Evas_Smart *etox_style_smart = NULL;

/**
 * etox_style_new - create a etox_style with specified text
 * @evas: the evas to display the text
 * @text: the text to use when creating the evas object
 * @style: the style to display the text
 *
 * Returns a pointer to the newly allocated etox_style on success, NULL on
 * failure. The etox_style doesn't get displayed until etox_style_show gets called.
 */
Evas_Object *etox_style_new(Evas *evas, char *text, char *style)
{
	Etox_Style *es;
	Evas_Object *obj = NULL;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Check to see if the internal data structures has been completed.
	 */
	if (!etox_style_setup_complete) {
		etox_style_smart = evas_smart_new("etox_style_smart", NULL,
				etox_style_free, etox_style_set_layer, NULL,
				NULL, NULL, NULL,
				etox_style_move, etox_style_resize, etox_style_show,
				etox_style_hide, etox_style_set_color, etox_style_set_clip,
				etox_style_unset_clip, NULL);
		_etox_style_color_init();
		_etox_style_style_add_path(PACKAGE_DATA_DIR "/styles");
		etox_style_setup_complete = 1;
	}

	es = (Etox_Style *) malloc(sizeof(Etox_Style));
	if (!es)
		return NULL;
	memset(es, 0, sizeof(Etox_Style));

	es->evas = evas;

	/*
	 * Create the Evas_Object and set the default font and font size.
	 */
	es->bit = evas_object_text_add(es->evas);
	evas_object_repeat_events_set(es->bit, 1);
	evas_object_text_font_set(es->bit, DEFAULT_FONT, DEFAULT_SIZE);

	obj = evas_object_smart_add(evas, etox_style_smart);
	evas_object_smart_member_add(es->bit, obj);
	evas_object_smart_data_set(obj, es);
	es->smart_obj = obj;

	/*
	 * Set some default colors, font, and font size.
	 */
	es->color = _etox_style_color_instance(255, 255, 255, 255);

	/*
	 * Create the style and set the default color.
	 */
	if (style)
		es->style = _etox_style_style_instance(style);
	etox_style_set_color(obj, 255, 255, 255, 255);
	if (es->style)
		_etox_style_style_draw(es, text);

	/*
	 * default to type of 0
	 */
	es->type = 0;

	etox_style_set_text(obj, text);

	return obj;
}

/**
 * etox_style_free - free the data associated with a etox_style
 * @es: the etox_style to free along with its data
 *
 * Returns no value. Frees the data in @es as well as @es itself.
 */
void etox_style_free(Evas_Object *obj)
{
	Evas_List *l;
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	if (es->bit)
		evas_object_del(es->bit);

	if (es->style)
		_etox_style_style_release(es->style, es->evas);

	if (es->callbacks) {
		for (l = es->callbacks; l; l = l->next)
			FREE(l->data);
		evas_list_free(es->callbacks);
	}

	FREE(es);
}

/**
 * etox_style_gc_collect - run through and free up cached styles
 *
 * Returns no value.
 */
void etox_style_gc_collect()
{
	_etox_style_style_info_collect();
}

/**
 * etox_style_show - display an etox_style or any changes made to a etox_style
 * @etox_style: the etox_style that needs display updates
 *
 * Returns no value. Updates the display to reflect changes to @etox_style.
 */
void etox_style_show(Evas_Object *obj)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	es->flags |= ETOX_STYLE_BIT_VISIBLE;
	_etox_style_style_show(es);
	evas_object_show(es->bit);
}


/**
 * etox_style_hide - hide the etox_style from the evas 
 * @es: the etox_style to hide
 *
 * Returns no value
 */
void etox_style_hide(Evas_Object *obj)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	es->flags &= ~ETOX_STYLE_BIT_VISIBLE;
	_etox_style_style_hide(es);
	evas_object_hide(es->bit);
}

/**
 * etox_style_move - move the etox_style into the correct position
 * @es: the etox_style to be moved
 * @x: the new x coordinate for the etox_style
 * @y: the new y coordinate for the etox_style
 *
 * Returns no value. Moves the requested etox_style into the requested position.
 */
void etox_style_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	if (es->x == x && es->y == y)
		return;

	es->x = x;
	es->y = y;
	evas_object_move(es->bit, 
	      (x + (es->style ? es->style->info->left_push : 0)),
	      (y + (es->style ? es->style->info->top_push : 0)));
	
	if (es->style)
		_etox_style_style_move(es);
}

/**
 * etox_style_resize - resize a fixed etox_style into the correct position
 * @es: the etox_style to be moved
 * @x: the new x coordinate for the etox_style
 * @y: the new y coordinate for the etox_style
 *
 * Returns no value. Moves the requested etox_style into the requested position.
 */
void etox_style_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	if (!(es->flags & ETOX_STYLE_BIT_FIXED))
		return;

	if (es->w == w && es->h == h)
		return;

	es->w = w;
	es->h = h;
	evas_object_resize(obj, w, h);
}

/**
 * etox_style_get_color - get the color of the etox_style
 * @es: the etox_style to get the color
 * @r: the pointer to the int to store the red value of the etox_style
 * @g: the pointer to the int to store the green value of the etox_style
 * @b: the pointer to the int to store the blue value of the etox_style
 * @a: the pointer to the int to store the alpha value of the etox_style
 *
 * Returns no value. Retrieves the color of the etox_style into the integer pointers
 * @r, @g, @b and @a.
 */
void etox_style_get_color(Evas_Object *obj, int *r, int *g, int *b, int *a)
{
	Etox_Style *es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

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
 * etox_style_set_color - change the color of the etox_style
 * @es: the etox_style to change color
 * @r: the red value of the etox_style
 * @g: the green value of the etox_style
 * @b: the blue value of the etox_style
 * @a: the alpha value of the etox_style
 *
 * Returns no value. Changes the color of the etox_style to that represented by
 * @r, @g, @b and @a.
 */
void etox_style_set_color(Evas_Object *obj, int r, int g, int b, int a)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	es->color = _etox_style_color_instance(r, g, b, a);
	evas_object_color_set(es->bit, r, g, b, a);
	_etox_style_style_set_color(es);
}

/**
 * etox_style_lookup_color_db - lookup a colors values by name in color db
 * @name: the name of the color to lookup in the color db
 * @r: the pointer to the int to store the red value of the etox_style
 * @g: the pointer to the int to store the green value of the etox_style
 * @b: the pointer to the int to store the blue value of the etox_style
 * @a: the pointer to the int to store the alpha value of the etox_style
 *
 * Returns no value. Looks up the color name in the color db and stores the
 * values in the integers pointed to by @r, @g, @b, and @a.
 */
void etox_style_lookup_color_db(char *name, int *r, int *g, int *b, int *a)
{
	Etox_Style_Color *color;

	CHECK_PARAM_POINTER("name", name);

	color = _etox_style_color_instance_db(name);

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
 * etox_style_set_color_db - set the color of an etox_style based on color name
 * @es: the etox_style to change the color
 */
void etox_style_set_color_db(Evas_Object *obj, char *name)
{
	Etox_Style *es;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("name", name);

	es = evas_object_smart_data_get(obj);

	es->color = _etox_style_color_instance_db(name);
	evas_object_color_set(es->bit, es->color->r, es->color->g,
			es->color->b, es->color->a);
	if (es->style)
		_etox_style_style_set_color(es);
}

/**
 * etox_style_get_style - get the style of the specified etox_style
 * @es: the etox_style to get the style of
 *
 * Returns the name the style or NULL if no style specified.
 */
char *etox_style_get_style(Evas_Object *obj)
{
	Etox_Style *es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	es = evas_object_smart_data_get(obj);

	if (!es->style)
		return NULL;

	return es->style->info->name;
}

/**
 * etox_style_set_style - change the style of the specified etox_style
 * @es: the etox_style to change style
 * @name: the name of the style to set for the etox_style
 *
 * Returns no value. The style of @es is changed to @name.
 */
void etox_style_set_style(Evas_Object *obj, char *name)
{
	int layer;
	const char *text;
	Evas_Object *clip;
	Etox_Style *es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	/*
	 * Set the style for this particular es.
	 */
	if (es->style)
		_etox_style_style_release(es->style, es->evas);

	if (!name || (es->style = _etox_style_style_instance(name)) == NULL)
		return;

	text = evas_object_text_text_get(es->bit);

	/*
	 * Draw the new style bits and set the correct layer for all of the
	 * etox_style contents. Also clip the contents to the clip rectangle.
	 */
	_etox_style_style_draw(es, (char *)text);

	layer = evas_object_layer_get(es->bit);
	etox_style_set_layer(obj, layer);

	_etox_style_style_move(es);

	if ((clip = evas_object_clip_get(es->bit)) != NULL)
		_etox_style_style_set_clip(es, clip);

	__etox_style_update_position(es);
	__etox_style_update_dimensions(es);

	if (es->flags & ETOX_STYLE_BIT_VISIBLE)
		_etox_style_style_show(es);
}

/**
 * etox_style_get_text - retrieve the text in the specified etox_style
 * @es: the etox_style to retrieve text
 *
 * Returns a pointer to a copy of the text in the etox_style @es on success, NULL
 * on failure.
 */
char *etox_style_get_text(Evas_Object *obj)
{
	int scount, i;
	const char *text;
	char *temp = NULL;
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	es = evas_object_smart_data_get(obj);

	text = evas_object_text_text_get(es->bit);
	if (text) {
		temp = calloc(es->length + 1, sizeof(char));
		if (temp) {
			scount = es->length - es->spaces;
			memmove(temp, text, scount);

			for (i = 0; i < es->spaces; i++)
				temp[scount + i] = text[scount + (2 * i)];
		}
	}

	return temp;
}

/**
 * etox_style_set_text - change the text on a specified etox_style
 * @es: the etox_style to change text
 * @text: the new text for the etox_style
 *
 * Returns no value. Changes the text in @es to @text and updates the display
 * if visible.
 */
void etox_style_set_text(Evas_Object *obj, char *text)
{
#ifdef DEBUG
	printf("etox_style_set_text() - called. text is (%s)\n", text);
#endif
	int i, scount;
	char *temp;
	Etox_Style *es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);
	if (!text)
		text = strdup("");

	/*
	 * Wheee!! what fun, to get the sizing right when spaces end the
	 * string we need to duplicate them. Start by counting the extra
	 * characters needed.
	 */
	scount = es->length = strlen(text);
#ifdef DEBUG
	printf("etox_style_set_text() - strlen returned %d\n", es->length);
#endif
	while (--scount > 0 && isspace(text[scount]));

	scount++;
	es->spaces = es->length - scount;

	/*
	 * Now copy in the starting string byte-by-byte, followed by the
	 * doubled spaces.
	 */
	temp = calloc(es->length + es->spaces + 1, sizeof(char));
	memmove(temp, text, scount);
	for (i = 0; i < es->spaces; i++) {
		temp[scount + (2 * i)] = text[scount + i];
		temp[scount + (2 * i) + 1] = text[scount + i];
	}

	/*
	 * Null terminate that sucka
	 */
	temp[scount + (2 * i)] = '\0';
	evas_object_text_text_set(es->bit, temp);
#ifdef DEBUG
	printf("etox_style_set_text() - setting object text to (%s)\n", temp);
#endif
	FREE(temp);

	/*
	 * Set the text for the style bits.
	 */
	if (es->style)
		_etox_style_style_set_text(es);

	/*
	 * Set new length and dimensions.
	 */
	__etox_style_update_dimensions(es);

        /*
         * Set the layer to update the internal style layers
         */
        etox_style_set_layer(obj, evas_object_layer_get(obj));
}

/**
 * etox_style_get_layer - retrieve the layer of the etox_style
 * @es: the etox_style to retrieve the current layer
 *
 * Returns the current layer of the etox_style @es.
 */
int etox_style_get_layer(Etox_Style * es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, FALSE);

	return evas_object_layer_get(es->bit);
}

/**
 * etox_style_set_layer - change the layer of a etox_style and it's style
 * @es: the etox_style to change layer
 * @layer: the new layer for the etox_style
 *
 * Returns no value. Changes the layer of @es to @layer along with all of
 * it's style bits.
 */
void etox_style_set_layer(Evas_Object *obj, int layer)
{
	Etox_Style *es;
	int index = 0;

	es = evas_object_smart_data_get(obj);

	evas_object_layer_set(es->bit, layer - 1);

	/*
	 * Changing the layer of the style bits must be broken into to steps to
	 * ensure the relative ordering remains the same.
	 */
	if (es->style)
		index = _etox_style_style_set_layer_lower(es, layer);

	evas_object_layer_set(es->bit, layer);

	if (es->style)
		_etox_style_style_set_layer_upper(es, layer, index);
}

/**
 * etox_style_get_font - retrieve a copy of the current font for an etox_style
 * @es: the etox_style to retrieve the current font
 *
 * Returns a pointer to a copy of the current font of the etox_style @es.
 */
char *etox_style_get_font(Evas_Object *obj)
{
	char *font;
	Evas_Font_Size size;
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);
	es = evas_object_smart_data_get(obj);
	evas_object_text_font_get(es->bit, &font, &size);
	return font;
}

/**
 * etox_style_get_font_size - retrieve the font size
 * @es: the etox_style to get the font size from
 *
 * Returns the font size (an int) or FALSE if an error occurs.
 */  
int etox_style_get_font_size(Evas_Object *obj)
{
	char *font;
	Evas_Font_Size size;
	Etox_Style *es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);
	es = evas_object_smart_data_get(obj);
	evas_object_text_font_get(es->bit, &font, &size);
	return (int)size;
}

/**
 * etox_style_set_font - change the font used for the specified etox_style
 * @es: the etox_style to change fonts
 * @font: the name of the font to use for the etox_style
 *
 * Returns no value. Changes the font for the specified etox_style to @name.
 */
void etox_style_set_font(Evas_Object *obj, char *font, int size)
{
	Etox_Style *es;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("font", font);

	es = evas_object_smart_data_get(obj);

	evas_object_text_font_set(es->bit, font, size);
	if (es->style)
		_etox_style_style_set_font(es, font, size);

	__etox_style_update_dimensions(es);
}

/**
 * etox_style_get_clip - get the clip box of an etox_style
 * @es: the etox_style to get the clip box from
 *
 * Returns an Evas_Object or NULL if no clip box specified.
 */
Evas_Object *etox_style_get_clip(Etox_Style *es)
{
	CHECK_PARAM_POINTER_RETURN("es", es, NULL);

	return evas_object_clip_get(es->bit);
} 

/**
 * etox_style_set_clip - set the clip box for the etox_style
 * @es: the etox_style to set the clip box
 * @clip: the clip box to set
 *
 * Returns no value. Sets the clip box of the etox_style to @clip.
 */
void etox_style_set_clip(Evas_Object *obj, Evas_Object *clip)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	evas_object_clip_set(es->bit, clip);

	if (es->style)
		_etox_style_style_set_clip(es, clip);
}

/**
 * etox_style_unset_clip - unset the clip box for the etox_style
 * @es: the etox_style to unset the clip box
 *
 * Returns no value. Unsets the clip box of the etox_style.
 */
void etox_style_unset_clip(Evas_Object *obj)
{
	Etox_Style *es;

	es = evas_object_smart_data_get(obj);

	evas_object_clip_unset(es->bit);

	if (es->style)
		_etox_style_style_set_clip(es, NULL);
}

/**
 * etox_style_merge - merge two etox_styles into a single etox_style
 * @es1: the destination etox_style that will contain the final text
 * @es2: the etox_style that will be freed, but it's text will be added to @es1
 *
 * Returns TRUE if the bits are able to be merged, otherwise FALSE. Appends the
 * text in @es2 to the text in @es1 and destroys @es2.
 */
int etox_style_merge(Evas_Object *obj1, Evas_Object *obj2)
{
	Etox_Style *es1;
	Etox_Style *es2;
	char *new_text, *text1, *text2;

	CHECK_PARAM_POINTER_RETURN("obj1", obj1, FALSE);
	CHECK_PARAM_POINTER_RETURN("obj2", obj2, FALSE);

	es1 = evas_object_smart_data_get(obj1);
	es2 = evas_object_smart_data_get(obj2);

	if (!BIT_MERGEABLE(es1, es2))
		return FALSE;

	text1 = etox_style_get_text(obj1);
	text2 = etox_style_get_text(obj2);

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
	FREE(text1);
	FREE(text2);

	etox_style_set_text(obj1, new_text);
    
	FREE(new_text);

	evas_object_del(obj2);

	return TRUE;
}

/**
 * etox_style_split - break a etox_style into multiple etox_styles at a specified index
 * @es: the etox_style to be split
 * @index: the index into the text to be split
 *
 * Returns the newly created etox_style on success, NULL on failure. The old
 * etox_style is updated to reflect the changes.
 */
Evas_Object *etox_style_split(Evas_Object *obj, unsigned int index)
{
	Etox_Style *es;
	char temp;
	char *content;
	Evas_Object *new_es;
	int r, g, b, a;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	es = evas_object_smart_data_get(obj);

	/*
	 * Don't bother to split if it will result in an empty string.
	 */
	if (!index)
		return NULL;

	/*
	 * Retrieve the contents of the etox_style to be split.
	 */
	content = etox_style_get_text(obj);
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
	 * Terminate the string at the index and set the old etox_style's content to
	 * the resulting string.
	 */
	temp = content[index];
	content[index] = '\0';

	etox_style_set_text(obj, content);

	/*
	 * Now place the content from index onwards into a new etox_style.
	 */
	content[index] = temp;
	new_es =
	    etox_style_new(es->evas, &(content[index]), es->style->info->name);

	/*
	 * Give this new etox_style the same settings as the previous etox_style.
	 */
	evas_object_clip_set(new_es, etox_style_get_clip(es));
	etox_style_set_font(new_es, etox_style_get_font(obj),
			etox_style_get_font_size(obj));

	/*
	 * Set the new color for the etox_style.
	 */
	etox_style_get_color(obj, &r, &g, &b, &a);
	etox_style_set_color(new_es, r, g, b, a);

	/*
	 * Put it on the same layer as the old etox_style
	 */
	etox_style_set_layer(new_es, etox_style_get_layer(es));

	if (evas_object_visible_get(es->bit))
		etox_style_show(new_es);

	FREE(content);

	return new_es;
}

/**
 * etox_style_length - retrieve the length of the text in an etox_style
 * @es: the etox_style to retrieve the length
 *
 * Returns the length of the text in the etox_style @es.
 */
inline int etox_style_length(Evas_Object *obj)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, FALSE);

	es = evas_object_smart_data_get(obj);

	return es->length;
}

/**
 * etox_style_geometry - retrieve the dimensions of the etox_style
 * @es: the etox_style to retrieve dimensions
 * @x: a pointer to an integer to store the x coordinate of the etox_style
 * @x: a pointer to an integer to store the y coordinate of the etox_style
 * @w: a pointer to an integer to store the width of the etox_style
 * @h: a pointer to an integer to store the height of the etox_style
 *
 * Returns no value. Stores the current x, y coordinates and the width, height
 * dimensions into @x, @y, @w, and @h respectively.
 */
void etox_style_geometry(Etox_Style * es, int *x, int *y, int *w, int *h)
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
 * etox_style_text_at - retrieve the character geometry for character at an index
 * @es: the etox_style to search for the character
 * @index: the position in the etox_style of the character
 * @char_x: a pointer to the integer to store the x coordinate of the character
 * @char_y: a pointer to the integer to store the y coordinate of the character
 * @char_w: a pointer to the integer to store the width of the character
 * @char_h: a pointer to the integer to store the height of the character
 *
 * Returns no value. The dimensions of the found character are stored in
 * @char_x, @char_y, @char_w, and @char_h.
 */
void
etox_style_text_at(Evas_Object *obj, int index, Evas_Coord *x, Evas_Coord *y,
		   Evas_Coord *w, Evas_Coord *h)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	evas_object_text_char_pos_get(es->bit, index,  x, y, w, h);

	if (x)
		*x += es->x;
	if (y)
		*y += es->y;
	if (w)
		*w += (es->style ?  es->style->info->left_push
			+ es->style->info->right_push : 0);
        if (h)
		*h += (es->style ?  es->style->info->top_push
			+ es->style->info->bottom_push : 0);
}

/**
 * etox_style_text_at_position - find the text at the given coords
 * @es: the etox_style to check for text intersects coordinates
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
int etox_style_text_at_position(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			    Evas_Coord *xx, Evas_Coord *yy,
			    Evas_Coord *ww, Evas_Coord *hh)
{
	int ret;
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);

	es = evas_object_smart_data_get(obj);

	ret = evas_object_text_char_coords_get(es->bit, x - es->x,
			y - es->y, xx, yy, ww, hh);

	if (xx)
		*xx += es->x;
	if (yy)
		*yy += es->y;
	if (ww)
		*ww += (es->style ?  es->style->info->left_push
			+ es->style->info->right_push : 0);
	if (hh)
		*hh += (es->style ?  es->style->info->top_push
			+ es->style->info->bottom_push : 0);

	return ret;
}

/**
 * etox_style_fixed - determine if the specified etox_style has fixed geometry
 * @es: the etox_style to check for fixed geometry
 *
 * Returns 0 if the etox_style does not have fixed geometry, > 0 otherwise.
 */
inline int etox_style_fixed(Evas_Object *obj)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);

	es = evas_object_smart_data_get(obj);

	return es->flags & ETOX_STYLE_BIT_FIXED;
}

/**
 * etox_style_fix_geometry - report a fixed geometry to querying functions
 * @es: the etox_style to fix the geometry
 * @x: the x coordinate to report
 * @y: the y coordinate to report
 * @w: the width to report
 * @h: the height to report
 *
 * Returns no value. Fixes the reported geometry of @es to @x, @y, @w, and @h
 * respectively.
 */
void etox_style_fix_geometry(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			 Evas_Coord w, Evas_Coord h)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	es->flags |= ETOX_STYLE_BIT_FIXED;
	evas_object_move(obj, x, y);
	evas_object_resize(obj, w, h);
}

/**
 * etox_style_unfix_geometry - remove the fixed property of the etox_style
 * @es: the etox_style to remove the fixed property
 *
 * Returns no value. Removes the fixed property of the etox_style and updates its
 * geometry to the actual geometry of the contents.
 */
void etox_style_unfix_geometry(Evas_Object *obj)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	evas_object_geometry_get(es->bit, &es->x, &es->y, &es->w, &es->h);
}

/*
 * __etox_style_update_position - update the position of the etox_style
 */
void __etox_style_update_position(Etox_Style *es)
{
	if (es->style)
		evas_object_move(es->bit,
			(Evas_Coord)(es->x + (es->style ?
				es->style->info->left_push : 0)),
			(Evas_Coord)(es->y + (es->style ?
				es->style->info->top_push : 0)));
}

/*
 * __etox_style_update_dimensions - update the dimensions of the etox_style
 */
void __etox_style_update_dimensions(Etox_Style *es)
{
	Evas_Coord x, y, w, h;
	
	/*
	 * If the etox_style doesn't have fixed dimensions then set it to the
	 * geometry of it's contents.
	 */
	if (!(es->flags & ETOX_STYLE_BIT_FIXED)) {
		evas_object_geometry_get(es->bit, &x, &y, &w, &h);

		es->w = D2I_ROUND(w) + (es->style ? es->style->info->left_push
			+ es->style->info->right_push : 0);
		es->h = D2I_ROUND(h) + (es->style ? es->style->info->top_push
			+ es->style->info->bottom_push : 0);
	}

	evas_object_resize(es->smart_obj, es->w, es->h);
}

/**
 * etox_style_callback_add - add a callback to the desired etox_style
 * @es: the etox_style to add the callback
 * @callback: the event type that triggers the callback function
 * @func: the function to call when the event occurs
 * @data: the data to pass to the function when called
 *
 * Returns no value. Adds a callback to @es that will be triggered when event
 * @callback occurs, function @func will be called with argument @data.
 */
void etox_style_callback_add(Evas_Object *obj, Evas_Callback_Type callback,
			       Etox_Style_Callback_Function func, void * data)
{
	Etox_Style * es;
	Etox_Style_Callback *cb;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	/*
	 * Allocate the new etox_style callback
	 */
	cb = malloc(sizeof(Etox_Style_Callback));
	cb->obj = obj;
	cb->type = callback;
	cb->data = data;
	cb->callback = func;


	/*
	 * Append the callback to the etox_style's list, and add the callback to
	 * the evas bits so that the wrapper function gets called.
	 */
	es->callbacks = evas_list_append(es->callbacks, (const void *)cb);
	evas_object_event_callback_add(es->bit, callback,
			  __etox_style_callback_dispatcher, cb);
	if (es->style->bits) {
		for (l = es->style->bits; l; l = l->next) {
			Evas_Object *o;

			o = l->data;
			evas_object_event_callback_add(o, callback,
					__etox_style_callback_dispatcher, cb);
		}
	}
}

/*
 * The dispatcher redirects the callback to the appropriate function with the
 * correct arguments.
 */
void __etox_style_callback_dispatcher(void *_data, Evas *_e, Evas_Object *_o,
				  void *event_info)
{
	Etox_Style_Callback *cb = _data;
	cb->callback(cb->data, cb->obj, event_info);
}

/*
 * etox_style_callback_del - remove all callbacks of the type @callback from etox_style
 * @es: the etox_style to delete the callback
 * @callback: the event type to be removed
 *
 * Returns no value.
 */
void etox_style_callback_del(Evas_Object *obj, Evas_Callback_Type callback,
		Etox_Style_Callback_Function func)
{
	Evas_List *l;
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	for (l = es->callbacks; l; l = l->next) {
		Etox_Style_Callback *cb;

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


void
etox_style_set_type(Evas_Object *obj, int type)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER("obj", obj);

	es = evas_object_smart_data_get(obj);

	es->type = type;

}

int 
etox_style_get_type(Evas_Object *obj)
{
	Etox_Style * es;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);

	es = evas_object_smart_data_get(obj);
	
	return es->type;
}
