#ifndef _ETOX_STYLE_STYLE_H
#define _ETOX_STYLE_STYLE_H


/*
 * The style layer must keep track of the positioning and color modifications
 * of the bits appearance.
 */
typedef struct _etox_style_style_layer Etox_Style_Style_Layer;
struct _etox_style_style_layer
{
	int stack;
	Evas_Coord x_offset, y_offset;
	int size_change;

	int a, r, g, b;
	int relative_color;
	int blend_alpha;
};

/*
 * The info structure keeps the important information about the style, but not
 * the bits used to display the text.
 */
typedef struct _etox_style_style_info Etox_Style_Style_Info;
struct _etox_style_style_info
{
	char *name;
	E_DB_File *style_db;
	Etox_Style_Heap *layers;
	Evas_Coord left_push, right_push, top_push, bottom_push;

	int references;
};

/*
 * The style of a text keeps a reference to style info for deciding how to
 * alter the text, and a list of the bits that are created by the style.
 */
typedef struct _etox_style_style Etox_Style_Style;
struct _etox_style_style
{
	Etox_Style_Style_Info *info;
	Evas_List *bits;
};


/*
 * Style constructors and destructors
 */
Etox_Style_Style *_etox_style_style_instance(char *name);
void _etox_style_style_release(Etox_Style_Style * style, Evas *ev);
void _etox_style_style_info_collect();

/*
 * Style display modifiers.
 */
void _etox_style_style_draw(Etox_Style * et, char *text);
void _etox_style_style_move(Etox_Style * bit);
void _etox_style_style_set_clip(Etox_Style * bit, Evas_Object *ob);
void _etox_style_style_set_color(Etox_Style * bit);
void _etox_style_style_set_font(Etox_Style *es, char *font, int size);
void _etox_style_style_set_text(Etox_Style *es);
int _etox_style_style_set_layer_lower(Etox_Style * bit, int layer);
int _etox_style_style_set_layer_upper(Etox_Style * bit, int layer, int start);

/*
 * Style info access functions
 */
void _etox_style_style_info_dereference(Etox_Style_Style_Info *info);
Etox_Style_Style_Info *_etox_style_style_info_reference(char *name);

/*
 * Style show and hide functions
 */
void _etox_style_style_show(Etox_Style *es);
void _etox_style_style_hide(Etox_Style *es);

/*
 * Style search path manipulators
 */
void _etox_style_style_add_path(char *path);
void _etox_style_style_remove_path(char *path);

#endif
