#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property ip4_config_properties[] = {
#if 0
  { .name = "Addresses", .sig = "aau", .offset = offsetof(E_NM_IP4_Config, addresses) },
  { .name = "Nameservers", .sig = "au", .offset = offsetof(E_NM_IP4_Config, nameservers) },
#endif
  { .name = "Domains", .sig = "as", .offset = offsetof(E_NM_IP4_Config, domains) },
#if 0
  { .name = "Routes", .sig = "aau", .offset = offsetof(E_NM_IP4_Config, routes) },
#endif
  { .name = NULL }
};

EAPI int
e_nm_ip4_config_get(E_NM *nm, const char *ip4_config,
                    int (*cb_func)(void *data, E_NM_IP4_Config *config),
                    void *data)
{
  E_NM_Internal            *nmi;
  E_NM_IP4_Config_Internal *config;
  Property_Data            *d;

  nmi = (E_NM_Internal *)nm;
  config = calloc(1, sizeof(E_NM_IP4_Config_Internal));
  config->nmi = nmi;
  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = config;
  d->property = ip4_config_properties;
  d->object = strdup(ip4_config);

  return e_nm_ip4_config_properties_get(nmi->conn, d->object, d->property->name, d->property->func, d) ? 1 : 0;
}

EAPI void
e_nm_ip4_config_free(E_NM_IP4_Config *config)
{
  if (!config) return;
  if (config->addresses) ecore_list_destroy(config->addresses);
  if (config->nameservers) ecore_list_destroy(config->nameservers);
  if (config->domains) ecore_list_destroy(config->domains);
  if (config->routes) ecore_list_destroy(config->routes);
  free(config);
}

EAPI void
e_nm_ip4_config_dump(E_NM_IP4_Config *config)
{
  if (!config) return;
  printf("E_NM_IP4_Config:\n");
  if (config->addresses)
  {
  }
  if (config->nameservers)
  {
  }
  if (config->domains)
  {
    const char *domain;

    printf("domains:\n");
    ecore_list_first_goto(config->domains);
    while ((domain = ecore_list_next(config->domains)))
      printf(" - %s\n", domain);
  }
  if (config->routes)
  {
  }
  printf("\n");
}

