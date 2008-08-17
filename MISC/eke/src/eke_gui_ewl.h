#ifndef EKE_GUI_EWL
#define EKE_GUI_EWL

#include "eke_gui.h"

int eke_gui_ewl_init(int *argc, char ** argv);
void eke_gui_ewl_shutdown(void);

void eke_gui_ewl_create(Eke *eke);
void eke_gui_ewl_feed_register(Eke *eke, Eke_Feed *feed);

void eke_gui_ewl_feed_change(Eke *eke, Eke_Feed *feed);
void eke_gui_ewl_feed_error(Eke *eke, Eke_Feed *feed);

void eke_gui_ewl_begin(void);

#endif


