#ifndef ENGRAVE_CANVAS_H
#define ENGRAVE_CANVAS_H

#include <Engrave.h>

/**
 * @file engrave_canvas.h Engrave_Canvas object functions.
 * @brief Conttains all of the functions related to the engrave canvas
 * object.
 */

/**
 * @defgroup Engrave_Canvas Engrave_Canvas: Functions to work with the engrave canvas.
 *
 * @{
 */

Evas_Object *engrave_canvas_new(Evas *e);
void engrave_canvas_file_set(Evas_Object *o, Engrave_File *ef);

void engrave_canvas_current_group_set(Evas_Object *o, Engrave_Group *eg);

/**
 * @}
 */

#endif

