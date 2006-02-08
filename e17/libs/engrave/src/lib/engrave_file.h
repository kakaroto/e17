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
 * Structure to hold the file information.
 */
struct _Engrave_File
{
  char *font_dir;    /**< The font directory */
  char *im_dir;      /**< The image directory */

  Evas_List *images; /**< The list of images in the file */
  Evas_List *fonts;  /**< The list of fonts in the file  */
  Evas_List *styles;  /**< The list of styles in the file  */
  Evas_List *data;   /**< The list of data items in the file */
  Evas_List *groups; /**< The list of groups in the file */
};

Engrave_File *engrave_file_new(void);
void engrave_file_free(Engrave_File *ef);

void engrave_file_image_dir_set(Engrave_File *ef, const char *dir);
void engrave_file_font_dir_set(Engrave_File *ef, const char *dir);

const char *engrave_file_image_dir_get(Engrave_File *ef);
const char *engrave_file_font_dir_get(Engrave_File *ef);

void engrave_file_font_add(Engrave_File *e, Engrave_Font *ef);
void engrave_file_style_add(Engrave_File *e, Engrave_Style *es);
void engrave_file_image_add(Engrave_File *ef, Engrave_Image *ei);
void engrave_file_data_add(Engrave_File *ef, Engrave_Data *ed);
void engrave_file_group_add(Engrave_File *ef, Engrave_Group *eg);

Engrave_Style *engrave_file_style_last_get(Engrave_File *ef);

Engrave_Group *engrave_file_group_last_get(Engrave_File *ef);
Engrave_Group *engrave_file_group_by_name_find(Engrave_File *ef,
                                              const char *name);
Engrave_Font *engrave_file_font_by_name_find(Engrave_File *ef, 
                                                const char *name);

Engrave_Image *engrave_file_image_by_name_find(Engrave_File *ef,
                                              const char *name);

int engrave_file_images_count(Engrave_File *ef);
int engrave_file_data_count(Engrave_File *ef);
int engrave_file_groups_count(Engrave_File *ef);
int engrave_file_fonts_count(Engrave_File *ef);
int engrave_file_styles_count(Engrave_File *ef);

void engrave_file_image_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Image *, void *),
                                    void *data);
void engrave_file_data_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Data *, void *),
                                    void *data);
void engrave_file_group_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Group *, void *),
                                    void *data);
void engrave_file_font_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Font *, void *),
                                    void *data);
void engrave_file_style_foreach(Engrave_File *ef,
                                    void (*func)(Engrave_Style *, void *),
                                    void *data);

Engrave_Data *engrave_file_data_by_key_find(Engrave_File *ef, 
                                    const char *key);

/**
 * @}
 */

#endif

