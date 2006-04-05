#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Source_File Source_File;

#else

#ifndef E_MOD_SOURCE_FILE_H_INCLUDED
#define E_MOD_SOURCE_FILE_H_INCLUDED

#define SOURCE_FILE_FONT_SIZE_DEFAULT 11

struct _Source_File
{
   DEVIANN *devian;

   Evas_Object *obj;

   /* Data file part */

   /* About the file parsing */
   Ecore_List *blocks;/** <List of buffer blocks of the file in order  and object */
   int lines_tot;/** <Number of lines buffered in blocks */
   int new_blocks;/** <Number of new blocks from last update */
   /* File and textblock */
   FILE *fd;
   Ecore_File_Monitor *monitor;
   Evas_Object *obj_tb;
};

/* Default things */

int DEVIANF(source_file_add) (DEVIANN *devian);
void DEVIANF(source_file_del) (Source_File *source);

int DEVIANF(source_file_change) (DEVIANN *devian, int option);
int DEVIANF(source_file_viewer) (DEVIANN *devian);

int DEVIANF(source_file_evas_object_get) (DEVIANN *devian, Evas_Object **file0, Evas_Object **file1);
int DEVIANF(source_file_original_size_get) (Source_File *source, int *w, int *h);
void DEVIANF(source_file_dialog_infos) (Source_File *source);
char *DEVIANF(source_file_name_get) (Source_File *source);
char *DEVIANF(source_file_infos_get) (Source_File *source);
void DEVIANF(source_file_update_change) (DEVIANN *devian, int active, int time);

#endif
#endif
