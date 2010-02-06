/** @file libexalt_dbus.c */
#include "Exalt_DBus.h"
#include "libexalt_dbus_private.h"

void _exalt_dbus_notify(void *data,DBusMessage *msg);
void _exalt_dbus_scan_notify(void *data, DBusMessage *msg);

int exalt_dbus_log_domain;

static int init = 0;

static void _exalt_dbus_start_server_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_get_name_owner(void *data, DBusMessage *msg, DBusError *err);
static void _exalt_dbus_start_server(Exalt_DBus_Conn *conn);
static void _exalt_dbus_report_connect(Exalt_DBus_Conn *conn, Eina_Bool success);
static void _exalt_dbus_name_owner_changed(void *data, DBusMessage *msg);

/**
 * @addtogroup General
 * @{
 */

/**
 * @brief Initialise the library
 * Don't forget to create a connection with exalt_dbus_connect() after
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
 *
 * If exaltd was down, it is tried to start it using DBus activation,
 * then the connection is retried.
 *
 * This call is asynchronous and will not block, instead it will be in
 * "not connected" state until @a connect_cb is called with either
 * success or failure. On failure, then no methods should be
 * called. On success you're now able to communicate with the server.
 *
 * Usually you should listen for server death/disconenction with
 * exalt_dbus_on_server_die_callback_set().
 *
 * @param connect_cb function to call to report connection success or
 *        failure. Do not call any other exalt_dbus method until
 *        this function returns. The first received parameter is the
 *        given argument @a data. Must @b not be @c NULL. 
 * @param data context to give back to @a connect_cb. May be @c NULL.
 * @param free_data function used to release @a data resources, if
 *        any. May be @c NULL. If this function exists, it will be
 *        explicitly calls exalt_dbus_frr().
 *
 * @return client instance or NULL if failed. If @a connect_cb is
 *         missing it returns @c NULL. If it fail for other
 *         conditions, @c NULL is also returned. The client instance 
 *         is not ready to be used until @a connect_cb is called.
 */
Exalt_DBus_Conn* exalt_dbus_connect(Exalt_DBus_Connect_Cb connect_cb, void *data, Eina_Free_Cb free_data_cb)
{
    Exalt_DBus_Conn* conn;
    DBusError err;
    conn = calloc(1,sizeof(Exalt_DBus_Conn));
    conn->msg_id = 1;
    conn->connect.cb = connect_cb;
    conn->connect.data = data;
    conn->connect.free_data = free_data_cb;

    EXALT_ASSERT_RETURN(connect_cb);

    //initialise the errors
    dbus_error_init(&err);
    // connect to the bus
    conn->conn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
    EXALT_ASSERT_CUSTOM_RET(conn->conn != NULL,
            EXALT_FREE(conn); return NULL;);

    conn->e_conn = e_dbus_connection_setup(conn->conn);
    EXALT_ASSERT_CUSTOM_RET(conn->e_conn != NULL,
            EXALT_FREE(conn); return NULL;);

    conn->name_owner_changed_handler = e_dbus_signal_handler_add(
            conn->e_conn, FDO_SERVICE, FDO_PATH, FDO_INTERFACE,
            "NameOwnerChanged", _exalt_dbus_name_owner_changed, conn);

    conn -> response_notify = calloc(1,sizeof(exalt_dbus_response_data));

    conn->pending_get_name_owner = e_dbus_get_name_owner(
            conn->e_conn, EXALTD_SERVICE, _exalt_dbus_get_name_owner,
            conn);

    return conn;
}

/**
 * Sets the callback to report server died.
 *
 * When server dies there is nothing you can do, just release
 * resources with exalt_dbus_free() and probably try to
 * connect again.
 *
 * Usually you should set this callback and handle this case, it does
 * happen!
 *
 * @param conn the connection to monitor. Must @b not be @c
 *        NULL.
 * @param server_die_cb function to call back when server dies. The
 *        first parameter will be the argument @a data. May be @c
 *        NULL.
 * @param data context to give back to @a server_die_cb. May be @c
 *        NULL.
 * @param free_data used to release @a data resources after user calls
 *        exalt_dbus_free().
 */
void exalt_dbus_on_server_die_callback_set(Exalt_DBus_Conn *conn, Exalt_DBus_Die_Cb server_die_cb, const void *data, Eina_Free_Cb free_data)
{
    EXALT_ASSERT_RETURN_VOID(conn != NULL);

    if (conn->die.free_data)
        conn->die.free_data(conn->die.data);

    conn->die.cb = server_die_cb;
    conn->die.data = (void *)data;
    conn->die.free_data = free_data;
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

    if ((*conn)->die.free_data)
        (*conn)->die.free_data((*conn)->die.data);

    if ((*conn)->connect.free_data)
        (*conn)->connect.free_data((*conn)->connect.data);

   if ((*conn)->scan_notify_handler)
     e_dbus_signal_handler_del((*conn)->e_conn, (*conn)->scan_notify_handler);
   if ((*conn)->notify_handler)
     e_dbus_signal_handler_del((*conn)->e_conn, (*conn)->notify_handler);
   if ((*conn)->name_owner_changed_handler)
     e_dbus_signal_handler_del((*conn)->e_conn, (*conn)->name_owner_changed_handler);

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


    static inline Eina_Bool 
__dbus_iter_type_check(int type, int expected, const char *expected_name)
{
    if (type == expected)
        return EINA_TRUE;

    EXALT_LOG_ERR("expected type %s (%c) but got %c instead!",
            expected_name, expected, type);

    return EINA_FALSE;
}
#define _dbus_iter_type_check(t, e) __dbus_iter_type_check(t, e, #e)


    static void
_exalt_dbus_start_server_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Conn *conn = data;
    DBusMessageIter iter;
    dbus_uint32_t ret;
    int t;

    EXALT_ASSERT_RETURN_VOID(conn != NULL);

    EXALT_DBUS_ERROR_PRINT(error);

    EXALT_ASSERT_RETURN_VOID(msg != NULL);

    conn->pending_start_service_by_name = NULL;

    dbus_message_iter_init(msg, &iter);
    t = dbus_message_iter_get_arg_type(&iter);
    if (!_dbus_iter_type_check(t, DBUS_TYPE_UINT32))
        goto error;

    dbus_message_iter_get_basic(&iter, &ret);
    if ((ret != 1) && (ret != 2))
    {
        EXALT_LOG_ERR("Error starting Exalt daemon service by its name: retcode %u",
                ret);
        goto error;
    }

    conn->server_started = 1;
    EXALT_LOG_DBG("Exalt daemon service started successfully (%d), now request its name",
            ret);

    if (conn->pending_get_name_owner)
    {
        EXALT_LOG_DBG("already requesting name owner, cancel and try again");
        dbus_pending_call_cancel(conn->pending_get_name_owner);
    }

    conn->pending_get_name_owner = e_dbus_get_name_owner
        (conn->e_conn, EXALTD_SERVICE, _exalt_dbus_get_name_owner,
         conn);
    if (!conn->pending_get_name_owner)
    {
        EXALT_LOG_ERR("could not create a get_name_owner request.");
        goto error;
    }

    return;

error:
    EXALT_LOG_ERR("failed to start Exalt daemon DBus service by its name.");
    _exalt_dbus_report_connect(conn, 0);
}

    static void
_exalt_dbus_get_name_owner(void *data, DBusMessage *msg, DBusError *error)
{
    DBusMessageIter iter;
    const char *uid;
    Exalt_DBus_Conn *conn = data;
    int t;

    conn->pending_get_name_owner = NULL;

    if (dbus_error_is_set(error) && (!conn->server_started))
    {
        EXALT_LOG_DBG("could not find server (%s), try to start it...", error->message);
        _exalt_dbus_start_server(conn);
        return;
    }

    EXALT_DBUS_ERROR_PRINT(error);
    dbus_message_iter_init(msg, &iter);
    t = dbus_message_iter_get_arg_type(&iter);
    if (!_dbus_iter_type_check(t, DBUS_TYPE_STRING))
        goto error;

    dbus_message_iter_get_basic(&iter, &uid);
    if (!uid)
    {
        EXALT_LOG_ERR("no name owner!");
        goto error;
    }

    EXALT_LOG_DBG("unique name = %s", uid);
    conn->unique_name = eina_stringshare_add(uid);

    conn->connected = 1;
    _exalt_dbus_report_connect(conn, 1);
    return;

error:
    _exalt_dbus_report_connect(conn, 0);
}


    static void
_exalt_dbus_start_server(Exalt_DBus_Conn *conn)
{
    if (conn->pending_start_service_by_name)
    {
        EXALT_LOG_DBG("already pending start service by name.");
        return;
    }

    conn->server_started = 0;
    conn->pending_start_service_by_name = e_dbus_start_service_by_name
        (conn->e_conn, EXALTD_SERVICE, 0, _exalt_dbus_start_server_cb,
         conn);
    if (!conn->pending_start_service_by_name)
    {
        EXALT_LOG_ERR("could not start service by name!");
        _exalt_dbus_report_connect(conn, 0);
    }
}

    static void
_exalt_dbus_report_connect(Exalt_DBus_Conn *conn, Eina_Bool success)
{
    if (!conn->connect.cb)
    {
        EXALT_LOG_ERR("already called?!");
        return;
    }

    conn->connect.cb(conn->connect.data, conn, success);
    if (conn->connect.free_data)
    {
        conn->connect.free_data(conn->connect.data);
        conn->connect.free_data = NULL;
    }
    conn->connect.cb = NULL;
    conn->connect.data = NULL;
}

static void
_exalt_dbus_name_owner_changed(void *data, DBusMessage *msg)
{
    DBusError err;
    const char *name, *from, *to;
    Exalt_DBus_Conn *conn = data;

    dbus_error_init(&err);
    if (!dbus_message_get_args(msg, &err,
                DBUS_TYPE_STRING, &name,
                DBUS_TYPE_STRING, &from,
                DBUS_TYPE_STRING, &to,
                DBUS_TYPE_INVALID))
    {
        EXALT_LOG_ERR("could not get NameOwnerChanged arguments: %s: %s",
                err.name, err.message);
        dbus_error_free(&err);
        return;
    }

    if(!from || !name)
        return ;
    if (strcmp(name, EXALTD_SERVICE) != 0)
        return;

    EXALT_LOG_DBG("NameOwnerChanged from=[%s] to=[%s]", from, to);

    if (from[0] != '\0' && to[0] == '\0')
    {
        EXALT_LOG_DBG("exit exaltd at %s", from);
        if (conn->unique_name && strcmp(conn->unique_name, from) != 0)
            EXALT_LOG_WARN("%s was not the known name %s, ignored.",
                    from, conn->unique_name);
        else
        {
            EXALT_LOG_ERR("server exit!!!");
            if (conn->die.cb)
                conn->die.cb(conn->die.data, conn);
        }
    }
    else
        EXALT_LOG_DBG("unknown change from %s to %s", from, to);
}


/** @} */

