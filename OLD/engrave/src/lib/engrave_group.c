#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_group_new - create a new Engrave_Group object.
 * 
 * @return Returns a pointer to a newly allocated Engrave_Group object on success or
 * NULL on failure.
 */
EAPI Engrave_Group *
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
 * engrave_group_free - free the group data
 * @param eg: The Engrave_Group to free
 *
 * @return Returns no value
 */
EAPI void
engrave_group_free(Engrave_Group *eg) 
{
  Eina_List *l;
  if (!eg) return;

  IF_FREE(eg->name);
  IF_FREE(eg->alias);
  for (l = eg->parts; l; l = l->next) {
    Engrave_Part *ep = l->data;
    engrave_part_free(ep);
  }
  eg->parts = eina_list_free(eg->parts);

  for (l = eg->programs; l; l = l->next) {
    Engrave_Program *ep = l->data;
    engrave_program_free(ep);
  }
  eg->programs = eina_list_free(eg->programs);

  for (l = eg->data; l; l = l->next) {
    Engrave_Data *ed = l->data;
    engrave_data_free(ed);
  }
  eg->data = eina_list_free(eg->data);

  FREE(eg);
}

/**
 * engrave_group_data_add - add the Engrave_Data to the group
 * @param eg: The Engrave_Group to add the data too.
 * @param ed: The Engrave_Data to add to the group.
 * 
 * @return Returns no value.
 */
EAPI void
engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed)
{
  if (!eg || !ed) return;
  eg->data = eina_list_append(eg->data, ed);
  engrave_data_parent_set(ed, eg);
}

/**
 * engrave_group_script_set - attach the script to the given group
 * @param eg: The Engrave_Group to attach the script too.
 * @param script: The script to attach to the group.
 *
 * @return Returns no value.
 */
EAPI void
engrave_group_script_set(Engrave_Group *eg, const char *script)
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
EAPI void
engrave_group_name_set(Engrave_Group *eg, const char *name)
{
  if (!eg) return;
  IF_FREE(eg->name);
  eg->name = (name ? strdup(name) : NULL);
}

/**
 * engrave_group_alias_set - set the alias of the group.
 * @param eg: The Engrave_Group to attach the alias too.
 * @param alias: The alias to attach to the group.
 *
 * @return Returns no value.
 */
EAPI void
engrave_group_alias_set(Engrave_Group *eg, const char *alias)
{
  if (!eg) return;
  IF_FREE(eg->alias);
  eg->alias = (alias ? strdup(alias) : NULL);
}

/**
 * engrave_group_min_size_set - set the min size of the group.
 * @param eg: The Engrave_Group on which to set the min size.
 * @param w: The min width to set on the group.
 * @param h: The min height to set on the group.
 *
 * @return Returns no value.
 */
EAPI void
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
EAPI void
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
EAPI void
engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep) 
{
  if (!eg || !ep) return;
  eg->parts = eina_list_append(eg->parts, ep);
  engrave_part_parent_set(ep, eg);
}

/**
 * engrave_group_program_add - add the program to the group
 * @param eg: The Engrave_Group to add the program too.
 * @param ep: The Engrave_Program to add to the group.
 *
 * @return Returns no value.
 */
EAPI void
engrave_group_program_add(Engrave_Group *eg, Engrave_Program *ep)
{
  eg->programs = eina_list_append(eg->programs, ep);
  engrave_program_parent_set(ep, eg);
}

/**
 * engrave_group_part_last_get - retrieve the last part in the group.
 * @param eg: The Engrave_Group to retrieve the last part from.
 *
 * @return Returns the last Engrave_Part in the group or NULL if no such
 * part exists.
 */
EAPI Engrave_Part *
engrave_group_part_last_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return eina_list_data_get(eina_list_last(eg->parts));
}

/**
 * engrave_group_program_last_get - retrieve the last program in the group.
 * @param eg: The Engrave_Group to retrieve the last program from.
 *
 * @return Returns the last Engrave_Program in the group or NULL if no such
 * program exists.
 */
EAPI Engrave_Program *
engrave_group_program_last_get(Engrave_Group *eg)
{
  if (!eg) return NULL;
  return eina_list_data_get(eina_list_last(eg->programs));
}

/**
 * engrave_group_data_count - count the data blocks in the group
 * @param eg: The Engrave_Group to check if there is data
 * 
 * @return Returns number of data blocks, 0 otherwise.
 */
EAPI int
engrave_group_data_count(Engrave_Group *eg)
{
  if (!eg) return 0;
  return eina_list_count(eg->data);
}

/**
 * engrave_group_parts_count - returns the number of parts in the group
 * @param eg: The Engrave_Group to check if there are parts
 * 
 * @return Returns the number of parts, 0 otherwise.
 */
EAPI int
engrave_group_parts_count(Engrave_Group *eg)
{
  if (!eg) return 0;
  return eina_list_count(eg->parts);
}

/**
 * engrave_group_programs_count - get number of programs in the group
 * @param eg: The Engrave_Group to check if there are programs
 * 
 * @return Returns number of programs, 0 otherwise.
 */
EAPI int
engrave_group_programs_count(Engrave_Group *eg)
{
  if (!eg) return 0;
  return eina_list_count(eg->programs);
}

/**
 * engrave_group_data_foreach - interate over the data in the group
 * @param eg: The Engrave_Group to iterate over the data
 * @param func: The function to call on each data block
 * @param data: any user data to pass to the function
 *
 * @return Returns no value.
 */
EAPI void
engrave_group_data_foreach(Engrave_Group *eg, 
                            void (*func)(Engrave_Data *, void *), void *data)
{
  Eina_List *l;

  if (!engrave_group_data_count(eg)) return;
  for (l = eg->data; l; l = l->next) {
    Engrave_Data *d = l->data;
    if (d) func(d, data);
  }
}

/**
 * engrave_group_name_get - get the name attached to the group
 * @param eg: The Engrave_Group to get the name from
 *
 * @return Returns the name of the group or NULL on failure.
 */
EAPI const char *
engrave_group_name_get(Engrave_Group *eg)
{
  return (eg ? eg->name : NULL);
}

/**
 * engrave_group_alias_get - get the alias attached to the group
 * @param eg: The Engrave_Group to get the alias from
 *
 * @return Returns the alias of the group or NULL on failure.
 */
EAPI const char *
engrave_group_alias_get(Engrave_Group *eg)
{
  return (eg ? eg->alias : NULL);
}

/**
 * engrave_group_script_get - get the script attached to the group
 * @param eg: The Engrave_Group to get the script from
 * 
 * @return Returns the script on success or NULL on failure.
 */
EAPI const char *
engrave_group_script_get(Engrave_Group *eg)
{
  return (eg ? eg->script : NULL);
}

/**
 * engrave_group_min_size_get - get the min size of the group
 * @param eg: The Engrave_Group to get the min size from
 * @param w: Will be set to the min width of the group
 * @param h: Will be set to the min height of the group
 *
 * @return Returns no value.
 */
EAPI void
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
EAPI void
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
EAPI void
engrave_group_parts_foreach(Engrave_Group *eg, 
                             void (*func)(Engrave_Part *, void *), void *data)
{
  Eina_List *l;

  if (!engrave_group_parts_count(eg)) return;
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
EAPI void
engrave_group_programs_foreach(Engrave_Group *eg, 
                        void (*func)(Engrave_Program *, void *), void *data)
{
  Eina_List *l;

  if (!engrave_group_programs_count(eg)) return;
  for (l = eg->programs; l; l = l->next) {
    Engrave_Program *p = l->data;
    if (p) func(p, data);
  }
}

/**
 * engrave_group_data_by_key_find - find the Engrave_Data by key
 * @param eg: The Engrave_Group to search
 * @param key: They key to search for
 *
 * @return Returns the Engrave_Data with the matching key or NULL if no such
 * data exists.
 */
EAPI Engrave_Data *
engrave_group_data_by_key_find(Engrave_Group *eg, const char *key)
{
    Eina_List *l;

    if (!eg || !key) return NULL;
    for (l = eg->data; l; l = l->next) {
        Engrave_Data *ed = l->data;
        const char *data_key = engrave_data_key_get(ed);

        if (!strcmp(key, data_key))
            return ed;
    }
    return NULL;
}

/**
 * engrave_group_part_by_name_find - find the Engrave_Part by name
 * @param eg: The Engrave_Group to search
 * @param part: They part name to search for
 *
 * @return Returns the Engrave_Part with the matching name or NULL if no such
 * name exists.
 */ 
EAPI Engrave_Part *
engrave_group_part_by_name_find(Engrave_Group *eg, const char *part)
{
    Eina_List *l;

    if (!eg || !part) return NULL;
    for (l = eg->parts; l; l = l->next) {
        Engrave_Part *ep = l->data;
        const char *name = engrave_part_name_get(ep);

        if (!strcmp(part, name))
            return ep;
    }
    return NULL;
}

/**
 * engrave_group_parent_set - set the parent pointer
 * @param eg: The Engrave_Group to set the parent pointer into
 * @param ef: The Engrave_File to set as the parent
 * 
 * @return Returns no value.
 */
EAPI void
engrave_group_parent_set(Engrave_Group *eg, void *ef)
{
    if (!eg) return;
    eg->parent = ef;
}

/**
 * engrave_group_parent_get - get the parent pointer
 * @param eg: The Engrave_Group to get the parent pointer from
 * 
 * @return Returns the Engrave_File parent pointer or NULL if none set
 */
EAPI void *
engrave_group_parent_get(Engrave_Group *eg)
{
    return (eg ? eg->parent : NULL);
}





