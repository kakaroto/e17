#include "libemote.h"

#if HAVE_DLFCN_H
# include <dlfcn.h>
#endif

/* local function prototypes */
static char *_emote_protocol_find(const char *name);
static int _emote_protocol_load(const char *file);
static int _emote_protocol_is_valid(Emote_Protocol *p);
static void _emote_protocol_cb_free(Emote_Protocol *p);
static Eina_Bool _emote_protocol_hash_cb_free(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata __UNUSED__);

/* local variables */
static Eina_Hash *_emote_protocols = NULL;

int
emote_protocol_init(void)
{
   char *irc, *aim;

   _emote_protocols = eina_hash_string_superfast_new(NULL);

   /* TODO: loop config and load needed protocols */

   /* load irc protocol as a test */
   if (!(irc = _emote_protocol_find("irc"))) return 0;
   printf("Found Protocol: %s\n", irc);

   if (!(_emote_protocol_load(irc)))
     {
        printf("Failed to load: %s\n", irc);
        if (irc) free(irc);
        return 0;
     }

   if (!(aim = _emote_protocol_find("aim"))) return 0;
   printf("Found Protocol: %s\n", aim);

   if (!(_emote_protocol_load(aim)))
     {
        printf("Failed to load: %s\n", aim);
        if (aim) free(aim);
        return 0;
     }

   return 1;
}

int
emote_protocol_shutdown(void)
{
   /* shutdown loaded protocols */
   if (_emote_protocols)
     {
        eina_hash_foreach(_emote_protocols, _emote_protocol_hash_cb_free, NULL);
        eina_hash_free(_emote_protocols);
     }

   return 1;
}

/* local functions */
static char *
_emote_protocol_find(const char *name)
{
   Eina_List *files;
   char buff[PATH_MAX], dir[PATH_MAX], *file;

   snprintf(dir, sizeof(dir), "%s", emote_paths.protocoldir);
   snprintf(buff, sizeof(buff), "%s.so", name);

   if (!(files = ecore_file_ls(dir))) return NULL;
   EINA_LIST_FREE(files, file)
     {
        if (!strcmp(file, buff))
          {
             /* could proally use a strcat here */
             snprintf(dir, sizeof(dir), "%s/%s", emote_paths.protocoldir, file);
             break;
          }
        free(file);
     }
   if (file)
     {
        free(file);
        return strdup(dir);
     }
   else
     return NULL;
}

static int
_emote_protocol_load(const char *file)
{
   Emote_Protocol *p;

   if (!file) return 0;

   p = EM_OBJECT_ALLOC(Emote_Protocol, EMOTE_PROTOCOL_TYPE,
                       _emote_protocol_cb_free);
   if (!p) return 0;

   /* clear any existing errors in dynamic loader */
   dlerror();

   if (!(p->handle = dlopen(file, (RTLD_NOW | RTLD_GLOBAL))))
     {
        printf("Cannot dlopen protocol: %s\n", dlerror());
        em_object_del(EM_OBJECT(p));
        return 0;
     }

   /* try to link to needed functions */
   p->api = dlsym(p->handle, "protocol_api");
   p->funcs.init = dlsym(p->handle, "protocol_init");
   p->funcs.shutdown = dlsym(p->handle, "protocol_shutdown");
   p->funcs.connect = dlsym(p->handle, "protocol_connect");
   p->funcs.disconnect = dlsym(p->handle, "protocol_disconnect");

   if (!_emote_protocol_is_valid(p)) return 0;

   /* do init */
   if (!p->funcs.init())
     {
        printf("Protocol failed to initialize\n");
        em_object_del(EM_OBJECT(p));
        return 0;
     }

   /* add to hash */
   eina_hash_add(_emote_protocols, file, p);

   return 1;
}

static int 
_emote_protocol_is_valid(Emote_Protocol *p)
{
   /* check support for needed functions */
   if ((!p->api) || (!p->funcs.init) || (!p->funcs.shutdown) ||
       (!p->funcs.connect) || (!p->funcs.disconnect))
     {
        printf("Protocol does not support needed functions\n");
        printf("Error: %s\n", dlerror());
        em_object_del(EM_OBJECT(p));
        return 0;
     }

   /* check version */
   if (p->api->version < EMOTE_PROTOCOL_API_VERSION)
     {
        printf("Protocol too old\n");
        em_object_del(EM_OBJECT(p));
        return 0;
     }

   return 1;
}

static void
_emote_protocol_cb_free(Emote_Protocol *p)
{
   if (!p) return;

   if (p->funcs.shutdown) p->funcs.shutdown();
   p->funcs.shutdown = NULL;
   p->funcs.init = NULL;
   p->api = NULL;

   if (p->handle) dlclose(p->handle);
   p->handle = NULL;

   EMOTE_FREE(p);
}

static Eina_Bool
_emote_protocol_hash_cb_free(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata __UNUSED__)
{
   Emote_Protocol *p;

   if (!(p = data)) return EINA_TRUE;
   em_object_del(EM_OBJECT(p));
   return EINA_TRUE;
}
