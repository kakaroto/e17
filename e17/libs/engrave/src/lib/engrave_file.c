#include <Engrave.h>

Engrave_File *
engrave_file_new(void)
{
  Engrave_File *ef;
  ef = NEW(Engrave_File, 1);
  return ef;
}

void
engrave_file_font_add(Engrave_File *e, Engrave_Font *ef)
{
  if (!e || !ef) return;
  e->fonts = evas_list_append(e->fonts, ef);
}

void
engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei)
{
  if (!ef || !ei) return;
  ef->images = evas_list_append(ef->images, ei);
}

void
engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed)
{
  if (!ef || !ed) return;
  ef->data = evas_list_append(ef->data, ed);
}

void
engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg)
{
  if (!ef || !eg) return;
  ef->groups = evas_list_append(ef->groups, eg);
}

Engrave_Group *
engrave_file_group_last_get(Engrave_File *ef)
{
  return evas_list_data(evas_list_last(ef->groups));
}

Engrave_Image *
engrave_file_image_by_name_find(Engrave_File *ef, char *name)
{
  Evas_List *l;
  for (l = ef->images; l; l = l->next)
  {
    Engrave_Image *im = l->data;
    if (im && !strcmp(im->name, name))
      return im;
  }
  return NULL;
}

