#ifndef _VIEW_H
#define	_VIEW_H

#include <Evas.h>

typedef enum _Document_View_Mode {
    VIEW_ORIGINAL,
    VIEW_FIT,
    VIEW_HFIT,
    VIEW_VFIT,
    VIEW_CUSTOM
} Document_View_Mode;

void zoom_original_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

void zoom_vfit_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

void zoom_hfit_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

void zoom_fit_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

void zoom_in_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

void zoom_out_clicked_cb(void *data, Evas_Object *obj,
        const char *emission, const char *src);

#endif	/* _VIEW_H */

