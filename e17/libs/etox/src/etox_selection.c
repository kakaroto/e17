#include "Etox_private.h"

Evas_List *active_selections = NULL;

Estyle *
etox_selection_split_bit(Etox_Line *line, Estyle *bit, int index)
{
	Evas_List *l;
	Estyle *point = bit;
	Etox_Selection *selected;

	/*
	 * Split the leading bit, only need to inform selections that end with
	 * this bit.
	 */
	if (index && index < estyle_length(bit)) {
		point = estyle_split(bit, index);
		line->bits = evas_list_append_relative(line->bits, point, bit);

		l = active_selections;
		while (l) {
			selected = l->data;
			if (selected->end.bit == bit)
				selected->end.bit = point;
			l = l->next;
		}
	}

	return point;
}

Etox_Selection *
etox_selection_new(Etox *etox, Etox_Line *l1, Etox_Line *l2,
		Estyle *s1, Estyle *s2, int i1, int i2)
{
	Estyle *temp;
	Etox_Selection *selected;

	/*
	 * Split bits on their index boundaries, this updates selections that
	 * contain the bits.
	 */
	temp = etox_selection_split_bit(l1, s1, i1);
	if (s1 == s2) {
		i2 -= i1;
		s2 = temp;
	}
	s1 = temp;

	/*
	 * Split on the ending index, we use the original s2 for the end,
	 * since it's the bit portion before the split.
	 */
	etox_selection_split_bit(l2, s2, i2);

	selected = calloc(1, sizeof(Etox_Selection));
	memset(selected, 0, sizeof(Etox_Selection));

	selected->etox = etox;

	selected->start.line = l1;
	selected->start.bit = s1;

	selected->end.line = l2;
	selected->end.bit = s2;

	selected->context = etox_context_save(etox);

	active_selections = evas_list_prepend(active_selections, selected);

	return selected;
}

/**
 */
void
etox_selection_free(Etox_Selection *selected)
{
	etox_context_free(selected->context);
	active_selections = evas_list_remove(active_selections, selected);
	FREE(selected);
}

/**
 */
void
etox_selection_free_by_etox(Etox *etox)
{
	Evas_List *l;
	Etox_Selection *selected;

	l = active_selections;
	while (l) {
		selected = l->data;
		if (selected->etox == etox) {
			active_selections = evas_list_remove(active_selections, selected);
			etox_context_free(selected->context);
			FREE(selected);
		}
	}
}

/**
 */
Etox_Selection *
etox_select_coords(Etox * et, int sx, int sy, int ex, int ey)
{
	int i1, i2;
	Etox_Line *sl, *el = NULL;
	Estyle *sb, *eb = NULL;
	Etox_Selection *selected = NULL;

	sl = _etox_coord_to_line(et, sy);
	if (!sl)
		goto out;

	el = _etox_coord_to_line(et, ey);
	if (!el)
		goto out;

	sb = etox_line_coord_to_bit(sl, sx);
	if (!sb)
		goto out;

	eb = etox_line_coord_to_bit(el, ex);
	if (!eb)
		goto out;

	i1 = estyle_text_at_position(sb, sx, sy, NULL, NULL, NULL, NULL);
	i2 = estyle_text_at_position(eb, sx, sy, NULL, NULL, NULL, NULL);

	selected = etox_selection_new(et, sl, el, sb, eb, i1, i2);

out:
	return selected;
}

/**
 * etox_select_index - create a selection based on two indices
 * @et: the etox to choose the selection
 * @si: the starting index of characters to be selected
 * @ei: the ending index of characters to be selected
 *
 * Returns a newly allocated selection on success, NULL on failure.
 */
Etox_Selection *
etox_select_index(Etox * et, int si, int ei)
{
	Etox_Line *sl = NULL, *el = NULL;
	Estyle *sb = NULL, *eb = NULL;
	Etox_Selection *selected = NULL;

	/*
	 * First determine the lines containing the indices.
	 */
	sl = _etox_index_to_line(et, &si);
	if (!sl)
		goto out;

	el = _etox_index_to_line(et, &ei);
	if (!el)
		goto out;

	sb = etox_line_index_to_bit(sl, &si);
	if (!sb)
		goto out;

	eb = etox_line_index_to_bit(sl, &ei);
	if (!eb)
		goto out;

	/*
	 * Create the new selection and assign it's fields
	 */
	selected = etox_selection_new(et, sl, el, sb, eb, si, ei);

out:
	return selected;
}

/**
 */
Etox_Selection *
etox_select_str(Etox * et, char *match, char **last)
{
	return NULL;
}

/**
 */
void
etox_selection_bounds(Etox_Selection *selected, int *sx, int *sy,
		int *ex, int *ey)
{
}

/**
 */
void
etox_selection_set_font(Etox_Selection *selected, char *font, int font_size)
{
	Evas_List *l, *bl;
	Etox_Line *line;
	Estyle *bit;

	/*
	 * Make the necessary context changes.
	 */
	IF_FREE(selected->context->font);
	selected->context->font = strdup(font);
	selected->context->font_size = font_size;

	/*
	 * Apply the settings to all bits between the selections start and end
	 * points.
	 */
	line = selected->start.line;
	l = evas_list_find_list(selected->etox->lines, selected->start.line);
	bl = evas_list_find_list(line->bits, selected->start.bit);
	while (bl) {
		bit = bl->data;
		estyle_set_font(bit, font, font_size);

		if (bit == selected->end.bit)
			goto out;

		if (!bl) {
			l = l->next;
			line = l->data;
			bl = line->bits;
		}
		else
			bl = bl->next;
	}

out:
	return;
}

/**
 */
void
etox_selection_set_style(Etox_Selection *selected, char *style)
{
	Evas_List *l, *bl;
	Etox_Line *line;
	Estyle *bit;

	/*
	 * Make the necessary context changes.
	 */
	IF_FREE(selected->context->style);
	selected->context->style = strdup(style);

	/*
	 * Apply the settings to all bits between the selections start and end
	 * points.
	 */
	line = selected->start.line;
	l = evas_list_find_list(selected->etox->lines, selected->start.line);
	bl = evas_list_find_list(line->bits, selected->start.bit);
	while (bl) {
		bit = bl->data;
		estyle_set_style(bit, style);

		if (bit == selected->end.bit)
			goto out;

		if (!bl) {
			l = l->next;
			line = l->data;
			bl = line->bits;
		}
		else
			bl = bl->next;
	}

out:
	return;
}

/**
 */
void
etox_selection_add_callback(Etox_Selection *selected,
		Evas_Callback_Type callback, void (*func) (void *data, Evas *e,
					       Evas_Object *o, int b, int x,
					       int y), void *data)
{
}

/**
 */
void
etox_selection_del_callback(Etox_Selection *selected,
		Evas_Callback_Type callback)
{
}
