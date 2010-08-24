#include <ctype.h>
#include <Evas.h>

#include "irc_parse.h"
#include "emote_private.h"

/* local function prototypes */
static Eina_List *_irc_parse_split_input(const char *input);
static int _irc_parse_split_prefix(const char *input, int start, Eina_Strbuf **prefix);
static int _irc_parse_split_command(const char *input, int start, Eina_Strbuf **cmd);
static int _irc_parse_split_params(const char *input, int start, Eina_Strbuf **params);
static int _irc_parse_remove_username(const char *input, int start, Eina_Strbuf **params);
static char *_irc_parse_utf8_to_markup(const char *text);
static char *_irc_str_append(char *str, const char *txt, int *len, int *alloc);

void 
irc_parse_input(char *input, const char *server, Emote_Protocol *m) 
{
   Eina_Strbuf *buff;
   Eina_Strbuf *prefix, *cmd, *params;
   Eina_List *lines, *l;
   char *line;

   printf("Parse Input: %s\n", input);

   /* NB: Any parsing of messages after this split will need to append
    * a new line if printing to the screen as this split strips them out.
    * 
    * This is currently handled in the individual _parse_ functions */
   lines = _irc_parse_split_input(input);

   buff = eina_strbuf_new();
   prefix = eina_strbuf_new();
   cmd = eina_strbuf_new();
   params = eina_strbuf_new();

   EINA_LIST_FOREACH(lines, l, line) 
     {
        const char *str;
        int pos;

        pos = _irc_parse_split_prefix(line, 0, &prefix);

        if (strstr(line, "PING")) 
          {
             //irc_pong(server, eina_strbuf_string_get(prefix));
             goto reset;
          }

        pos = _irc_parse_split_command(line, pos, &cmd);
        printf("Command: %s\n", eina_strbuf_string_get(cmd));

        pos = _irc_parse_split_params(line, pos, &params);
        printf("Params: %s\n", eina_strbuf_string_get(params));

        /* NB: Based on the command, pass off to parsing functions.
         * The 'params' may need special parsing based on command */
        if ((str = eina_strbuf_string_get(cmd))) 
          {
             const char *p;
             char *msg = NULL;

             /* Don't print Server Info */
             if (!strcmp(str, "004")) goto reset;
             /* Don't print current channels user list to the tb */
             if (!strcmp(str, "353")) goto reset;

             p = eina_strbuf_string_get(params);
             if (!strcmp(str, "NOTICE")) 
               {
                  if ((msg = _irc_parse_utf8_to_markup(p)))
                    {
                       char fmt[5012];
                       Emote_Event_Chat_Server_Message *d;

                       snprintf(fmt, sizeof(fmt), " <color=#ff0000>%s</> ", msg);
                       d = EMOTE_NEW(Emote_Event_Chat_Server_Message, 1);
                       d->protocol = m;
                       d->server = server;
                       d->message = msg;
                       emote_event_send(EMOTE_EVENT_CHAT_SERVER_MESSAGE_RECEIVED, d);
                    }
                  goto reset;
               }
             else if (!strcmp(str, "JOIN"))
               {
                  Emote_Event_Chat_Channel_Add *d;

                  d = EMOTE_NEW(Emote_Event_Chat_Channel_Add, 1);
                  d->protocol = m;
                  d->server = server;
                  d->channel = p;
                  emote_event_send(EMOTE_EVENT_CHAT_CHANNEL_ADD, d);
                  goto reset;
               }
             else if (
                      ((atoi(str) >= 001) && (atoi(str) <= 003)) ||
                      (atoi(str) == 251) || (atoi(str) == 255) || 
                      (atoi(str) == 250) || 
                      ((atoi(str) >= 372) && (atoi(str) <= 376))
                      )
               {
                  /* I DON'T LIKE THIS AT ALL :(
                   * NOT HAPPY WITH THE WAY THIS IS GETTING DONE */

                  Eina_Strbuf *tmp;
                  const char *txt;
                  int pos2;
                  Emote_Event_Chat_Server_Message *d;

                  /* NB: For these messages, we need to strip the username :*/
                  tmp = eina_strbuf_new();
                  txt = eina_strbuf_string_get(params);
                  pos2 = _irc_parse_remove_username(txt, 0, &tmp);
                  p = eina_strbuf_string_get(tmp);
                  msg = _irc_parse_utf8_to_markup(p);
                  d = EMOTE_NEW(Emote_Event_Chat_Server_Message, 1);
                  d->protocol = m;
                  d->server = server;
                  d->message = msg;
                  emote_event_send(EMOTE_EVENT_CHAT_SERVER_MESSAGE_RECEIVED, d);
                  eina_strbuf_free(tmp);
               }
             else
               {
                  Emote_Event_Chat_Channel_Message *d;

                  msg = _irc_parse_utf8_to_markup(p);
                  d = EMOTE_NEW(Emote_Event_Chat_Channel_Message, 1);
                  d->protocol = m;
                  d->server = server;
                  d->channel = NULL;
                  d->user = NULL;
                  d->message = msg;
                  emote_event_send(EMOTE_EVENT_CHAT_CHANNEL_MESSAGE_RECEIVED, d);
               }
          }
reset:
        eina_strbuf_reset(buff);
        eina_strbuf_reset(prefix);
        eina_strbuf_reset(cmd);
        eina_strbuf_reset(params);
     }

   /* clenaup */
   eina_strbuf_free(buff);
   eina_strbuf_free(prefix);
   eina_strbuf_free(cmd);
   eina_strbuf_free(params);

   EINA_LIST_FREE(l, line)
     free(line);
}

/* local functions */
static Eina_List *
_irc_parse_split_input(const char *input) 
{
   Eina_List *l = NULL;
   char *tok = NULL, *str;

   str = strdup(input);
   while ((tok = strsep(&str, "\r\n"))) 
     {
        if ((*tok == '\0') || (*tok == '\n')) continue;
        l = eina_list_append(l, strdup(tok));
     }
   free(str);
   return l;
}

static int 
_irc_parse_split_prefix(const char *input, int start, Eina_Strbuf **prefix)
{
   int chr, pos = 0;

   if (!input) return 0;

   pos = start;
   while ((pos = evas_string_char_next_get((char *)input, pos, &chr))) 
     {
        if ((chr <= 0) || (pos <= 0)) break;
        if (chr == ':') 
          {
             int chr2, pos2 = 0;

             pos2 = pos;
//             eina_strbuf_append_char(*prefix, chr);
             while ((pos2 = evas_string_char_next_get((char *)input, pos2, &chr2))) 
               {
                  if ((chr2 <= 0) || (pos2 <= 0)) break;
                  if (chr2 != ' ') 
                    eina_strbuf_append_char(*prefix, chr2);
                  else 
                    return pos2;
               }
             pos = pos2;
          }
     }
   return pos;
}

static int 
_irc_parse_split_command(const char *input, int start, Eina_Strbuf **cmd)
{
   int chr, pos = 0;

   if (!input) return 0;

   pos = start - 1;
   while ((pos = evas_string_char_next_get((char *)input, pos, &chr))) 
     {
        if ((chr <= 0) || (pos <= 0)) break;
        if (chr != ' ') 
          {
             int chr2, pos2 = 0;

             pos2 = pos;
             eina_strbuf_append_char(*cmd, chr);
             while ((pos2 = evas_string_char_next_get((char *)input, pos2, &chr2))) 
               {
                  if ((chr2 <= 0) || (pos2 <= 0)) break;
                  if (chr2 != ' ') 
                    eina_strbuf_append_char(*cmd, chr2);
                  else 
                    return pos2;
               }
             pos = pos2;
          }
     }
   return pos;
}

static int 
_irc_parse_split_params(const char *input, int start, Eina_Strbuf **params) 
{
   int chr, pos = 0;

   if (!input) return 0;

   pos = start;
   while ((pos = evas_string_char_next_get((char *)input, pos, &chr))) 
     {
        if ((chr <= 0) || (pos <= 0)) break;
        if (chr == '\r') 
          eina_strbuf_append_char(*params, '\n');
        else if (!iscntrl(chr))
          eina_strbuf_append_char(*params, chr);
     }
   eina_strbuf_append_char(*params, '\n');
   return pos;
}

static int 
_irc_parse_remove_username(const char *input, int start, Eina_Strbuf **params) 
{
   int chr, pos = 0;

   if (!input) return 0;

   pos = start;
   while ((pos = evas_string_char_next_get((char *)input, pos, &chr))) 
     {
        int chr2, pos2 = 0;

        if ((chr <= 0) || (pos < 0)) break;

        /* skip until we hit the : */
        if (chr != ':') continue;

        pos2 = pos;
        eina_strbuf_append_char(*params, chr);
        while ((pos2 = evas_string_char_next_get((char *)input, pos2, &chr2))) 
          {
             if ((chr2 <= 0) || (pos2 <= 0)) break;
             if (chr2 == '\r') 
               eina_strbuf_append_char(*params, '\n');
             else if (!iscntrl(chr2))
               eina_strbuf_append_char(*params, chr2);
          }
        pos = pos2;
     }
   eina_strbuf_append_char(*params, '\n');
   return pos;
}

static char *
_irc_parse_utf8_to_markup(const char *text)
{
   char *str = NULL;
   int str_len = 0, str_alloc = 0;
   int ch, pos = 0, pos2 = 0;

   if (!text) return NULL;
   for (;;)
     {
	pos = pos2;
        pos2 = evas_string_char_next_get((char *)(text), pos2, &ch);
        if ((ch <= 0) || (pos2 <= 0)) break;
	if (ch == '\n')
          str = _irc_str_append(str, "<br>", &str_len, &str_alloc);
	else if (ch == '\t')
          str = _irc_str_append(str, "<\t>", &str_len, &str_alloc);
	else if (ch == '<')
          str = _irc_str_append(str, "&lt;", &str_len, &str_alloc);
	else if (ch == '>')
          str = _irc_str_append(str, "&gt;", &str_len, &str_alloc);
	else if (ch == '&')
          str = _irc_str_append(str, "&amp;", &str_len, &str_alloc);
	else
	  {
	     char tstr[16];

	     strncpy(tstr, text + pos, pos2 - pos);
	     tstr[pos2 - pos] = 0;
	     str = _irc_str_append(str, tstr, &str_len, &str_alloc);
	  }
     }
   return str;
}

static char *
_irc_str_append(char *str, const char *txt, int *len, int *alloc)
{
   int txt_len = strlen(txt);

   if (txt_len <= 0) return str;
   if ((*len + txt_len) >= *alloc)
     {
	char *str2;
	int alloc2;

	alloc2 = *alloc + txt_len + 128;
	str2 = realloc(str, alloc2);
	if (!str2) return str;
	*alloc = alloc2;
	str = str2;
     }
   strcpy(str + *len, txt);
   *len += txt_len;
   return str;
}
