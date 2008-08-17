#include "Etox_private.h"

/*
 * etox_obstacle_new - create a new obstacle with given dimensions
 * @et: the etox to add an obstacle
 * @x: the x coordinate of the obstacle that is added to @et
 * @y: the y coordinate of the obstacle that is added to @et
 * @w: the width of the obstacle that is added to @et
 * @h: the height of the obstacle that is added to @et
 *
 * Returns the newly created obstacle on success, NULL on failure. Creates a
 * new obstacle on the etox @et for the text to wrap around.
 */
Etox_Obstacle *etox_obstacle_new(Etox * et, int x, int y, int w, int h)
{
	Etox_Obstacle *obst;

	CHECK_PARAM_POINTER_RETURN("et", et, NULL);

	/*
	 * Allocate the obstacle and bit. Also, give the obstacle's bit correct
	 * position and fixed geometry.
	 */
	obst = (Etox_Obstacle *) calloc(1, sizeof(Etox_Obstacle));
	if (obst) {
		obst->et = et;
		obst->bit = etox_style_new(et->evas, "", NULL);
		etox_style_set_type(obst->bit, ETOX_BIT_TYPE_OBSTACLE);
		evas_object_smart_member_add(obst->bit, et->smart_obj);
		etox_style_fix_geometry(obst->bit, x, y, w, h);
	}

	return obst;
}

/*
 * etox_obstacle_free - remove an obstacle and free it
 */
void etox_obstacle_free(Etox * et, Etox_Obstacle * obstacle)
{
	CHECK_PARAM_POINTER("obstacle", obstacle);

	etox_obstacle_unplace(obstacle);

	FREE(obstacle);
}

/*
 * etox_obstacle_place - position an obstacle in the etox
 * @et: the etox to place the obstacle within
 * @obst: the obstacle to place in the etox
 *
 * Returns no value. Places the obstacle @obst within the lines of etox @et.
 */
void etox_obstacle_place(Etox_Obstacle * obst)
{
	int i = 0;
	Evas_Coord x, y, w, h;
	Etox_Line *line;
	Evas_List *l;

	CHECK_PARAM_POINTER("obst", obst);

	/*
	 * Check the simple return cases first, ie. does the obstacle fall
	 * outside of the etox.
	 */
	evas_object_geometry_get(obst->bit, &x, &y, &w, &h);
	if (x > obst->et->x + obst->et->w)
		return;

	if (x + w < obst->et->x)
		return;

	if (y > obst->et->y + obst->et->h)
		return;

	if (y + h < obst->et->y)
		return;

	/*
	 * Run through to determine the lines to determine which intersect the
	 * obstacle
	 */
	for (l = obst->et->lines; l; l = l->next) {
		line = l->data;

		if (line->y > y + h)
			break;

		if (line->y + line->h >= y)
			etox_obstacle_line_insert(line, obst);

		/*
		 * Check if the obstacle starts at this line
		 */
		i++;
	}
}

/*
 * etox_obstacle_unplace - set empty positioning on an obstacle in the etox
 */
void etox_obstacle_unplace(Etox_Obstacle * obst)
{
	Etox_Line *line;

	CHECK_PARAM_POINTER("obst", obst);

	/*
	 * On each line within the obstacle bounds, remove the obstacle from
	 * the list of bits.
	 */
	while (obst->lines) {
		line = obst->lines->data;
		etox_line_remove(line, obst->bit);
		obst->lines = evas_list_remove(obst->lines, line);
	}
}

/*
 * etox_obstacle_line_insert - place an obstacle within a line
 */
void etox_obstacle_line_insert(Etox_Line * line, Etox_Obstacle * obst)
{
	int i;
	Evas_Object *bit;
	Evas_Coord x;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("obst", obst);

	evas_object_geometry_get(obst->bit, &x, NULL, NULL, NULL);

	/*
	 * Find the position to place the obstacle within the line
	 */
	bit = etox_line_coord_to_bit(line, x);
	if (!bit)
		return;

	/*
	 * Find the index into the line of the obstacle.
	 */
	i = etox_style_text_at_position(bit, x, line->y + (line->h / 2), NULL, NULL,
			NULL, NULL);

	/*
	 * Check if we can append it after this bit, possibly after splitting.
	 */
	if (i > 0) {
		if (i < etox_style_length(bit)) {
			etox_split_bit(line, bit, i);
		}
		line->bits = evas_list_append_relative(line->bits, obst->bit,
				bit);
	}
	else {
		/*
		 * Otherwise, stick it in front of this bit.
		 */
		line->bits = evas_list_prepend_relative(line->bits, obst->bit,
				bit);
	}

	obst->lines = evas_list_append(obst->lines, line);
}

/*
 * etox_rect_intersect - check for intersection on two rectangles
 */
inline int
etox_rect_intersect(Evas_Coord x1, Evas_Coord y1, Evas_Coord w1, Evas_Coord h1,
		    Evas_Coord x2, Evas_Coord y2, Evas_Coord w2, Evas_Coord h2)
{
	if (x1 > x2 + w2)
		return FALSE;

	if (y1 > y2 + h2)
		return FALSE;

	if (x1 + w1 < x2)
		return FALSE;

	if (y1 + h1 < y2)
		return FALSE;

	return TRUE;
}
