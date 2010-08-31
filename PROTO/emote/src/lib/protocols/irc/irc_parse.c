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
   PARSE_MIDDLE=5,
   PARSE_TRAILING=6,
   PARSE_END=7
};

enum _IRC_COMMANDS
{
   RPL_TOPIC=332,
   RPL_TOPICUSER=333,
   RPL_NAMREPLY=353,
   RPL_ENDOFNAMES=366
};

/* local function prototypes */
static Eina_List *_irc_parse_split_input(const char *input);
static void _irc_cleanup_irc_line(IRC_Line *line);
static char *_irc_parse_utf8_to_markup(const char *text);
static char *_irc_str_append(char *str, const char *txt, int *len, int *alloc);
static int _irc_parse_line(const char *line, IRC_Line *out);

void
irc_parse_input(char *input, const char *server, Emote_Protocol *m)
{
   static char buf[8192];
   static int length = 0;
   IRC_Line ln;
   Eina_List *lines, *l, *p;
   char *line, *param;
   int size;

   if ((length == 1) && (buf[0] == 0))
     length = 0;

   strncpy(&(buf[length]), input, sizeof(buf)-length);
   length += strlen(input);

   /* NB: Any parsing of messages after this split will need to append
    * a new line if printing to the screen as this split strips them out
    */
   lines = _irc_parse_split_input(buf);
   EINA_LIST_FOREACH(lines, l, line)
     {
        printf("Parse Line: %s\n", line);

        size = strlen(line);
        length -= size+2;
        memmove(buf, &(buf[size+2]), length);
        buf[length] = 0;

        if(!_irc_parse_line(line, &ln)) continue;

/*        printf("\tPrefix = %s\n\tSource = %s\n\tUser = %s\n\tHost = %s\n\tCmd: %s\n",
               ln.prefix, ln.source, ln.user, ln.host, ln.cmd);*/
        /*EINA_LIST_FOREACH(ln.params, p, param)
          {
             printf("\tParam: %s\n", param);
          }*/

        if (!strcmp(ln.cmd, "PING"))
          {
             protocol_irc_pong(server, ln.prefix);
          }
        else if (!strcmp(ln.cmd, "NOTICE"))
          {
             Emote_Event *d;

             d = emote_event_new(
                 m,
                 EMOTE_EVENT_SERVER_MESSAGE_RECEIVED,
                 server,
                 _irc_parse_utf8_to_markup(eina_list_nth(ln.params,1))
             );
             emote_event_send(d);
          }
        else if (!strcmp(ln.cmd, "PRIVMSG"))
          {
             Emote_Event *d;

             d = emote_event_new
                 (
                    m,
                    EMOTE_EVENT_CHAT_MESSAGE_RECEIVED,
                    server,
                    _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
                    _irc_parse_utf8_to_markup(ln.source),
                    _irc_parse_utf8_to_markup(eina_list_nth(ln.params,1))
                 );
             emote_event_send(d);
          }
        else if (!strcmp(ln.cmd, "JOIN"))
          {
             Emote_Event *d;

             d = emote_event_new
                 (
                    m,
                    EMOTE_EVENT_CHAT_JOINED,
                    server,
                    _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
                    _irc_parse_utf8_to_markup(ln.source)
                 );
             emote_event_send(d);
          }
        else if (!strcmp(ln.cmd, "PART"))
          {
             Emote_Event *d;

             printf("Received PART: %s, %s, %s\n", server, _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)), _irc_parse_utf8_to_markup(ln.source));

             d = emote_event_new
                 (
                    m,
                    EMOTE_EVENT_CHAT_PARTED,
                    server,
                    _irc_parse_utf8_to_markup(eina_list_nth(ln.params,0)),
                    _irc_parse_utf8_to_markup(ln.source)
                 );
             emote_event_send(d);
          }
        else if (atoi(ln.cmd) ==  RPL_NAMREPLY)
          {
            // TODO: Add command for sending channel list
          }
        else if (atoi(ln.cmd) == RPL_TOPICUSER)
          {
            // Don't really need to show this.
          }
        else if (atoi(ln.cmd) != 0)
          {
             Emote_Event *d;
             Eina_List *l;
             char *p;
             char buf[8192];

             buf[0] = 0;
             EINA_LIST_FOREACH(ln.params->next, l, p)
               {
                  strncat(buf, p, sizeof(buf));
                  strncat(buf, " ", sizeof(buf));
               }

             d = emote_event_new(
                 m,
                 EMOTE_EVENT_SERVER_MESSAGE_RECEIVED,
                 server,
                 _irc_parse_utf8_to_markup(buf)
             );
             emote_event_send(d);
          }

        _irc_cleanup_irc_line(&ln);
     }

   EINA_LIST_FREE(l, line)
   free(line);
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
_irc_parse_split_input(const char *input)
{
   char buf[8192];
   Eina_List *l = NULL;
   char *tok = NULL, *str, *str2;
   int length, pos, pos2;
   int i = 0;

   str2 = str = strdup(input);
   while ((tok = strsep(&str, "\r\n")))
     {
        if (!str) break;
        if ((*tok == '\0') || (*tok == '\n')) continue;
        l = eina_list_append(l, strdup(tok));
     }
   free(str2);
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
   if (line->params)
     {
        EINA_LIST_FREE(line->params, param)
          eina_stringshare_del(param);
     }
}

static int
_irc_parse_line(const char *line, IRC_Line *out)
{
   char buf[8192];
   PARSE_STATE state;
   int pos, pos2;
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
                   state = PARSE_CMD;

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
                  state = PARSE_TRAILING;
                }
                else
                  state = PARSE_MIDDLE;
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
                          state = PARSE_TRAILING;
                          break;
                       }
                  }
                break;

             case PARSE_TRAILING:
                while (pos2 < length)
                  {
                     pos = pos2;
                     pos2 = _irc_find_token_pos(buf, pos, sizeof(buf), ':', 0);
                     buf[pos2] = 0;
                     out->params = eina_list_append(out->params, eina_stringshare_add(&(buf[pos])));
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
