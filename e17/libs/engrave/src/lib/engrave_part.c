#include <Engrave.h>

Engrave_Part *
engrave_part_new(Engrave_Part_Type type)
{
  Engrave_Part *part;
  part = NEW(Engrave_Part, 1);
  part->type = type;
  return part;
}

void
engrave_part_mouse_events_set(Engrave_Part *ep, int val)
{
  if (!ep) return;
  ep->mouse_events = val;
}

void
engrave_part_repeat_events_set(Engrave_Part *ep, int val)
{
  if (!ep) return;
  ep->repeat_events = val;
}

void
engrave_part_name_set(Engrave_Part *ep, char *name)
{
  if (!ep) return;
  if (ep->name) free(ep->name);
  ep->name = (name ? strdup(name) : NULL);
}

void
engrave_part_type_set(Engrave_Part *ep, Engrave_Part_Type type)
{
  if (!ep) return;
  ep->type = type;
}

void
engrave_part_effect_set(Engrave_Part *ep, Engrave_Text_Effect effect)
{
  if (!ep) return;
  ep->effect = effect;
}

void
engrave_part_clip_to_set(Engrave_Part *ep, char *clip_to)
{
  if (!ep) return;
  if (ep->clip_to) free(ep->clip_to);
  ep->clip_to = (clip_to ? strdup(clip_to) : NULL);
}

Engrave_Part_State *
engrave_part_state_last_get(Engrave_Part *ep)
{
  if (!ep) return NULL;
  return evas_list_data(evas_list_last(ep->states));
}

void
engrave_part_dragable_x_set(Engrave_Part *ep, int x, int step, int count)
{
  if (!ep) return;
  ep->dragable.x = x;
  ep->dragable.step.x = step;
  ep->dragable.count.x = count;
}

void
engrave_part_dragable_y_set(Engrave_Part *ep, int y, int step, int count)
{
  if (!ep) return;
  ep->dragable.y = y;
  ep->dragable.step.y = step;
  ep->dragable.count.y = count;
}

void
engrave_part_dragable_confine_set(Engrave_Part *ep, char *confine)
{
  if (!ep) return;
  if (ep->dragable.confine) free (ep->dragable.confine);
  ep->dragable.confine = (confine ? strdup(confine) : NULL);
}

void
engrave_part_state_add(Engrave_Part *ep, Engrave_Part_State *eps)
{
  if (!ep || !eps) return;
  ep->states = evas_list_append(ep->states, eps);
}



