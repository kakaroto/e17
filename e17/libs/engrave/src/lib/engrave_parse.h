#ifndef ENGRAVE_PARSE_H
#define ENGRAVE_PARSE_H

#include "Engrave.h"

Engrave_File * engrave_parse(const char *file, const char *imdir, 
                             const char *fontdir);

void engrave_parse_font(char *file, char *name);
void engrave_parse_image(char *name, Engrave_Image_Type type, double value);
void engrave_parse_data(char *key, char *value, int int_val);
void engrave_parse_group();
void engrave_parse_group_data(char *key, char *value, int int_value);
void engrave_parse_group_script(char *script);
void engrave_parse_group_name(char *name);
void engrave_parse_group_alias(char *alias);
void engrave_parse_group_min(int w, int h);
void engrave_parse_group_max(int w, int h);

void engrave_parse_style();
void engrave_parse_style_name(char * name);
void engrave_parse_style_base(char * base);
void engrave_parse_style_tag(char * key, char * val);

Engrave_Color_Class * engrave_parse_file_color_class();
void engrave_parse_color_class_name(char *name);
void engrave_parse_color_class_color(int num, int r, int g, int b, int a);

void engrave_parse_spectrum();
void engrave_parse_spectrum_name(char * name);
void engrave_parse_spectrum_color(int r, int g, int b, int a, int d);

void engrave_parse_base(char *base);

void engrave_parse_part();
void engrave_parse_part_name(char *name);
void engrave_parse_part_type(Engrave_Part_Type type);
void engrave_parse_part_source(char *source);
void engrave_parse_part_effect(Engrave_Text_Effect effect);
void engrave_parse_part_mouse_events(int mouse_events);
void engrave_parse_part_repeat_events(int repeat_events);
void engrave_parse_part_precise_is_inside(int precise);
void engrave_parse_part_pointer_mode(int mode);
void engrave_parse_part_clip_to(char *clip_to);
void engrave_parse_part_dragable_x(int x, int step, int count);
void engrave_parse_part_dragable_y(int y, int step, int count);
void engrave_parse_part_dragable_confine(char *confine);
void engrave_parse_state();
void engrave_parse_state_name(char *name, double value);
void engrave_parse_state_visible(int visible);
void engrave_parse_state_inherit(char *name, double val);
void engrave_parse_state_align(double x, double y);
void engrave_parse_state_step(double x, double y);
void engrave_parse_state_min(double w, double h);
void engrave_parse_state_fixed(double w, double h);
void engrave_parse_state_max(double w, double h);
void engrave_parse_state_aspect(double w, double h);
void engrave_parse_state_aspect_preference(Engrave_Aspect_Preference prefer);
void engrave_parse_state_rel1_relative(double x, double y);
void engrave_parse_state_rel1_offset(int x, int y);
void engrave_parse_state_rel1_to_x(char *to);
void engrave_parse_state_rel1_to_y(char *to);
void engrave_parse_state_rel1_to(char *to);
void engrave_parse_state_rel2_relative(double x, double y);
void engrave_parse_state_rel2_offset(int x, int y);
void engrave_parse_state_rel2_to_x(char *to);
void engrave_parse_state_rel2_to_y(char *to);
void engrave_parse_state_rel2_to(char *to);
void engrave_parse_state_image_normal(char *name);
void engrave_parse_state_image_middle(int middle);
void engrave_parse_state_image_tween(char *name);
void engrave_parse_image_border(int l, int r, int t, int b);
void engrave_parse_state_color_class(char *color_class);
void engrave_parse_state_color(int r, int g, int b, int a);
void engrave_parse_state_color2(int r, int g, int b, int a);
void engrave_parse_state_color3(int r, int g, int b, int a);
void engrave_parse_state_fill_smooth(int smooth);
void engrave_parse_state_fill_type(int type);
void engrave_parse_state_fill_origin_relative(double x, double y);
void engrave_parse_state_fill_size_relative(double x, double y);
void engrave_parse_state_fill_origin_offset(int x, int y);
void engrave_parse_state_fill_size_offset(int x, int y);
void engrave_parse_state_text_text(char *text);
void engrave_parse_state_text_elipsis(double val);
void engrave_parse_state_text_style(char * style);
void engrave_parse_state_text_text_add(char *text); /* used to concat texts */
void engrave_parse_state_text_text_class(char *text_class);
void engrave_parse_state_text_text_source(char *text_source);
void engrave_parse_state_text_text_style(char *text_style);
void engrave_parse_state_text_source(char *source);
void engrave_parse_state_text_font(char *font);
void engrave_parse_state_text_size(int size);
void engrave_parse_state_text_fit(int x, int y);
void engrave_parse_state_text_min(int x, int y);
void engrave_parse_state_text_max(int x, int y);
void engrave_parse_state_text_align(double x, double y);
void engrave_parse_program();
void engrave_parse_program_script(char *script);
void engrave_parse_program_name(char *name);
void engrave_parse_program_signal(char *signal);
void engrave_parse_program_source(char *source);
void engrave_parse_program_target(char *target);
void engrave_parse_program_after(char *after);
void engrave_parse_program_in(double from, double range);
void engrave_parse_program_action(Engrave_Action action, char *state, 
                                  char *state2, double value, 
                                  double value2);
void engrave_parse_program_transition(Engrave_Transition transition, 
                                      double duration);

void engrave_parse_state_gradient_spectrum(char * spec);
void engrave_parse_state_gradient_type(char * type);
void engrave_parse_state_gradient_rel1_relative(double x, double y);
void engrave_parse_state_gradient_rel2_relative(double x, double y);
void engrave_parse_state_gradient_rel1_offset(int x, int y);
void engrave_parse_state_gradient_rel2_offset(int x, int y);
#endif

