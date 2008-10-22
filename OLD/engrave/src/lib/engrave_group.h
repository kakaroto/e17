#ifndef ENGRAVE_GROUP_H
#define ENGRAVE_GROUP_H

/**
 * @file engrave_group.h Engrave_Group functions
 * @brief Contains all of the functions to maniuplate Engrave_Group objects.
 */

/**
 * @defgroup Engrave_Group Engrave_Group: Functions to work with an Engrave_Group.
 *
 * @{
 */

/**
 * The Engrave_Group typedef
 */
typedef struct _Engrave_Group Engrave_Group;

/**
 * @brief Stores the needed group information.
 */
struct _Engrave_Group
{
  char *name;           /**< The group name */
  char *alias;           /**< The group alias */
  /**
   * The max/min size values for the group 
   */
  struct
  {
    int w;          /**< The width */
    int h;          /**< The height */
  } min, max;

  Eina_List *parts;     /**< The list of parts in this group */
  Eina_List *programs;  /**< The list of programs in this group */
  Eina_List *data;      /**< The list of data used in this group */

  char *script;         /**< The script attached to this group */

  void *parent; /**< Pointer to parent */
};

EAPI Engrave_Group *engrave_group_new(void);
EAPI void engrave_group_free(Engrave_Group *eg);

EAPI void engrave_group_parent_set(Engrave_Group *eg, void *ef);
EAPI void *engrave_group_parent_get(Engrave_Group *eg);

EAPI void engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed);
EAPI void engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep);
EAPI void engrave_group_program_add(Engrave_Group *eg, Engrave_Program *ep);

EAPI void engrave_group_script_set(Engrave_Group *eg, const char *script);
EAPI void engrave_group_name_set(Engrave_Group *eg, const char *name);
EAPI void engrave_group_alias_set(Engrave_Group *eg, const char *alias);
EAPI void engrave_group_min_size_set(Engrave_Group *eg, int w, int h);
EAPI void engrave_group_max_size_set(Engrave_Group *eg, int w, int h);

EAPI Engrave_Part * engrave_group_part_last_get(Engrave_Group *eg);
EAPI Engrave_Program * engrave_group_program_last_get(Engrave_Group *eg);

EAPI const char *engrave_group_name_get(Engrave_Group *eg);
EAPI const char *engrave_group_alias_get(Engrave_Group *eg);
EAPI const char *engrave_group_script_get(Engrave_Group *eg);
EAPI void engrave_group_min_size_get(Engrave_Group *eg, int *w, int *h);
EAPI void engrave_group_max_size_get(Engrave_Group *eg, int *w, int *h);

EAPI int engrave_group_data_count(Engrave_Group *eg);
EAPI int engrave_group_parts_count(Engrave_Group *eg);
EAPI int engrave_group_programs_count(Engrave_Group *eg);

EAPI void engrave_group_data_foreach(Engrave_Group *eg, 
                                     void (*func)(Engrave_Data *, void *), void *data);
EAPI void engrave_group_parts_foreach(Engrave_Group *eg, 
                                      void (*func)(Engrave_Part *, void *), void *data);
EAPI void engrave_group_programs_foreach(Engrave_Group *eg, 
                                         void (*func)(Engrave_Program *, void *), void *data);

EAPI Engrave_Part *engrave_group_part_by_name_find(Engrave_Group *eg, 
                                                   const char *part);
EAPI Engrave_Data *engrave_group_data_by_key_find(Engrave_Group *eg, 
                                                  const char *key);

/**
 * @}
 */

#endif

