#include "E_Nm.h"
#include "e_nm_private.h"
#include "e_dbus_private.h"
#include <string.h>

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
  unsigned int *u;
  const char   *domain;
  Ecore_List   *list;

  if (!config) return;
  E_DBUS_LOG_INFO("E_NM_IP4_Config:");
  if (config->addresses)
  {
    char buffer[1024];

    E_DBUS_LOG_INFO("addresses  :");
    ecore_list_first_goto(config->addresses);
    while ((list = ecore_list_next(config->addresses)))
    {
      strcpy(buffer, " -");
      ecore_list_first_goto(list);
      while ((u = ecore_list_next(list)))
	{
	   strcat(buffer, " ");
	   strcat(buffer, ip4_address2str(*u));
	}
      E_DBUS_LOG_INFO(buffer);
    }
  }
  if (config->nameservers)
  {
    E_DBUS_LOG_INFO("nameservers:");
    ecore_list_first_goto(config->nameservers);
    while ((u = ecore_list_next(config->nameservers)))
      E_DBUS_LOG_INFO(" - %s", ip4_address2str(*u));
  }
  if (config->domains)
  {
    E_DBUS_LOG_INFO("domains    :");
    ecore_list_first_goto(config->domains);
    while ((domain = ecore_list_next(config->domains)))
      E_DBUS_LOG_INFO(" - %s", domain);
  }
  if (config->routes)
  {
    E_DBUS_LOG_INFO("routes     :");
    ecore_list_first_goto(config->routes);
    while ((list = ecore_list_next(config->routes)))
    {
      char buffer[1024];

      strcpy(buffer, " -");
      ecore_list_first_goto(list);
      while ((u = ecore_list_next(list)))
	{
	   strcat(buffer, " ");
	   strcat(buffer, ip4_address2str(*u));
	}
      E_DBUS_LOG_INFO("%s", buffer);
    }
  }
  E_DBUS_LOG_INFO("");
}

