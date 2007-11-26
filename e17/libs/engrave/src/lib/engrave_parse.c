#include "engrave_private.h"
#include <locale.h>
#include <Engrave.h>

static Engrave_File *engrave_file = 0;
extern FILE *yyin;
int yyparse(void);

Engrave_File *
engrave_parse(const char *file, const char *imdir, const char *fontdir)
{
  int ret;
  char locale[128];
  engrave_file = engrave_file_new();
  engrave_file_image_dir_set(engrave_file, imdir);
  engrave_file_font_dir_set(engrave_file, fontdir);

  /* set locale posix compliant*/
  strncpy(locale, setlocale(LC_NUMERIC, NULL), sizeof(locale));
  setlocale(LC_NUMERIC, "C");
   
  yyin = fopen(file, "r");
  ret = yyparse();
  fclose(yyin);

  /* reset locale to system default*/
  setlocale(LC_NUMERIC, locale);
   
  if (ret == 0)
     return (engrave_file);
  
  return (NULL);
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
engrave_parse_data(char *key, char *value, int int_val)
{
  Engrave_Data *data;
  data = engrave_data_new(key, value);

  if (!value)
      engrave_data_int_value_set(data, int_val);

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
engrave_parse_group_data(char *key, char *value, int int_value)
{
  Engrave_Group *group;
  Engrave_Data *data;
 
  data = engrave_data_new(key, value);
  if (!value)
      engrave_data_int_value_set(data, int_value);

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
engrave_parse_color_class_name(char *name)
{
  if (!name) return;
  Engrave_Color_Class * ecc = engrave_file_color_class_last_get(engrave_file);
  engrave_color_class_name_set(ecc, name);
}

void 
engrave_parse_color_class_color(int num, int r, int g, int b, int a)
{
  Engrave_Color_Class * ecc = engrave_file_color_class_last_get(engrave_file);
  Engrave_Color_Class_Color * ec = engrave_color_class_color_new(r, g, b, a);
  engrave_color_class_color_set(ecc, ec, num);
}

void
engrave_parse_group_alias(char *alias)
{
  Engrave_Group *group;
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_alias_set(group, alias);
}

Engrave_Color_Class *
engrave_parse_file_color_class()
{
  Engrave_Color_Class *ecc;
  ecc = engrave_color_class_new();
  engrave_file_color_class_add(engrave_file, ecc);
  engrave_color_class_parent_set(ecc, engrave_file);
  return ecc;
}

void
engrave_parse_spectrum()
{
  Engrave_Spectrum *es;
  es = engrave_spectrum_new();
  engrave_file_spectrum_add(engrave_file, es);
  engrave_spectrum_parent_set(es, engrave_file);
}

void
engrave_parse_spectrum_name(char *name)
{
  Engrave_Spectrum *es;
  es = engrave_file_spectrum_last_get(engrave_file);
  engrave_spectrum_name_set(es, name);
}

void
engrave_parse_spectrum_color(int r, int g, int b, int a, int d)
{
  Engrave_Spectrum *es;
  Engrave_Spectrum_Color *esc;

  esc = engrave_spectrum_color_new(r, g, b, a, d);
  es = engrave_file_spectrum_last_get(engrave_file);
  
  engrave_spectrum_color_add(es, esc);
  
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
engrave_parse_style()
{
  Engrave_Style *style;

  style = engrave_style_new();
  engrave_file_style_add(engrave_file, style);
}

void
engrave_parse_base(char *base)
{
  Engrave_Style *style;

  style = engrave_file_style_last_get(engrave_file);
  engrave_style_base_set(style, base);
}



void
engrave_parse_style_name(char *name)
{
  Engrave_Style *style;

  style = engrave_file_style_last_get(engrave_file);
  engrave_style_name_set(style, name);
}

void
engrave_parse_style_base(char *base)
{
  Engrave_Style *style;

  style = engrave_file_style_last_get(engrave_file);
  engrave_style_base_set(style, base);
}

void
engrave_parse_style_tag(char *key, char *val)
{
  Engrave_Style *style;
  Engrave_Tag *tag;

  style = engrave_file_style_last_get(engrave_file);

  tag = engrave_tag_new(key, val);
  engrave_style_tag_add(style, tag);
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
engrave_parse_part_source(char * source)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_source_set(part, source);
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
engrave_parse_part_precise_is_inside(int precise)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_precise_is_inside_set(part, precise);
}

void
engrave_parse_part_pointer_mode(int mode)
{
  Engrave_Group *group;
  Engrave_Part *part;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  engrave_part_pointer_mode_set(part, mode);
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
engrave_parse_state_inherit(char *name, double val)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *to;
  Engrave_Part_State *from;
  const char *state_name;
  double state_value;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);

  to = engrave_part_state_last_get(part);
  state_name = engrave_part_state_name_get(to, &state_value);

  /* must have a name set before we can be inherited into */
  if (!state_name) {
    const char *part_name = engrave_part_name_get(part);
    fprintf(stderr, "part %s: inherit may only be used after state!\n",
                                                            part_name);
    return;
  }

  /* can't inherit into the default part */
  if ((strlen(state_name) == 7) && (!strncmp(state_name, "default", 7)) && state_value == 0.0 ) {
    const char *part_name = engrave_part_name_get(part);
    fprintf(stderr, "part %s: "
              "inherit may not be used in the default description!\n",
              part_name);
    return;
  }

  from = engrave_part_state_by_name_value_find(part, name, val);
  if (from)
    engrave_part_state_copy(from, to);
  else
    fprintf(stderr, "Unable to locate description %s %f\n", name, val);
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
engrave_parse_state_fixed(double w, double h)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_fixed_size_set(state, w, h);
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
  engrave_part_state_max_size_set(state, w, h);
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
engrave_parse_state_gradient_spectrum(char * spec)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_spectrum_set(state, spec);
}

void
engrave_parse_state_gradient_rel1_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_rel1_relative_set(state, x, y);
}

void
engrave_parse_state_gradient_rel2_relative(double x, double y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_rel2_relative_set(state, x, y);
}

void
engrave_parse_state_gradient_rel1_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_rel1_offset_set(state, x, y);
}

void
engrave_parse_state_gradient_rel2_offset(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_rel2_offset_set(state, x, y);
}

void
engrave_parse_state_gradient_type(char * type)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_gradient_type_set(state, type);
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
  Engrave_Image *im;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  im = engrave_file_image_by_name_find(engrave_file, name);
  if (im)
    engrave_part_state_image_normal_set(state, im);
  else
    printf("Error: image \"%s\" does not exist\n", name);
}

void
engrave_parse_state_image_middle(int middle)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_image_middle_set(state, middle);

}

void
engrave_parse_state_image_tween(char *name)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;
  Engrave_Image *im;
 
  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);

  im = engrave_file_image_by_name_find(engrave_file, name);
  if (im)
    engrave_part_state_image_tween_add(state, im);
  else
    printf("Error: image \"%s\" does not exist\n", name);
}

void
engrave_parse_image_border(int l, int r, int t, int b)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_image_border_set(state, l, r, t, b);
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
  engrave_part_state_color_class_set(state, color_class);
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
  engrave_part_state_color_set(state, r, g, b, a);
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
  engrave_part_state_color2_set(state, r, g, b, a);
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
  engrave_part_state_color3_set(state, r, g, b, a);
}

void
engrave_parse_state_fill_smooth(int smooth)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_fill_smooth_set(state, smooth);
}

void
engrave_parse_state_fill_type(int type)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_fill_type_set(state, type);
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
  engrave_part_state_fill_origin_relative_set(state, x, y);
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
  engrave_part_state_fill_size_relative_set(state, x, y);
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
  engrave_part_state_fill_origin_offset_set(state, x, y);
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
  engrave_part_state_fill_size_offset_set(state, x, y);
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
  engrave_part_state_text_text_set(state, text);
}

void
engrave_parse_state_text_text_add(char *text)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_text_append(state, text);
}

void
engrave_parse_state_text_elipsis(double val)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_elipsis_set(state, val);
}


void
engrave_parse_state_text_text_source(char *text_source)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_text_source_set(state, text_source);
}

void
engrave_parse_state_text_source(char *source)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_source_set(state, source);
}


void
engrave_parse_state_text_style(char *style)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_style_set(state, style);
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
  engrave_part_state_text_text_class_set(state, text_class);
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
  engrave_part_state_text_font_set(state, font);
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
  engrave_part_state_text_size_set(state, size);
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
  engrave_part_state_text_fit_set(state, x, y);
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
  engrave_part_state_text_min_set(state, x, y);
}

void
engrave_parse_state_text_max(int x, int y)
{
  Engrave_Group *group;
  Engrave_Part *part;
  Engrave_Part_State *state;

  group = engrave_file_group_last_get(engrave_file);
  part = engrave_group_part_last_get(group);
  state = engrave_part_state_last_get(part);
  engrave_part_state_text_max_set(state, x, y);
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
  engrave_part_state_text_align_set(state, x, y);
}

void
engrave_parse_program()
{
  Engrave_Group *group;
  Engrave_Program *program;

  program = engrave_program_new();
  group = engrave_file_group_last_get(engrave_file);
  engrave_group_program_add(group, program);
}

void
engrave_parse_program_script(char *script)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_script_set(program, script);
}

void
engrave_parse_program_name(char *name)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_name_set(program, name);
}

void
engrave_parse_program_signal(char *signal)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_signal_set(program, signal);
}

void
engrave_parse_program_source(char *source)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_source_set(program, source);
}

void
engrave_parse_program_target(char *target)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_target_add(program, target);
}

void
engrave_parse_program_after(char *after)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_after_add(program, after);
}

void
engrave_parse_program_in(double from, double range)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_in_set(program, from, range);
}

/* handle different action types */
void
engrave_parse_program_action(Engrave_Action action, char *state, 
                                char *state2, double value, double value2)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_action_set(program, action, state, state2, value, value2);
}

void
engrave_parse_program_transition(Engrave_Transition transition, double duration)
{
  Engrave_Group *group;
  Engrave_Program *program;

  group = engrave_file_group_last_get(engrave_file);
  program = engrave_group_program_last_get(group);
  engrave_program_transition_set(program, transition, duration);
}

