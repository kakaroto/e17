#ifndef _ESTYLE_COLOR_H
#define _ESTYLE_COLOR_H

/*
 * The color struct simply keeps track of the various colors available
 */
typedef struct _estyle_color Estyle_Color;
struct _estyle_color {
	int a, r, g, b;
};

Estyle_Color *_estyle_color_instance(int r, int g, int b, int a);
Estyle_Color *_estyle_color_instance_db(char *color);
void _estyle_color_init();

#define ESTYLE_HASH_COLOR(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)


#endif
