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
   
#define YYSTYPE char *
      
#include "libevolve_la-evolve.h"

int yylex(void);
int evolve_lnum = 1;
int evolve_col = 0;
char *evolve_cur_file = NULL;
extern char *evolve_filename;

static int comment = 0, cpp_comment = 0;
static int edje = 0;

#define EDJE_START() BEGIN(SC_EDJE);
   
#define EDJE_OBRACE() \
   if (edje >= 1) \
     { \
	++edje; \
	EDJE_CODE(); \
     } else { \
	++edje; \
     } \
   
#define EDJE_CBRACE() \
   if (edje == 1) \
     { \
	edje = 0; \
	BEGIN(INITIAL); \
     } \
   else if (edje > 1) \
     { \
	--edje; \
	EDJE_CODE(); \
     } else { \
	--edje; \
     } \
   if (edje < 0) \
     { edje = 0; \
	BEGIN(INITIAL); \
     } \
   
#define EDJE_NEWLINE() evolve_lnum ++; evolve_col = 0;
#define EDJE_CODE() evolve_col += yyleng; return (EDJECODETOK)
   
#define COMMENT_START() if (!edje) { BEGIN(SC_COMMENT); comment++; }
#define COMMENT_END()   if (!edje) { comment--; if (comment <= 0) { comment = 0; BEGIN(INITIAL); } }
#define CPP_COMMENT()   if (!edje ) { comment = 1; cpp_comment = 1; }


#define WHITE_SPACE()   evolve_col += yyleng;

#define NEW_LINE()  evolve_lnum ++; \
		    if (cpp_comment) { \
                        cpp_comment = 0; \
			comment = 0; \
                    } \
                    evolve_col = 0;
		    
#define KEYWORD_RETURN(x)   evolve_col += yyleng; \
			    if (!comment && !edje) return (x);

#define VAL_RETURN(x)   evolve_col += yyleng; \
			if (!comment && !edje) return (x);
			
#define STRING_RETURN() VAL_RETURN(STRING);

#define FLOAT_RETURN()  yylval.val = atof(yytext); \
                        VAL_RETURN(FLOAT);

#define CHAR_RETURN()   VAL_RETURN(yytext[0]);


%}

%option nounput
  
%x SC_COMMENT
%x SC_EDJE  

%%
  
widget                  KEYWORD_RETURN(WIDGETTOK);
type                    KEYWORD_RETURN(TYPETOK);
name			KEYWORD_RETURN(NAMETOK);
parent			KEYWORD_RETURN(PARENTTOK);
packing			KEYWORD_RETURN(PACKINGTOK);
signal                  KEYWORD_RETURN(SIGNALTOK);
data                    KEYWORD_RETURN(DATATOK);
"edje" { EDJE_START(); }

\:                      KEYWORD_RETURN(COLON);
[0-9]+                  yylval=strdup(yytext); KEYWORD_RETURN(NUMBER);
[a-zA-Z][a-zA-Z0-9_-]*	yylval=strdup(yytext); KEYWORD_RETURN(WORD);

(\"[^\"]*\")*       {{
                        char *tmp = NULL;
                        int i = 0;

                        yylval = (char *)calloc(yyleng - 1, sizeof(char));

                        for(i = 0; i < yyleng; i++) {
                            if (yytext[i] == '"') {
                                if (tmp != NULL) {
                                    yytext[i] = '\0';
                                    strcat(yylval, tmp);
                                    tmp = NULL;
                                }
                                continue;
                            }

                            if (tmp == NULL)
                                tmp = yytext + i;
                        }
                        if (tmp != NULL)
                            strcat(yylval, tmp);

			    STRING_RETURN();
                    }}                           
[a-zA-Z0-9\/.-]+        yylval=strdup(yytext); KEYWORD_RETURN(FILENAME);
\"                      KEYWORD_RETURN(QUOTE);
\{                      KEYWORD_RETURN(OBRACE);
\}                      KEYWORD_RETURN(EBRACE);
;                       KEYWORD_RETURN(SEMICOLON);
\n                      NEW_LINE();
[ \t]+                  WHITE_SPACE();
\/\/                            { CPP_COMMENT(); }
#                   { CPP_COMMENT(); }
"/*"                { COMMENT_START(); }
<SC_COMMENT>{
  "/*"              { COMMENT_START(); }
  \n                { NEW_LINE(); }

  "*/"              { COMMENT_END(); }
  . 
}

<SC_EDJE>{
  "edje"            { yylval=strdup(yytext); EDJE_START(); }   
  "{"               { yylval=strdup(yytext); EDJE_OBRACE(); }
  \n                { EDJE_NEWLINE(); }
  ([^\{\}])+        { yylval=strdup(yytext); EDJE_CODE(); }  
  "}"               { yylval=strdup(yytext); EDJE_CBRACE(); }  
}

#[ \t]+[0-9]+[ \t]+\".*\".* {
    /* this handles the #line <num> <name> <something>
	 * so that the lnum and cur_file hold correct 
	 * values for this file
	 */              

	int c;          
	char *txt = yytext;
	char *lpos;     
	int count = 1;
	char *tmp;
	
	c = *txt;
	while(c == '#' || c == ' ') 
		c = *++txt; 

	lpos = txt;
	while(c >= 48 && c <= 57) {
		c = *++txt;
		count ++;
	}       
	
	tmp = (char *)calloc(count + 1, sizeof(char));
	strncpy(tmp, lpos, count);

	/* subtract 1 becase gcc returns the no of the *next* line
	 * and we will add one when we hit the \n anyway */
	evolve_lnum = atoi(tmp) - 1;
	free(tmp);

	while(c == ' ' || c == '\"') 
		c = *++txt;   

	lpos = txt;
	count = 1;
	while(c != '\"') {
		c = *++txt; 
		count ++;
	}
	count --;

	if (evolve_cur_file)
		free(evolve_cur_file);

	evolve_cur_file = (char *)calloc(count + 1, sizeof(char));
	strncpy(evolve_cur_file, lpos, count);

	if (!strcmp(evolve_cur_file, "<stdin>")) {
		free(evolve_cur_file);
		evolve_cur_file = strdup(evolve_filename);
	}
}


%%
