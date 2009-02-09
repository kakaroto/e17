#ifndef _EVE_NAVIGATOR_H_
#define _EVE_NAVIGATOR_H_


#include <Evas.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


  EAPI Evas_Object *eve_navigator_add(Evas *e);

  EAPI bool         eve_navigator_theme_file_set(Evas_Object *o, const char *file);
  EAPI const char  *eve_navigator_theme_file_get(const Evas_Object *o);
  EAPI bool         eve_navigator_theme_group_set(Evas_Object *o, const char *group);
  EAPI const char  *eve_navigator_theme_group_get(const Evas_Object *o);

  EAPI Evas_Object *eve_navigator_scrolled_get(const Evas_Object *o);
  EAPI Evas_Object *eve_navigator_webview_get(const Evas_Object *o);
  EAPI Evas_Object *eve_navigator_page_get(const Evas_Object *o);

  EAPI void         eve_navigator_load_url(Evas_Object *o, const char *url);


#ifdef __cplusplus
}
#endif


#endif /* _EVE_NAVIGATOR_H_ */
