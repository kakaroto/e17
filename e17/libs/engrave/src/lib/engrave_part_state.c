#include <Engrave.h>

Engrave_Part_State *
engrave_part_state_new(void)
{
  Engrave_Part_State *state;

  state = NEW(Engrave_Part_State, 1);

  /* defaults */
  state->visible = 1;
  state->align.x = 0.5;
  state->align.y = 0.5;
  state->min.w = 0;
  state->min.h = 0;
  state->max.w = -1;
  state->max.h = -1;
  state->rel1.relative.x = 0.0;
  state->rel1.relative.y = 0.0;
  state->rel1.offset.x = 0;
  state->rel1.offset.y = 0;
  state->rel2.relative.x = 1.0;
  state->rel2.relative.y = 1.0;
  state->rel2.offset.x = -1;
  state->rel2.offset.y = -1;
  state->fill.smooth = 1;
  state->fill.pos_rel.x = 0.0;
  state->fill.pos_abs.x = 0;
  state->fill.rel.x = 1.0;
  state->fill.abs.x = 0;
  state->fill.pos_rel.y = 0.0;
  state->fill.pos_abs.y = 0;
  state->fill.rel.y = 1.0;
  state->fill.abs.y = 0;
  state->color_class = NULL;
  state->color.r = 255;
  state->color.g = 255;
  state->color.b = 255;
  state->color.a = 255;
  state->color2.r = 0;
  state->color2.g = 0;
  state->color2.b = 0;
  state->color2.a = 255;
  state->color3.r = 0;
  state->color3.g = 0;
  state->color3.b = 0;
  state->color3.a = 128;
  state->text.align.x = 0.5;
  state->text.align.y = 0.5;

  return state;
}

void
engrave_part_state_name_set(Engrave_Part_State *eps, char *name, double value)
{
  if (!eps) return;
  if (eps->name) free(eps->name);
  eps->name = (name ? strdup(name) : NULL);
  eps->value = value;
}

void
engrave_part_state_visible_set(Engrave_Part_State *eps, int visible)
{
  if (!eps) return;
  eps->visible = visible;
}

void
engrave_part_state_align_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->align.x = x;
  eps->align.y = y;
}

void
engrave_part_state_step_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->step.x = x;
  eps->step.y = y;
}

void
engrave_part_state_min_size_set(Engrave_Part_State *eps, int w, int h)
{
  if (!eps) return;
  eps->min.w = w;
  eps->min.h = h;
}

void
engrave_part_state_max_size_set(Engrave_Part_State *eps, int w, int h)
{
  if (!eps) return;
  eps->max.w = w;
  eps->max.h = h;
}

void
engrave_part_state_aspect_set(Engrave_Part_State *eps, int w, int h)
{
  if (!eps) return;
  eps->aspect.w = w;
  eps->aspect.h = h;
}

void
engrave_part_state_aspect_preference_set(Engrave_Part_State *eps,
                                    Engrave_Aspect_Preference prefer)
{
  if (!eps) return;
  eps->aspect.prefer = prefer;
}

void
engrave_part_state_rel1_relative_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->rel1.relative.x = x;
  eps->rel1.relative.y = y;
}

void
engrave_part_state_rel2_relative_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->rel2.relative.x = x;
  eps->rel2.relative.y = y;
}

void
engrave_part_state_rel1_offset_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->rel1.offset.x = x;
  eps->rel1.offset.y = y;
}

void
engrave_part_state_rel2_offset_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->rel2.offset.x = x;
  eps->rel2.offset.y = y;
}

void
engrave_part_state_rel1_to_x_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  if (eps->rel1.to_x) free(eps->rel1.to_x);
  eps->rel1.to_x = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel2_to_x_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  if (eps->rel2.to_x) free(eps->rel2.to_x);
  eps->rel2.to_x = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel1_to_y_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  if (eps->rel1.to_y) free(eps->rel1.to_y);
  eps->rel1.to_y = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel2_to_y_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  if (eps->rel2.to_y) free(eps->rel2.to_y);
  eps->rel2.to_y = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel1_to_set(Engrave_Part_State *eps, char *to)
{
  engrave_part_state_rel1_to_x_set(eps, to);
  engrave_part_state_rel1_to_y_set(eps, to);
}

void
engrave_part_state_rel2_to_set(Engrave_Part_State *eps, char *to)
{
  engrave_part_state_rel2_to_x_set(eps, to);
  engrave_part_state_rel2_to_y_set(eps, to);
}

