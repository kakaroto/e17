#ifndef _EWL_ROW_H
#define _EWL_ROW_H

typedef struct _ewl_row Ewl_Row;
#define EWL_ROW(t) ((Ewl_Row *)t)

/*
 * @themekey /row/file
 * @themekey /row/group
 */

/*
 * Tree rows layout cells in a left to right fashion.
 */
struct _ewl_row
{
	Ewl_Container container;
	Ewl_Row *header;
};

Ewl_Widget *ewl_row_new(void);
int ewl_row_init(Ewl_Row *row);

void ewl_row_set_header(Ewl_Row *row, Ewl_Row *header);
Ewl_Widget *ewl_row_get_column(Ewl_Row *row, short n);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_row_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_row_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_row_header_configure_cb(Ewl_Widget * w, void *ev_data,
				 void *user_data);
void ewl_row_header_destroy_cb(Ewl_Widget * w, void *ev_data,
			       void *user_data);

void ewl_row_add_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_row_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
		       Ewl_Orientation o);

#endif
