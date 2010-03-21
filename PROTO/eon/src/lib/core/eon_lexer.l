%{
#include <stdio.h>
#include "libeon_core_la-eon_grammar.h"
%}

%%
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

			    return STRING;
                    }}                           
[a-zA-Z0-9]*            yylval = strdup(yytext); return WORD;
\=                      return EQUAL;
\"                      return QUOTE;
\{                      return OBRACE;
\}                      return EBRACE;
;                       return SEMICOLON;
\n                      /* ignore end of line */
[ \t]+                  /* ignore whitespace */;
%%
