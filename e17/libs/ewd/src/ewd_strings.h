#ifndef _EWD_STRING_H
#define _EWD_STRING_H

typedef struct _ewd_string Ewd_String;
struct _ewd_string {
	char *string;
	int references;
};

char *ewd_string_instance(char *string);
void ewd_string_release(char *string);

#endif
