#include <Engrave.h>

Engrave_Group *
engrave_group_new(void)
{
  Engrave_Group *group;
  group = NEW(Engrave_Group, 1);

  /* defaults */
  group->max.w = -1;
  group->max.h = -1;
  return group;
}

void
engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed)
{
  if (!eg || !ed) return;
  eg->data = evas_list_append(eg->data, ed);
}

void
engrave_group_script_set(Engrave_Group *eg, char *script)
{
  if (!eg) return;
  if (eg->script) free(eg->script);
  eg->script = (script ? strdup(script) : NULL); 
}

void
engrave_group_name_set(Engrave_Group *eg, char *name)
{
  if (!eg) return;
  if (eg->name) free(eg->name);
  eg->name = (name ? strdup(name) : NULL);
}

void
engrave_group_min_size_set(Engrave_Group *eg, int w, int h)
{
  if (!eg) return;
  eg->min.w = w;
  eg->min.h = h;
}

void
engrave_group_max_size_set(Engrave_Group *eg, int w, int h)
{
  if (!eg) return;
  eg->max.w = w;
  eg->max.h = h;
}

void
engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep) 
{
  if (!eg || !ep) return;
  eg->parts = evas_list_append(eg->parts, ep);
}

Engrave_Part *
engrave_group_part_last_get(Engrave_Group *eg)
{
  if (!eg) return;
  return evas_list_data(evas_list_last(eg->parts));
}

Engrave_Program *
engrave_group_program_last_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return evas_list_data(evas_list_last(eg->programs));
}


