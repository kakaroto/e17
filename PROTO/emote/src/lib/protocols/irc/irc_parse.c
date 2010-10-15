#include "irc.h"
#include "emote_private.h"

#include <ctype.h>
#include <Evas.h>
#include <unistd.h>

typedef struct _IRC_Line IRC_Line;

struct _IRC_Line
{
   const char *prefix;
   const char *source;
   const char *user;
   const char *host;
   const char *cmd;
   const char *trailing;
   const char *param_str;
   Eina_List *params;
};

typedef enum _PARSE_STATE PARSE_STATE;
typedef enum _IRC_COMMANDS IRC_COMMANDS;

enum _PARSE_STATE
{
   PARSE_PREFIX=0,
   PARSE_SOURCE=1,
   PARSE_USER=2,
   PARSE_HOST=3,
   PARSE_CMD=4,
   PARSE_PARAMS=5,
   PARSE_MIDDLE=6,
   PARSE_TRAILING_STR=7,
   PARSE_TRAILING=8,
   PARSE_END=9
};

enum _IRC_COMMANDS
{
   RPL_TOPIC=332,
   RPL_TOPICUSER=333,
   RPL_NAMREPLY=353,
   RPL_ENDOFNAMES=366
};

/* local function prototypes */
static void _irc_cleanup_irc_line(IRC_Line *line);
static char *_irc_parse_utf8_to_markup(const char *text);
static char *_irc_str_append(char *str, const char *txt, int *len, int *alloc);
static int _irc_split_line(const char *line, IRC_Line *out);
static void _irc_parse_line(char *line, const char *server, Emote_Protocol *m);
static Eina_List *_irc_parse_split_input(Eina_Strbuf **input);

void
irc_parse_input(char *input, const char *server, Emote_Protocol *m)
{
   Eina_List *lines, *l;
   static Eina_Strbuf *buf = NULL;
   char *line;

   if (!buf) buf = eina_strbuf_new();

   // Append new data to the buffer
   eina_strbuf_append(buf, input);

   lines = _irc_parse_split_input(&buf);

   EINA_LIST_FOREACH(lines, l, line)
     {
        _irc_parse_line(line, server, m);
     }

   EINA_LIST_FREE(l, line)
   free(line);
}

static void
_irc_parse_line(char *line, const char *server, Emote_Protocol *m)
{
   Emote_Event *event;
   Eina_List *p;
   char *param;
   IRC_Line ln;
   int ncmd;

//   printf("Parse Line: %s\n", line);

   if(!_irc_split_line(line, &ln)) return;

//   printf("\tPrefix = %s\n\tSource = %s\n\tUser = %s\n\tHost = %s\n\tCmd: %s\n\tTrailing: %s\n",
//          ln.prefix, ln.source, ln.user, ln.host, ln.cmd, ln.trailing);
   EINA_LIST_FOREACH(ln.params, p, param)
     {
//        printf("\tParam: %s\n", param);
     }

   ncmd = atoi(ln.cmd);
   event = NULL;

   if (!strcmp(ln.cmd, "PING"))
     {
        protocol_irc_pong(server, eina_list_nth(ln.params,0));
     }
   else if (!strcmp(ln.cmd, "NOTICE"))
     {
        event = emote_event_new(
            m,
            EMOTE_EVENT_SERVER_MESSAGE_RECEIVED,
            server,
            _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
            _irc_parse_utf8_to_markup(eina_list_nth(ln.params,1))
        );
     }
   else if (!strcmp(ln.cmd, "PRIVMSG"))
     {
        event = emote_event_new
            (
               m,
               EMOTE_EVENT_CHAT_MESSAGE_RECEIVED,
               server,
               _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
               _irc_parse_utf8_to_markup(ln.source),
               _irc_parse_utf8_to_markup(ln.trailing)
            );
     }
   else if (!strcmp(ln.cmd, "JOIN"))
     {
        event = emote_event_new
            (
               m,
               EMOTE_EVENT_CHAT_JOINED,
               server,
               _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
               _irc_parse_utf8_to_markup(ln.source)
            );
     }
   else if (!strcmp(ln.cmd, "PART"))
     {
        event = emote_event_new
            (
               m,
               EMOTE_EVENT_CHAT_PARTED,
               server,
               _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
               _irc_parse_utf8_to_markup(ln.source)
            );
     }
   else if (!strcmp(ln.cmd, "NICK"))
     {
        event = emote_event_new
                (
                   m,
                   EMOTE_EVENT_SERVER_NICK_CHANGED,
                   server,
                   _irc_parse_utf8_to_markup(ln.source),
                   _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0))
                );
     }
   else if (ncmd == RPL_TOPIC)
     {
        event = emote_event_new
                (
                   m,
                   EMOTE_EVENT_CHAT_TOPIC,
                   server,
                   _irc_parse_utf8_to_markup(eina_list_nth(ln.params,1)),
                   NULL,
                   _irc_parse_utf8_to_markup(ln.trailing)
                );
     }
   else if (!strcmp(ln.cmd, "TOPIC"))
     {
        event = emote_event_new
                (
                   m,
                   EMOTE_EVENT_CHAT_TOPIC,
                   server,
                   _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
                   _irc_parse_utf8_to_markup(ln.source),
                   _irc_parse_utf8_to_markup(ln.trailing)
                );
     }
   else if (ncmd ==  RPL_NAMREPLY)
     {
       event = emote_event_new
               (
                  m,
                  EMOTE_EVENT_CHAT_USERS,
                  server,
                  _irc_parse_utf8_to_markup(eina_list_nth(ln.params,2)),
                  NULL,
                  _irc_parse_utf8_to_markup(ln.trailing)
               );
     }
   else if (ncmd == RPL_ENDOFNAMES)
     {
       event = emote_event_new
               (
                  m,
                  EMOTE_EVENT_CHAT_USERS,
                  server,
                  _irc_parse_utf8_to_markup(eina_list_nth(ln.params,1)),
                  NULL,
                  NULL
               );
     }
   else if (ncmd == RPL_TOPICUSER)
     {
        // Don't really need to show this.
     }
   else if (ncmd != 0)
     {
        event = emote_event_new(
            m,
            EMOTE_EVENT_SERVER_MESSAGE_RECEIVED,
            server,
            NULL,
            _irc_parse_utf8_to_markup(ln.param_str)
        );
     }

   if (event) emote_event_send(event);
   _irc_cleanup_irc_line(&ln);
}

/* local functions */
static int
_irc_find_token_pos(const char *buf, int pos, int end, const char token, const char token2)
{
   int pos2;

   pos2 = pos;
   while ((pos2 < end) && (buf[++pos2] != token) && (buf[pos2] != token2));

   return pos2;
}

static Eina_List *
_irc_parse_split_input(Eina_Strbuf **input)
{
   Eina_List *l = NULL;
   int i, j, len;
   char *tok, *str;

   if (!input || !*input) return NULL;

   len = eina_strbuf_length_get(*input);
   if (len < 2) return NULL;

   // Copy strbuf string into a new location that we can modify
   str = strdup(eina_strbuf_string_get(*input));
   tok = str;
   j = 0;
   for (i = 0; i < (len-1); ++i)
   {
     if ((tok[i] == '\r') && (tok[i+1] == '\n'))
     {
        tok[i++] = 0;

        l = eina_list_append(l, strdup(&(tok[j])));
        j = ++i;
     }
   }

   // Update strbuf with remainder
   eina_strbuf_reset(*input);
   eina_strbuf_append(*input, &(tok[j]));

   free(str);

   return l;
}

static void
_irc_cleanup_irc_line(IRC_Line *line)
{
   char *param;

   if (!line) return;

   if (line->prefix)
      eina_stringshare_del(line->prefix);
   if (line->source)
      eina_stringshare_del(line->source);
   if (line->user)
      eina_stringshare_del(line->user);
   if (line->host)
      eina_stringshare_del(line->host);
   if (line->cmd)
      eina_stringshare_del(line->cmd);
   if (line->trailing)
      eina_stringshare_del(line->trailing);
   if (line->params)
     {
        EINA_LIST_FREE(line->params, param)
          eina_stringshare_del(param);
     }
}

static int
_irc_split_line(const char *line, IRC_Line *out)
{
   char buf[8192];
   PARSE_STATE state;
   int pos, pos2, pos3;
   int length;

   // Check for null or blank line
   if (!line || !line[0]) return 0;

   strncpy(buf, line, sizeof(buf));
   length = strlen(buf);

   memset(out, 0, sizeof(IRC_Line));

   state = PARSE_PREFIX;
   pos = pos2 = 0;
   while(state < PARSE_END)
     {
        switch(state)
          {
             case PARSE_PREFIX:
                if (buf[0] != ':')
                {
                   state = PARSE_CMD;
                   break;
                }

                pos = 0;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), 0, ' ');

                state = PARSE_SOURCE;

                buf[pos2] = 0;
                out->prefix = eina_stringshare_add(&(buf[pos]));
                buf[pos2] = ' ';
                break;
             case PARSE_SOURCE:
                // Check if there is a prefix or not
                if (buf[0] != ':')
                   state = PARSE_CMD;

                pos = 1;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), '!', ' ');

                if (buf[pos2] == '!')
                  state = PARSE_USER;
                else
                  state = PARSE_CMD;

                buf[pos2] = 0;
                out->source = eina_stringshare_add(&(buf[pos]));

                pos2++;
                break;

             case PARSE_USER:
                pos = pos2;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), '@', ' ');

                if (buf[pos2] == '@')
                  state = PARSE_HOST;
                else
                  state = PARSE_CMD;

                buf[pos2] = 0;
                out->user = eina_stringshare_add(&(buf[pos]));

                pos2++;
                break;

             case PARSE_HOST:
                pos = pos2;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), 0, ' ');

                state = PARSE_CMD;

                buf[pos2] = 0;
                out->host = eina_stringshare_add(&(buf[pos]));

                pos2++;
                break;

             case PARSE_CMD:
                pos = pos2;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), 0, ' ');
                buf[pos2] = 0;
                out->cmd = eina_stringshare_add(&(buf[pos]));

                pos2++;
                if (buf[pos2] == ':')
                {
                  pos2++;
                  state = PARSE_TRAILING_STR;
                }
                else
                  state = PARSE_PARAMS;
                break;

             case PARSE_PARAMS:
                pos3 = pos = pos2;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), 0, 0);
                out->param_str = eina_stringshare_add(&(buf[pos]));

                state = PARSE_MIDDLE;
                pos2 = pos3;
                break;

             case PARSE_MIDDLE:
                state = PARSE_END;
                while (pos2 < length)
                  {
                     pos = pos2;
                     pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), ' ', 0);
                     buf[pos2] = 0;
                     out->params = eina_list_append(out->params, eina_stringshare_add(&(buf[pos])));

                     pos2++;
                     if (buf[pos2] == ':')
                       {
                          pos2++;
                          state = PARSE_TRAILING_STR;
                          break;
                       }
                  }
                break;

             case PARSE_TRAILING_STR:
                pos3 = pos = pos2;
                pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), 0, 0);
                out->trailing = eina_stringshare_add(&(buf[pos]));

                state = PARSE_TRAILING;
                pos2 = pos3;
                break;

             case PARSE_TRAILING:
                while (pos2 < length)
                  {
                     pos = pos2;
                     pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), ':', 0);
                     buf[pos2] = 0;
                     out->params = eina_list_append(out->params, eina_stringshare_add(&(buf[pos])));
                     pos2++;
                  }
                state = PARSE_END;
                break;

             case PARSE_END:
             default:
                break;
        }
     }

  return 1;
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
