#include "Emote.h"
#include "emote_private.h"

/* local function prototypes */
static char *_emote_protocol_find(const char *name);
static Emote_Protocol *_emote_protocol_load_file(const char *file);
static int _emote_protocol_is_valid(Emote_Protocol *p);
static void _emote_protocol_cb_free(Emote_Protocol *p);
static Eina_Bool _emote_protocol_hash_cb_free(const Eina_Hash *hash __UNUSED__, const void *key __UNUSED__, void *data, void *fdata __UNUSED__);

/* local variables */
static Eina_Hash *_emote_protocols = NULL;

EM_INTERN int
emote_protocol_init(void)
{
   /* TODO: loop config and load needed protocols */
   _emote_protocols = eina_hash_pointer_new(NULL);

   return 1;
}

EM_INTERN int
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

EMAPI Eina_List *
emote_protocol_list(void)
{
   Eina_List *files, *out;
   Emote_Protocol *p;
   char buf[PATH_MAX], *file, *c;

   if (!(files = ecore_file_ls(emote_paths.protocoldir))) return NULL;

   out = NULL;
   EINA_LIST_FREE(files, file)
     {
        strncpy(buf, file, sizeof(buf));
        if ((c = strstr(buf, ".so")) != NULL)
          {
             *c = 0;

             if ((p = emote_protocol_load(buf)) != NULL)
               {
                  out = eina_list_append(out, strdup(buf));
                  emote_protocol_unload(p);
               }
          }
     }

   return out;
}

EMAPI Emote_Protocol *
emote_protocol_load(const char *name)
{
   char *f;
   Emote_Protocol *p;

   if (!(f = _emote_protocol_find(name))) return NULL;
   printf("Found Protocol: %s\n", f);

   if (!(p = _emote_protocol_load_file(f)))
     {
        printf("Failed to load: %s\n", f);
        if (f) free(f);
        return NULL;
     }

   if (f) free(f);
   return p;
}

EMAPI void
emote_protocol_unload(Emote_Protocol *p)
{
   em_object_del(EM_OBJECT(p));
   eina_hash_del(_emote_protocols, NULL, p);
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

static Emote_Protocol *
_emote_protocol_load_file(const char *file)
{
   Emote_Protocol *p;

   if (!file) return NULL;

   p = EM_OBJECT_ALLOC(Emote_Protocol, EMOTE_PROTOCOL_TYPE,
                       _emote_protocol_cb_free);
   if (!p) return NULL;

   /* clear any existing errors in dynamic loader */
   dlerror();

   if (!(p->handle = dlopen(file, (RTLD_NOW | RTLD_GLOBAL))))
     {
        printf("Cannot dlopen protocol: %s\n", dlerror());
        em_object_del(EM_OBJECT(p));
        return NULL;
     }

   /* try to link to needed functions */
   p->api = dlsym(p->handle, "protocol_api");
   p->funcs.init = dlsym(p->handle, "protocol_init");
   p->funcs.shutdown = dlsym(p->handle, "protocol_shutdown");
   p->funcs.connect = dlsym(p->handle, "protocol_connect");
   p->funcs.disconnect = dlsym(p->handle, "protocol_disconnect");

   if (!_emote_protocol_is_valid(p))
     {
        printf("Protocol is not valid\n");
        em_object_del(EM_OBJECT(p));
        return NULL;
     }

   /* do init */
   if (!p->funcs.init(p))
     {
        printf("Protocol failed to initialize\n");
        em_object_del(EM_OBJECT(p));
        return NULL;
     }

   /* add to hash */
   eina_hash_add(_emote_protocols, file, p);

   return p;
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
        return 0;
     }

   /* check version */
   if (p->api->version < EMOTE_PROTOCOL_API_VERSION)
     {
        printf("Protocol too old\n");
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
