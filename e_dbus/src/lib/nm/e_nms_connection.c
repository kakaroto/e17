#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

EAPI int
e_nms_connection_get(E_NMS *nms, E_NMS_Context context, const char *connection, int (*cb_func)(void *data, E_NMS_Connection *conn), void *data)
{
  E_NMS_Connection_Internal *conn;

  conn = calloc(1, sizeof(E_NMS_Connection_Internal));
  conn->path = strdup(connection);
  conn->context = context;
  (*cb_func)(data, (E_NMS_Connection *)conn);
  return 1;
}

EAPI void
e_nms_connection_free(E_NMS_Connection *connection)
{
  E_NMS_Connection_Internal *conn;
  if (!connection) return;
  conn = (E_NMS_Connection_Internal *)connection;

  if (conn->path) free(conn->path);
  free(conn);
}

EAPI void
e_nms_connection_dump(E_NMS_Connection *connection)
{
  E_NMS_Connection_Internal *conn;
  if (!connection) return;
  conn = (E_NMS_Connection_Internal *)connection;

  printf("E_NMS_Connection:\n");
  printf("context: ");
  switch (conn->context)
  {
    case E_NMS_CONTEXT_SYSTEM:
      printf("E_NMS_CONTEXT_SYSTEM\n");
      break;
    case E_NMS_CONTEXT_USER:
      printf("E_NMS_CONTEXT_USER\n");
      break;
  }
  printf("path   : %s\n", conn->path);
  printf("\n");
}

