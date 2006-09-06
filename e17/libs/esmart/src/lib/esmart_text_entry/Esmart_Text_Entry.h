#ifndef _ESMART_TEXT_ENTRY_H
#define _ESMART_TEXT_ENTRY_H

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
extern "C" {
#endif

/* create a new text entry */
EAPI Evas_Object *esmart_text_entry_new (Evas * e);

EAPI void esmart_text_entry_text_set (Evas_Object * o, const char *str);
EAPI void esmart_text_entry_is_password_set (Evas_Object * o, int val);
EAPI void esmart_text_entry_max_chars_set (Evas_Object * o, int max);
EAPI void esmart_text_entry_edje_part_set (Evas_Object * o, Evas_Object * edje,
					   const char *part);
EAPI Evas_Object *esmart_text_entry_edje_object_get(Evas_Object * o);
EAPI const char *esmart_text_entry_edje_part_get(Evas_Object * o);

/* you've gotta free this resut */
EAPI const char *esmart_text_entry_text_get(Evas_Object *o);

EAPI void esmart_text_entry_return_key_callback_set (Evas_Object * o,
						     void (*func) (void *data,
								   const char *str),
						     void *data);

#ifdef __cplusplus
}
#endif

#endif
