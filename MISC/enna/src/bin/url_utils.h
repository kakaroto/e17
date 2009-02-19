#ifndef __ENNA_URL_UTILS_H__
#define __ENNA_URL_UTILS_H__

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

typedef struct url_data_s
{
    int status;
    char *buffer;
    size_t size;
} url_data_t;

url_data_t url_get_data(CURL *curl, char *url);

void url_escape_string(char *outbuf, const char *inbuf);

void url_save_to_disk (CURL *curl, char *src, char *dst);

#endif
