/* Interface */

#include "enna.h"
#include "xml_utils.h"
#include "url_utils.h"

#define ENNA_MODULE_NAME "metadata_amazon"
#define ENNA_GRABBER_NAME "amazon"
#define ENNA_GRABBER_PRIORITY   4

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
    int fd, n;

    xmlDocPtr doc;
    xmlNode *img;
    xmlChar *asin, *cover_url;

    if (!search_type || !keywords || !escaped_keywords)
        return NULL;

    /* 2. Prepare Amazon WebService URL for Search */
    memset(url, '\0', MAX_URL_SIZE);
    snprintf(url, MAX_URL_SIZE, AMAZON_SEARCH,
    AMAZON_HOSTNAME, AMAZON_LICENSE_KEY, escaped_keywords, search_type);

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Search Request: %s", url);

    /* 3. Perform request */
    data = url_get_data(mod->curl, url);
    if (data.status != CURLE_OK)
        return NULL;

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME,
             "Search Reply: %s", data.buffer);

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

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Found Amazon ASIN: %s", asin);

    /* 5. Prepare Amazon WebService URL for Cover Search */
    memset(url, '\0', MAX_URL_SIZE);
    snprintf(url, MAX_URL_SIZE, AMAZON_SEARCH_COVER,
    AMAZON_HOSTNAME, AMAZON_LICENSE_KEY, asin);
    xmlFree(asin);

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME,
             "Cover Search Request: %s", url);

    /* 6. Perform request */
    data = url_get_data(mod->curl, url);
    if (data.status != CURLE_OK)
        return NULL;

    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Cover Search Reply: %s",
            data.buffer);

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

    n = write(fd, data.buffer, data.size);
    free(data.buffer);

    xmlFree(cover_url);

    return cover;
}

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void
amazon_grab (Enna_Metadata *meta, int caps)
{
    char *escaped_keywords;
    char *search_type = NULL;
    char *cover;

    if (!meta)
        return;

    /* do not grab if already known */
    if (meta->cover)
        return;

    if (!meta->keywords)
        return;

    /* check file type to ensure looking for meta has a sense */
    if (meta->type == ENNA_METADATA_AUDIO)
        search_type = AMAZON_SEARCH_MUSIC;
    else if (meta->type == ENNA_METADATA_VIDEO)
        search_type = AMAZON_SEARCH_MOVIE;
    else
        return;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Grabbing info from %s", meta->uri);
    
    /* Format the keywords */
    escaped_keywords = calloc (1, 2 * strlen (meta->keywords));
    url_escape_string(escaped_keywords, meta->keywords);
            
    cover = amazon_cover_get (search_type, meta->keywords, escaped_keywords);
    if (cover)
    {
        meta->cover = strdup (cover);
        free (cover);
    }

    free (escaped_keywords);
}

static Enna_Metadata_Grabber grabber = {
    ENNA_GRABBER_NAME,
    ENNA_GRABBER_PRIORITY,
    1,
    ENNA_GRABBER_CAP_COVER,
    amazon_grab,
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_METADATA,
    ENNA_MODULE_NAME
};

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Amazon));

    mod->em = em;
    mod->evas = em->evas;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    mod->curl = curl_easy_init();
    enna_metadata_add_grabber (&grabber);
}

void module_shutdown(Enna_Module *em)
{
    //enna_metadata_remove_grabber (ENNA_GRABBER_NAME);
    if (mod->curl)
        curl_easy_cleanup(mod->curl);
    curl_global_cleanup();
    free(mod);
}
