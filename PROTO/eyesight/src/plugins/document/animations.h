#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Evas.h>

typedef struct _Document_Show_Anim_Data
{
   Evas *evas;
   Evas_Object *object;
   int start_x;
   int ew;
} Document_Show_Anim_Data;

typedef struct _Document_Nav_Animator_Data
{
   Evas_Object *controls;
   Evas_Object *border;
   Evas_Object *tmp_border;
   int step;
   int top_margin;
   int bottom_margin;
} Document_Nav_Animator_Data;

int show_anim(void *data);

int page_next_animator(void *_data);
int page_prev_animator(void *_data);

#endif /*ANIMATIONS_H*/
