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
cb_get_devices(void *data, Ecore_List *list)
{
    E_NM_Device *device;

    if (list)
    {
        ecore_list_first_goto(list);
        while ((device = ecore_list_next(list)))
            e_nm_device_dump(device);
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
    e_nm_dump(nm);
    //e_nm_get_devices(nm, cb_get_devices, nm);
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
    e_nm_free(nm);
    e_nms_free(nms);
   
    e_dbus_shutdown();
    eina_stringshare_shutdown();
    ecore_shutdown();
    return 0;
}
