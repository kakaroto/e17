#ifndef ENGRAVE_FILE_H
#define ENGRAVE_FILE_H

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
 * The Engrave_File typedef.
 */
typedef struct _Engrave_File Engrave_File;

/**
 * @brief Structure to hold the file information.
 */
struct _Engrave_File
{
  char *font_dir;    /**< The font directory */
  char *im_dir;      /**< The image directory */

  Evas_List *images; /**< The list of images in the file */
  Evas_List *fonts;  /**< The list of fonts in the file  */
  Evas_List *spectra;  /**< The list of spectra in the file  */
  Evas_List *color_classes;  /**< The list of color_classes in the file  */
  Evas_List *styles;  /**< The list of styles in the file  */
  Evas_List *data;   /**< The list of data items in the file */
  Evas_List *groups; /**< The list of groups in the file */
};

EAPI Engrave_File *engrave_file_new(void);
EAPI void engrave_file_free(Engrave_File *ef);

EAPI void engrave_file_image_dir_set(Engrave_File *ef, const char *dir);
EAPI void engrave_file_font_dir_set(Engrave_File *ef, const char *dir);

EAPI const char *engrave_file_image_dir_get(Engrave_File *ef);
EAPI const char *engrave_file_font_dir_get(Engrave_File *ef);

EAPI void engrave_file_font_add(Engrave_File *e, Engrave_Font *ef);
EAPI void engrave_file_spectrum_add(Engrave_File *e, Engrave_Spectrum *es);
EAPI void engrave_file_color_class_add(Engrave_File *e, Engrave_Color_Class *ecc);
EAPI void engrave_file_style_add(Engrave_File *e, Engrave_Style *es);
EAPI void engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei);
EAPI void engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed);
EAPI void engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg);

EAPI Engrave_Style *engrave_file_style_last_get(Engrave_File *ef);
EAPI Engrave_Spectrum *engrave_file_spectrum_last_get(Engrave_File *ef);
EAPI Engrave_Color_Class *engrave_file_color_class_last_get(Engrave_File *ef);

EAPI Engrave_Group *engrave_file_group_last_get(Engrave_File *ef);
EAPI Engrave_Group *engrave_file_group_by_name_find(Engrave_File *ef,
                                                    const char *name);
EAPI Engrave_Font *engrave_file_font_by_name_find(Engrave_File *ef, 
                                                  const char *name);

EAPI Engrave_Image *engrave_file_image_by_name_find(Engrave_File *ef,
                                                    const char *name);

EAPI int engrave_file_images_count(Engrave_File *ef);
EAPI int engrave_file_data_count(Engrave_File *ef);
EAPI int engrave_file_groups_count(Engrave_File *ef);
EAPI int engrave_file_fonts_count(Engrave_File *ef);
EAPI int engrave_file_spectra_count(Engrave_File *ef);
EAPI int engrave_file_styles_count(Engrave_File *ef);
EAPI int engrave_file_color_classes_count(Engrave_File *ef);

EAPI void engrave_file_image_foreach(Engrave_File *ef,
                                     void (*func)(Engrave_Image *, void *),
                                     void *data);
EAPI void engrave_file_data_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Data *, void *),
                                    void *data);
EAPI void engrave_file_group_foreach(Engrave_File *ef,
                                     void (*func)(Engrave_Group *, void *),
                                     void *data);
EAPI void engrave_file_font_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Font *, void *),
                                    void *data);
EAPI void engrave_file_spectrum_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Spectrum *, void *),
                                    void *data);
EAPI void engrave_file_style_foreach(Engrave_File *ef,
                                     void (*func)(Engrave_Style *, void *),
                                     void *data);
EAPI void engrave_file_color_class_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Color_Class *, void *),
                                    void *data);

EAPI Engrave_Data *engrave_file_data_by_key_find(Engrave_File *ef, 
                                                 const char *key);

/**
 * @}
 */

#endif

