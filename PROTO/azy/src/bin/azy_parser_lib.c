/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "azy_parser_lib.h"

#ifndef HAVE_STRNDUP
char *
strndup(const char *s, size_t n)
{
   char *ret;

   ret = malloc(n + 1);
   if (!ret) return NULL;

   memcpy(ret, s, n);
   ret[n] = 0;
   return ret;
}
#endif

Azy_Stream *
azy_parser_stream_new(const char *str)
{
   Azy_Stream *s;

   if (!str) return NULL;

   s = calloc(1, sizeof(Azy_Stream));
   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);
   s->buffer = strdup(str);
   s->length = strlen(s->buffer);
   return s;
}

void
azy_parser_stream_next(Azy_Stream *s,
                       int length)
{
   int i = 0;

   EINA_SAFETY_ON_NULL_RETURN(s);

   for (; i < length && s->index < s->length; i++, s->index++)
     {
        s->col++;

        if (s->buffer[s->index] == '\n')
          {
             s->col = 0;
             s->line++;
          }
     }
}

Azy_Token *
azy_parser_token_new(Azy_Stream *s,
                     int type,
                     int length)
{
   int i;
   Azy_Token *t;

   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);

   t = calloc(1, sizeof(Azy_Token));
   EINA_SAFETY_ON_NULL_RETURN_VAL(t, NULL);

   t->type = type;
   t->text = eina_stringshare_add_length(s->buffer + s->index, length);
   t->length = length;
   t->sline = t->eline = s->line;
   t->scol = t->ecol = s->col;

   for (i = 0; i < length; i++)
     {
        t->ecol++;

        if (t->text[i] == '\n')
          {
             t->ecol = 0;
             t->eline++;
          }
     }

   azy_parser_stream_next(s, length);
   return t;
}

void
azy_parser_token_free(Azy_Token *t)
{
   EINA_SAFETY_ON_NULL_RETURN(t);

   eina_stringshare_del(t->text);
   free(t);
}

static int
azy_parser_parse(Azy_Parser *parser,
                 Azy_Parser_Cb cb,
                 Azy_Parser_New_Cb new_cb,
                 Azy_Parser_Free_Cb free_cb,
                 Azy_Token_Cb token_cb)
{
   void *p;
   Azy_Token *t;
   Azy_Stream *s;
   int retval = -1;

   if ((!parser) || (parser->error) || (!parser->stream))
     return retval;

   s = parser->stream;
   p = new_cb((void *(*)(size_t))malloc);

   while ((t = token_cb(s)) != NULL)
     {
        if (t->type < 0)
          {
             parser->error = eina_stringshare_printf("Unknown Azy_Token '%s' at line %d char %d\n", t->text, t->sline, t->scol);
             azy_parser_token_free(t);
             goto err;
          }
        else if (!t->type)
          {
             azy_parser_token_free(t);
             cb(p, 0, NULL, parser);
             break;
          }

        cb(p, t->type, t, parser);

        if (parser->error)
          goto err;
     }

   retval = 0;
err:
   free_cb(p, free);
   return retval;
}

void *
azy_parser_string_parse_azy(const char *str,
                            Eina_Bool *err,
                            Azy_Parser_Cb cb,
                            Azy_Parser_New_Cb new_cb,
                            Azy_Parser_Free_Cb free_cb,
                            Azy_Token_Cb token_cb)
{
   Azy_Parser *parser;
   void *ret;

   EINA_SAFETY_ON_NULL_RETURN_VAL(str, NULL);

   parser = calloc(1, sizeof(Azy_Parser));
   EINA_SAFETY_ON_NULL_RETURN_VAL(parser, NULL);

   parser->stream = azy_parser_stream_new(str);

   if (!parser->stream)
     {
        free(parser);
        fprintf(stderr, "Stream can't be created!");
        *err = EINA_TRUE;
        return NULL;
     }

   azy_parser_parse(parser, cb, new_cb, free_cb, token_cb);
   if (parser->error)
     {
        fprintf(stderr, "%s\n", parser->error);
        *err = EINA_TRUE;
        return NULL;
     }
   /* validate types */
   {
      Eina_List *l;
      Azy_Typedef *t;
      EINA_LIST_FOREACH(parser->data->types, l, t)
        {
           if (t->struct_members || (t->type != TD_STRUCT)) continue;

           fprintf(stderr, "Undefined type %s!\n", t->name);
           exit(1);
        }
   }

   if (parser->error)
     {
        fprintf(stderr, "%s\n", parser->error);
        *err = EINA_TRUE;
        return NULL;
     }

   ret = parser->data;
   free(parser->stream);
   free(parser);
   return ret;
}

void *
azy_parser_file_parse_azy(const char *path,
                          Eina_Bool *err,
                          Azy_Parser_Cb cb,
                          Azy_Parser_New_Cb new_cb,
                          Azy_Parser_Free_Cb free_cb,
                          Azy_Token_Cb token_cb)
{
   void *data, *buf;
   struct stat s;
   int fd;

   if (stat(path, &s) < 0) return NULL;
   if ((fd = open(path, O_RDONLY)) < 0) return NULL;

   buf = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
   close(fd);
   if (buf == MAP_FAILED) return NULL;

   data = azy_parser_string_parse_azy(buf, err, cb, new_cb, free_cb, token_cb);
   munmap(buf, s.st_size);
   return data;
}

