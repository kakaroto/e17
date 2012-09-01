#ifndef EMPOWER_MARSHAL_H
#define EMPOWER_MARSHAL_H

#include "Empower.h"

// Subject Structures
void marshal_subject(DBusMessageIter *itr, Empower_Subject *subject);

// Identity Structures
void marshal_identity(DBusMessageIter *itr, Empower_Identity *id);
void unmarshal_identity(DBusMessageIter *itr, Empower_Identity *id);

// Other
Eina_Hash *unmarshal_dict_string_string(DBusMessageIter *itr);

#endif
