/* Interface */

#include "enna.h"
static void _monitor_cb(void *data, Ecore_File_Monitor *em,
        Ecore_File_Event event, const char *path);
static int em_init(Enna_Module *em);
static int em_shutdown(Enna_Module *em);

typedef struct _Enna_Module_Polling Enna_Module_Polling;

struct _Enna_Module_Polling
{
    Evas *e;
    Enna_Module *em;
    Ecore_File_Monitor *monitor;
};

static Enna_Module_Polling *mod;

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "polling"
};

/* Static functions */
static void _monitor_cb(void *data, Ecore_File_Monitor *em,
        Ecore_File_Event event, const char *path)
{

    Enna_Module_Polling *mod = data;

    if (!mod || !path || strncmp(path, "/etc/mtab", 6))
        return;

    switch (event)
    {
        case ECORE_FILE_EVENT_MODIFIED:
            break;
        default:
            return;
    }
}

/* Module interface */

static int em_init(Enna_Module *em)
{
    mod = calloc(1, sizeof(Enna_Module_Polling));
    mod->em = em;
    em->mod = mod;

    mod->monitor = ecore_file_monitor_add("/etc/mtab", _monitor_cb, mod);

    return 1;
}

static int em_shutdown(Enna_Module *em)
{

    if (mod->monitor)
        ecore_file_monitor_del(mod->monitor);

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
