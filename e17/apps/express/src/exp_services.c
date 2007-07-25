#include "Express.h"

static Ecore_Hash *services = NULL;
static int init_count = 0;

static void exp_service_node_free(void *data);

int
exp_services_init(void)
{
    init_count++;
    if (!services) 
    {
        services = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_value_cb_set(services, exp_service_node_free);
    }
    return init_count;
}

void
exp_services_shutdown(void)
{
    if (init_count > 0)
    {
        init_count--;
        ecore_hash_destroy(services);
        services = NULL;
    }
}

Exp_Service *
exp_service_find(const char *name)
{
    return ecore_hash_get(services, name);
}

Exp_Service *
exp_service_new(const char *name)
{
    Exp_Service *exp = NULL;

    if (!name) return NULL;

    exp = calloc(1, sizeof(Exp_Service));
    exp->name = strdup(name);
    ecore_hash_set(services, exp->name, exp);

    return exp;
}

void
exp_service_del(Exp_Service *exp)
{
    if (!exp) return;

    ecore_hash_remove(services, exp->name);

    if (exp->name) free(exp->name);
    exp->name = NULL;

    if (exp->colour) free(exp->colour);
    exp->colour = NULL;
    free(exp);
    exp = NULL;
}

void
exp_service_colour_set(Exp_Service *exp, const char *colour)
{
    if (!exp || !colour) return;
    if (exp->colour) free(exp->colour);
    exp->colour = strdup(colour);
}

const char *
exp_service_colour_get(Exp_Service *exp)
{
    return (exp ? exp->colour : NULL);
}

void
exp_service_capabilities_set(Exp_Service *exp, int caps)
{
    if (!exp) return;
    exp->capabilities = caps;
}

int
exp_service_capabilities_get(Exp_Service *exp)
{
    return (exp ? exp->capabilities : 0);
}

static void
exp_service_node_free(void *data)
{
    if (data) exp_service_del(data);
}


