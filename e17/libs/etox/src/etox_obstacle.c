#include "Etox_private.h"

static void _etox_obstacle_line_insert(Etox_Line * line,
				       Etox_Obstacle * obst);

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
	obst = (Etox_Obstacle *) malloc(sizeof(Etox_Obstacle));
	obst->bit = estyle_new(et->evas, "", NULL);
	estyle_fix_geometry(obst->bit, x, y, w, h);

	return obst;
}

/*
 * etox_obstacle_free - remove an obstacle and free it
 */
void etox_obstacle_free(Etox * et, Etox_Obstacle * obstacle)
{
	CHECK_PARAM_POINTER("obstacle", obstacle);

	etox_obstacle_unplace(et, obstacle);

	FREE(obstacle);
}

/*
 * etox_obstacle_place - position an obstacle in the etox
 * @et: the etox to place the obstacle within
 * @obst: the obstacle to place in the etox
 *
 * Returns no value. Places the obstacle @obst within the lines of etox @et.
 */
void etox_obstacle_place(Etox * et, Etox_Obstacle * obst)
{
	int i = 0;
	int j = 0;
	int x, y, w, h;
	Etox_Line *line;
	Evas_List l_nodes, l;

	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("obst", obst);

	/*
	 * Check the simple return cases first, ie. does the obstacle fall
	 * outside of the etox.
	 */
	estyle_geometry(obst->bit, &x, &y, &w, &h);
	if (x > et->x + et->w)
		return;

	if (x + w < et->x)
		return;

	if (y > et->y + et->h)
		return;

	if (y + h < et->y)
		return;

	/*
	 * We know the obstacle intersects this etox, so now determine
	 * starting and ending lines, as well as split lines appropriately.
	 */
	for (l_nodes = et->lines; l_nodes; l_nodes = l_nodes->next)
		j++;
	obst->start_line = j;
	obst->end_line = -1;

	/*
	 * Run through to determine the lines to determine which intersect the
	 * obstacle
	 */
	for (l = et->lines; l; l = l->next) {
		line = l->data;
		if (line->y < y + h) {
			if (line->y > y) {
				/*
				 * Check if the obstacle starts at this line
				 */
				if (i < obst->start_line)
					obst->start_line = i;
				_etox_obstacle_line_insert(line, obst);
			}
		} else
			break;

		/*
		 * Check if the obstacle starts at this line
		 */
		i++;
	}

	obst->end_line = i - 1;
}

/*
 * etox_obstacle_unplace - set empty positioning on an obstacle in the etox
 */
void etox_obstacle_unplace(Etox * et, Etox_Obstacle * obst)
{
	int i, j;
	Estyle *bit;
	Etox_Line *line;
	Evas_List l, ll;

	CHECK_PARAM_POINTER("et", et);
	CHECK_PARAM_POINTER("obst", obst);

	/*
	 * Only adjust the lines that intersect the obstacle.
	 */
	i = obst->start_line;

	/*
	 * On each line within the obstacle bounds, remove the obstacle from
	 * the list of bits.
	 */
	for (j = 1, l = et->lines; j <= obst->end_line && l;
	     l = l->next, j++) {
		if (j < i);
		else {
			line = l->data;
			/*
			 * Now find the obstacle on the list of bits and remove it.
			 */
			for (ll = line->bits; ll; ll = ll->next) {
				bit = ll->data;
				if (bit != obst->bit);
				else {
					ll = evas_list_remove(ll, bit);
					break;
				}
			}
		}
	}
}

/*
 * etox_obstacle_line_insert - place an obstacle within a line
 */
static void _etox_obstacle_line_insert(Etox_Line * line,
				       Etox_Obstacle * obst)
{
	Estyle *bit;
	int x, y, w, h;
	Evas_List l;

	CHECK_PARAM_POINTER("line", line);
	CHECK_PARAM_POINTER("obst", obst);

	estyle_geometry(obst->bit, &x, &y, &w, &h);

	/*
	 * Find the position to place the obstacle within the line
	 */
	for (l = line->bits; l; l = l->next) {
		int tx, ty, tw, th;
		bit = l->data;

		estyle_geometry(bit, &tx, &ty, &tw, &th);
		if (etox_rect_intersect(x, y, w, h, tx, ty, tw, th)) {
			if (!bit)
				return;
			/*
			   * FIXME: We need to do some bit-splitting here, just need to get
			   * around to it.
			 */
			l = evas_list_prepend_relative(l, obst->bit,
						       l->next);
			break;
		}
	}
}

/*
 * etox_rect_intersect - check for intersection on two rectangles
 */
inline int etox_rect_intersect(int x1, int y1, int w1, int h1,
			       int x2, int y2, int w2, int h2)
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
