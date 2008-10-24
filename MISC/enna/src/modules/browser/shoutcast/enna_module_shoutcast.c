/* Interface */

#include "enna.h"
#include "xml_utils.h"
#include "url_utils.h"

#define ENNA_MODULE_NAME "shoutcast"

#define SHOUTCAST_URL      "http://www.shoutcast.com"
#define SHOUTCAST_GENRE    "shoutcast://"
#define SHOUTCAST_LIST     "http://www.shoutcast.com/sbin/newxml.phtml"
#define SHOUTCAST_STATION  "http://www.shoutcast.com/sbin/newxml.phtml?genre="
#define MAX_URL 1024

typedef struct Enna_Module_Music_s
{
    Evas *e;
    Enna_Module *em;
    CURL *curl;
} Enna_Module_Music;

static Enna_Module_Music *mod;

static Eina_List * browse_list(void)
{
    url_data_t chunk;
    xmlDocPtr doc;
    xmlNode *list, *n;
    Eina_List *files = NULL;

    chunk = url_get_data(mod->curl, SHOUTCAST_LIST);

    doc = xmlReadMemory(chunk.buffer, chunk.size, NULL, NULL, 0);
    if (!doc)
        return NULL;

    list = get_node_xml_tree(xmlDocGetRootElement(doc), "genrelist");
    for (n = list->children; n; n = n->next)
    {
        Enna_Vfs_File *file;
        xmlChar *genre;
        char *uri;

        if (!xmlHasProp(n, (xmlChar *) "name"))
            continue;

        genre = xmlGetProp(n, (xmlChar *) "name");

        uri = malloc(strlen(SHOUTCAST_GENRE) + strlen((char *) genre) + 1);
        sprintf(uri, "%s%s", SHOUTCAST_GENRE, genre);

        file = enna_vfs_create_directory(uri, (char *) genre, "icon/webradio",
                NULL);
        free(uri);
        files = eina_list_append(files, file);
    }

    free(chunk.buffer);
    xmlFreeDoc(doc);

    return files;
}

static Eina_List * browse_by_genre(const char *path)
{
    url_data_t chunk;
    xmlDocPtr doc;
    xmlNode *list, *n;
    char url[MAX_URL];
    Eina_List *files = NULL;
    xmlChar *tunein = NULL;
    const char *genre = path + strlen(SHOUTCAST_GENRE);

    memset(url, '\0', MAX_URL);
    snprintf(url, MAX_URL, "%s%s", SHOUTCAST_STATION, genre);
    chunk = url_get_data(mod->curl, url);

    doc = xmlReadMemory(chunk.buffer, chunk.size, NULL, NULL, 0);
    if (!doc)
        return NULL;

    list = get_node_xml_tree(xmlDocGetRootElement(doc), "stationlist");
    for (n = list->children; n; n = n->next)
    {
        if (!n->name)
            continue;

        if (!xmlStrcmp(n->name, (xmlChar *) "tunein"))
        {
            if (!xmlHasProp(n, (xmlChar *) "base"))
                continue;

            tunein = xmlGetProp(n, (xmlChar *) "base");
            continue;
        }

        if (!xmlStrcmp(n->name, (xmlChar *) "station"))
        {
            Enna_Vfs_File *file;
            xmlChar *id, *name;
            char uri[MAX_URL];

            if (!xmlHasProp(n, (xmlChar *) "name") || !xmlHasProp(n,
                    (xmlChar *) "id") || !tunein)
                continue;

            name = xmlGetProp(n, (xmlChar *) "name");
            id = xmlGetProp(n, (xmlChar *) "id");
            memset(uri, '\0', MAX_URL);
            snprintf(uri, MAX_URL, "%s%s?id=%s", SHOUTCAST_URL, tunein, id);

            file = enna_vfs_create_file(uri, (char *) name, "icon/music", NULL);
            files = eina_list_append(files, file);
        }
    }

    free(chunk.buffer);
    xmlFreeDoc(doc);

    return files;
}

static Eina_List * _class_browse_up(const char *path)
{
    if (!path)
        return browse_list();

    if (strstr(path, SHOUTCAST_GENRE))
        return browse_by_genre(path);

    return NULL;
}

static Eina_List * _class_browse_down(void)
{
    return browse_list();
}

static Enna_Vfs_File * _class_vfs_get(void)
{
    return enna_vfs_create_directory(NULL, NULL,
            (char *) evas_stringshare_add("icon/music"), NULL);
}

static Enna_Class_Vfs class_shoutcast =
{ "shoutcast", 1, "SHOUTcast Streaming", NULL, "icon/shoutcast",
{ NULL, NULL, _class_browse_up, _class_browse_down, _class_vfs_get, 
},
};

/* Module interface */

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "shoutcast"
};

EAPI void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Music));
    mod->em = em;
    em->mod = mod;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    mod->curl = curl_easy_init();

    enna_vfs_append("shoutcast", ENNA_CAPS_MUSIC, &class_shoutcast);
}

EAPI void module_shutdown(Enna_Module *em)
{
    Enna_Module_Music *mod = em->mod;

    if (mod->curl)
        curl_easy_cleanup(mod->curl);
    curl_global_cleanup();
}
