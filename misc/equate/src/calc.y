%{
#define YYSTYPE double
#define YYERROR_VERBOSE
#include <math.h>

void
yyerror (const char *s);

YYSTYPE _result;

%}

/* BISON Declarations */
%token NUM
%token OBRAK
%token CBRAK
%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation        */

/* Grammar follows */
%%
input:    /* empty string */
        | exp { printf ("result:\t%.10g\n", $1); _result = $1;}
;

exp:      NUM                { $$ = $1;         }
        | exp '+' exp        { $$ = $1 + $3;    }
        | exp '-' exp        { $$ = $1 - $3;    }
        | exp '*' exp        { $$ = $1 * $3;    }
        | exp '/' exp        { $$ = $1 / $3;    }
        | '-' exp  %prec NEG { $$ = -$2;        }
        | exp '^' exp        { $$ = pow ($1, $3); }
        | OBRAK exp CBRAK    { $$ = $2;         }
;
%%


#include <stdio.h>
#include "lex.yy.c"

void
yyerror (const char *s)  /* Called by yyparse on error */
{
  printf ("%s\n", s);
}

YYSTYPE yyresult (void)
{
  return _result;
}

/*
int
main ( argc, argv )
{
  char *str = "(4*5)";

  yy_scan_string(str);
  yyparse();

  return 0;
}
*/

