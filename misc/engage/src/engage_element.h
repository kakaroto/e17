#ifndef ENGAGE_ELEMENT_H
#define ENGAGE_ELEMENT_H

typedef struct _Engage_Element Engage_Element;

struct _Engage_Element
{
    Evas_Object *clip;
    Evas_Object *icon;
    Evas_Object *behavior;
    Evas_Coord x, y, w, h;
};

#endif
