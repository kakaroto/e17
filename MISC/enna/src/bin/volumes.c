#include "enna.h"

static Eina_Hash *_volumes = NULL;

void _hash_data_free_cb(void *data)
{
    Eina_List *l;
    Enna_Volume *v;

    EINA_LIST_FOREACH(data, l, v)
    {
	l = eina_list_remove(l, v);
	eina_stringshare_del(v->icon);
	eina_stringshare_del(v->type);
	eina_stringshare_del(v->uri);
	eina_stringshare_del(v->name);
	eina_stringshare_del(v->label);

    }
    eina_list_free(l);

}

void enna_volumes_init(void)
{

    if (_volumes)
	eina_hash_free(_volumes);
    _volumes = NULL;
    ENNA_EVENT_VOLUME_ADDED = ecore_event_type_new();
    ENNA_EVENT_VOLUME_REMOVED = ecore_event_type_new();
    _volumes = eina_hash_pointer_new(_hash_data_free_cb);
}

void enna_volumes_shutdown(void)
{
    if (_volumes)
	eina_hash_free(_volumes);
    _volumes = NULL;
}

void enna_volumes_append(const char *type, Enna_Volume *v)
{
    Eina_List *l = NULL;
    Enna_Volume *ev;

    if (!v) return;

    l = eina_hash_find(_volumes, type);
    l = eina_list_append(l, v);

    if (eina_hash_add(_volumes, type, l))
    {
	ev = calloc(1, sizeof(Enna_Volume));
   	memcpy(ev, v, sizeof(Enna_Volume));
	enna_log(ENNA_MSG_EVENT, NULL, "ENNA_EVENT_VOLUME_ADDED Sent");
	ecore_event_add(ENNA_EVENT_VOLUME_ADDED, ev, NULL, NULL);
    }

}

void enna_volumes_remove(const char *type, Enna_Volume *v)
{
    Enna_Volume *ev;
    Eina_List *l = eina_hash_find(_volumes, type);

    l = eina_list_remove(l, v);

    ev = calloc(1, sizeof(Enna_Volume));
    memcpy(ev, v, sizeof(Enna_Volume));
    enna_log(ENNA_MSG_EVENT, NULL, "ENNA_EVENT_VOLUME_REMOVED Sent");
    ecore_event_add(ENNA_EVENT_VOLUME_REMOVED, ev, NULL, NULL);

}

Eina_List *enna_volumes_get(const char *type)
{
    return eina_hash_find(_volumes, type);
}

