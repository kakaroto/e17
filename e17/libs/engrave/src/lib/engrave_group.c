#include <Engrave.h>

/**
 * engrave_group_new - create a new Engrave_Group object.
 * 
 * @return Returns a pointer to a newly allocated Engrave_Group object on success or
 * NULL on failure.
 */
Engrave_Group *
engrave_group_new(void)
{
  Engrave_Group *group;
  group = NEW(Engrave_Group, 1);
  if (!group) return NULL;

  /* defaults */
  group->max.w = -1;
  group->max.h = -1;
  return group;
}

/**
 * engrave_group_data_add - add the Engrave_Data to the group
 * @param eg: The Engrave_Group to add the data too.
 * @param ed: The Engrave_Data to add to the group.
 * 
 * @return Returns no value.
 */
void
engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed)
{
  if (!eg || !ed) return;
  eg->data = evas_list_append(eg->data, ed);
}

/**
 * engrave_group_script_set - attach the script to the given group
 * @param eg: The Engrave_Group to attach the script too.
 * @param script: The script to attach to the group.
 *
 * @return Returns no value.
 */
void
engrave_group_script_set(Engrave_Group *eg, char *script)
{
  if (!eg) return;
  IF_FREE(eg->script);
  eg->script = (script ? strdup(script) : NULL); 
}

/**
 * engrave_group_name_set - set the name of the group to the given name.
 * @param eg: The Engrave_Group to attach the name too.
 * @param name: The name to attach to the group.
 *
 * @return Returns no value.
 */
void
engrave_group_name_set(Engrave_Group *eg, char *name)
{
  if (!eg) return;
  IF_FREE(eg->name);
  eg->name = (name ? strdup(name) : NULL);
}

/**
 * engrave_group_min_size_set - set the min size of the group.
 * @param eg: The Engrave_Group on which to set the min size.
 * @param w: The min width to set on the group.
 * @param h: The min height to set on the group.
 *
 * @return Returns no value.
 */
void
engrave_group_min_size_set(Engrave_Group *eg, int w, int h)
{
  if (!eg) return;
  eg->min.w = w;
  eg->min.h = h;
}

/**
 * engrave_group_max_size_set - set the max size of the group.
 * @param eg: The Engrave_Group on which to set the max size.
 * @param w: The max width to set on the group.
 * @param h: The max height to set on the group.
 *
 * @return Returns no value.
 */
void
engrave_group_max_size_set(Engrave_Group *eg, int w, int h)
{
  if (!eg) return;
  eg->max.w = w;
  eg->max.h = h;
}

/**
 * engrave_group_part_add - add the given part to the group
 * @param eg: The Engrave_Group to attach the part too.
 * @param ep: The Engrave_Part to add too the group.
 *
 * @return Returns no value.
 */
void
engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep) 
{
  if (!eg || !ep) return;
  eg->parts = evas_list_append(eg->parts, ep);
}

/**
 * engrave_group_program_add - add the program to the group
 * @param eg: The Engrave_Group to add the program too.
 * @param ep: The Engrave_Program to add to the group.
 *
 * @return Returns no value.
 */
void
engrave_group_program_add(Engrave_Group *eg, Engrave_Program *ep)
{
  eg->programs = evas_list_append(eg->programs, ep);
}

/**
 * engrave_group_part_last_get - retrieve the last part in the group.
 * @param eg: The Engrave_Group to retrieve the last part from.
 *
 * @return Returns the last Engrave_Part in the group or NULL if no such
 * part exists.
 */
Engrave_Part *
engrave_group_part_last_get(Engrave_Group *eg)
{
  if (!eg) return;
  return evas_list_data(evas_list_last(eg->parts));
}

/**
 * engrave_group_program_last_get - retrieve the last program in the group.
 * @param eg: The Engrave_Group to retrieve the last program from.
 *
 * @return Returns the last Engrave_Program in the group or NULL if no such
 * program exists.
 */
Engrave_Program *
engrave_group_program_last_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return evas_list_data(evas_list_last(eg->programs));
}


