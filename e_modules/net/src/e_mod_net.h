#ifndef E_MOD_NET_H
#define E_MOD_NET_H

EAPI int _cb_poll(void *data);
EAPI void _cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
EAPI void _cb_mouse_in(void *data, Evas_Object *obj, const char *emission, const char *source);
EAPI void _cb_mouse_out(void *data, Evas_Object *obj, const char *emission, const char *source);

#endif
