#include "Engrave.h"

static Engrave_File *engrave_file = 0;
extern FILE *yyin;

Engrave_File *
engrave_parse(char *file)
{
  engrave_file = (Engrave_File *)calloc(1, sizeof(Engrave_File));

  yyin = fopen(file, "r");
  yyparse();
  fclose(yyin);

  return(engrave_file);
}

void
engrave_parse_font(char *file, char *name)
{
  Engrave_Font *font;

  font = (Engrave_Font *)calloc(1, sizeof(Engrave_Font));
  font->file = (char *)strdup(file);
  font->name = (char *)strdup(name);

  engrave_file->fonts = evas_list_append(engrave_file->fonts, font);
  return;
}

void
engrave_parse_image(char *name, Engrave_Image_Type type, double value)
{
  Engrave_Image *image;

  image = (Engrave_Image *)calloc(1, sizeof(Engrave_Image));
  image->name = (char *)strdup(name);
  image->type = type; 
  image->value = value;

  engrave_file->images = evas_list_append(engrave_file->images, image);
  return;
}

void
engrave_parse_data(char *key, char *value)
{
  Engrave_Data *data;

  data = (Engrave_Data *)calloc(1, sizeof(Engrave_Data));
  data->key = (char *)strdup(key);
  data->value = (char *)strdup(value);

  engrave_file->data = evas_list_append(engrave_file->data, data);
  return;
}

void
engrave_parse_group()
{
  Engrave_Group *group;
  group = (Engrave_Group *)calloc(1, sizeof(Engrave_Group));

  /* defaults */
  group->max.w = -1;
  group->max.h = -1;
  
  engrave_file->groups = evas_list_append(engrave_file->groups, group);
  return;
}

void
engrave_parse_group_data(char *key, char *value)
{
  Engrave_Group *group;
  Engrave_Data *data;

  group = evas_list_data(evas_list_last(engrave_file->groups));

  data = (Engrave_Data *)calloc(1, sizeof(Engrave_Data));
  data->key = (char *)strdup(key);
  data->value = (char *)strdup(value);

  engrave_file->data = evas_list_append(engrave_file->data, data);

  group = evas_list_data(evas_list_last(engrave_file->groups));

  group->data = evas_list_append(group->data, data);
}

void
engrave_parse_group_script(char *script)
{
  Engrave_Group *group;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  
  if (group->script) free(group->script);
  group->script = (char *)strdup(script); 
}

void
engrave_parse_group_name(char *name)
{
  Engrave_Group *group;

  group = evas_list_data(evas_list_last(engrave_file->groups));

  if(group->name) free(group->name);
  group->name = (char *)strdup(name);
}

void
engrave_parse_group_min(int w, int h)
{
  Engrave_Group *group;

  group = evas_list_data(evas_list_last(engrave_file->groups));

  group->min.w = w;
  group->min.h = h;
}

void
engrave_parse_group_max(int w, int h)
{
  Engrave_Group *group;

  group = evas_list_data(evas_list_last(engrave_file->groups));

  group->max.w = w;
  group->max.h = h;
}


void
engrave_parse_part()
{
  Engrave_Group *group;
  Engrave_Part *part;

  part = (Engrave_Part *)calloc(1, sizeof(Engrave_Part));
  group = evas_list_data(evas_list_last(engrave_file->groups));

  part->type = ENGRAVE_PART_TYPE_IMAGE;
  part->mouse_events = 1;
  part->repeat_events = 0;

  group->parts = evas_list_append(group->parts, part);
}

void
engrave_parse_part_name(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if(part->name) free(part->name);
  part->name = (char *)strdup(name);
}

void
engrave_parse_part_type(Engrave_Part_Type type)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->type = type;
}

void
engrave_parse_part_effect(Engrave_Text_Effect effect)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->effect = effect;
}

void
engrave_parse_part_mouse_events(int mouse_events)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->mouse_events = mouse_events;
}

void
engrave_parse_part_repeat_events(int repeat_events)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->repeat_events = repeat_events;
}

void
engrave_parse_part_clip_to(char *clip_to)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if(part->clip_to) free(part->clip_to);
  part->clip_to = (char *)strdup(clip_to);
}

void
engrave_parse_part_dragable_x(int x, int step, int count)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->dragable.x = x;
  part->dragable.step.x = step;
  part->dragable.count.x = count;
}

void
engrave_parse_part_dragable_y(int y, int step, int count)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  part->dragable.y = y;
  part->dragable.step.y = step;
  part->dragable.count.y = count;
}

void
engrave_parse_part_dragable_confine(char *confine)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));

  if (part->dragable.confine) free (part->dragable.confine);
  part->dragable.confine = (char *)strdup(confine);
}

void
engrave_parse_state()
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  state = (Engrave_Part_State *)calloc(1, sizeof(Engrave_Part_State));

  group = evas_list_data(evas_list_last(engrave_file->groups));
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
engrave_parse_state_name(char *name, double value)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if(state->name) free(state->name);
  state->name = (char *)strdup(name);
  state->value = value;
}

void
engrave_parse_state_visible(int visible)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->visible = visible;
}

void
engrave_parse_state_align(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->align.x = x;
  state->align.y = y;
}

void
engrave_parse_state_step(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->step.x = x;
  state->step.y = y;
}

void
engrave_parse_state_min(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->min.w = w;
  state->min.h = h;
}

void
engrave_parse_state_max(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->max.w = w;
  state->max.h = h;
}

void
engrave_parse_state_aspect(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->aspect.w = w;
  state->aspect.h = h;
}

void
engrave_parse_state_aspect_preference(Engrave_Aspect_Preference prefer)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->aspect.prefer = prefer;
}


void
engrave_parse_state_rel1_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel1.relative.x = x;
  state->rel1.relative.y = y;
}

void
engrave_parse_state_rel1_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel1.offset.x = x;
  state->rel1.offset.y = y;
}


void
engrave_parse_state_rel1_to_x(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_x) free(state->rel1.to_x);
  state->rel1.to_x = (char *)strdup(to);
}

void
engrave_parse_state_rel1_to_y(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_y) free(state->rel1.to_y);
  state->rel1.to_y = (char *)strdup(to);
}

void
engrave_parse_state_rel1_to(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel1.to_x) free(state->rel1.to_x);
  state->rel1.to_x = (char *)strdup(to);
  if (state->rel1.to_y) free(state->rel1.to_y);
  state->rel1.to_y = (char *)strdup(to);
}

void
engrave_parse_state_rel2_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel2.relative.x = x;
  state->rel2.relative.y = y;
}

void
engrave_parse_state_rel2_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->rel2.offset.x = x;
  state->rel2.offset.y = y;
}


void
engrave_parse_state_rel2_to_x(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_x) free(state->rel2.to_x);
  state->rel2.to_x = (char *)strdup(to);
}

void
engrave_parse_state_rel2_to_y(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_y) free(state->rel2.to_y);
  state->rel2.to_y = (char *)strdup(to);
}

void
engrave_parse_state_rel2_to(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->rel2.to_x) free(state->rel2.to_x);
  state->rel2.to_x = (char *)strdup(to);
  if (state->rel2.to_y) free(state->rel2.to_y);
  state->rel2.to_y = (char *)strdup(to);
}

void
engrave_parse_state_image_normal(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;
  Evas_List *l;
 
  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  for (l = engrave_file->images; l; l = l->next)
  {
    Engrave_Image *im = l->data;
    if (im && !strcmp(im->name, name))
    {
      state->image.normal = im;
      return;
    }
  }
  printf("Error: image \"%s\" does not exist\n", name);
}

void
engrave_parse_state_image_tween(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;
  Evas_List *l;
 
  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  for (l = engrave_file->images; l; l = l->next)
  {
    Engrave_Image *im = l->data;
    if (im && !strcmp(im->name, name))
    {
      state->image.tween = evas_list_append(state->image.tween, im);
      return;
    }
  }
  printf("Error: image \"%s\" does not exist\n", name);
}

void
engrave_parse_state_border(int l, int r, int t, int b)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->border.l = l;
  state->border.r = r;
  state->border.t = t;
  state->border.b = b;
}

void
engrave_parse_state_color_class(char *color_class)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->color_class) free(state->color_class);
  state->color_class = (char *)strdup(color_class);
}

void
engrave_parse_state_color(int r, int g, int b, int a)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color.r = r;
  state->color.g = g;
  state->color.b = b;
  state->color.a = a;
}

void
engrave_parse_state_color2(int r, int g, int b, int a)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color2.r = r;
  state->color2.g = g;
  state->color2.b = b;
  state->color2.a = a;
}

void
engrave_parse_state_color3(int r, int g, int b, int a)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->color3.r = r;
  state->color3.g = g;
  state->color3.b = b;
  state->color3.a = a;
}

/* FIXME fill and text handlers */

void
engrave_parse_state_fill_smooth(int smooth)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.smooth = smooth;
}

void
engrave_parse_state_fill_origin_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.pos_rel.x = x;
  state->fill.pos_rel.y = y;
}

void
engrave_parse_state_fill_size_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.rel.x = x;
  state->fill.rel.y = y;
}

void
engrave_parse_state_fill_origin_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.pos_abs.x = x;
  state->fill.pos_abs.y = y;
}

void
engrave_parse_state_fill_size_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->fill.abs.x = x;
  state->fill.abs.y = y;
}


void
engrave_parse_state_text_text(char *text)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.text) free(state->text.text);
  state->text.text = (char *)strdup(text);
}

void
engrave_parse_state_text_text_class(char *text_class)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.text_class) free(state->text.text_class);
  state->text.text_class = (char *)strdup(text_class);
}

void
engrave_parse_state_text_font(char *font)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  if (state->text.font) free(state->text.font);
  state->text.font = (char *)strdup(font);
}

void
engrave_parse_state_text_size(int size)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.size = size;
}

void
engrave_parse_state_text_fit(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.fit.x = x;
  state->text.fit.y = y;
}

void
engrave_parse_state_text_min(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.min.x = x;
  state->text.min.y = y;
}

void
engrave_parse_state_text_align(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  part = evas_list_data(evas_list_last(group->parts));
  state = evas_list_data(evas_list_last(part->states));

  state->text.align.x = x;
  state->text.align.y = y;
}




void
engrave_parse_program()
{
  Engrave_Group *group;
  Engrave_Program *program;

  program = (Engrave_Program *)calloc(1, sizeof(Engrave_Program));
  group = evas_list_data(evas_list_last(engrave_file->groups));

  group->programs = evas_list_append(group->programs, program);
}

void
engrave_parse_program_script(char *script)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->script) free(program->script);
  program->script = (char *)strdup(script);
  program->action = ENGRAVE_ACTION_SCRIPT;
}

void
engrave_parse_program_name(char *name)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->name) free(program->name);
  program->name = (char *)strdup(name);
}

void
engrave_parse_program_signal(char *signal)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->signal) free(program->signal);
  program->signal = (char *)strdup(signal);
}

void
engrave_parse_program_source(char *source)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  if(program->source) free(program->source);
  program->source = (char *)strdup(source);
}

void
engrave_parse_program_target(char *target)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->targets = evas_list_append(program->targets, (char *)strdup(target));
}

void
engrave_parse_program_after(char *after)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->afters = evas_list_append(program->afters, (char *)strdup(after));
}

void
engrave_parse_program_in(double from, double range)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->in.from = from;
  program->in.range = range;
}

/* handle different action types */
void
engrave_parse_program_action(Engrave_Action action, char *state, char *state2, double value, double value2)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
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
engrave_parse_program_transition(Engrave_Transition transition, double duration)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = evas_list_data(evas_list_last(engrave_file->groups));
  program = evas_list_data(evas_list_last(group->programs));

  program->transition = transition;
  program->duration = duration;
}

