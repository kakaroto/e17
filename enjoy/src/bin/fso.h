#ifndef ENJOY_FSO_H
#define ENJOY_FSO_H

#ifdef _HAVE_FSO_

#include <E_DBus.h>
#include "private.h"


void fso_init(void);

void fso_shutdown(void);

void fso_request_reource(const char *resource);

void fso_release_resource(const char *resource);

void fso_request_reource_cb(void *data, DBusMessage *replymsg, DBusError *error);

void fso_release_resource_cb(void *data, DBusMessage *replymsg, DBusError *error);

#endif /* _HAVE_FSO_ */

#endif /* ENJOY_FSO_H */
