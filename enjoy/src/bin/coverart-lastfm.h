#ifndef COVERART_LASTFM_H_
#define COVERART_LASTFM_H_

#include <Ecore.h>
#include <Ecore_Con.h>
#include <Ecore_File.h>

#include "private.h"

typedef struct _LastFM_Cover_Request LastFM_Cover_Request;

void                  lastfm_cover_init(void);
void                  lastfm_cover_shutdown(void);
LastFM_Cover_Request *lastfm_cover_search_request(const char *artist, const char *album,
                                                  void (*completed_cb)(void *, Eina_Hash *),
                                                  void *data);
void                  lastfm_response_free(Eina_Hash *urls);
char                 *lastfm_local_path_from_url_get(char *url);
Eina_Bool             lastfm_cover_download(char *url, void (*completed_cb)(void *data, char *local_file, Eina_Bool success), void *data);
void                  lastfm_cover_cache_package_set(const char *package);

#endif
