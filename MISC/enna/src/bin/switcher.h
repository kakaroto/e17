#ifndef _ENNA_SWITCHER_H
#define _ENNA_SWITCHER_H

Evas_Object *enna_switcher_add(Evas * evas);
void         enna_switcher_transition_set(Evas_Object *obj, const char *transition);
void         enna_switcher_objects_switch(Evas_Object *obj, Evas_Object * new_slide);
#endif
