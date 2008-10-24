#ifndef _ETOX_STYLE_PRIVATE_H
#define _ETOX_STYLE_PRIVATE_H

#include <Ecore.h>
#include <Ecore_Data.h>
#include "Etox_Style.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/*
 * The etox_style holds all information necessary for display and layout of the text
 */
typedef struct _etox_style Etox_Style;

#include "../../config.h"
#include "etox_style_heap.h"
#include "etox_style_color.h"
#include "etox_style_style.h"
#include <Edb.h>

typedef enum _etox_style_bit_flags Etox_Style_Bit_Flags;
enum _etox_style_bit_flags
{
	ETOX_STYLE_BIT_FIXED = 1,
	ETOX_STYLE_BIT_VISIBLE = 2,
};

typedef void (*Etox_Style_Callback_Function) (void *_data, Evas_Object * _es,
					  void *event_info);

typedef struct _etox_style_callback Etox_Style_Callback;
struct _etox_style_callback
{
	Evas_Object *obj;
	Evas_Callback_Type type;
	void *data;
	Etox_Style_Callback_Function callback;
};

struct _etox_style
{
	/*
	 * Keep a pointer in the etox_style to the evas.
	 */
	Evas *evas;

	/*
	 * Each etox_style can have a different color than other bits in the
	 * same etox_style.
	 */
	Etox_Style_Color *color;

	/*
	 * Keep track of the geometry of the text to aid in layout. In the
	 * case of a tab character, the width of the bit does not match the
	 * width of the actual character printed.
	 */
	Evas_Coord x, y, w, h;

	/*
	 * The flags field is used to indicate that the bit is not to be moved
	 * or resized, or which default settings are being used.
	 */
	char flags;

	/*
	 * The default style for the bits in the etox_style.
	 */
	Etox_Style_Style *style;

	/*
	 * The visual representation of the text.
	 */
	Evas_Object *bit;

	/*
	 * The smart object to tie the structure together.
	 */
	Evas_Object *smart_obj;

	/*
	 * Keep track of the length of text stored in this bit to avoid
	 * needing to recalculate this often.
	 */
	int length;

	/*
	 * Count of spaces at end of string to determine what was duplicated
	 * for sizing workaround.
	 */
	int spaces;

	/*
	 * A field for setting types
	 * (What the values mean is up to the user)
	 */
	int type;
	 
	/*
	 * Callbacks.
	 */
	Eina_List *callbacks;
};

/*
 * This rounds the double to the nearest integer and returns it as an int
 */
#define D2I_ROUND(d) (int)(d + 0.5)
#define BIT_MERGEABLE(es1, es2) (es1 && es2 && \
		!(es1->flags & ETOX_STYLE_BIT_FIXED) && \
		!(es2->flags & ETOX_STYLE_BIT_FIXED) && es1 != es2) && \
		(es1->color->a == es2->color->a) && \
		(es1->color->r == es2->color->r) && \
		(es1->color->g == es2->color->g) && \
		(es1->color->b == es2->color->b) && \
		(es1->style) && (es2->style) && \
		!(strcmp(es1->style->info->name, es2->style->info->name))

void etox_style_free(Evas_Object *obj);
void etox_style_show(Evas_Object *obj);
void etox_style_hide(Evas_Object *obj);
void etox_style_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void etox_style_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
void etox_style_set_layer(Evas_Object *obj, int layer);
void etox_style_set_color(Evas_Object *obj, int r, int g, int b, int a);
void etox_style_set_clip(Evas_Object *obj, Evas_Object *clip);
void etox_style_unset_clip(Evas_Object *obj);

#endif
