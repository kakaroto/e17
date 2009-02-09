#ifndef _EVE_SCROLLED_WEBVIEW_H_
#define _EVE_SCROLLED_WEBVIEW_H_


#include <Evas.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


  EAPI Evas_Object *eve_scrolled_webview_add(Evas *e);

  EAPI bool         eve_scrolled_webview_theme_file_set(Evas_Object *o, const char *file);
  EAPI const char  *eve_scrolled_webview_theme_file_get(const Evas_Object *o);
  EAPI bool         eve_scrolled_webview_theme_group_set(Evas_Object *o, const char *group);
  EAPI const char  *eve_scrolled_webview_theme_group_get(const Evas_Object *o);

  EAPI Evas_Object *eve_scrolled_webview_get(const Evas_Object *o);
  EAPI Evas_Object *eve_scrolled_webview_page_get(const Evas_Object *o);

  EAPI void         eve_scrolled_webview_scroll_report(const Evas_Object *o);


#ifdef __cplusplus
}
#endif


#endif /* _EVE_SCROLLED_WEBVIEW_H_ */
