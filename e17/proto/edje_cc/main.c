#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "Etcher.h"

char *filename = NULL;
static char *work_file = NULL;
static void clean_tmp_file(void);

int main(int argc, char ** argv)
{
	int c = 0, fd = 0;
	char *file = NULL;
	extern FILE *yyin;
	extern int yydebug;
	extern Etcher_File *etcher_file;
	extern char *cur_file;
	static char tmpn[4096];
	char buf[4096];

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
	filename = (char *)strdup(argv[optind]);

	strcpy(tmpn, "/tmp/etcher_cc.edc-tmp-XXXXXX");
	fd = mkstemp(tmpn);
	if (fd >= 0) {
		int ret = 0;

		work_file = tmpn;
		close(fd);

		atexit(clean_tmp_file);
		snprintf(buf, sizeof(buf), "cat %s | cpp -E -o %s", filename, tmpn);
		ret = system(buf);

		if (ret < 0) {
			snprintf(buf, sizeof(buf), "gcc -E -o %s %s", tmpn, filename);
			ret = system(buf);
		}

		if (ret >= 0) 
			file = tmpn;
		else {
			fprintf(stderr, "Unable to create temp file (%s)\n", tmpn);
			return 0;
		}

	} else {
		fprintf(stderr, "Unable to create temp file (%s): %s\n", 
				tmpn, strerror(errno));
		return 1;
	}

	yyin = fopen(file, "r");
	if (!yyin) {
		fprintf(stderr, "Unable to open file (%s): %s\n", 
				file, strerror(errno));
		free(file);
		return 0;
	}

	free(file);

	etcher_parse_init();

	yyparse();
	fclose(yyin);

	etcher_file_output(etcher_file, "test.out");

        /* FIXME: make this a complete test suite */
#if 0        
        {
          Evas_List *l;

          printf("-------------------------------------\n");

          printf("Done parsing, print out data keys:\n");
          l = etcher_file->data;
          while (l)
          {
            Etcher_Data *d = l->data;
            printf("key: %s, value: %s\n", d->key, d->value);
            l = l->next;
          }
          
          printf("Groups:\n");

          l = etcher_file->groups;
          while (l)
          {
            Etcher_Group *g = l->data;
            Evas_List *ll;

            printf("  name: %s\n", g->name);

            printf("  group data:\n");
            ll = g->data;
            while(ll)
            {
              Etcher_Data *d = ll->data;
              printf("    key: %s, value: %s\n", d->key, d->value);
              ll = ll->next;
            }
            if (g->script)
              printf("  script:\n-----\n%s\n-----\n", g->script);
            l = l->next;
          }
        }
#endif
	return 0;
}

static void
clean_tmp_file(void)
{
	if (work_file)
		unlink(work_file);
}

