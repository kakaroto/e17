#include <stdio.h>
#include <getopt.h>
#include "Etcher.h"

int main(int argc, char ** argv)
{
	int c;
	char *file = NULL;
	extern FILE *yyin;
	extern int yydebug;
        extern Etcher_File *etcher_file;

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

        etcher_parse_init();

	yyparse();
	fclose(yyin);

        /* FIXME: make this a complete test suite */
        {
          Evas_List *l;
          l = etcher_file->data;
          while (l)
          {
            Etcher_Data *d = l->data;
            printf("-------------------------------------\n");
            printf("Done parsing, print out data keys:\n");
            printf("key: %s, value: %s\n", d->key, d->value);
            l = l->next;
          }
        }
	return 0;
}

