#ifndef _ETOX_REGION_H
#define _ETOX_REGION_H

/*
 * These functions select regions of the etox.
 */
Etox_Selection *etox_select_coords(Etox * et, int sx, int sy, int ex,
				   int ey);
Etox_Selection *etox_select_index(Etox * et, int si, int ei);
Etox_Selection *etox_select_str(Etox * et, char *match, char **last);

/*
 * Release a selection that is no longer needed.
 */
void etox_selection_free(Etox_Selection * selected);

/*
 * This function gets a rectangular bound on the selection.
 */
void etox_selection_bounds(Etox_Selection * selected, int *x, int *y,
			   int *w, int *h);

/*
 * These methods alter the appearance of the selected region.
 */
void etox_selection_set_font(Etox_Selection * selected, char *font,
			     int size);
void etox_selection_set_style(Etox_Selection * selected, char *style);

/*
 * These functions manipulate callbacks on the selected region.
 */
void etox_selection_add_callback(Etox_Selection * selected,
				 Evas_Callback_Type callback,
				 void (*func) (void *data, Evas e,
					       Evas_Object o, int b, int x,
					       int y), void *data);
void etox_selection_del_callback(Etox_Selection * selected,
				 Evas_Callback_Type callback);

#endif
