#ifndef EMPOWER_PKA_H
#define EMPOWER_PKA_H

#include "Empower.h"

Eina_Bool pka_init();
Eina_Bool pka_shutdown();
Empower_Auth_State pka_state();
Empower_Auth_Info * pka_pending();
Eina_Bool pka_exec(Empower_Auth_Info *info);
Eina_Bool pka_cancel(Empower_Auth_Info *info);


#endif
