#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "Etcher.h"

#define MAIN_EDC_NAME "main_edje_source.edc"

char *filename = NULL;
static char *work_file = NULL;
static char *work_dir = NULL;
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
	char *cpp_extra = NULL;

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

	/* do we have a .eet instead of .edc file? */
	if (strstr(filename, ".eet")) {
		char *cmd = NULL;
		char *old_fname = (char *)strdup(filename);
		int len = 0;
		int ret = 0;

		free(filename);
		memset(tmpn, '\0', sizeof(tmpn));
		strcpy(tmpn, "/tmp/etcher_cc.edc-tmp-XXXXXX");
		if (mkdtemp(tmpn) == NULL) {
			fprintf(stderr, "Can't create working dir: %s",
							strerror(errno));
			return 0;
		}
		work_dir = (char *)strdup(tmpn);

		len = strlen(work_dir) + strlen(old_fname) + 5;
		cmd = (char *)calloc(len,sizeof(char));
		snprintf(cmd, len, "cp %s %s", old_fname, work_dir);
		ret = system(cmd);
		free(cmd);

		if (ret < 0) {
			fprintf(stderr, "Unable to copy %s to tmp dir %s: %s\n",
					old_fname, work_dir, strerror(errno));
			return 0;
		}

		/* we change to the work dir because edje_cc will extract into the
		 * current directory. 
		 */
		getcwd(tmpn, sizeof(tmpn));
		if (chdir(work_dir) == -1) {
			fprintf(stderr, "Can't change to work dir %s: %s\n", work_dir,
							strerror(errno));
			return 0;
		}

		len = strlen(work_dir) + strlen(old_fname) + 12;
		cmd = (char *)calloc(len, sizeof(char));
		snprintf(cmd, len, "edje_decc %s/%s", work_dir, old_fname);
		ret = system(cmd);
		free(cmd);

		if (ret < 0) {
			fprintf(stderr, "Unable to de-compile %s\n", old_fname);
			return 0;
		}

		/* change back to the original dir because edje_cc will write into
		 * that dir currently
		 */
		if (chdir(tmpn) == -1) {
			fprintf(stderr, "Can't change back to current dir: %s\n", 
							strerror(errno));
			return 0;
		}

		cmd = strstr(old_fname, ".eet");
		*cmd = '\0';

		/* we need the info on the work dir to pass the cpp so it can
		 * include files correctly 
		 */
		len = strlen(old_fname) + strlen(work_dir) + 4;
		cpp_extra = (char *)calloc(len, sizeof(char));
		snprintf(cpp_extra, len, "-I%s/%s", work_dir, old_fname);

		len = strlen(work_dir) + strlen(old_fname) +
						strlen(MAIN_EDC_NAME) + 3;
		filename = (char *)calloc(len, sizeof(char));
		snprintf(filename, len, "%s/%s/%s", work_dir, old_fname, 
							MAIN_EDC_NAME);
		free(old_fname);
	}

	/* run it through cpp */
	strcpy(tmpn, "/tmp/etcher_cc.edc-tmp-XXXXXX");
	fd = mkstemp(tmpn);
	if (fd >= 0) {
		int ret = 0;

		work_file = (char *)strdup(tmpn);
		close(fd);

		atexit(clean_tmp_file);
		snprintf(buf, sizeof(buf), "cat %s | cpp -E %s -o %s", filename, 
						(cpp_extra ? cpp_extra : ""), tmpn);
		ret = system(buf);

		if (ret < 0) {
			snprintf(buf, sizeof(buf), "gcc -E %s -o %s %s", 
						(cpp_extra ? cpp_extra : ""), 
						tmpn, filename);
			ret = system(buf);
		}
		if (cpp_extra)
			free(cpp_extra);

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
	
	if (work_dir) {
		char *cmd = (char *)calloc(strlen(work_dir) + 7, sizeof(char));
		sprintf(cmd, "rm -r %s", work_dir);
		system(cmd);
	}
}


