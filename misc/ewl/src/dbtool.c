#include <string.h>
#include "list.h"
#include "ewldb.h"
#include "util.h"

void dump_db(char *path);
void insert_db(char *path, char *key, char *value);
void insert_db_type(char *path, char *key, char *type, char *value);
void getval_db(char *path, char *key);
void print_usage_and_exit(char *app);

int main(int argc, char *argv[])
{
	if (argc==1)	{
		print_usage_and_exit(argv[0]);
	} else if (!strncmp("-?",argv[1],2)||
	           !strncmp("-h",argv[1],2)||
	           !strncmp("--help",argv[1],4)) {
		print_usage_and_exit(argv[0]);
	} else if (argc==2)	{
		dump_db(argv[1]);
	} else if (argc==3) {
		getval_db(argv[1], argv[2]);
	} else if (argc==4) {
		insert_db(argv[1], argv[2], argv[3]);
	} else if (argc==5) {
		insert_db_type(argv[1], argv[2], argv[3], argv[4]);
	}

	return 0;
}

void cb_dump(EwlListNode *node, void *data)
{
	UNUSED(data);
	printf(" %s\n", (char*)node->data );
	return;
}

void dump_db(char *path)
{
	EwlDB *db = NULL;
	EwlList *list = NULL;
	
	db = ewl_db_open(path);
	if (!db)	{
		fprintf(stderr,"ERROR: Couldn't open db \"%s\".\n", path?path:"");
		return;
	}
	list = ewl_db_dump(db);
	printf("%s keys:\n", path);
	ewl_list_foreach(list,cb_dump,NULL);
	fflush(stdout);
	ewl_db_close(db);
	return;
}

void insert_db(char *path, char *key, char *value)
{
	EwlDB *db = NULL;
	db = ewl_db_open_writeable(path);
	if (!db)	{
		fprintf(stderr,"ERROR: Couldn't open db \"%s\".\n", path?path:"");
		return;
	}
	ewl_db_string_set(db,key,value);
	ewl_db_close(db);
	return;
}

void insert_db_type(char *path, char *key, char *type, char *value)
{
	EwlDB *db = NULL;
	int    i  = 0; 
	float  f  = 0;
	db = ewl_db_open_writeable(path);
	if (!db)	{
		fprintf(stderr,"ERROR: Couldn't open db \"%s\".\n", path?path:"");
		return;
	}
	if (!strncasecmp(type,"string",5)||!strncasecmp(type,"str",3)) {
		ewl_db_string_set(db,key,value);
	} else if (!strncasecmp(type,"integer",7)||!strncasecmp(type,"int",3)) {
		i = atoi(value);
		ewl_db_int_set(db,key,i);
	} else if (!strncasecmp(type,"float",5)||!strncasecmp(type,"flt",3)) {
		f = atof(value);
		ewl_db_float_set(db,key,i);
	} else {
		fprintf(stderr,"ERROR: Unknown type \"%s\".\n", type);
	}
	ewl_db_close(db);
	return;
}

void getval_db(char *path, char *key)
{
	char  *val = NULL;
	EwlDB *db = NULL;
	db = ewl_db_open(path);
	if (!db)	{
		fprintf(stderr,"ERROR: Couldn't open db \"%s\".\n", path?path:"");
		return;
	}
	val = ewl_db_string_get(db,key);
	if (!val)	{
		fprintf(stderr,"ERROR: Unknown key \"%s\".\n", key);
	} else {
		printf("%s\n", val);
	}
	ewl_db_close(db);
	return;
}

void print_usage_and_exit(char *app)
{
	fprintf(stderr,
	    "ewldbtool 0.3 by Paul Duncan <pabs@pablotron.org>\n"
	    "Usage:\n"
	    "  %s [-?|-h|--help]         - display usage information.\n"
	    "  %s <db> <key> <value>        - insert key into <db>.\n"
	    "  %s <db> <key> <type> <value> - insert key into <db> as <type> .\n"
	    "  %s <db>                      - print a list of keys from <db>.\n"
	    "Valid Types: \"string\" or \"str\", \"integer\" or \"int\", \"float\" or \"flt\".\n\n"
	    "Distributed with EWL.\n"
	    "Please see the file COPYING for licensing information.\n",
	        app, app, app, app);
	exit(-1);
}
