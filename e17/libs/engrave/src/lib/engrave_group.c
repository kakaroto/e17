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

/**
 * engrave_group_has_data - check if this group has a data block
 * @param eg: The Engrave_Group to check if there is data
 * 
 * @return Returns 1 if data exists, 0 otherwise.
 */
int
engrave_group_has_data(Engrave_Group *eg)
{
  if (!eg) return 0;
  if (evas_list_count(eg->data) > 0) return 1;
  return 0;
}

/**
 * engrave_group_has_parts - check if this group has part blocks
 * @param eg: The Engrave_Group to check if there are parts
 * 
 * @return Returns 1 if parts exist, 0 otherwise.
 */
int
engrave_group_has_parts(Engrave_Group *eg)
{
  if (!eg) return 0;
  if (evas_list_count(eg->parts) > 0) return 1;
  return 0;
}

/**
 * engrave_group_has_programs - check if this group has program blocks
 * @param eg: The Engrave_Group to check if there are programs
 * 
 * @return Returns 1 if programs exist, 0 otherwise.
 */
int
engrave_group_has_programs(Engrave_Group *eg)
{
  if (!eg) return 0;
  if (evas_list_count(eg->programs) > 0) return 1;
  return 0;
}

/**
 * engrave_group_data_foreach - interate over the data in the group
 * @param eg: The Engrave_Group to iterate over the data
 * @param func: The function to call on each data block
 * @param data: any user data to pass to the function
 *
 * @return Returns no value.
 */
void
engrave_group_data_foreach(Engrave_Group *eg, 
                            void (*func)(Engrave_Data *, void *), void *data)
{
  Evas_List *l;

  if (!engrave_group_has_data(eg)) return;
  for (l = eg->data; l; l = l->next) {
    Engrave_Data *d = l->data;
    if (d) func(d, data);
  }
}

/**
 * engrave_group_name_get - get the name attached to the group
 * @param eg: The Engrave_Group to get the name from
 *
 * @return Returns a pointer to the name of the group or NULL on failure.
 * This pointer must be free'd by the user
 */
char *
engrave_group_name_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return (eg->name ? strdup(eg->name) : NULL);
}

/**
 * engrave_group_script_get - get the script attached to the group
 * @param eg: The Engrave_Group to get the script from
 * 
 * @return Returns a pointer to the script on success or NULL on failure.
 * This pointer must be free'd by the user.
 */
char *
engrave_group_script_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return (eg->script ? strdup(eg->script) : NULL);
}

/**
 * engrave_group_min_size_get - get the min size of the group
 * @param eg: The Engrave_Group to get the min size from
 * @param w: Will be set to the min width of the group
 * @param h: Will be set to the min height of the group
 *
 * @return Returns no value.
 */
void
engrave_group_min_size_get(Engrave_Group *eg, int *w, int *h)
{
  int width, height;

  if (!eg) {
    width = 0;
    height = 0;
  } else {
    width = eg->min.w;
    height = eg->min.h;
  }
  if (w) *w = width;
  if (h) *h = height;
}

/**
 * engrave_group_max_size_get - get the max size of the group
 * @param eg: The Engrave_Group to get the max size from
 * @param w: Will be set to the max width of the group
 * @param h: Will be set to the max height of the group
 *
 * @return Returns no value.
 */
void
engrave_group_max_size_get(Engrave_Group *eg, int *w, int *h)
{
  int width, height;
  if (!eg) {
    width = 0;
    height = 0;
  } else {
    width = eg->max.w;
    height = eg->max.h;
  }
  if (w) *w = width;
  if (h) *h = height;
}

/**
 * engrave_group_parts_foreach - Iterate over the parts in the Engrave_group
 * @param eg: The Engrave_Group to get the parts from
 * @param func: the function to call for each part
 * @param data: The user data to pass to the function
 *
 * @return Returns no value.
 */
void
engrave_group_parts_foreach(Engrave_Group *eg, 
                             void (*func)(Engrave_Part *, void *), void *data)
{
  Evas_List *l;

  if (!engrave_group_has_parts(eg)) return;
  for (l = eg->parts; l; l = l->next) {
    Engrave_Part *p = l->data;
    if (p) func(p, data);
  }
}

/**
 * engrave_group_programs_foreach - Iterate over the groups programs
 * @param eg: The Engrave_Group to iterate over
 * @param func: The function to call for each program
 * @param data: User data
 *
 * @return Returns no value.
 */
void
engrave_group_programs_foreach(Engrave_Group *eg, 
                        void (*func)(Engrave_Program *, void *), void *data)
{
  Evas_List *l;

  if (!engrave_group_has_programs(eg)) return;
  for (l = eg->programs; l; l = l->next) {
    Engrave_Program *p = l->data;
    if (p) func(p, data);
  }
}


