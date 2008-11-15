#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property active_connection_properties[] = {
  { .name = "ServiceName", .sig = "s", .offset = offsetof(E_NM_Active_Connection, service_name) },
  { .name = "Connection", .sig = "o", .offset = offsetof(E_NM_Active_Connection, connection) },
  { .name = "SpecificObject", .sig = "o", .offset = offsetof(E_NM_Active_Connection, specific_object) },
  { .name = "Devices", .sig = "ao", .offset = offsetof(E_NM_Active_Connection, devices) },
  { .name = "State", .sig = "u", .offset = offsetof(E_NM_Active_Connection, state) },
  { .name = "Default", .sig = "b", .offset = offsetof(E_NM_Active_Connection, def) },
  { .name = NULL }
};


EAPI int
e_nm_active_connection_get(E_NM *nm, const char *connection,
                           int (*cb_func)(void *data, E_NM_Active_Connection *conn),
                           void *data)
{
  E_NM_Internal                   *nmi;
  E_NM_Active_Connection_Internal *conn;
  Property_Data                   *d;

  nmi = (E_NM_Internal *)nm;
  conn = calloc(1, sizeof(E_NM_Active_Connection_Internal));
  conn->nmi = nmi;
  conn->conn.path = strdup(connection);
  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = conn;
  d->property = active_connection_properties;
  d->object = strdup(connection);

  return e_nm_active_connection_properties_get(nmi->conn, d->object, d->property->name, property, d) ? 1 : 0;
}

EAPI void
e_nm_active_connection_free(E_NM_Active_Connection *conn)
{
  if (!conn) return;
  if (conn->path) free(conn->path);
  if (conn->service_name) free(conn->service_name);
  if (conn->connection) free(conn->connection);
  if (conn->specific_object) free(conn->specific_object);
  if (conn->devices) ecore_list_destroy(conn->devices);
  free(conn);
}

EAPI void
e_nm_active_connection_dump(E_NM_Active_Connection *conn)
{
  const char *device;

  if (!conn) return;
  printf("E_NM_Active_Connection:\n");
  printf("service_name   : %s\n", conn->service_name);
  printf("connection     : %s\n", conn->connection);
  printf("specific_object: %s\n", conn->specific_object);
  printf("devices        :\n");
  ecore_list_first_goto(conn->devices);
  while ((device = ecore_list_next(conn->devices)))
    printf(" - %s\n", device);
  printf("state          : ");
  switch (conn->state)
  {
    case E_NM_ACTIVE_CONNECTION_STATE_UNKNOWN:
      printf("E_NM_ACTIVE_CONNECTION_STATE_UNKNOWN\n");
      break;
    case E_NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
      printf("E_NM_ACTIVE_CONNECTION_STATE_ACTIVATING\n");
      break;
    case E_NM_ACTIVE_CONNECTION_STATE_ACTIVATED:
      printf("E_NM_ACTIVE_CONNECTION_STATE_ACTIVATED\n");
      break;
  }
  printf("default        : %d\n", conn->def);
  printf("\n");
}

