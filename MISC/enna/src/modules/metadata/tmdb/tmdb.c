/* Interface */

#include "enna.h"
#include "xml_utils.h"
#include "url_utils.h"

#define ENNA_MODULE_NAME        "metadata_tmdb"
#define ENNA_GRABBER_NAME       "tmdb"
#define ENNA_GRABBER_PRIORITY   3

#define PATH_BACKDROPS          "backdrops"
#define PATH_COVERS             "covers"

#define MAX_URL_SIZE            1024

#define TMDB_HOSTNAME           "api.themoviedb.org"

#define TMDB_API_KEY            "5401cd030990fba60e1c23d2832de62e"

#define TMDB_QUERY_SEARCH "http://%s/2.0/Movie.search?title=%s&api_key=%s"
#define TMDB_QUERY_INFO   "http://%s/2.0/Movie.getInfo?id=%s&api_key=%s"

typedef struct _Enna_Metadata_Tmdb
{
    Evas *evas;
    Enna_Module *em;
    CURL *curl;
} Enna_Metadata_Tmdb;

static Enna_Metadata_Tmdb *mod;

/*****************************************************************************/
/*                         TheMovieDB.org Helpers                            */
/*****************************************************************************/

static void
tmdb_parse (Enna_Metadata *meta)
{
    char url[MAX_URL_SIZE];
    char *escaped_keywords;
    url_data_t data;

    xmlDocPtr doc = NULL;
    xmlChar *tmp;
    
    if (!meta || !meta->keywords)
        return;

    /* TMDB only has sense on video files */
    if (meta->type != ENNA_METADATA_VIDEO)
        return;
    
    /* get HTTP compliant keywords */
    escaped_keywords = calloc (1, 2 * strlen (meta->keywords));
    url_escape_string (escaped_keywords, meta->keywords);

    /* proceed with TMDB search request */
    memset (url, '\0', MAX_URL_SIZE);
    snprintf (url, MAX_URL_SIZE, TMDB_QUERY_SEARCH,
              TMDB_HOSTNAME, escaped_keywords, TMDB_API_KEY);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Search Request: %s", url);

    data = url_get_data (mod->curl, url);
    if (data.status != CURLE_OK)
        goto error;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Search Reply: %s", data.buffer);

    /* parse the XML answer */
    doc = xmlReadMemory (data.buffer, data.size, NULL, NULL, 0);
    free (data.buffer);
    if (!doc)
        goto error;

    /* check for total number of results */
    tmp = get_prop_value_from_xml_tree (xmlDocGetRootElement (doc),
                                        "totalResults");
    if (!tmp)
    {
        enna_log (ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                  "Unable to find the item \"%s\"", escaped_keywords);
        goto error;
    }

    /* check that requested item is known on TMDB */
    if (!xmlStrcmp ((unsigned char *) tmp, (unsigned char *) "0"))
        goto error;
    xmlFree (tmp);

    /* get TMDB Movie ID */
    tmp = get_prop_value_from_xml_tree (xmlDocGetRootElement (doc), "id");
    if (!tmp)
        goto error;

    xmlFreeDoc (doc);
    doc = NULL;

    /* proceed with TMDB search request */
    memset (url, '\0', MAX_URL_SIZE);
    snprintf (url, MAX_URL_SIZE,
              TMDB_QUERY_INFO, TMDB_HOSTNAME, tmp, TMDB_API_KEY);
    xmlFree (tmp);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Info Request: %s", url);

    data = url_get_data (mod->curl, url);
    if (data.status != CURLE_OK)
        goto error;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME, "Info Reply: %s", data.buffer);

    /* parse the XML answer */
    doc = xmlReadMemory (data.buffer, data.size, NULL, NULL, 0);
    free (data.buffer);
    if (!doc)
        goto error;

    /* fetch movie overview description */
    if (!meta->overview)
    {
        tmp = get_prop_value_from_xml_tree (xmlDocGetRootElement (doc),
                                            "short_overview");
        if (tmp)
        {
            meta->overview = strdup ((char *) tmp);
            xmlFree (tmp);
        }
    }

    /* fetch movie runtime (in minutes) */
    if (!meta->runtime)
    {
        tmp = get_prop_value_from_xml_tree (xmlDocGetRootElement (doc),
                                            "runtime");
        if (tmp)
        {
            meta->runtime = atoi ((char *) tmp);
            xmlFree (tmp);
        }
    }
    
    /* fetch movie year of production */
    if (!meta->year)
    {
        tmp = get_prop_value_from_xml_tree (xmlDocGetRootElement (doc),
                                            "release");
        if (tmp)
        {
            int r, y, m, d;
            r = sscanf ((char *) tmp, "%d-%d-%d", &y, &m, &d);
            xmlFree (tmp);
            if (r == 0)
                meta->year = y;
        }
    }

    /* fetch movie categories */
    if (!meta->categories)
    {
        xmlNode *cat;
        int i;

        cat = get_node_xml_tree (xmlDocGetRootElement (doc), "category");
        for (i = 0; i < 4; i++)
        {
            if (!cat)
                break;
            
            tmp = get_prop_value_from_xml_tree (cat, "name");
            if (tmp)
            {
                enna_metadata_add_category (meta, (char *) tmp);
                xmlFree (tmp);
            }
            cat = cat->next;
        }
    }

    /* fetch movie poster/cover */
    if (!meta->cover)
    {
        tmp = get_prop_value_from_xml_tree_by_attr (xmlDocGetRootElement (doc),
                                                    "poster", "size", "cover");

        if (tmp)
        {
            char cover[1024];
                        
            snprintf (cover, sizeof (cover), "%s/.enna/%s/%s.png",
                      enna_util_user_home_get(), PATH_COVERS, meta->md5);
            url_save_to_disk (mod->curl, (char *) tmp, cover);
            xmlFree (tmp);
            
            meta->cover = strdup (cover);
        }
    }

    /* fetch movie backdrop */
    if (!meta->backdrop)
    {
        tmp = get_prop_value_from_xml_tree_by_attr (xmlDocGetRootElement (doc),
                                                    "backdrop", "size", "mid");

        if (tmp)
        {
            char back[1024];
                        
            snprintf (back, sizeof (back), "%s/.enna/%s/%s.png",
                      enna_util_user_home_get(), PATH_BACKDROPS, meta->md5);
            url_save_to_disk (mod->curl, (char *) tmp, back);
            xmlFree (tmp);
            
            meta->backdrop = strdup (back);
        }
    }
    
 error:
    if (doc)
        xmlFreeDoc (doc);
    ENNA_FREE (escaped_keywords);
}

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void
tmdb_grab (Enna_Metadata *meta, int caps)
{
    if (!meta || !meta->keywords)
        return;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Grabbing info from %s", meta->uri);

    tmdb_parse (meta);
}

static Enna_Metadata_Grabber grabber = {
    ENNA_GRABBER_NAME,
    ENNA_GRABBER_PRIORITY,
    1,
    ENNA_GRABBER_CAP_COVER,
    tmdb_grab,
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

void
module_init (Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof (Enna_Metadata_Tmdb));

    mod->em = em;
    mod->evas = em->evas;

    curl_global_init (CURL_GLOBAL_DEFAULT);
    mod->curl = curl_easy_init ();
    enna_metadata_add_grabber (&grabber);
}

void
module_shutdown (Enna_Module *em)
{
    //enna_metadata_remove_grabber (ENNA_GRABBER_NAME);
    if (mod->curl)
        curl_easy_cleanup (mod->curl);
    curl_global_cleanup ();
    free (mod);
}
