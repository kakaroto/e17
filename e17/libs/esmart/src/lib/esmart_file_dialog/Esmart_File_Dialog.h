#ifndef _ESMART_FILE_DIALOG_H
#define _ESMART_FILE_DIALOG_H

#include <Evas.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	ESMART_FILE_DIALOG_NEW = 1,
	ESMART_FILE_DIALOG_RENAME,
	ESMART_FILE_DIALOG_DELETE,
	ESMART_FILE_DIALOG_OK,
	ESMART_FILE_DIALOG_CANCEL,
	ESMART_FILE_DIALOG_DIR_CHANGED,
} Esmart_File_Dialog_Op;

/**
 * esmart_file_dialog_new 
 * @param evas the evas to add the object to
 * @param edje the file to load the edje parts from
 */
  Evas_Object *esmart_file_dialog_new (Evas * evas, const char *edje_file);

  void esmart_file_dialog_callback_add (Evas_Object * efd,
					void (*func) (void *data,
						      Evas_Object * edje,
						      Esmart_File_Dialog_Op op), void *data);
  Evas_Object *esmart_file_dialog_edje_get (Evas_Object * efd);
  Evas_List *esmart_file_dialog_selections_get (Evas_Object * efd);
  const char *esmart_file_dialog_current_directory_get (Evas_Object * efd);

#ifdef __cplusplus
}
#endif

#endif
