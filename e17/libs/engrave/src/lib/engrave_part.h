#ifndef ENGRAVE_PART_H
#define ENGRAVE_PART_H

typedef struct _Engrave_Part Engrave_Part;
struct _Engrave_Part
{
  char *name;
  Engrave_Part_Type type;
  Engrave_Text_Effect effect;
  int mouse_events;
  int repeat_events;
  char *clip_to;

  struct
  {
    signed char x, y; /* can drag in x/y, and which dir to count in */
    struct
    {
      int x, y;
    } step, count; 
    char *confine;

  } dragable;

  Evas_List *states;
};

Engrave_Part * engrave_part_new(Engrave_Part_Type type);
void engrave_part_name_set(Engrave_Part *ep, char *name);
void engrave_part_mouse_events_set(Engrave_Part *ep, int val);
void engrave_part_repeat_events_set(Engrave_Part *ep, int val);
void engrave_part_type_set(Engrave_Part *ep, Engrave_Part_Type type);
void engrave_part_effect_set(Engrave_Part *ep, Engrave_Text_Effect effect);
void engrave_part_clip_to_set(Engrave_Part *ep, char *clip_to);

Engrave_Part_State *engrave_part_state_last_get(Engrave_Part *ep);

void engrave_part_dragable_x_set(Engrave_Part *ep, int x, int step, int count);
void engrave_part_dragable_y_set(Engrave_Part *ep, int y, int step, int count);
void engrave_part_dragable_confine_set(Engrave_Part *ep, char *confine);
void engrave_part_state_add(Engrave_Part *ep, Engrave_Part_State *eps);

#endif

