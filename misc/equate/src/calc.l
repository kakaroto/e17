/* scanner for a toy Pascal-like language */

%{
/* need this for the call to atof() below */
#include <math.h>
%}

DIGIT    [0-9]
ID       [a-z][a-z0-9]*

%option noyywrap

%%

{DIGIT}+    {
            printf( "An integer: %s (%d)\n", yytext,
                    atoi( yytext ) );
            yylval.val= atoi( yytext );
            return NUM;
            }

{DIGIT}+"."{DIGIT}*        {
            printf( "A float: %s (%g)\n", yytext,
                    atof( yytext ) );
            yylval.val= atof( yytext );
            return NUM;
            }

{ID}        {
            symrec *s;
            printf( "An identifier: %s\n", yytext );
            s = getsym (yytext);
            if (s == 0)
                    s = putsym (yytext, VAR);
            yylval.tptr = s;
            return s->type;
            }

"+"|"-"|"*"|"/"|"^"|"=" {printf( "An operator: %s\n", yytext );
            return (char) yytext[0];}

"("         return OBRAK;
")"         return CBRAK;

[ \n\t]+          /* eat up whitespace */

.           printf( "Unrecognized character: %s\n", yytext );
%%

