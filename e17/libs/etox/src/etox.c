#include "etox-config.h"
#include "Etox_private.h"

static Ewd_List *_etox_break_text(Etox * et, char *text);
static void _etox_layout(Etox * et, int y);

/**
 * etox_new - create a new etox with default settings
 * @evas: the evas for rendering text
 *
 * Returns a pointer to a newly allocated etox on success, NULL on failure.
 */
Etox *etox_new(Evas evas)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Create the etox and assign it's evas to draw on.
	 */
	et = (Etox *) malloc(sizeof(Etox));
	memset(et, 0, sizeof(Etox));

	et->evas = evas;

	/*
	 * Allocate space for the default context
	 */
	et->context = (Etox_Context *) malloc(sizeof(Etox_Context));
	memset(et->context, 0, sizeof(Etox_Context));

	/*
	 * Setup the default color
	 */
	et->context->r = 255;
	et->context->g = 255;
	et->context->b = 255;
	et->context->a = 255;

	/*
	 * Setup the default style
	 */
	et->context->style = strdup("outline");

	/*
	 * Set up the default font
	 */
	evas_font_add_path(evas, PACKAGE_DATA_DIR "/fonts");
	et->context->font = strdup("nationff");
	et->context->font_size = 14;

	/*
	 * Setup the default alignment
	 */
	et->context->align = ETOX_ALIGN_LEFT | ETOX_ALIGN_TOP;

	/*
	 * Set the clip rectangle for the etox
	 */
	et->clip = evas_add_rectangle(evas);
	evas_set_color(et->evas, et->clip, 255, 255, 255, 255);

	return et;
}

/**
 * etox_new_all - create a new etox with specified settings
 * @evas: the evas for rendering text
 * @x: the x coordinate of the positioning for the etox
 * @y: the y coordinate of the positioning for the etox
 * @w: the width of the size for the etox
 * @h: the height of the size for the etox
 * @alpha: the alpha value to apply to the text in the etox
 * @padding: the padding surrounding the text in the etox
 * @style: the default style to be applied to the text of the etox
 * @align: the defaul alignment to apply to the text of the etox
 *
 * Returns a pointer to a newly allocated etox on success, NULL on failure.
 */
Etox *etox_new_all(Evas evas, int x, int y, int w, int h, int alpha,
		Etox_Alignment align)
{
	Etox *et;

	CHECK_PARAM_POINTER_RETURN("evas", evas, NULL);

	/*
	 * Create the etox and then override the default values with those
	 * passed in to etox_new_all.
	 */
	et = etox_new(evas);
	et->context->align = align;
	et->x = x;
	et->y = y;
	et->w = w;
	et->h = h;

	et->alpha = alpha;

	return et;
}

/**
 * etox_free - free the memory used by the etox and remove it from the evas
 * @et: the etox that will be freed
 *
 * Returns no value. Removes the etox @et from the evas it is drawn on, and
 * then deallocates the memory that it was using.
 */
void etox_free(Etox * et)
{
	Etox_Obstacle *obst;

	CHECK_PARAM_POINTER("et", et);

	etox_clear(et);
	FREE(et->context->style);

	while (et->obstacles && (obst = ewd_list_remove_last(et->obstacles)))
		FREE(obst);
}

/**
 * etox_show - display the etox
 * @et: the etox to be displayed
 *
 * Returns no value. Adds the text from @et to it's evas.
 */
void etox_show(Etox * et)
{
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	et->visible = TRUE;

	ewd_list_goto_first(et->lines);
	while ((line = ewd_list_next(et->lines))) {
		printf("Etox showing line %d\n", ewd_list_index(et->lines));
		etox_line_show(line);
	}

	/*
	 * Display and position the clip box with the correct size.
	 */
	evas_show(et->evas, et->clip);
	evas_move(et->evas, et->clip, (double)(et->x), (double)(et->y));
	evas_resize(et->evas, et->clip, (double)(et->w), (double)(et->h));
}

/**
 * etox_hide - hide the etox
 * @et: the etox to hide
 *
 * Returns no value. Hides the text from @et from it's evas
 */
void etox_hide(Etox * et)
{
	CHECK_PARAM_POINTER("et", et);

	et->visible = FALSE;

	evas_hide(et->evas, et->clip);
}

/**
 * etox_append_text - append text to the existing text in the etox
 * @et: the etox to append the text
 * @text: the text to add at the end of the etox
 *
 * Returns no value. Appends @text to the text already existing in @et, and
 * updates the layout and display of the etox.
 */
void etox_append_text(Etox * et, char *text)
{
	Ewd_List *lines;
	Etox_Line *end, *start;

	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("text", text);

	/*
	 * Break the incoming text into lines, and merge the first line of the
	 * new text with the last line of the old text.
	 */
	lines = _etox_break_text(et, text);
	start = ewd_list_remove_first(lines);
	end = ewd_list_remove_last(et->lines);

	/*
	 * Need to adjust the height and length of the line to reflect the
	 * text that was added.
	 */
	et->length -= start->length;
	et->h -= start->h;
	etox_line_merge(start, end);
	et->length += start->length;
	et->h += start->h;

	ewd_list_append(lines, start);

	/*
	 * Now add the remaining lines to the end of the line list.
	 */
	ewd_list_goto_first(et->lines);
	while ((start = ewd_list_remove_first(lines))) {
		if (start->w > et->w)
			et->w = start->w;

		et->h += start->h;
		et->length += start->length;
		ewd_list_append(lines, start);
	}

	/*
	 * Layout the lines on the etox starting at the newly added text.
	 */
	_etox_layout(et, end->y);

	/*
	 * Destroy the temporary list of lines now that it is empty.
	 */
	ewd_list_destroy(lines);
}

/**
 * etox_prepend_text - prepend text to the existing text in the etox
 * @et: the etox to prepend the text
 * @text: the text to add at the end of the etox
 *
 * Returns no value. Prepends @text to the text already existing in @et, and
 * updates the layout and display of the etox.
 */
void etox_prepend_text(Etox * et, char *text)
{
	Ewd_List *lines;
	Etox_Line *end, *start;

	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("text", text);

	/*
	 * Break the incoming text into lines, and merge the first line of the
	 * new text with the last line of the old text.
	 */
	lines = _etox_break_text(et, text);
	start = ewd_list_remove_first(et->lines);
	end = ewd_list_remove_last(lines);

	/*
	 * Need to adjust the height and length of the line to reflect the
	 * text that was added.
	 */
	et->length -= end->length;
	et->h -= end->h;
	etox_line_merge(end, start);
	et->length += end->length;
	et->h += end->h;

	ewd_list_append(lines, end);

	/*
	 * Now add the remaining lines to the end of the line list.
	 */
	ewd_list_goto_first(et->lines);
	while ((start = ewd_list_remove_first(et->lines))) {
		if (start->w > et->w)
			et->w = start->w;

		et->h += start->h;
		et->length += start->length;
		ewd_list_prepend(lines, start);
	}

	/*
	 * Add the newly created lines to the end of the list of lines.
	 */
	while ((start = ewd_list_remove_first(lines)))
		ewd_list_append(et->lines, start);

	/*
	 * Layout the lines on the etox.
	 */
	ewd_list_goto_first(et->lines);
	_etox_layout(et, et->y);

	/*
	 * Destroy the temporary list of lines now that it is empty.
	 */
	ewd_list_destroy(lines);
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
void etox_insert_text(Etox * et, char *text, int index)
{
	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("text", text);
}

/**
 * etox_set_text - change the text in the etox
 * @et: the etox to change text
 * @text: the new text to display in @et
 *
 * Returns no value. Changes the text displayed by @et to itext.
 */
void etox_set_text(Etox * et, char *text)
{
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	if (!text)
		text = strdup("");

	text = strdup(text);

	etox_clear(et);

	/*
	 * Layout the text and add to the display
	 */
	et->lines = _etox_break_text(et, text);

	ewd_list_goto_first(et->lines);
	_etox_layout(et, et->y);

	/*
	 * Sum up the length and height of the text in the etox.
	 */
	et->h = 0;
	et->length = 0;
	ewd_list_goto_first(et->lines);
	while ((line = ewd_list_next(et->lines))) {
		/*
		 * Grab the largest line width for the width of the etox.
		 */
		if (line->w > et->w)
			et->w = line->w;

		et->h += line->h;
		et->length += line->length;
	}

	FREE(text);
}

/**
 * etox_get_text - retrieve the text in the etox
 * @et: the etox to retrieve the text
 *
 * Returns the text in the etox @et on success, NULL on failure.
 */
char *etox_get_text(Etox * et)
{
	char *ret, *temp;
	Etox_Line *line;

	CHECK_PARAM_POINTER_RETURN("et", et, NULL);

	/*
	 * Return NULL on an empty etox.
	 */
	if (!et->lines)
		return NULL;

	ret = (char *)malloc((et->length + 1) * sizeof(char));
	memset(ret, 0, (et->length + 1) * sizeof(char));

	temp = ret;
	ewd_list_goto_first(et->lines);

	/*
	 * Concatenate the text into the newly allocated buffer.
	 */
	while ((line = ewd_list_next(et->lines))) {
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
void etox_clear(Etox * et)
{
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	/*
	 * If there aren't any lines currently available, then return
	 * immediately.
	 */
	if (!et->lines)
		return;

	while ((line = ewd_list_remove_last(et->lines)))
		etox_line_free(line);
}

/**
 * etox_set_layer - change the layer where the etox is displayed
 * @et: the etox to change layers
 * @layer: the layer to display the etox on the evas
 *
 * Returns no value. Moves all bits of the etox onto the @layer layer of the
 * evas. All further text added will be drawn on this layer.
 */
void etox_set_layer(Etox * et, int layer)
{
	Estyle *bit;
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	et->layer = layer;

	if (!et->lines)
		return;

	/*
	 * Set the layer for every line
	 */
	ewd_list_goto_first(et->lines);
	while ((line = ewd_list_next(et->lines))) {
		if (line->bits) {

			/*
			 * Set the layer for each bit in the line
			 */
			ewd_list_goto_first(line->bits);
			while ((bit = ewd_list_next(line->bits)))
				estyle_set_layer(bit, layer);
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
void etox_set_alpha(Etox *et, int alpha)
{
	int r, g, b, a;

	CHECK_PARAM_POINTER("et", et);

	if (et->alpha == alpha)
		return;

	evas_get_color(et->evas, et->clip, &r, &g, &b, &a);
	evas_set_color(et->evas, et->clip, r, g, b, alpha);
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
void etox_move(Etox * et, int x, int y)
{
	CHECK_PARAM_POINTER("et", et);

	if (et->x == x && et->y == y)
		return;

	et->x = x;
	et->y = y;

	/*
	 * Layout lines if appropriate.
	 */
	if (et->lines) {
		ewd_list_goto_first(et->lines);
		_etox_layout(et, et->y);
	}

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_move(et->evas, et->clip, (double)(et->x), (double)(et->y));
	evas_resize(et->evas, et->clip, (double)(et->w), (double)(et->h));
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
void etox_resize(Etox * et, int w, int h)
{
	CHECK_PARAM_POINTER("et", et);

	if (et->w == w && et->h == h)
		return;

	/*
	 * Layout lines if appropriate.
	 */
	if (et->lines) {
		ewd_list_goto_first(et->lines);
		_etox_layout(et, et->y);
	}

	/*
	 * Adjust the clip box to display the contents correctly. We need to
	 * do both a move and a resize in case the size has been adjusted
	 * during layout.
	 */
	evas_move(et->evas, et->clip, (double)(et->x), (double)(et->y));
	evas_resize(et->evas, et->clip, (double)(et->w), (double)(et->h));
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
 * etox_get_letter_geometry - retrieve information about a letters geometry
 * @et: the etox to inquire the geometry
 * @x: a pointer to an int to store the x coordinate of the etox
 * @y: a pointer to an int to store the y coordinate of the etox
 * @w: a pointer to an int to store the width of the etox
 * @h: a pointer to an int to store the height of the etox
 *
 * Returns no value. Stores the current geometry of the letter at index @index
 * in @et into the integers pointed to by @x, @y, @w, and @h.
 */
void etox_index_to_geometry(Etox * et, int index, int *x, int *y,
		int *w, int *h)
{
	int sum;
	Estyle *bit;
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	if (index > et->length) {
		sum = et->length;
		line = ewd_list_goto_last(et->lines);

		*h = line->h;
		*w = line->w / line->length;
		*y = line->y;
		*x = line->x + line->w;
	}
	else {
		/*
		 * Find the line that contains the character
		 */
		sum = 0;
		ewd_list_goto_first(et->lines);
		while ((line = (Etox_Line *)ewd_list_next(et->lines)) &&
				sum + line->length < index)
			sum += line->length;


		/*
		 * Find the bit that contains the character
		 */
		ewd_list_goto_first(line->bits);
		while ((bit = ewd_list_next(line->bits)) &&
				sum + estyle_length(bit) < index)
			sum += estyle_length(bit);

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
 * etox_get_coord_geometry - retrieve information about a letters geometry
 * @et: the etox to inquire the geometry
 * @x: a pointer to an int to store the x coordinate of the etox
 * @y: a pointer to an int to store the y coordinate of the etox
 * @w: a pointer to an int to store the width of the etox
 * @h: a pointer to an int to store the height of the etox
 *
 * Returns no value. Stores the geometry of the letter at coordinates @xc, @yc
 * in @et into the integers pointed to by @x, @y, @w, and @h.
 */
int etox_coord_to_geometry(Etox * et, int xc, int yc, int *x, int *y,
		int *w, int *h)
{
	int sum;
	Estyle *bit;
	Etox_Line *line;
	int tx, ty, tw, th;

	CHECK_PARAM_POINTER_RETURN("et", et, 0);

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
	line = ewd_list_goto_first(et->lines);
	while ((line = (Etox_Line *)ewd_list_next(et->lines)) &&
			line->y + line->h < yc) {
		sum += line->length;
	}

	/*
	 * Bring the coordinate into the line if it falls outside, this may
	 * happen with centered or right alignment.
	 */
	bit = ewd_list_goto_first(line->bits);
	estyle_geometry(bit, &tx, &ty, &tw, &th);
	if (xc < tx)
		xc = tx;

	/*
	 * Find the bit that contains the character, be sure to check that
	 * it's not above this line, which means the click happened above the
	 * etox, and should be placed over the first character.
	 */
	while ((bit = ewd_list_next(line->bits))) {

		estyle_geometry(bit, &tx, &ty, &tw, &th);
		if (xc >= tx && xc <= tx + tw)
			break;
		sum += estyle_length(bit);
	}

	/*
	 * Click occurred outside this line, return the end of line
	 * character's index.
	 */
	if (!bit) {
		bit = ewd_list_goto_first(line->bits);

		/*
		 * Estimate the average width and height of the line.
		 */
		*w = line->w / line->length;
		*h = line->h;

		/*
		 * Now position it based on the total width of the line, and
		 * the starting position of the bits.
		 */
		estyle_geometry(bit, &tx, &ty, &tw, &th);
		*x = tx + line->w;
		*y = line->y;
	}
	else {

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
void etox_set_clip(Etox * et, Evas_Object clip)
{
	CHECK_PARAM_POINTER("et", et);

	if (clip == NULL)
		evas_unset_clip(et->evas, et->clip);
	else
		evas_set_clip(et->evas, et->clip, clip);
}

/**
 * etox_obstacle_add - add an obstacle that the text will wrap
 * @et: the etox to wrap around an obstacle
 * @x: the x coordinate of the obstacle
 * @y: the y coordinate of the obstacle
 * @w: the width of the obstacle
 * @h: the height of the obstacle
 *
 * Returns no value. Adds an obstacle to the etox @et that the text will wrap
 * around.
 */
Etox_Obstacle *etox_obstacle_add(Etox * et, int x, int y, int w, int h)
{
	Etox_Obstacle *obst;

	CHECK_PARAM_POINTER_RETURN("et", et, NULL);

	if (!et->obstacles)
		et->obstacles = ewd_list_new();

	obst = etox_obstacle_new(et, x, y, w, h);

	ewd_list_append(et->obstacles, obst);

	etox_obstacle_place(et, obst);

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
void etox_obstacle_remove(Etox * et, Etox_Obstacle * obstacle)
{
	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("obstacle", obstacle);

	ewd_list_goto(et->obstacles, obstacle);
	ewd_list_remove(et->obstacles);

	etox_obstacle_free(et, obstacle);
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
void etox_obstacle_move(Etox * et, Etox_Obstacle * obst, int x, int y)
{
	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("obst", obst);

	estyle_move(obst->bit, x, y);
	etox_obstacle_unplace(et, obst);
	etox_obstacle_place(et, obst);
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
void etox_obstacle_resize(Etox * et, Etox_Obstacle * obst, int x, int y)
{
	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("obst", obst);
}

/*
 * _etox_break_text - break up the text into lines and bits
 * @et: the etox that will contain the text
 * @text: the text that will be broken up into bits
 *
 * Returns no value. Separates the text into lines and bits if specific
 * characters are contained in the text.
 */
static Ewd_List *_etox_break_text(Etox * et, char *text)
{
	Ewd_List *ret;
	Estyle *bit;
	Etox_Line *line = NULL;
	char *walk = text;
	char t = '\0';

	CHECK_PARAM_POINTER_RETURN("et", et, NULL);
	CHECK_PARAM_POINTER_RETURN("text", text, NULL);

	/*
	 * Setup the list for adding lines that will be returned to the
	 * calling function
	 */
	ret = ewd_list_new();
	line = etox_line_new(et->context->align);
	ewd_list_append(ret, line);
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
			bit = estyle_new(et->evas , text, et->context->style);
			estyle_set_clip(bit, et->clip);
			estyle_set_color(bit, et->context->r, et->context->g,
					et->context->b, et->context->a);
			etox_line_append(line, bit);
			estyle_show(bit);

			*walk = '\t';
			text = walk + 1;
			t = *text;

			/*
			 * Make a bit for the tab character
			 */
			*text = '\0';
			bit = estyle_new(et->evas, walk, et->context->style);
			estyle_set_color(bit, et->context->r, et->context->g,
					et->context->b, et->context->a);
			estyle_set_clip(bit, et->clip);
			etox_line_append(line, bit);
			estyle_show(bit);
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
			bit = estyle_new(et->evas, text, et->context->style);
			estyle_set_color(bit, et->context->r, et->context->g,
					et->context->b, et->context->a);
			estyle_set_clip(bit, et->clip);
			etox_line_append(line, bit);
			estyle_show(bit);

			*walk = '\n';
			text = walk + 1;

			/*
			 * Create a new line for the next text
			 */
			line = etox_line_new(line->flags);
			ewd_list_append(ret, line);
			line->et = et;

			break;
		}
		walk++;
	}

	if (*text) {
		bit = estyle_new(et->evas, text, et->context->style);
		estyle_set_color(bit, et->context->r, et->context->g,
				et->context->b, et->context->a);
		estyle_set_clip(bit, et->clip);
		etox_line_append(line, bit);
		estyle_show(bit);
	}
	else if (line->bits == NULL) {
			etox_line_free(line);
			ewd_list_remove_last(ret);
	}

	return ret;
}

/*
 * _etox_layout - deals with the actual laying out of lines within the etox
 * @et: the etox to be laid out
 *
 * Returns no value. Updates the positions of liens within the etox in order
 * to display them in the correct positions. The layout is done from the
 * current position in the line list, so that should be set appropriately
 * before performing this operation.
 */
static void _etox_layout(Etox * et, int y)
{
	Etox_Line *line;

	CHECK_PARAM_POINTER("et", et);

	/*
	 * Traverse the list displaying each line, moving down the screen after
	 * each line.
	 */
	while ((line = ewd_list_next(et->lines))) {
		line->x = et->x;
		line->y = y;

		etox_line_layout(line);
		y += line->h;
	}

	/*
	 * Adjust the height of the etox to the height of all lines
	 */
	et->h = y - et->y;

	evas_resize(et->evas, et->clip, et->w, et->h);
}
