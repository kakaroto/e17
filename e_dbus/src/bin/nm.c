#include <E_Nm.h>
#include <Ecore_Data.h>

E_NM *nm = NULL;
E_NMS *nms = NULL;

static void
dump_prop(E_NM_Property *prop)
{
    if (!prop) return;
    switch (prop->type)
    {
        case 'a': {
          E_NM_Property *subprop;

          printf("\n   - ");
          ecore_list_first_goto(prop->a);
          while ((subprop = ecore_list_next(prop->a)))
              dump_prop(subprop);
          break;
        }
        case 's':
        case 'o':
            printf("%s ", prop->s);
            break;
        case 'u':
            printf("%d ", prop->u);
            break;
        case 'b':
            printf("%d ", prop->b);
            break;
        case 'y':
            printf("%d ", prop->y);
            break;
        case 't':
            printf("%lld ", prop->t);
            break;
    }
}

static void
dump_values(void *value, void *data)
{
    Ecore_Hash_Node *node;

    node = value;
    printf(" - name: %s - ", (char *)node->key);
    dump_prop(node->value);
    printf("\n");
}
 
static void
dump_settings(void *value, void *data)
{
    Ecore_Hash_Node *node;

    node = value;
    printf("name: %s\n", (char *)node->key);
    printf("values:\n");
    ecore_hash_for_each_node(node->value, dump_values, NULL);
    printf("\n");
}

static int
cb_nms_connection_settings(void *data, Ecore_Hash *settings)
{
    ecore_hash_for_each_node(settings, dump_settings, NULL);
    return 1;
}

static int
cb_nms_connections(void *data, Ecore_List *list)
{
    E_NMS_Connection *conn;

    if (list)
    {
        ecore_list_first_goto(list);
        while ((conn = ecore_list_next(list)))
        {
            e_nms_connection_dump(conn);
            e_nms_connection_get_settings(conn, cb_nms_connection_settings, NULL);
        }
        ecore_list_destroy(list);
    }
    //ecore_main_loop_quit();
    return 1;
}

static int
cb_access_point(void *data, E_NM_Access_Point *ap)
{
    e_nm_access_point_dump(ap);
    e_nm_access_point_free(ap);
    return 1;
}

static int
cb_active_connection(void *data, E_NM_Active_Connection *conn)
{
    e_nm_active_connection_dump(conn);
    e_nm_active_connection_free(conn);
    return 1;
}

static int
cb_ip4_config(void *data, E_NM_IP4_Config *config)
{
    e_nm_ip4_config_dump(config);
    e_nm_ip4_config_free(config);
    return 1;
}

static int
cb_get_devices(void *data, Ecore_List *list)
{
    E_NM_Device *device;

    if (list)
    {
        ecore_list_first_goto(list);
        while ((device = ecore_list_next(list)))
        {
            e_nm_device_dump(device);
            if (device->device_type == E_NM_DEVICE_TYPE_WIRELESS)
            {
                e_nm_access_point_get(nm, device->wireless.active_access_point, cb_access_point, NULL);
                e_nm_ip4_config_get(nm, device->ip4_config, cb_ip4_config, NULL);
            }
        }
        ecore_list_destroy(list);
    }
    //ecore_main_loop_quit();
    return 1;
}

static int
cb_nms(void *data, E_NMS *reply)
{
    if (!reply)
    {
        ecore_main_loop_quit();
        return 1;
    }
    nms = reply;
    e_nms_dump(nms);
    e_nms_list_connections(nms, cb_nms_connections, nms);
    return 1;
}

static int
cb_nm(void *data, E_NM *reply)
{
    if (!reply)
    {
        ecore_main_loop_quit();
        return 1;
    }
    nm = reply;
    /*
    e_nm_dump(nm);
    if (nm->active_connections)
    {
        const char *conn;
        ecore_list_first_goto(nm->active_connections);
        while ((conn = ecore_list_next(nm->active_connections)))
            e_nm_active_connection_get(nm, conn, cb_active_connection, NULL);
    }
    e_nm_get_devices(nm, cb_get_devices, nm);
    */
    e_nms_get(nm, cb_nms, nm);
    return 1;
}
   

int 
main(int argc, char **argv)
{
    ecore_init();
    eina_stringshare_init();
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
    eina_stringshare_shutdown();
    ecore_shutdown();
    return 0;
}
