#ifndef _ETOX_PRIVATE_H
#define _ETOX_PRIVATE_H

#include "Etox.h"
#include <Edb.h>

/*
 * There is some subtelty to this enum. It leaves the bits used by alignment
 * untouched so that a single char can hold all of the necessary information.
 */
typedef enum _etox_line_flags Etox_Line_Flags;
enum _etox_line_flags {
	ETOX_SOFT_WRAP = 0x10,
	ETOX_LINE_WRAPPED = 0x20,
};

/*
 * Line information helps process the bits layout
 */
typedef struct _etox_line Etox_Line;
struct _etox_line {

	/*
	 * The etox containing this line, used for getting back to necessary
	 * etox info when drawing bits.
	 */
	Etox *et;

	/*
	 * This is a pointer to a list of bits
	 */
	Evas_List bits;

	/*
	 * The dimensions of this line.
	 */
	int x, y, w, h;

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
 * The bit holds all information necessary for display and layout of the text
 */
typedef struct _etox_bit Etox_Bit;
struct _etox_bit {
	/*
	 * This is the estyle that displays the bit.
	 */
	Evas_List estyles;

	/*
	 * Regions that reference this bit.
	 */
	Evas_List regions;
};

#include "etox_line.h"
#include "etox_obstacle.h"

#endif
