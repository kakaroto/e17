#ifndef _ESTYLE_PRIVATE_H
#define _ESTYLE_PRIVATE_H

#include "Estyle.h"

/*
 * The estyle holds all information necessary for display and layout of the text
 */
typedef struct _estyle Estyle;

#include "../estyle-config.h"
#include "estyle_heap.h"
#include "estyle_color.h"
#include "estyle_style.h"
#include <Edb.h>

typedef enum _estyle_bit_flags Estyle_Bit_Flags;
enum _estyle_bit_flags {
	ESTYLE_BIT_FIXED = 1,
	ESTYLE_BIT_VISIBLE = 2,
};

typedef void (*Estyle_Callback_Function) (void *_data, Evas_Object * _es,
					  void *event_info);

typedef struct _estyle_callback Estyle_Callback;
struct _estyle_callback {
	Evas_Object *obj;
	Evas_Callback_Type type;
	void *data;
	Estyle_Callback_Function callback;
};

struct _estyle {
	/*
	 * Keep a pointer in the estyle to the evas.
	 */
	Evas *evas;

	/*
	 * Each estyle can have a different color than other bits in the
	 * same estyle.
	 */
	Estyle_Color *color;

	/*
	 * Keep track of the geometry of the text to aid in layout. In the
	 * case of a tab character, the width of the bit does not match the
	 * width of the actual character printed.
	 */
	double x, y, w, h;

	/*
	 * The flags field is used to indicate that the bit is not to be moved
	 * or resized, or which default settings are being used.
	 */
	char flags;

	/*
	 * The default style for the bits in the estyle.
	 */
	Estyle_Style *style;

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
	 * Callbacks.
	 */
	Evas_List *callbacks;
};

/*
 * This rounds the double to the nearest integer and returns it as an int
 */
#define D2I_ROUND(d) (int)(d + 0.5)
#define BIT_MERGEABLE(es1, es2) (es1 && es2 && \
		!(es1->flags & ESTYLE_BIT_FIXED) && \
		!(es2->flags & ESTYLE_BIT_FIXED) && es1 != es2)

void estyle_free(Evas_Object *obj);
void estyle_show(Evas_Object *obj);
void estyle_hide(Evas_Object *obj);
void estyle_move(Evas_Object *obj, double x, double y);
void estyle_resize(Evas_Object *obj, double w, double h);
void estyle_set_layer(Evas_Object *obj, int layer);
void estyle_set_color(Evas_Object *obj, int r, int g, int b, int a);
void estyle_set_clip(Evas_Object *obj, Evas_Object *clip);
void estyle_unset_clip(Evas_Object *obj);

#endif
