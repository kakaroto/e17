#ifndef _ETOX_PRIVATE_H
#define _ETOX_PRIVATE_H

#include "Etox.h"
#include <Edb.h>

/*
 * The color struct simply keeps track of the various colors available
 */
typedef struct _etox_color Etox_Color;
struct _etox_color
{
	int a, r, g, b;
};

/*
 * Text layout requires knowing the font layout, size, ascent and descent.
 */
typedef struct _etox_font Etox_Font;
struct _etox_font
{
	char *name;
	int size, ascent, descent;
};

/*
 * The etox keeps track of the display and layout information for all of the
 * text enclosed.
 */
typedef struct _etox Etox;
struct _etox
{
	/*
	 * Evas for drawing the text
	 */
	Evas *evas;

	/*
	 * The smart object that refers to this etox.
	 */
	Evas_Object *smart_obj;

	/*
	 * Clip box on evas that bounds the text display and applies an alpha
	 * layer.
	 */
	Evas_Object *clip;

	/*
	 * The layer in the evas to set the text
	 */
	int layer;

	/*
	 * Geometry of the etox
	 */
	double x, y, w, h;

	/*
	 * Geometry the text prefers w/o wraps.
	 */
	double tw, th;

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
	Evas_List *lines;

	/*
	 * List of obstacles in the etox
	 */
	Evas_List *obstacles;

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
 * There is some subtelty to this enum. It leaves the bits used by alignment
 * untouched so that a single char can hold all of the necessary information.
 */
typedef enum _etox_line_flags Etox_Line_Flags;
enum _etox_line_flags
{
	ETOX_SOFT_WRAP = 0x10,
	ETOX_LINE_WRAPPED = 0x20,
};

/*
 * Line information helps process the bits layout
 */
typedef struct _etox_line Etox_Line;
struct _etox_line
{

	/*
	 * The etox containing this line, used for getting back to necessary
	 * etox info when drawing bits.
	 */
	Etox *et;

	/*
	 * This is a pointer to a list of bits
	 */
	Evas_List *bits;

	/*
	 * The dimensions of this line.
	 */
	double x, y, w, h;

	/*
	 * Flags indicating alignment, or if this is a "softline", ie. etox
	 * wrapped the line because it was too long to fit within the etox's
	 * bounds.
	 */
	char flags;

	/*
	 * Keep track of the length of text stored in this bit to avoid
	 * needing to recalculate this often.
	 */
	int length;
};

/*
 * Etox obstacles keep track of the lines that they intersect and the bit that
 * represents it.
 */
struct _etox_obstacle
{
	Etox *et;
	Evas_Object *bit;
	Evas_List *lines;
};

/*
 * Selection are used to manipulate previously composed etox, it is
 * recommended to keep the number of active selections to a minimum, and if
 * possible, compose using contexts and setup time.
 */
struct _etox_selection
{
	Etox *etox;

	struct
	{
		Etox_Line *line;
		Evas_Object *bit;
	} start, end;

	Etox_Context *context;
};

void etox_free(Evas_Object * et);
void etox_show(Evas_Object * et);
void etox_hide(Evas_Object * et);
void etox_move(Evas_Object * et, double x, double y);
void etox_resize(Evas_Object * et, double w, double h);
void etox_set_layer(Evas_Object * et, int layer);
void etox_set_clip(Evas_Object * et, Evas_Object *clip);
void etox_unset_clip(Evas_Object * et);

void etox_layout(Etox * et);
Evas_Object * etox_split_bit(Etox_Line *line, Evas_Object *bit, int index);

Etox_Line * etox_coord_to_line(Etox *et, int y);
Etox_Line * etox_index_to_line(Etox *et, int *i);

void etox_print_lines(Etox *et);

#include "etox_line.h"
#include "etox_obstacle.h"

#endif
