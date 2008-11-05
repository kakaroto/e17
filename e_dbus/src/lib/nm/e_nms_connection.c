#include "E_Nm.h"
#include "e_nm_private.h"

EAPI int
e_nms_connection_get(E_NMS *nms, const char *connection, int (*cb_func)(void *data, E_NMS_Connection *conn), void *data)
{
  E_NMS_Connection_Internal *conn;

  conn = calloc(1, sizeof(E_NMS_Connection_Internal));
  (*cb_func)(data, (E_NMS_Connection *)conn);
  return 1;
}

EAPI void
e_nms_connection_free(E_NMS_Connection *conn)
{
  if (!conn) return;
  free(conn);
}

EAPI void
e_nms_connection_dump(E_NMS_Connection *conn)
{
  if (!conn) return;
  printf("E_NMS_Connection:\n");
  printf("\n");
}

