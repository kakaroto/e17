#ifndef _ETOX_STYLE_COLOR_H
#define _ETOX_STYLE_COLOR_H

/*
 * The color struct simply keeps track of the various colors available
 */
typedef struct _etox_style_color Etox_Style_Color;
struct _etox_style_color {
	int a, r, g, b;
};

Etox_Style_Color *_etox_style_color_instance(int r, int g, int b, int a);
Etox_Style_Color *_etox_style_color_instance_db(char *color);
void _etox_style_color_init();

#define ETOX_STYLE_HASH_COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)


#endif
