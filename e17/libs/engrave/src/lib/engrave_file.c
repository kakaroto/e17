#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_file_new - create a new Engrave_File object
 *
 * @return Returns a pointer to a newly allocated file object on success, NULL on
 * failure.
 */
Engrave_File *
engrave_file_new(void)
{
  Engrave_File *ef;
  ef = NEW(Engrave_File, 1);
  return ef;
}

/**
 * engrave_file_free - free the file object
 * @param ef: The Engrave_File to free
 *
 * @return Returns no value.
 */
void
engrave_file_free(Engrave_File *ef)
{
  Evas_List *l;
  if (!ef) return;

  IF_FREE(ef->im_dir);
  IF_FREE(ef->font_dir);

  for (l = ef->images; l; l = l->next) {
    Engrave_Image *ei = l->data;
    ef->images = evas_list_remove(ef->images, ei);
    engrave_image_free(ei);
  }
  ef->images = evas_list_free(ef->images);

  for (l = ef->fonts; l; l = l->next) {
    Engrave_Font *font = l->data;
    ef->fonts = evas_list_remove(ef->fonts, font);
    engrave_font_free(font);
  }
  ef->fonts = evas_list_free(ef->fonts);

  for (l = ef->data; l; l = l->next) {
    Engrave_Data *ed = l->data;
    ef->data = evas_list_remove(ef->data, ed);
    engrave_data_free(ed);
  }
  ef->data = evas_list_free(ef->data);

  for (l = ef->groups; l; l = l->next) {
    Engrave_Group *eg = l->data;
    ef->groups = evas_list_remove(ef->groups, eg);
    engrave_group_free(eg);
  }
  ef->groups = evas_list_free(ef->groups);

  FREE(ef);
}

/**
 * engrave_file_image_dir_set - set the image directory for the file
 * @param ef: The Engrave_File to set the image dir into
 * @param dir: The directory to set
 *
 * @return Returns no value.
 */
void
engrave_file_image_dir_set(Engrave_File *ef, const char *dir)
{
  if (!ef || !dir) return;
  IF_FREE(ef->im_dir);
  ef->im_dir = strdup(dir);
}

/**
 * engrave_file_font_dir_set - set the font dir for the file
 * @param ef: The Engrave_File to set the font dir on
 * @param dir: The directory to set the font dir too
 *
 * @return Returns no value.
 */
void
engrave_file_font_dir_set(Engrave_File *ef, const char *dir)
{
  if (!ef || !dir) return;
  IF_FREE(ef->font_dir);
  ef->font_dir = strdup(dir);
}

/**
 * engrave_file_image_dir_get - get the image directory
 * @param ef: The Engrave_File to get the directory from
 *
 * @return Returns the current image directory or NULL on failure
 */
const char *
engrave_file_image_dir_get(Engrave_File *ef)
{
  return (ef ? ef->im_dir : NULL);
}

/**
 * engrave_file_font_dir_get - get the font directory
 * @param ef: The Engrave_File to get the directory from
 * 
 * @return Returns the current font directory
 */
const char *
engrave_file_font_dir_get(Engrave_File *ef)
{
  return (ef ? ef->font_dir : NULL);
}

/**
 * engrave_file_font_add - add the font to the engrave file.
 * @param e: The Engrave_File to add the font too.
 * @param ef: The Engrave_Font to add to the file.
 *
 * @return Returns no value.
 */
void
engrave_file_font_add(Engrave_File *e, Engrave_Font *ef)
{
  if (!e || !ef) return;
  e->fonts = evas_list_append(e->fonts, ef);
}

/**
 * engrave_file_image_add - add the image to the engrave file.
 * @param ef: The Engrave_File to add the image too.
 * @param ei: The Engrave_Image to add to the file.
 *
 * @return Returns no value.
 */
void
engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei)
{
  if (!ef || !ei) return;
  ef->images = evas_list_append(ef->images, ei);
}

/**
 * engrave_file_data_add - add the data to the engrave file.
 * @param ef: The Engrave_File to add the data too.
 * @param ed: The Engrave_Data to add to the file.
 *
 * @return Returns no value.
 */
void
engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed)
{
  if (!ef || !ed) return;
  ef->data = evas_list_append(ef->data, ed);
}

/**
 * engrave_file_group_add - add the group to the given file
 * @param ef: The Engrave_File to add the group too.
 * @param eg: The Engrave_Group to add to the file.
 * 
 * @return Returns no value.
 */
void
engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg)
{
  if (!ef || !eg) return;
  ef->groups = evas_list_append(ef->groups, eg);
}

/**
 * engrave_file_group_last_get - returns the last group in the file
 * @param ef: The Engrave_File from which to retrieve the group
 *
 * @return Returns the last Engrave_Group in the engrave file @a ef or NULL
 * if there are no available groups.
 */
Engrave_Group *
engrave_file_group_last_get(Engrave_File *ef)
{
  return evas_list_data(evas_list_last(ef->groups));
}

/**
 * engrave_file_image_by_name_find - returns the Engrave_Image with the given name.
 * @param ef: The Engrave_File to search for the image in.
 * @param name: The name of the image to search for.
 *
 * @return Returns the Engrave_Image with the given @a name or NULL if no
 * corresponding image can be found.
 */
Engrave_Image *
engrave_file_image_by_name_find(Engrave_File *ef, const char *name)
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

/**
 * engrave_file_images_count - get the number of images
 * @param ef: The Engrave_File to check for images
 * 
 * @return Returns the number of images, 0 otherwise
 */
int
engrave_file_images_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->images);
}

/**
 * engrave_file_data_count - count data blocks in file
 * @param ef: The Engrave_File to check for data
 * 
 * @return Returns number of data blocks, 0 otherwise
 */
int
engrave_file_data_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->data);
}

/**
 * engrave_file_groups_count - count the groups in the file
 * @param ef: The Engrave_File to check for groups
 * 
 * @return Returns then number of groups, 0 otherwise
 */
int
engrave_file_groups_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->groups);
}

/**
 * engrave_file_fonts_count - count the fonts in the file
 * @param ef: The Engrave_File to check for fonts
 * 
 * @return Returns the number of fonts in the file, 0 otherwise
 */
int
engrave_file_fonts_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->fonts);
}

/**
 * engrave_file_image_foreach - call the given function for each image object
 * @param ef: The Engrave_File for which the images should be iterated over
 * @param func: The function to call for each image
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
void
engrave_file_image_foreach(Engrave_File *ef, 
                      void (*func)(Engrave_Image *, void *data),
                      void *data)
{
  Evas_List *l;

  if (!engrave_file_images_count(ef)) return;
  for (l = ef->images; l; l = l->next) {
    Engrave_Image *image = l->data;
    if (image) func(image, data);
  }
}

/**
 * engrave_file_data_foreach - call the given function for each data object
 * @param ef: The Engrave_File for which the data should be iterated over
 * @param func: The function to call for each piece of data
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
void
engrave_file_data_foreach(Engrave_File *ef, 
                        void (*func)(Engrave_Data *, void *data),
                        void *udata)
{
  Evas_List *l;

  if (!engrave_file_data_count(ef)) return;
  for (l = ef->data; l; l = l->next) {
    Engrave_Data *data = l->data;
    if (data) func(data, udata);
  }
}

/**
 * engrave_file_group_foreach - call the given function for each group object
 * @param ef: The Engrave_File for which the groups should be iterated over
 * @param func: The function to call for each group
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
void
engrave_file_group_foreach(Engrave_File *ef, 
                          void (*func)(Engrave_Group *, void *data),
                          void *data)
{
  Evas_List *l;

  if (!engrave_file_groups_count(ef)) return;
  for (l = ef->groups; l; l = l->next) {
    Engrave_Group *group = l->data;
    if (group) func(group, data);
  }
}

/**
 * engrave_file_font_foreach - call the given function for each font object
 * @param ef: The Engrave_File for which the fonts should be iterated over
 * @param func: The function to call for each font
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
void
engrave_file_font_foreach(Engrave_File *ef, 
                            void (*func)(Engrave_Font *, void *data), 
                            void *data)
{
  Evas_List *l;

  if (!engrave_file_fonts_count(ef)) return;
  for (l = ef->fonts; l; l = l->next) {
    Engrave_Font *font = l->data;
    if (font) func(font, data);
  }
}


