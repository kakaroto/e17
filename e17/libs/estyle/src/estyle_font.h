#ifndef _ESTYLE_FONT_H
#define _ESTYLE_FONT_H

/*
 * Text layout requires knowing the font layout, size, ascent and descent.
 */
typedef struct _estyle_font Estyle_Font;
struct _estyle_font {
	char *name;
	int size, ascent, descent;
};

Estyle_Font *estyle_font_instance(char *name);

#endif
