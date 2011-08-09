#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <Evas.h>
#include    <Ecore.h>
#include    <Ecore_Evas.h>
#include    <Edje.h>

#include "eenvaders_evas_smart.h"

/* Prototypes -{{{-*/

static Evas_Object *
eenvaders_ext_add(void *data, Evas *evas, Evas_Object *parent,
                  const Eina_List *params, const char *part_name);
static void
eenvaders_ext_state_set(void *data, Evas_Object *obj,
                        const void *from_params,
                        const void *to_params, float pos);
static void
eenvaders_ext_signal_emit(void *data, Evas_Object *obj,
                          const char *emission, const char *source);
static Eina_Bool
eenvaders_ext_param_set(void *data, Evas_Object *obj,
                        const Edje_External_Param *param);
static Eina_Bool
eenvaders_ext_param_get(void *data, const Evas_Object *obj,
                        Edje_External_Param *param);
static Evas_Object *
eenvaders_ext_content_get(void *data, const Evas_Object *obj,
                          const char *content);
static void*
eenvaders_ext_params_parse(void *data, Evas_Object *obj,
                           const Eina_List *params);
static void
eenvaders_ext_params_free(void *params);
static const char*
eenvaders_ext_label_get(void *data);
static const char*
eenvaders_ext_description_get(void *data);
static Evas_Object *
eenvaders_ext_icon_add(void *data, Evas *e);
static Evas_Object *
eenvaders_ext_preview_add(void *data, Evas *e);
static const char*
eenvaders_ext_translate(void *data, const char *orig);

/* }}} */
/* Globals -{{{-*/

static struct {
    Edje_External_Type ext_type;
} eenvaders_g = {
    .ext_type = {
        .abi_version = EDJE_EXTERNAL_TYPE_ABI_VERSION,
        .module = "eenvaders",
        .module_name = "eenvaders",
        .add = eenvaders_ext_add,
        .state_set = eenvaders_ext_state_set,
        .signal_emit = eenvaders_ext_signal_emit,
        .param_set = eenvaders_ext_param_set,
        .param_get = eenvaders_ext_param_get,
        .content_get = eenvaders_ext_content_get,
        .params_parse = eenvaders_ext_params_parse,
        .params_free = eenvaders_ext_params_free,
        .label_get = eenvaders_ext_label_get,
        .description_get = eenvaders_ext_description_get,
        .icon_add = eenvaders_ext_icon_add,
        .preview_add = eenvaders_ext_preview_add,
        .translate = eenvaders_ext_translate,
    },
};
#define _G eenvaders_g

/* }}} */
/* Edje External -{{{-*/

static Evas_Object *
eenvaders_ext_add(void *data, Evas *evas, Evas_Object *parent,
                  const Eina_List *params, const char *part_name)
{
    return eenvaders_smart_new(evas);
}

static void
eenvaders_ext_state_set(void *data, Evas_Object *obj,
                        const void *from_params,
                        const void *to_params, float pos)
{
}

static void
eenvaders_ext_signal_emit(void *data, Evas_Object *obj,
                          const char *emission, const char *source)
{
    evas_object_smart_callback_call(obj, emission, NULL);
}

static Eina_Bool
eenvaders_ext_param_set(void *data, Evas_Object *obj,
                        const Edje_External_Param *param)
{
    return EINA_TRUE;
}

static Eina_Bool
eenvaders_ext_param_get(void *data, const Evas_Object *obj,
                        Edje_External_Param *param)
{
    return EINA_TRUE;
}

static Evas_Object *
eenvaders_ext_content_get(void *data, const Evas_Object *obj,
                          const char *content)
{
    return NULL;
}

static void*
eenvaders_ext_params_parse(void *data, Evas_Object *obj,
                           const Eina_List *params)
{
    return NULL;
}

static void
eenvaders_ext_params_free(void *params)
{
}

static const char*
eenvaders_ext_label_get(void *data)
{
    return NULL;
}

static const char*
eenvaders_ext_description_get(void *data)
{
    return NULL;
}

static Evas_Object *
eenvaders_ext_icon_add(void *data, Evas *e)
{
    return NULL;
}

static Evas_Object *
eenvaders_ext_preview_add(void *data, Evas *e)
{
    return NULL;
}

static const char*
eenvaders_ext_translate(void *data, const char *orig)
{
    return NULL;
}
/* }}} */
/* Init/Shutdown -{{{-*/

Eina_Bool
eenvaders_init(void)
{
    unsigned int seedval = time(NULL);
    int fd;

    if ((fd = open("/dev/random", O_RDONLY)) >= 0) {
        read(fd, &seedval, sizeof(seedval));
        close(fd);
    }
    srand(seedval);

    edje_external_type_register("eenvaders", &_G.ext_type);

    return EINA_TRUE;
}

void
eenvaders_shutdown(void)
{
    edje_external_type_unregister("eenvaders");
}

EINA_MODULE_INIT(eenvaders_init);
EINA_MODULE_SHUTDOWN(eenvaders_shutdown);

/* }}} */
