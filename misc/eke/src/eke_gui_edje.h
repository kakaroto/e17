#ifndef EKE_GUI_EDJE
#define EKE_GUI_EDJE

#include "eke_gui.h"

int eke_gui_edje_init(void);
void eke_gui_edje_shutdown(void);

void eke_gui_edje_create(Eke *eke);
void eke_gui_edje_feed_register(Eke *eke, Eke_Feed *feed);

void eke_gui_edje_feed_change(Eke *eke, Eke_Feed *feed);
void eke_gui_edje_feed_error(Eke *eke, Eke_Feed *feed);

void eke_gui_edje_begin(void);

#endif

