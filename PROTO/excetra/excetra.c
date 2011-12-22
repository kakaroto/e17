#include <Evas.h>
#include <Ecore_Con.h>
#include <Azy.h>
#include <Lyricwiki_Common_Azy.h>

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#ifndef strdupa
# define strdupa(str)       strcpy(alloca(strlen(str) + 1), str)
#endif

#ifndef strndupa
# define strndupa(str, len) strncpy(alloca(len + 1), str, len)
#endif

#define DBG(...)            EINA_LOG_DOM_DBG(excetra_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(excetra_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(excetra_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(excetra_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(excetra_log_dom, __VA_ARGS__)

int excetra_log_dom = -1;

#define LYRICWIKI_SEARCH_TRACK_SIZE sizeof("http://lyrics.wikia.com/api.php?artist=&song=&fmt=json")
#define LYRICWIKI_SEARCH_TRACK "http://lyrics.wikia.com/api.php?artist="


Eina_Bool
lyricwiki_data(void *d __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Url_Data *ev)
{
   Eina_Strbuf *buf = ecore_con_url_data_get(ev->url_con);

   //DBG("Received %i bytes of lyric: %s", ev->size, ecore_con_url_url_get(ev->url_con));
   if (buf)
     eina_strbuf_append_length(buf, (char*)&ev->data[0], ev->size);
   else
     {
        buf = eina_strbuf_new();
        eina_strbuf_append_length(buf, (char*)&ev->data[7], ev->size - 7);
     }
   ecore_con_url_data_set(ev->url_con, buf);
   return ECORE_CALLBACK_RENEW;
}

char *
lyricwiki_parse_lyric(Eina_Strbuf *buf)
{
   size_t size;
   char *lyric_start, *lyric_end;
   size = eina_strbuf_length_get(buf);
   if (!size)
     {
        ERR("size is 0!");
        return NULL;
     }
   /* FIXME: overflows */
   lyric_start = strstr(eina_strbuf_string_get(buf), "phone_right.gif");
   if (!lyric_start) goto error;
   lyric_start = strstr(lyric_start, "</div>");
   if (!lyric_start) goto error;
   lyric_start += sizeof("</div>") - 1;
   lyric_end = strstr(lyric_start, "<!--");
   if (!lyric_end) goto error;
   *lyric_end = 0;
   return evas_textblock_text_markup_to_utf8(NULL, lyric_start);
error:
   ERR("Parsing error!");
   return NULL;
}

void
lyricwiki_parse_stub(Eina_Strbuf *buf)
{
   size_t size;
   Azy_Net *net;
   Azy_Content *content;
   Lyricwiki_Lyric *lwl;
   size = eina_strbuf_length_get(buf);
   DBG("\n%s", eina_strbuf_string_get(buf));
   if (!size)
     {
        ERR("size is 0!");
        return;
     }
   net = azy_net_buffer_new(eina_strbuf_string_steal(buf), size, AZY_NET_TRANSPORT_JSON, EINA_TRUE);
   content = azy_content_new(NULL);
   azy_content_deserialize(content, net);
   if (azy_value_to_Lyricwiki_Lyric(azy_content_retval_get(content), &lwl))
     {
        Ecore_Con_Url *url;
        INF("STUB:");
        Lyricwiki_Lyric_print(NULL, 0, lwl);
        url = ecore_con_url_new(lwl->url);
        ecore_con_url_data_set(url, eina_strbuf_new());
        ecore_con_url_get(url);
     }

   Lyricwiki_Lyric_free(lwl);
   azy_net_free(net);
   azy_content_free(content);
}

Eina_Bool
lyricwiki_complete(void *d __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Url_Complete *ev)
{
   Eina_Strbuf *buf = ecore_con_url_data_get(ev->url_con);
   const Eina_List *headers, *l;
   const char *h;
   char *lyric = NULL;
   Eina_Bool stub = EINA_TRUE;
   Azy_Net_Transport tp;
   DBG("%d code for lyrics: %s", ev->status, ecore_con_url_url_get(ev->url_con));
   headers = ecore_con_url_response_headers_get(ev->url_con);
   EINA_LIST_FOREACH(headers, l, h)
     {
        if (strncasecmp(h, "content-type: ", sizeof("content-type: ") - 1)) continue;
        h += sizeof("content-type: ") - 1;

        tp = azy_transport_get(h);
        if (tp == AZY_NET_TRANSPORT_JAVASCRIPT) break;
        if (tp == AZY_NET_TRANSPORT_HTML)
          {
             stub = EINA_FALSE;
             break;
          }
        
        eina_strbuf_free(buf);
        ecore_con_url_free(ev->url_con);
        return ECORE_CALLBACK_RENEW;
     }
   if (ev->status != 200)
     {
        eina_strbuf_free(buf);
        ecore_con_url_free(ev->url_con);
        return ECORE_CALLBACK_RENEW;
     }
   if (stub) lyricwiki_parse_stub(buf);
   else
     lyric = lyricwiki_parse_lyric(buf);
   if (lyric)
     {
        INF("\n%s", lyric);
        free(lyric);
        ecore_main_loop_quit();
     }
   eina_strbuf_free(buf);
   ecore_con_url_free(ev->url_con);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
lyricwiki_request(const char *artist, const char *song)
{
   Ecore_Con_Url *url;
   size_t size;
   char *buf;
   char *a, *s, *p;

   if ((!artist) || (!artist[0]) || (!song) || (!song[0])) return EINA_FALSE;

   a = strdup(artist);
   for (p = strchr(a, ' '); p; p = strchr(p + 1, ' '))
     *p = '_';
   s = strdup(song);
   for (p = strchr(s, ' '); p; p = strchr(p + 1, ' '))
     *p = '_';
   size = strlen(artist) + strlen(song) + LYRICWIKI_SEARCH_TRACK_SIZE;
   buf = malloc(size);
   snprintf(buf, size, LYRICWIKI_SEARCH_TRACK "%s&song=%s&fmt=json", a, s);
   free(a);
   free(s);
   url = ecore_con_url_new(buf);
   ecore_con_url_get(url);
   return EINA_TRUE;
}

int
excetra_init(void)
{
   eina_init();
   ecore_init();
   excetra_log_dom = eina_log_domain_register("excetra", EINA_COLOR_CYAN);
   eina_log_domain_level_set("excetra", EINA_LOG_LEVEL_DBG);
   if (excetra_log_dom == -1)
     {
        fprintf(stderr, "Could not init log domain!\n");
        return 0;
     }
   if (!ecore_con_url_init())
     {
        ERR("CURL support is required!");
        return 0;
     }
   evas_init();
   azy_init();
   ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, (Ecore_Event_Handler_Cb)lyricwiki_complete, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, (Ecore_Event_Handler_Cb)lyricwiki_data, NULL);
   return 1;
}

int
main(int argc, char *argv[])
{
   if (argc != 3)
     {
        fprintf(stderr, "USAGE: %s ARTIST SONG\n", argv[0]);
        exit(1);
     }
   if (!excetra_init()) exit(1);
   lyricwiki_request(argv[1], argv[2]);
   ecore_main_loop_begin();
   return 0;
}
