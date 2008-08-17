#ifndef _EW_PREVIEW_H
#define _EW_PREVIEW_H

typedef struct  _Entrance_Preview {
	Etk_Widget *owner;
	Etk_Widget *box;
	Evas_Object *preview_smart;
} *Entrance_Preview;


Entrance_Preview ew_preview_new(int w, int h);
Evas *ew_preview_evas_get(Entrance_Preview ep, int w, int h, int vw, int vh);
#endif
