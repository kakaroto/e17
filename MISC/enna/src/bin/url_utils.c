#include "enna.h"
#include "url_utils.h"

static size_t
url_buffer_get (void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  url_data_t *mem = (url_data_t *) data;

  mem->buffer = realloc (mem->buffer, mem->size + realsize + 1);
  if (mem->buffer)
  {
    memcpy (&(mem->buffer[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0;
  }
  
  return realsize;
}

url_data_t
url_get_data (CURL *curl, char *url)
{
  url_data_t chunk;

  chunk.buffer = NULL; /* we expect realloc(NULL, size) to work */
  chunk.size = 0;      /* no data at this point */

  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_buffer_get);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) &chunk);

  chunk.status = curl_easy_perform (curl);

  return chunk;
}

void
url_escape_string (char *outbuf, const char *inbuf)
{
  unsigned char c, c1, c2;
  int i, len;

  len = strlen (inbuf);

  for  (i = 0; i < len; i++)
  {
    c = inbuf[i];
    if ((c == '%') && i < len - 2)
    { /* need 2 more characters */
      c1 = toupper (inbuf[i + 1]);
      c2 = toupper (inbuf[i + 2]);
      /* need uppercase chars */
    }
    else
    {
      c1 = 129;
      c2 = 129;
      /* not escaped chars */
    }

    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || (c >= '0' && c <= '9') || (c >= 0x7f))
      *outbuf++ = c;
    else if (c =='%'
             && ((c1 >= '0' && c1 <= '9') || (c1 >= 'A' && c1 <= 'F'))
             && ((c2 >= '0' && c2 <= '9') || (c2 >= 'A' && c2 <= 'F')))
    {
      /* check if part of an escape sequence */
      *outbuf++ = c; /* already escaped */
    }
    else
    {
      /* all others will be escaped */
      c1 = ((c & 0xf0) >> 4);
      c2 = (c & 0x0f);
      if (c1 < 10)
        c1 += '0';
      else
        c1 += 'A' - 10;
      if (c2 < 10)
        c2 += '0';
      else
        c2 += 'A' - 10;
      *outbuf++ = '%';
      *outbuf++ = c1;
      *outbuf++ = c2;
    }
  }
  *outbuf++='\0';
}
