/** @file libexalt_dbus.c */
#include "Exalt_dbus.h"
#include "libexalt_dbus_private.h"

void _exalt_dbus_notify(void *data,DBusMessage *msg);
void _exalt_dbus_scan_notify(void *data, DBusMessage *msg);

int exalt_dbus_log_domain;

static int init = 0;



/**
 * @addtogroup General
 * @{
 */

/**
 * @brief Initialise the library
 * Don't forget to create a connection with Exalt_DBus_Connect() after
 * @return Returns 0 if failed (if the exalt's service doesn't exist), else 1 or more
 */
int exalt_dbus_init()
{
    if( ++init != 1) return init;
    ecore_init();
    e_dbus_init();
    eina_init();
    eina_log_level_set(EINA_LOG_LEVEL_DBG);

    exalt_dbus_log_domain = eina_log_domain_register("EXALT-DBUS",EINA_COLOR_LIGHTRED);

    return 1;
}

/**
 * @brief test if the exalt dbus service exists
 * @param conn a connection
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_exalt_service_exists(Exalt_DBus_Conn *conn)
{
    return exalt_dbus_service_exists(conn,EXALTD_SERVICE);
}

/**
 * @brief test if a dbus service exists
 * @param conn a connection
 * @param service_name the name of the service
 * @return Returns 1 if succes, else 0
 */
int exalt_dbus_service_exists(Exalt_DBus_Conn *conn, const char* service_name)
{
    EXALT_ASSERT_RETURN(conn!=NULL);

    DBusMessageIter args;
    int bool;

    DBusPendingCall *dbus_call = e_dbus_name_has_owner(conn->e_conn, service_name, NULL, NULL);
    dbus_pending_call_block(dbus_call);
    DBusMessage *reply = dbus_pending_call_steal_reply(dbus_call);
    if (!reply) return 0;

    dbus_message_iter_init(reply, &args);
    if(DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args))
    {
        dbus_pending_call_ref(dbus_call);
        return 0;
    }
    dbus_message_iter_get_basic(&args, &bool);
    if(!bool)
    {
        dbus_pending_call_ref(dbus_call);
        return 0;
    }
    dbus_pending_call_ref(dbus_call);

    return 1;
}

/**
 * @brief Create a connection
 * @return Returns a new DBus connection
 */
Exalt_DBus_Conn* exalt_dbus_connect()
{
    Exalt_DBus_Conn* conn;
    DBusError err;
    conn = calloc(1,sizeof(Exalt_DBus_Conn));
    //initialise the errors
    dbus_error_init(&err);
    // connect to the bus
    conn->conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
    EXALT_ASSERT_CUSTOM_RET(conn->conn != NULL,
            EXALT_FREE(conn); return NULL;);

    conn->e_conn = e_dbus_connection_setup(conn->conn);
    EXALT_ASSERT_CUSTOM_RET(conn->e_conn != NULL,
            EXALT_FREE(conn); return NULL;);

    conn->msg_id = 1;

    conn -> response_notify = calloc(1,sizeof(exalt_dbus_response_data));

    return conn;
}

/**
 * @brief Free an exalt DBus connection
 * @param conn a connection
 */
void exalt_dbus_free(Exalt_DBus_Conn** conn)
{
    EXALT_ASSERT_RETURN_VOID(conn != NULL);
    EXALT_ASSERT_RETURN_VOID(*conn != NULL);
    EXALT_ASSERT_RETURN_VOID( (*conn)->e_conn != NULL);

    e_dbus_signal_handler_del((*conn)->e_conn, (*conn)->scan_notify_handler);
    e_dbus_signal_handler_del((*conn)->e_conn, (*conn)->notify_handler);

    e_dbus_connection_close((*conn)->e_conn);
    EXALT_FREE((*conn)->notify);
    EXALT_FREE((*conn)->scan_notify);
    EXALT_FREE(*conn);
}

/**
 * @brief shutdown the library
 */
void exalt_dbus_shutdown()
{
    if( --init ) return;
    eina_log_domain_unregister(exalt_dbus_log_domain);
    exalt_dbus_log_domain = -1;
    e_dbus_shutdown();
    eina_shutdown();
    ecore_shutdown();
}

/**
 * @brief set the callback function used to notify a change, for example if an IP address change. <br>
 * See Exalt_Enum_Action for more information.
 * @param conn a connection
 * @param cb the callback function
 * @param user_data the user data
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_notify_set(Exalt_DBus_Conn* conn, exalt_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN(conn!=NULL);

    if(conn->notify_handler)
        e_dbus_signal_handler_del(conn->e_conn, conn->notify_handler);

    conn -> notify = malloc(sizeof(exalt_dbus_notify_data));
    conn -> notify -> cb = cb;
    conn -> notify -> user_data = user_data;

    conn->notify_handler = e_dbus_signal_handler_add(conn->e_conn,
            EXALTD_SERVICE, EXALTD_PATH_NOTIFY,
            EXALTD_INTERFACE_NOTIFY, "notify",
            _exalt_dbus_notify, conn);

    if(conn->notify_handler)
        return 1;
    else
        return 0;
}

/**
 * @brief set the callback function used to return the result of a network scan
 * @param conn
 * @param cb the callback function
 * @param user_data the user data
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_scan_notify_set(Exalt_DBus_Conn* conn, exalt_scan_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN(conn!=NULL);

    if(conn->scan_notify_handler)
        e_dbus_signal_handler_del(conn->e_conn, conn->scan_notify_handler);

    conn -> scan_notify = malloc(sizeof(exalt_dbus_scan_notify_data));
    conn -> scan_notify -> cb = cb;
    conn -> scan_notify -> user_data = user_data;

    conn->scan_notify_handler = e_dbus_signal_handler_add(conn->e_conn,
            EXALTD_SERVICE, EXALTD_PATH_NOTIFY,
            EXALTD_INTERFACE_NOTIFY, "scan_notify",
            _exalt_dbus_scan_notify, conn);

    if(conn->scan_notify_handler)
        return 1;
    else
        return 0;
}

/**
 * @brief set the callback function used to return a response
 * @param conn
 * @param cb the callback function
 * @param user_data the user data
 */
void exalt_dbus_response_notify_set(Exalt_DBus_Conn* conn, exalt_response_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    conn -> response_notify -> cb = cb;
    conn -> response_notify -> user_data = user_data;
}


/** @} */

