#include <stdio.h>
#include "Etcher.h"

void etcher_file_output(Etcher_File *etcher_file, char *path);

static void _etcher_output_group(Etcher_Group *group, FILE *out);
static void _etcher_output_part(Etcher_Part *part, FILE *out);
static void _etcher_output_program(Etcher_Program *program, FILE *out);
static void _etcher_output_state(Etcher_Part_State *state, FILE *out);


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
  fprintf(out, "fonts {\n");
  for (l = etcher_file->fonts; l; l = l->next)
  {
    Etcher_Font *font = l->data;
    if (font)
      fprintf(out, "\tfont: \"%s\" \"%s\";\n", font->file, font->name);
  }
  fprintf(out, "}\n");

  /* images */
  fprintf(out, "images {\n");
  for (l = etcher_file->images; l; l = l->next)
  {
    Etcher_Image *image = l->data;
    if (image)
    {
      if (image->type == ETCHER_IMAGE_TYPE_LOSSY)
        fprintf(out, "\timage: \"%s\" %s %.2f;\n", image->name,
                _image_type_string[image->type],
                image->value);
      else
        fprintf(out, "\timage: \"%s\" %s;\n", image->name,
                _image_type_string[image->type]);
    }
  }
  fprintf(out, "}\n");

  /* data */
  fprintf(out, "data {\n");
  for (l = etcher_file->data; l; l = l->next)
  {
    Etcher_Data *data = l->data;
    if (data)
    {
        fprintf(out, "\titem: \"%s\" \"%s\";\n",
                data->key, data->value);
    }
  }
  fprintf(out, "}\n");

  /* groups */
  fprintf(out, "collections {\n");
  for (l = etcher_file->groups; l; l = l->next)
  {
    Etcher_Group *group = l->data;
    if (group) _etcher_output_group(group, out);
  }
  fprintf(out, "}\n");

  fclose(out);

}

void
_etcher_output_group(Etcher_Group *group, FILE *out)
{
  Evas_List *l;

  fprintf(out, "\tgroup {\n");
  /* FIXME max is 0 by default, this is bad... */
  fprintf(out, "\t\tname: \"%s\";\n", group->name);
          

  if (group->min.w != 0 || group->min.h != 0)
    fprintf(out, "\t\tmin: %d %d;\n", group->min.w, group->min.h);

  if(group->max.w != 0x7fffffff || group->max.h != 0x7fffffff)
    fprintf(out, "\t\tmax: %d %d;\n", group->max.w, group->max.h);
    
  /* data */
  fprintf(out, "\t\tdata {\n");
  for (l = group->data; l; l = l->next)
  {
    Etcher_Data *data = l->data;
    if (data)
    {
        fprintf(out, "\t\t\titem: \"%s\" \"%s\";\n",
                data->key, data->value);
    }
  }
  fprintf(out, "\t\t}\n");

  /* script */
  if (group->script)
    fprintf(out, "\t\tscript {%s\t\t}\n", group->script);

  /* parts */
  fprintf(out, "\t\tparts {\n");
  for (l = group->parts; l; l = l->next)
  {
    Etcher_Part *part = l->data;
    if (part) _etcher_output_part(part, out);
  }
  fprintf(out, "\t\t}\n");

  /* programs */
  fprintf(out, "\t\tprograms {\n");
  for (l = group->programs; l; l = l->next)
  {
    Etcher_Program *program = l->data;
    if (program) _etcher_output_program(program, out);
  }
  fprintf(out, "\t\t}\n");


  fprintf(out, "\t}\n");
}

void
_etcher_output_part(Etcher_Part *part, FILE *out)
{
  Evas_List *l;

  fprintf(out, "\t\t\tpart {\n");
  fprintf(out, "\t\t\t\tname: \"%s\";\n\t\t\t\ttype: %s;\n\t\t\t\tmouse_events: %d;\n\t\t\t\trepeat_events: %d;\n",
          part->name,
          _part_type_string[part->type],
          part->mouse_events,
          part->repeat_events
          );

  if (part->effect)
    fprintf(out, "\t\t\t\teffect: %s;\n", _text_effect_string[part->effect]);

  if (part->clip_to)
    fprintf(out, "\t\t\t\tclip_to: \"%s\";\n", part->clip_to);

  if (part->dragable.x || part->dragable.y)
    fprintf(out, "\t\t\t\tdragable {\n\t\t\t\t\tx: %d %d %d;\n\t\t\t\t\ty: %d %d %d;\n\t\t\t\t\tconfine: \"%s\"\n\t\t\t\t}\n",
            part->dragable.x,
            part->dragable.step.x,
            part->dragable.count.x,
            part->dragable.y,
            part->dragable.step.y,
            part->dragable.count.y,
            part->dragable.confine ? part->dragable.confine : ""
            );

  for (l = part->states; l; l = l->next)
  {
    Etcher_Part_State *state = l->data;
    if (state) _etcher_output_state(state, out);
  }


  fprintf(out, "\t\t\t}\n");
}


void
_etcher_output_program(Etcher_Program *program, FILE *out)
{
  Evas_List *l;

  fprintf(out, "\t\t\tprogram {\n");

  fprintf(out, "\t\t\t\tname: \"%s\";\n\t\t\t\tsignal: \"%s\";\n\t\t\t\tsource: \"%s\";\n",
          program->name,
          program->signal,
          program->source
          );

  if(program->action == ETCHER_ACTION_STOP)
  {
    fprintf(out, "\t\t\t\taction: %s;\n", _action_string[program->action]);
  }
  else if (program->action == ETCHER_ACTION_DRAG_VAL_SET ||
           program->action == ETCHER_ACTION_DRAG_VAL_STEP ||
           program->action == ETCHER_ACTION_DRAG_VAL_PAGE)
  {
    fprintf(out, "\t\t\t\taction: %s %.2f %.2f;\n",
            _action_string[program->action],
            program->value,
            program->value2
            );
  }
  else if (program->action == ETCHER_ACTION_STATE_SET)
  {
    fprintf(out, "\t\t\t\taction: %s \"%s\" %.2f;\n",
            _action_string[program->action],
            program->state ? program->state : "",
            program->value
            );
  }
  else if (program->action == ETCHER_ACTION_SIGNAL_EMIT)
  {
    fprintf(out, "\t\t\t\taction: %s \"%s\" \"%s\";\n",
            _action_string[program->action],
            program->state,
            program->state2
            );
  }

  if(program->action == ETCHER_ACTION_STATE_SET) 
    fprintf(out, "\t\t\t\ttransition: %s %.2f\n",
            _transition_string[program->transition],
            program->duration
            );

  if (program->in.from || program->in.range)
    fprintf(out, "\t\t\t\tin: %.2f %.2f\n",
            program->in.from,
            program->in.range
            );

  for (l = program->targets; l; l = l->next)
  {
    fprintf(out, "\t\t\t\ttarget: \"%s\"\n", (char *)l->data);
  }

  for (l = program->afters; l; l = l->next)
  {
    fprintf(out, "\t\t\t\tafter: \"%s\"\n", (char *)l->data);
  }
  fprintf(out, "\t\t\t}\n");
}


void
_etcher_output_state(Etcher_Part_State *state, FILE *out)
{
  /* FIXME: fille me in */
  
}
