#ifndef _ESTYLE_H
#define _ESTYLE_H

#include <Ewd.h>
#include <Edb.h>
#include <Evas.h>

/*
 * Simple alignment bitfield
 */
typedef enum _estyle_alignment Estyle_Alignment;
enum _estyle_alignment {
	ESTYLE_ALIGN_CENTER = 0,
	ESTYLE_ALIGN_LEFT = 1,
	ESTYLE_ALIGN_RIGHT = 2,
	ESTYLE_ALIGN_TOP = 4,
	ESTYLE_ALIGN_BOTTOM = 8
};

/*
 * The bit holds all information necessary for display and layout of the text
 */
typedef struct _estyle Estyle;

Estyle *estyle_new(Evas evas, char *text, char *style);
void estyle_free(Estyle * es);
void estyle_set_text(Estyle * es, char *text);
void estyle_show(Estyle * es);
void estyle_hide(Estyle * es);
void estyle_set_layer(Estyle * es, int layer);
void estyle_set_text(Estyle * es, char *text);
void estyle_merge(Estyle * es1, Estyle * es2);
void estyle_set_style(Estyle * es, char *name);
void estyle_set_color(Estyle * es, int r, int g, int b, int a);
void estyle_set_color_db(Estyle * es, char *name);
void estyle_geometry(Estyle *es, int *x, int *y, int *w, int *h);
void estyle_text_at(Estyle *es, int index, int *char_x, int *char_y,
		int *char_w, int *char_h);
int estyle_text_at_position(Estyle *es, int x, int y, int *char_x, int *char_y,
		int *char_w, int *char_h);

#define BIT_MERGEABLE(es1, es2) (es1 && es2 && \
		!(es1->flags & ESTYLE_BIT_FIXED) && \
		!(es2->flags & ESTYLE_BIT_FIXED) && es1 != es2)

#endif
