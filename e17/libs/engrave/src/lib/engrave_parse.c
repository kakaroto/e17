#include "Engrave.h"

static Engrave_File *engrave_file = 0;
extern FILE *yyin;

Engrave_File *
engrave_parse(char *file)
{
  engrave_file = engrave_file_new();

  yyin = fopen(file, "r");
  yyparse();
  fclose(yyin);

  return (engrave_file);
}

void
engrave_parse_font(char *file, char *name)
{
  Engrave_Font *font;
  font = engrave_font_new(file, name);
  engrave_file_font_add(engrave_file, font);
}

void
engrave_parse_image(char *name, Engrave_Image_Type type, double value)
{
  Engrave_Image *image;
  image = engrave_image_new(name, type, value);
  engrave_file_image_add(engrave_file, image);
}

void
engrave_parse_data(char *key, char *value)
{
  Engrave_Data *data;
  data = engrave_data_new(key, value);
  engrave_file_data_add(engrave_file, data);
}

void
engrave_parse_group()
{
  Engrave_Group *group;
  group = engrave_group_new();
  engrave_file_group_add(engrave_file, group);
}

void
engrave_parse_group_data(char *key, char *value)
{
  Engrave_Group *group;
  Engrave_Data *data;
 
  /* XXX why is this put inboth file and group data? */
  data = engrave_data_new(key, value);
  engrave_file_data_add(engrave_file, data);

  group = engrave_file_group_last_get(engrave_file);
  engrave_group_data_add(group, data);
}

void
engrave_parse_group_script(char *script)
{
  Engrave_Group *group;
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_script_set(group, script); 
}

void
engrave_parse_group_name(char *name)
{
  Engrave_Group *group;
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_name_set(group, name);
}

void
engrave_parse_group_min(int w, int h)
{
  Engrave_Group *group;
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_min_size_set(group, w, h);
}

void
engrave_parse_group_max(int w, int h)
{
  Engrave_Group *group;
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_max_size_set(group, w, h);
}

void
engrave_parse_part()
{
  Engrave_Group *group;
  Engrave_Part *part;

  part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
  engrave_part_mouse_events_set(part, 1);
  engrave_part_repeat_events_set(part, 0);
 
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_part_add(group, part);
}

void
engrave_parse_part_name(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_name_set(part, name);
}

void
engrave_parse_part_type(Engrave_Part_Type type)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_type_set(part, type);
}

void
engrave_parse_part_effect(Engrave_Text_Effect effect)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_effect_set(part, effect);
}

void
engrave_parse_part_mouse_events(int mouse_events)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_mouse_events_set(part, mouse_events);
}

void
engrave_parse_part_repeat_events(int repeat_events)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_repeat_events_set(part, repeat_events);
}

void
engrave_parse_part_clip_to(char *clip_to)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_clip_to_set(part, clip_to);
}

void
engrave_parse_part_dragable_x(int x, int step, int count)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_dragable_x_set(part, x, step, count);
}

void
engrave_parse_part_dragable_y(int y, int step, int count)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_dragable_y_set(part, y, step, count);
}

void
engrave_parse_part_dragable_confine(char *confine)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_dragable_confine_set(part, confine);
}

void
engrave_parse_state()
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  state = engrave_part_state_new();

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_state_add(part, state);
}

void
engrave_parse_state_name(char *name, double value)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_name_set(state, name, value);
}

void
engrave_parse_state_visible(int visible)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_visible_set(state, visible);
}

void
engrave_parse_state_align(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_align_set(state, x, y);
}

void
engrave_parse_state_step(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_step_set(state, x, y);
}

void
engrave_parse_state_min(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_min_size_set(state, w, h);
}

void
engrave_parse_state_max(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_min_size_set(state, w, h);
}

void
engrave_parse_state_aspect(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_aspect_set(state, w, h);
}

void
engrave_parse_state_aspect_preference(Engrave_Aspect_Preference prefer)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_aspect_preference_set(state, prefer);
}

void
engrave_parse_state_rel1_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel1_relative_set(state, x, y);
}

void
engrave_parse_state_rel1_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel1_offset_set(state, x, y);
}

void
engrave_parse_state_rel1_to_x(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel1_to_x_set(state, to);
}

void
engrave_parse_state_rel1_to_y(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel1_to_y_set(state, to);
}

void
engrave_parse_state_rel1_to(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel1_to_set(state, to);
}

void
engrave_parse_state_rel2_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel2_relative_set(state, x, y);
}

void
engrave_parse_state_rel2_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel2_offset_set(state, x, y);
}

void
engrave_parse_state_rel2_to_x(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel2_to_x_set(state, to);
}

void
engrave_parse_state_rel2_to_y(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel2_to_y_set(state, to);
}

void
engrave_parse_state_rel2_to(char *to)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_rel2_to_set(state, to);
}

void
engrave_parse_state_image_normal(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;
  Evas_List *l;
 
  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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
 
  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  if (state->color_class) free(state->color_class);
  state->color_class = (char *)strdup(color_class);
}

void
engrave_parse_state_color(int r, int g, int b, int a)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

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

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->fill.smooth = smooth;
}

void
engrave_parse_state_fill_origin_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->fill.pos_rel.x = x;
  state->fill.pos_rel.y = y;
}

void
engrave_parse_state_fill_size_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->fill.rel.x = x;
  state->fill.rel.y = y;
}

void
engrave_parse_state_fill_origin_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->fill.pos_abs.x = x;
  state->fill.pos_abs.y = y;
}

void
engrave_parse_state_fill_size_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->fill.abs.x = x;
  state->fill.abs.y = y;
}


void
engrave_parse_state_text_text(char *text)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  if (state->text.text) free(state->text.text);
  state->text.text = (char *)strdup(text);
}

void
engrave_parse_state_text_text_class(char *text_class)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  if (state->text.text_class) free(state->text.text_class);
  state->text.text_class = (char *)strdup(text_class);
}

void
engrave_parse_state_text_font(char *font)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  if (state->text.font) free(state->text.font);
  state->text.font = (char *)strdup(font);
}

void
engrave_parse_state_text_size(int size)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->text.size = size;
}

void
engrave_parse_state_text_fit(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->text.fit.x = x;
  state->text.fit.y = y;
}

void
engrave_parse_state_text_min(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->text.min.x = x;
  state->text.min.y = y;
}

void
engrave_parse_state_text_align(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  state->text.align.x = x;
  state->text.align.y = y;
}

void
engrave_parse_program()
{
  Engrave_Group *group;
  Engrave_Program *program;

  program = (Engrave_Program *)calloc(1, sizeof(Engrave_Program));
  group = engrave_file_group_last_get(engrave_file);

  group->programs = evas_list_append(group->programs, program);
}

void
engrave_parse_program_script(char *script)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  if(program->script) free(program->script);
  program->script = (char *)strdup(script);
  program->action = ENGRAVE_ACTION_SCRIPT;
}

void
engrave_parse_program_name(char *name)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  if(program->name) free(program->name);
  program->name = (char *)strdup(name);
}

void
engrave_parse_program_signal(char *signal)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  if(program->signal) free(program->signal);
  program->signal = (char *)strdup(signal);
}

void
engrave_parse_program_source(char *source)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  if(program->source) free(program->source);
  program->source = (char *)strdup(source);
}

void
engrave_parse_program_target(char *target)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  program->targets = evas_list_append(program->targets, (char *)strdup(target));
}

void
engrave_parse_program_after(char *after)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  program->afters = evas_list_append(program->afters, (char *)strdup(after));
}

void
engrave_parse_program_in(double from, double range)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  program->in.from = from;
  program->in.range = range;
}

/* handle different action types */
void
engrave_parse_program_action(Engrave_Action action, char *state, char *state2, double value, double value2)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

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

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);

  program->transition = transition;
  program->duration = duration;
}

