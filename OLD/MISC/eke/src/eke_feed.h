#ifndef EKE_FEED
#define EKE_FEED

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Con.h>

typedef enum {
    EKE_FEED_DATA_RESPONSE_NO_CHANGE,
    EKE_FEED_DATA_RESPONSE_CHANGED,
    EKE_FEED_DATA_RESPONSE_ERROR
} Eke_Feed_Data_Response_Type;

typedef struct Eke_Feed Eke_Feed;
struct Eke_Feed {
    char *name;

    struct {
        char *name;
        int port;
        char *path;

        struct {
            char *name;
            int port;
            char *user;
            char *passwd;
        } proxy;

        struct {
            char *data;
            int size;
        } data;
    } server;

    struct {
        Eke_Feed_Data_Response_Type type;
        char *data;
        int size;
    } data;

    struct {
        int interval;
        time_t last_time;
        char *crypt;
    } update;

    char *title;
    char *link;
    char *desc;
    float rss_version;

    Ecore_List *items;
    Ecore_Timer *timer;
};

typedef struct Eke_Feed_Event Eke_Feed_Event;
struct Eke_Feed_Event {
    Eke_Feed *feed;
};

int EKE_FEED_EVENT_DATA;

int eke_feed_init(const char *dir);
int eke_feed_shutdown(void);

void eke_feed_del(Eke_Feed *feed);

Eke_Feed *eke_feed_new_from_uri(const char *uri);
Eke_Feed *eke_feed_new(const char *name, const char *srv, int port, 
                                                    const char *path);
void eke_feed_proxy_set(Eke_Feed *feed, const char *srv, int port,
                        const char *uname, const char *passwd);
int eke_feed_update(void *data);
int eke_feed_update_execute(Eke_Feed *feed);

void eke_feed_name_set(Eke_Feed *feed, const char *name);

void eke_feed_event_free(void *data, void *ev);

#endif

