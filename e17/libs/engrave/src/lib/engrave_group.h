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
 * Stores the needed group information.
 */
struct _Engrave_Group
{
  char *name;           /**< The group name */
  /**
   * The max/min size values for the group 
   */
  struct
  {
    int w;          /**< The width */
    int h;          /**< The height */
  } min, max;

  Evas_List *parts;     /**< The list of parts in this group */
  Evas_List *programs;  /**< The list of programs in this group */
  Evas_List *data;      /**< The list of data used in this group */

  char *script;         /**< The script attached to this group */
};

Engrave_Group *engrave_group_new(void);
void engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed);
void engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep);
void engrave_group_program_add(Engrave_Group *eg, Engrave_Program *ep);

void engrave_group_script_set(Engrave_Group *eg, char *script);
void engrave_group_name_set(Engrave_Group *eg, char *name);
void engrave_group_min_size_set(Engrave_Group *eg, int w, int h);
void engrave_group_max_size_set(Engrave_Group *eg, int w, int h);

Engrave_Part * engrave_group_part_last_get(Engrave_Group *eg);
Engrave_Program * engrave_group_program_last_get(Engrave_Group *eg);

/**
 * @}
 */

#endif

