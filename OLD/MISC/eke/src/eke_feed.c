#include <time.h>

#include "config.h"
#include "eke_feed.h"
#include "eke_crypt.h"
#include "eke_parse.h"
#include "eke_macros.h"
#include "eke_file.h"
#include "eke_feed_item.h"

#define MINUTES     * 60

static int init_count = 0;
static char *cache_dir = NULL;

static Ecore_Event_Handler *add = NULL, *del = NULL, *data = NULL;
static int eke_feed_server_add_cb(void *data, int type, void *ev);
static int eke_feed_server_del_cb(void *data, int type, void *ev);
static int eke_feed_server_data_cb(void *data, int type, void *ev);
static char *eke_feed_cache_name_get(const char *name, const char *path);

static void eke_feed_data_handle(Eke_Feed *feed);

int
eke_feed_init(const char *dir)
{
    if (!ecore_init()) return 0;
    if (!ecore_con_init()) return 0;

    init_count++;
    if (!EKE_FEED_EVENT_DATA) {
        add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD,
                                        eke_feed_server_add_cb, NULL);
        del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL,
                                        eke_feed_server_del_cb, NULL);
        data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
                                        eke_feed_server_data_cb, NULL);

        if (!add || !del || !data) {
            init_count--;
            return 0;
        }

        /* if we have a dir make sure to sub the ~ with the real path or
         * wierd things happen, heh
         */
        if (dir) {
            if (*dir == '~') {
                char *home_dir;
                int len;

                home_dir = getenv("HOME");
                len = strlen(home_dir) + strlen(dir);

                cache_dir = NEW(char, len);
                snprintf(cache_dir, len, "%s%s", home_dir, dir + 1);

            } else
                cache_dir = strdup(dir);
        } else
            cache_dir = strdup("/tmp/eke_cache");

        if (!eke_file_mkpath(cache_dir)) {
            printf("Unable to make cache dir (%s)\n", dir);

            ecore_event_handler_del(add);
            ecore_event_handler_del(del);
            ecore_event_handler_del(data);
            init_count--;

            return 0;
        }
        EKE_FEED_EVENT_DATA = ecore_event_type_new();
    }
    return init_count;
}

int
eke_feed_shutdown(void)
{
    if (init_count > 0) {
        init_count--;
        if (init_count > 0) return init_count;

        IF_FREE(cache_dir);
        if (add) ecore_event_handler_del(add);
        if (del) ecore_event_handler_del(del);
        if (data) ecore_event_handler_del(data);
        ecore_con_shutdown();
        ecore_shutdown();
    }
    return 0;
}

Eke_Feed *
eke_feed_new_from_uri(const char *uri)
{
    Eke_Feed *feed;
    char *name, *srv, *path;
    char *dest, *ptr, *tmp, *end;
    int port;
    
    dest = strdup(uri);
    ptr = dest + strlen("http://");
    tmp = strchr(ptr, ':');
    if (!tmp) {
        port = 80;

        tmp = strchr(ptr, '/');
        if (!tmp) {
            srv = strdup(ptr);
            name = strdup(ptr);

        } else {
            char c;

            c = *tmp;
            *tmp = '\0';

            srv = strdup(ptr);
            name = strdup(ptr);

            *tmp = c;
        }

    } else {
        char c;

        end = strchr(tmp, '/');
        if (!end) 
            port = atoi(tmp + 1);
        else {
            c = *end;
            *end = '\0';

            port = atoi(tmp + 1);
            *end = c;
        }

        c = *tmp;
        *tmp = '\0';

        srv = strdup(ptr);
        name = strdup(ptr);

        *tmp = c;
    }
    path = strdup(dest);
    FREE(dest);

    feed = eke_feed_new(NULL, srv, port, path);

    FREE(name);
    FREE(srv);
    FREE(path);

    return feed;
}

Eke_Feed *
eke_feed_new(const char *name, const char *srv, int port, const char *path)
{
    Eke_Feed *feed;
    char *tmp;

    feed = NEW(Eke_Feed, 1);

    if (name) feed->name = strdup(name);
    if (srv) feed->server.name = strdup(srv);
    if (path) feed->server.path = strdup(path);
    feed->server.port = port;

    feed->update.interval = 5 MINUTES;
    feed->items = ecore_list_new();

    tmp = eke_feed_cache_name_get(name, path);
    if (eke_file_exists(tmp)) {
        feed->update.last_time = eke_file_mtime_get(tmp);
        feed->update.crypt = eke_file_crypt_get(tmp);

        if (!feed->update.crypt)
            feed->update.last_time = 0;
    }
    FREE(tmp);

    return feed;
}

void
eke_feed_proxy_set(Eke_Feed *feed, const char *srv, int port, 
                            const char *uname, const char *passwd)
{
    if (!feed) return;

    if (srv) feed->server.proxy.name = strdup(srv);
    if (uname) feed->server.proxy.user = strdup(uname);
    if (passwd) feed->server.proxy.passwd = strdup(passwd);
    feed->server.proxy.port = port;
}

int
eke_feed_update(void *data)
{
    time_t now;
    Eke_Feed *feed;

    feed = data;

    if (!feed || !feed->server.name) 
        return 0;

    if (!feed->timer)
        feed->timer = ecore_timer_add(feed->update.interval + 5,
                                            eke_feed_update, feed);

    /* check if update timeout is exceeded */
    now = time(NULL);
    if ((feed->update.last_time + feed->update.interval) > now) {
        IF_FREE(feed->data.data);

        feed->data.data = eke_feed_cache_name_get(feed->name, feed->server.path);
        feed->data.size = strlen(feed->data.data);

        eke_parse_file(feed);

        if (feed->data.type != EKE_FEED_DATA_RESPONSE_NO_CHANGE) {
            Eke_Feed_Event *ev;

            ev = NEW(Eke_Feed_Event, 1);
            ev->feed = feed;

            ecore_event_add(EKE_FEED_EVENT_DATA, ev, 
                                        eke_feed_event_free, NULL);
        }
        return 1;
    }
    return eke_feed_update_execute(feed);
}

int
eke_feed_update_execute(Eke_Feed *feed)
{
    IF_FREE(feed->server.data.data);
    feed->server.data.size = 0;

    /* XXX proxy stuff ... */
    if (!ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM,
                                feed->server.name, feed->server.port, feed))
            printf("Error with connection to (%s)\n", feed->server.name);

    return 1;
}

static int
eke_feed_server_add_cb(void *data, int type, void *ev)
{
    Ecore_Con_Event_Server_Add *e;
    Eke_Feed *feed;
    char buf[1024];

    e = ev;
    feed = ecore_con_server_data_get(e->server);

    printf("requesting \"%s\" ...\n", feed->server.path);

    snprintf(buf, 1024, "GET %s HTTP/1.0\r\n", feed->server.path);
    ecore_con_server_send(e->server, buf, strlen(buf));

    snprintf(buf, 1024, "User-Agent: %s/%s\r\n", PACKAGE, VERSION);
    ecore_con_server_send(e->server, buf, strlen(buf));

    snprintf(buf, 1024, "\r\n");
    ecore_con_server_send(e->server, buf, strlen(buf));

    return 1;
    data = NULL;
    type = 0;
}

static int
eke_feed_server_del_cb(void *data, int type, void *ev)
{
    Ecore_Con_Event_Server_Del *e;
    Eke_Feed *feed;

    e = ev;

    feed = ecore_con_server_data_get(e->server);
/*    ecore_con_server_del(e->server);
*/

    eke_feed_data_handle(feed);

    if (feed->data.type != EKE_FEED_DATA_RESPONSE_NO_CHANGE) {
        Eke_Feed_Event *feed_ev;

        feed_ev = NEW(Eke_Feed_Event, 1);
        feed_ev->feed = feed;

        ecore_event_add(EKE_FEED_EVENT_DATA, feed_ev, eke_feed_event_free, NULL);
    } 
    return 1;
    type = 0;
    data = NULL;
}

static int
eke_feed_server_data_cb(void *data, int type, void *ev)
{
    Ecore_Con_Event_Server_Data *e;
    Eke_Feed *feed;

    e = ev;

    feed = ecore_con_server_data_get(e->server);
    feed->server.data.data = realloc(feed->server.data.data, 
                                feed->server.data.size + e->size + 1);
    memcpy(feed->server.data.data + feed->server.data.size, e->data, e->size);

    feed->server.data.size += e->size;
    feed->server.data.data[feed->server.data.size] = '\0';

    return 1;
    type = 0;
    data = NULL;
}

static void
eke_feed_data_handle(Eke_Feed *feed) 
{
    char *ptr;
    float http_version;
    int ret_code;
    char ret[256], *name;

    IF_FREE(feed->data.data);

    ptr = strchr(feed->server.data.data, '\r');
    if (!ptr) {
        feed->data.type = EKE_FEED_DATA_RESPONSE_ERROR;
        feed->data.data = strdup("Incorrect payload");
        
        IF_FREE(feed->server.data.data);
        feed->server.data.size = 0;

        return;
    }
    *++ptr = '\0';

    sscanf(feed->server.data.data, "HTTP/%f %d %[a-zA-Z0-9_ ]\r", &http_version,
                                                        &ret_code, ret);

    /* if this isn't http OK then its an error ... */
    if (ret_code != 200) {
        feed->data.type = EKE_FEED_DATA_RESPONSE_ERROR;
        feed->data.data = strdup(ret);

        FREE(feed->server.data.data);
        feed->server.data.size = 0;

        return;
    }
    *ptr = '\n';

    /* find the payload itself */
    ptr = strstr(feed->server.data.data, "\r\n\r\n");
    if (!ptr) {
        feed->data.type = EKE_FEED_DATA_RESPONSE_ERROR;
        feed->data.data = strdup("Payload body not found");
        
        FREE(feed->server.data.data);
        feed->server.data.size = 0;

        return;
    }
    ptr += 4;
  
    /* write the cache file out */
    name = eke_feed_cache_name_get(feed->name, feed->server.path);
    eke_file_write(name, ptr);
    FREE(name);

    feed->update.last_time = time(NULL);

    /* check to see if we actually need to do anything 
     * if feed->items is empty, run the parser as this maybe
     * the first time going through ... 
     */
    {
         char *crypt;

         crypt = eke_crypt_get(ptr);
         if (feed->update.crypt && !strcmp(crypt, feed->update.crypt) 
                && ecore_list_count(feed->items)) {
            FREE(crypt);
            feed->data.type = EKE_FEED_DATA_RESPONSE_NO_CHANGE;

            FREE(feed->server.data.data);
            feed->server.data.size = 0;

            return;
         }
         IF_FREE(feed->update.crypt);
         feed->update.crypt = crypt;
    }

    /* get the xml document out */
    feed->data.data = strdup(ptr);
    feed->data.size = strlen(ptr);

    FREE(feed->server.data.data);
    feed->server.data.size = 0;

    eke_parse_memory(feed);
}
 
void
eke_feed_event_free(void *data, void *ev)
{
    Eke_Feed_Event *feed;

    feed = ev;
    IF_FREE(feed);

    return;
    data = NULL;
}

void
eke_feed_name_set(Eke_Feed *feed, const char *name)
{
    if (!feed || !name) return;

    IF_FREE(feed->name);
    feed->name = strdup(name);
}

static char *
eke_feed_cache_name_get(const char *name, const char *path)
{
    char c_name[PATH_MAX];
    char *path_tmp, *t, *p;

    if (!name) {
        path_tmp = strdup(path);
        if (path_tmp[strlen(path_tmp) - 1] == '/')
            path_tmp[strlen(path_tmp) - 1] = '\0';

        p = path_tmp + strlen("http://"); 
        while ((t = strchr(p, '/')) != NULL)
            *t = '_';

        while ((t = strchr(p, '.')) != NULL)
            *t = '_';

    } else {
        path_tmp = strdup(name);
        p = path_tmp;
    }

    snprintf(c_name, PATH_MAX, "%s/%s.xml", cache_dir, p);
    FREE(path_tmp);

    return strdup(c_name);
}

void
eke_feed_del(Eke_Feed *feed)
{
    if (!feed) return;

    if (feed->timer) ecore_timer_del(feed->timer);
    IF_FREE(feed->name);
    IF_FREE(feed->server.name);
    IF_FREE(feed->server.path);
    IF_FREE(feed->server.proxy.name);
    IF_FREE(feed->server.proxy.user);
    IF_FREE(feed->server.proxy.passwd);
    IF_FREE(feed->server.data.data);
    IF_FREE(feed->data.data);
    IF_FREE(feed->update.crypt);
    IF_FREE(feed->title);
    IF_FREE(feed->link);
    IF_FREE(feed->desc);

    ecore_list_free_cb_set(feed->items, ECORE_FREE_CB(eke_feed_item_free));
    ecore_list_destroy(feed->items);

    FREE(feed);
}



