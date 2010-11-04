#include "ZMServer.zxrs.h"

int main(int ac, char* av[])
{
  GError* err = NULL;
  zxr_servlet_def* defs[2] = {
    __ZMServerServlet_def(),
    NULL
  };
  
  g_thread_init(NULL);
  
  zxr_debug_enabled = ZXR_DEBUG_ALL;

  // start HTTPS server, with 5 threads in the pool, listening on port 4444,
  // and implementing ZMServer servlet.
  zxr_server_simple("server.pem", 5, "*:1234", defs, &err);
  if (err)
    g_print("error: %s\n", err->message);

  return !!err;
}
