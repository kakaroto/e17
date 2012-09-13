#ifndef EMPOWER_LOGIND_H
#define EMPOWER_LOGIND_H

#include "Empower.h"

Eina_Bool   logind_init(E_DBus_Connection *conn);
const char *logind_session_get();

#endif
