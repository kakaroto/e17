#ifndef EKE_GUI
#define EKE_GUI

#include "Eke.h"
#include "eke_feed.h"
#include "eke_feed_item.h"
#include "eke_macros.h"

#define EKE_GUI_WIDTH   640
#define EKE_GUI_HEIGHT  480

int eke_gui_init(Eke *eke, int *argv, char ** argc);
int eke_gui_shutdown(Eke *eke);

void eke_gui_create(Eke *eke);
void eke_gui_feed_register(Eke *eke, Eke_Feed *feed);

void eke_gui_begin(Eke *eke);

int eke_gui_value_changed_cb(void *data, int type, void *ev);

#endif

