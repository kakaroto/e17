#ifndef _ESMART_FILE_DIALOG_H
#define _ESMART_FILE_DIALOG_H

#include <Evas.h>
#include <Edje.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define FILE_NEW 1
#define FILE_RENAME 2
#define FILE_DELETE 3
#define FILE_OK 4
#define FILE_CANCEL 5
#define DIR_CHANGED 6

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
