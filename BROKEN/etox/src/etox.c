#include <ctype.h>
#include "config.h"
#include "Etox_private.h"

//#define DEBUG ON

static void etox_free(Evas_Object * et);
static void etox_show(Evas_Object * et);
static void etox_hide(Evas_Object * et);
static void etox_move(Evas_Object * et, Evas_Coord x, Evas_Coord y);
static void etox_resize(Evas_Object * et, Evas_Coord w, Evas_Coord h);
static void etox_set_layer(Evas_Object * et, int layer);
static void etox_set_clip(Evas_Object * et, Evas_Object *clip);
static void etox_unset_clip(Evas_Object * et);

static Evas_List *_etox_break_text(Etox * et, char *text);

static Evas_Smart *etox_smart = NULL;

/**
 * etox_new - create a new etox with default settings
 * @evas: the evas for rendering text
 *
 * Returns a pointer to a newly allocated etox on success, NULL on failure.
 */
Evas_Object *etox_new(Evas *evas)
{
	Etox *et;
	const Evas_List *font_paths;
	int path_found = 0;
	char buf[PATH_MAX];

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	strncpy(buf, PACKAGE_DATA_DIR "/fonts", PATH_MAX);
	font_paths = evas_font_path_list(evas);
	while (font_paths) {
		char *path = font_paths->data;

		if (!strcmp(path, buf)) {
			path_found = 1;
			break;
		}

		font_paths = font_paths->next;
	}

	if (!path_found)
		evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts");
	if (!etox_smart) {
		etox_smart = evas_smart_new("etox_smart", NULL, etox_free,
				etox_set_layer, NULL, NULL, NULL, NULL,
				etox_move, etox_resize, etox_show, etox_hide,
				NULL, etox_set_clip, etox_unset_clip,
				NULL);
	}

	/*
	 * Create the etox and assign it's evas to draw on.
	 */
	et = (Etox *) calloc(1, sizeof(Etox));

	et->evas = evas;
	et->smart_obj = evas_object_smart_add(evas, etox_smart);
	evas_object_smart_data_set(et->smart_obj, et);

	/*
	 * Allocate the default context
	 */
	et->context = etox_context_new();

	/*
	 * Set the clip rectangle for the etox
	 */
	et->clip = evas_object_rectangle_add(evas);
	evas_object_pass_events_set(et->clip, 1);
	evas_object_color_set(et->clip, 255, 255, 255, 255);

	return et->smart_obj;
}

/**
 * etox_new_all - create a new etox with specified settings
 * @evas: the evas for rendering text
 * @x: the x coordinate of the positioning for the etox
 * @y: the y coordinate of the positioning for the etox
 * @w: the width of the size for the etox
 * @h: the height of the size for the etox
 * @alpha: the alpha value to apply to the text in the etox
 * @align: the defaul alignment to apply to the text of the etox
 *
 * Returns a pointer to a newly allocated etox on success, NULL on failure.
 */
Evas_Object *
etox_new_all(Evas *evas, Evas_Coord x, Evas_Coord y, Evas_Coord w,
	     Evas_Coord h, int alpha, Etox_Alignment align)
{
	Evas_Object *et;
        Etox_Context *ec;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Create the etox and then override the default values with those
	 * passed in to etox_new_all.
	 */
	et = etox_new(evas);
        ec = etox_get_context(et);
	etox_context_set_align(ec, align);
	etox_set_alpha(et, alpha);
	evas_object_move(et, x, y);
	evas_object_resize(et, w, h);

	return et;
}

/**
 * etox_free - free the memory used by the etox and remove it from the evas
 * @et: the etox that will be freed
 *
 * Returns no value. Removes the etox @et from the evas it is drawn on, and
 * then deallocates the memory that it was using.
 */
static void etox_free(Evas_Object * obj)
{
	Etox *et;
	Etox_Obstacle *obst;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	etox_clear(obj);
	etox_context_free(et->context);
	etox_selection_free_by_etox(obj);

	l = et->obstacles;
	while (l) {
		obst = l->data;
		l = evas_list_remove(l, obst);
		FREE(obst);
	}

	FREE(et);
}

/**
 * etox_gc_collect - free memory used by cached objects 
 */
void etox_gc_collect()
{
	etox_style_gc_collect();
}

/**
 * etox_show - display the etox
 * @et: the etox to be displayed
 *
 * Returns no value. Adds the text from @et to it's evas.
 */
static void etox_show(Evas_Object * obj)
{
	Etox *et;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	for (l = et->lines; l; l = l->next) {
		if (l->data) {
			line = l->data;
			etox_line_show(line);
		}
	}

	/*
	 * Display and position the clip box with the correct size.
	 */
	if (et->lines)
		evas_object_show(et->clip);
	evas_object_move(et->clip, (et->x), (et->y));
	evas_object_resize(et->clip, (et->w), (et->h));
}

/**
 * etox_hide - hide the etox
 * @et: the etox to hide
 *
 * Returns no value. Hides the text from @et from it's evas
 */
static void etox_hide(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	evas_object_hide(et->clip);
}

/**
 * etox_append_text - append text to the existing text in the etox
 * @et: the etox to append the text
 * @text: the text to add at the end of the etox
 *
 * Returns no value. Appends @text to the text already existing in @et, and
 * updates the layout and display of the etox.
 */
void etox_append_text(Evas_Object * obj, char *text)
{
	char *text2;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("text", text);

	text2 = etox_get_text(obj);
	if (text2) {
		text2 = realloc(text2, strlen(text2) + strlen(text) + 1);
		strcat(text2, text);

	} else
	  text2 = strdup(text);

	etox_set_text(obj, text2);
	free(text2);
}

/**
 * etox_prepend_text - prepend text to the existing text in the etox
 * @et: the etox to prepend the text
 * @text: the text to add at the end of the etox
 *
 * Returns no value. Prepends @text to the text already existing in @et, and
 * updates the layout and display of the etox.
 */
void etox_prepend_text(Evas_Object * obj, char *text)
{
	char *text2, *text3;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("text", text);

	text2 = strdup(text);
	text3 = etox_get_text(obj);
	if (text3) {
		text2 = realloc(text2, strlen(text2) + strlen(text3) + 1);
		strcat(text2, text3);
		free(text3);
	}
	etox_set_text(obj, text2);
	free(text2);
}

/**
 * etox_insert_text - insert text into an etox at specified index
 * @et: the etox to insert the text
 * @text: the text to insert into the etox
 * @index: the index into the etox to insert the etox
 *
 * Returns no value. Places @text into the etox @et at position @index and
 * updates the layout and display of the etox.
 */
void etox_insert_text(Evas_Object * obj, char *text, int index)
{
	char *text2;
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("text", text);

	et = evas_object_smart_data_get(obj);

	if (!index) {
		etox_prepend_text(obj, text);
		return;
	}
	else if (index >= et->length) {
		etox_append_text(obj, text);
		return;
	}

	text2 = etox_get_text(obj);
	if (text2) {
		int len, len2;
		char *tmp;

		len = strlen(text);
		len2 = strlen(text2);

		text2 = realloc(text2, len2 + len + 1);
		tmp = text2 + index;
		memmove(text2 + index + len, tmp, strlen(tmp));
		memmove(text2 + index, text, len);
		text2[len + len2] = '\0';

	} else
		text2 = strdup(text);

	etox_set_text(obj, text2);
	free(text2);
}

/**
 * etox_delete_text - delete text from an etox at specified index
 * @et: the etox to delete the text from
 * @index: the start position to delete from
 * @len: the number of chars to delete
 *
 * Returns no value. Removes @len characters from text starting at @index
 * and updates the layout and display of the etox.
 */
void etox_delete_text(Evas_Object * obj, unsigned int index, unsigned int len)
{
	char *text2;

	CHECK_PARAM_POINTER("obj", obj);

	text2 = etox_get_text(obj);
	if (text2) {
		int len2;
		char *tmp;

		len2 = strlen(text2);
		tmp = text2 + index + len;
		memmove(text2 + index, tmp, strlen(tmp));
		text2[len2 - len] = '\0';

	} else
		text2 = strdup("");

	etox_set_text(obj, text2);
	free(text2);
}

/**
 * etox_set_text - change the text in the etox
 * @et: the etox to change text
 * @text: the new text to display in @et
 *
 * Returns no value. Changes the text displayed by @et to itext.
 */
void etox_set_text(Evas_Object * obj, char *text)
{
	Etox *et;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	etox_clear(obj);

	/*
	 * Layout the text and add to the display. Duplicate text to avoid
	 * read-only memory segv's when parsing.
	 */
	if (text) {
		text = strdup(text);
	}
	else {
		text = strdup("");
	}

	/*
	 * Sum up the length and height of the text in the etox.
	 */
	et->th = 0;
	et->tw = 0;
	et->length = strlen(text);

	et->lines = _etox_break_text(et, text);
	FREE(text);

	for (l = et->lines; l; l = l->next) {
		line = l->data;
		/*
		 * Grab the largest line width for the width of the etox.
		 */
		if (line->w > et->tw)
			et->tw = line->w;
	}

#ifdef DEBUG
	printf("etox_set_text() - calling etox_layout()\n");
#endif
	etox_layout(et);
	if (et->lines && evas_object_visible_get(obj))
		evas_object_show(et->clip);
	else
		evas_object_hide(et->clip);
}

/**
 * etox_get_text - retrieve the text in the etox
 * @et: the etox to retrieve the text
 *
 * Returns the text in the etox @et on success, NULL on failure.
 */
char *etox_get_text(Evas_Object * obj)
{
	Etox *et;
	char *ret, *temp;
	Etox_Line *line;
	Evas_List *l;
	int len;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	/*
	 * Return NULL on an empty etox.
	 */
	if (!et->lines)
		return NULL;
#ifdef DEBUG
	printf("etox_get_text() - etox length = %d\n", et->length);
#endif
	ret = (char *) calloc(et->length + 1, sizeof(char));
	temp = ret;

	/*
	 * Concatenate the text into the newly allocated buffer.
	 */
	len = et->length;
	for (l = et->lines; l; l = l->next) {
		line = l->data;
#ifdef DEBUG
		printf("etox_get_text() - getting up to %d characters\n", len);
#endif
		etox_line_get_text(line, temp, len);
#ifdef DEBUG
		printf("etox_get_text() - line text is : (%s)\n", temp);
#endif

		/*
		 * FIXME: Currently, in etox_line_get_text(), line->length
		 * is set to the actual length of what gets filled into the
		 * buffer. If this isn't done, then line->length will often
		 * be too long, resulting in an early \0 terminating our 
		 * string.
		 * Is there a better way to do this?
		 */
		if (l->next) {
			Etox_Line * nline = l->next->data;
			if (!(nline->flags & ETOX_LINE_WRAPPED)) {
				strcat(temp, "\n");
#ifdef DEBUG
				printf("etox_get_text() - appended newline\n");
#endif
				temp++;
				len--;
			}
		}
		temp += line->length;
		len -= line->length;

		if (len < 1) break;
	}
	return ret;
}

/**
 * etox_clear - remove all text from the etox
 * @et: the etox to be cleared
 *
 * Returns no value. Removes all text from the etox.
 */
void etox_clear(Evas_Object * obj)
{
	Etox *et;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	/*
	 * If there aren't any lines currently available, then return
	 * immediately.
	 */
	if (!et->lines)
		return;

	for (l = et->lines; l; l = evas_list_remove(l, l->data)) {
		line = l->data;
		etox_line_free(line);
	}

	et->lines = NULL;
	evas_object_hide(et->clip);

	// Reset the active selections
	etox_selection_free_by_etox(obj);
}

/**
 * etox_set_soft_wrap - turns on soft wrapping of lines that are
 * longer than the etox is wide
 * @obj: the etox evas object to set for
 * @boolean: 0 is off, anything else is on
 * 
 * Returns no value. changes current context alignment value.
 */
void etox_set_soft_wrap(Evas_Object *obj, int boolean)
{
	Etox *et;
	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (boolean)
		et->flags |= ETOX_SOFT_WRAP;
	else
		et->flags &= ~ETOX_SOFT_WRAP;

#ifdef DEBUG
	printf("\netox_set_soft_wrap() - calling etox_layout()\n");
#endif
	etox_layout(et);
}




/**
 * etox_set_layer - change the layer where the etox is displayed
 * @et: the etox to change layers
 * @layer: the layer to display the etox on the evas
 *
 * Returns no value. Moves all bits of the etox onto the @layer layer of the
 * evas. All further text added will be drawn on this layer.
 */
static void etox_set_layer(Evas_Object * obj, int layer)
{
	Etox *et;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->layer = layer;

	if (!et->lines)
		return;

	/*
	 * Set the layer for every line
	 */
	for (l = et->lines; l; l = l->next) {
		line = l->data;
                etox_line_set_layer(line, et->layer);
	}
}

/**
 * etox_set_word_wrap - turns on word wrapping when soft wrap enabled
 * @obj: the etox evas object to set for
 * @boolean: 0 is off, anything else is on
 * 
 * Returns no value. changes current context alignment value.
 */
void etox_set_word_wrap(Evas_Object *obj, int boolean)
{
	Etox *et;
	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (boolean)
		et->flags |= ETOX_BREAK_WORDS;
	else
		et->flags &= ~ETOX_BREAK_WORDS;

#ifdef DEBUG
	printf("\netox_set_word_wrap() - calling etox_layout()\n");
#endif
	etox_layout(et);
}

/**
 * etox_set_alpha - change the alpha value for the etox
 * @et: the etox to change alpha
 * @alpha: the new alpha value for the etox
 *
 * Returns no value. Sets the alpha value of the etox @et to @alpha.
 */
void etox_set_alpha(Evas_Object * obj, int alpha)
{
	Etox *et;
	int r, g, b, a;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (et->alpha == alpha)
		return;

	evas_object_color_get(et->clip, &r, &g, &b, &a);
	evas_object_color_set(et->clip, r, g, b, alpha);
}

/**
 * etox_get_length - get the length of the etox's text
 * @obj: the etox object
 *
 * Returns the length of the text contained in the etox, including the \n's
 * at the end of each line. (This will match the strlen of etox_get_text()).
 */
int
etox_get_length(Evas_Object *obj)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);

	et = evas_object_smart_data_get(obj);
	return et->length;
}


/**
 * etox_move - move the etox into a new desired position
 * @et: the etox to change position
 * @x: the new x coordinate of the etox
 * @y: the new y coordinate of the etox
 *
 * Returns no value. Changes the position of the etox @et to the specified
 * position.
 */
static void etox_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (et->x == x && et->y == y)
		return;

	et->x = x;
	et->y = y;

	evas_object_move(obj, et->x, et->y);

	/*
	 * Layout lines if appropriate.
	 */
	if (et->lines)
	{
#ifdef DEBUG
		printf("\netox_move() - calling etox_layout()\n");
#endif
		etox_layout(et);
	}

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_object_move(et->clip, (et->x), (et->y));
	evas_object_resize(et->clip, (et->w), (et->h));
}

/**
 * etox_resize - resize the etox into a new desired size
 * @et: the etox to change position
 * @w: the new width of the etox
 * @y: the new height of the etox
 *
 * Returns no value. Changes the dimensions of the etox to match the specified
 * dimensions.
 */
static void etox_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
#ifdef DEBUG
	printf("\netox_resize() - called. w=%d  h=%d\n", w, h);
#endif
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (et->w == w && et->h == h)
		return;

	et->w = w;
	et->h = h;

	/*
	 * Layout lines if appropriate.
	 */
#ifdef DEBUG
	printf("etox_resize() - calling etox_layout()\n");
#endif
	etox_layout(et);

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_object_move(et->clip, (et->x), (et->y));
	evas_object_resize(et->clip, (et->w), (et->h));
	/* evas_object_resize(obj, et->w, et->h); */
}

/**
 * etox_get_geometry - retrieve information about etox geometry
 * @et: the etox to inquire the geometry
 * @x: a pointer to an int to store the x coordinate of the etox
 * @y: a pointer to an int to store the y coordinate of the etox
 * @w: a pointer to an int to store the width of the etox
 * @h: a pointer to an int to store the height of the etox
 *
 * Returns no value. Stores the current geometry of the etox @et into the
 * integers pointed to by @x, @y, @w, and @h.
 */
void etox_get_geometry(Etox * et, int *x, int *y, int *w, int *h)
{
	CHECK_PARAM_POINTER("et", et);

	if (x)
		*x = et->x;

	if (y)
		*y = et->y;

	if (w)
		*w = et->w;

	if (h)
		*h = et->h;
}

/**
 * etox_index_to_geometry - retrieve information about a letters geometry
 * @et: the etox to inquire the geometry
 * @x: a pointer to an int to store the x coordinate of the etox
 * @y: a pointer to an int to store the y coordinate of the etox
 * @w: a pointer to an int to store the width of the etox
 * @h: a pointer to an int to store the height of the etox
 *
 * Returns no value. Stores the current geometry of the letter at index @index
 * in @et into the integers pointed to by @x, @y, @w, and @h.
 */
void
etox_index_to_geometry(Evas_Object * obj, int index, Evas_Coord *x,
		       Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
	Etox *et;
	int sum = 0;
	Etox_Line *line = NULL;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (index >= et->length) {
		sum = et->length;
                line = evas_list_data(evas_list_last(et->lines));

		if (line) {
			if (h) *h = line->h;
			if (w) *w = line->w / (line->length ? line->length : 1);
			if (y) *y = line->y;
			if (x) *x = line->x + line->w;
		}
	}
	else {
		Evas_List *l;
		for (l = et->lines; l; l = l->next) {
			line = l->data;
			sum += line->length;
			if (sum > index) {
				sum -= line->length;
				break;
			}
		}
		if (line)
			etox_line_index_to_geometry(line, index - sum, x, y,
						    w, h);
	}
}

/**
 * etox_coord_to_index - retrieve the index of a character at a coordinate
 * @et: the etox to map coordinates to a character index
 * @x: the x coordinate to find the matching character
 * @y: the y coordinate to find the matching character
 *
 * Returns the index in the text of the found character.
 */
int etox_coord_to_index(Evas_Object *et, Evas_Coord x, Evas_Coord y)
{
	CHECK_PARAM_POINTER_RETURN("et", et, 0);
	return etox_coord_to_geometry(et, x, y, NULL, NULL, NULL, NULL);
}

/**
 * etox_coord_to_geometry - retrieve information about a letters geometry
 * @et: the etox to inquire the geometry
 * @x: a pointer to an int to store the x coordinate of the etox
 * @y: a pointer to an int to store the y coordinate of the etox
 * @w: a pointer to an int to store the width of the etox
 * @h: a pointer to an int to store the height of the etox
 *
 * Returns the index in the text of the found character. Stores the geometry
 * of the letter at coordinates @xc, @yc in @et into the integers pointed to by
 * @x, @y, @w, and @h.
 */
int
etox_coord_to_geometry(Evas_Object * obj, Evas_Coord xc, Evas_Coord yc,
		       Evas_Coord *x, Evas_Coord *y, Evas_Coord *w,
		       Evas_Coord *h)
{
	Etox *et;
	int sum;
	Etox_Line *line = NULL;
	Evas_Object *bit = NULL;
	Evas_Coord tx, ty, tw, th;
	Evas_List *l;

	CHECK_PARAM_POINTER_RETURN("obj", obj, 0);

	et = evas_object_smart_data_get(obj);

	/*
	 * Put the click within the bounds of the etox.
	 */
	if (xc < et->x)
		xc = et->x + 1;

	if (yc < et->y)
		yc = et->y + 1;

	if (yc > et->y + et->h)
		yc = et->y + et->h - 1;

	/*
	 * Find the line that contains the character
	 */
	sum = 0;

	for (l = et->lines; l; l = l->next) {
		line = l->data;
		if (line->y + line->h < yc)
			sum += line->length;
		else
			break;
	}

	if (!et->lines)
		return sum;

	if (!line)
		line = (evas_list_last(et->lines))->data;

	/*
	 * Bring the coordinate into the line if it falls outside, this may
	 * happen with centered or right alignment.
	 */
	if (xc < line->x)
		xc = line->x;

	/*
	 * Find the bit that contains the character, be sure to check that
	 * it's not above this line, which means the click happened above the
	 * etox, and should be placed over the first character.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
		if (xc >= tx && xc <= tx + tw)
			break;
		sum += etox_style_length(bit);
	}

	/*
	 * Click occurred outside this line, return the end of line
	 * character's index.
	 */
	if (!l) {

		/*
		 * Estimate the average width and height of the line.
		 */
		if (w)
			*w = line->w / line->length;
		if (h)
			*h = line->h;

		/*
		 * Now position it based on the total width of the line, and
		 * the starting position of the bits.
		 */
		evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
		if (x)
			*x = tx + tw;
		if (y)
			*y = line->y;
	}
	else {

		/*
		 * Now get the actual geometry from the bit
		 */
		sum += etox_style_text_at_position(bit, xc, yc, x, y, w, h);
	}

	return sum;
}

/**
 * etox_set_clip - set the evas rectangle that will clip the etox
 * @et: the etox to set the clip rectangle
 * @clip: the clip rectangle that will be used by the etox
 *
 * Returns no value. Changes the clip rectangle for the etox @et to the clip
 * rectangle @clip and updates the display.
 */
static void etox_set_clip(Evas_Object * obj, Evas_Object *clip)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	evas_object_clip_set(et->clip, clip);
}

/**
 * etox_unset_clip - unset the evas rectangle that will clip the etox
 * @et: the etox to unset the clip rectangle
 *
 * Returns no value. Removes the clip rectangle for the etox @et.
 */
static void etox_unset_clip(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);
	evas_object_clip_unset(et->clip);
}

/**
 * etox_obstacle_add - add an obstacle that the text will wrap
 * @et: the etox to wrap around an obstacle
 * @x: the x coordinate of the obstacle
 * @y: the y coordinate of the obstacle
 * @w: the width of the obstacle
 * @h: the height of the obstacle
 *
 * Returns a pointer to the new obstacle object on success, NULL on failure.
 * Adds an obstacle to the etox @et that the text will wrap around.
 */
Etox_Obstacle *
etox_obstacle_add(Evas_Object * obj, Evas_Coord x, Evas_Coord y,
		  Evas_Coord w, Evas_Coord h)
{
	Etox *et;
	Etox_Obstacle *obst;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	obst = etox_obstacle_new(et, x, y, w, h);

	if (obst)
		et->obstacles = evas_list_append(et->obstacles, obst);

#ifdef DEBUG
	printf("\netox_obstacle_add() - calling etox_layout()\n");
#endif
	etox_layout(et);

	return obst;
}

/**
 * etox_obstacle_remove - remove an obstacle from the etox
 * @et: the etox to remove the obstacle
 * @obstacle: the obstacle to be removed from the etox
 *
 * Returns no value. Removes an obstacle from the etox and updates any lines
 * surrounding it.
 */
void etox_obstacle_remove(Etox_Obstacle * obstacle)
{
	CHECK_PARAM_POINTER("obstacle", obstacle);

	obstacle->et->obstacles = evas_list_remove(obstacle->et->obstacles,
			obstacle);

	etox_obstacle_free(obstacle->et, obstacle);
}

/**
 * etox_obstacle_move - change the position of an obstacle in the etox
 * @et: the etox containing the obstacle
 * @obst: the obstacle to be moved
 * @x: the new x coordinate of the obstacle
 * @y: the new y coordinate of the obstacle
 *
 * Returns no value. Changes the position information for @obst and updates the
 * etox to work around the new position.
 */
void etox_obstacle_move(Etox_Obstacle * obst, Evas_Coord x, Evas_Coord y)
{
	CHECK_PARAM_POINTER("obst", obst);

	evas_object_move(obst->bit, x, y);
	etox_obstacle_unplace(obst);
	etox_obstacle_place(obst);
}

/**
 * etox_obstacle_resize - change the size of an obstacle in the etox
 * @et: the etox containing the obstacle
 * @obst: the obstacle to be resized
 * @w: the new width of the obstacle
 * @h: the new height of the obstacle
 *
 * Returns no value. Changes the size information for @obst and updates the
 * etox to work around the new position.
 */
void etox_obstacle_resize(Etox_Obstacle * obst, Evas_Coord x, Evas_Coord y)
{
	CHECK_PARAM_POINTER("obst", obst);

	/*
	 * FIXME: Need to fill in the meat of this function
	 */
}

/*
 * _etox_break_text - break up the text into lines and bits
 * @et: the etox that will contain the text
 * @text: the text that will be broken up into bits
 *
 * Returns a list of lines on success, NULL on failure. Separates the text into
 * lines and bits if specific characters are contained in the text.
 */
static Evas_List *_etox_break_text(Etox * et, char *text)
{
	Evas_List *ret = NULL;
	Evas_Object *bit;
	Etox_Line *line = NULL;
	char *walk = text;
	char t = '\0';

	CHECK_PARAM_POINTER_RETURN("et", et, NULL);
	CHECK_PARAM_POINTER_RETURN("text", text, NULL);

	/*
	 * Setup the list for adding lines that will be returned to the
	 * calling function
	 */
	line = etox_line_new(et->context->flags);
	ret = evas_list_append(ret, line);
	line->et = et;

	while (*walk) {

		switch (*walk) {

			/*
			 * For tab characters we put a placeholder bit in
			 * place that gets sized larger than the actual space
			 * occupied by the bit.
			 */
		case '\t':
			*walk = '\0';

			/*
			 * Make a bit for the preceding text
			 */
			bit = etox_style_new(et->evas, text, et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_clip_set(bit, et->clip);
			evas_object_color_set(bit, et->context->r,
					      et->context->g, et->context->b,
					      et->context->a);
			etox_style_set_font(bit, et->context->font,
					et->context->font_size);
			etox_line_append(line, bit);
			evas_object_show(bit);

			*walk = '\t';
			text = walk + 1;

			/*
			 * Make a bit for the tab character
			 */
			bit = etox_style_new(et->evas, "        ",
					     et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_color_set(bit, et->context->r,
					      et->context->g, et->context->b,
					      et->context->a);
			evas_object_clip_set(bit, et->clip);
			etox_style_set_font(bit, et->context->font,
					et->context->font_size);
			etox_style_set_type(bit, ETOX_BIT_TYPE_TAB);
			etox_line_append(line, bit);
			evas_object_show(bit);

                        // We just converted 1 character into 8, so adjust the line length
			line->length += 7;

			break;

			/*
			 * Return character reached, so put a place holder at
			 * the end of the line, then add a new line to the
			 * list of lines.
			 */
		case '\n':
		case '\r':
			t = *walk;
			*walk = '\0';

			/*
			 * Create a bit for the text preceding \n
			 */
			bit = etox_style_new(et->evas, text, et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_color_set(bit, et->context->r,
					 et->context->g, et->context->b,
					 et->context->a);
			evas_object_clip_set(bit, et->clip);
			etox_style_set_font(bit, et->context->font,
					et->context->font_size);
			etox_line_append(line, bit);
			evas_object_show(bit);

			/*
			 * Only create one line for a \r\n combination.
			 */
			*walk = t;
			text = walk + 1;
			if (*walk == '\r' && *text == '\n') {
				walk++;
				text++;
				// This skips a character that was counted in the
				// original length, so decrement length by 1.
//??				et->length--;
			}

			if (line->w > et->tw)
				et->tw = line->w;
			if (*text)
				line->length++;

			/*
			 * Create a new line for the next text
			 */
			line = etox_line_new(line->flags);
			ret = evas_list_append(ret, line);
			line->et = et;

			break;
		default:
			break;
		}
		walk++;
	}

	/*
	 * Add any remaining text after the last line break or tab.
	 */
	if (*text || !line->bits) {
		bit = etox_style_new(et->evas, text, et->context->style);
		evas_object_smart_member_add(bit, et->smart_obj);
		evas_object_color_set(bit, et->context->r, et->context->g,
				      et->context->b, et->context->a);
		evas_object_clip_set(bit, et->clip);
		etox_style_set_font(bit, et->context->font,
				    et->context->font_size);
		etox_line_append(line, bit);
		evas_object_show(bit);
	}

	return ret;
}

/*
 * etox_layout - deals with the actual laying out of lines within the etox
 * @et: the etox to be laid out
 *
 * Returns no value. Updates the positions of liens within the etox in order
 * to display them in the correct positions. The layout is done from the
 * current position in the line list, so that should be set appropriately
 * before performing this operation.
 */
void etox_layout(Etox * et)
{
	int y;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("et", et);

#ifdef DEBUG
	printf("\netox_layout() - called\n");
	printf("etox_layout() - etox size is w=%d, h=%d\n", et->w, et->h);
#endif

	if (!(et->flags & ETOX_SOFT_WRAP))
		et->w = 0;

	if (!et->w)
		et->w = et->tw;

	/*
	 * Don't skip out just because the size is 0 x 0, we need to resize
	 * things later.
	 */
	if (!et->lines)
		return;

	y = et->y;

	/*
	 * Remove all the obstacles from their places in the etox.
	 */
	l = et->obstacles;
	while (l) {
		Etox_Obstacle *obst = l->data;
		etox_obstacle_unplace(obst);
		l = l->next;
	}

	/*
	 * Traverse the list displaying each line, moving down the screen after
	 * each line.
	 */
	l = et->lines;
	while (l) {
		Evas_List *ll;

		line = l->data;
		line->x = et->x;
		line->y = y;

#ifdef DEBUG
		char *tmpLine = calloc(256, sizeof(char));
		etox_line_get_text(line, tmpLine, 256);
		printf("\n\netox_layout() - current line is : %s\n", tmpLine);
                free(tmpLine);
#endif
		
		/*
		 * Unwrap lines if they were wrapped
		 */
		if (l->next) {
			Etox_Line *temp = l->next->data;

			if (temp->flags & ETOX_LINE_WRAPPED)
				etox_line_unwrap(et, line);
		}

		/*
		 * Re-place all the obstacles into their places in the etox.
		 */
		ll = et->obstacles;
		while (ll) {
			Evas_Coord ox, oy, ow, oh;

			Etox_Obstacle *obst = ll->data;
			evas_object_geometry_get(obst->bit, &ox, &oy, &ow, &oh);
			if (etox_rect_intersect(ox, oy, ow, oh,
						line->x, line->y,
						line->w, line->h))
				etox_obstacle_line_insert(line, obst);
			ll = ll->next;
		}

#ifdef DEBUG
		printf("etox_layout() - performing initial line layout\n");
#endif
		etox_line_layout(line);

		/*
		 * If we need to wrap the line, we need to re-layout since
		 * the width affects alignment.
		 */
		if ((et->flags & ETOX_SOFT_WRAP) && (line->w > et->w)) {
#ifdef DEBUG
		   printf("etox_layout() - line needs wrapping. calling etox_line_wrap()\n");
#endif
		   int index = etox_line_wrap(et, line);
		   if (index >= 0)
		     {
#ifdef DEBUG
			printf("etox_layout() - re-laying out wrapped line.\n");
#endif
			etox_line_layout(line);
		     }
		}
#ifdef DEBUG
		printf("etox_layout() - line complete. current etox size is w=%d, h=%d\n", et->w, et->h);
#endif

		l = l->next;
		y += line->h;
	}
	et->th = y - et->y;
#ifdef DEBUG
	printf("etox_layout() - Unwrapped text size is w=%d, h=%d\n", et->tw, et->th);
	printf("etox_layout() - etox size is w=%d, h=%d\n", et->w, et->h);
	printf("etox_layout() - done.\n\n");
#endif
}

Etox_Line *
etox_coord_to_line(Etox *et, int y)
{
	Evas_List *l;
	Etox_Line *line = NULL;;

	l = et->lines;
	while (l) {
		line = l->data;
		if (y < line->y + line->h)
			break;
		l = l->next;
	}

	if (!l)
		line = NULL;

	return line;
}

Etox_Line *
etox_index_to_line(Etox *et, int *i)
{
#ifdef DEBUG
	printf("etox_index_to_line() - called. index = %d\n", *i);
#endif
	int begin_line_index = 0;
	int line_length;
	Evas_List *l;
	Etox_Line *line = NULL;;

	l = et->lines;
	while (l) {
		line = l->data;
#ifdef DEBUG
		printf("etox_index_to_line() - checking line :\n");
		etox_line_print_bits(line);
		printf("etox_index_to_line() - line length is %d\n", line->length);
#endif
		line_length = line->length;
		if (!(line->flags & ETOX_LINE_WRAPPED))
		{
#ifdef DEBUG
		  printf("etox_index_to_line() - not a wrapped line. length++\n");
#endif
		  line_length++;
		}
#ifdef DEBUG
		else
		{
		  printf("etox_index_to_line() - wrapped line\n");
		}
#endif
		if (*i < (begin_line_index + line_length)) break;
	
		begin_line_index += line_length;
#ifdef DEBUG
		printf("etox_index_to_line() - begin_line_index is %d\n", begin_line_index);
#endif
		l = l->next;
	}

	if (line)
	{
#ifdef DEBUG
		printf("etox_index_to_line() - found line!\n");
#endif
		*i -= begin_line_index;
	}
	return line;
}

void
etox_print_lines(Etox *et)
{
	int i = 0;
	Evas_List *l;

	for (l = et->lines; l; l = l->next) {
		printf("Line %d:\n", i);
		etox_line_print_bits(l->data);
		i++;
	}
}

/** Printout the text buffer to support debugging */
void
etox_print_text(Evas_Object *obj)
{
  char *charptr_orig = etox_get_text(obj);
  char *charptr = charptr_orig;
  int char_num = 0;
  while (*charptr)
  {
    // Print line prefix with index
    if (char_num == 0)
    {
      printf("\netox_print_lines() - %5.5d : ", (charptr-charptr_orig));
    }

    // Print characters for printables, decimal otherwise
    if (isprint(*charptr))
    {
      printf(" %c ", *charptr);
    }
    else
    {
      printf("%2.2d ", *charptr);
    }
    fflush(stdout);

    // 16 characters per line
    if (++char_num == 16)
    {
      char_num = 0;
    }
    charptr++;
  }
  free(charptr_orig);
}

/**
 * etox_text_geometry_get - get the desired text geometry
 * @obj: the etox to get the geometry from
 * @w: variable to set the width into, or NULL if not needed
 * @h: variable to set the height into, or NULL if not needed
 *
 * Returns the desired width and height of the etox
 */
void etox_text_geometry_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
    Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

    if (w) *w = et->tw;
    if (h) *h = et->th;
}


