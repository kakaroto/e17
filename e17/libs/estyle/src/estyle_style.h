#ifndef _ESTYLE_STYLE_H
#define _ESTYLE_STYLE_H


/*
 * The style layer must keep track of the positioning and color modifications
 * of the bits appearance.
 */
typedef struct _estyle_style_layer Estyle_Style_Layer;
struct _estyle_style_layer {
	int stack;
	int x_offset, y_offset;
	int size_change;

	int a, r, g, b;
	int relative_color;
};

/*
 * The info structure keeps the important information about the style, but not
 * the bits used to display the text.
 */
typedef struct _estyle_style_info Estyle_Style_Info;
struct _estyle_style_info {
	char *name;
	E_DB_File *style_db;
	Estyle_Heap *layers;
	int left_push, right_push, top_push, bottom_push;

	int references;
};

/*
 * The style of a text keeps a reference to style info for deciding how to
 * alter the text, and a list of the bits that are created by the style.
 */
typedef struct _estyle_style Estyle_Style;
struct _estyle_style {
	Estyle_Style_Info *info;
	Evas_List bits;
};


/*
 * Style constructors and destructors
 */
Estyle_Style *_estyle_style_instance(char *name);
void _estyle_style_release(Estyle_Style * style, Evas ev);

/*
 * Style display modifiers.
 */
void _estyle_style_draw(Estyle * et, char *text);
void _estyle_style_move(Estyle * bit);
void _estyle_style_set_clip(Estyle * bit, Evas_Object ob);
void _estyle_style_set_color(Estyle * bit);
void _estyle_style_set_font(Estyle * es, char *font, int size);
void _estyle_style_set_text(Estyle * es);
int _estyle_style_set_layer_lower(Estyle * bit, int layer);
int _estyle_style_set_layer_upper(Estyle * bit, int layer, int start);

/*
 * Style info access functions
 */
void _estyle_style_info_dereference(Estyle_Style_Info * info);
Estyle_Style_Info *_estyle_style_info_reference(char *name);

/*
 * Style show and hide functions
 */
void _estyle_style_show(Estyle * es);
void _estyle_style_hide(Estyle * es);

/*
 * Style search path manipulators
 */
void _estyle_style_add_path(char *path);
void _estyle_style_remove_path(char *path);

#endif
