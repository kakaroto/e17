#include <Engrave.h>

Engrave_Part_State *
engrave_part_state_new(void)
{
  Engrave_Part_State *state;

  state = NEW(Engrave_Part_State, 1);
  if (!state) return NULL;

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
  IF_FREE(eps->name);
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
  IF_FREE(eps->rel1.to_x);
  eps->rel1.to_x = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel2_to_x_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  IF_FREE(eps->rel2.to_x);
  eps->rel2.to_x = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel1_to_y_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  IF_FREE(eps->rel1.to_y);
  eps->rel1.to_y = (to ? strdup(to) : NULL);
}

void
engrave_part_state_rel2_to_y_set(Engrave_Part_State *eps, char *to)
{
  if (!eps) return;
  IF_FREE(eps->rel2.to_y);
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

void
engrave_part_state_image_normal_set(Engrave_Part_State *eps, 
                                      Engrave_Image *im)
{
  if (!eps) return;
  eps->image.normal = im;
}

void
engrave_part_state_image_tween_add(Engrave_Part_State *eps,
                                      Engrave_Image *im)
{
  if (!eps || !im) return;
  eps->image.tween = evas_list_append(eps->image.tween, im);
}

void
engrave_part_state_border_set(Engrave_Part_State *eps, 
                                int l, int r, int t, int b)
{
  if (!eps) return;
  eps->border.l = l;
  eps->border.r = r;
  eps->border.t = t;
  eps->border.b = b;
}

void
engrave_part_state_color_class_set(Engrave_Part_State *eps, char *class)
{
  if (!eps) return;
  IF_FREE(eps->color_class);
  eps->color_class = (class ? strdup(class) : NULL);
}

void
engrave_part_state_color_set(Engrave_Part_State *eps, 
                                int r, int g, int b, int a)
{
  if (!eps) return;
  eps->color.r = r;
  eps->color.g = g;
  eps->color.b = b;
  eps->color.a = a;
}

void
engrave_part_state_color2_set(Engrave_Part_State *eps, 
                                int r, int g, int b, int a)
{
  if (!eps) return;
  eps->color2.r = r;
  eps->color2.g = g;
  eps->color2.b = b;
  eps->color2.a = a;
}

void
engrave_part_state_color3_set(Engrave_Part_State *eps, 
                                int r, int g, int b, int a)
{
  if (!eps) return;
  eps->color3.r = r;
  eps->color3.g = g;
  eps->color3.b = b;
  eps->color3.a = a;
}

void
engrave_part_state_fill_smooth_set(Engrave_Part_State *eps, int smooth)
{
  if (!eps) return;
  eps->fill.smooth = smooth;
}

void
engrave_part_state_fill_origin_relative_set(Engrave_Part_State *eps,
                                                double x, double y)
{
  if (!eps) return;
  eps->fill.pos_rel.x = x;
  eps->fill.pos_rel.y = y;
}

void
engrave_part_state_fill_size_relative_set(Engrave_Part_State *eps,
                                                double x, double y)
{
  if (!eps) return;
  eps->fill.rel.x = x;
  eps->fill.rel.y = y;
}

void
engrave_part_state_fill_origin_offset_set(Engrave_Part_State *eps,
                                                      int x, int y)
{
  if (!eps) return;
  eps->fill.pos_abs.x = x;
  eps->fill.pos_abs.y = y;
}

void
engrave_part_state_fill_size_offset_set(Engrave_Part_State *eps,
                                                int x, int y)
{
  if (!eps) return;
  eps->fill.abs.x = x;
  eps->fill.abs.y = y;
}

void
engrave_part_state_text_text_set(Engrave_Part_State *eps, char *text)
{
  if (!eps) return;
  IF_FREE(eps->text.text);
  eps->text.text = (text ? strdup(text) : NULL); 
}

void
engrave_part_state_text_text_class_set(Engrave_Part_State *eps,
                                                char *text_class)
{
  if (!eps) return;
  IF_FREE(eps->text.text_class);
  eps->text.text_class = (text_class ? strdup(text_class) : NULL); 
}

void
engrave_part_state_text_font_set(Engrave_Part_State *eps,
                                                char *font)
{
  if (!eps) return;
  IF_FREE(eps->text.font);
  eps->text.font = (font ? strdup(font) : NULL); 
}

void
engrave_part_state_text_size_set(Engrave_Part_State *eps, int size)
{
  if (!eps) return;
  eps->text.size = size;
}

void
engrave_part_state_text_fit_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->text.fit.x = x;
  eps->text.fit.y = y;
}

void
engrave_part_state_text_min_set(Engrave_Part_State *eps, int x, int y)
{
  if (!eps) return;
  eps->text.min.x = x;
  eps->text.min.y = y;
}

void
engrave_part_state_text_align_set(Engrave_Part_State *eps, double x, double y)
{
  if (!eps) return;
  eps->text.align.x = x;
  eps->text.align.y = y;
}


