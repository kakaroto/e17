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
            E(2, "An integer: %s (%g)\n", yytext, atof( yytext ) );
            yylval.val= atof( yytext );
            return NUM;
            }

{DIGIT}+"."{DIGIT}*        {
            E(2, "A float: %s (%g)\n", yytext, atof( yytext ) );
            yylval.val= atof( yytext );
            return NUM;
            }
{DIGIT}*"."{DIGIT}+        {
            E(2, "A float: %s (%g)\n", yytext, atof( yytext ) );
            yylval.val= atof( yytext );
            return NUM;
            }

{ID}        {
            symrec *s;
            E(2, "An identifier: %s\n", yytext);
            s = getsym (yytext);
            if (s == 0)
                    s = putsym (yytext, VAR);
            yylval.tptr = s;
            return s->type;
            }

"+"|"-"|"*"|"/"|"^"|"=" {
            E(2, "An operator: %s\n", yytext );
            return (char) yytext[0];}

"("         {
            E(2, "(\n", NULL);
            return OBRAK;
            }
")"         {
            E(2, ")\n", NULL);
            return CBRAK;
            }

[ \n\t]+          /* eat up whitespace */

.           fprintf(stderr, "Unrecognized character: %s\n", yytext );
%%

