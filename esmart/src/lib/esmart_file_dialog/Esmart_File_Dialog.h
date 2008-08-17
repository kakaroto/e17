#ifndef _ESMART_FILE_DIALOG_H
#define _ESMART_FILE_DIALOG_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

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
  EAPI Evas_Object *esmart_file_dialog_new (Evas * evas, const char *edje_file);

  EAPI void esmart_file_dialog_callback_add (Evas_Object * efd,
					     void (*func) (void *data,
							   Evas_Object * edje,
							   Esmart_File_Dialog_Op op),
					     void *data);
  EAPI Evas_Object *esmart_file_dialog_edje_get (Evas_Object * efd);
  EAPI Evas_List *esmart_file_dialog_selections_get (Evas_Object * efd);
  EAPI const char *esmart_file_dialog_current_directory_get (Evas_Object * efd);

#ifdef __cplusplus
}
#endif

#endif
