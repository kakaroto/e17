#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <Ecore.h>

#include <E_Nm.h>

E_NM *nm = NULL;
E_NMS *nms = NULL;

static void *
memdup(const void *p, size_t n)
{
    void *q;

    q = malloc(n);
    memcpy(q, p, n);
    return q;
}

static void
dump_variant(E_NM_Variant *var)
{
    if (!var) return;
    switch (var->type)
    {
        case 'a': {
          E_NM_Variant *subvar;
	  Eina_List    *l;

          printf("a:");
          EINA_LIST_FOREACH(var->a, l, subvar)
          {
              dump_variant(subvar);
              printf(";");
          }
          printf("\n");
          break;
        }
        case 's':
            printf("s:%s", var->s);
            break;
        case 'o':
            printf("o:%s", var->s);
            break;
        case 'u':
            printf("u:%d", var->u);
            break;
        case 'b':
            printf("b:%d", var->b);
            break;
        case 'y':
            printf("y:%d", var->y);
            break;
        case 't':
            printf("t:%lld", var->t);
            break;
    }
}

static Eina_Bool
dump_values(const Eina_Hash *hash __UNUSED__, const void *key, void *value, void *data __UNUSED__)
{
    printf(" - name: %s - ", (char *)key);
    dump_variant(value);
    printf("\n");

    return 1;
}
 
static Eina_Bool
dump_settings(const Eina_Hash *hash __UNUSED__, const void *key, void *value, void *fdata __UNUSED__)
{
    printf("name: %s\n", (char *)key);
    printf("values:\n");
    eina_hash_foreach(value, dump_values, NULL);
    printf("\n");

    return 1;
}

static int
cb_nms_connection_secrets(void *data __UNUSED__, Eina_Hash *secrets)
{
    printf("Secrets:\n");
    if (secrets)
        eina_hash_foreach(secrets, dump_settings, NULL);
    return 1;
}

static int
cb_nms_connection_settings(void *data, Eina_Hash *settings)
{
    printf("Settings:\n");
    if (settings)
    {
        if (eina_hash_find(settings, "802-11-wireless-security"))
            e_nms_connection_secrets_get_secrets(data, "802-11-wireless-security", NULL, 0, cb_nms_connection_secrets, NULL);
        eina_hash_foreach(settings, dump_settings, NULL);
	eina_hash_free(settings);
    }
    return 1;
}

static int
cb_nms_connections(void *data __UNUSED__, Eina_List *list)
{
    E_NMS_Connection *conn;
    Eina_List        *l;

    EINA_LIST_FOREACH(list, l, conn)
    {
        e_nms_connection_dump(conn);
        e_nms_connection_get_settings(conn, cb_nms_connection_settings, conn);
    }
    //ecore_main_loop_quit();
    //e_nms_list_connections(nms, cb_nms_connections, nms);
    return 1;
}

static int
cb_access_point(void *data __UNUSED__, E_NM_Access_Point *ap)
{
    e_nm_access_point_dump(ap);
    e_nm_access_point_free(ap);
    return 1;
}

static int
cb_activate_connection(void *data, E_NM_Device *device)
{
    E_NM_Active_Connection *conn;

    conn = data;
    sleep(1);
    e_nm_active_connection_dump(conn);
    e_nm_activate_connection(nm, conn->service_name, conn->path, device, conn->specific_object);
    return 1;
}

static int
cb_active_connection(void *data __UNUSED__, E_NM_Active_Connection *conn)
{
    const char *device;
    Eina_List  *l;

    e_nm_deactivate_connection(nm, conn);
    EINA_LIST_FOREACH(conn->devices, l, device)
         e_nm_device_get(nm, device, cb_activate_connection, conn);
    /*
    e_nm_active_connection_dump(conn);
    e_nm_active_connection_free(conn);
    */
    return 1;
}

static int
cb_ip4_config(void *data __UNUSED__, E_NM_IP4_Config *config)
{
    e_nm_ip4_config_dump(config);
    e_nm_ip4_config_free(config);
    return 1;
}

static int
cb_access_points(void *data __UNUSED__, Eina_List *list)
{
    E_NM_Access_Point *ap;

    EINA_LIST_FREE(list, ap)
    {
        e_nm_access_point_dump(ap);
	e_nm_access_point_free(ap);
    }
    return 1;
}

static int
cb_get_devices(void *data __UNUSED__, Eina_List *list)
{
    E_NM_Device *device;
    Eina_List   *l;

    EINA_LIST_FOREACH(list, l, device)
    {
        e_nm_device_dump(device);
        if (device->device_type == E_NM_DEVICE_TYPE_WIRELESS)
        {
            /*
            e_nm_device_wireless_get_access_points(device, cb_access_points, NULL);
            e_nm_access_point_get(nm, device->wireless.active_access_point, cb_access_point, NULL);
            e_nm_ip4_config_get(nm, device->ip4_config, cb_ip4_config, NULL);
            */
        }
    }
    //ecore_main_loop_quit();
    return 1;
}

static int
cb_nms(void *data __UNUSED__, E_NMS *reply)
{
    Eina_Hash *settings, *values;
    E_NM_Variant variant;
    const char ssid[] = { };
    const char *bssids[] = { };

    settings = eina_hash_string_small_new(EINA_FREE_CB(eina_hash_free));
    /* connection */
    values = eina_hash_string_small_new(EINA_FREE_CB(e_nm_variant_free));
    eina_hash_add(settings, strdup("connection"), values);
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 't';
    variant.t = 1228201388;
    eina_hash_add(values, strdup("timestamp"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("");
    eina_hash_add(values, strdup("id"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("");
    eina_hash_add(values, strdup("uuid"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("802-11-wireless");
    eina_hash_add(values, strdup("type"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 'b';
    variant.b = 0;
    eina_hash_add(values, strdup("autoconnect"), memdup(&variant, sizeof(E_NM_Variant)));
    /* 802-11-wireless */
    values = eina_hash_string_small_new(EINA_FREE_CB(e_nm_variant_free));
    eina_hash_add(settings, strdup("802-11-wireless"), values);
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("mode");
    eina_hash_add(values, strdup("infrastructure"), memdup(&variant, sizeof(E_NM_Variant)));
    eina_hash_add(values, strdup("ssid"), e_nm_variant_array_new('y', ssid, sizeof(ssid) / sizeof(ssid[0])));
    eina_hash_add(values, strdup("seen-bssids"), e_nm_variant_array_new('s', bssids, sizeof(bssids) / sizeof(bssids[0])));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("802-11-wireless-security");
    eina_hash_add(values, strdup("security"), memdup(&variant, sizeof(E_NM_Variant)));
    /* ipv4 */
    values = eina_hash_string_small_new(EINA_FREE_CB(e_nm_variant_free));
    eina_hash_add(settings, strdup("ipv4"), values);
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("auto");
    eina_hash_add(values, strdup("method"), memdup(&variant, sizeof(E_NM_Variant)));
    /* 802-11-wireless-security */
    values = eina_hash_string_small_new(EINA_FREE_CB(e_nm_variant_free));
    eina_hash_add(settings, strdup("802-11-wireless-security"), values);
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("none");
    eina_hash_add(values, strdup("key-mgmt"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("open");
    eina_hash_add(values, strdup("auth-alg"), memdup(&variant, sizeof(E_NM_Variant)));
    memset(&variant, 0, sizeof(E_NM_Variant));
    variant.type = 's';
    variant.s = strdup("");
    eina_hash_add(values, strdup("wep-key0"), memdup(&variant, sizeof(E_NM_Variant)));

    nms = reply;
    e_nms_dump(nms);
    //ecore_hash_for_each_node(settings, dump_settings, NULL);
    //e_nms_system_add_connection(nms, settings);
    //sleep(1);
    e_nms_list_connections(nms, cb_nms_connections, nms);
    return 1;
}

static int
cb_nm(void *data __UNUSED__, E_NM *reply)
{
    if (!reply)
    {
        ecore_main_loop_quit();
        return 1;
    }
    nm = reply;
    /*
    e_nm_wireless_enabled_set(nm, 1);
    if (nm->active_connections)
    {
        const char *conn;
        ecore_list_first_goto(nm->active_connections);
        while ((conn = ecore_list_next(nm->active_connections)))
            e_nm_active_connection_get(nm, conn, cb_active_connection, NULL);
    }
    */
    /*
    e_nm_get_devices(nm, cb_get_devices, nm);
    */
    e_nms_get(nm, E_NMS_CONTEXT_SYSTEM, cb_nms, NULL);
    return 1;
}
   

int 
main()
{
    ecore_init();
    eina_init();
    e_dbus_init();
   
    if (!e_nm_get(cb_nm, (void *)0xdeadbeef))
    {
        printf("Error connecting to system bus. Is it running?\n");
        return 1;
    }
   
    ecore_main_loop_begin();
    e_nms_free(nms);
    e_nm_free(nm);
   
    e_dbus_shutdown();
    eina_shutdown();
    ecore_shutdown();
    return 0;
}
