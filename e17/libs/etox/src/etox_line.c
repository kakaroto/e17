#include "Etox_private.h"

#include <string.h>

/*
 * etox_line_new - create a new line with the specified alignment
 * @align: the alignment of the current line
 *
 * Returns a pointer to the newly allocated line on success, NULL on failure.
 */
Etox_Line *etox_line_new(char align)
{
	Etox_Line *ret;

	ret = (Etox_Line *) calloc(1, sizeof(Etox_Line));
	if (ret) {
		ret->flags |= align;
		ret->length = 1;
	}

	return ret;
}

/*
 * etox_line_free - free the data structures in a line
 * @line: the line that will be freed
 *
 * Returns no value. Frees all of the data tracked by @line as well as @line
 * itself.
 */
void etox_line_free(Etox_Line * line)
{
	Estyle *bit;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Free all of the bits on the line.
	 */
	while (line->bits) {
		bit = line->bits->data;
		estyle_free(bit);
		line->bits = evas_list_remove(line->bits, bit);
	}

	FREE(line);
}

/*
 * etox_line_show - display all of the bits in the selected line
 * @line: the line to be displayed
 *
 * Returns no value. Displays the text on the specified line.
 */
void etox_line_show(Etox_Line * line)
{
	Estyle *bit;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Display all of the bits in the line.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		estyle_show(bit);
	}
}


/*
 * etox_line_hide - hide all the bits in the selected line
 * @line: the line to hide
 *
 * Returns no value
 */
void etox_line_hide(Etox_Line * line)
{
	Estyle *bit;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Hide all the bits in this line
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		estyle_hide(bit);
	}
}


/*
 * etox_line_append - append a bit to a line
 * @line: the line to append the bit
 * @bit: the bit to append to the line
 *
 * Returns no value. Appends the bit @bit to the line @line and updates
 * display to reflect the change.
 */
void etox_line_append(Etox_Line * line, Estyle * bit)
{
	int x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Append the text and update necessary fields
	 */
	line->bits = evas_list_append(line->bits, bit);
	estyle_geometry(bit, &x, &y, &w, &h);

	line->w += w;
	if (h > line->h)
		line->h = h;
	line->length += estyle_length(bit);
}

/*
 * etox_line_prepend - prepend a bit to a line
 * @line: the line to prepend the bit
 * @bit: the bit to prepend to the line
 *
 * Returns no value. Prepends the bit @bit to the line @line and updates
 * display to reflect the change.
 */
void etox_line_prepend(Etox_Line * line, Estyle * bit)
{
	int x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Prepend the text and update necessary fields
	 */
	line->bits = evas_list_prepend(line->bits, bit);
	estyle_geometry(bit, &x, &y, &w, &h);

	line->w += w;
	line->length += estyle_length(bit);
}

/*
 * etox_line_remove - remove a bit from the line
 * @line: the line to remove the bit
 * @bit: the bit to be from @line
 *
 * Removes @bit from @line and updates the appearance of surrounding bits to
 * reflect this change.
 */
void etox_line_remove(Etox_Line * line, Estyle * bit)
{
	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	line->bits = evas_list_remove(line->bits, bit);
	line->length -= estyle_length(bit);
	etox_line_minimize(line);
}

/*
 * etox_line_layout - layout the bits in a line across the etox
 * @line: the line that has the list of bits and bounding geometry
 *
 * Returns no value. Places the bits in @line across the screen and wraps them
 * appropriately around any fixed bits.
 */
void etox_line_layout(Etox_Line * line)
{
	int x;
	Estyle *bit;
	int tx, ty, tw, th;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	if (!line->bits)
		return;

	/*
	 * Determine the horizontal alignment of the text and set the starting
	 * x coordinate appropriately.
	 */
	if (line->flags & ETOX_ALIGN_LEFT) {
		x = line->x;
	} else if (line->flags & ETOX_ALIGN_RIGHT) {
		x = line->et->x + line->et->w - line->w;
	} else {
		x = line->et->x + (line->et->w / 2) - (line->w / 2);
	}

	/*
	 * Determine the veritcal alignment and perform the layout of the
	 * bits.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		if (!estyle_fixed(bit)) {

			estyle_geometry(bit, &tx, &ty, &tw, &th);
			if (line->h < th)
				line->h = th;

			/*
			 * Adjust the y position based on alignment.
			 */
			if (line->flags & ETOX_ALIGN_TOP)
				ty = line->y;
			else if (line->flags & ETOX_ALIGN_BOTTOM)
				ty = line->y + line->h - th;
			else
				ty = line->y + (line->h / 2) - (th / 2);

			/*
			 * Move the evas object into place.
			 */
			estyle_move(bit, x, ty);
		}

		/*
		 * Move horizontally to place the next bit.
		 */
		x += tw;
	}
}

/*
 * etox_line_minimize - reduce the number of bits on a line
 */
void etox_line_minimize(Etox_Line * line)
{
	Estyle *bit, *last_bit = NULL;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);

	l = line->bits;
	while ((bit = l->data)) {

		/*
		 * Attempt to merge the bits if possible, remove the second
		 * one if successful.
		 */
		if (estyle_merge(last_bit, bit))
			l = evas_list_remove(l, bit);
		else {
			last_bit = bit;
			l = l->next;
		}
	}
}

/*
 * etox_line_merge - merge two lines into the first line, free the second
 * @line1: the destination of the merged lines
 * @line2: the line that will be merged with line1
 *
 * Returns no value. Moves the bits from line2 into line 1.
 */
void etox_line_merge(Etox_Line * line1, Etox_Line * line2)
{
	Estyle *bit;

	CHECK_PARAM_POINTER("line1", line1);
	CHECK_PARAM_POINTER("line2", line2);

	/*
	 * Move the bits from line2 to line1.
	 */
	while (line2->bits) {
		bit = line2->bits->data;
		line1->bits = evas_list_append(line1->bits, bit);
		line2->bits = evas_list_remove(line2->bits, bit);
	}
	/*
	 * Adjust the height, width and length of the merged line.
	 */
	line1->w += line2->w;
	if (line2->h > line1->h)
		line1->h = line2->h;
	line1->length += line2->length;

	/*
	 * Destroy the line that was merged.
	 */
	etox_line_free(line2);
}

/*
 * etox_line_get_text - retrieve the text from a specified line into a buffer
 * @line: the line to retrieve text
 * @buf: the char buffer to store the found text, must have enough space
 *
 * Returns no value. Saves the text from the line @line into the char buffer
 * @buf.
 */
void etox_line_get_text(Etox_Line * line, char *buf)
{
	char *temp;
	Estyle *es;
	Evas_List *l;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("buf", buf);

	/*
	 * Examine each bit on the list of bits and cat it's text onto the end
	 * of the buffer. Then append a \n to the buffer at the end of the
	 * line.
	 */
	for (l = line->bits; l; l = l->next) {
		es = l->data;
		temp = estyle_get_text(es);
		strcat(buf, temp);
	}

	strcat(buf, "\n");
}

void
etox_line_wrap(Etox *et, Etox_Line *line)
{
	Evas_List *ll;
	Etox_Line *newline;
	Estyle *bit = NULL, *split = NULL, *marker;
	int index = -1;

	/* iterate through the bits to find the one on the border */
	for (ll = line->bits; ll; ll = ll->next) {
		bit = ll->data;

		/* get the index of the character on the edge */
		index = estyle_text_at_position(bit, et->x + et->w, line->y,
				NULL, NULL, NULL, NULL);

		/* if this bit contained the character on the edge, break */
		if (index >= 0)
			break;
	}

	/* if we have an index */
	if (index != -1) {
		char *tmp;

		/* don't start a new line with a space */
		tmp = estyle_get_text(bit);
		while (tmp[index] == ' ')
			index++;
		FREE(tmp);

		/* split the edge bit */
		split = estyle_split(bit, index);
	}

	/* if split successful, set up the new bit */
	if (split) {
		/* create a marker bit. */
		marker = estyle_new(et->evas, et->context->marker.text,
				et->context->marker.style);
		estyle_set_color(marker, et->context->marker.r,
				et->context->marker.g, et->context->marker.b,
				et->context->marker.a);
		estyle_set_clip(marker, et->clip);
		estyle_set_font(bit, et->context->font, et->context->font_size);
		estyle_show(marker);

		/* create a new line, with the marker and the split bits */
		newline = etox_line_new(line->flags | ETOX_LINE_WRAPPED);
		newline->et = et;
		etox_line_append(newline, marker);
		etox_line_append(newline, split);

		/* move the remaining bits to the new line */
		for (ll = ll->next; ll; ll = ll->next) {
			bit = ll->data;
			etox_line_remove(line, bit);
			etox_line_append(newline, split);
		}

		/* add the newline after the current one */
		et->lines = evas_list_append_relative(et->lines, newline, line);
	}
}

Estyle *
etox_line_coord_to_bit(Etox_Line *line, int x)
{
	int bx;
	Evas_List *l = NULL;
	Estyle *bit = NULL;

	/*
	 * Find the bit on this line
	 */
	l = line->bits;
	while (l) {
		bit = l->data;
		estyle_geometry(bit, &bx, NULL, NULL, NULL);
		if (bx < x)
			break;
		l = l->next;
	}

	return bit;
}

Estyle *
etox_line_index_to_bit(Etox_Line *line, int *i)
{
	int len = 0;
	Evas_List *l = NULL;
	Estyle *bit = NULL;

	l = line->bits;
	while (l) {
		bit = l->data;
		len += estyle_length(bit);
		if (*i < len)
			break;
		l = l->next;
	}

	if (l)
		*i -= (len - estyle_length(bit));

	return bit;
}
