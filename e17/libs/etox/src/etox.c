#include "etox-config.h"
#include "Etox_private.h"

static Evas_List *_etox_break_text(Etox * et, char *text);

Evas_Smart *etox_smart = NULL;

/**
 * etox_new - create a new etox with default settings
 * @evas: the evas for rendering text
 *
 * Returns a pointer to a newly allocated etox on success, NULL on failure.
 */
Evas_Object *etox_new(Evas *evas)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	if (!etox_smart)
		etox_smart = evas_smart_new("etox_smart", NULL, etox_free,
				etox_set_layer, NULL, NULL, NULL, NULL,
				etox_move, etox_resize, etox_show, etox_hide,
				NULL, etox_set_clip, etox_unset_clip,
				NULL);

	/*
	 * Create the etox and assign it's evas to draw on.
	 */
	et = (Etox *) calloc(1, sizeof(Etox));

	et->evas = evas;
	evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts");
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
etox_new_all(Evas *evas, double x, double y, double w, double h, int alpha,
		   Etox_Alignment align)
{
	Evas_Object *et;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Create the etox and then override the default values with those
	 * passed in to etox_new_all.
	 */
	et = etox_new(evas);
	etox_context_set_align(et, align);
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
void etox_free(Evas_Object * obj)
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
}

/**
 * etox_show - display the etox
 * @et: the etox to be displayed
 *
 * Returns no value. Adds the text from @et to it's evas.
 */
void etox_show(Evas_Object * obj)
{
	Etox *et;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->visible = TRUE;

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
	evas_object_move(et->clip, (double) (et->x), (double) (et->y));
	evas_object_resize(et->clip, (double) (et->w), (double) (et->h));
}

/**
 * etox_hide - hide the etox
 * @et: the etox to hide
 *
 * Returns no value. Hides the text from @et from it's evas
 */
void etox_hide(Evas_Object * obj)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	et->visible = FALSE;

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
	Etox *et;
	Evas_List *lines;
	Etox_Line *end = NULL, *start;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("text", text);

	et = evas_object_smart_data_get(obj);

	/*
	 * Break the incoming text into lines, and merge the first line of the
	 * new text with the last line of the old text.
	 */
	lines = _etox_break_text(et, text);
	if (!lines)
		return;

	/*
	 * Merge the last line of the existing text with the first line of the
	 * new text.
	 */
	if (et->lines) {
		Evas_List *l;

		l = evas_list_last(et->lines);
		end = l->data;
		start = lines->data;
		lines = evas_list_remove(lines, start);

		/*
		 * Need to adjust the length, height, and width of the line to
		 * reflect the text that was added.
		 */
		et->length -= end->length;
		et->h -= end->h;
		etox_line_merge_append(end, start);
		etox_line_free(start);
		et->length += end->length;
		et->h += end->h;
		if (end->w > et->tw)
			et->tw = end->w;
	}

	/*
	 * Now add the remaining lines to the end of the line list.
	 */
	while (lines) {
		start = lines->data;

		if (start->w > et->tw)
			et->tw = start->w;

		et->h += start->h;
		et->length += start->length;
		et->lines = evas_list_append(et->lines, start);
		lines = evas_list_remove(lines, start);
	}

	/*
	 * Layout the lines on the etox starting at the newly added text.
	 */
	etox_layout(et);
	if (et->lines && et->visible)
		evas_object_show(et->clip);
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
	Etox *et;
	Evas_List *lines;
	Etox_Line *end = NULL, *start;

	CHECK_PARAM_POINTER("obj", obj);
	CHECK_PARAM_POINTER("text", text);

	et = evas_object_smart_data_get(obj);

	/*
	 * Break the incoming text into lines, and merge the first line of the
	 * new text with the last line of the old text.
	 */
	lines = _etox_break_text(et, text);
	if (!lines)
		return;

	/*
	 * Merge the first line of the existing text with the last line of the
	 * new text.
	 */
	if (et->lines) {
		Evas_List *l;

		l = evas_list_last(lines);
		start = l->data;
		lines = evas_list_remove(lines, start);
		end = et->lines->data;

		/*
		 * Need to adjust the height and length of the line to reflect
		 * the text that was added.
		 */
		et->length -= end->length;
		et->h -= end->h;
		etox_line_merge_prepend(start, end);
		etox_line_free(start);
		et->length += end->length;
		et->h += end->h;
		if (end->w > et->tw)
			et->tw = end->w;
	}

	/*
	 * Now add the remaining lines to the end of the line list.
	 */
	while (lines) {
		Evas_List *l;

		l = evas_list_last(lines);
		end = l->data;

		if (end->w > et->tw)
			et->tw = end->w;

		et->h += end->h;
		et->length += end->length;
		et->lines = evas_list_prepend(et->lines, end);
		lines = evas_list_remove(lines, end);
	}

	/*
	 * Layout the lines on the etox.
	 */
	etox_layout(et);
	if (et->lines && et->visible)
		evas_object_show(et->clip);
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
	Etox *et;
	Evas_Object *bit;
	Evas_List *lines, *ll;
	Etox_Line *start, *end, *temp;

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

	/*
	 * Break the incoming text into lines, and merge the first line of the
	 * new text with the last line of the old text.
	 */
	lines = _etox_break_text(et, text);
	if (!lines)
		return;

	start = etox_index_to_line(et, &index);
	bit = etox_line_index_to_bit(start, &index);
	etox_line_split(start, bit, index);

	/*
	 * Setup the merger betweeen the beginning of the existing text and the
	 * beginning of the added text.
	 */
	temp = lines->data;
	lines = evas_list_remove(lines, temp);
	etox_line_merge_append(start, temp);

	/*
	 * Now merge the end of the added text with the remainder of the
	 * existing text.
	 */
	ll = evas_list_last(lines);
	temp = ll->data;
	lines = evas_list_remove(lines, temp);
	ll = evas_list_find_list(et->lines, start);
	end = ll->next->data;
	etox_line_merge_prepend(temp, end);

	/*
	 * Now add the remaining lines to the end of the line list.
	 */
	while (lines) {
		end = lines->data;

		if (end->w > et->tw)
			et->tw = end->w;

		et->h += end->h;
		et->length += end->length;
		et->lines = evas_list_append_relative(et->lines, end, start);
		lines = evas_list_remove(lines, end);
		start = end;
	}

	etox_layout(et);
	if (et->lines && et->visible)
		evas_object_show(et->clip);
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
	if (text && *text) {
		text = strdup(text);
		et->lines = _etox_break_text(et, text);
		FREE(text);
	}

	/*
	 * Sum up the length and height of the text in the etox.
	 */
	et->h = 0;
	et->length = 0;
	for (l = et->lines; l; l = l->next) {
		line = l->data;
		/*
		   * Grab the largest line width for the width of the etox.
		 */
		if (line->w > et->tw)
			et->tw = line->w;

		et->h += line->h;
		et->length += line->length;
	}

	etox_layout(et);
	if (et->lines && et->visible)
		evas_object_show(et->clip);
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

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	/*
	 * Return NULL on an empty etox.
	 */
	if (!et->lines)
		return NULL;

	ret = (char *) calloc((et->length + 1), sizeof(char));

	temp = ret;

	/*
	 * Concatenate the text into the newly allocated buffer.
	 */
	for (l = et->lines; l; l = l->next) {
		line = l->data;
		etox_line_get_text(line, temp);
		temp += line->length;
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
}

/**
 * etox_set_layer - change the layer where the etox is displayed
 * @et: the etox to change layers
 * @layer: the layer to display the etox on the evas
 *
 * Returns no value. Moves all bits of the etox onto the @layer layer of the
 * evas. All further text added will be drawn on this layer.
 */
void etox_set_layer(Evas_Object * obj, int layer)
{
	Etox *et;
	Evas_Object *bit;
	Etox_Line *line;
	Evas_List *l, *ll;

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

		if (line->bits) {

			/*
			 * Set the layer for each bit in the line
			 */
			for (ll = line->bits; ll; ll = ll->next) {
				bit = ll->data;
				evas_object_layer_set(bit, layer);
			}
		}
	}
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
 * etox_move - move the etox into a new desired position
 * @et: the etox to change position
 * @x: the new x coordinate of the etox
 * @y: the new y coordinate of the etox
 *
 * Returns no value. Changes the position of the etox @et to the specified
 * position.
 */
void etox_move(Evas_Object * obj, double x, double y)
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
		etox_layout(et);

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_object_move(et->clip, (double) (et->x), (double) (et->y));
	evas_object_resize(et->clip, (double) (et->w), (double) (et->h));
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
void etox_resize(Evas_Object * obj, double w, double h)
{
	Etox *et;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (et->w == w && et->h == h)
		return;

	et->w = w;

	/*
	 * Layout lines if appropriate.
	 */
	if (et->lines)
		etox_layout(et);

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_object_move(et->clip, (double) (et->x), (double) (et->y));
	evas_object_resize(et->clip, (double) (et->w), (double) (et->h));
	evas_object_resize(obj, et->w, et->h);
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
void etox_index_to_geometry(Evas_Object * obj, int index, double *x, double *y,
			    double *w, double *h)
{
	Etox *et;
	int sum;
	Evas_Object *bit = NULL;
	Etox_Line *line = NULL;
	Evas_List *l, *ll, *lll;

	CHECK_PARAM_POINTER("obj", obj);

	et = evas_object_smart_data_get(obj);

	if (index > et->length) {
		sum = et->length;
		for (lll = et->lines; lll; lll = lll->next)
			line = lll->data;

		*h = line->h;
		*w = line->w / line->length;
		*y = line->y;
		*x = line->x + line->w;
	} else {
		/*
		 * Find the line that contains the character
		 */
		sum = 0;
		for (l = et->lines; l; l = l->next) {
			line = l->data;
			if (sum + line->length < index)
				break;
			sum += line->length;
		}

		/*
		 * Find the bit that contains the character
		 */
		for (ll = line->bits; ll; ll = ll->next) {
			bit = ll->data;
			if (sum + estyle_length(bit) < index)
				break;
			sum += estyle_length(bit);
		}

		/*
		 * No bit intersects, so set the geometry to the start of the
		 * line, with the average character width on the line.
		 */
		if (!bit) {
			*h = line->h;
			*w = line->w / line->length;
			*y = line->y;
			*x = line->x + line->w;
			return;
		}

		/*
		 * Now get the actual geometry from the bit
		 */
		estyle_text_at(bit, index - sum, x, y, w, h);
	}
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
int etox_coord_to_geometry(Evas_Object * obj, double xc, double yc, double *x,
		double *y, double *w, double *h)
{
	Etox *et;
	int sum;
	Evas_Object *bit;
	Etox_Line *line = NULL;
	double tx, ty, tw, th;
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

	/*
	 * Bring the coordinate into the line if it falls outside, this may
	 * happen with centered or right alignment.
	 */
	bit = line->bits->data;
	evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
	if (xc < tx)
		xc = tx;

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
		sum += estyle_length(bit);
	}

	/*
	 * Click occurred outside this line, return the end of line
	 * character's index.
	 */
	if (!l) {
		bit = line->bits->data;

		/*
		 * Estimate the average width and height of the line.
		 */
		*w = line->w / line->length;
		*h = line->h;

		/*
		 * Now position it based on the total width of the line, and
		 * the starting position of the bits.
		 */
		evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
		*x = tx + line->w;
		*y = line->y;
	} else {

		/*
		 * Now get the actual geometry from the bit
		 */
		sum += estyle_text_at_position(bit, xc, yc, x, y, w, h);
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
void etox_set_clip(Evas_Object * obj, Evas_Object *clip)
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
void etox_unset_clip(Evas_Object * obj)
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
etox_obstacle_add(Evas_Object * obj, double x, double y, double w, double h)
{
	Etox *et;
	Etox_Obstacle *obst;

	CHECK_PARAM_POINTER_RETURN("obj", obj, NULL);

	et = evas_object_smart_data_get(obj);

	obst = etox_obstacle_new(et, x, y, w, h);

	if (obst)
		et->obstacles = evas_list_append(et->obstacles, obst);

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
void etox_obstacle_move(Etox_Obstacle * obst, double x, double y)
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
void etox_obstacle_resize(Etox_Obstacle * obst, double x, double y)
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
			bit =
			    estyle_new(et->evas, text, et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_clip_set(bit, et->clip);
			evas_object_color_set(bit, et->context->r,
					 et->context->g, et->context->b,
					 et->context->a);
			estyle_set_font(bit, et->context->font, et->context->font_size);
			etox_line_append(line, bit);
			evas_object_show(bit);

			*walk = '\t';
			text = walk + 1;
			t = *text;

			/*
			 * Make a bit for the tab character
			 */
			*text = '\0';
			bit =
			    estyle_new(et->evas, walk, et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_color_set(bit, et->context->r,
					 et->context->g, et->context->b,
					 et->context->a);
			evas_object_clip_set(bit, et->clip);
			estyle_set_font(bit, et->context->font, et->context->font_size);
			etox_line_append(line, bit);
			evas_object_show(bit);
			*text = t;

			break;

			/*
			 * Return character reached, so put a place holder at
			 * the end of the line, then add a new line to the
			 * list of lines.
			 */
		case '\n':
			*walk = '\0';

			/*
			 * Create a bit for the text preceding \n
			 */
			bit =
			    estyle_new(et->evas, text, et->context->style);
			evas_object_smart_member_add(bit, et->smart_obj);
			evas_object_color_set(bit, et->context->r,
					 et->context->g, et->context->b,
					 et->context->a);
			evas_object_clip_set(bit, et->clip);
			estyle_set_font(bit, et->context->font,
					et->context->font_size);
			etox_line_append(line, bit);
			evas_object_show(bit);

			*walk = '\n';
			text = walk + 1;

			/*
			 * Create a new line for the next text
			 */
			if (line->w > et->tw)
				et->tw = line->w;
			line = etox_line_new(line->flags);
			ret = evas_list_append(ret, line);
			line->et = et;

			break;
		}
		walk++;
	}

	/*
	 * Add any remaining text after the last line break or tab.
	 */
	if (*text) {
		bit = estyle_new(et->evas, text, et->context->style);
		evas_object_smart_member_add(bit, et->smart_obj);
		evas_object_color_set(bit, et->context->r, et->context->g,
				 et->context->b, et->context->a);
		evas_object_clip_set(bit, et->clip);
		estyle_set_font(bit, et->context->font, et->context->font_size);
		etox_line_append(line, bit);
		evas_object_show(bit);
	} else if (line->bits == NULL) {
		ret = evas_list_remove(ret, line);
		etox_line_free(line);
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

	if (!et->w)
		et->w = et->tw;

	/*
	 * What the hell, do you expect us to "just know" what text to
	 * display, you've got to set some dumbass!
	 */
	if (!et->lines || et->w <= 0 || et->h <= 0)
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
			double ox, oy, ow, oh;

			Etox_Obstacle *obst = ll->data;
			evas_object_geometry_get(obst->bit, &ox, &oy, &ow, &oh);
			if (etox_rect_intersect(ox, oy, ow, oh,
						line->x, line->y,
						line->w, line->h))
				etox_obstacle_line_insert(line, obst);
			ll = ll->next;
		}

		etox_line_layout(line);

		/*
		 * If we need to wrap the line, we don't need to re-layout since
		 * the maximal width was used for splitting.
		 */
		if ((et->context->flags & ETOX_SOFT_WRAP) && (line->w > et->w))
				etox_line_wrap(et, line);

		l = l->next;
		y += line->h;
	}

	/*
	 * Adjust the height of the etox to the height of all lines
	 */
	et->h = y - et->y;

	if (et->context->flags & ETOX_SOFT_WRAP)
		evas_object_resize(et->clip, et->w, et->h);
	else
		evas_object_resize(et->clip, et->tw, et->h);
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
	int len = 0;
	Evas_List *l;
	Etox_Line *line = NULL;;

	l = et->lines;
	while (l) {
		line = l->data;
		len += line->length;
		if (*i < len)
			break;
		l = l->next;
	}

	if (line)
		*i -= len - line->length;

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
