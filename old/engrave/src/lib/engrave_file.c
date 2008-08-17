#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_file_new - create a new Engrave_File object
 *
 * @return Returns a pointer to a newly allocated file object on success, NULL on
 * failure.
 */
EAPI Engrave_File *
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
EAPI void
engrave_file_free(Engrave_File *ef)
{
  Evas_List *l;
  if (!ef) return;

  IF_FREE(ef->im_dir);
  IF_FREE(ef->font_dir);

  for (l = ef->images; l; l = l->next) {
    Engrave_Image *ei = l->data;
    engrave_image_free(ei);
  }
  ef->images = evas_list_free(ef->images);

  for (l = ef->fonts; l; l = l->next) {
    Engrave_Font *font = l->data;
    engrave_font_free(font);
  }
  ef->fonts = evas_list_free(ef->fonts);

  for (l = ef->data; l; l = l->next) {
    Engrave_Data *ed = l->data;
    engrave_data_free(ed);
  }
  ef->data = evas_list_free(ef->data);

  for (l = ef->groups; l; l = l->next) {
    Engrave_Group *eg = l->data;
    engrave_group_free(eg);
  }
  ef->groups = evas_list_free(ef->groups);

  for (l = ef->spectra; l; l = l->next) {
    Engrave_Spectrum *es = l->data;
    engrave_spectrum_free(es);
  }
  ef->spectra = evas_list_free(ef->spectra);

  for (l = ef->color_classes; l; l = l->next) {
    Engrave_Color_Class *ecc = l->data;
    engrave_color_class_free(ecc);
  }
  ef->color_classes = evas_list_free(ef->color_classes);

  FREE(ef);
}

/**
 * engrave_file_image_dir_set - set the image directory for the file
 * @param ef: The Engrave_File to set the image dir into
 * @param dir: The directory to set
 *
 * @return Returns no value.
 */
EAPI void
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
EAPI void
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
EAPI const char *
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
EAPI const char *
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
EAPI void
engrave_file_font_add(Engrave_File *e, Engrave_Font *ef)
{
  if (!e || !ef) return;
  e->fonts = evas_list_append(e->fonts, ef);
  engrave_font_parent_set(ef, e);
}

/**
 * engrave_file_spectrum_add - add the spectrum to the engrave file.
 * @param e: The Engrave_File to add the spectrum too.
 * @param es: The Engrave_Spectrum to add to the file.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_spectrum_add(Engrave_File *e, Engrave_Spectrum *es)
{
  if (!e || !es) return;
  e->spectra = evas_list_append(e->spectra, es);
  engrave_spectrum_parent_set(es, e);
}

/**
 * engrave_file_color_class_add - add the color_class to the engrave file.
 * @param e: The Engrave_File to add the color_class too.
 * @param ecc: The Engrave_Color_Class to add to the file.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_color_class_add(Engrave_File *e, Engrave_Color_Class *ecc)
{
  if (!e || !ecc) return;
  e->color_classes = evas_list_append(e->color_classes, ecc);
  engrave_color_class_parent_set(ecc, e);
}

/**
 * engrave_file_style_add - add the style to the engrave file.
 * @param e: The Engrave_File to add the style too.
 * @param es: The Engrave_Style to add to the file.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_style_add(Engrave_File *e, Engrave_Style *es)
{
  if (!e || !es) return;
  e->styles = evas_list_append(e->styles, es);
  engrave_style_parent_set(es, e);
}

/**
 * engrave_file_image_add - add the image to the engrave file.
 * @param ef: The Engrave_File to add the image too.
 * @param ei: The Engrave_Image to add to the file.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei)
{
  if (!ef || !ei) return;
  ef->images = evas_list_append(ef->images, ei);
  engrave_image_parent_set(ei, ef);
}

/**
 * engrave_file_data_add - add the data to the engrave file.
 * @param ef: The Engrave_File to add the data too.
 * @param ed: The Engrave_Data to add to the file.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed)
{
  if (!ef || !ed) return;
  ef->data = evas_list_append(ef->data, ed);
  engrave_data_parent_set(ed, ef);
}

/**
 * engrave_file_group_add - add the group to the given file
 * @param ef: The Engrave_File to add the group too.
 * @param eg: The Engrave_Group to add to the file.
 * 
 * @return Returns no value.
 */
EAPI void
engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg)
{
  if (!ef || !eg) return;
  ef->groups = evas_list_append(ef->groups, eg);
  engrave_group_parent_set(eg, ef);
}

/**
 * engrave_file_spectrum_last_get - returns the last spectrum in the file
 * @param ef: The Engrave_File from which to retrieve the group
 *
 * @return Returns the last Engrave_Spectrum in the engrave file @a ef or NULL
 */
EAPI Engrave_Spectrum *
engrave_file_spectrum_last_get(Engrave_File *ef)
{
  if (!ef) return NULL;
  return evas_list_data(evas_list_last(ef->spectra));
}

/**
 * engrave_file_color_class_last_get - returns the last color_class in the file
 * @param ef: The Engrave_File from which to retrieve the color_class
 *
 * @return Returns the last Engrave_Color_Class in the engrave file @a ef or NULL
 */
EAPI Engrave_Color_Class *
engrave_file_color_class_last_get(Engrave_File *ef)
{
  if (!ef) return NULL;
  return evas_list_data(evas_list_last(ef->color_classes));
}


/**
 * engrave_file_style_last_get - returns the last style in the file
 * @param ef: The Engrave_File from which to retrieve the group
 *
 * @return Returns the last Engrave_Style in the engrave file @a ef or NULL
 * if there are no available groups.
 */
EAPI Engrave_Style *
engrave_file_style_last_get(Engrave_File *ef)
{
  if (!ef) return NULL;
  return evas_list_data(evas_list_last(ef->styles));
}

/**
 * engrave_file_group_last_get - returns the last group in the file
 * @param ef: The Engrave_File from which to retrieve the group
 *
 * @return Returns the last Engrave_Group in the engrave file @a ef or NULL
 * if there are no available groups.
 */
EAPI Engrave_Group *
engrave_file_group_last_get(Engrave_File *ef)
{
  if (!ef) return NULL;
  return evas_list_data(evas_list_last(ef->groups));
}

/**
 * engrave_file_group_by_name_find - returns the Engrave_Group with the given name.
 * @param ef: The Engrave_File to search for the group in.
 * @param name: The name of the group to search for.
 *
 * @return Returns the Engrave_Group with the given @a name or NULL if no
 * corresponding group can be found.
 */
EAPI Engrave_Group *
engrave_file_group_by_name_find(Engrave_File *ef, const char *name)
{
  Evas_List *l;
  for (l = ef->groups; l; l = l->next)
  {
    Engrave_Group *eg = l->data;
    if (eg && !strcmp(engrave_group_name_get(eg), name))
      return eg;
  }
  return NULL;
}

/**
 * engrave_file_image_by_name_find - returns the Engrave_Image with the given name.
 * @param ef: The Engrave_File to search for the image in.
 * @param name: The name of the image to search for.
 *
 * @return Returns the Engrave_Image with the given @a name or NULL if no
 * corresponding image can be found.
 */
EAPI Engrave_Image *
engrave_file_image_by_name_find(Engrave_File *ef, const char *name)
{
  Evas_List *l;
  for (l = ef->images; l; l = l->next)
  {
    Engrave_Image *im = l->data;
    if (im && !strcmp(engrave_image_name_get(im), name))
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
EAPI int
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
EAPI int
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
EAPI int
engrave_file_groups_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->groups);
}

/**
 * engrave_file_styles_count - count the styles in the file
 * @param ef: The Engrave_File to check for styles
 * 
 * @return Returns the number of styles in the file, 0 otherwise
 */
EAPI int
engrave_file_styles_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->styles);
}

/**
 * engrave_file_fonts_count - count the fonts in the file
 * @param ef: The Engrave_File to check for fonts
 * 
 * @return Returns the number of fonts in the file, 0 otherwise
 */
EAPI int
engrave_file_fonts_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->fonts);
}

/**
 * engrave_file_spectra_count - count the spectra in the file
 * @param ef: The Engrave_File to check for spectra
 * 
 * @return Returns the number of spectra in the file, 0 otherwise
 */
EAPI int
engrave_file_spectra_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->spectra);
}

/**
 * engrave_file_color_classes_count - count the color_classes in the file
 * @param ef: The Engrave_File to check for color_classes
 * 
 * @return Returns the number of color_classes in the file, 0 otherwise
 */
EAPI int
engrave_file_color_classes_count(Engrave_File *ef)
{
  if (!ef) return 0;
  return evas_list_count(ef->color_classes);
}


/**
 * engrave_file_image_foreach - call the given function for each image object
 * @param ef: The Engrave_File for which the images should be iterated over
 * @param func: The function to call for each image
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
EAPI void
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
EAPI void
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
EAPI void
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
 * engrave_file_style_foreach - call the given function for each style object
 * @param ef: The Engrave_File for which the styles should be iterated over
 * @param func: The function to call for each style
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_style_foreach(Engrave_File *ef, 
                            void (*func)(Engrave_Style *, void *data), 
                            void *data)
{
  Evas_List *l;

  if (!engrave_file_styles_count(ef)) return;
  for (l = ef->styles; l; l = l->next) {
    Engrave_Style *style = l->data;
    if (style) func(style, data);
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
EAPI void
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

/**
 * engrave_file_spectrum_foreach - call the given function for each spectrum object
 * @param ef: The Engrave_File for which the spectra should be iterated over
 * @param func: The function to call for each spectrum
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_spectrum_foreach(Engrave_File *ef, 
                            void (*func)(Engrave_Spectrum *, void *data), 
                            void *data)
{
  Evas_List *l;

  if (!engrave_file_spectra_count(ef)) return;

  for (l = ef->spectra; l; l = l->next) {
    Engrave_Spectrum *es = l->data;
    if (es) func(es, data);
  }
}

/**
 * engrave_file_color_class_foreach - call the given function for each color_class object
 * @param ef: The Engrave_File for which the color_classes should be iterated over
 * @param func: The function to call for each color_class
 * @param data: Any user data to pass to the given function.
 *
 * @return Returns no value.
 */
EAPI void
engrave_file_color_class_foreach(Engrave_File *ef, 
                            void (*func)(Engrave_Color_Class *, void *data), 
                            void *data)
{
  Evas_List *l;

  if (!engrave_file_color_classes_count(ef)) return;

  for (l = ef->color_classes; l; l = l->next) {
    Engrave_Color_Class *ecc = l->data;
    if (ecc) func(ecc, data);
  }
}

/**
 * engrave_file_data_by_key_find - find the Engrave_Data by key
 * @param ef: The Engrave_File to search
 * @param key: They key to search for
 *
 * @return Returns the Engrave_Data with the matching key or NULL if no such
 * data exists.
 */
EAPI Engrave_Data *
engrave_file_data_by_key_find(Engrave_File *ef, const char *key)
{
    Evas_List *l;

    if (!ef || !key) return NULL;
    for (l = ef->data; l; l = l->next) {
        Engrave_Data *ed = l->data;
        const char *data_key = engrave_data_key_get(ed);

        if (!strcmp(key, data_key))
            return ed;
    }
    return NULL;
}

/**
 * engrave_file_font_by_name_find - find the Engrave_Font by name
 * @param ef: The Engrave_File to search
 * @param name: They name to search for
 *
 * @return Returns the Engrave_Font with the matching name or NULL if no such
 * font exists.
 */
EAPI Engrave_Font *
engrave_file_font_by_name_find(Engrave_File *ef, const char *name)
{
    Evas_List *l;

    if (!ef || !name) return NULL;
    for (l = ef->fonts; l; l = l->next) {
        Engrave_Font *ef = l->data;
        const char *font_name = engrave_font_name_get(ef);

        if (!strcmp(name, font_name))
            return ef;
    }
    return NULL;
}




