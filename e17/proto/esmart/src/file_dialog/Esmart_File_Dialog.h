#ifndef ESMART_FILE_DIALOG
#define ESMART_FILE_DIALOG

#include <Evas.h>
#include <Edje.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Esmart_File_Dialog Esmart_File_Dialog;

#define FILE_NEW 1
#define FILE_RENAME 2
#define FILE_DELETE 3
#define FILE_OK 4
#define FILE_CANCEL 5
#define DIR_CHANGED 6

struct _Esmart_File_Dialog
{
  Evas_Object *clip;		/* clipped area for the file dialog */
  Evas_Object *edje;		/* the edje file we load groups from */
  Evas_Object *directories;	/* directory containers */
  char *directories_dragbar;	/* dragable part name for the
				 * directories */
  Evas_Object *files;		/* file container */
  char *files_dragbar;		/* the dragable for the file container */

  Evas_Object *entry;		/* Esmart_Text_Entry */
  char *path;			/* the cwd for the dialog */

  /* the client callback for intercepting file dialog specific stuff */
  void (*func) (void *data, Evas_Object * edje, int type);
  /* the data that's passed to the file dialog callback */
  void *fdata;

  /* the current geometry/location of the file dialog */
  Evas_Coord x, y, w, h;

  /* the files the user wants to load/open/save(?)/ */
  Evas_List *selections;
};

/**
 * esmart_file_dialog_new 
 * @param evas the evas to add the object to
 * @param edje the file to load the edje parts from
 */
Evas_Object *esmart_file_dialog_new (Evas * evas, const char *edje_file);

void esmart_file_dialog_callback_add (Evas_Object * efd,
				      void (*func) (void *data,
						    Evas_Object * edje,
						    int type), void *data);
Evas_Object *esmart_file_dialog_edje_get (Evas_Object * efd);
Evas_List *esmart_file_dialog_selections_get (Evas_Object * efd);
const char *esmart_file_dialog_current_directory_get (Evas_Object * efd);

#ifdef __cplusplus
}
#endif

#endif
