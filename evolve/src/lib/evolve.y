/* Evolve
 * Copyright (C) 2007-2008 Hisham Mardam-Bey 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

%{
#include <stdio.h>
#include <string.h>
   
#include "Evolve.h"
#include "evolve_private.h"
   
#define YYSTYPE char *
#define YYERROR_VERBOSE 1

int yylex(void);   
void yyerror(char *format, ...);
void parse_error(void);
extern char *evolve_cur_file;                                          
extern int evolve_lnum;
extern int evolve_col;
static int section;
   
void yyerror(char *format, ...)
{
   va_list ap;
   
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
   parse_error();
}
 
void parse_error()
{
   fprintf(stderr, "file: %s, line: %d, column: %d\n\n",
	   evolve_cur_file, evolve_lnum, evolve_col);
   exit(-1);                                                               
} 
   
int yywrap()
{
   return 1;
} 
        
%}

%token WIDGETTOK TYPETOK NAMETOK PARENTTOK COLON NUMBER WORD STRING EDJECODETOK
%token FILENAME PACKINGTOK SIGNALTOK DATATOK QUOTE OBRACE EBRACE SEMICOLON

%%  
  
widgets: /* empty */
       | widgets widget edje
       ;

widget: WIDGETTOK OBRACE { section = WIDGETTOK; } type name parent properties EBRACE
      ;

type: TYPETOK COLON type_type SEMICOLON
    ;

type_type: STRING 
         {
	    switch (section)
	      {
	         case WIDGETTOK:
		   evolve_parse_widget($1);
		 break;
	         case DATATOK:
		   evolve_parse_signal_data_property("type", $1);
		 break;
	      }
	 }
         ;

name: NAMETOK COLON STRING SEMICOLON
    {
       switch (section)
       {
	  case WIDGETTOK:
	    evolve_parse_name($3);
	  break;
	  case SIGNALTOK:
	    evolve_parse_signal_property("name", $3);
	  break;
	  case DATATOK:
	    evolve_parse_signal_data_property("name", $3);
	  break;
	  default:
	  break;
       }
    }
    ;

parent: /* empty */
      | PARENTTOK COLON STRING SEMICOLON
      {
	 evolve_parse_parent($3);
      }
      ;

properties: /* empty */
          | properties property
          ;

property:        
        WORD COLON NUMBER SEMICOLON
        {
	   if (!evolve_parse_property_number($1, $3))
	     yyerror("Property %s not found for this widget type.\n", $1);
	}
        |
        WORD COLON STRING SEMICOLON
        {
	   if (!evolve_parse_property_string($1, $3))
	     yyerror("Property %s not found for this widget type.\n", $1);
	}
        | WORD COLON WORD SEMICOLON
        {
	   if (!evolve_parse_property_string($1, $3))
	     yyerror("Property %s not found for this widget type.\n", $1);
	}
        | WORD COLON FILENAME SEMICOLON
        {
	   if (!evolve_parse_property_string($1, $3))
	     yyerror("Property %s not found for this widget type.\n", $1);
	}
        | packing
        | signal
        ;

packing: PACKINGTOK OBRACE packing_properties EBRACE
       ;

packing_properties: /* empty */
                  | packing_properties packing_property
                  ;

packing_property: WORD COLON NUMBER SEMICOLON
                {
		   evolve_parse_packing_property_number($1, $3);
		}
                |
                WORD COLON STRING SEMICOLON
                {
		   evolve_parse_packing_property_string($1, $3);
		}
                ;

signal: SIGNALTOK OBRACE { section = SIGNALTOK; } signal_create signal_properties EBRACE
      ;

signal_create:
             {
		evolve_parse_signal();
             }
             ;

signal_properties: /* empty */
                 | signal_properties signal_property
                 ;

signal_property: WORD COLON NUMBER SEMICOLON
                {
		   evolve_parse_signal_property($1, $3);
		}
                |
                WORD COLON STRING SEMICOLON
                {
		   evolve_parse_signal_property($1, $3);
		}
                | name
                | DATATOK OBRACE { section = DATATOK; } signal_data EBRACE
                ;

signal_data: /* empty */
           | signal_data signal_datum
           ;

signal_datum: WORD COLON NUMBER SEMICOLON
             {
		evolve_parse_signal_data_property($1, $3);
	     }
             |
             WORD COLON STRING SEMICOLON
             {
		evolve_parse_signal_data_property($1, $3);
	     }
             | type
             | name
             ;
            
edje: /* empty */ 
    |   edjecode
    ;

edjecode: /* empty */
        | edjecode EDJECODETOK  
        {
	   evolve_parse_edje($2);
        }
        ;

%%
  
