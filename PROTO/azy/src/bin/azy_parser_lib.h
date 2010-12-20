/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PARSER_LIB_H
#define PARSER_LIB_H

#include <Eina.h>
#include "azy.h"

typedef struct
{
   const char *path;
   const char *buffer;
   int         index;
   int         line;
   int         col;
   int         length;
} Azy_Stream;

typedef struct
{
   int         type;
   const char *text; /* Azy_Token text */
   int         length;
   int         sline; /* Azy_Token location (starts from 0) */
   int         scol;
   int         eline;
   int         ecol;
} Azy_Token;

typedef struct
{
   void       *data;
   Azy_Stream *stream;
   const char *error;
} Azy_Parser;

typedef void      *(*Azy_Parser_New_Cb)(void *(*)(size_t));
typedef void       (*Azy_Parser_Free_Cb)(void *, void (*)(void *));
typedef void       (*Azy_Parser_Cb)(void *, int, Azy_Token *, Azy_Parser *parser);
typedef Azy_Token *(*Azy_Token_Cb)(Azy_Stream *);

#define SYNTAX_ERROR(TOKEN)                                                                                 \
  if (parser->error)                                                                                        \
    eina_stringshare_del(parser->error);                                                                    \
  if ((!TOKEN) || (!TOKEN->type))                                                                           \
    parser->error = eina_stringshare_add("Unexpected EOF!\n");                                              \
  else                                                                                                      \
    parser->error = eina_stringshare_printf("Syntax error on line %d column %d: unexpected Azy_Token %s\n", \
      TOKEN->sline + 1, TOKEN->scol + 1, TOKEN->type == AZY_UNKNOWN ? "AZY_UNKNOWN" : yyTokenName[TOKEN->type])

Azy_Stream *azy_parser_stream_new(const char *str);
void    azy_parser_stream_next(Azy_Stream *s,
                       int     length);

Azy_Token *azy_parser_token_new(Azy_Stream *s,
                     int     type,
                     int     length);
void  azy_parser_token_free(Azy_Token *t);

void *azy_parser_string_parse(const char  *str,
                     Eina_Bool   *err,
                     Azy_Parser_Cb       cb,
                     Azy_Parser_New_Cb new_cb,
                     Azy_Parser_Free_Cb  free_cb,
                     Azy_Token_Cb        token_cb);
void *azy_parser_file_parse(const char  *path,
                   Eina_Bool   *err,
                   Azy_Parser_Cb       cb,
                   Azy_Parser_New_Cb new_cb,
                   Azy_Parser_Free_Cb  free_cb,
                   Azy_Token_Cb        token_cb);

#endif
