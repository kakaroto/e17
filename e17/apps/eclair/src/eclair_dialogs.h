#ifndef _ECLAIR_DIALOGS_H_
#define _ECLAIR_DIALOGS_H_

#include "eclair_private.h"

void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair);
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_open_file_dialog(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_popup_menu(Eclair_Dialogs_Manager *dialogs_manager);

#endif
