#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "Engrave.h"

static void _engrave_output_part(Engrave_Part *part, void *data);
static void _engrave_output_program(Engrave_Program *program, void *data);
static void _engrave_output_state(Engrave_Part_State *state, Engrave_Part *part, void *data);
static void _engrave_output_image(Engrave_Image *image, void *data);
static void _engrave_output_font(Engrave_Font *font, void *data);
static void _engrave_output_data(Engrave_Data *data, void *udata);
static void _engrave_output_group(Engrave_Group *group, void *data);
static void _engrave_program_output_after(char *after, void *data);
static void _engrave_program_output_target(char *target, void *data);
static void _engrave_part_state_output_tween(Engrave_Image *ei, void *data);

static int level = 0;

static char *_image_type_string[ENGRAVE_IMAGE_TYPE_NUM] = {
        "RAW",
        "COMP",
        "LOSSY"
        };

static char *_part_type_string[ENGRAVE_PART_TYPE_NUM] = {
        "IMAGE",
        "TEXT",
        "RECT",
        "SWALLOW"
        };

static char *_text_effect_string[ENGRAVE_TEXT_EFFECT_NUM] = {
        "NONE",
        "PLAIN",
        "OUTLINE",
        "SOFT_OUTLINE",
        "SHADOW",
        "OUTLINE_SHADOW",
        "SOFT_SHADOW",
        "OUTLINE_SOFT_SHADOW"
        };

static char *_action_string[ENGRAVE_ACTION_NUM] = {
        "STATE_SET",
        "ACTION_STOP",
        "SIGNAL_EMIT",
        "DRAG_VAL_SET",
        "DRAG_VAL_STEP",
        "DRAG_VAL_PAGE",
        "SCRIPT"
        };

static char *_transition_string[ENGRAVE_TRANSITION_NUM] = {
        "LINEAR",
        "SINUSOIDAL",
        "ACCELERATE",
        "DECELERATE"
        };

static char *_aspect_preference_string[ENGRAVE_ASPECT_PREFERENCE_NUM] = {
        "NONE",
        "VERTICAL",
        "HORIZONTAL",
        "BOTH"
        };

static char *
engrave_output_mk_tabs(void)
{
    char *buf = (char *)calloc(level + 1, sizeof(char));
    int i = 0;
    for(i = 0; i < level; i++)
        buf[i] = '\t';
    return buf;
}

static void
engrave_out_start(FILE *out, char *type)
{
    char *buf = engrave_output_mk_tabs();
    fprintf(out, "%s%s {\n", buf, type);
    FREE(buf);
    level ++;
}

static void
engrave_out_end(FILE *out)
{
    char *buf = NULL;
    level--;
    buf = engrave_output_mk_tabs();
    fprintf(out, "%s}\n", buf);
    FREE(buf);
}

static void
engrave_out_data(FILE *out, char *name, char *fmt, ...)
{
    va_list ap;
    char *fmt_new = (char *)calloc(strlen(fmt) + strlen(name) + level + 5, sizeof(char));
    char *buf = engrave_output_mk_tabs();

    sprintf(fmt_new, "%s%s: %s;\n", buf, name, fmt);
    va_start(ap, fmt);
    vfprintf(out, fmt_new, ap);
    va_end(ap);
    FREE(buf);
}

/** 
 * engrave_eet_output -- Create an EET file from the in-memory data.
 * @param engrave_file: The Engrave_File to use to create the EET file.
 * @param path: The filename to save the EET file too.
 *
 * @return Returns 1 on success 0 otherwise.
 */
int
engrave_eet_output(Engrave_File *engrave_file, char *path)
{
  static char tmpn[1024];
  int len = 0, fd = 0, ret = 0;
  char *cmd = NULL;

  strcpy(tmpn, "/tmp/engrave_cc.edc-tmp-XXXXXX");
  fd = mkstemp(tmpn);
  if (fd < 0) {
    fprintf(stderr, "Unable to create tmp file: %s\n", strerror(errno));
    return 0;
  }
  close(fd);

  engrave_edc_output(engrave_file, tmpn);
  /* FIXME images and fonts ??? */

  len = strlen(tmpn) + strlen(path) + 13;
  cmd = (char *)calloc(len, sizeof(char));
  snprintf(cmd, len, "edje_cc -v %s %s", tmpn, path);
  ret = system(cmd);

  if (ret < 0) {
    fprintf(stderr, "Unable to execute edje_cc on tmp file: %s\n", 
                                                    strerror(errno));
    return 0;                                                    
  }

  unlink(tmpn);
  return 1;
}

/** 
 * engrave_edc_output -- Create an EDC file from the in-memory data.
 * @param engrave_file: The Engrave_File to use to create the EET file.
 * @param path: The filename to save the EDC file too.
 *
 * @return Returns 1 on success 0 otherwise.
 */
int
engrave_edc_output(Engrave_File *engrave_file, char *path)
{
  FILE *out = NULL;
  
  if (!engrave_file) return 0;

  out = fopen(path, "w");
  if (!out)
  {
    printf("can't open %s for writing\n", path);
    return 0;
  }

  /* fonts */
  engrave_out_start(out, "fonts");
  engrave_file_font_foreach(engrave_file, _engrave_output_font, out);
  engrave_out_end(out);

  /* images */
  engrave_out_start(out, "images");
  engrave_file_image_foreach(engrave_file, _engrave_output_image, out);
  engrave_out_end(out);

  /* data */
  engrave_out_start(out, "data");
  engrave_file_data_foreach(engrave_file, _engrave_output_data, out);
  engrave_out_end(out);

  /* collections */
  engrave_out_start(out, "collections");
  engrave_file_group_foreach(engrave_file, _engrave_output_group, out);
  engrave_out_end(out);

  fclose(out);
  return 1;
}

static void
_engrave_output_group(Engrave_Group *group, void *data)
{
  FILE *out = data;
  char *tmp = NULL;
  int w, h;

  engrave_out_start(out, "group");

  tmp = engrave_group_name_get(group);
  engrave_out_data(out, "name", "\"%s\"", tmp);
  if (tmp) free(tmp);

  engrave_group_min_size_get(group, &w, &h);
  if (w != 0 || h != 0)
    engrave_out_data(out, "min", "%d %d", w, h);

  engrave_group_max_size_get(group, &w, &h);
  if(w >= 0 || h >= 0)
    engrave_out_data(out, "max", "%d %d", w, h);
    
  /* data */
  if (engrave_group_has_data(group))
  {
    engrave_out_start(out, "data");
    engrave_group_data_foreach(group, _engrave_output_data, out);
    engrave_out_end(out);
  }

  /* script */
  tmp = engrave_group_script_get(group);
  if (tmp) {
    engrave_out_start(out, "script");
    /* FIXME scripts are wierd... */
    fprintf(out, "%s", tmp);
    engrave_out_end(out);
    free(tmp);
  }

  /* parts */
  engrave_out_start(out, "parts");
  engrave_group_parts_foreach(group, _engrave_output_part, out);
  engrave_out_end(out);

  /* programs */
  engrave_out_start(out, "programs");
  engrave_group_programs_foreach(group, _engrave_output_program, out);
  engrave_out_end(out);

  engrave_out_end(out);   /* group */
}

static void
_engrave_output_part(Engrave_Part *part, void *data)
{
  FILE *out = data;
  char *tmp;
  int x, step_x, count_x;
  int y, step_y, count_y;

  engrave_out_start(out, "part");

  tmp = engrave_part_name_get(part);
  engrave_out_data(out, "name", "\"%s\"", tmp);
  if (tmp) free(tmp);

  engrave_out_data(out, "type", "%s", 
        _part_type_string[engrave_part_type_get(part)]);

  if (!engrave_part_mouse_events_get(part))
    engrave_out_data(out, "mouse_events", "%d",
                    engrave_part_mouse_events_get(part));

  if (engrave_part_repeat_events_get(part))
    engrave_out_data(out, "repeat_events", "%d",
                  engrave_part_repeat_events_get(part));

  if (engrave_part_effect_get(part) != ENGRAVE_TEXT_EFFECT_NONE)
    engrave_out_data(out, "effect", "%s", 
                  _text_effect_string[engrave_part_effect_get(part)]);

  tmp = engrave_part_clip_to_get(part);
  if (tmp)
  {
    engrave_out_data(out, "clip_to", "\"%s\"", tmp);
    free(tmp);
  }

  engrave_part_dragable_x_get(part, &x, &step_x, &count_x);
  engrave_part_dragable_y_get(part, &y, &step_y, &count_y);
  if (x || y)
  {
    engrave_out_start(out, "dragable");
    engrave_out_data(out, "x", "%d %d %d", x, step_x, count_x);
    engrave_out_data(out, "y", "%d %d %d", y, step_y, count_y);

    tmp = engrave_part_dragable_confine_get(part);
    if (tmp)
    {
      engrave_out_data(out, "confine", "\"%s\"", tmp);
      free(tmp);
    }
    engrave_out_end(out);
  }
  engrave_part_state_foreach(part, _engrave_output_state, out);
  engrave_out_end(out);
}

static void
_engrave_output_program(Engrave_Program *program, void *data)
{
  FILE *out = data;
  char *tmp;
  Engrave_Action action;
  double value, value2;
  char state[125], state2[125];

  engrave_out_start(out, "program");

  tmp = engrave_program_name_get(program);
  engrave_out_data(out, "name", "\"%s\"", tmp);
  if (tmp) free(tmp);

  tmp = engrave_program_signal_get(program);
  engrave_out_data(out, "signal", "\"%s\"", tmp);
  if (tmp) free(tmp);

  tmp = engrave_program_source_get(program);
  engrave_out_data(out, "source", "\"%s\"", tmp);
  if (tmp) free(tmp);

  engrave_program_action_get(program, &action, state, 
                              state2, &value, &value2);
  switch (action) {
    case ENGRAVE_ACTION_STOP:
      engrave_out_data(out, "action", "%s", _action_string[action]);
      break;

    case ENGRAVE_ACTION_DRAG_VAL_SET:
    case ENGRAVE_ACTION_DRAG_VAL_STEP:
    case ENGRAVE_ACTION_DRAG_VAL_PAGE:
      engrave_out_data(out, "action", "%s %.2f %.2f",
            _action_string[action], value, value2);
      break;

    case ENGRAVE_ACTION_STATE_SET:
      {
        Engrave_Transition transition;
        double duration;

        engrave_out_data(out, "action", "%s \"%s\" %.2f",
              _action_string[action], state ? state : "", value);

        engrave_program_transition_get(program, &transition, &duration);
        engrave_out_data(out, "transition", "%s %.2f",
              _transition_string[transition], duration);
      }
      break;

    case ENGRAVE_ACTION_SIGNAL_EMIT:
      engrave_out_data(out, "action", "%s \"%s\" \"%s\"",
            _action_string[action], state, state2);
      break;

    case ENGRAVE_ACTION_SCRIPT:
      {
        char *script = engrave_program_script_get(program);
        if (script)
        {
          engrave_out_start(out, "script");
          /* FIXME scripts are wierd ... */
          fprintf(out, "%s", script);
          engrave_out_end(out);
          free(script);
        }
      }
      break;

    case ENGRAVE_ACTION_NUM:
      break;

    default:
      fprintf(stderr, "Unknown program action: %d\n", action);
      break;
  }

  engrave_program_in_get(program, &value, &value2);
  if (value || value2)
    engrave_out_data(out, "in", "%.2f %.2f", value, value2 );

  engrave_program_target_foreach(program, _engrave_program_output_target, out);
  engrave_program_after_foreach(program, _engrave_program_output_after, out);

  engrave_out_end(out);
}

/* Note: We only output if the value is different from the default to save
 * space 
 */
static void
_engrave_output_state(Engrave_Part_State *state, Engrave_Part *part, void *data)
{
  FILE *out = data;
  
  engrave_out_start(out, "description");

  engrave_out_data(out, "state", "\"%s\" %.2f", state->name, state->value);
  engrave_out_data(out, "visible", "%d", state->visible);

  if (state->align.x != .5 || state->align.y != .5)
    engrave_out_data(out, "align", "%.2f %.2f", state->align.x, state->align.y);

  if (state->step.x || state->step.y)
    engrave_out_data(out, "step", "%.2f %.2f", state->step.x, state->step.y);

  if (state->min.w > 0 || state->min.h > 0)
    engrave_out_data(out, "min", "%d %d", state->min.w, state->min.h);

  if (state->max.w >= 0 || state->max.h >= 0)
    engrave_out_data(out, "max", "%d %d", state->max.w, state->max.h);

  if (state->aspect.w || state->aspect.h)
    engrave_out_data(out, "aspect", "%.2f %.2f", 
            state->aspect.w, state->aspect.h);

  if (state->aspect.prefer)
    engrave_out_data(out, "aspect_preference", "%s",
            _aspect_preference_string[state->aspect.prefer]);
  
  /* rel 1 */ 
  engrave_out_start(out, "rel1");
  engrave_out_data(out, "relative", "%.2f %.2f",
          state->rel1.relative.x, state->rel1.relative.y);
  engrave_out_data(out, "offset", "%d %d",
          state->rel1.offset.x, state->rel1.offset.y);

  if ( state->rel1.to_x || state->rel1.to_y)
  {
    if (!strcmp(state->rel1.to_x, state->rel1.to_y))
    {
      engrave_out_data(out, "to", "\"%s\"", state->rel1.to_x);
    }
    else
    {
      engrave_out_data(out, "to_x", "\"%s\"", state->rel1.to_x);
      engrave_out_data(out, "to_y", "\"%s\"", state->rel1.to_y);
    }
  }
  engrave_out_end(out);

  /* rel 2 */ 
  engrave_out_start(out, "rel2");
  engrave_out_data(out, "relative", "%.2f %.2f",
          state->rel2.relative.x, state->rel2.relative.y);
  engrave_out_data(out, "offset", "%d %d",
          state->rel2.offset.x, state->rel2.offset.y);

  if ( state->rel2.to_x || state->rel2.to_y)
  {
    if (!strcmp(state->rel2.to_x, state->rel2.to_y))
    {
      engrave_out_data(out, "to", "\"%s\"", state->rel2.to_x);
    }
    else
    {
      engrave_out_data(out, "to_x", "\"%s\"", state->rel2.to_x);
      engrave_out_data(out, "to_y", "\"%s\"", state->rel2.to_y);
    }
  }
  engrave_out_end(out);

  if (state->color_class)
    engrave_out_data(out, "color_class", "\"%s\"", state->color_class);

  if (state->color.r != 255 || state->color.g != 255 ||
      state->color.b != 255 || state->color.a != 255)
    engrave_out_data(out, "color", "%d %d %d %d", 
            state->color.r, state->color.g, state->color.b, state->color.a);

  if (state->color2.r != 0 || state->color2.g != 0 ||
      state->color2.b != 0 || state->color2.a != 255)
    engrave_out_data(out, "color2", "%d %d %d %d",
            state->color2.r, state->color2.g, state->color2.b, state->color2.a);

  if (state->color3.r != 0 || state->color3.g != 0 ||
      state->color3.b != 0 || state->color3.a != 128)
    engrave_out_data(out, "color3", "%d %d %d %d", 
            state->color3.r, state->color3.g, state->color3.b, state->color3.a);

  if (part->type == ENGRAVE_PART_TYPE_IMAGE && state->image.normal )
  {
    engrave_out_start(out, "image");
    engrave_out_data(out, "normal", "\"%s\"", state->image.normal->name);
    engrave_part_state_tween_foreach(state,
                        _engrave_part_state_output_tween, out);

    if (state->image.border.l || state->image.border.r 
            || state->image.border.t || state->image.border.b)
      engrave_out_data(out, "border", "%d %d %d %d", 
             state->image.border.l, state->image.border.r,
             state->image.border.t, state->image.border.b);
    engrave_out_end(out);
  }
  else if (part->type == ENGRAVE_PART_TYPE_TEXT)
  {
    engrave_out_start(out, "text");

    if (state->text.text)
      engrave_out_data(out, "text", "\"%s\"", state->text.text);
    
    if (state->text.text_class)
      engrave_out_data(out, "text_class", "\"%s\"", state->text.text_class);

    if (state->text.font)
      engrave_out_data(out, "font", "\"%s\"", state->text.font);
    
    if (state->text.size)
      engrave_out_data(out, "size", "%d", state->text.size);

    if (state->text.fit.x || state->text.fit.y)
      engrave_out_data(out, "fit", "%d %d", 
              state->text.fit.x, state->text.fit.y);
  
    if (state->text.min.x || state->text.min.y)
      engrave_out_data(out, "min", "%d %d",
              state->text.min.x, state->text.min.y);

    if (state->text.align.x || state->text.align.y)
      engrave_out_data(out, "align", "%.2f %.2f",
              state->text.align.x, state->text.align.y);

    engrave_out_end(out);
  }

  engrave_out_end(out);
}

static void
_engrave_output_image(Engrave_Image *image, void *data)
{
  char *name;
  FILE *out;

  out = data;
  name = engrave_image_name_get(image);

  if (engrave_image_type_get(image) == ENGRAVE_IMAGE_TYPE_LOSSY)
    engrave_out_data(out, "image", "\"%s\" %s %.2f", name,
              _image_type_string[engrave_image_type_get(image)],
              engrave_image_value_get(image));
  else
    engrave_out_data(out, "image", "\"%s\" %s", name,
              _image_type_string[engrave_image_type_get(image)]);

  if (name) free(name);
}

static void
_engrave_output_font(Engrave_Font *font, void *data)
{
  char *name, *path;
  FILE *out;

  out = data;
  name = engrave_font_name_get(font);
  path = engrave_font_path_get(font);

  engrave_out_data(out, "font", "\"%s\" \"%s\"", path, name);
  if (name) free(name);
  if (path) free(path);
}

static void
_engrave_output_data(Engrave_Data *data, void *udata)
{
  char *key, *value;
  FILE *out;

  out = udata;
  key = engrave_data_key_get(data);
  value = engrave_data_value_get(data);

  if (value)
    engrave_out_data(out, "item", "\"%s\" \"%s\"", key, value);
  else
    engrave_out_data(out, "item", "\"%s\" %d",
                            key, engrave_data_int_value_get(data));

  free(key);
  free(value);
}

static void
_engrave_program_output_target(char *target, void *data)
{
    FILE *out = data;
    engrave_out_data(out, "target", "\"%s\"", target);
}

static void
_engrave_program_output_after(char *after, void *data)
{
  FILE *out = data;
  engrave_out_data(out, "after", "\"%s\"", after);
}

static void
_engrave_part_state_output_tween(Engrave_Image *ei, void *data)
{
  FILE *out = data;
  char *name = engrave_image_name_get(ei);

  engrave_out_data(out, "tween", "\"%s\"", name);
  if (name) free(name);
}


