#include "Etcher.h"
#include <stdlib.h>

Etcher_File *etcher_file = 0;

void
etcher_parse_init()
{
  etcher_file = (Etcher_File *)calloc(1, sizeof(Etcher_File));
  return;
}

void
etcher_parse_font(char *file, char *name)
{
  Etcher_Font *font;

  font = (Etcher_Font *)calloc(1, sizeof(Etcher_Font));
  font->file = (char *)strdup(file);
  font->name = (char *)strdup(name);

  etcher_file->fonts = evas_list_append(etcher_file->fonts, font);
  return;
}

void
etcher_parse_image(char *name, Etcher_Image_Type type, double value)
{
  Etcher_Image *image;

  image = (Etcher_Image *)calloc(1, sizeof(Etcher_Image));
  image->name = (char *)strdup(name);
  image->type = type; 
  image->value = value;

  etcher_file->images = evas_list_append(etcher_file->images, image);
  return;
}

void
etcher_parse_data(char *key, char *value, int val)
{
  Etcher_Data *data;

  data = (Etcher_Data *)calloc(1, sizeof(Etcher_Data));
  data->key = (char *)strdup(key);

  if (value)
    data->value = (char *)strdup(value);
  else {
    data->value = NULL;
	data->int_value = val;
  }

  etcher_file->data = evas_list_append(etcher_file->data, data);
  return;
}

void
etcher_parse_group()
{
  Etcher_Group *group;
  group = (Etcher_Group *)calloc(1, sizeof(Etcher_Group));

  /* defaults */
  group->max.w = -1;
  group->max.h = -1;
  
  etcher_file->groups = evas_list_append(etcher_file->groups, group);
  return;
}

void
etcher_parse_group_data(char *key, char *value, int val)
{
  Etcher_Group *group;
  Etcher_Data *data;

  group = evas_list_data(evas_list_last(etcher_file->groups));

  data = (Etcher_Data *)calloc(1, sizeof(Etcher_Data));
  data->key = (char *)strdup(key);

  if (value)
    data->value = (char *)strdup(value);
  else {
    data->value = NULL;
	data->int_value = val;
  }

  etcher_file->data = evas_list_append(etcher_file->data, data);

  group = evas_list_data(evas_list_last(etcher_file->groups));

  group->data = evas_list_append(group->data, data);
}

void
etcher_parse_group_script(char *script)
{
  Etcher_Group *group;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  
  if (group->script) free(group->script);
  group->script = (char *)strdup(script); 
}

void
etcher_parse_group_name(char *name)
{
  Etcher_Group *group;

  group = evas_list_data(evas_list_last(etcher_file->groups));

  if(group->name) free(group->name);
  group->name = (char *)strdup(name);
}

void
etcher_parse_group_min(int w, int h)
{
  Etcher_Group *group;

  group = evas_list_data(evas_list_last(etcher_file->groups));

  group->min.w = w;
  group->min.h = h;
}

void
etcher_parse_group_max(int w, int h)
{
  Etcher_Group *group;

  group = evas_list_data(evas_list_last(etcher_file->groups));

  group->max.w = w;
  group->max.h = h;
}


void
etcher_parse_part()
{
  Etcher_Group *group;
  Etcher_Part *part;

  part = (Etcher_Part *)calloc(1, sizeof(Etcher_Part));
  group = evas_list_data(evas_list_last(etcher_file->groups));

  part->type = ETCHER_PART_TYPE_IMAGE;
  part->mouse_events = 1;
  part->repeat_events = 0;

  group->parts = evas_list_append(group->parts, part);
}

void
etcher_parse_part_name(char *name)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if(part->name) free(part->name);
  part->name = (char *)strdup(name);
}

void
etcher_parse_part_type(Etcher_Part_Type type)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->type = type;
}

void
etcher_parse_part_effect(Etcher_Text_Effect effect)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->effect = effect;
}

void
etcher_parse_part_mouse_events(int mouse_events)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->mouse_events = mouse_events;
}

void
etcher_parse_part_repeat_events(int repeat_events)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->repeat_events = repeat_events;
}

void
etcher_parse_part_clip_to(char *clip_to)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if(part->clip_to) free(part->clip_to);
  part->clip_to = (char *)strdup(clip_to);
}

void
etcher_parse_part_dragable_x(int x, int step, int count)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->dragable.x = x;
  part->dragable.step.x = step;
  part->dragable.count.x = count;
}

void
etcher_parse_part_dragable_y(int y, int step, int count)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->dragable.y = y;
  part->dragable.step.y = step;
  part->dragable.count.y = count;
}

void
etcher_parse_part_dragable_confine(char *confine)
{
  Etcher_Group *group;
  Etcher_Part *part;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if (part->dragable.confine) free (part->dragable.confine);
  part->dragable.confine = (char *)strdup(confine);
}

void
etcher_parse_state()
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  state = (Etcher_Part_State *)calloc(1, sizeof(Etcher_Part_State));

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

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

  part->states = evas_list_append(part->states, state);
}

void
etcher_parse_state_name(char *name, double value)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if(state->name) free(state->name);
  state->name = (char *)strdup(name);
  state->value = value;
}

void
etcher_parse_state_visible(int visible)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->visible = visible;
}

void
etcher_parse_state_align(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->align.x = x;
  state->align.y = y;
}

void
etcher_parse_state_step(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->step.x = x;
  state->step.y = y;
}

void
etcher_parse_state_min(double w, double h)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->min.w = w;
  state->min.h = h;
}

void
etcher_parse_state_max(double w, double h)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->max.w = w;
  state->max.h = h;
}

void
etcher_parse_state_aspect(double w, double h)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->aspect.w = w;
  state->aspect.h = h;
}

void
etcher_parse_state_aspect_preference(Etcher_Aspect_Preference prefer)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->aspect.prefer = prefer;
}


void
etcher_parse_state_rel1_relative(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel1.relative.x = x;
  state->rel1.relative.y = y;
}

void
etcher_parse_state_rel1_offset(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel1.offset.x = x;
  state->rel1.offset.y = y;
}


void
etcher_parse_state_rel1_to_x(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_x) free(state->rel1.to_x);
  state->rel1.to_x = (char *)strdup(to);
}

void
etcher_parse_state_rel1_to_y(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_y) free(state->rel1.to_y);
  state->rel1.to_y = (char *)strdup(to);
}

void
etcher_parse_state_rel1_to(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_x) free(state->rel1.to_x);
  state->rel1.to_x = (char *)strdup(to);
  if (state->rel1.to_y) free(state->rel1.to_y);
  state->rel1.to_y = (char *)strdup(to);
}

void
etcher_parse_state_rel2_relative(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel2.relative.x = x;
  state->rel2.relative.y = y;
}

void
etcher_parse_state_rel2_offset(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel2.offset.x = x;
  state->rel2.offset.y = y;
}


void
etcher_parse_state_rel2_to_x(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_x) free(state->rel2.to_x);
  state->rel2.to_x = (char *)strdup(to);
}

void
etcher_parse_state_rel2_to_y(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_y) free(state->rel2.to_y);
  state->rel2.to_y = (char *)strdup(to);
}

void
etcher_parse_state_rel2_to(char *to)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_x) free(state->rel2.to_x);
  state->rel2.to_x = (char *)strdup(to);
  if (state->rel2.to_y) free(state->rel2.to_y);
  state->rel2.to_y = (char *)strdup(to);
}

void
etcher_parse_state_image_normal(char *name)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;
  Evas_List *l;
 
  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  for (l = etcher_file->images; l; l = l->next)
  {
    Etcher_Image *im = l->data;
    if (im && !strcmp(im->name, name))
    {
      state->image.normal = im;
      return;
    }
  }
  printf("Error: image \"%s\" does not exist\n", name);
}

void
etcher_parse_state_image_tween(char *name)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;
  Evas_List *l;
 
  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  for (l = etcher_file->images; l; l = l->next)
  {
    Etcher_Image *im = l->data;
    if (im && !strcmp(im->name, name))
    {
      state->image.tween = evas_list_append(state->image.tween, im);
      return;
    }
  }
  printf("Error: image \"%s\" does not exist\n", name);
}

void
etcher_parse_state_border(int l, int r, int t, int b)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->border.l = l;
  state->border.r = r;
  state->border.t = t;
  state->border.b = b;
}

void
etcher_parse_state_color_class(char *color_class)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->color_class) free(state->color_class);
  state->color_class = (char *)strdup(color_class);
}

void
etcher_parse_state_color(int r, int g, int b, int a)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color.r = r;
  state->color.g = g;
  state->color.b = b;
  state->color.a = a;
}

void
etcher_parse_state_color2(int r, int g, int b, int a)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color2.r = r;
  state->color2.g = g;
  state->color2.b = b;
  state->color2.a = a;
}

void
etcher_parse_state_color3(int r, int g, int b, int a)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color3.r = r;
  state->color3.g = g;
  state->color3.b = b;
  state->color3.a = a;
}

/* FIXME fill and text handlers */

void
etcher_parse_state_fill_smooth(int smooth)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.smooth = smooth;
}

void
etcher_parse_state_fill_origin_relative(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.pos_rel.x = x;
  state->fill.pos_rel.y = y;
}

void
etcher_parse_state_fill_size_relative(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.rel.x = x;
  state->fill.rel.y = y;
}

void
etcher_parse_state_fill_origin_offset(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.pos_abs.x = x;
  state->fill.pos_abs.y = y;
}

void
etcher_parse_state_fill_size_offset(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.abs.x = x;
  state->fill.abs.y = y;
}


void
etcher_parse_state_text_text(char *text)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.text) free(state->text.text);
  state->text.text = (char *)strdup(text);
}

void
etcher_parse_state_text_text_class(char *text_class)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.text_class) free(state->text.text_class);
  state->text.text_class = (char *)strdup(text_class);
}

void
etcher_parse_state_text_font(char *font)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.font) free(state->text.font);
  state->text.font = (char *)strdup(font);
}

void
etcher_parse_state_text_size(int size)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.size = size;
}

void
etcher_parse_state_text_fit(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.fit.x = x;
  state->text.fit.y = y;
}

void
etcher_parse_state_text_min(int x, int y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.min.x = x;
  state->text.min.y = y;
}

void
etcher_parse_state_text_align(double x, double y)
{
  Etcher_Group *group;
  Etcher_Part *part;
  Etcher_Part_State *state;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.align.x = x;
  state->text.align.y = y;
}




void
etcher_parse_program()
{
  Etcher_Group *group;
  Etcher_Program *program;

  program = (Etcher_Program *)calloc(1, sizeof(Etcher_Program));
  group = evas_list_data(evas_list_last(etcher_file->groups));

  group->programs = evas_list_append(group->programs, program);
}

void
etcher_parse_program_script(char *script)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->script) free(program->script);
  program->script = (char *)strdup(script);
  program->action = ETCHER_ACTION_SCRIPT;
}

void
etcher_parse_program_name(char *name)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->name) free(program->name);
  program->name = (char *)strdup(name);
}

void
etcher_parse_program_signal(char *signal)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->signal) free(program->signal);
  program->signal = (char *)strdup(signal);
}

void
etcher_parse_program_source(char *source)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->source) free(program->source);
  program->source = (char *)strdup(source);
}

void
etcher_parse_program_target(char *target)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->targets = evas_list_append(program->targets, (char *)strdup(target));
}

void
etcher_parse_program_after(char *after)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->afters = evas_list_append(program->afters, (char *)strdup(after));
}

void
etcher_parse_program_in(double from, double range)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->in.from = from;
  program->in.range = range;
}

/* handle different action types */
void
etcher_parse_program_action(Etcher_Action action, char *state, char *state2, double value, double value2)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));


  if (program->state) free(program->state);
  if (program->state2) free(program->state2);

  program->action = action;
  if (state) program->state = (char *)strdup(state);
  if (state2) program->state2 = (char *)strdup(state2);
  program->value = value;
  program->value2 = value2;
}

void
etcher_parse_program_transition(Etcher_Transition transition, double duration)
{
  Etcher_Group *group;
  Etcher_Program *program;

  group = evas_list_data(evas_list_last(etcher_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->transition = transition;
  program->duration = duration;
}

