#ifndef ENGRAVE_GROUP_H
#define ENGRAVE_GROUP_H

typedef struct _Engrave_Group Engrave_Group;
struct _Engrave_Group
{
  char *name;
  struct
  {
    int w, h;
  } min, max;

  Evas_List *parts;
  Evas_List *programs;
  Evas_List *data;

  char *script;
};

Engrave_Group * engrave_group_new(void);
void engrave_group_data_add(Engrave_Group *eg, Engrave_Data *ed);
void engrave_group_part_add(Engrave_Group *eg, Engrave_Part *ep);
void engrave_group_script_set(Engrave_Group *eg, char *script);
void engrave_group_name_set(Engrave_Group *eg, char *name);
void engrave_group_min_size_set(Engrave_Group *eg, int w, int h);
void engrave_group_max_size_set(Engrave_Group *eg, int w, int h);

Engrave_Part * engrave_group_part_last_get(Engrave_Group *eg);
Engrave_Program * engrave_group_program_last_get(Engrave_Group *eg);

#endif

