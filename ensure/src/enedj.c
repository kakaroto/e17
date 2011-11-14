#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eet.h>
#include <Eina.h>
#include <Edje.h>

#include "ensure.h"

struct enedj
{
   const char  *file;

   int          nitems;
   int          nalloc;
   const char **images;
};

typedef struct Edje_File                  Edje_File;
typedef struct Edje_Image_Directory       Edje_Image_Directory;
typedef struct Edje_Image_Directory_Entry Edje_Image_Directory_Entry;

struct Edje_File
{
   Edje_Image_Directory *image_dir;
};

struct Edje_Image_Directory
{
   Eina_List *entries; /* a list of Edje_Image_Directory_Entry */
};

struct Edje_Image_Directory_Entry
{
   char *entry; /* the nominal name of the image - if any */
   int   id; /* the id no. of the image */
};

static const char *enedj_index_lookup(const struct enedj *enedj, const char *index);
static int         enedj_load(const char *filename);
static int         enedj_init(void);

static Eina_List *edjes;
static int edjeinit;

static Eet_Data_Descriptor *_edje_edd_edje_file = NULL;
static Eet_Data_Descriptor *_edje_edd_edje_image_directory = NULL;
static Eet_Data_Descriptor *_edje_edd_edje_image_directory_entry = NULL;

/**
 * Get the filename for a key.
 */
const char *
enedj_get_imagename(const char *edjefile, const char *index)
{
   Eina_List *l;
   struct enedj *enedj;

   if (!edjefile || !index)
     return NULL;

   EINA_LIST_FOREACH (edjes, l, enedj)
     {
        if (streq(enedj->file, edjefile))
          {
             return enedj_index_lookup(enedj, index);
          }
     }
   /* load it */
   enedj_load(edjefile);

   /* try again */
   EINA_LIST_FOREACH (edjes, l, enedj)
     {
        if (streq(enedj->file, edjefile))
          {
             return enedj_index_lookup(enedj, index);
          }
     }
   return NULL;
}

static const char *
enedj_index_lookup(const struct enedj *ensure_restrict enedj, const char *ensure_restrict index)
{
   long val;
   assert(enedj);
   assert(index);
   if (!index || !enedj)
     return NULL;

   if (*index == 'i')
     {
        index = strchr(index, '/');
        if (!index || !*(index + 1))
          return NULL;
        index++;
     }

   if (!isdigit(*index))
     return NULL;

   errno = 0;
   val = strtol(index, NULL, 10);
   if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno && val == 0))
     {
        return NULL;
     }

   if (val > enedj->nitems)
     {
        printf("Image out of range %d (max %d)\n", (int)val, enedj->nitems);
        return NULL;
     }

   return enedj->images[val];
}

static int
enedj_load(const char *filename)
{
   Edje_Image_Directory_Entry *ei;
   Eet_File *ef;
   Edje_File *file;
   struct enedj *images;
   Eina_List *l;
   int count;

   assert(filename);

   if (!edjeinit)
     enedj_init();
   edjeinit++;

   ef = eet_open(filename, EET_FILE_MODE_READ);
   if (!ef)
     {
        printf("Could not open eet file\n");
        return -1;
     }

   file = eet_data_read(ef, _edje_edd_edje_file, "edje_file");
   if (!file)
     {
        printf("Did not parse edje file\n");
        /* FIXME: Leaks */
        return -1;
     }

   if (!file->image_dir)
     {
        printf("No images found!\n");
        return -1;
     }

   count = eina_list_count(file->image_dir->entries);
   images = calloc(1, sizeof(struct enedj));
   images->file = eina_stringshare_add(filename);
   images->images = calloc(count + 1, sizeof(char *));
   images->nitems = count;
   images->nalloc = count + 1;
   edjes = eina_list_append(edjes, images);

   EINA_LIST_FOREACH (file->image_dir->entries, l, ei)
     {
        if (ei->id > images->nalloc)
          {
             printf("Illegal ID (%d, max %d)\n", ei->id, images->nalloc);
             continue;
          }
        /* FIXME: Check it's in range */
        images->images[ei->id] = eina_stringshare_add(ei->entry);
     }

   return 0;
}

static int
enedj_init(void)
{
   Eet_Data_Descriptor_Class eddc;
   if (edjeinit)
     return edjeinit;

   edjeinit++;

   /* image directory */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Edje_Image_Directory_Entry);
   _edje_edd_edje_image_directory_entry = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_edje_edd_edje_image_directory_entry, Edje_Image_Directory_Entry, "entry", entry, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_edje_edd_edje_image_directory_entry, Edje_Image_Directory_Entry, "id", id, EET_T_INT);
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Edje_Image_Directory);
   _edje_edd_edje_image_directory = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_LIST(_edje_edd_edje_image_directory, Edje_Image_Directory, "entries", entries, _edje_edd_edje_image_directory_entry);

   /* the main file directory */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Edje_File);
   _edje_edd_edje_file = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_SUB(_edje_edd_edje_file, Edje_File, "image_dir", image_dir, _edje_edd_edje_image_directory);

   return 0;
}

