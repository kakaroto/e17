#ifndef ENGRAVE_FILE_H
#define ENGRAVE_FILE_H

typedef struct _Engrave_File Engrave_File;
struct _Engrave_File
{
  Evas_List *images;
  Evas_List *fonts;
  Evas_List *data;
  Evas_List *groups;  
};

Engrave_File *engrave_file_new(void);
void engrave_file_font_add(Engrave_File *e, Engrave_Font *ef);
void engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei);
void engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed);
void engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg);

Engrave_Group *engrave_file_group_last_get(Engrave_File *ef);

Engrave_Image * engrave_file_image_by_name_find(Engrave_File *ef,
                                                      char *name);

#endif

