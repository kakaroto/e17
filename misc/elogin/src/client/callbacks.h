#ifndef _ELOGIN_CALLBACKS
#define _ELOGIN_CALLBACKS

#include "elogin.h"

void elogin_bg_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b,
                          int _x, int _y);
int elogin_return_key_cb(E_Login_Session e, char *buffer);


#endif
