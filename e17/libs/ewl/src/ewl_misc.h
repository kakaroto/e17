
#ifndef __EWL_MISC_H
#define __EWL_MISC_H

struct _ewl_options {
	int debug_level;
};

typedef struct _ewl_options Ewl_Options;

void ewl_init(int argc, char ** argv);
void ewl_main();
void ewl_main_quit();

#endif
