#include "ZMServer.zxrc.h"

static int print_error(GError* err)
{
  if (err)
  {
    g_print("** ERROR **: %s\n", err->message);
    g_error_free(err);
    return 1;
  }
  return 0;
}

int main(int ac, char* av[])
{
  GError* err = NULL;
  char* uri = ac == 2 ? av[1] : "https://localhost:1234/ZMServer";

  zxr_debug_enabled = ZXR_DEBUG_ALL;

  g_thread_init(NULL);

  // create object for performing client connections
  zxr_client_conn* conn = zxr_client_new(&err);
  if (print_error(err))
    goto err;

  // connect to the servlet on the server specified by uri
  zxr_client_open(conn, uri, &err);
  if (print_error(err))
    goto err;

  // login
  ZMServer_auth(conn, "bob", "qwe", &err);
  if (print_error(err))
    goto err;
  
  // change password
  ZMServer_changeUserPassword(conn, "qqq", &err);
  if (print_error(err))
    goto err;

 err:
  // free connections object
  zxr_client_free(conn);
  return 0;
}
