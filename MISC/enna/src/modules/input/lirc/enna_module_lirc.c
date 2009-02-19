/* Interface */

#include "enna.h"
#include <lirc/lirc_client.h>

#define ENNA_MODULE_NAME "lirc"

static void _class_init(int dummy);
static void _class_shutdown(int dummy);
static void _class_event_cb_set(void (*event_cb)(void *data, char *event), void *data);
static int em_init(Enna_Module *em);
static int em_shutdown(Enna_Module *em);

typedef struct _Enna_Module_Lirc Enna_Module_Lirc;

struct _Enna_Module_Lirc
{
    Evas *e;
    Enna_Module *em;
    int fd;
    Ecore_Fd_Handler *fd_handler;
    const char *config_filename;
    struct lirc_config *lirc_config;
    void (*event_cb)(void *data, char *event);
    void *event_cb_data;
};

static Enna_Module_Lirc *mod;

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_INPUT,
    ENNA_MODULE_NAME
};

static Enna_Class_Input class =
{ "lirc",
{ _class_init, _class_shutdown, _class_event_cb_set,
}
};

static int _lirc_code_received(void *data, Ecore_Fd_Handler * fd_handler)
{
    char *code, *event;
    int ret = -1;

    while (lirc_nextcode(&code) == 0 && code != NULL)
    {
        while ((ret = lirc_code2char(mod->lirc_config, code, &event)) == 0
                && event != NULL)
        {
            if (mod->event_cb)
                mod->event_cb(mod->event_cb_data, event);
        }
    }
    return 1;
}

static void _class_init(int dummy)
{
    int fd;
    char cfg_file[4096];
    struct lirc_config *config;

    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "class LIRC INPUT init");

    if ((fd = lirc_init("enna", 1)) == -1)
    {
        enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                "could not initialize LIRC support");
        return;
    }

    snprintf(cfg_file, sizeof(cfg_file), "%s/.lircrc",
            enna_util_user_home_get());
    if (lirc_readconfig(cfg_file, &config, NULL) != 0)
    {
        lirc_deinit();
        enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                "could not find Lirc config file");
        return;
    }

    mod->config_filename = evas_stringshare_add(cfg_file);
    mod->lirc_config = config;
    fcntl(fd, F_SETFL, O_NONBLOCK);
    fcntl(fd, F_SETFD, FD_CLOEXEC);

    mod->fd_handler = ecore_main_fd_handler_add(fd, ECORE_FD_READ,
            _lirc_code_received, NULL, NULL, NULL);
}

static void _class_event_cb_set(void (*event_cb)(void *data, char *event), void *data)
{
    mod->event_cb_data = data;
    mod->event_cb = event_cb;
}

static void _class_shutdown(int dummy)
{

    if (mod->fd_handler)
    {

        lirc_freeconfig(mod->lirc_config);
        enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "class LIRC INPUT shutdown");
        evas_stringshare_del(mod->config_filename);
        ecore_main_fd_handler_del(mod->fd_handler);
        lirc_deinit();
    }
}

/* Module interface */

static int em_init(Enna_Module *em)
{

    mod = calloc(1, sizeof(Enna_Module_Lirc));
    mod->em = em;
    em->mod = mod;

    enna_input_class_register(em, &class);

    return 1;
}

static int em_shutdown(Enna_Module *em)
{

    return 1;
}

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    if (!em_init(em))
        return;
}

void module_shutdown(Enna_Module *em)
{
    em_shutdown(em);
}
