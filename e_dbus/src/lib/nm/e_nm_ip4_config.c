#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "E_Nm.h"
#include "e_nm_private.h"
#include "e_dbus_private.h"

static const Property ip4_config_properties[] = {
  { .name = "Addresses", .sig = "aau", .offset = offsetof(E_NM_IP4_Config, addresses) },
  { .name = "Nameservers", .sig = "au", .offset = offsetof(E_NM_IP4_Config, nameservers) },
  { .name = "Domains", .sig = "as", .offset = offsetof(E_NM_IP4_Config, domains) },
  { .name = "Routes", .sig = "aau", .offset = offsetof(E_NM_IP4_Config, routes) },
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

  if (!ip4_config) return 0;

  nmi = (E_NM_Internal *)nm;
  config = calloc(1, sizeof(E_NM_IP4_Config_Internal));
  config->nmi = nmi;
  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = config;
  d->property = ip4_config_properties;
  d->service = E_NM_SERVICE;
  d->object = strdup(ip4_config);
  d->interface = E_NM_INTERFACE_IP4CONFIG;

  return property_get(nmi->conn, d);
}

EAPI void
e_nm_ip4_config_free(E_NM_IP4_Config *config)
{
  Eina_List *l;
  void *data;

  if (!config) return;
  EINA_LIST_FREE(config->addresses, l)
    EINA_LIST_FREE(l, data)
      free(data);
  EINA_LIST_FREE(config->nameservers, data)
    free(data);
  EINA_LIST_FREE(config->domains, data)
    free(data);
  EINA_LIST_FREE(config->routes, l)
    EINA_LIST_FREE(l, data)
      free(data);
  free(config);
}

EAPI void
e_nm_ip4_config_dump(E_NM_IP4_Config *config)
{
  unsigned int *u;
  const char   *domain;
  Eina_List    *list, *l;

  if (!config) return;
  printf("E_NM_IP4_Config:\n");
  printf("addresses  :\n");
  EINA_LIST_FOREACH(config->addresses, l, list)
  {
    char buffer[1024];
    Eina_List *l2;

    strcpy(buffer, " -");
    EINA_LIST_FOREACH(list, l2, u)
    {
      strcat(buffer, " ");
      strcat(buffer, ip4_address2str(*u));
    }
    printf("%s\n", buffer);
  }
  printf("nameservers:\n");
  EINA_LIST_FOREACH(config->nameservers, l, u)
  {
    printf(" - %s\n", ip4_address2str(*u));
  }
  printf("domains    :\n");
  EINA_LIST_FOREACH(config->domains, l, domain)
  {
    printf(" - %s\n", domain);
  }
  printf("routes     :\n");
  EINA_LIST_FOREACH(config->routes, l, list)
  {
    char buffer[1024];
    Eina_List *l2;

    strcpy(buffer, " -");
    EINA_LIST_FOREACH(list, l2, u)
    {
      strcat(buffer, " ");
      strcat(buffer, ip4_address2str(*u));
    }
    printf("%s\n", buffer);
  }
}

