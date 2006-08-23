#include <Evas.h>
#include <Etk.h>
#include <Esmart_Preview.h>
#include "Entrance_Widgets.h"

Entrance_Widget
ew_preview_new(int w, int h)
{
	Entrance_Widget ep = ew_new();
	if(!ep)
		return NULL;

	ep->owner = etk_canvas_new();
	etk_widget_size_request_set(ep->owner, w, h);

	return ep;
}

Evas*
ew_preview_evas_get(Entrance_Widget ep, int w, int h, int vw, int vh)
{
	if(ep->preview_smart)
		esmart_preview_evas_get(ep->preview_smart);

	Evas *evas = etk_widget_toplevel_evas_get(ETK_WIDGET(ep->owner));

	ep->preview_smart = esmart_preview_new(evas);
	etk_canvas_object_add(ETK_CANVAS(ep->owner), ep->preview_smart);

	esmart_preview_virtual_size_set(ep->preview_smart, vw, vh);
	evas_object_resize(ep->preview_smart, w, h);
	etk_canvas_object_move(ETK_CANVAS(ep->owner), ep->preview_smart, 0, 0);
	evas_object_show(ep->preview_smart);

	return esmart_preview_evas_get(ep->preview_smart);
}
