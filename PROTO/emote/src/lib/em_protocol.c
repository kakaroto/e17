#include "em_private.h"

static Eina_Hash *_em_protocols = NULL;

static int
_em_protocol_loaded(const char* protocol)
{
  return (eina_hash_find(_em_protocols, protocol) != NULL);
}

static void
_em_protocol_add(const char* name, em_functions *funcs)
{
  eina_hash_set(_em_protocols, name, funcs);
}

int
em_protocol_load(const char* protocol)
{
  char lib[PATH_MAX];
  void *handle;
  em_functions *functions;

  if(!_em_protocol_loaded(protocol))
  {
    snprintf(lib, PATH_MAX, "%s/emote/protocols/%s.so", em_paths.libdir,
             protocol);

    handle = dlopen(lib, RTLD_LAZY);
    if(handle)
    {
        functions = (em_functions*)dlsym(handle, "em_plugin");

        _em_protocol_add(protocol, functions);
    }
    else
    {
        printf("Error loading %s protocol: %s\n", protocol, dlerror());
        return 0;
    }
  }

  return 1;
}

int
em_protocol_init()
{
  _em_protocols = eina_hash_string_superfast_new(NULL);

  return (_em_protocols != NULL);
}

void
em_protocol_shutdown()
{
  if(_em_protocols)
    eina_hash_free(_em_protocols);
}
