#ifndef _ETOX_PRIVATE_H
#define _ETOX_PRIVATE_H

#include "Etox.h"
#include <Edb.h>

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
 * The bit holds all information necessary for display and layout of the text
 */
typedef struct _etox_bit Etox_Bit;
struct _etox_bit
{
	/*
	 * This is the estyle that displays the bit.
	 */
	Evas_List *estyles;

	/*
	 * Regions that reference this bit.
	 */
	Evas_List *regions;
};

void etox_layout(Etox * et);

Etox_Line * etox_coord_to_line(Etox *et, int y);
Etox_Line * etox_index_to_line(Etox *et, int *i);

#include "etox_line.h"
#include "etox_obstacle.h"

#endif
