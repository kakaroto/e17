#include <Engrave.h>

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

/**
 * engrave_file_has_images - check if the file has images
 * @param ef: The Engrave_File to check for images
 * 
 * @return Returns 1 if the file contains images, 0 otherwise
 */
int
engrave_file_has_images(Engrave_File *ef)
{
  if (!ef) return 0;
  if (evas_list_count(ef->images) > 0) return 1;
  return 0;
}

/**
 * engrave_file_has_images - check if the file has data
 * @param ef: The Engrave_File to check for data
 * 
 * @return Returns 1 if the file contains data, 0 otherwise
 */
int
engrave_file_has_data(Engrave_File *ef)
{
  if (!ef) return 0;
  if (evas_list_count(ef->data) > 0) return 1;
  return 0;
}

/**
 * engrave_file_has_images - check if the file has groups
 * @param ef: The Engrave_File to check for groups
 * 
 * @return Returns 1 if the file contains groups, 0 otherwise
 */
int
engrave_file_has_groups(Engrave_File *ef)
{
  if (!ef) return 0;
  if (evas_list_count(ef->groups) > 0) return 1;
  return 0;
}

/**
 * engrave_file_has_images - check if the file has fonts
 * @param ef: The Engrave_File to check for fonts
 * 
 * @return Returns 1 if the file contains fonts, 0 otherwise
 */
int
engrave_file_has_fonts(Engrave_File *ef)
{
  if (!ef) return 0;
  if (evas_list_count(ef->fonts) > 0) return 1;
  return 0;
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

  if (!engrave_file_has_images(ef)) return;
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

  if (!engrave_file_has_data(ef)) return;
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

  if (!engrave_file_has_groups(ef)) return;
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

  if (!engrave_file_has_fonts(ef)) return;
  for (l = ef->fonts; l; l = l->next) {
    Engrave_Font *font = l->data;
    if (font) func(font, data);
  }
}


