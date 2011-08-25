/*
 * This code is loosely-based on Rhythmbox plugin to fetch coverart from LastFM.
 */
#include "private.h"
#include "coverart-lastfm.h"

#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

typedef enum {
   LASTFM_COVER_SMALL,
   LASTFM_COVER_MEDIUM,
   LASTFM_COVER_LARGE,
   LASTFM_COVER_EXTRALARGE,
} LastFM_Cover_Size;

struct _LastFM_Cover_Request {
   Ecore_Con_Url *con_url; 
   const char *artist;
   const char *album;
   char *data;
   size_t data_len;
   void (*completed_cb)(void *data, Eina_Hash *imgs);
   void *cb_data;
};

static char *_local_cache_dir = NULL;

/*
 * This API Key belongs to leandro@profusion.mobi -- do not use it for commercial
 * purposes, please.           -- Leandro, 11/11/2010
 */
static const char *api_key = "eb6cf992423f32665bd3275c6b121e37";
static const char *api_url = "http://ws.audioscrobbler.com/2.0/";

static struct {
  const char *str;
  regex_t *exp;
} disc_number_regexes[] = {
  { "\\((disc|cd) *[0-9]+\\)", NULL },
  { "\\[(disc|cd) *[0-9]+\\]", NULL },
  { "-? *(disc|cd) *[0-9]+$", NULL },
  { NULL, NULL }
};

static Ecore_Event_Handler *data_handler = NULL;
static Ecore_Event_Handler *completed_handler = NULL;

void
lastfm_response_free(Eina_Hash *urls)
{
    eina_hash_free(urls);
}

static void
_lastfm_cover_request_free(LastFM_Cover_Request *req)
{
    if (!req) return;
    ecore_con_url_free(req->con_url);
    eina_stringshare_del(req->artist);
    eina_stringshare_del(req->album);
    free(req->data);
    free(req);
}

static Eina_Bool
_lastfm_cover_request_data_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
    Ecore_Con_Event_Url_Data *e = event;
    Ecore_Con_Url *con_url = e->url_con;
    LastFM_Cover_Request *req = ecore_con_url_data_get(con_url);
    char *new_data = realloc(req->data, req->data_len + e->size);
    if (new_data)
      req->data = new_data;
    else
      {
         ERR("could not reallocate data, cancelling job");
         _lastfm_cover_request_free(req);
         return EINA_FALSE;
      }
    
    memcpy(req->data + req->data_len, e->data, e->size);
    req->data_len += e->size;
    
    return EINA_TRUE;
}

static Eina_Bool
_lastfm_cover_request_completed_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
    Ecore_Con_Event_Url_Data *e = event;
    Ecore_Con_Url *con_url = e->url_con;
    LastFM_Cover_Request *req = ecore_con_url_data_get(con_url);
    Eina_Hash *ret = NULL;
    char *image;
    
    /* FIXME we should *really* use a real XML parser here */
    if (!(ret = eina_hash_string_small_new(free))) goto end;
    if (req->data_len == 0) goto end;
    if (!strstr(req->data, "<lfm status=\"ok\">")) goto end;
    
    if (!(image = strstr(req->data, "<image size=\""))) goto end;
    do {
        LastFM_Cover_Size size;
        int *_size;
        char *url;
        
        image += sizeof("<image size=\"") - 1;
        if (!strncmp(image, "small", sizeof("small") - 1))
          size = LASTFM_COVER_SMALL;
        else if (!strncmp(image, "medium", sizeof("medium") - 1))
          size = LASTFM_COVER_MEDIUM;
        else if (!strncmp(image, "large", sizeof("large") - 1))
          size = LASTFM_COVER_LARGE;
        else if (!strncmp(image, "extralarge", sizeof("extralarge") - 1))
          size = LASTFM_COVER_EXTRALARGE;
        else
          {
             ERR("unknown cover image type");
             continue;
          }
        if (!(url = strchr(image, '>')))
          {
             ERR("image tag not closed");
             continue;
          }
        if (!(image = strstr(image, "</image>")))
          {
             ERR("could not find closing tag");
             continue;
          }
        *image = 0;
        ++url;
        if (!*url) continue;
        if (!(_size = malloc(sizeof(int))))
          {
             ERR("could not allocate size tag");
             continue;
          }
        *_size = size;
        eina_hash_set(ret, url, _size);
    } while (image && (image = strstr(image + 1, "<image size=\"")));

end:
    req->completed_cb(req->cb_data, ret);
    _lastfm_cover_request_free(req);
    return EINA_FALSE;
}

static void
_remove_disc_info(char *info)
{
    int i;

    for (i = 0; disc_number_regexes[i].str; i++)
      {
         regmatch_t matches[32];
         int j;

         if (!disc_number_regexes[i].exp) continue;
         if (regexec(disc_number_regexes[i].exp, info, 32, matches, 0)) continue;

         for (j = 0; j < 32; j++)
           {
             if (matches[j].rm_so < 0 || matches[j].rm_eo < 0) break;
             memset(info + matches[j].rm_so, ' ', matches[j].rm_eo - matches[j].rm_so);
           }
      }
}

static char *
_url_encode(const char *s)
{
    static const char *hex_digits = "0123456789ABCDEF";
    int new_size, i, j;
    char *output, *no_disc;

    if (!s) return NULL;

    no_disc = strdup(s);
    if (!no_disc) return NULL;
    _remove_disc_info(no_disc);

    for (new_size = 0, output = no_disc; *output; output++)
      {
         if (isalnum(*output)) new_size++;
         else new_size += 3;
      }

    output = malloc(new_size + 1);
    if (!output) return NULL;

    for (i = j = 0; i < new_size; i++, j++)
      {
         if (isalnum(no_disc[j]))
           output[i] = no_disc[j];
         else
           {
              output[i] = '%';
              output[++i] = hex_digits[(no_disc[j] & 0xf0) >> 4];
              output[++i] = hex_digits[no_disc[j] & 0x0f];
           }
      }
    output[i] = '\0';
    free(no_disc);

    return output;
}

LastFM_Cover_Request *
lastfm_cover_search_request(const char *artist, const char *album,
                             void (*completed_cb)(void *, Eina_Hash *),
                             void *data)
{
    LastFM_Cover_Request *req = NULL;
    Ecore_Con_Url *con_url;
    char *clear_album, *clear_artist;
    char *url = NULL;

    if (!completed_cb) return NULL;

    clear_artist = _url_encode(artist);
    clear_album = _url_encode(album);

    if (asprintf(&url, "%s?method=album.getinfo%s%s%s%s&api_key=%s",
                 api_url,
                 clear_artist ? "&artist=" : "", clear_artist ? clear_artist : "",
                 clear_album ? "&album=" : "", clear_album ? clear_album : "",
                 api_key) < 0)
      {
         ERR("failed to allocate memory");
         goto error;
      }

    con_url = ecore_con_url_custom_new(url, "GET");
    if (!con_url)
      {
         ERR("Failed to add an Ecore_Conjob");
         goto error;
      }
    
    req = calloc(1, sizeof(*req));
    if (!req)
      {
         ERR("Failed to allocate a request");
         goto send_error;
      }
    req->artist = eina_stringshare_add(artist);
    req->album = eina_stringshare_add(album);
    req->data = NULL;
    req->data_len = 0;
    req->con_url = con_url;
    req->completed_cb = completed_cb;
    req->cb_data = data;
    
    ecore_con_url_data_set(con_url, req);
    if (!ecore_con_url_get(con_url))
      {
         ERR("Failed to send request");
         goto send_error;
      }

    free(clear_album);
    free(clear_artist);
    
    return req;

send_error:
    _lastfm_cover_request_free(req);
error:
    free(url);
    free(clear_album);
    free(clear_artist);
    
    return NULL;
}

void
lastfm_cover_init(void)
{
    int i;

    ecore_init();
    ecore_file_init();
    ecore_con_url_init();
    
    data_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA,
                                           _lastfm_cover_request_data_cb,
                                           NULL);
    if (!data_handler)
      ERR("could not register ECORE_CON_EVENT_URL_DATA handler");
    
    completed_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE,
                                                _lastfm_cover_request_completed_cb,
                                                NULL);
    if (!completed_handler)
      ERR("could not register ECORE_CON_EVENT_URL_COMPLETE handler");

    for (i = 0; disc_number_regexes[i].str; i++)
      {
         disc_number_regexes[i].exp = malloc(sizeof(regex_t));
         if (!disc_number_regexes[i].exp) continue;
         if (regcomp(disc_number_regexes[i].exp,
                     disc_number_regexes[i].str,
                     REG_EXTENDED | REG_ICASE))
           {
              ERR("could not compile regex: %s", disc_number_regexes[i].str);
              free(disc_number_regexes[i].exp);
              disc_number_regexes[i].exp = NULL;
           }
      }

    _local_cache_dir = enjoy_cache_dir_get();
    if (!_local_cache_dir) CRITICAL("could not get cache directory!");
}

void
lastfm_cover_shutdown(void)
{
    int i;

    for (i = 0; disc_number_regexes[i].str; i++)
      {
         regfree(disc_number_regexes[i].exp);
         free(disc_number_regexes[i].exp);
         disc_number_regexes[i].exp = NULL;
      }

    ecore_event_handler_del(data_handler);
    ecore_event_handler_del(completed_handler);
    data_handler = completed_handler = NULL;

    ecore_con_url_shutdown();
    ecore_file_shutdown();
    ecore_shutdown();
}

char *
lastfm_local_path_from_url_get(char *url)
{
    char *local, *p, *plus_cache;
    local = strdup(url);
    if (!local) return NULL;
    for (p = local; *p; p++)
      if (*p == ':' || *p == '/') *p = '_';
    if (asprintf(&plus_cache, "%s/%s", _local_cache_dir, local) > 0)
      {
         free(local);
         return plus_cache;
      }
    free(local);
    return NULL;
}

struct LastFM_Cover_Download_Data {
    void *data;
    char *local_file;
    void (*cb)(void *, char *, Eina_Bool);
};

static void
_lastfm_cover_download_completed_cb(void *data, const char *file __UNUSED__, int status __UNUSED__)
{
    struct LastFM_Cover_Download_Data *lcdd = data;
    /* FIXME: for some reason, status isn't that reliable */
    if (lcdd->cb)
      lcdd->cb(lcdd->data, lcdd->local_file, EINA_TRUE);
    free(lcdd->local_file);
    free(lcdd);
}

Eina_Bool
lastfm_cover_download(char *url,
                      void (*completed_cb)(void *data, char *local_file, Eina_Bool success),
                      void *data)
{
    struct LastFM_Cover_Download_Data *lcdd = calloc(1, sizeof(*lcdd));
    Ecore_File_Download_Job *job;
    char *dest;
    
    if (!lcdd)
      {
         ERR("could not allocate memory");
         return EINA_FALSE;
      }
    
    if (!_local_cache_dir)
      {
         ERR("cache package not set");
         return EINA_FALSE;
      }

    dest = lastfm_local_path_from_url_get(url);
    if (!dest)
      {
         ERR("could not obtain local path for url %s", url);
         return EINA_FALSE;
      }
    if (ecore_file_exists(dest))
      {
         free(dest);
         return EINA_TRUE;
      }
    
    lcdd->data = data;
    lcdd->local_file = dest;
    lcdd->cb = completed_cb;
    
    if (!ecore_file_download(url, dest, _lastfm_cover_download_completed_cb,
                             NULL, lcdd, &job) || !job)
      {
         ERR("could not download cover");
         ecore_file_remove(dest);
         free(dest);
         return EINA_FALSE;
      }

    return EINA_TRUE;
}

#ifdef LASTFM_COVER_TEST
static void
_test_completed_cb(Eina_Hash *urls)
{
    Eina_Iterator *iter = eina_hash_iterator_tuple_new(urls);
    Eina_Hash_Tuple *tuple;
    if (!iter) return;
    
    EINA_ITERATOR_FOREACH(iter, tuple)
    {
       printf("Got URL: %s (size %d)\n", tuple->key, *((int *)tuple->data));
    }
    lastfm_response_free(urls);
    ecore_main_loop_quit();
}

int
main(void)
{
    char artist[512];
    char album[512];
    
    lastfm_cover_init();
    
    printf("An empty artist quits the program\n");
    for (;;)
      {
        LastFM_Cover_Request *req;
        
        printf("Artist? ");
        fgets(artist, 512, stdin);
        if (artist[0] == '\n' || artist[0] == '\0') break;

        printf("Album? ");
        fgets(album, 512, stdin);
        
        printf("Making request.");
        req = lastfm_cover_search_request(artist, album, _test_completed_cb);
        if (!req)
          {
             printf("Could not create request\n");
             continue;
          }
        printf("Request sent. Waiting for response.\n");
        ecore_main_loop_begin();
      }
    
    lastfm_cover_shutdown();
}
#endif
