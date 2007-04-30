#ifndef _PYTHON_EVAS_UTILS_H_
#define _PYTHON_EVAS_UTILS_H_

#include <Evas.h>

typedef void (*evas_event_callback_t)(void *data, Evas *e, Evas_Object *obj, void *event_info);

typedef struct
{
    int x;
    int y;
} Evas_Point;

typedef struct
{
    Evas_Point output;
    Evas_Point canvas;
} Evas_Position;

#endif /* _PYTHON_EVAS_UTILS_H_ */
