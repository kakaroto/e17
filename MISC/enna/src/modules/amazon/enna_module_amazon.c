/* Interface */

#include "enna.h"
#include "xml_utils.h"
#include "url_utils.h"

#define DEBUG 1

#define ENNA_MODULE_NAME "amazon"

#define MAX_URL_SIZE        1024
#define MAX_KEYWORD_SIZE    1024
#define MAX_BUF_LEN         65535

#define AMAZON_HOSTNAME     "webservices.amazon.com"
#define AMAZON_LICENSE_KEY  "0P1862RFDFSF4KYZQNG2"

#define AMAZON_SEARCH "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s&Operation=ItemSearch&Keywords=%s&SearchIndex=%s"
#define AMAZON_SEARCH_MUSIC "Music"
#define AMAZON_SEARCH_MOVIE "DVD"

#define AMAZON_SEARCH_COVER "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s&Operation=ItemLookup&ItemId=%s&ResponseGroup=Images"
typedef struct _Enna_Module_Amazon
{
    Evas *evas;
    Enna_Module *em;
    CURL *curl;
} Enna_Module_Amazon;

static Enna_Module_Amazon *mod;

/*****************************************************************************/
/*                             Amazon Helpers                                */
/*****************************************************************************/

static char * amazon_cover_get(char *search_type, char *keywords,
        char *escaped_keywords)
{
    char *cover;
    char url[MAX_URL_SIZE];
    url_data_t data;
    char *md5;
    int fd;

    xmlDocPtr doc;
    xmlNode *img;
    xmlChar *asin, *cover_url;

    if (!search_type || !keywords || !escaped_keywords)
        return NULL;

    /* 2. Prepare Amazon WebService URL for Search */
    memset(url, '\0', MAX_URL_SIZE);
    snprintf(url, MAX_URL_SIZE, AMAZON_SEARCH, 
    AMAZON_HOSTNAME, AMAZON_LICENSE_KEY, escaped_keywords, search_type);

#ifdef DEBUG
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Search Request: %s", url);
#endif

    /* 3. Perform request */
    data = url_get_data(mod->curl, url);
    if (data.status != CURLE_OK)
        return NULL;

#ifdef DEBUG
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Search Reply: %s", data.buffer);
#endif

    /* 4. Parse the answer to get ASIN value */
    doc = xmlReadMemory(data.buffer, data.size, NULL, NULL, 0);
    free(data.buffer);

    if (!doc)
        return NULL;

    asin = get_prop_value_from_xml_tree(xmlDocGetRootElement(doc), "ASIN");
    xmlFreeDoc(doc);

    if (!asin)
    {
        enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                "Unable to find the item \"%s\"", escaped_keywords);
        return NULL;
    }

#ifdef DEBUG
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Found Amazon ASIN: %s", asin);
#endif

    /* 5. Prepare Amazon WebService URL for Cover Search */
    memset(url, '\0', MAX_URL_SIZE);
    snprintf(url, MAX_URL_SIZE, AMAZON_SEARCH_COVER, 
    AMAZON_HOSTNAME, AMAZON_LICENSE_KEY, asin);
    xmlFree(asin);

#ifdef DEBUG
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Cover Search Request: %s", url);
#endif

    /* 6. Perform request */
    data = url_get_data(mod->curl, url);
    if (data.status != CURLE_OK)
        return NULL;

#ifdef DEBUG
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Cover Search Reply: %s",
            data.buffer);
#endif

    /* 7. Parse the answer to get cover URL */
    doc = xmlReadMemory(data.buffer, data.size, NULL, NULL, 0);
    free(data.buffer);

    if (!doc)
        return NULL;

    img = get_node_xml_tree(xmlDocGetRootElement(doc), "LargeImage");
    if (!img)
        img = get_node_xml_tree(xmlDocGetRootElement(doc), "MediumImage");
    if (!img)
        img = get_node_xml_tree(xmlDocGetRootElement(doc), "SmallImage");

    if (!img)
    {
        xmlFreeDoc(doc);
        return NULL;
    }

    cover_url = get_prop_value_from_xml_tree(img, "URL");
    if (!cover_url)
    {
        enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                "Unable to find the cover for %s", escaped_keywords);
        xmlFreeDoc(doc);
        return NULL;
    }

    xmlFreeDoc(doc);

    /* 8. Download cover and save to disk */
    md5 = md5sum(keywords);
    cover = malloc(MAX_URL_SIZE);
    snprintf(cover, MAX_URL_SIZE, "%s/.enna/covers/%s.png",
            enna_util_user_home_get(), md5);
    free(md5);

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Saving %s to %s", cover_url,
            cover);

    data = url_get_data(mod->curl, (char *) cover_url);
    if (data.status != CURLE_OK)
    {
        enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                "Unable to download requested cover file");
        return NULL;
    }

    fd = open(cover, O_WRONLY | O_CREAT, 0666);
    if (fd < 0)
    {
        enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                "Unable to open stream to save cover file");

        free(data.buffer);
        return NULL;
    }

    write(fd, data.buffer, data.size);
    free(data.buffer);

    xmlFree(cover_url);

    return cover;
}

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static char * amazon_music_cover_get(const char *artist, const char *album)
{
    char keywords[MAX_KEYWORD_SIZE];
    char escaped_keywords[2*MAX_KEYWORD_SIZE];

    if (!artist || !album)
        return NULL;

    /* Format the keywords */
    memset(keywords, '\0', MAX_KEYWORD_SIZE);
    memset(escaped_keywords, '\0', 2 * MAX_KEYWORD_SIZE);
    snprintf(keywords, MAX_KEYWORD_SIZE, "%s %s", artist, album);
    url_escape_string(escaped_keywords, keywords);

    return amazon_cover_get(AMAZON_SEARCH_MUSIC, keywords, escaped_keywords);
}

static char * amazon_movie_cover_get(const char *movie)
{
    char keywords[MAX_KEYWORD_SIZE];
    char escaped_keywords[2*MAX_KEYWORD_SIZE];

    if (!movie)
        return NULL;

    /* Format the keywords */
    memset(keywords, '\0', MAX_KEYWORD_SIZE);
    memset(escaped_keywords, '\0', 2 * MAX_KEYWORD_SIZE);
    snprintf(keywords, MAX_KEYWORD_SIZE, movie);
    url_escape_string(escaped_keywords, keywords);

    return amazon_cover_get(AMAZON_SEARCH_MOVIE, keywords, escaped_keywords);
}

static Enna_Class_CoverPlugin class =
{ "amazon", amazon_music_cover_get, amazon_movie_cover_get, 
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "amazon"
};

EAPI void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Amazon));

    mod->em = em;
    mod->evas = em->evas;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    mod->curl = curl_easy_init();
    enna_cover_plugin_register(&class);
}

EAPI void module_shutdown(Enna_Module *em)
{
    if (mod->curl)
        curl_easy_cleanup(mod->curl);
    curl_global_cleanup();
    free(mod);
}
