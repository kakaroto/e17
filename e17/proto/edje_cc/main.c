#include <stdio.h>
#include <getopt.h>

int main(int argc, char ** argv)
{
	int c;
	char *file = NULL;
	extern FILE *yyin;
	extern int yydebug;

	static struct option long_opts[] = {
		{"yydebug", no_argument, NULL, 'y'},
		{0, 0, 0, 0}
	};

	yydebug = 0;
	while((c = getopt_long(argc, argv, "y", long_opts, NULL)) != -1) {
		switch(c) {
			case 'y':
				yydebug = 1;
				break;

			default:
				printf("Unknown option\n");
				break;
		}
	}

	if (argv[optind] == NULL) {
		printf("need input file\n");
		return 1;
	}
	file = (char *)strdup(argv[optind]);
	yyin = fopen(file, "r");
	free(file);

	yyparse();
	fclose(yyin);

	return 0;
}

