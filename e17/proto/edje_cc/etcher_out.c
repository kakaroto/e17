#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "Etcher.h"

void etcher_file_output(Etcher_File *etcher_file, char *path);

static void _etcher_output_group(Etcher_Group *group, FILE *out);
static void _etcher_output_part(Etcher_Part *part, FILE *out);
static void _etcher_output_program(Etcher_Program *program, FILE *out);
static void _etcher_output_state(Etcher_Part *part, Etcher_Part_State *state, FILE *out);

static int level = 0;

char *_image_type_string[ETCHER_IMAGE_TYPE_NUM] = {
        "RAW",
        "COMP",
        "LOSSY",
        "USER"
        };

char *_part_type_string[ETCHER_PART_TYPE_NUM] = {
        "IMAGE",
        "TEXT",
        "RECT",
        "SWALLOW"
        };

char *_text_effect_string[ETCHER_TEXT_EFFECT_NUM] = {
        "NONE",
        "PLAIN",
        "OUTLINE",
        "SOFT_OUTLINE",
        "SHADOW",
        "OUTLINE_SHADOW",
        "SOFT_SHADOW",
        "OUTLINE_SOFT_SHADOW"
        };

char *_action_string[ETCHER_ACTION_NUM] = {
  	"STATE_SET",
  	"ACTION_STOP",
  	"SIGNAL_EMIT",
  	"DRAG_VAL_SET",
  	"DRAG_VAL_STEP",
  	"DRAG_VAL_PAGE",
  	"SCRIPT"
        };

char *_transition_string[ETCHER_TRANSITION_NUM] = {
	"LINEAR",
	"SINUSOIDAL",
	"ACCELERATE",
	"DECELERATE"
        };

char *_aspect_preference_string[ETCHER_ASPECT_PREFERENCE_NUM] = {
	"NONE",
	"VERTICAL",
	"HORIZONTAL",
	"BOTH"
        };

static char *
etcher_output_mk_tabs(void)
{
    char *buf = (char *)calloc(level + 1, sizeof(char));
    int i = 0;
    for(i = 0; i < level; i++)
        buf[i] = '\t';
    return buf;
}

static void
etcher_out_start(FILE *out, char *type)
{
    char *buf = etcher_output_mk_tabs();
    fprintf(out, "%s%s {\n", buf, type);
    free(buf);

    level ++;
}

static void
etcher_out_end(FILE *out)
{
    char *buf = NULL;

    level--;
    buf = etcher_output_mk_tabs();
    fprintf(out, "%s}\n", buf);
    free(buf);

}

static void
etcher_out_data(FILE *out, char *name, char *fmt, ...)
{
    va_list ap;
    char *fmt_new = (char *)calloc(strlen(fmt) + strlen(name) + level + 5, sizeof(char));
    char *buf = etcher_output_mk_tabs();

    sprintf(fmt_new, "%s%s: %s;\n", buf, name, fmt);
    va_start(ap, fmt);
    vfprintf(out, fmt_new, ap);
    va_end(ap);

    free(buf);
}

int
etcher_eet_output(Etcher_File *etcher_file, char *path)
{
  static char tmpn[1024];
  int len = 0, fd = 0, ret = 0;
  char *cmd = NULL;

  strcpy(tmpn, "/tmp/etcher_cc.edc-tmp-XXXXXX");
  fd = mkstemp(tmpn);
  if (fd < 0) {
    fprintf(stderr, "Unable to create tmp file: %s\n", strerror(errno));
    return 0;
  }
  close(fd);

  etcher_file_output(etcher_file, tmpn);
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

void
etcher_file_output(Etcher_File *etcher_file, char *path)
{
  FILE *out = NULL;
  Evas_List *l;
  
  if (!etcher_file) return;

  out = fopen(path, "w");

  if (!out)
  {
    printf("can't open %s for writing\n", path);
    return;
  }

  /* fonts */
  etcher_out_start(out, "fonts");
  for (l = etcher_file->fonts; l; l = l->next)
  {
    Etcher_Font *font = l->data;
    if (font)
      etcher_out_data(out, "font", "\"%s\" \"%s\"", font->file, font->name);
  }
  etcher_out_end(out);

  /* images */
  etcher_out_start(out, "images");
  for (l = etcher_file->images; l; l = l->next)
  {
    Etcher_Image *image = l->data;
    if (image)
    {
      if (image->type == ETCHER_IMAGE_TYPE_LOSSY)
        etcher_out_data(out, "image", "\"%s\" %s %.2f", image->name,
                _image_type_string[image->type],
                image->value);
      else
        etcher_out_data(out, "image", "\"%s\" %s", image->name,
                _image_type_string[image->type]);
    }
  }
  etcher_out_end(out);

  /* data */
  etcher_out_start(out, "data");
  for (l = etcher_file->data; l; l = l->next)
  {
    Etcher_Data *data = l->data;
    if (data)
    {
      if (data->value)
        etcher_out_data(out, "item", "\"%s\" \"%s\"",
                data->key, data->value);
      else
        etcher_out_data(out, "item", "\"%s\" %d",
                data->key, data->int_value);
    }
  }
  etcher_out_end(out);

  /* collections */
  etcher_out_start(out, "collections");
  for (l = etcher_file->groups; l; l = l->next)
  {
    Etcher_Group *group = l->data;
    if (group) _etcher_output_group(group, out);
  }
  etcher_out_end(out);

  fclose(out);

}

void
_etcher_output_group(Etcher_Group *group, FILE *out)
{
  Evas_List *l;

  etcher_out_start(out, "group");

  /* FIXME max is 0 by default, this is bad... */
  etcher_out_data(out, "name", "\"%s\"", group->name);

  if (group->min.w != 0 || group->min.h != 0)
    etcher_out_data(out, "min", "%d %d", group->min.w, group->min.h);

  if(group->max.w >= 0 || group->max.h >= 0)
    etcher_out_data(out, "max", "%d %d", group->max.w, group->max.h);
    
  /* data */
  if (group->data)
  {
    etcher_out_start(out, "data");
    for (l = group->data; l; l = l->next)
    {
      Etcher_Data *data = l->data;
      if (data)
      {
        if (data->value)
          etcher_out_data(out, "item", "\"%s\" \"%s\"",
                    data->key, data->value);
        else
          etcher_out_data(out, "item", "\"%s\" %d",
                    data->key, data->int_value);
		
      }
    }
    etcher_out_end(out);
  }

  /* script */
  if (group->script) {
    etcher_out_start(out, "script");
    /* FIXME scripts are wierd... */
    fprintf(out, "%s", group->script);
    etcher_out_end(out);
  }

  /* parts */
  etcher_out_start(out, "parts");
  for (l = group->parts; l; l = l->next)
  {
    Etcher_Part *part = l->data;
    if (part) _etcher_output_part(part, out);
  }
  etcher_out_end(out);

  /* programs */
  etcher_out_start(out, "programs");
  for (l = group->programs; l; l = l->next)
  {
    Etcher_Program *program = l->data;
    if (program) _etcher_output_program(program, out);
  }
  etcher_out_end(out);

  etcher_out_end(out);   /* group */
}

void
_etcher_output_part(Etcher_Part *part, FILE *out)
{
  Evas_List *l;

  etcher_out_start(out, "part");

  etcher_out_data(out, "name", "\"%s\"", part->name);
  etcher_out_data(out, "type", "%s", _part_type_string[part->type]);

  if (!part->mouse_events)
    etcher_out_data(out, "mouse_events", "%d", part->mouse_events);

  if (part->repeat_events)
    etcher_out_data(out, "repeat_events", "%d", part->repeat_events);

  if (part->effect)
    etcher_out_data(out, "effect", "%s", _text_effect_string[part->effect]);

  if (part->clip_to)
    etcher_out_data(out, "clip_to", "\"%s\"", part->clip_to);

  if (part->dragable.x || part->dragable.y)
  {
    etcher_out_start(out, "dragable");
    etcher_out_data(out, "x", "%d %d %d", part->dragable.x,
                            part->dragable.step.x, part->dragable.count.x);
    etcher_out_data(out, "y", "%d %d %d", part->dragable.y,
                            part->dragable.step.y, part->dragable.count.y);
    if (part->dragable.confine)
      etcher_out_data(out, "confine", "\"%s\"", part->dragable.confine);

    etcher_out_end(out);
  }
  for (l = part->states; l; l = l->next)
  {
    Etcher_Part_State *state = l->data;
    if (state) _etcher_output_state(part, state, out);
  }

  etcher_out_end(out);
}


void
_etcher_output_program(Etcher_Program *program, FILE *out)
{
  Evas_List *l;

  etcher_out_start(out, "program");

  etcher_out_data(out, "name", "\"%s\"", program->name);
  etcher_out_data(out, "signal", "\"%s\"", program->signal);
  etcher_out_data(out, "source", "\"%s\"", program->source);

  if(program->action == ETCHER_ACTION_STOP)
  {
    etcher_out_data(out, "action", "%s", _action_string[program->action]);
  }
  else if (program->action == ETCHER_ACTION_DRAG_VAL_SET ||
           program->action == ETCHER_ACTION_DRAG_VAL_STEP ||
           program->action == ETCHER_ACTION_DRAG_VAL_PAGE)
  {
    etcher_out_data(out, "action", "%s %.2f %.2f",
            _action_string[program->action], 
            program->value,
            program->value2
            );
  }
  else if (program->action == ETCHER_ACTION_STATE_SET)
  {
    etcher_out_data(out, "action", "%s \"%s\" %.2f",
            _action_string[program->action],
            program->state ? program->state : "",
            program->value
            );

    etcher_out_data(out, "transition", "%s %.2f",
            _transition_string[program->transition],
            program->duration
            );
  }
  else if (program->action == ETCHER_ACTION_SIGNAL_EMIT)
  {
    etcher_out_data(out, "action", "%s \"%s\" \"%s\"",
            _action_string[program->action],
            program->state,
            program->state2
            );
  }
  else if(program->action == ETCHER_ACTION_SCRIPT) 
  {
    if (program->script)
    {
      etcher_out_start(out, "script");
      /* FIXME scripts are wierd ... */
      fprintf(out, "%s", program->script);
      etcher_out_end(out);
    }
  }

  if (program->in.from || program->in.range)
    etcher_out_data(out, "in", "%.2f %.2f",
            program->in.from,
            program->in.range
            );

  for (l = program->targets; l; l = l->next)
  {
    etcher_out_data(out, "target", "\"%s\"", (char *)l->data);
  }

  for (l = program->afters; l; l = l->next)
  {
    etcher_out_data(out, "after", "\"%s\"", (char *)l->data);
  }

  etcher_out_end(out);
}


void
_etcher_output_state(Etcher_Part *part, Etcher_Part_State *state, FILE *out)
{
  Evas_List *l;
  
  /* NB: many are only written if different from defaults (to save space) */

  etcher_out_start(out, "description");

  etcher_out_data(out, "state", "\"%s\" %.2f", state->name, state->value);
  etcher_out_data(out, "visible", "%d", state->visible);

  if (state->align.x != .5 || state->align.y != .5)
    etcher_out_data(out, "align", "%.2f %.2f", state->align.x, state->align.y);

  if (state->step.x || state->step.y)
    etcher_out_data(out, "step", "%.2f %.2f", state->step.x, state->step.y);

  if (state->min.w > 0 || state->min.h > 0)
    etcher_out_data(out, "min", "%d %d", state->min.w, state->min.h);

  if (state->max.w >= 0 || state->max.h >= 0)
    etcher_out_data(out, "max", "%d %d", state->max.w, state->max.h);

  if (state->aspect.w || state->aspect.h)
    etcher_out_data(out, "aspect", "%.2f %.2f", 
            state->aspect.w, state->aspect.h);

  if (state->aspect.prefer)
    etcher_out_data(out, "aspect_preference", "%s",
            _aspect_preference_string[state->aspect.prefer]);
  
  /* rel 1 */ 
  etcher_out_start(out, "rel1");
  etcher_out_data(out, "relative", "%.2f %.2f",
          state->rel1.relative.x, state->rel1.relative.y);
  etcher_out_data(out, "offset", "%d %d",
          state->rel1.offset.x, state->rel1.offset.y);

  if ( state->rel1.to_x || state->rel1.to_y)
  {
    if (!strcmp(state->rel1.to_x, state->rel1.to_y))
    {
      etcher_out_data(out, "to", "\"%s\"", state->rel1.to_x);
    }
    else
    {
      etcher_out_data(out, "to_x", "\"%s\"", state->rel1.to_x);
      etcher_out_data(out, "to_y", "\"%s\"", state->rel1.to_y);
    }
  }
  etcher_out_end(out);

  /* rel 2 */ 
  etcher_out_start(out, "rel2");
  etcher_out_data(out, "relative", "%.2f %.2f",
          state->rel2.relative.x, state->rel2.relative.y);
  etcher_out_data(out, "offset", "%d %d",
          state->rel2.offset.x, state->rel2.offset.y);

  if ( state->rel2.to_x || state->rel2.to_y)
  {
    if (!strcmp(state->rel2.to_x, state->rel2.to_y))
    {
      etcher_out_data(out, "to", "\"%s\"", state->rel2.to_x);
    }
    else
    {
      etcher_out_data(out, "to_x", "\"%s\"", state->rel2.to_x);
      etcher_out_data(out, "to_y", "\"%s\"", state->rel2.to_y);
    }
  }
  etcher_out_end(out);

  if (state->color_class)
    etcher_out_data(out, "color_class", "\"%s\"", state->color_class);

  if (state->border.l || state->border.r || state->border.t || state->border.b)
    etcher_out_data(out, "border", "%d %d %d %d", 
            state->border.l, state->border.r, state->border.t, state->border.b);

  if (state->color.r != 255 || state->color.g != 255 ||
      state->color.b != 255 || state->color.a != 255)
    etcher_out_data(out, "color", "%d %d %d %d", 
            state->color.r, state->color.g, state->color.b, state->color.a);

  if (state->color2.r != 0 || state->color2.g != 0 ||
      state->color2.b != 0 || state->color2.a != 255)
    etcher_out_data(out, "color2", "%d %d %d %d",
            state->color2.r, state->color2.g, state->color2.b, state->color2.a);

  if (state->color3.r != 0 || state->color3.g != 0 ||
      state->color3.b != 0 || state->color3.a != 128)
    etcher_out_data(out, "color3", "%d %d %d %d", 
            state->color3.r, state->color3.g, state->color3.b, state->color3.a);

  if (part->type == ETCHER_PART_TYPE_IMAGE && state->image.normal )
  {
    etcher_out_start(out, "image");
    etcher_out_data(out, "normal", "\"%s\"", state->image.normal->name);
    for (l = state->image.tween; l; l = l->next)
    {
      Etcher_Image *tw = l->data;
      etcher_out_data(out, "tween", "\"%s\"", tw->name);
    }
    etcher_out_end(out);
  }
  else if (part->type == ETCHER_PART_TYPE_TEXT)
  {
    etcher_out_start(out, "text");

    if (state->text.text)
      etcher_out_data(out, "text", "\"%s\"", state->text.text);
    
    if (state->text.text_class)
      etcher_out_data(out, "text_class", "\"%s\"", state->text.text_class);

    if (state->text.font)
      etcher_out_data(out, "font", "\"%s\"", state->text.font);
    
    if (state->text.size)
      etcher_out_data(out, "size", "%d", state->text.size);

    if (state->text.fit.x || state->text.fit.y)
      etcher_out_data(out, "fit", "%d %d", 
              state->text.fit.x, state->text.fit.y);
  
    if (state->text.min.x || state->text.min.y)
      etcher_out_data(out, "min", "%d %d",
              state->text.min.x, state->text.min.y);

    if (state->text.align.x || state->text.align.y)
      etcher_out_data(out, "align", "%.2f %.2f",
              state->text.align.x, state->text.align.y);

    etcher_out_end(out);
  }

  etcher_out_end(out);
}


