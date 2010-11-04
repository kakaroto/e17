/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

/*
 * Copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 *
 * This file is part of libxr.
 *
 * Libxr is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option) any
 * later version.
 *
 * Libxr is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libxr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "parser_lib.h"

/* stream api */

stream *
stream_new_from_string(const char *str)
{
   if (str == NULL)
     return NULL;

   stream *s = calloc(sizeof(stream), 1);
   s->buffer = eina_stringshare_add(str);
   s->length = eina_stringshare_strlen(s->buffer);
   return s;
}

void
stream_advance(stream *s,
               int     length)
{
   int i;

   for (i = 0; i < length && s->index < s->length; i++, s->index++)
     {
        s->col++;

        if (s->buffer[s->index] == '\n')
          {
             s->col = 0;
             s->line++;
          }
     }
}

/* tokenizer api */

token *
token_new(stream *s,
          int     type,
          int     length)
{
   int i;
   token *t = calloc(sizeof(token), 1);

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

   stream_advance(s, length);
   return t;
}

void
token_free(token *t)
{
   if (t == NULL)
     return;

   eina_stringshare_del(t->text);
   free(t);
}

/* parser api */

static int
__parse(parser_context *ctx,
        parser          parser_cb,
        parser_alloc    parser_alloc_cb,
        parser_free     parser_free_cb,
        lexer           lexer_cb)
{
   void *parser;
   token *t;
   stream *s = ctx->stream;
   int retval = -1;

   if (ctx == NULL || ctx->error != NULL || ctx->stream == NULL)
     return -1;

   parser = parser_alloc_cb((void *(*)(size_t))malloc);

   while ((t = lexer_cb(s)) != NULL)
     {
        if (t->type == TK_UNKNOWN)
          {
             ctx->error = eina_stringshare_printf("Unknown token '%s' at line %d char %d\n", t->text, t->sline, t->scol);
             token_free(t);
             goto err;
          }
        else if (t->type == TK_EOF)
          {
             token_free(t);
             parser_cb(parser, 0, NULL, ctx);
             break;
          }

        parser_cb(parser, t->type, t, ctx);

        if (ctx->error)
          goto err;
     }

   retval = 0;
err:
   parser_free_cb(parser, free);
   return retval;
}

void *
__parse_string(const char  *str,
               Eina_Bool   *err,
               parser       parser_cb,
               parser_alloc parser_alloc_cb,
               parser_free  parser_free_cb,
               lexer        lexer_cb)
{
   if (!str)
     return NULL;
   parser_context *ctx = calloc(sizeof(parser_context), 1);

   ctx->stream = stream_new_from_string(str);

   if (ctx->stream == NULL)
     {
        free(ctx);
        fprintf(stderr, "Stream can't be created!");
        *err = EINA_TRUE;
        return NULL;
     }

   __parse(ctx, parser_cb, parser_alloc_cb, parser_free_cb, lexer_cb);

   if (ctx->error)
     {
        fprintf(stderr, "%s\n", ctx->error);
        *err = EINA_TRUE;
        return NULL;
     }

   void *data = ctx->data;
   free(ctx);
   return data;
}

void *
__parse_file(const char  *path,
             Eina_Bool   *err,
             parser       parser_cb,
             parser_alloc parser_alloc_cb,
             parser_free  parser_free_cb,
             lexer        lexer_cb)
{
   Eina_Strbuf *buffer;
   char buf[4096];
   FILE *file;
   void *data;
   size_t x;

   if (!(file = fopen(path, "r")))
     return NULL;

   buffer = eina_strbuf_new();
   while (!feof(file) && !ferror(file))
     {
        x = fread(buf, sizeof(char), sizeof(buf), file);
        eina_strbuf_append_length(buffer, buf, x);
     }

   if (!eina_strbuf_length_get(buffer))
     return NULL;

   data = __parse_string(eina_strbuf_string_get(buffer), err, parser_cb, parser_alloc_cb, parser_free_cb, lexer_cb);
   eina_strbuf_free(buffer);
   return data;
}

