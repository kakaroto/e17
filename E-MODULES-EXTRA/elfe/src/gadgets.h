#ifndef _GADGETS_H_
#define _GADGETS_H_

#include <Elementary.h>

Evas_Object *elfe_gadgets_zone_add(Evas_Object *parent, int desktop, const char *desktop_name);
void elfe_gadgets_gadget_add(Evas_Object *obj, E_Gadcon_Client_Class *gcc);
#endif /* _GADGETS_H_ */
