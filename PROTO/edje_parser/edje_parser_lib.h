/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
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

#ifndef EDJE_PARSER_LIB_H
#define EDJE_PARSER_LIB_H

#include <Eina.h>
#include "edje.h"

#define EDJE_DOUBLEQUOTES (-2)
typedef struct
{
   char *buffer;
   int         index;
   int         line;
   int         col;
   int         length;
} Edje_Stream;

typedef struct
{
   int         type;
   char *text; /* text */
   int         length;
   int         sline; /* token location (starts from 0) */
   int         scol;
   int         eline;
   int         ecol;
} Edje_Parser_Token;

typedef struct
{
   Edje                           *edje;
   Edje_Stream                    *stream;
   const char                     *error;

   Edje_Collection                *collection; /* current collection */
   Edje_Color_Class               *color_class;
   Edje_Data                      *data;
   Edje_External                  *external;
   Edje_Font                      *font;
   Edje_Fonts                     *fonts;
   Edje_Group                     *group;
   Edje_Images                    *images;
   Edje_Part                      *part;
   Edje_Parts                     *parts;
   Edje_Part_Description_Relative *rel;
   Edje_Part_Description          *description;
   Edje_Program                   *program;
   Edje_Programs                  *programs;
   Edje_Set                       *set;
   Edje_Set_Image                 *set_image;
   Edje_Style                     *style;
} Edje_Parser;

typedef void *             (*Edje_Parser_New_Cb)(void *(*)(size_t));
typedef void               (*Edje_Parser_Free_Cb)(void *, void (*)(void *));
typedef void               (*Edje_Parser_Cb)(void *, int, Edje_Parser_Token *, Edje_Parser *);
typedef Edje_Parser_Token *(*Edje_Token_Cb)(Edje_Stream *);
Eina_Bool edje_parser_parse_color(Edje_Parser * ep,
                                  Edje_Parser_Token * P,
                                  Edje_Parser_Token * I[],
                                  int *color[]);
Eina_Bool
edje_parser_property_check(const char *in,
                           const char *cmp);

Eina_Bool edje_parser_parse_min(Edje_Parser       *ep,
                                Edje_Parser_Token *I,
                                Edje_Parser_Token *J,
                                int               *min,
                                int               *max);
Eina_Bool edje_parser_parse_max(Edje_Parser       *ep,
                                Edje_Parser_Token *I,
                                Edje_Parser_Token *J,
                                int               *min,
                                int               *max);

Edje_Stream *edje_parser_stream_new(const char *str);
void         edje_parser_stream_next(Edje_Stream *s,
                                     int          length);

Edje_Parser_Token *edje_parser_token_new(Edje_Stream *s,
                                         int          type,
                                         int          length);
void  edje_parser_token_free(Edje_Parser_Token *t);

Edje *edje_parser_string_parse(const char         *str,
                               Eina_Bool          *err,
                               Edje_Parser_Cb      cb,
                               Edje_Parser_New_Cb  new_cb,
                               Edje_Parser_Free_Cb free_cb,
                               Edje_Token_Cb       token_cb);
Edje *edje_parser_file_parse(const char         *path,
                             Eina_Bool          *err,
                             Edje_Parser_Cb      cb,
                             Edje_Parser_New_Cb  new_cb,
                             Edje_Parser_Free_Cb free_cb,
                             Edje_Token_Cb       token_cb);
Edje *edje_parse_file(const char *path,
                      Eina_Bool  *err);
Edje *edje_parse_string(const char *str,
                        Eina_Bool  *err);

#endif
