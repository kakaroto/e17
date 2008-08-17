#ifndef _ETOX_H
#define _ETOX_H

#include <Edb.h>
#include <Evas.h>

/*
 * Simple alignment bitfield
 */
enum _etox_alignment
{
	ETOX_ALIGN_CENTER = 0,
	ETOX_ALIGN_LEFT = 1,
	ETOX_ALIGN_RIGHT = 2,
	ETOX_ALIGN_TOP = 4,
	ETOX_ALIGN_BOTTOM = 8
};
typedef enum _etox_alignment Etox_Alignment;

#define ETOX_ALIGN_MASK 0xF

enum _etox_bit_type
{
	ETOX_BIT_TYPE_TEXT = 0,
	ETOX_BIT_TYPE_OBSTACLE = 1,
	ETOX_BIT_TYPE_WRAP_MARKER = 2,
	ETOX_BIT_TYPE_TAB = 3
};
typedef enum _etox_bit_type Etox_Bit_Type;

enum _etox_marker_placement
{
	ETOX_MARKER_BEGINNING = 0,
	ETOX_MARKER_END = 1,
};
typedef enum _etox_marker_placement Etox_Marker_Placement;

/*
 * The context structure holds information relative to the appearance of text
 * that is added to the etox.
 */
typedef struct _etox_context Etox_Context;
struct _etox_context
{
	/*
	 * Color for displaying the text
	 */
	int r;
	int g;
	int b;
	int a;

	/*
	 * Font used for displaying the text
	 */
	char *font;

	/*
	 * The size of the font used for displaying the text
	 */
	int font_size;

	/*
	 * Style used for displaying the text
	 */
	char *style;

	/*
	 * Default alignment of text on etox
	 */
	char flags;

	/*
	 * Padding surrounding the text
	 */
	int padding;

	/*
	 * A marker for wrapped lines
	 */
	struct
	{
		char *text;
		char *style;
		int r, g, b, a;
		Etox_Marker_Placement placement;
	} marker;
};

typedef struct _etox_rect Etox_Rect;
struct _etox_rect
{
	Evas_Coord x, y;
	Evas_Coord w, h;
};

typedef struct _etox_obstacle Etox_Obstacle;
typedef struct _etox_selection Etox_Selection;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Etox creation and deletion functions
 */
Evas_Object *etox_new(Evas *evas);
Evas_Object *etox_new_all(Evas *evas, Evas_Coord x, Evas_Coord y, Evas_Coord w,
			  Evas_Coord h, int alpha, Etox_Alignment align);
void etox_gc_collect();

/*
 * Context management functions
 */
Etox_Context *etox_context_new();
Etox_Context *etox_get_context(Evas_Object * et);
void etox_set_context(Evas_Object * et, Etox_Context * context);
void etox_context_copy(Etox_Context *dst, Etox_Context *src);
Etox_Context *etox_context_save(Evas_Object * et);
void etox_context_load(Evas_Object * et, Etox_Context * context);
void etox_context_free(Etox_Context * context);

/*
 * Color context management functions
 */
void etox_context_get_color(Etox_Context * context, int *r, int *g, int *b, int *a);
void etox_context_set_color(Etox_Context * context, int r, int g, int b, int a);
void etox_context_set_color_db(Etox_Context * context, char *name);

/*
 * Callback context management functions
 */
/*
int etox_context_clear_callbacks(Etox *et);
int etox_context_add_callback(Etox *et, int type, Etox_Cb_Func func, void *data);
int etox_context_del_callback(Etox *et, int index);
*/

/*
 * Font context managment functions
 */
char *etox_context_get_font(Etox_Context * context, int *size);
void etox_context_set_font(Etox_Context * context, char *fontname, int size);

/*
 * Style context management functions
 */
char *etox_context_get_style(Etox_Context * context);
void etox_context_set_style(Etox_Context * context, char *stylename);

/*
 * Alignment context management functions
 */
int etox_context_get_align(Etox_Context * context);
void etox_context_set_align(Etox_Context * context, int align);

/* 
 * Wrap marker functions
 */
void etox_context_set_wrap_marker(Etox_Context * context, char *marker, char *style);
void etox_context_set_wrap_marker_color(Etox_Context * context, int r, int g, int b, int a);
Etox_Marker_Placement etox_context_get_wrap_marker_place(Etox_Context *context);
void etox_context_set_wrap_marker_place(Etox_Context *context,
					Etox_Marker_Placement place);

/*
 * Text manipulation functions
 */
void etox_append_text(Evas_Object * et, char *text);
void etox_prepend_text(Evas_Object * et, char *text);
void etox_insert_text(Evas_Object * et, char *text, int index);
void etox_delete_text(Evas_Object * et, unsigned int index, unsigned int len);
void etox_set_text(Evas_Object * et, char *text);
char *etox_get_text(Evas_Object * et);
void etox_clear(Evas_Object * et);

int etox_get_length(Evas_Object *obj);

void etox_text_geometry_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h);

/*
 * Geometry retrieval functions
 */
int etox_coord_to_index(Evas_Object * et, Evas_Coord x, Evas_Coord y);
void etox_index_to_geometry(Evas_Object * et, int index, Evas_Coord *x,
			    Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);
int etox_coord_to_geometry(Evas_Object * et, Evas_Coord xc, Evas_Coord yc,
			   Evas_Coord *x, Evas_Coord *y,
			   Evas_Coord *w, Evas_Coord *h);

void etox_set_soft_wrap(Evas_Object * et, int boolean);
void etox_set_word_wrap(Evas_Object * et, int boolean);

/*
 * Appearance altering functions
 */
void etox_set_alpha(Evas_Object * et, int alpha);

/*
 * Obstacle manipulation functions
 */
Etox_Obstacle *etox_obstacle_add(Evas_Object * et, Evas_Coord x, Evas_Coord y,
				 Evas_Coord w, Evas_Coord h);
void etox_obstacle_remove(Etox_Obstacle * obstacle);
void etox_obstacle_move(Etox_Obstacle * obstacle, Evas_Coord x, Evas_Coord y);
void etox_obstacle_resize(Etox_Obstacle * obstacle, Evas_Coord w, Evas_Coord h);

/*
 * These functions select regions of the etox.
 */
Etox_Selection *etox_select_coords(Evas_Object * et, Evas_Coord sx,
				   Evas_Coord sy, Evas_Coord ex, Evas_Coord ey);
Etox_Selection *etox_select_index(Evas_Object * et, int si, int ei);
Etox_Selection *etox_select_str(Evas_Object * et, char *match, int *index);

/*
 * Release a selection that is no longer needed.
 */
void etox_selection_free(Etox_Selection * selected);
void etox_selection_free_by_etox(Evas_Object *etox);

/*
 * This function gets a rectangular bound on the selection.
 */
void etox_selection_bounds(Etox_Selection * selected, Evas_Coord *x,
			   Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);

/*
 * These methods alter the appearance of the selected region.
 */
void etox_selection_set_font(Etox_Selection * selected, char *font, int size);
void etox_selection_set_style(Etox_Selection * selected, char *style);
void etox_selection_set_color(Etox_Selection *selected, int r, int g, int b,
		int a);
void etox_selection_set_wrap_marker(Etox_Selection *selected, char *marker,
		char *style);
void etox_selection_set_wrap_marker_color(Etox_Selection *selected, int r,
		int g, int b, int a);

/*
 * These functions manipulate callbacks on the selected region.
 */
void etox_selection_add_callback(Etox_Selection * selected,
				 Evas_Callback_Type callback,
				 void (*func) (void *data, Evas *e,
					       Evas_Object *o, int b, int x,
					       int y), void *data);
void etox_selection_del_callback(Etox_Selection * selected,
				 Evas_Callback_Type callback);
void etox_selection_apply_context(Etox_Selection *selected,
                                  Etox_Context *context);
Etox_Rect *etox_selection_get_geometry(Etox_Selection *selected, int *num);

/** Printout the text buffer to support debugging */
void etox_print_text(Evas_Object *obj);

#ifdef __cplusplus
}
#endif

#endif
