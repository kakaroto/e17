#ifndef _EWL_ROW_H
#define _EWL_ROW_H

/**
 * @file ewl_row.h
 *
 * @defgroup Ewl_Row Row The Row widget
 * @{
 */

/**
 * @themekey /row/file
 * @themekey /row/group
 */

#define EWL_ROW_TYPE "row"

typedef struct Ewl_Row Ewl_Row;
#define EWL_ROW(t) ((Ewl_Row *)t)

/*
 * Tree rows layout cells in a left to right fashion.
 */
struct Ewl_Row
{
	Ewl_Container container;
	Ewl_Row *header;
};

Ewl_Widget 	*ewl_row_new(void);
int 		 ewl_row_init(Ewl_Row *row);

void 		 ewl_row_header_set(Ewl_Row *row, Ewl_Row *header);
Ewl_Widget 	*ewl_row_column_get(Ewl_Row *row, short n);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_row_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_row_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_row_header_configure_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);
void ewl_row_header_destroy_cb(Ewl_Widget *w, void *ev_data,
						void *user_data);

void ewl_row_child_show_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_row_child_hide_cb(Ewl_Container *c, Ewl_Widget *w);
void ewl_row_resize_cb(Ewl_Container *c, Ewl_Widget *w, int size,
					       Ewl_Orientation o);

/**
 * @}
 */

#endif
