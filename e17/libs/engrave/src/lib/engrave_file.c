#include <Engrave.h>

/**
 * @file engrave_file.h Engrave_File object functions.
 * @brief Contains all of the functions related to the Engrave file object
 */

/**
 * @defgroup Engrave_File Engrave_File: Functions to work with the Engrave file object
 *
 * @{
 */

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
 * @}
 */

