#ifndef _ETOX_H
#define _ETOX_H

#include <Ewd.h>
#include <Edb.h>
#include <Evas.h>
#include <Estyle.h>

/*
 * Simple alignment bitfield
 */
typedef enum _etox_alignment Etox_Alignment;
enum _etox_alignment {
	ETOX_ALIGN_CENTER = 0,
	ETOX_ALIGN_LEFT = 1,
	ETOX_ALIGN_RIGHT = 2,
	ETOX_ALIGN_TOP = 4,
	ETOX_ALIGN_BOTTOM = 8
};

/*
 * The color struct simply keeps track of the various colors available
 */
typedef struct _etox_color Etox_Color;
struct _etox_color {
	int a, r, g, b;
};

/*
 * Text layout requires knowing the font layout, size, ascent and descent.
 */
typedef struct _etox_font Etox_Font;
struct _etox_font {
	char *name;
	int size, ascent, descent;
};

/*
 * Etox obstacles keep track of the lines that they intersect and the bit that
 * represents it.
 */
typedef struct _etox_obstacle Etox_Obstacle;
struct _etox_obstacle {
	Estyle *bit;
	int start_line;
	int end_line;
};

/*
 * The info structure keeps the important information about the style, but not
 * the bits used to display the text.
 */
typedef struct _etox_style_info Etox_Style_Info;
struct _etox_style_info {
	char *name;
	E_DB_File *style_db;
	Ewd_Sheap *layers;
	int references;
};

/*
 * The context structure holds information relative to the appearance of text
 * that is added to the etox.
 */
typedef struct _etox_context Etox_Context;
struct _etox_context {
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
	char align;

	/*
	 * Padding surrounding the text
	 */
	int padding;
};

/*
 * The etox keeps track of the display and layout information for all of the
 * text enclosed.
 */
typedef struct _etox Etox;
struct _etox {
	/*
	 * Evas for drawing the text
	 */
	Evas evas;

	/*
	 * Clip box on evas that bounds the text display and applies an alpha
	 * layer.
	 */
	Evas_Object clip;

	/*
	 * The layer in the evas to set the text
	 */
	int layer;

	/*
	 * Geometry of the etox
	 */
	int x, y, w, h;

	/*
	 * The length text in the etox
	 */
	int length;

	/*
	 * The current context that is used when adding text
	 */
	Etox_Context *context;

	/*
	 * List of lines in the etox
	 */
	Ewd_List *lines;

	/*
	 * List of obstacles in the etox
	 */
	Ewd_List *obstacles;

	/*
	 * Determine if the etox has been displayed yet.
	 */
	char visible;

	/*
	 * Alpha level of clip box that is applied to the text
	 */
	int alpha;
};

/*
 * Etox creation and deletion functions
 */
Etox *etox_new(Evas evas);
Etox *etox_new_all(Evas evas, int x, int y, int w, int h, int alpha,
		   Etox_Alignment align);
void etox_free(Etox * et);

/*
 * Visibility altering functions
 */
void etox_show(Etox * et);
void etox_hide(Etox * et);

/*
 * Context management functions
 */
Etox_Context *etox_context_save(Etox * et);
void etox_context_load(Etox * et, Etox_Context * context);
void etox_context_free(Etox_Context * context);

/*
 * Color context management functions
 */
void etox_context_get_color(Etox * et, int *r, int *g, int *b, int *a);
void etox_context_set_color(Etox * et, int r, int g, int b, int a);
void etox_context_set_color_db(Etox * et, char *name);

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
char *etox_context_get_font(Etox * et, int *size);
void etox_context_set_font(Etox * et, char *fontname, int size);

/*
 * Style context management functions
 */
char *etox_context_get_style(Etox * et);
void etox_context_set_style(Etox * et, char *stylename);

/*
 * Alignment context management functions
 */
int etox_context_get_align(Etox * et);
void etox_context_set_align(Etox * et, int align);

/*
 * Text manipulation functions
 */
void etox_append_text(Etox * et, char *text);
void etox_prepend_text(Etox * et, char *text);
void etox_insert_text(Etox * et, char *text, int index);
void etox_set_text(Etox * et, char *text);
char *etox_get_text(Etox * et);
void etox_clear(Etox * et);

/*
 * Geometry altering functions
 */
void etox_move(Etox * et, int x, int y);
void etox_resize(Etox * et, int w, int h);

/*
 * Geometry retrieval functions
 */
void etox_get_geometry(Etox * et, int *x, int *y, int *w, int *h);
int etox_coord_to_index(Etox * et, int x, int y);
void etox_index_to_geometry(Etox * et, int index, int *x, int *y,
		int *w, int *h);
int etox_coord_to_geometry(Etox * et, int xc, int yc, int *x, int *y,
		int *w, int *h);

/*
 * Appearance altering functions
 */
void etox_set_layer(Etox * et, int layer);
void etox_set_clip(Etox * et, Evas_Object clip);
void etox_set_alpha(Etox * et, int alpha);

/*
 * Region selection and release
 */
Ewd_List *etox_region_select(Etox * et, int start, int end);
Ewd_List *etox_region_select_str(Etox * et, char *search, char *last);
void etox_region_release(Ewd_List * region);

/*
 * Region altering appearance modifiers
 */
void etox_region_set_font(Ewd_List * region, char *name, int size);
void etox_region_set_color(Ewd_List * region, int r, int g, int b, int a);
void etox_region_set_style(Ewd_List * region, char *stylename);

/*
 * Obstacle manipulation functions
 */
Etox_Obstacle *etox_obstacle_add(Etox * et, int x, int y, int w, int h);
void etox_obstacle_remove(Etox * et, Etox_Obstacle * obstacle);
void etox_obstacle_move(Etox * et, Etox_Obstacle * obstacle, int x, int y);
void etox_obstacle_resize(Etox * et, Etox_Obstacle * obstacle, int w,
			  int h);

#endif
