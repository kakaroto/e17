#include "Etox_private.h"

#include <ctype.h>
#include <string.h>

//#define DEBUG ON


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
		ret->length = 0;
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
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Free all of the bits on the line.
	 */
	while (line->bits) {
		bit = line->bits->data;
		evas_object_del(bit);
		line->bits = eina_list_remove(line->bits, bit);
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
	Evas_Object *bit;
	Eina_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Display all of the bits in the line.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_show(bit);
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
	Evas_Object *bit;
	Eina_List *l;

	CHECK_PARAM_POINTER("line", line);

	/*
	 * Hide all the bits in this line
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_hide(bit);
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
void etox_line_append(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Append the text and update necessary fields
	 */
	line->bits = eina_list_append(line->bits, bit);
	evas_object_geometry_get(bit, &x, &y, &w, &h);

	line->w += w;
	if (h > line->h)
		line->h = h;
	line->length += etox_style_length(bit);

        etox_selections_update(bit, line);
}

/*
 * etox_line_prepend - prepend a bit to a line
 * @line: the line to prepend the bit
 * @bit: the bit to prepend to the line
 *
 * Returns no value. Prepends the bit @bit to the line @line and updates
 * display to reflect the change.
 */
void etox_line_prepend(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord x, y, w, h;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	/*
	 * Prepend the text and update necessary fields
	 */
	line->bits = eina_list_prepend(line->bits, bit);
	evas_object_geometry_get(bit, &x, &y, &w, &h);

	line->w += w;
	if (h > line->h)
		line->h = h;
	line->length += etox_style_length(bit);

        etox_selections_update(bit, line);
}

/*
 * etox_line_remove - remove a bit from the line
 * @line: the line to remove the bit
 * @bit: the bit to be from @line
 *
 * Removes @bit from @line and updates the appearance of surrounding bits to
 * reflect this change.
 */
void etox_line_remove(Etox_Line * line, Evas_Object * bit)
{
	Evas_Coord w;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("bit", bit);

	line->bits = eina_list_remove(line->bits, bit);
	line->length -= etox_style_length(bit);
	evas_object_geometry_get(bit, NULL, NULL, &w, NULL);
	line->w -= w;

	/*
	 * FIXME: Need to fix-up line minimizing to ensure it doesn't stomp on
	 * selections.
	 * etox_line_minimize(line);
	 */
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
	Evas_Object *bit;
	Evas_Coord tx, ty, tw, th;
	Eina_List *l;

	CHECK_PARAM_POINTER("line", line);

	if (!line->bits)
		return;

	/*
	 * Determine the horizontal alignment of the text and set the starting
	 * x coordinate appropriately.
	 */
	if (line->flags & ETOX_ALIGN_LEFT) {
		x = line->et->x;
	} else if (line->flags & ETOX_ALIGN_RIGHT) {
		x = line->et->x + line->et->w - line->w;
	} else {
		x = line->et->x + (line->et->w / 2) - (line->w / 2);
	}

	if ((line->et->flags & ETOX_SOFT_WRAP) && (x < line->et->x))
		x = line->et->x;

	/*
	 * Determine the veritcal alignment and perform the layout of the
	 * bits.
	 */
	for (l = line->bits; l; l = l->next) {
		bit = l->data;
		evas_object_geometry_get(bit, &tx, &ty, &tw, &th);
		if (!etox_style_fixed(bit)) {

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
#ifdef DEBUG
			printf("etox_line_layout() - moving bit to %d,%d. Bit text is (%s)\n",
			       x, ty, etox_style_get_text(bit));
#endif
			evas_object_move(bit, x, ty);
		}
#ifdef DEBUG
		else
			printf("Encountered an obstacle!!\n");
#endif

		/*
		 * Move horizontally to place the next bit.
		 */
		x += tw;
	}
#ifdef DEBUG
	printf("etox_line_layout() - done\n");
#endif
}

/*
 * etox_line_minimize - reduce the number of bits on a line
 */
void etox_line_minimize(Etox_Line * line)
{
	Evas_Object *bit, *last_bit = NULL;
	Eina_List *l;

	CHECK_PARAM_POINTER("line", line);

	l = line->bits;
	if (!l)
		return;

	last_bit = l->data;
	l = l->next;
	while (l) {
		bit = l->data;

		/*
		 * Attempt to merge the bits if possible, remove the second
		 * one if successful.
		 */
		if (etox_style_merge(last_bit, bit)) {
			line->bits = eina_list_remove(line->bits, bit);
			l = eina_list_data_find_list(line->bits, last_bit);
			l = l->next;
		}
		else {
			last_bit = bit;
			l = l->next;
		}
	}
}

/*
 * etox_line_merge_append - merge lines into the first line, empty the second
 * @line1: the destination of the merged lines
 * @line2: the line that will be merged with line1
 *
 * Returns no value. Moves the bits from line2 into line 1.
 */
void etox_line_merge_append(Etox_Line * line1, Etox_Line * line2)
{
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line1", line1);
	CHECK_PARAM_POINTER("line2", line2);

	/*
	 * Move the bits from line2 to line1.
	 */
	while (line2->bits) {
		bit = line2->bits->data;
		line1->bits = eina_list_append(line1->bits, bit);
		line2->bits = eina_list_remove(line2->bits, bit);
		line1->length += etox_style_length(bit);

        	etox_selections_update(bit, line1);
	}
	/*
	 * Adjust the height, width and length of the merged line.
	 */
	line1->w += line2->w;
	if (line2->h > line1->h)
		line1->h = line2->h;
}

/*
 * etox_line_merge_prepend - merge lines into the second line, empty the first
 * @line1: the destination of the merged lines
 * @line2: the line that will be merged with line1
 *
 * Returns no value. Moves the bits from line2 into line 1.
 */
void etox_line_merge_prepend(Etox_Line * line1, Etox_Line * line2)
{
	Evas_Object *bit;

	CHECK_PARAM_POINTER("line1", line1);
	CHECK_PARAM_POINTER("line2", line2);

	/*
	 * Move the bits from line2 to line1.
	 */
	while (line1->bits) {
		bit = line1->bits->data;
		line2->bits = eina_list_prepend(line2->bits, bit);
		line1->bits = eina_list_remove(line1->bits, bit);
		line2->length += etox_style_length(bit);
	}
	/*
	 * Adjust the height, width and length of the merged line.
	 */
	line2->w += line1->w;
	if (line1->h > line2->h)
		line2->h = line1->h;
}

/*
 * etox_line_get_text - retrieve the text from a specified line into a buffer
 * @line: the line to retrieve text
 * @buf: the char buffer to store the found text, must have enough space
 *
 * Returns no value. Saves the text from the line @line into the char buffer
 * @buf.
 */
void etox_line_get_text(Etox_Line * line, char *buf, int len)
{
#ifdef DEBUG
	printf("etox_line_get_text() - called. len = %d\n", len);
#endif
	char *temp;
	Evas_Object *es;
	Eina_List *l;
	int sum = 0, pos = 0;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("buf", buf);

	/*
	 * Examine each bit on the list of bits and cat it's text onto the end
	 * of the buffer. Then append a \n to the buffer at the end of the
	 * line.
	 */
	if (len < 1) return;
	buf[0] = 0;
	for (l = line->bits; l; l = l->next) {
		int t;
		int tlen;
		es = l->data;

		sum += etox_style_length(es);
#ifdef DEBUG
		printf("etox_line_get_text() - etox_style_length() returned %d\n", etox_style_length(es));
#endif

		t = etox_style_get_type(es);
		if (t == ETOX_BIT_TYPE_WRAP_MARKER)
			continue;
		else if (t == ETOX_BIT_TYPE_TAB)
		{
			temp = strdup("\t");
			sum -= 7; // etox_style_length returns 8 but we're only inserting 1
		}                   
		else
			temp = etox_style_get_text(es);
		tlen = strlen(temp);
#ifdef DEBUG
		printf("etox_line_get_text() - actual length of returned text is %d\n", tlen);
#endif
		if (pos + tlen < len) {
#ifdef DEBUG
			printf("etox_line_get_text() - appending %d characters at pos = %d\n", tlen, pos);
#endif
			pos += tlen;
			strcat(buf, temp);
		}
		else {
#ifdef DEBUG
			printf("etox_line_get_text() - appending %d characters (limited) at pos = %d\n", (len-pos), pos);
#endif
			strncat(buf, temp, (len - pos));
			pos = len;
		}
		FREE(temp);
	}
	line->length = sum;
#ifdef DEBUG
	printf("etox_line_get_text() - done\n");
#endif
}

int
etox_line_wrap(Etox *et, Etox_Line *line)
{
	Eina_List *ll;
	Evas_Object *bit = NULL, *marker, *split_bit = NULL;
	Evas_Coord x, w, y, h;
	int index = -1, ok = 0;
	char *tmp;

#ifdef DEBUG
	printf("etox_line_wrap() - trying to wrap line:\n");
	etox_line_print_bits(line);
#endif

   ok= 1;
   for (ll = line->bits; ll && ok; ll = ll->next)
     {
	bit = ll->data;
	
	tmp = etox_style_get_text(bit);
	evas_object_geometry_get(bit, &x, &y, &w, &h);

	/* if we are down to 1 character... or 1 char +space - abort */
	if (
	    (strlen(tmp) <= 1) ||
	    ((strlen(tmp) == 2) && (tmp[1] == ' '))
	    ) {
	     if (w > et->w) {
		  ok = 0;
#ifdef DEBUG
		  printf("etox_line_wrap() - WARNING: Could not wrap line - etox too small!!\n");
#endif
	       }
	  }
	FREE(tmp);

        /* Find the bit that is on the border (right edje of the etox) */
	if (x + w > et->x + et->w) {
           split_bit = bit;
           break;
        }
     }

   if (!ok) {
      return -1;
   }

   if (!split_bit) {
#ifdef DEBUG
      printf("etox_line_wrap() - WARNING: Could not find the bit to split. Line "
             "width is probably wrong.\n");
#endif
      return -1;
   }

	/* get the index of the character on the edge */
	bit = split_bit;
	if (bit)
		index = etox_style_text_at_position(bit, et->x + et->w, y + (h / 2),
				NULL, NULL, NULL, NULL);
	if (index == -1) {
#ifdef DEBUG
	   printf("etox_line_wrap() - WARNING: Could not find the character within the "
	          "bit to split on. bit geometry is probably wrong?.\n");
#endif
	   return -1;
	}

	/* Adjust the index to find the actual character we want to wrap. Invalid
         * wrap locations:
         *  - index 0 of the first bit
         *  - index 0 of the second bit if this is a wrapped line
         */
	if (index > 0 ||
            (!(bit == line->bits->data) &&
             !(line->flags & ETOX_LINE_WRAPPED && bit == line->bits->next->data))) {
		char *tmp;

		tmp = etox_style_get_text(bit);

		/* If word wrap is on, back up to some whitespace */
		if (et->flags & ETOX_BREAK_WORDS) {
                        Evas_Object *original_bit = bit;
                        int space_index = index;
                        int done = 0;
                        int found_space = 0;

                        /* Back up until we find the proper word wrap index/bit */
			while (!done) {

                           /* If this is a space, we're done! */
                           if (isspace(tmp[space_index])) {
                              found_space = 1;
                              done = 1;
                           }

                           /* If this is the beginning of the bit and it is not the
                            * first bit, back up into the previous bit */
                           else if (space_index == 0 && bit != line->bits->data) {
                              ll = ll->prev;
                              bit = ll->data;
                              FREE(tmp);
                              tmp = etox_style_get_text(bit);
                              space_index = strlen(tmp) - 1;
                              if (space_index < 0) space_index = 0;
                           }

                           /* If we're at the beginning of the line - give up! */
                           else if (space_index == 0) {
                              done = 1;
                              /* Point to the original bit */
                              bit = original_bit;
                              FREE(tmp);
                              tmp = etox_style_get_text(bit);
#ifdef DEBUG
                              printf("etox_line_wrap() - WARNING: Could not word wrap line - "
			             "reverting to soft wrap.\n");
#endif
                           }

                           /* If this is not the beginning of the bit, back up */
                           else if (space_index > 0) {
                              space_index--;
                           }

			}

			/* If a space was found, then use it, otherwise revert to
 			   simple soft wrap and wrap at the character on the edge */
			if (found_space) {
			   index = space_index;
			}
		}

		/* don't start a new line with a space */
		while (index < strlen(tmp) && isspace(tmp[index]))
			index++;

		FREE(tmp);
	}

	/* Wrap if we've found a reasonable position. Invalid wrap locations:
	 *  - index 0 of the first bit
	 *  - index 0 of the second bit if this is a wrapped line
	 */
	if (index > 0 ||
	    (!(bit == line->bits->data) &&
	     !(line->flags & ETOX_LINE_WRAPPED && bit == line->bits->next->data))) {
#ifdef DEBUG
		char *tmp = etox_style_get_text(bit);
		printf("etox_line_wrap() - going to split line at index %d of bit:"
		       "(%s)\n", index, tmp);
		FREE(tmp);
#endif
		etox_line_split(line, bit, index);
		ll = eina_list_data_find_list(et->lines, line);
		ll = ll->next;

		/* create a marker bit. */
		marker = etox_style_new(et->evas, et->context->marker.text,
				et->context->marker.style);
		etox_style_set_type(marker, ETOX_BIT_TYPE_WRAP_MARKER);
		evas_object_smart_member_add(marker, et->smart_obj);
		evas_object_color_set(marker, et->context->marker.r,
				et->context->marker.g,
				et->context->marker.b,
				et->context->marker.a);
		evas_object_clip_set(marker, et->clip);
		etox_style_set_font(marker, et->context->font,
				et->context->font_size);
		evas_object_show(marker);
		if (et->context->marker.placement == ETOX_MARKER_BEGINNING)
			etox_line_prepend(ll->data, marker);
		else
			etox_line_append(line, marker);
	}
	else
		index = -1;

#ifdef DEBUG
		printf("etox_line_wrap() - done\n");
#endif
	return index;
}

void
etox_line_split(Etox_Line *line, Evas_Object *bit, int index)
{
	Eina_List *ll;
	Etox_Line *newline;
	Evas_Object *split = NULL;

	ll = eina_list_data_find_list(line->bits, bit);

	/*
	 * add the newline after the current one
	 */
	newline = etox_line_new(line->flags | ETOX_LINE_WRAPPED);
	newline->et = line->et;
	line->et->lines = eina_list_append_relative(line->et->lines, newline,
			line);

	/*
	 * If the bit starts on the boundary, simply move it to the next line.
	 */
	if (index > 0) {
		if (index < etox_style_length(bit)) {
			/*
 			 * FIXME: There appears to be a problem here where the widths
			 * of the split bits do not add up to the width of the original
			 * bit! It is noticeable when the first split bit ends with 
			 * a space. Is it counted in the middle of a bit, but not at
			 * the end? This causes a problem where the width of a line
			 * does not match the sum of its bits and then etox_line_wrap
			 * tries to wrap a line that does not have a bit on the edge.
			 */
			split = etox_split_bit(line, bit, index);
		}
		ll = ll->next;
	}

	/*
	 * Move the remaining bits to the new line
	 */
	while (ll) {
		/*
		 * Immediately move to the next object, as the node in the
		 * list pointed to by ll will get removed.
		 */
		bit = ll->data;
		ll = ll->next;

		etox_line_remove(line, bit);
		etox_line_append(newline, bit);
	}
}

void
etox_line_unwrap(Etox *et, Etox_Line *line)
{
	Eina_List *l, *prevline;
	Evas_Object *marker;

	if (!et->lines)
		return;

	prevline = eina_list_data_find_list(et->lines, line);

	l = prevline->next;
	while (l) {
		Eina_List *ll;

		line = l->data;
		if (!(line->flags & ETOX_LINE_WRAPPED))
			break;

		/* remove any wrap marker bits */
		ll = line->bits;
		while (ll) {
			int t;
			marker = ll->data;

			ll = ll->next;

			t = etox_style_get_type(marker);
			if (t == ETOX_BIT_TYPE_WRAP_MARKER) {
				line->bits = eina_list_remove(line->bits,
							      marker);
				evas_object_del(marker);
			}
		}

		/* remove the line from the list */
		et->lines = eina_list_remove(et->lines, line);

		/* merge the two lines */
		etox_line_merge_append(prevline->data, line);
		etox_line_free(line);

		l = prevline->next;
	}
        
//        etox_line_minimize(line);
}

Evas_Object *
etox_line_coord_to_bit(Etox_Line *line, int x)
{
	Evas_Coord bx;
	Eina_List *l = NULL;
	Evas_Object *bit = NULL;

	/*
	 * Find the bit on this line
	 */
	l = line->bits;
	while (l) {
		bit = l->data;
		evas_object_geometry_get(bit, &bx, NULL, NULL, NULL);
		if (bx < x)
			break;
		l = l->next;
	}

	return bit;
}

Evas_Object *
etox_line_index_to_bit(Etox_Line *line, int *i)
{
	int len = 0;
	Eina_List *l = NULL;
	Evas_Object *bit = NULL;

	l = line->bits;
	while (l) {
		bit = l->data;
		len += etox_style_length(bit);
		if (*i < len)
			break;
		l = l->next;
	}

	if (l)
		*i -= (len - etox_style_length(bit));

	return bit;
}

void
etox_line_print_bits(Etox_Line *line)
{
	int i = 0;
	Eina_List *l;

	for (l = line->bits; l; l = l->next) {
		printf("\tBit %d: (%s)\n", i, etox_style_get_text(l->data));
		i++;
	}
}

void
etox_line_set_layer(Etox_Line *line, int layer)
{
  Eina_List *l;

  if (!line->bits) return;
  
  for (l = line->bits; l; l = l->next)
  {
    Evas_Object *bit;

    bit = l->data;
    evas_object_layer_set(bit, layer);
  }
}

void
etox_line_index_to_geometry(Etox_Line *line, int index, Evas_Coord *x,
			    Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
	Evas_Object *bit = NULL;
	Eina_List *l;
	int sum = 0;

	/* find the bit containing the character */
	for (l = line->bits; l; l = l->next) {
	int length;
    
		bit = l->data;
		length = etox_style_length(bit);

		if (sum + length > index)
			break;

		sum += length;

		if (!l->next)
			bit = NULL;
	}

	/*
	 * No bit intersects, so set the geometry to the end of the
	 * line, with the average character width on the line
	 */
	if (!bit) {
		if (h)
			*h = line->h;
		if (w)
			*w = line->w / (line->length ? line->length : 1);
		if (y)
			*y = line->y;
		if (x)
			*x = line->x + line->w;
	}
	else
		/* get the geometry from the bit */
		etox_style_text_at(bit, index - sum, x, y, w, h); 
}

void
etox_line_apply_context(Etox_Line *line, Etox_Context *context, Evas_Object *start, Evas_Object *end)
{
#ifdef DEBUG
  printf("etox_line_apply_context() - called\n");
fflush(stdout);
#endif
  Eina_List *l, *ls = NULL, *le = NULL;

  ls = eina_list_data_find_list(line->bits, start);
  le = eina_list_data_find_list(line->bits, end);
#ifdef DEBUG
  printf("etox_line_apply_context() - found start and end bits\n");
fflush(stdout);
#endif
  
  /* make sure start and end exist and are in line->bits */
  if ( !ls )
    ls = line->bits;
  if ( !le ) 
    le = eina_list_last(line->bits);

  for (l = ls; l; l = l->next)
  {
    Evas_Object *bit;

    bit = l->data;

    if (!l->prev && line->flags & ETOX_LINE_WRAPPED)
    {
#ifdef DEBUG
  printf("etox_line_apply_context() - first bit of line. skipping obstacles...\n");
fflush(stdout);
#endif
      /* go past any obstacles */
      while (etox_style_fixed(bit))
      {
        /* if there are only obstacles on the line (can this happen?) */
        if (!l->next)
          return;

        l = l->next;
        bit = l->data;
      }
#ifdef DEBUG
  printf("etox_line_apply_context() - applying context to marker bit\n");
fflush(stdout);
#endif
      etox_style_set_text(bit, context->marker.text);
      etox_style_set_style(bit, context->marker.style);
      evas_object_color_set(bit, context->marker.r, context->marker.g,
                           context->marker.b, context->marker.a);
    }
    else
    {
#ifdef DEBUG
  printf("etox_line_apply_context() - applying context to bit\n");
fflush(stdout);
#endif
      etox_style_set_style(bit, context->style);
      evas_object_color_set(bit, context->r, context->g, context->b,
                            context->a);
      etox_style_set_font(bit, context->font, context->font_size);
    }
    if (l == le)
      break;
  }
#ifdef DEBUG
  printf("etox_line_apply_context() - done\n");
fflush(stdout);
#endif
}

void
etox_line_get_geometry(Etox_Line *line, Evas_Coord *x, Evas_Coord *y,
                       Evas_Coord *w, Evas_Coord *h)
{
  if (!line)
  {
    if (x) *x = 0;
    if (y) *y = 0;
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }
  
  if (x) *x = line->x;
  if (y) *y = line->y;
  if (w) *w = line->w;
  if (h) *h = line->h;
}
