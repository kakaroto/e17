#include <stdio.h>

int main(int argc, char ** argv)
{
	extern FILE *yyin;
	extern int yydebug;

	yydebug = 0;
	yyin = fopen(argv[1], "r");
	yyparse();
	fclose(yyin);

	return 0;
}

