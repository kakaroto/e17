#include <E_Nm.h>
#include <Ecore_Data.h>

E_NM *nm = NULL;
E_NMS *nms = NULL;

static int
cb_nms_connections(void *data, Ecore_List *list)
{
    E_NMS_Connection *conn;

    if (list)
    {
        ecore_list_first_goto(list);
        while ((conn = ecore_list_next(list)))
            e_nms_connection_dump(conn);
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
                e_nm_access_point_get(nm, device->wireless.active_access_point, cb_access_point, NULL);
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
    const char *conn;

    if (!reply)
    {
        ecore_main_loop_quit();
        return 1;
    }
    nm = reply;
    e_nm_dump(nm);
    ecore_list_first_goto(nm->active_connections);
    while ((conn = ecore_list_next(nm->active_connections)))
        e_nm_active_connection_get(nm, conn, cb_active_connection, NULL);
    e_nm_get_devices(nm, cb_get_devices, nm);
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
