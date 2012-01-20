#ifndef EKBD_SEND_H_
#define EKBD_SEND_H_
 typedef enum _Ekbd_Mod
 {
     EKBD_MOD_SHIFT = (1 << 0),
     EKBD_MOD_CTRL  = (1 << 1),
     EKBD_MOD_ALT   = (1 << 2),
     EKBD_MOD_WIN   = (1 << 3)
 } Ekbd_Mod;

EAPI void ekbd_send_string_press(const char *str, Ekbd_Mod mod);
EAPI void ekbd_send_keysym_press(const char *key, Ekbd_Mod mod);
EAPI void ekbd_send_update(Smart_Data *sd);
#endif /* EKBD_SEND_H_ */

