#ifndef _ETOX_PRIVATE_H
#define _ETOX_PRIVATE_H

#include <Ecore.h>
#include "Etox.h"
#include <Edb.h>
#include "style/Etox_Style.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define IF_FREE(ptr) if (ptr) free(ptr); ptr = NULL;
#define FREE(ptr) free(ptr); ptr = NULL;

#define CHECK_PARAM_POINTER_RETURN(sparam, param, ret) \
     if (!(param)) \
	 { \
	    fprintf(stderr, "Fix: func: %s, param: %s\n", __FUNCTION__, sparam); \
	    return ret; \
	 }

#define CHECK_PARAM_POINTER(sparam, param) \
     if (!(param)) \
	 { \
	    fprintf(stderr, "Fix: func: %s, param: %s\n", __FUNCTION__, sparam); \
	    return; \
	 }


typedef enum _etox_flags Etox_Flags;
enum _etox_flags
{
	ETOX_SOFT_WRAP = 0x1,
	ETOX_BREAK_WORDS = 0x2
};


/*
 * Eventually, where to place the wrap marker (if ETOX_SOFT_WRAP is set)
 * (at start or end of line, and inside or outside etox bounds)
 */
typedef enum _etox_marker_position Etox_Marker_Position;
enum _etox_marker_position
{
	ETOX_MARKER_POSITION_FIRST = 0x10,
	ETOX_MARKER_POSITION_LAST = 0x20,
	ETOX_MARKER_POSITION_INSIDE = 0x40,
	ETOX_MARKER_POSITION_OUTSIDE = 0x80
};

/*
 * There is some subtelty to this enum. It leaves the bits used by alignment
 * untouched so that a single char can hold all of the necessary information.
 */
typedef enum _etox_line_flags Etox_Line_Flags;
enum _etox_line_flags
{
	ETOX_LINE_WRAPPED = 0x10
};


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
	Evas_Coord x, y, w, h;

	/*
	 * Geometry the text prefers w/o wraps.
	 */
	Evas_Coord tw, th;

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
	Eina_List *lines;

	/*
	 * List of obstacles in the etox
	 */
	Eina_List *obstacles;

	/*
	 * Whether to wrap the etox or not.
	 */
	char flags;

	/*
	 * Alpha level of clip box that is applied to the text
	 */
	int alpha;
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
	Eina_List *bits;

	/*
	 * The dimensions of this line.
	 */
	Evas_Coord x, y, w, h;

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
	Eina_List *lines;
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
};

void etox_layout(Etox * et);
Evas_Object * etox_split_bit(Etox_Line *line, Evas_Object *bit, int index);

Etox_Line * etox_coord_to_line(Etox *et, int y);
Etox_Line * etox_index_to_line(Etox *et, int *i);

void etox_print_lines(Etox *et);
void etox_selections_update(Evas_Object *bit, Etox_Line *line);

#include "etox_line.h"
#include "etox_obstacle.h"

#endif
