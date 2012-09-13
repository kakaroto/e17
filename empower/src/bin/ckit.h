#ifndef EMPOWER_CKIT_H
#define EMPOWER_CKIT_H

#include "Empower.h"

Eina_Bool   ckit_init(E_DBus_Connection *conn);
const char *ckit_session_get();

#endif
