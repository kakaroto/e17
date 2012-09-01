#ifndef EMPOWER_PKA_H
#define EMPOWER_PKA_H

#include "Empower.h"

#define CONKIT_BUS "org.freedesktop.ConsoleKit"
#define CONKIT_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CONKIT_INTERFACE "org.freedesktop.ConsoleKit.Manager"

#define POLKIT_BUS "org.freedesktop.PolicyKit1"
#define POLKIT_PATH "/org/freedesktop/PolicyKit1/Authority"
#define POLKIT_INTERFACE "org.freedesktop.PolicyKit1.Authority"

#define AGENT_BUS "auth.empower"
#define AGENT_PATH "/auth/empower/Agent"
#define AGENT_INTERFACE "org.freedesktop.PolicyKit1.AuthenticationAgent"

Eina_Bool pka_init();
Eina_Bool pka_shutdown();
Empower_Auth_State pka_state();
Empower_Auth_Info * pka_pending();
Eina_Bool pka_exec(Empower_Auth_Info *info);
Eina_Bool pka_cancel(Empower_Auth_Info *info);


#endif
