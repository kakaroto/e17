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
 * Style constructors and destructors
 */
Estyle_Style *estyle_style_instance(char *name);
void estyle_style_release(Estyle_Style * style, Evas ev);

/*
 * Style display modifiers.
 */
void estyle_style_draw(Estyle * et, char *text);
void estyle_style_move(Estyle * bit);
void estyle_style_set_clip(Estyle * bit, Evas_Object ob);
void estyle_style_set_color(Estyle * bit);
void estyle_style_set_font(Estyle *es);
void estyle_style_set_text(Estyle *es);
int estyle_style_set_layer_lower(Estyle * bit, int layer);
int estyle_style_set_layer_upper(Estyle * bit, int layer, int start);

/*
 * Style info access functions
 */
void estyle_style_info_dereference(Estyle_Style_Info *info);
Estyle_Style_Info *estyle_style_info_reference(char *name);

/*
 * Style show and hide functions
 */
void estyle_style_show(Estyle *es);
void estyle_style_hide(Estyle *es);

/*
 * Style search path manipulators
 */
void estyle_style_add_path(char *path);
void estyle_style_remove_path(char *path);

#endif
